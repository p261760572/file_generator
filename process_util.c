#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <uuid/uuid.h>

#include "ibdcs.h"
#include "process_util.h"
#include "gen_sql.h"
#include "data_acl.h"
#include "cdefs.h"
#include "cstr.h"
#include "cbuf.h"
#include "carray.h"
#include "cfile.h"
#include "cdate.h"
#include "cbin.h"
#include "cdes.h"
#include "ciconv.h"
#include "exp.h"
#include "sql.h"
#include "libxl.h"

#define SELECT_TOTAL "select count(1) total"
//#define SELECT_PAGE_START "select * from (select rownum rn, a.* from ("
//#define SELECT_PAGE_END ") a where rownum <= :b%d*:b%d) where rn > (:b%d-1)*:b%d"

#define SELECT_PAGE_START "select * from (select rownum rn, a.* from ("
#define SELECT_PAGE_END ") a) where rn <= :b%d*:b%d and rn > (:b%d-1)*:b%d"


//#define SELECT_PAGE_START "select a.* from("
//#define SELECT_PAGE_END ") a where rn_ > (:b%d-1)*:b%d and rn_ <= :b%d*:b%d"

extern char *document_root;

fun_info_t g_fun_info[MAX_FUN_INFO_SIZE];
int g_fun_info_len = 0;


fun_config_t g_fun_config[MAX_FUN_CONFIG_SIZE];
int g_fun_config_len = 0;

char *get_current_value(void *data, const char *name) {
    process_ctx_t *ctx = (process_ctx_t *)data;


    if(strcmp(name, "CurrentAction") == 0) {
        return ctx->action;
    } else if(strcmp(name, "CurrentLogId") == 0) {
        return ctx->log_id;
    } else if(ctx->session == NULL) {
        return NULL;
    } else {
        session_attr_t *attr = (session_attr_t *)ctx->session->remark;
        if(strcmp(name, "CurrentAttr1") == 0) {
            return attr->attr1;
        } else if(strcmp(name, "CurrentAttr2") == 0) {
            return attr->attr2;
        } else if(strcmp(name, "CurrentAttr3") == 0) {
            return attr->attr3;
        }
    }

    return NULL;
}


char *get_value(void *data, const char *name) {
    void **param = (void **)data;
    process_ctx_t *ctx = param[0];
    json_object *obj = param[1];


    //dcs_debug(0, 0, "at %s(%s:%d) [%s]",__FUNCTION__,__FILE__,__LINE__, name);

    if(strncmp(name, "Current", 7) == 0) {
        return get_current_value(ctx, name);
    }
    //dcs_debug(0, 0, "at %s(%s:%d) [%s]",__FUNCTION__,__FILE__,__LINE__, name);
    return (char *)json_util_object_get_string(obj, name);
}

oci_connection_t *get_connection(void *data) {
    process_ctx_t *ctx = (process_ctx_t *)data;
    return ctx->con;
}

static void print_bind(carray_t *bind) {
    int i, len;
    char *p;
    if(bind != NULL) {
        len = carray_size(bind);
        for(i = 0; i < len; i ++) {
            p = (char *)carray_get(bind, i);
            if(p) {
                dcs_debug(0, 0, "[%s]", p);
            } else {
                dcs_debug(0, 0, "[NULL]");
            }
        }
    }
}


int select_fetch_rows_handler(void *ctx, oci_resultset_t *rs, int rownum) {

    void **params = (void **)ctx;
    int rowcount = *((int *)params[0]), i;
    json_object *rows = (json_object *)params[1], *row;
    const char *value;

    if(rownum > rowcount) {
        return 0; //限制
    }

    row = json_object_new_object();
    for(i = 1; i <= oci_get_column_count(rs); i ++) {
        value = oci_get_string(rs, i);
        value = (value == NULL ? "" : value);
        json_object_object_add(row, oci_get_column_name(rs, i), json_object_new_string(value));
    }
    json_object_array_add(rows, row);
    return 1;
}


int select_fetch_row_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    int i;
    const char *value;
    json_object *row = (json_object *)ctx;
    for(i = 1; i <= oci_get_column_count(rs); i ++) {
        value = oci_get_string(rs, i);
        value = (value == NULL ? "" : value);
        json_object_object_add(row, oci_get_column_name(rs, i), json_object_new_string(value));
    }
    return 0;
}

int select_fetch_total_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    json_object *row = (json_object *)ctx;
    json_object_object_add(row, oci_get_column_name(rs, 1), json_object_new_int(oci_get_int(rs, 1)));
    return 0; //终止fetch
}

int select_fetch_count_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    int *count = (int *)ctx;

    *count = oci_get_int(rs, 1);

    return 0;
}


int select_fetch_column_handler(void *ctx, oci_resultset_t *rs, int rownum) {

    void **params = (void **)ctx;
    int rowcount = *((int *)params[0]);
    json_object *rows = (json_object *)params[1];
    const char *value;

    if(rownum > rowcount) {
        return 0; //限制
    }

    value = oci_get_string(rs, 1);
    value = (value == NULL ? "" : value);

    json_object_array_add(rows, json_object_new_string(value));
    return 1;
}


static int select_fetch_export_txt_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    int i;
    const char *value;
    void **arr = (void **)ctx;
    int fd= *((int *)arr[0]);
    char *delim = (char *)arr[1];

    char line[MAX_BUFFER_SIZE];
    size_t line_size = sizeof(line);
    int len = 0;

    bzero(line, sizeof(line));
    for(i = 1; i <= oci_get_column_count(rs); i ++) {
        value = oci_get_string(rs, i);
        value = (value == NULL ? "" : value);
        if(i == 1) {
            len += snprintf(line + len, line_size-len, "%s", value);
        } else {
            len += snprintf(line + len, line_size-len, "%s%s", delim, value);
        }
    }

    len += snprintf(line + len, line_size-len, "\r\n");

    if(write(fd, line, len) < 0) {
        dcs_log(0, 0, "at %s(%s:%d) error write[%d] %s",__FUNCTION__,__FILE__,__LINE__,fd,strerror(errno));
        return -1;
    }

    return 1;
}

