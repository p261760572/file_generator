#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "db_hander.h"
#include "ibdcs.h"
#include "file_generate.h"

typedef struct
{
    int     signo;
    void (*handler)(int signo);
} signal_t;


void signal_handler(int signo);

signal_t signals[] =
{
    {SIGSEGV, signal_handler},
    {SIGTERM, signal_handler},
    {SIGCHLD, signal_handler},
    {SIGUSR1, signal_handler},
    {0, NULL}
};

//sig_atomic_t cod_restart = 0;

sig_atomic_t sys_exit = 0;

int reload_flag = 0;
char *document_root = NULL;


void signal_handler(int signo)
{
    dcs_log(0,0,"at %s(%s:%d) signal %d",__FUNCTION__,__FILE__,__LINE__,signo);

    switch(signo)
    {
        case SIGSEGV:
        {
            exit(-1);
            break;
        }
        case SIGCHLD:
        {
            //忽略
            break;
        }
        case SIGUSR1:
        {
        		sys_exit = 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

int init_signals()
{
    signal_t      *sig;
    struct sigaction   sa;

    for(sig = signals; sig->signo != 0; sig++)
    {
        bzero(&sa, sizeof(sa));
        sa.sa_handler = sig->handler;
        sigemptyset(&sa.sa_mask);
        if(sigaction(sig->signo, &sa, NULL) == -1)
        {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    char log_path[128];

    snprintf(log_path, sizeof(log_path), "%s.log", argv[0]);

    if(dcs_log_open(log_path, argv[0]) != 0)
    {
        return -1;
    }

    if(init_signals() != 0)
    {
        dcs_log(0,0,"at %s(%s:%d) init_signals设置信号函数出错",__FUNCTION__,__FILE__,__LINE__);
        return -1;
    }

    document_root = getenv("DOCUMENT_ROOT");
    if(document_root == NULL)
    {
        dcs_log(0,0,"at %s(%s:%d) 没有配置DOCUMENT_ROOT环境变量",__FUNCTION__,__FILE__,__LINE__);
        return -1;
    }

    while(connect_init() < 0)
    {
        dcs_log(0,0,"at %s(%s:%d) connect_init连接数据库失败",__FUNCTION__,__FILE__,__LINE__);
        sleep(100);
    }
		while(load_config() < 0) sleep(10);
		dcs_log(0,0,"at %s(%s:%d) startup success",__FUNCTION__,__FILE__,__LINE__);

    while(sys_exit == 0)
    {
				while(process()) {
					 if(sys_exit == 1 && db_reload(sys_exit) > 0)
					 		 sys_exit = 0;
		       sleep(60);
        }
        sys_exit = 2;
    }

    dcs_log(0,0,"at %s(%s:%d) exit",__FUNCTION__,__FILE__,__LINE__);

		if(sys_exit == 2)
		{
			//30分钟后自动重启
			dcs_log(0,0,"at %s(%s:%d) waiting restart",__FUNCTION__,__FILE__,__LINE__, strerror(errno));	
			sleep(1800);
			if(execv(argv[0], argv) == -1)
			{
				dcs_log(0,0,"at %s(%s:%d) restart fail",__FUNCTION__,__FILE__,__LINE__, strerror(errno));	
			}
		}

    return 0;
}

