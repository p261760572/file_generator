/******************************************************************************
  �� �� ��   : clist.h
  �� �� ��   : ����
  ��    ��   : xjb(beymy.en@gmail.com)
  ��������   : 2014��2��25��
  ��������   : clist.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��2��25��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __clist_h
#define __clist_h


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct clist_elem_s
{
    void *data;
    struct clist_elem_s *next;
};

typedef struct clist_elem_s clist_elem_t;

struct clist_s
{
    int size;
    void (*free_fn)(void *data);
    clist_elem_t *head;
    clist_elem_t *tail;
};

typedef struct clist_s clist_t;

extern void clist_init(clist_t *list, void (*free_fn)(void *data));
extern void clist_destroy(clist_t *list);
extern clist_t *clist_new(void (*free_fn)(void *data));
extern void clist_free(clist_t *list);
extern int clist_insert_first(clist_t *list, void *data);
extern int clist_append(clist_t *list, void *data);
extern int clist_insert_next(clist_t *list, clist_elem_t *element, void *data);
extern int clist_remove_first(clist_t *list, clist_elem_t *element, void **data);
extern int clist_remove_next(clist_t *list, clist_elem_t *element, void **data);

#define clist_size(list) ((list)->size)
#define clist_head(list) ((list)->head)
#define clist_tail(list) ((list)->tail)
#define clist_is_head(list, element) ((element) == (list)->head ? 1 : 0)
#define clist_is_tail(element) ((element)->next == NULL ? 1 : 0)
#define clist_data(element) ((element)->data)
#define clist_next(element) ((element)->next)

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __clist_h */