static int select_fetch_export_xls_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    int i;
    void **arr = (void **)ctx;
    SheetHandle sheet= (SheetHandle)arr[0];
    int row= *((int *)arr[1]);
    int col= *((int *)arr[2]);
    const char *value;

    for(i = 1; i <= oci_get_column_count(rs); i ++) {
        if(oci_get_column_type(rs, i) == OCI_CDT_NUMERIC) {
            xlSheetWriteNumA(sheet, row + rownum-1, col + i - 1, oci_get_double(rs, i), 0);
        } else {
            value = oci_get_string(rs, i);
            xlSheetWriteStrA(sheet, row + rownum - 1, col + i - 1, (value == NULL ? "" : value), 0);
        }
    }

    return 1;
}


int gen_uuid(unsigned char buf[33]) {
    static unsigned char hex[] = "0123456789abcdef";

    unsigned char *pbuf = buf;

    uuid_t uuid;
    uuid_generate(uuid);

    unsigned char *p = uuid;
    int i;
    for(i = 0; i < sizeof(uuid_t); i++, p++) {
        *(pbuf)++ = hex[(p[0] >> 4) & 0xf];
        *(pbuf)++ = hex[p[0] & 0xf];
    }
    *pbuf = '\0';

    return 0;
}

/**
*测试功能表达式
*@param test 表达式
*@return 结果真返回1,结果假返回0
*/
int fun_config_test_value(char *test, process_ctx_t *ctx, json_object *request) {
    int test_value = 1;
    if(!cstr_empty(test)) {
        exp_context_t exp_ctx;
        void *data[] = {ctx, request};
        exp_ctx.data = data;
        exp_ctx.get = get_value;
        test_value = test_exp(&exp_ctx, test);
    }

    //dcs_log(0, 0, "at %s(%s:%d) %d %s",__FUNCTION__,__FILE__,__LINE__,test_value, json_object_to_json_string(request));

    return test_value;
}

int check_data_acl(process_ctx_t *ctx, sql_ctx_t *sql_ctx, char *stmt_id, json_object *request, int rowcount) {
    int ret = 0, count = -1;
    cbuf_t sqlbuf, acl_sqlbuf;;
    carray_t bind;
    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    cbuf_init(&acl_sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);
    if(generate_sql(sql_ctx, stmt_id, &sqlbuf, &bind) != 0) {
        ret = -1;
        //snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        char *start;
        start = strstr(cbuf_str(&sqlbuf), " from ");
        if(start == NULL) {
            //配置错误
            ret = -1;
            dcs_log(0, 0, "at %s(%s:%d) 配置错误\n%s",__FUNCTION__,__FILE__,__LINE__,cbuf_str(&sqlbuf));
        } else {
            cbuf_append(&acl_sqlbuf, SELECT_TOTAL, sizeof(SELECT_TOTAL)-1);
            cbuf_append(&acl_sqlbuf, start, strlen(start));

            if(sql_execute(ctx->con, cbuf_str(&acl_sqlbuf), &bind, select_fetch_count_handler, &count, NULL, 0) <= 0) {
                ret = -1;
            } else if(rowcount != count) {
                //数据权限检查失败
                ret = -1;
                dcs_log(0, 0, "at %s(%s:%d) 数据权限检查失败 [%d][%d]",__FUNCTION__,__FILE__,__LINE__,rowcount,count);
            }
        }
    }
    cbuf_destroy(&sqlbuf);
    cbuf_destroy(&acl_sqlbuf);
    carray_destory(&bind);

    return ret;
}

int module_select_one(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[2];
    int params_len;
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;
    char *key;
    json_object *data;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len <= 0) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    //organize json
    key = "data";
    if(params_len > 1) {
        key = params[1];
    }

    if(strcmp(key, "request") == 0) {
        data = request;
    } else {
        data = json_util_object_get(response, key);
        if(data == NULL) {
            data = json_object_new_object();
            json_object_object_add(response, key, data);
        }
    }

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);

    if(generate_sql(&sql_ctx, params[0], &sqlbuf, &bind) != 0) {
        ret = -1;
        snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, select_fetch_row_handler, data, err_msg, err_size) < 0) {
            dcs_log(0, 0, "at %s(%s:%d) db_execute_select fail",__FUNCTION__,__FILE__,__LINE__);
            ret = -1;
        }
    }
    carray_destory(&bind);
    cbuf_destroy(&sqlbuf);

    return ret;
}

int module_batch_select_one(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;


    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "数据不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; i < len; i ++) {
            json_object *row = json_object_array_get_idx(request, i);

            if(module_select_one(config, ctx, row, response, err_msg, err_size) < 0) {
                ret = -1;
                break;
            }
        }
    }

    return ret;
}


