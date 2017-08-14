
#ifndef __db_hander_h
#define __db_hander_h

#include "ocilib.h"
#include "process_util.h"
#include "carray.h"
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */
#define TEST_DATABSE_SQL "select 1 from dual"

char *g_db, *g_db_user, *g_db_pwd, *g_db_pwd_mode;
oci_connection_t *g_db_con;

int sql_execute(oci_connection_t *con, char *sql, carray_t *bind, fetch_caller fetch, void *ctx, char *err_msg, size_t err_size);
int test_databse_connection(oci_connection_t *con);
void db_errmsg_trans(char *err_msg, size_t err_size);
int connect_init();
int load_config();
int db_reload(int);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __db_hander_h */
