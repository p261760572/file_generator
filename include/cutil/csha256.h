/******************************************************************************
  �� �� ��   : csha256.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��12��14��
  ��������   : csha256.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��12��14��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __csha256_h
#define __csha256_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdint.h>


struct csha256_ctx_s {
    uint32_t h0, h1, h2, h3, h4, h5, h6, h7;
	uint64_t length;
    unsigned char buf[64];
    unsigned int buf_len;
};

typedef struct csha256_ctx_s csha256_ctx_t;

extern void csha256_final(csha256_ctx_t *ctx, char digest[65]);
extern void csha256_init(csha256_ctx_t *ctx);
extern void csha256_update(csha256_ctx_t *ctx, char *data, unsigned int data_size);
extern char *csha256_hash(char digest[65], ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __csha256_h */
