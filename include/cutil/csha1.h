/******************************************************************************
  �� �� ��   : csha1.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��12��14��
  ��������   : csha1.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��12��14��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __csha1_h
#define __csha1_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdint.h>


struct csha1_ctx_s {
    uint32_t h0, h1, h2, h3, h4;
	uint64_t length;
    unsigned char buf[64];
    unsigned int buf_len;
};

typedef struct csha1_ctx_s csha1_ctx_t;

extern void csha1_final(csha1_ctx_t *ctx, char digest[41]);
extern void csha1_init(csha1_ctx_t *ctx);
extern void csha1_update(csha1_ctx_t *ctx, char *data, unsigned int data_size);
extern char *csha1_hash(char digest[41], ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __csha1_h */