int module_select_page(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[2];
    int params_len, page, page_size;
    char page_str[20], page_size_str[20];
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf, totalbuf, rowsbuf;
    carray_t bind;
    char *key, *sql_stmt, *start, *end;
    const char *p;
    json_object *rows;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len <= 0) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    //organize json
    key = "rows";
    if(params_len > 1) {
        key = params[1];
    }

    rows = json_util_object_get(response, key);
    if(rows == NULL) {
        rows = json_object_new_array();
        json_object_object_add(response, key, rows);
    }

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    cbuf_init(&totalbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    cbuf_init(&rowsbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);


    if(generate_sql(&sql_ctx, params[0], &sqlbuf, &bind) != 0) {
        ret = -1;
        snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        p = json_util_object_get_string(request, "page");
        if(p == NULL) {
            p = "1";
        }
        page = atoi(p);

        p = json_util_object_get_string(request, "rows");
        if(p == NULL) {
            p = "10";
        }
        page_size = atoi(p);

        if(page <= 0) {
            page = 1;
        }

        if(page_size <= 0) {
            page_size = 10;
        }

        sql_stmt = cbuf_str(&sqlbuf);
        start = strstr(sql_stmt, " from ");
        end = strstr(sql_stmt, " order by ");

        if(start == NULL) {
            goto error;
        }

        if(end == NULL) {
            end = sql_stmt + strlen(sql_stmt);
        }

        cbuf_append(&totalbuf, SELECT_TOTAL, sizeof(SELECT_TOTAL)-1);
        cbuf_append(&totalbuf, start,  end - start);


        if(sql_execute(ctx->con, cbuf_str(&totalbuf), &bind, select_fetch_total_handler, response, err_msg, err_size) < 0) {
            dcs_log(0, 0, "at %s(%s:%d) db_execute_select fail",__FUNCTION__,__FILE__,__LINE__);
            ret = -1;
            goto error;
        }

        int l = carray_size(&bind);

        cbuf_append(&rowsbuf, SELECT_PAGE_START, sizeof(SELECT_PAGE_START)-1);
        cbuf_append(&rowsbuf, sql_stmt,  strlen(sql_stmt));
        cbuf_printf(&rowsbuf, SELECT_PAGE_END, l, l + 1, l + 2, l + 3);

        snprintf(page_str, sizeof(page_str), "%d", page);
        snprintf(page_size_str, sizeof(page_size_str), "%d", page_size);

        carray_append(&bind, page_str);
        carray_append(&bind, page_size_str);
        carray_append(&bind, page_str);
        carray_append(&bind, page_size_str);

        void *fetch_ctx[] = {&page_size, rows};
        if(sql_execute(ctx->con, cbuf_str(&rowsbuf), &bind, select_fetch_rows_handler, fetch_ctx, err_msg, err_size) < 0) {
            dcs_log(0, 0, "at %s(%s:%d) db_execute_select fail",__FUNCTION__,__FILE__,__LINE__);
            ret = -1;
        }
    }
error:

    carray_destory(&bind);
    cbuf_destroy(&rowsbuf);
    cbuf_destroy(&totalbuf);
    cbuf_destroy(&sqlbuf);

    return ret;
}

int module_select_list(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[3], *key, *temp_key;
    int params_len, rowcount;
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;
    json_object *rows;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len <= 0) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    //organize json
    key = "rows";
    if(params_len > 1) {
        key = params[1];
    }

    rowcount = 1000;
    if(params_len > 2) {
        rowcount = atoi(params[2]);
    }

    if(strcmp(key, "request") == 0) {
        rows = json_object_new_array();
        json_object_object_add(request, "rows", rows);
    } else if(strncmp(key, "request.", 8) == 0) {
        temp_key = key + 8;
        rows = json_object_new_array();
        json_object_object_add(request, temp_key, rows);
    } else {
        rows = json_object_new_array();
        json_object_object_add(response, key, rows);
    }

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);

    if(generate_sql(&sql_ctx, params[0], &sqlbuf, &bind) != 0) {
        ret = -1;
        snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        //dcs_debug(0, 0, "[%s] %d", cbuf_str(&sqlbuf), carray_size(&bind));
        void *fetch_ctx[] = {&rowcount, rows};
        if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, select_fetch_rows_handler, fetch_ctx, err_msg, err_size) < 0) {
            ret = -1;
        }
    }
    carray_destory(&bind);
    cbuf_destroy(&sqlbuf);

    return ret;
}

int module_batch_select_list(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;


    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "数据不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; i < len; i ++) {
            json_object *row = json_object_array_get_idx(request, i);

            if(module_select_list(config, ctx, row, response, err_msg, err_size) < 0) {
                ret = -1;
                break;
            }
        }
    }

    return ret;
}


int module_select_column_list(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[3], *key, *temp_key;
    int params_len, rowcount;
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;
    json_object *rows;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len <= 0) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    //organize json
    key = "rows";
    if(params_len > 1) {
        key = params[1];
    }

    rowcount = 1000;
    if(params_len > 2) {
        rowcount = atoi(params[2]);
    }

    if(strcmp(key, "request") == 0) {
        rows = json_object_new_array();
        json_object_object_add(request, "rows", rows);
    } else if(strncmp(key, "request.", 8) == 0) {
        temp_key = key + 8;
        rows = json_object_new_array();
        json_object_object_add(request, temp_key, rows);
    } else {
        rows = json_object_new_array();
        json_object_object_add(response, key, rows);
    }

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);

    if(generate_sql(&sql_ctx, params[0], &sqlbuf, &bind) != 0) {
        ret = -1;
        snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        //dcs_debug(0, 0, "[%s] %d", cbuf_str(&sqlbuf), carray_size(&bind));
        void *fetch_ctx[] = {&rowcount, rows};
        if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, select_fetch_column_handler, fetch_ctx, err_msg, err_size) < 0) {
            ret = -1;
        }
    }
    carray_destory(&bind);
    cbuf_destroy(&sqlbuf);

    return ret;
}


int module_batch_select_column_list(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;


    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "数据不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; i < len; i ++) {
            json_object *row = json_object_array_get_idx(request, i);

            if(module_select_column_list(config, ctx, row, response, err_msg, err_size) < 0) {
                ret = -1;
                break;
            }
        }
    }

    return ret;
}

