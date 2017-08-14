/******************************************************************************
  �� �� ��   : exp.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��10��19��
  ��������   : exp.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��10��19��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __exp_h
#define __exp_h

#include "carray.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct chash_entry_int_s
{
    char *k;
    int v;
};

typedef struct chash_entry_int_s chash_entry_int_t;


struct exp_context_s
{
    void *data;
    char *(*get)(void *data, const char *name);
};

typedef struct exp_context_s exp_context_t;

struct exp_item_s
{
    void *value;
    int type;
};

typedef struct exp_item_s exp_item_t;


#define MAX_KEYWORD_SIZE 32

extern void exp_destroy();
extern int exp_init();
extern int test_exp(exp_context_t *context, char *expression);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __exp_h */
