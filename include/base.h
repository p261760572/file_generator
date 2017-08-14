#ifndef _BASE_H_
#define _BASE_H_

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <limits.h>
#include "settings.h"
#include "buffer.h"

#include "fdevent.h"
#include "array.h"


#define MAX_JOB 100
/* fcgi_response_header contains ... */
#define HTTP_STATUS         BV(0)
#define HTTP_CONNECTION     BV(1)
#define HTTP_CONTENT_LENGTH BV(2)
#define HTTP_DATE           BV(3)
#define HTTP_LOCATION       BV(4)

typedef struct
{
    struct sockaddr_in addr;
    int       fd;
    int       fde_ndx;
} server_socket;

typedef struct
{
    server_socket ptr[5];

//  size_t size;
    size_t used;
} server_socket_array;

#define  FIRST   1
#define  SECEND  2;

typedef struct
{

    /* timestamps */
    time_t read_idle_ts;
    time_t close_timeout_ts;
    time_t connection_start;
    time_t request_start;
//  struct timeval start_tv;
    size_t request_count;        /* number of requests handled in this connection */
    int fd;                      /* the FD for this connection */
//  int fde_ndx;                 /* index for the fdevent-handler */
//  int ndx;                     /* reverse mapping to server->connection[ndx] */
    /* fd states */
    int head_len;
    int is_readable;
    int is_writable;
    int used;               /*ʹ�ñ�ʶ 0, δʹ�� 1ʹ��*/
    int state;             /*״̬ ָʾһ������������Ƿ��Ѿ���ȡ������
                            0��δ��ʼ��1���ڽ����У�2��������ݽ��� */
    int type;            /*��ʾһ����������ݳ��ȸ�ʽ���� 1�����ֽڱ�ʶ����
                         2�����ֽڱ�ʶ����
                         3��httpЭ���ʶ�ĳ��� */
    int mode;            /* ��ʾsocket�Ƿ�Ӧ��socket���Ǽ�������ӵ�socket*/
    int msg_len;         /* �������ĵõ��İ��峤�� */
    int data_len;        /*�յ�����ʵ���ݳ��� */
    struct sockaddr_in dst_addr;
    unsigned char buf[1024*16];

} connection;

typedef struct
{
    connection *ptr;
    size_t size;
} connections;

typedef struct
{
    int port;
    int type;
    int timeout;
} srv_socket;

typedef struct
{
    srv_socket *srv_socks;
    int srv_sock_cnt;  /*sock�ѷ���λ��*/
    int used;    /* sock��ʹ��λ��  */
    int shm_rows;    /*�����ڴ����õļ�¼�����*/
    char config_file_name[512];
    char log_file_name[512];
} config;

typedef struct server
{

    config  cfg;
    fdevents ev;
    int dont_daemon;

    int max_workers;
    int max_fds;    /* max possible fds */
    int cur_fds;    /* currently used fds */
    size_t max_conns;
    time_t startup_ts;
    connections conns;
    void *shm_ptr;
    fdevent_handler_t event_handler;

} server;
/* �Ự���ݵ������ndx+key���
   1���»Ự��seqno��Ϊɢ�еĵķ�ʽȷ���ûỰ���λ�á�
   2���ϻỰ��ndx��Ϊ���ҸûỰ����λ�ã����Ը�λ�õĿ��Խ��бȽ��Ժ�׼�Ự�ĺϷ���
*/
typedef struct session
{
    char ip[15+1];
    char name[32+1];
    char key[128+1];
    char login_flag;   // �ڿͻ��˵�¼ʱ���������˲��������������
//  �Կͻ���������������ʹ�ã����׶λ�������������֤�׶γ�ʱʱ��ֻ��10�룬Ҫ��ͻ����������û���������
// ���ȷ��𽻻������ָ���ʹ�õ�¼ָ��
    int  ndx;    /*������*/
    int  used;   /*ʹ�ñ��*/
//  unsigned long seqno;  /*������˳���*/
//  unsigned int rand;   /*���ڶ�˳��Ž��л���*/
//  char     method ;         /*���ӷ���  */
    time_t build_time;
    time_t last_time;
    time_t idle_time;
    char remark[512];
} session;

typedef struct shm_data
{
    int max_rows;
    int sem_lock;
    int last_ndx;
    session ptr[1];
} shm_data;

typedef struct JOB
{
    int sock_id;
    int fd;
    int read_or_write; //������������д����
    time_t last_time;  //�����ʱ��
    long file_size;    //�ļ���С
    long off_set;      //�Ѵ����ļ�����ƫ����
    char file_name[128];
    char file_path[256];
    char file_head[64];
    char cache_buf[90];
    char remark[512];
    int remark_len;
    int (*begin_proc)(int sock_id,int fd,char  *file_name,char  *file_path,char *cache_buf,char *file_cache, char *remark,long *off_set, long *file_size,void *buf,int buf_len);
    int (*every_proc)(int sock_id, char *file_name ,char *file_path, long off_set, char *remark);//ÿһ��ͨ��������ɵ�Ӧ�ô���
    int (*end_proc)(int sock_id, char *file_name ,char *file_path, long off_set,char *remark);//ͨ��ȫ����ɺ��Ӧ�ô���
    int (*error_proc)(int sock_id, char *file_name ,char *file_path, long off_set, char *remark);//�����г����Ӧ�ô���
} JOB;

typedef struct JOB_LIST
{
    int num;
    JOB workers[MAX_JOB];
} JOB_LIST;

#endif

