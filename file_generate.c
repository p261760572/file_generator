#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <uuid/uuid.h>
#include <curl/curl.h>
#include "db_hander.h"
#include "process_util.h"
#include "cstr.h"
#include "ibdcs.h"
#include "file_generate.h"

#define FILE_SQL "select settle_date, id, url from generate_list where status = '0' order by start_time"

#define STR(a)	#a
#define STR2(a)	STR(a)
extern int g_fun_config_len;
extern fun_config_t g_fun_config[MAX_FUN_CONFIG_SIZE];
Generate_list generate_list;

void get_current_date(char *str, size_t str_size)
{
    time_t tt;
    struct tm local_time;

    time(&tt);
    localtime_r(&tt, &local_time);
    strftime(str, str_size, "%Y%m%d", &local_time);
}


/*
@return 成功返回0,出错返回-1
*/
//
#define DEFAULT_DIR_MODE (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
static int mkdirs(const char *dir, mode_t mode)
{
    int ret = 0;
    char copy_dir[1024];
    char *p = copy_dir;

		if(dir[0] == '\0')
			return -1;

    cstr_copy(copy_dir, dir, sizeof(copy_dir));

    while((p = strchr(p+1, '/')) != NULL)
    {
        *p = '\0';
        ret = mkdir(copy_dir, mode);
        *p = '/';
        if(ret != 0 && errno != EEXIST)
        {
            dcs_debug(0,0,"at %s(%s:%d) %s",__FUNCTION__,__FILE__,__LINE__,strerror(errno));
            return -1;
        }
    }

    ret = mkdir(copy_dir, mode);

    if(ret != 0 && errno != EEXIST)
    {
				dcs_debug(0,0,"at %s(%s:%d) %s",__FUNCTION__,__FILE__,__LINE__,strerror(errno));
        return -1;
    }

    return 0;
}


int select_generate_file_handler(void *ctx, oci_resultset_t *rs, int rownum) {
		int i, num;
    const char *value;
    if(generate_list.list >= MAX_DOWN_FILE) return 0;
    for(i = 1; i <= oci_get_column_count(rs); i ++) {
        value = oci_get_string(rs, i);
        value = (value == NULL ? "" : value);
        if(i == 1) {
        		strcpy(generate_list.download_list[generate_list.list].settle_date, value);
        }
        else if(i == 2) {
		        num = atoi(value);
        		generate_list.download_list[generate_list.list].id = num;		
        }
        else if(i == 3) {
        		strcpy(generate_list.download_list[generate_list.list].url, value);
        }
    }
    generate_list.list++;
    return 1;
}

int load_generate_file() {
		char err_msg[1024];
		size_t err_size = sizeof(err_msg);
		bzero(&generate_list, sizeof(generate_list));
		if(sql_execute(g_db_con, FILE_SQL, NULL, select_generate_file_handler, NULL, err_msg, err_size) < 0) {
        dcs_log(0, 0, "at %s(%s:%d) db_execute_select fail",__FUNCTION__,__FILE__,__LINE__);
        return -1;
    }
    return 1;
}

    
int file_prepare_foreach(int *id, char *settle_date, char *url) {
		if(generate_list.fetch >= generate_list.list) return 0;
		memset(settle_date, 0, sizeof(settle_date));
		memset(url, 0, sizeof(url));
		strcpy(settle_date, generate_list.download_list[generate_list.fetch].settle_date);
		*id = generate_list.download_list[generate_list.fetch].id;		
		strcpy(url, generate_list.download_list[generate_list.fetch].url);
		generate_list.fetch ++;
    return 1;
}

int process() {
		int i, id, config_len;
		char err_msg[1024] = {0}, *success, v_id[6];
		char settle_date[9], url[128];
    size_t err_size = sizeof(err_msg);
    fun_config_t *config;
    process_ctx_t ctx;
    bzero(&ctx, sizeof(ctx));
    ctx.con = g_db_con;
    
    if(test_databse_connection(g_db_con) < 0) return 0;
    json_object *request = json_object_new_object();
    json_object *response = json_object_new_object();
    if(load_generate_file() < 0) return -1;
    generate_list.fetch = 0;
    dcs_log(0, 0, "at (%s:%d)待处理记录[%d]", __FILE__, __LINE__, generate_list.list);
    while(file_prepare_foreach(&id, settle_date, url) > 0) {
    		success = "1";
    		sprintf(v_id, "%d", id);
    		json_object_object_add(request, "settle_date", json_object_new_string(settle_date));
    		json_object_object_add(request, "id", json_object_new_string(v_id));
    		
    		config = search_fun_config_min(url);
    		if(config == NULL) {
    				success = "0";
    		}
        config_len = 0;
        while(config && config+config_len < g_fun_config+g_fun_config_len &&
              strcmp(config[config_len].url, config[0].url) == 0)
            config_len ++;
        //事务1
        for(i = 0;  i < config_len ; i ++) {
            if(config[i].exec_type[0] != '0') {			
                break;
            } else {
                if(execute_config(config+i, &ctx, request, response, err_msg, err_size) != 0) {
                    db_errmsg_trans(err_msg, err_size);
                    success = "0";
                    oci_rollback(g_db_con);
                    break;
                }
            }
        }
        json_object_object_add(request, "success", json_object_new_string(success));
        // 事务2 成功处理/失败处理
        for(;  i < config_len ; i ++) {
            if(config[i].exec_type[0] != '1') {
                continue;
            } else {
                if(execute_config(config+i, &ctx, request, response, err_msg, err_size) != 0) {
                    db_errmsg_trans(err_msg, err_size);
                    oci_rollback(g_db_con);
                    break;
                }
            }
        }
        oci_commit(g_db_con);
        dcs_log(0, 0, "at (%s:%d)%s [%d] %s处理完成，结果%s，记录[%d]\n%s", __FILE__, __LINE__, settle_date, 
        				id, url, (success[0] == '1'? "成功": "失败"), generate_list.fetch, err_msg);
    }
    json_object_put(request);
    json_object_put(response);
    return 1;
}
