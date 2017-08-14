/******************************************************************************
  �� �� ��   : json_ext.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��12��3��
  ��������   : json_ext.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��12��3��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __json_ext_h
#define __json_ext_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern json_object* json_util_object_from_file(const char *filename, const char *fromcode, const char *tocode);
extern json_object *json_util_object_get(json_object *obj, const char *key);
extern int json_util_object_get_int(json_object* obj, const char *key);
extern const char *json_util_object_get_string(json_object* obj, const char *key);
extern int json_util_object_to_file(const char *filename, struct json_object *obj, const char *fromcode, const char *tocode);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __json_ext_h */
