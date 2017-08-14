/******************************************************************************
  �� �� ��   : cbit.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2015��3��31��
  ��������   : cbit.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2015��3��31��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cbit_h
#define __cbit_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern int cbit_get(const unsigned char *bits, int pos);
extern void cbit_rot_left(unsigned char *bits, int size, int count);
extern void cbit_rot_right(unsigned char *bits, int size, int count);
extern void cbit_set(unsigned char *bits, int pos, int state);
extern void cbit_xor(const unsigned char *bits1, const unsigned char *bits2,
                       unsigned char *bitsx, int size);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cbit_h */
