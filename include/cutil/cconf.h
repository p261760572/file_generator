/******************************************************************************
  �� �� ��   : conf.h
  �� �� ��   : ����
  ��    ��   : xjb(beymy.en@gmail.com)
  ��������   : 2014��2��23��
  ��������   : conf.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��2��23��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __conf_h
#define __conf_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define CCONF_LINE_SIZE 1024

typedef int (*config_option_handler)(void *,const char *, const char *);


/**
*���������ļ�
*@return �ɹ�����0,������-1
*/
extern int config_parse_file(char *filename, void *data, config_option_handler handler);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __conf_h */
