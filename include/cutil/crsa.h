/******************************************************************************
  �� �� ��   : rsa.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��12��22��
  ��������   : rsa.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��12��22��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __crsa_h
#define __crsa_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern cbig_num_t crsa_encrypt(cbig_num_t plaintext, cbig_num_t e, cbig_num_t n);
extern  cbig_num_t crsa_decrypt(cbig_num_t ciphertext, cbig_num_t d, cbig_num_t n);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __crsa_h */
