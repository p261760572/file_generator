/******************************************************************************
  �� �� ��   : cmd5.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��6��9��
  ��������   : cmd5.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��6��9��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cmd5_h
#define __cmd5_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct cmd5_ctx_s {
    unsigned int buf[4];
    unsigned int bits[2];
    unsigned char in[64];
} cmd5_ctx_t;

extern void cmd5_init(cmd5_ctx_t * ctx);
extern void cmd5_update(cmd5_ctx_t * ctx,unsigned char * buf,unsigned int len);
extern void cmd5_digest(cmd5_ctx_t * ctx, unsigned char digest[16]);
extern void cmd5_hexdigest(cmd5_ctx_t * ctx, unsigned char buf[33]);
extern unsigned char *cmd5_hash(unsigned char buf[33], ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cmd5_h */
