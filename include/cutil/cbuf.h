/******************************************************************************
  �� �� ��   : cbuf.h
  �� �� ��   : ����
  ��    ��   : xjb(beymy.en@gmail.com)
  ��������   : 2014��2��27��
  ��������   : cbuf.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��2��27��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/


#ifndef __cbuf_h
#define __cbuf_h

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

struct cbuf_s
{
    char *start; /**<  ����Ŀ�ʼλ�� */
    char *pos; /**<  ���嵱ǰ��дλ�� */
    char *end; /**<  ����Ľ���λ��+1 */

    size_t size_min; /**<  �������С��С */
    size_t size_max; /**<  ���������С,0��ʾ������ */
    size_t size_real; /**<  �����ʵ�ʴ�С */
};

typedef struct cbuf_s cbuf_t;

extern int cbuf_append(cbuf_t * buf, const char *str, size_t len);
extern int cbuf_remove(cbuf_t * buf, size_t n);
extern void cbuf_destroy(cbuf_t * buf);
extern void cbuf_free(cbuf_t * buf);
extern int cbuf_init(cbuf_t *buf, size_t min, size_t max);
extern cbuf_t *cbuf_new(size_t min, size_t max);
extern int cbuf_vprintf(cbuf_t * buf, const char *format, va_list args);
extern int cbuf_printf(cbuf_t * buf, const char *format, ...);
extern int cbuf_printf_hex(cbuf_t * buf, const char *str, size_t len);
extern void cbuf_reset(cbuf_t *buf);

#define cbuf_len(buf) ((buf)->pos - (buf)->start)
#define cbuf_str(buf) ((buf)->start)

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif //__cbuf_h

