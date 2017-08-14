/******************************************************************************
  �� �� ��   : ini_parser.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2016��6��11��
  ��������   : ini_parser.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��6��11��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __ini_parser_h
#define __ini_parser_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

typedef struct ini_parser_s ini_parser_t;

extern char *ini_get(ini_parser_t *self, char *section, char *option);
extern void ini_parse(ini_parser_t *self, char* buffer);
extern void ini_parser_free(ini_parser_t *self);
extern ini_parser_t *ini_parser_new(char comment_char, char delim_char);
extern void ini_set(ini_parser_t *self, const char *section, const char *option, const char *value);
extern void ini_to_file(ini_parser_t *self, FILE *fp);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ini_parser_h */