int module_insert(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, rowcount = 0;
    char param_list[512+1];
    char *params[3], *sql_id = NULL, *data_sql_id = NULL, *key_sql_id = NULL; //最多3个参数
    int params_len;

    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));
    if(params_len < 1) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    sql_id = params[0];

    if(params_len > 1) {
        data_sql_id = params[1];
    }

    if(params_len > 2) {
        key_sql_id = params[2];
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    dcs_debug(0, 0, "at %s(%s:%d) [%s][%d]",__FUNCTION__,__FILE__,__LINE__,config->param_list,params_len);

    //生成key
    if(!cstr_empty(key_sql_id)) {

        cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
        carray_init(&bind, NULL);

        if(generate_sql(&sql_ctx, key_sql_id, &sqlbuf, &bind) != 0) {
            ret = -1;
            snprintf(err_msg, err_size, "系统错误,生成SQL失败");
        } else {
            if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, select_fetch_row_handler, request, err_msg, err_size) < 0) {
                dcs_log(0, 0, "at %s(%s:%d) sql_execute fail",__FUNCTION__,__FILE__,__LINE__);
                ret = -1;
            }
        }

        carray_destory(&bind);
        cbuf_destroy(&sqlbuf);
    }

    if(ret == 0) {
        //执行insert
        cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
        carray_init(&bind, NULL);

        if(generate_sql(&sql_ctx, sql_id, &sqlbuf, &bind) != 0) {
            ret = -1;
            snprintf(err_msg, err_size, "系统错误,生成SQL失败");
        } else {
            //dcs_debug(0, 0, "[%s] %d", cbuf_str(&sqlbuf), carray_size(&bind));
            if((rowcount = sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, NULL, NULL, err_msg, err_size)) < 0) {
                ret = -1;
            } else if(!cstr_empty(data_sql_id) && check_data_acl(ctx, &sql_ctx, data_sql_id, request, rowcount) != 0) {
                //数据权限检查失败
                ret = -1;
                snprintf(err_msg, err_size, "没有权限操作当前记录,数据权限检查失败");
                dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
            }
        }

        carray_destory(&bind);
        cbuf_destroy(&sqlbuf);
    }

    return ret;
}

int module_batch_insert(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;

    dcs_debug(0, 0, "at %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__);

    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "数据不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; i < len; i ++) {
            dcs_debug(0, 0, "at %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
            json_object *row = json_object_array_get_idx(request, i);

            if(module_insert(config, ctx, row, response, err_msg, err_size) < 0) {
                ret = -1;
                break;
            }
            dcs_debug(0, 0, "at %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        }
    }

    dcs_debug(0, 0, "at %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__);

    return ret;
}

int module_update(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, rowcount = 0;
    char param_list[512+1];
    char *params[2]; //最多两个参数
    int params_len;

    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);

    if(generate_sql(&sql_ctx, params[0], &sqlbuf, &bind) != 0) {
        ret = -1;
        snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        //dcs_debug(0, 0, "[%s] %d", cbuf_str(&sqlbuf), carray_size(&bind));

        if((rowcount = sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, NULL, NULL, err_msg, err_size)) < 0) {
            ret = -1;
        } else if(rowcount == 0) {
            //未找到可更新的数据
            ret = -1;
            snprintf(err_msg, err_size, "未找到可更新的数据,请确认有权限操作当前记录");
            dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
        } else if(params_len > 1 && check_data_acl(ctx, &sql_ctx, params[1], request, rowcount) != 0) {
            //数据权限检查失败
            ret = -1;
            snprintf(err_msg, err_size, "没有权限操作当前记录,数据权限检查失败");
            dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
        }
    }

    carray_destory(&bind);
    cbuf_destroy(&sqlbuf);

    return ret;
}


int module_batch_update(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;

    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "数据不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; i < len; i ++) {
            //dcs_debug(0, 0, "at %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
            json_object *row = json_object_array_get_idx(request, i);

            if(module_update(config, ctx, row, response, err_msg, err_size) < 0) {
                ret = -1;
                break;
            }
            //dcs_debug(0, 0, "at %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__);
        }
    }

    return ret;
}



int module_delete(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, rowcount = 0;
    char param_list[512+1];
    char *params[2]; //最多两个参数
    int params_len;

    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);

    if(generate_sql(&sql_ctx, params[0], &sqlbuf, &bind) != 0) {
        ret = -1;
        snprintf(err_msg, err_size, "系统错误,生成SQL失败");
    } else {
        //dcs_debug(0, 0, "[%s] %d", cbuf_str(&sqlbuf), carray_size(&bind));

        if((rowcount = sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, NULL, NULL, err_msg, err_size)) < 0) {
            ret = -1;
        } else if(params_len > 1 && atoi(params[1]) > rowcount) {
            //未找到可删除的数据
            ret = -1;
            snprintf(err_msg, err_size, "未找到可删除的数据");
            dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__,err_size,err_msg);
        }
    }

    carray_destory(&bind);
    cbuf_destroy(&sqlbuf);

    return ret;
}

int module_batch_delete(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;

    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__, err_size, err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; i < len; i ++) {
            json_object *row = json_object_array_get_idx(request, i);

            if(module_delete(config, ctx, row, response, err_msg, err_size) < 0) {
                ret = -1;
                break;
            }
        }
    }

    return ret;
}


