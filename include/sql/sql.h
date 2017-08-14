/******************************************************************************
  �� �� ��   : sql.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��10��21��
  ��������   : sql.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��10��21��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __sql_h
#define __sql_h

#include "json.h"
#include "cbuf.h"
#include "carray.h"
#include "ocilib.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct sql_ctx_s sql_ctx_t;

struct sql_ctx_s
{
	json_object *json;
	void *data;
	char *(*get)(void *data, const char *name);
	json_object *(*tag_get)(void *data, const char *name);
	int (*gen_data_acl)(sql_ctx_t *ctx, char *res_code, char *alias, cbuf_t *psql);
};

extern int sql_parse(sql_ctx_t *ctx, char *string, cbuf_t *sqlbuf, carray_t *bind);

extern int generate_sql(sql_ctx_t *ctx, char *stmt_id, cbuf_t *sqlbuf, carray_t *bind);

extern void sql_ctx_init(sql_ctx_t *ctx, json_object *json, void *data, char *(*get)(void *data, const char *name), int (*gen_data_acl)(sql_ctx_t *ctx, char *res_code, char *alias, cbuf_t *psql));

extern int sql_init(oci_connection_t *con);

extern void sql_destroy();


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __sql_h */
