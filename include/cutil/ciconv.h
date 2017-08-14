/******************************************************************************
  �� �� ��   : ciconv.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��12��3��
  ��������   : ciconv.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��12��3��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __ciconv_h
#define __ciconv_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

extern int code_convert(const char *fromcode, const char *tocode, char *inbuf, size_t inlen, char *outbuf, size_t outlen);
extern int gbk_to_utf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
extern int utf8_to_gbk(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ciconv_h */