int module_callproc(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[128];
    int params_len;
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;
    char out_param[MAX_OUT_SIZE][MAX_OUT_VARCHAR_SIZE];
    int out_count = 0;
    void *sql_data[] = {ctx, request};

    //dcs_debug(0, 0, "xjb %s %s", config->test, json_object_to_json_string(request));

    //前提表达式检测
    if(!fun_config_test_value(config->test, ctx, request)) {
        return 0;
    }

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
    carray_init(&bind, NULL);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len <= 0) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    //第一个参数是存储过程名
    cbuf_printf(&sqlbuf, "begin %s(", params[0]);

    int i;
    for(i = 1; i < params_len; i ++) {
        if(i == 1) {
            cbuf_printf(&sqlbuf, ":b%d", i-1);
        } else {
            cbuf_printf(&sqlbuf, ",:b%d", i-1);
        }

        if(params[i][0] == '#') {
            //目前只支持一个输出
            memset(out_param[out_count], ' ', sizeof(out_param[out_count]));
            out_param[out_count][sizeof(out_param[out_count])-1] = '\0';
            carray_append(&bind, out_param[out_count]);
            out_count ++;
        } else if(params[i][0] == ':') {
            carray_append(&bind, get_value(sql_data, params[i]+1));
        } else {
            carray_append(&bind, params[i]);
        }
    }

    cbuf_printf(&sqlbuf, "); end;");

    //dcs_debug(0, 0, "[%s] %d", cbuf_str(&sqlbuf), carray_size(&bind));
    print_bind(&bind);
    if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, NULL, NULL, err_msg, err_size) < 0) {
        ret = -1;
    } else {
        out_count = 0;
        for(i = 1; i < params_len; i ++) {
            if(params[i][0] == '#') {
                //目前只支持一个输出
                cstr_rtrim(out_param[out_count]);
                dcs_debug(0, 0, "out[%s]", out_param[out_count]);
                json_object_object_add(request, params[i]+1, json_object_new_string(out_param[out_count]));
                out_count ++;
            }
        }
    }

    carray_destory(&bind);
    cbuf_destroy(&sqlbuf);

    return ret;
}

int module_batch_callproc(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    int i, len;

    if(json_object_get_type(request) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "不是数组,不能进行批量操作");
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__, err_size, err_msg);
    } else {
        len = json_object_array_length(request);

        for(i = 0; ret == 0 && i < len; i ++) {
            json_object *row = json_object_array_get_idx(request, i);
            json_object *temp = row;

            //非object处理
            if(json_object_get_type(row) != json_type_object) {
                temp = json_object_new_object();
                json_object_object_add(temp, config->input, row);
                json_object_get(row); //计数加1
            }

            if(module_callproc(config, ctx, temp, response, err_msg, err_size) < 0) {
                ret = -1; //break
            }

            if(json_object_get_type(row) != json_type_object) {
                json_object_put(temp);
            }
        }
    }

    return ret;
}

static json_object *json_util_copy_get(char *dest, json_object *request, json_object *response) {
    json_object *dest_json = NULL;

    char dest_copy[128+1];
    char *params[2];
    int params_len;

    cstr_copy(dest_copy, dest, sizeof(dest_copy));
    params_len = cstr_split(dest_copy, ".", params, ARRAY_SIZE(params));

    if(strcmp(params[0], "request") == 0) {
        dest_json = request;
    } else if(strcmp(params[0], "response") == 0) {
        dest_json = response;
    }

    if(params_len > 1) {
        dest_json = json_util_object_get(dest_json, params[1]);
    }

    return dest_json;
}

static json_object *json_util_copy_add(char *dest, json_object *request, json_object *response, json_type type) {
    json_object *dest_json = NULL, *temp_json;

    char dest_copy[128+1];
    char *params[2];
    int params_len;

    cstr_copy(dest_copy, dest, sizeof(dest_copy));
    params_len = cstr_split(dest_copy, ".", params, ARRAY_SIZE(params));

    if(strcmp(params[0], "request") == 0) {
        dest_json = request;
    } else if(strcmp(params[0], "response") == 0) {
        dest_json = response;
    }

    if(params_len > 1) {
        temp_json = json_util_object_get(dest_json, params[1]);
        if(temp_json == NULL) {
            if(type == json_type_object) {
                temp_json = json_object_new_object();
            } else if(type == json_type_array) {
                temp_json = json_object_new_array();
            }

            json_object_object_add(dest_json, params[1], temp_json);
        }

        dest_json = temp_json;
    }

    return dest_json;
}


int module_copy(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[4];
    int params_len;
    char *from, *to, *key, *new_key;
    json_object *from_json, *to_json, *key_obj;

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len < 4) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }

    from = params[0];
    to = params[1];
    key = params[2];
    new_key = params[3];

    from_json = json_util_copy_get(from, request, response);
    to_json = json_util_copy_get(to, request, response);

    if(to_json == NULL) {
        to_json = json_util_copy_add(to, request, response, json_type_object);
    }

    if(from_json == NULL || to_json == NULL) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }

    //dest_obj = json_object_new_object();
    //json_object_object_add(to_json, dest, dest_obj);

    key_obj = json_util_object_get(from_json, key);

    //dcs_debug(0, 0, "at %s(%s:%d)\n%s",__FUNCTION__,__FILE__,__LINE__,json_object_to_json_string(from_json));
    if(json_object_get_type(to_json) != json_type_object) {
        ret = -1;
        snprintf(err_msg, err_size, "%s不是对象,不能进行操作", to);
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__, err_size, err_msg);
    } else {
        json_object_object_add(to_json, new_key, json_object_get(key_obj));
    }

    return ret;
}


int module_batch_copy(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, len, i;
    char param_list[512+1];
    char *params[4];
    int params_len;
    char *from, *to, *key, *new_key;
    json_object *from_json, *to_json, *key_obj;

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len < 4) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }

    from = params[0];
    to = params[1];
    key = params[2];
    new_key = params[3];

    from_json = json_util_copy_get(from, request, response);
    to_json = json_util_copy_get(to, request, response);

    if(from_json == NULL || to_json == NULL) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s ",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }

    key_obj = json_util_object_get(from_json, key);

    if(json_object_get_type(to_json) != json_type_array) {
        ret = -1;
        snprintf(err_msg, err_size, "%s不是数组,不能进行批量操作", to);
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__, err_size, err_msg);
    } else {
        len = json_object_array_length(to_json);
        for(i = 0; i < len; i ++) {
            json_object *temp = json_object_array_get_idx(to_json, i);
            if(json_object_get_type(temp) != json_type_object) {
                ret = -1;
                dcs_log(0, 0, "at %s(%s:%d) %s不是对象数组",__FUNCTION__,__FILE__,__LINE__, to);
                break;
            } else {
                json_object_object_add(temp, new_key, key_obj);
                json_object_get(key_obj);
            }
        }
    }

    return ret;
}


