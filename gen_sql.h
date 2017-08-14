/******************************************************************************
  �� �� ��   : gen_sql.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��11��7��
  ��������   : gen_sql.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��11��7��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __gen_sql_h
#define __gen_sql_h

#include "cbuf.h"
#include "carray.h"
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern int gen_sql(char *sql, cbuf_t *sqlbuf, carray_t *bind);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __gen_sql_h */
