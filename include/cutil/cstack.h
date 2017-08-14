/******************************************************************************
  �� �� ��   : cstack.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��10��17��
  ��������   : cstack.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��10��17��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cstack_h
#define __cstack_h

#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef clist_t cstack_t;

#define cstack_init clist_init

#define cstack_destroy clist_destroy

#define cstack_new clist_new

#define cstack_free clist_free

int cstack_push(cstack_t *stack, void *data);

int cstack_pop(cstack_t *stack, void **data);

#define cstack_peek(stack) (clist_head(stack) == NULL ? NULL : clist_data(clist_head(stack)))

#define cstack_size clist_size


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cstack_h */