int module_add(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, i;
    char param_list[512+1];
    char *params[8];
    int params_len;

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len%2 != 0) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }

    for(i = 0; i < params_len; i +=2) {
        json_object_object_add(request, params[i], json_object_new_string(params[i+1]));
    }

    return ret;
}


int module_del(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, i;
    char param_list[512+1];
    char *params[10];
    int params_len;

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    for(i = 0; i < params_len; i +=1) {
        json_object_put(json_util_object_get(request, params[i]));
    }

    return ret;
}

// 远端地址， 远端文件名， 本地文件， 验证信息
int module_upload(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, i;
    char param_list[512+1];
    char *params[8];
    int params_len;
    char remotepath[128];
    char localpath[128];

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len != 4) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }
		
		char *remotefile = params[1];
		if(remotefile[0] == ':') {
				remotefile = json_util_object_get_string(request, params[1]+1);
		}
    sprintf(remotepath, "ftp://%s/%s", params[0], remotefile);
    sprintf(localpath, "%s/download/%s", document_root, params[2]);
		if(params[2][0] == ':') {
				sprintf(localpath, "%s/download/%s", document_root, json_util_object_get_string(request, params[2]+1));
		}
		if(upload(remotepath, localpath, params[3], 1, 3) < 0) 
			ret = -1;
		dcs_log(0, 0, "at %s(%s:%d) %s@%s:%s",__FUNCTION__,__FILE__,__LINE__,remotepath,params[3],localpath);
    return ret;
}

// 远端地址， 远端文件名， 本地文件， 验证信息
int module_download(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, i;
    char param_list[512+1];
    char *params[8];
    int params_len;
    char remotepath[128];
    char localpath[128];

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len != 4) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误:%s",__FUNCTION__,__FILE__,__LINE__,config->module_name,config->param_list);
        return -1;
    }
		
		char *remotefile = params[1];
		if(remotefile[0] == ':') {
				remotefile = json_util_object_get_string(request, params[1]+1);
		}
    sprintf(remotepath, "ftp://%s/%s", params[0], remotefile);
    sprintf(localpath, "%s/download/%s", document_root, params[2]);
		if(params[2][0] == ':') {
				sprintf(localpath, "%s/download/%s", document_root, json_util_object_get_string(request, params[2]+1));
		}
		if(download(remotepath, localpath, params[3], 1, 3) < 0) 
			ret = -1;
		dcs_log(0, 0, "at %s(%s:%d) %s@%s:%s",__FUNCTION__,__FILE__,__LINE__,remotepath,params[3],localpath);
    return ret;
}

int create_file(char *url, size_t url_size, char *prefix, char *suffix) {
    int fd = -1;

    char datetime[20];
    struct tm now = cdate_now();
    cdate_strftime(&now, "%Y%m%d%H%M%S", datetime, sizeof(datetime));

    //unsigned char uuid_buf[33];
    //gen_uuid(uuid_buf);

    char microseconds[20];
    struct timeval tv;
    gettimeofday(&tv, NULL);
    snprintf(microseconds, sizeof(microseconds), "%ld", tv.tv_usec);

    char filepath[CFILE_MAX_PATH];
    char filedir[CFILE_MAX_PATH];

    //char *prefix = "mchnt_";
    snprintf(filedir, sizeof(filedir), "%s/download", document_root);
    snprintf(filepath, sizeof(filepath), "%s/download/%s", document_root, url);

		char *p = strrchr(filepath, '/');
		snprintf(filedir, p-filepath+1, "%s", filepath);
    if(cfile_mkdirs(filedir, CFILE_DEFAULT_DIR_MODE) != 0) {
        dcs_log(0,0,"at %s(%s:%d) 创建目录[%s]出错",__FUNCTION__,__FILE__,__LINE__,filedir);
    } else if((fd = open(filepath, O_CREAT|O_TRUNC|O_RDWR, S_IRWXU | S_IRWXG | S_IROTH)) < 0) {
        dcs_log(0,0,"at %s(%s:%d) 打开文件失败[%s]",__FUNCTION__,__FILE__,__LINE__,filepath);
    }

    return fd;
}


int module_export_txt(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0;
    char param_list[512+1];
    char *params[3], *sql_id, *delim, *url;
    int params_len;
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len < 3) {
        dcs_log(0, 0, "at %s(%s:%d) %s模块参数配置错误",__FUNCTION__,__FILE__,__LINE__,config->module_name);
        return -1;
    }

    int fd = -1;

    sql_id = params[0];
    delim = params[1];
    url = params[2];
    if(cstr_empty(delim)) {
        delim = ",";
    }
		
		if(url[0] == ':') {
				url = json_util_object_get_string(request, params[2]+1);
		}
    if((fd = create_file(url, sizeof(url), NULL, "txt")) < 0) {
        ret = -1;
        snprintf(err_msg, err_size, "创建文件失败");
        dcs_log(0, 0, "at %s(%s:%d) %s[%s]",__FUNCTION__,__FILE__,__LINE__,err_msg,url);
    } else {
        dcs_log(0, 0, "at %s(%s:%d) fd[%d]",__FUNCTION__,__FILE__,__LINE__,fd);
        cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
        carray_init(&bind, NULL);

        if(generate_sql(&sql_ctx, sql_id, &sqlbuf, &bind) != 0) {
            ret = -1;
            snprintf(err_msg, err_size, "系统错误,生成SQL失败");
        } else {
            void *fetch_ctx[] = {&fd, delim};

            if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, select_fetch_export_txt_handler, fetch_ctx, err_msg, err_size) < 0) {
                ret = -1;
            } else {
                json_object_object_add(response, "url", json_object_new_string(url));
            }
        }
        carray_destory(&bind);
        cbuf_destroy(&sqlbuf);

        close(fd);
    }

    return ret;
}


