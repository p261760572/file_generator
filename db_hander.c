#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "db_hander.h"
#include "ibdcs.h"
#include "cbuf.h"
#include "cstr.h"
#include "gen_sql.h"
#include "sql.h"
#include "cbin.h"
#include "cdes.h"

static int reconnect_flag = 0;

int db_reconnect(int (*caller)()) {
		if(reconnect_flag && caller)
		{
			reconnect_flag = 0;
			return caller();
		}
		return 0;
}

void sql_error_handler(oci_error_t *err) {
    int code = oci_get_error_code(err);
    oci_statement_t *stmt = oci_get_error_stmt(err);

    if(code == -3114 || code == -3113 || code == -3135 ||
       code == 3114 || code == 3113 || code == 3135) {
        reconnect_flag = 1;
    }

    if(code == 1403 && oci_get_stmt_type(stmt) == OCI_ST_SELECT &&
       oci_get_row_count(stmt) > 0) {
        //nothing
    } else {
        dcs_log(0, 0, "at %s(%s:%d) %d %d\n%s", __FUNCTION__, __FILE__, __LINE__,code,oci_get_error_warning(err),oci_get_error_msg(err));
    }
}

/**
*加载功能配置
*@param con 数据库连接
*@param sql 要执行的SQL
*@param bind 绑定变量数组
*@param fetch fetch数据的回调函数,rownum从1开始
*@param ctx fetch函数的上下文
*@param err 错误信息buf
*@param err_size 错误信息buf大小
*
*@return 成功返回影响的记录数,失败返回-1
*/
int sql_execute(oci_connection_t *con, char *sql, carray_t *bind, fetch_caller fetch, void *ctx, char *err_msg, size_t err_size) {
    boolean res = FALSE;
    int count = -1, i, code;
    void *value;
    oci_error_t *oci_err = NULL;

    if(strcmp(sql, TEST_DATABSE_SQL) != 0) {
        cbuf_t buf;
        cbuf_init(&buf, DEFAULT_SQL_SIZE, MAX_SQL_SIZE);
        gen_sql(sql, &buf, bind);
        dcs_debug(0, 0, "at %s(%s:%d) SQL\n%s",__FUNCTION__,__FILE__,__LINE__,cbuf_str(&buf));
        cbuf_destroy(&buf);
    }

    oci_statement_t *stmt = oci_statement_new(con);
    if(stmt == NULL) {
        oci_err = oci_get_last_error();
    } else {
        if(oci_prepare(stmt, sql) != TRUE) {
            oci_err = oci_get_last_error();
        } else {
            for(i = 1; i <= oci_get_bind_count(stmt); i++) {
                if(i > carray_size(bind)) {
                    oci_bind_string_by_pos(stmt, i, "");
                } else if((value = carray_get(bind, i-1)) == NULL) {
                    oci_bind_string_by_pos(stmt, i, "");
                } else {
                    oci_bind_string_by_pos(stmt, i, value);
                }
            }

            if(oci_execute(stmt) != TRUE) {
                oci_err = oci_get_last_error();
            } else {
                res = TRUE;
                if(oci_get_stmt_type(stmt) == OCI_ST_SELECT) {
                    oci_resultset_t *rs = oci_get_resultset(stmt);
                    int fetch_break = 0;
                    for(i = 1; oci_fetch_next(rs); i ++) {
                        if(fetch != NULL) {
                            int ret = fetch(ctx, rs, i);
                            if(ret == 0) {
                                fetch_break = 1;
                                break;
                            } else if(ret < 0) {
                                fetch_break = 1;
                                res = FALSE;
                                break;
                            }
                        }
                    }
                    //fetch可能出错
                    if(fetch_break == 0) {
                        oci_err = oci_get_last_error();
                        code = oci_get_error_code(oci_err);
                        if(code != 0 && code != 1403) {
                            res = FALSE;
                        } else {
                            oci_err = NULL;
                        }
                    }
                }
            }
        }

        if(res == TRUE) {
            count = oci_get_row_count(stmt);
        }
        oci_statement_free(stmt);
    }

    if(oci_err != NULL && err_msg != NULL) {
        cstr_copy(err_msg, oci_get_error_msg(oci_err), err_size);
    }

    return count;
}

static void decrypt_db_pwd(char *db_pwd, char *buf, size_t buf_size) {
    int i;
    unsigned char temp[256+1];
    unsigned char ciphertext[128+1];
    unsigned char plaintext[128+1];
    unsigned char key[128+1] = "2SWqhYC41BGCNKBJg4sRJGaWDL/C78C5EzoGFDCBYtGM64G1Mn/oCQkDBmmaay53yCIJDRlWfwtwuky63I9Wf3oIAXxQNMfob4oJWaYnFweaE7L1BhFPLWbxl5uTV5HC";

    bzero(temp, sizeof(temp));
    bzero(ciphertext, sizeof(ciphertext));
    bzero(plaintext, sizeof(plaintext));

    cstr_copy((char *)temp, db_pwd, sizeof(temp));
    cbin_hex_to_bin(temp, ciphertext, 256);

    for(i = 0; i < 128; i +=8) {
        cdes_decrypt(ciphertext+i, plaintext+i, key+i);
    }

    cstr_copy(buf, (char *)plaintext, buf_size);
}

