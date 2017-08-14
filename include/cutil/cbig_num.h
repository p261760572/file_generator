/******************************************************************************
  �� �� ��   : cbig_num.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��12��22��
  ��������   : cbig_num.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��12��22��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cbig_num_h
#define __cbig_num_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdint.h>

#define CBIG_NUM_SIZE 100
#define CBIG_NUM_BYTE_SIZE 100*(sizeof(uint32_t)/sizeof(char))


struct cbig_num_s {
	uint32_t d[CBIG_NUM_SIZE];	
};

typedef struct cbig_num_s cbig_num_t;

extern cbig_num_t cbig_num_add(cbig_num_t a, uint64_t b);
extern cbig_num_t cbig_num_add2(cbig_num_t a, cbig_num_t b);
extern int cbig_num_cmp(cbig_num_t a, cbig_num_t b);
extern cbig_num_t cbig_num_div(cbig_num_t a, uint64_t b);
extern cbig_num_t cbig_num_div2(cbig_num_t a, cbig_num_t b);
extern cbig_num_t cbig_num_mod(cbig_num_t a, cbig_num_t b);
extern cbig_num_t cbig_num_mul(cbig_num_t a, uint64_t b);
extern cbig_num_t cbig_num_mul2(cbig_num_t a, cbig_num_t b);
extern cbig_num_t cbig_num_parse(char *s, int system);
extern void cbig_num_print(cbig_num_t a, char *info);
extern cbig_num_t cbig_num_set(char *value, int value_length);
extern cbig_num_t cbig_num_sub(cbig_num_t a, cbig_num_t b);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cbig_num_h */
