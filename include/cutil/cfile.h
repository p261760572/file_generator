/******************************************************************************
  �� �� ��   : cfile.h
  �� �� ��   : ����
  ��    ��   : xjb
  ��������   : 2014��10��17��
  ��������   : cfile.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��10��17��
    ��    ��   : xjb
    �޸�����   : �����ļ�

*****************************************************************************/

#ifndef __cfile_h
#define __cfile_h


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#define CFILE_MAX_PATH 1024

#define CFILE_DEFAULT_DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define CFILE_DEFAULT_FILE_MODE (S_IRWXU | S_IRGRP | S_IROTH)

extern int cfile_mkdirs(const char *dir, mode_t mode);
extern char *cfile_get_filename(char *path);
extern char *cfile_get_suffix(char *filename);
extern int cfile_create(char *path);
extern int cfile_copy(char *source_path, char *dest_path);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __cfile_h */
