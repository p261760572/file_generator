/******************************************************************************
  �� �� ��   : cdes.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��4��3��
  ��������   : cdes.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��4��3��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cdes_h
#define __cdes_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern void cdes_cbc_decrypt(const unsigned char *ciphertext, unsigned char *plaintext,
                           const unsigned char *key, int size);
extern void cdes_cbc_encrypt(const unsigned char *plaintext, unsigned char *ciphertext,
                               const unsigned char *key, int size);
extern void cdes_decrypt(const unsigned char *ciphertext, unsigned char *plaintext,
                                  const unsigned char *key);
extern void cdes_ecb_decrypt(const unsigned char *ciphertext, unsigned char *plaintext,
                           const unsigned char *key, int size);
extern void cdes_ecb_encrypt(const unsigned char *plaintext, unsigned char *ciphertext,
                               const unsigned char *key, int size);
extern void cdes_encrypt(const unsigned char *plaintext, unsigned char *ciphertext,
                                  const unsigned char *key);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cdes_h */
