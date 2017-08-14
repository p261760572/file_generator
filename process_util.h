
#ifndef __process_util_h
#define __process_util_h

#include "base.h"
#include "ocilib.h"
#include "json.h"
#include "json_ext.h"



#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */
#define MAX_JSON_DEEP 3
#define DEFAULT_SQL_SIZE 1024
#define MAX_SQL_SIZE 8192
#define MAX_OUT_VARCHAR_SIZE 512
#define MAX_OUT_SIZE 4
#define MAX_ERR_MSG_SIZE 512
#define MAX_BUFFER_SIZE 4096
#define MAX_FUN_CONFIG_SIZE 4096
#define MAX_FUN_INFO_SIZE 1024
#define DEFAULT_BUFFER_SIZE 1024

struct fun_config_s {
    char url[256+1];
    char module_name[128+1];
    char param_list[512+1];
    char exec_type[1+1];
    char input[32+1];
    char test[128+1];
    char test_input[128+1];
};

typedef struct fun_config_s fun_config_t;

struct fun_info_s {
    char url[256+1];
    char op[1+1];
    char log_flag[1+1];
};

typedef struct fun_info_s fun_info_t;

struct session_attr_s
{
	char session_flag;
	char captcha[6+1];
	char img_captcha[6+1];
	char userid[38+1];
	char login_name[32+1];
	char inst_id[8+1];
	char user_level[1+1];
	char login_type[1+1];
	char province[2+1];
	char city[2+1];
	char district[2+1];
	char attr1[11+1];
	char attr2[11+1];
	char attr3[11+1];
	char attr4[11+1];
	char attr5[11+1];
};

typedef struct session_attr_s session_attr_t;


struct process_ctx_s
{
    shm_data *shm;
    session *session;
	char action[128+1];
    char ip[15+1];
	//int code;
	const char *sign;
	const char *body;
	int body_len;
	void *user_data1;
    //session_attr_t attr;
    char *headers;
	int headers_size;
	oci_connection_t *con;
	char op[1+1];
	char log_flag[1+1];
	char log_id[28+1];
};

typedef struct process_ctx_s process_ctx_t;

typedef int (*fetch_caller)(void *ctx, oci_resultset_t *rs, int rownum);

typedef int (*module_fn)(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size);


struct execute_module {
    char *module_name;
    module_fn fn;
};

int execute_config(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size);

int load_fun_config(oci_connection_t *con);
fun_config_t *search_fun_config_min(char *url);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __db_hander_h */
