/******************************************************************************
  �� �� ��   : data_acl.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��3��30��
  ��������   : data_acl.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��3��30��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __data_acl_h
#define __data_acl_h

#include "process_util.h"
#include "db_hander.h"
#include "cbuf.h"
#include "sql.h"
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern int gen_data_acl(sql_ctx_t *ctx, char *res_code, char *alias, cbuf_t *buf);
extern int load_data_rule();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __data_acl_h */
