/******************************************************************************
  �� �� ��   : cdes3.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��11��16��
  ��������   : cdes3.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��11��16��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __c3des_h
#define __c3des_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern void cdes3_cbc_decrypt(const unsigned char *ciphertext, unsigned char *plaintext,
                           const unsigned char *key, int size);
extern void cdes3_cbc_encrypt(const unsigned char *plaintext, unsigned char *ciphertext,
                               const unsigned char *key, int size);
extern void cdes3_decrypt(const unsigned char *ciphertext, unsigned char *plaintext,
                                  const unsigned char *key);
extern void cdes3_ecb_decrypt(const unsigned char *ciphertext, unsigned char *plaintext,
                           const unsigned char *key, int size);
extern void cdes3_ecb_encrypt(const unsigned char *plaintext, unsigned char *ciphertext,
                               const unsigned char *key, int size);
extern void cdes3_encrypt(const unsigned char *plaintext, unsigned char *ciphertext,
                                  const unsigned char *key);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __c3des_h */