int module_export_xls(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    int ret = 0, i;
    char param_list[512+1];
    char *params[20], *template_file, *suffix, *url;
    int params_len, row, col;
    sql_ctx_t sql_ctx;
    cbuf_t sqlbuf;
    carray_t bind;

    //init sql_ctx
    sql_ctx_init(&sql_ctx, request, ctx, get_current_value, gen_data_acl);

    cstr_copy(param_list, config->param_list, sizeof(param_list));
    params_len = cstr_split(param_list, ",", params, ARRAY_SIZE(params));

    if(params_len < 6 || (params_len-2)%4 != 0) {
        snprintf(err_msg, err_size, "%s模块参数配置错误", config->module_name);
        dcs_log(0, 0, "at %s(%s:%d) %s",__FUNCTION__,__FILE__,__LINE__,err_msg);
        return -1;
    }

    int fd = -1;
    char filename[CFILE_MAX_PATH];
    char *p;

    template_file = params[0];

    //计算后缀
    suffix = "xls";
    p = template_file + strlen(template_file);
    while(--p >= template_file) {
        if(*p == '.') {
            suffix = p + 1;
            break;
        }
    }
		
		url = params[1];
		if(url[0] == ':') {
				url = json_util_object_get_string(request, params[1]+1);
		}
    if((fd = create_file(url, sizeof(url), NULL, suffix)) < 0) {
        ret = -1;
        snprintf(err_msg, err_size, "创建文件失败");
        dcs_log(0, 0, "at %s(%s:%d) %s[%s]",__FUNCTION__,__FILE__,__LINE__,err_msg,url);
    } else {
        dcs_log(0, 0, "at %s(%s:%d) fd[%d]",__FUNCTION__,__FILE__,__LINE__, fd);
        close(fd); //关闭文件

        BookHandle book = NULL;
        SheetHandle sheet = NULL;

        snprintf(filename, sizeof(filename), "%s/template/%s", document_root, template_file);
        //拷贝模板文件,加载模板
        if(strcmp(suffix, "xlsx") == 0) {
            book = xlCreateXMLBook();
        } else {
            book = xlCreateBook();
        }

        if(book == NULL || xlBookLoad(book,filename) == 0) {
            ret = -1;
            snprintf(err_msg, err_size, "加载模板文件%s失败", template_file);
        } else {

            cbuf_init(&sqlbuf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
            for(i = 2; ret == 0 && i < params_len; i += 4) {
                if((sheet = xlBookGetSheet(book, atoi(params[i+1]))) == NULL) {

                    ret = -1;
                    snprintf(err_msg, err_size, "sheet下标配置错误");
                    dcs_log(0, 0, "at %s(%s:%d) %s[%s]",__FUNCTION__,__FILE__,__LINE__,err_msg, params[i+1]);
                } else {
                    row = atoi(params[i+2]);
                    col = atoi(params[i+3]);

                    void *fetch_ctx[] = {sheet, &row, &col};

                    cbuf_reset(&sqlbuf);
                    carray_init(&bind, NULL);
                    if(generate_sql(&sql_ctx, params[i], &sqlbuf, &bind) != 0) {
                        ret = -1;
                        snprintf(err_msg, err_size, "SQL配置错误");
                        dcs_log(0, 0, "at %s(%s:%d) %s[%s]",__FUNCTION__,__FILE__,__LINE__,err_msg, params[i]);
                    } else if(sql_execute(ctx->con, cbuf_str(&sqlbuf), &bind, select_fetch_export_xls_handler, fetch_ctx, err_msg, err_size) < 0) {
                        ret = -1;
                    }
                    carray_destory(&bind);
                }
            }


            if(ret == 0) {
                snprintf(filename, sizeof(filename), "%s/download/%s", document_root, url);
                if(xlBookSave(book, filename) == 0) {
                    ret = -1;
                    snprintf(err_msg, err_size, "保存文件失败");
                } else {
                    json_object_object_add(response, "url", json_object_new_string(url));
                }
            }

            cbuf_destroy(&sqlbuf);
        }

        if(book != NULL) {
            xlBookRelease(book);
        }
    }

    return ret;
}

struct execute_module my_check_module[] = {
    {"select_page", &module_select_page},
    {"select_one", &module_select_one},
    {"select_list", &module_select_list},
    {"insert", &module_insert},
    {"update", &module_update},
    {"delete", &module_delete},
    {"callproc", &module_callproc},
    {"copy", &module_copy},
    {"batch_select_one", &module_batch_select_one},
    {"batch_select_list", &module_batch_select_list},
    {"batch_select_column_list", &module_batch_select_column_list},
    {"batch_insert", &module_batch_insert},
    {"batch_update", &module_batch_update},
    {"batch_delete", &module_batch_delete},
    {"batch_callproc", &module_batch_callproc},
    {"batch_copy", &module_batch_copy},
    {"export_txt", &module_export_txt},
    {"export_xls", &module_export_xls},
    {"add", &module_add},
    {"del", &module_del},
    {"upload", &module_upload},
    {"download", &module_download},
    {NULL,NULL}
};

module_fn search_execute_module(const char *module_name) {
    if(module_name == NULL)
        return NULL;

    int i = 0;

    while(my_check_module[i].module_name != NULL) {
        if(strcmp(my_check_module[i].module_name,module_name) == 0)
            break;
        i++;
    }
    return (my_check_module[i].module_name == NULL ? NULL : my_check_module[i].fn);
}

int execute_config(fun_config_t *config, process_ctx_t *ctx, json_object *request, json_object *response, char *err_msg, size_t err_size) {
    module_fn fn = search_execute_module(config->module_name);

    int ret;

    if(fn) {
        //前提表达式检测
        if(fun_config_test_value(config->test_input, ctx, request)) {
            json_object *input = request;
            if(!cstr_empty(config->input)) {
                input = json_util_object_get(request, config->input);
            }

            ret = fn(config, ctx, input, response, err_msg, err_size);
        }
    } else {
        ret = -1;
        snprintf(err_msg, err_size, "配置错误,没有该模块,module_name=%s", config->module_name);
        dcs_log(0, 0, "at %s(%s:%d) %.*s",__FUNCTION__,__FILE__,__LINE__, err_size, err_msg);
    }

    return ret;
}

static int select_fetch_fun_config_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    fun_config_t *config;

    config = &g_fun_config[rownum-1];
    cstr_copy(config->url, oci_get_string(rs, 1), sizeof(g_fun_config[0].url));
    cstr_copy(config->module_name, oci_get_string(rs, 2), sizeof(g_fun_config[0].module_name));
    cstr_copy(config->param_list, oci_get_string(rs, 3), sizeof(g_fun_config[0].param_list));
    cstr_copy(config->exec_type, oci_get_string(rs, 4), sizeof(g_fun_config[0].exec_type));
    cstr_copy(config->input, oci_get_string(rs, 5), sizeof(g_fun_config[0].input));
    cstr_copy(config->test, oci_get_string(rs, 6), sizeof(g_fun_config[0].test));
    cstr_copy(config->test_input, oci_get_string(rs, 7), sizeof(g_fun_config[0].test_input));

    return 1;
}



