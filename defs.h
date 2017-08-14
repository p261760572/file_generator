
#ifndef __DEFS_H__
#define __DEFS_H__

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define MAX_BUFFER_SIZE 4096

#define MAX_PATH 1024

//oracle
#define MAX_NUM_COLUMN 128
#define MAX_LENGTH_COLUMN_NAME 64


#define strempty(a) \
    (a == NULL || strlen(a) == 0)

#define strcpy_s(a,b,c) {\
    size_t l = c;\
    strncpy(a,b,l);\
    a[l-1]='\0';\
}

#define rtrim(a) {                  \
    char *p = strchr(a, '\0');      \
    while(p>a && isspace(p[-1]))    \
        p--;                        \
    *p = '\0';                      \
}


//内部调试日志
#define c_error(...) {  \
    fprintf(stderr,"error=>at %s(%s:%d) ",__FUNCTION__,__FILE__,__LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n");  \
}

#define c_debug(...) {  \
    fprintf(stdout, "debug=>at %s(%s:%d) ",__FUNCTION__,__FILE__,__LINE__); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n");  \
}

extern int dcs_log_open(char *, char *);
extern void dcs_debug(void *ptrbytes, int nbytes,const char * message,...);
extern void dcs_log(void *ptrbytes, int nbytes,const char * message,...);
extern int DasConectDB();
extern int DasEnd(int);



#endif /* __DEFS_H__ */