static int reconnect() {
    char db_pwd[256+1];
    reconnect_flag = 0;

    oci_connection_free(g_db_con);

    bzero(db_pwd, sizeof(db_pwd));
    if(strcmp(g_db_pwd_mode, "1") == 0) {
        decrypt_db_pwd(g_db_pwd, db_pwd, sizeof(db_pwd));
    } else {
        cstr_copy(db_pwd, g_db_pwd, sizeof(db_pwd));
    }
    if((g_db_con = oci_connection_new(g_db, g_db_user, db_pwd)) == NULL) {
        dcs_log(0, 0, "at %s(%s:%d) 重新连接数据库,oci_connection_new失败", __FUNCTION__, __FILE__, __LINE__);
        return -1;
    }

    return 0;
}

int test_databse_connection(oci_connection_t *con) {
    sql_execute(con, TEST_DATABSE_SQL, NULL, NULL, NULL, NULL, 0);
    if(reconnect_flag == 1) {
        if(reconnect() != 0) {
            return -1;
        }
    }

    return 0;
}

void db_errmsg_trans(char *err_msg, size_t err_size) {
    if(strncmp(err_msg, "ORA-20999: ", 11) == 0) {
        char *p = strchr(err_msg, '\n');
        if(p) {
            *p = '\0';
        }
        memmove(err_msg, err_msg+11, err_size - 11);
    }
}

int connect_init() {
		char db_pwd[256+1];

    dcs_log(0, 0, "初始化中...");
    
    g_db = getenv("DB");
    if(cstr_empty(g_db)) {
        dcs_log(0, 0, "at %s(%s:%d) 没有配置环境变量DB", __FUNCTION__, __FILE__, __LINE__);
        return -1;
    }

    g_db_user = getenv("DB_USER");
    if(cstr_empty(g_db_user)) {
        dcs_log(0, 0, "at %s(%s:%d) 没有配置环境变量DB_USER", __FUNCTION__, __FILE__, __LINE__);
        return -1;
    }

    g_db_pwd = getenv("DB_PWD");
    if(cstr_empty(g_db_pwd)) {
        dcs_log(0, 0, "at %s(%s:%d) 没有配置环境变量DB_PWD", __FUNCTION__, __FILE__, __LINE__);
        return -1;
    }

    g_db_pwd_mode = getenv("DB_PWD_MODE");
    if(cstr_empty(g_db_pwd_mode)) {
        g_db_pwd_mode = "0";
    }

    if(oci_initialize(sql_error_handler, OCI_ENV_DEFAULT) == FALSE) {
        dcs_log(0, 0, "oci_initialize失败");
        goto error;
    }

    bzero(db_pwd, sizeof(db_pwd));
    if(strcmp(g_db_pwd_mode, "1") == 0) {
        decrypt_db_pwd(g_db_pwd, db_pwd, sizeof(db_pwd));
    } else {
        cstr_copy(db_pwd, g_db_pwd, sizeof(db_pwd));
    }

    if((g_db_con = oci_connection_new(g_db, g_db_user, db_pwd)) == NULL) {
        dcs_log(0, 0, "oci_connection_new失败");
        goto error;
    }
    
    return 1;

error:
    sql_destroy();
    oci_connection_free(g_db_con);
    oci_cleanup();

    return -1;
}

int load_config() {
		
    if(sql_init(g_db_con) != 0) {
        dcs_log(0, 0, "at %s(%s:%d) SQL库初始化失败", __FUNCTION__, __FILE__, __LINE__);
        goto error;
    }

    if(load_fun_config(g_db_con) != 0) {
        dcs_log(0, 0, "at %s(%s:%d) 加载功能配置失败", __FUNCTION__, __FILE__, __LINE__);
        goto error;
    }
    
    return 1;

error:
    sql_destroy();
    oci_connection_free(g_db_con);
    oci_cleanup();

    return -1;
}

int db_reload(int sys_exit) {
		if(sys_exit == 1) {
        sql_destroy();
        if(sql_init(g_db_con) != 0) {
            dcs_log(0, 0, "at %s(%s:%d) 重新加载配置,初始化SQL库失败", __FUNCTION__, __FILE__, __LINE__);
            return -1;
        }
        else {
            dcs_log(0, 0, "at %s(%s:%d) 重新加载配置,初始化SQL库成功", __FUNCTION__, __FILE__, __LINE__);
        }

				if(load_fun_config(g_db_con) != 0) {
            dcs_log(0, 0, "at %s(%s:%d) 重新加载配置,缓存功能配置数据失败", __FUNCTION__, __FILE__, __LINE__);
            return -1;
        }
        else {
            dcs_log(0, 0, "at %s(%s:%d) 重新加载配置,缓存功能配置数据成功", __FUNCTION__, __FILE__, __LINE__);
        }
    }	
}