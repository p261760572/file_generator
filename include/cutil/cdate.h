/******************************************************************************
  �� �� ��   : cdate.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��11��14��
  ��������   : cdate.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��11��14��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cdate_h
#define __cdate_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern struct tm cdate_now();
extern void cdate_now_date(char *buf, size_t buf_size);
extern void cdate_now_datetime(char *buf, size_t buf_size);
extern void cdate_strftime(struct tm *t, char *format, char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cdate_h */
