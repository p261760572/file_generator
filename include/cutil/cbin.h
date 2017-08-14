/******************************************************************************
  �� �� ��   : cbin.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��4��4��
  ��������   : cbin.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��4��4��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cbin_h
#define __cbin_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern void cbin_bin_to_hex(const unsigned char *bin, unsigned char *hex, int size);
extern void cbin_hex_to_bin(const unsigned char *hex, unsigned char *bin, int size);
extern void cbin_bcd_to_asc(const unsigned char *bcd, unsigned char *asc, int size);
extern void cbin_asc_to_bcd(const unsigned char *asc, unsigned char *bcd, int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cbin_h */