/**
*加载功能配置
*@param con 数据库连接
*@return 成功返回0,失败返回-1
*/
int load_fun_config(oci_connection_t *con) {
    int res = 0;
    char *sql;

 
    bzero(g_fun_config, sizeof(g_fun_config));
    g_fun_config_len = 0;

    if(res == 0) {
        sql = "select url, module_name, param_list, exec_type, input, test, test_input from fun_config a order by url, exec_type, order_no";
        if((g_fun_config_len = sql_execute(con, sql, NULL, select_fetch_fun_config_handler, NULL, NULL, 0)) < 0) {
            res = -1;
        }
    }

    return res;
}

//查找fun_info
int search_compare_fun_info(const void *k1, const void *k2) {
    char *url = (char *)k1;
    fun_info_t *d2= (fun_info_t *)k2;

    return strcmp(url, d2->url);
}


fun_info_t *search_fun_info(char *url) {
    return bsearch(url, g_fun_info, g_fun_info_len, sizeof(g_fun_info[0]), search_compare_fun_info);
}

//查找fun_config最小下标记录
int search_compare_fun_config_min(const void *k1, const void *k2) {
    int ret;
    char *url = (char *)k1;
    fun_config_t *d2= (fun_config_t *)k2;
    ret = strcmp(url, d2->url);
    if(ret == 0 && d2 > g_fun_config &&
       strcmp(d2[-1].url, d2[0].url) == 0) {
        ret = -1;
    }

    return ret;
}


fun_config_t *search_fun_config_min(char *url) {
    return bsearch(url, g_fun_config, g_fun_config_len, sizeof(g_fun_config[0]), search_compare_fun_config_min);
}

static int select_fetch_id_handler(void *ctx, oci_resultset_t *rs, int rownum) {
    process_ctx_t *pctx = (process_ctx_t *)ctx;
    cstr_copy(pctx->log_id, oci_get_string(rs, 1), sizeof(pctx->log_id));
    return 0; //终止fetch
}


int generate_log_id(process_ctx_t *ctx, char *err_msg, size_t err_size) {
    if(sql_execute(ctx->con, "select seq_log_id.nextval from dual", NULL, select_fetch_id_handler, ctx, err_msg, err_size) <= 0) {
        return -1; //出错
    }
    return 0; //成功
}

int write_oper_log(process_ctx_t *ctx, char *err_msg, int flag) {
    int res = 0;

    session_attr_t *attr = (session_attr_t *)ctx->session->remark;

    carray_t bind;

    carray_init(&bind, NULL);
    carray_append(&bind, ctx->log_id);
    carray_append(&bind, attr->attr1);
    carray_append(&bind, ctx->ip);
    carray_append(&bind, flag == 2 ? "1" : "0");
    carray_append(&bind, err_msg);

    if(sql_execute(ctx->con, "begin write_oper_log(:log_id, :login_name, :login_ip, :flag, :remark); end;", &bind, NULL, NULL, NULL, 0) < 0) {
        res = -1;
    }
    carray_destory(&bind);

    return res;
}

int str_replace(char* str,char* str_src, char* str_des) {
    char *ptr=NULL;
    char buff[256];
    char buff2[256];
    int i = 0;
    
    if(str != NULL){
        strcpy(buff2, str);
    }else{
        printf("str_replace err!/n");
        return -1;
    }
    memset(buff, 0x00, sizeof(buff));
    while((ptr = strstr( buff2, str_src)) !=0)
    {
        if(ptr-buff2 != 0) memcpy(&buff[i], buff2, ptr - buff2);
        memcpy(&buff[i + ptr - buff2], str_des, strlen(str_des));
        i += ptr - buff2 + strlen(str_des);
        strcpy(buff2, ptr + strlen(str_src));
    }
    strcat(buff,buff2);
    strcpy(str,buff);
    return 0;
}

int strsplit(const char *str, char *delim, char **array, int array_size) {
    int count = 0;
    int i, l = strlen(delim);
    char *str_copy, *start, *p;

    if(array_size > 0)
    {
        str_copy = strdup(str);
        p = str_copy;
        for(start = p; count < array_size-1 && p[0] != '\0'; p++)
        {
            for(i = 0; i < l; i ++)
            {
                if(p[0] == delim[i])
                {
                    p[0] = '\0';
                    array[count++] = start;
                    start = p + 1;
                }
            }
        }

        array[count++] = start;
    }

    return count;
}