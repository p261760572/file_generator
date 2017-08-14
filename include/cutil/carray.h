/******************************************************************************
  �� �� ��   : carray.h
  �� �� ��   : ����
  ��    ��   : xjb(beymy.en@gmail.com)
  ��������   : 2014��2��25��
  ��������   : carray.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��2��25��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __carray_h
#define __carray_h


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CARRAY_DEFAULT_SIZE 32

struct carray_s
{
    void **array;
    int size;
    int capacity;
    void (*free_fn)(void *data);
};

typedef struct carray_s carray_t;

extern int carray_init(carray_t *arr, void (*free_fn)(void *data));
extern void carray_destory(carray_t *arr);
extern carray_t *carray_new(void (*free_fn)(void *data));
extern void carray_free(carray_t *arr);
extern void *carray_get(carray_t *arr, int idx);
extern int carray_insert(carray_t *arr, int idx, void *data);
extern int carray_append(carray_t *arr, void *data);
extern int carray_remove(carray_t *arr, int idx, void **data);
extern int carray_size(carray_t *arr);
extern void carray_sort(carray_t *arr, int(*sort_fn)(const void *key1, const void *key2));

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __carray_h */
