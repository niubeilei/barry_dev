#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <time.h>
#include <pthread.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include "module.h"
#include "pro_head.h"
#include "sqlite3.h"


#define DM_MAX_LEN 8192
//#define AM_auth_MAX 5


enum {
	CMD_AUTHENTICATE=1,
	CMD_AUTHORIZE=2,
	CMD_SECURECMDAUTH=3,
	CMD_WEBAUTHEN=5
};


static char clientPrefix[5]={0xaf,0xbf,0xcf,0xdf,0xef};
extern int str_segment(char *str,char *item[],char seg,int field);

struct parameter
{
	char	debug;
	int		listen_port;
	char	*module;
	char	*module_args;
	char	*logfile;
	char    *d_base;
};

struct parameter param;
//struct AM_auth *head[AM_auth_MAX];

int authlogfd=0;
int line=0;
pthread_mutex_t mutex;

#define PROTO_NUM 3
struct tcp_pro_data *conn_proto[PROTO_NUM];


//struct daemon for sharing data for threads  

#define DM_KER_SEQ 1024
#define DM_REP_SEQ 4

/***********************************
struct daemon_fd
{
    int flag;
    char *session_id;
    int fd_send[DM_REP_SEQ];
}aos_daemon[DM_KER_SEQ];
**************************************/

struct daemon_fd aos_daemon[DM_KER_SEQ];

char *aos_LogChgTime2Str()
{
	static char buf[64];
	struct tm *local;
	time_t t;
	
	t=time(NULL);
	local=localtime(&t);
	
	strftime( buf, 64, "%F %T", local );
	
	return buf;
}


static int authlog_open(char * filename)
{
	if(!filename)
		return -1;
	
	authlogfd = open( filename, O_WRONLY|O_SYNC|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR );
	if(authlogfd<0)
	{
		printf("Open AM log file:%s error:%s\n", filename, strerror(errno));
		exit(1);
	}
	
	return 0;
}

static int authlog_close( void )
{
	if(authlogfd>=0)
		close( authlogfd );
	return 0;
}

int authlog( const char * fmt, ... )
{
	char buf[1024]; 
	va_list args; 
	
	if(authlogfd<0)
		return -1;
	va_start(args, fmt); 
	vsprintf(buf, fmt, args);  
	va_end(args); 
	
	{
		write( authlogfd, buf, strlen(buf) );
		alog( LOG_ERR, buf );
	}
	return 0;
}


static void init_arg(void)
{
	int i=0;
	long bytes;
	memset( &param, 0, sizeof(param) );
	param.listen_port = 1818;
	char *buf;
	char *item[32];
	FILE *fp;
	fp=fopen("/usr/local/keyou/Bin/daemon.conf","r");
	if(!fp)
	{
		printf("Cannot find configure file\n");
		exit (1);
	}
	fseek(fp,0,SEEK_END);
	bytes=ftell(fp);
	buf=malloc(bytes+1);
	fseek(fp,0,SEEK_SET);
	fread(buf,bytes,1,fp);
	i=str_segment(buf,item,':',32);
	if(i<6)
	{
		printf("Read configure file error,Please check configure file\n");
		exit (1);
	}

	param.d_base=strdup(item[1]);
	param.logfile=strdup(item[3]);
	param.listen_port = atoi(item[5]);

	free (buf);


	for(i=0;i<DM_KER_SEQ;i++)
	{
	    aos_daemon[i].flag=0;
	    aos_daemon[i].session_id=NULL;
	    aos_daemon[i].proto=NULL;
	    memset(aos_daemon[i].fd_send,0,sizeof(int)*DM_REP_SEQ);
	    aos_daemon[i].pro_data=NULL;
	    memset( &aos_daemon[i].pro_thread_data, 0, sizeof(VCHAR) );
	}
       
	conn_proto[0]=&ftp_pro_data;
	conn_proto[1]=&telnet_pro_data;
	conn_proto[2]=&ssh_pro_data;

}

static void usage(void)
{
	puts("Usage: am [OPTION]...");
	puts("  -h\tHelp");
	puts("  -d\tDebug mode");
	puts("  -p\tThe port to listen on, default is 1818");
	puts("  -f\tSpecify log file name");
	exit(0);
}
static void parse_arg(int argc, char **argv)
{
	int ret;
	
	while(1)
	{
		ret = getopt(argc, argv, "hdp:m:b:f:");
		if(ret == -1)
			break;
		switch (ret)
		{
		case 'h':
			usage();
			break;
		case 'd':
			param.debug = 1;
			break;
		case 'p':
			param.listen_port = atoi(optarg);
			break;
		case 'm':
			param.module = optarg;
			break;
		case 'b':
			free(param.d_base);
			param.d_base = optarg;
			break;
		case 'f':
			free(param.logfile);
			param.logfile = optarg;
			break;
		}
	}
}

static void print_arg(void)
{
	printf("listen port: %d\n", param.listen_port );
//	printf("conf file: %s\n", aos_user.auth_data.filename);
	printf("log file: %s\n", param.logfile);
	
	return;
}

static void sig_int(int signo)
{
	exit(0);
}


static void str_remove_rn( char *s)
{
     int slen = strlen(s);
     if(!s||!slen)
	 return;
     int i=0;
     for(i=slen-1;i>=slen-2&&i>=0;i--)
	 if( s[i]=='\r'||s[i]=='\n' )
	 {
	     s[i]='\0';
//	     break;
	 }
}

static int receive_from_kernel(char *command,u32 lenth,u32 ifnfirst,int *position) 
{
	
	char *recv_message = command;
	int count;
	if(!ifnfirst)
	{
	
	    pthread_mutex_lock (&mutex);
	    {
	    char *item[32];
	    int i,f_flag=0;
	    
	    str_remove_rn(recv_message);
	    count=str_segment(recv_message,item,' ',2);
	    for(i=0;i<DM_KER_SEQ;i++)
		if( aos_daemon[i].session_id &&\
		   !strcmp(aos_daemon[i].session_id,item[0]) )
		{
		    f_flag=1;
		    break;
		}
	    if(!f_flag)
	    {
      	
		for(i=0;i<DM_KER_SEQ;i++)
		    if(aos_daemon[i].flag==0)
			break;
		if(i==DM_KER_SEQ)
		    return -1;
		aos_daemon[i].flag=1;
		aos_daemon[i].session_id=strdup(item[0]);
		aos_daemon[i].proto=strdup(item[1]);

		int j=0;
		for(j=0;j<PROTO_NUM;j++)
		    if( strncmp( aos_daemon[i].proto, conn_proto[j]->name, strlen( conn_proto[j]->name ) )==0)
		    {
			aos_daemon[i].pro_data=conn_proto[j];
			break;
		    }
		if( j == PROTO_NUM )
		{
		    return -1;
		}
	    }
	    *position=i;
	    }
	    pthread_mutex_unlock(&mutex);
	}
	else
	{
	    int k=0;
	    struct daemon_fd *mon = &aos_daemon[*position];

	    if ( lenth > 5 && \
		 !memcmp(command,clientPrefix,5) )
	    {
	    	u32 realLen = lenth-5;
	    	recv_message = &command[5];
		if( mon->pro_data->flags&0x01 )
		{
		    for(k=0;k<DM_REP_SEQ;k++)
			if( mon->fd_send[k] )
			    write( mon->fd_send[k], recv_message, realLen );
		}
		//printf( "%s\n", (char *)recv_message );
		mon->pro_data->client_to_server( (u8 *)recv_message, &realLen, mon);
	    }
	    else
	    {
		if( mon->pro_data->flags&0x02 )
		{
		    for(k=0;k<DM_REP_SEQ;k++)
			if( mon->fd_send[k] )
			    write( mon->fd_send[k], recv_message, lenth );
		}
		mon->pro_data->server_to_client( command, &lenth, mon );
	    }
	}	    
	    
	//printf("This is Daemon,Thread %u:%s\n",(unsigned int)pthread_self(),command);
	return 0;
	
}


/*
static int send_to_replay(char *command) 
{
    return 0;
}
*/

static void write_dblog(struct daemon_fd *aos_daemon)
{
    time_t end_time;
    end_time = time(NULL);
    
    sqlite3 *db=NULL;
    char *zErrMsg = NULL;
    int rc; 
    rc = sqlite3_open("/usr/local/keyou/Log/hacref.db", &db); 
    if( rc )
    {
	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	//sqlite3_close(db);
	exit(1);
    }

    else 
	printf("You have opened a sqlite3 database named hacref.db successfully!\n");
    
    char sql[512];//char *sql=malloc(512);
    
    snprintf(sql,512,"update session set endtime=%u where id='%s';",(unsigned int)end_time,aos_daemon->session_id) ;
    sqlite3_exec( db , sql , 0 , 0 , &zErrMsg );
    
    
    sqlite3_close(db); 
}

static void* daemon_child( void * private )
{
	fd_set          rset;
	fd_set          eset;
	int		ret;
	
	int connfd = (int)private;
	int position=-1;
	
	alog(LOG_ERR, "child thread pid:%d, tid:%d\n", getpid(), (int)pthread_self() );
	u32 ifnfirst=0;
	while(1)
	{
		FD_ZERO( &rset );
		FD_ZERO( &eset );
		FD_SET( connfd, &rset );
		FD_SET( connfd, &eset );
		ret = select( 1+connfd, &rset, 0, &eset, 0 );
		if(ret<0)
		{
			alog(LOG_ERR, "select error: %s",strerror(errno));
			goto out;
		}
		else if(FD_ISSET(connfd, &rset))
		{
			char buf[DM_MAX_LEN+1];
			u32 len;
			
			len = read( connfd, buf, DM_MAX_LEN );

			if(len<=0)goto out;
			buf[len] = 0;
			    
			if( !ifnfirst )
				printf("==================%s===================\n", buf);
			if( receive_from_kernel( buf,len,ifnfirst,&position) < 0 )
			    goto out;
	    		ifnfirst++;
		}
		else
		{
			alog(LOG_ERR, "select returns: %s",strerror(errno));
			goto out;
		}
		
	}
	
out:
	alog(LOG_ERR, "exit child thread pid:%d, tid:%d\n", getpid(), (int)pthread_self() );
	//printf("Haha\n");
	if( position >= 0 )
	{
	    write_dblog( &aos_daemon[position] );
	    aos_daemon[position].pro_data->pro_destory( &aos_daemon[position] );
	    aos_daemon[position].flag=0;
	    memset(aos_daemon[position].fd_send,0,sizeof(int)*DM_REP_SEQ);
	    free(aos_daemon[position].session_id);
	    aos_daemon[position].session_id=0;
	    free(aos_daemon[position].proto);
	    aos_daemon[position].pro_data=NULL;
	}
	close( connfd );
	return 0;
}

static void* daemon_child_r( void * private )
{
	fd_set          rset;
	fd_set          eset;
	int		ret;
	
	int connfd = (int)private;
	int value=0;
	int position=-1,numb=-1;
	printf("fd,thread:%d,,,%u\n",connfd,(int)pthread_self());
	
	alog(LOG_ERR, "child thread pid:%d, tid:%d\n", getpid(), (int)pthread_self() );
	
	while(1)
	{
		FD_ZERO( &rset );
		FD_ZERO( &eset );
		FD_SET( connfd, &rset );
		FD_SET( connfd, &eset );
		ret = select( 1+connfd, &rset, 0, &eset, 0 );
		if(ret<0)
		{
			alog(LOG_ERR, "select error: %s",strerror(errno));
			goto out;
		}
		else if(FD_ISSET(connfd, &rset))
		{
			char buf[DM_MAX_LEN+1];
			int len;
			
			len = read( connfd, buf, DM_MAX_LEN );
			if(len<=0)
			    goto out;
			if(!value)
			{
			str_remove_rn(buf);
			char *session;
			session=buf;
			for(len=0;len<DM_KER_SEQ;len++)
			    if(aos_daemon[len].session_id!=NULL&&strcmp(aos_daemon[len].session_id,session)==0)
				break;
			if(len>=DM_KER_SEQ)
			{
			    write( connfd, "No this session_id alive \n",26 );
			    goto out;
			}
			int j;
			for(j=0;j<DM_REP_SEQ;j++)
			    if(aos_daemon[len].fd_send[j]==0)
				break;
			if(j>=DM_REP_SEQ)
			{
			    write( connfd, "Reach the maximum connect number.\nPlease try again later !\n",59 );
			    goto out;
			}
			position=len;
			numb=j;
			aos_daemon[position].fd_send[numb]=connfd;
			{
			    char *shell_cmd=malloc(256);
			    if( aos_daemon[position].pro_data->flags&0x02 )
			    {
				printf("Write cs sc !\n");
				sprintf(shell_cmd,"/usr/local/keyou/Bin/mysudo /usr/local/keyou/Bin/Cli.exe -cmd \\\"app proxy userland recv %s 1 on\\\"",aos_daemon[position].session_id);
				system(shell_cmd);
			    }
			    free(shell_cmd);
			    
			}
			value=1;
			}
		}
		else
		{
			alog(LOG_ERR, "select returns: %s",strerror(errno));
			goto out;
		}
		
	}
	
out:
	alog(LOG_ERR, "exit child thread pid:%d, tid:%d\n", getpid(), (int)pthread_self() );
	if(value)
	    aos_daemon[position].fd_send[numb]=0;
	close( connfd );
	return 0;
}


static void tcp_listen(int listenfd[2])
{       
	int	  ret;
	const int on = 1;
	struct    sockaddr_in addr;
	struct    sockaddr_in addr_r;
	
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(1818);
	
	listenfd[0] = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd[0] < 0)
	{
		printf("socket() error\n");
		exit(1);
	}
	
	setsockopt(listenfd[0], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if ( bind(listenfd[0], (struct sockaddr*)&addr, sizeof(addr)) )
	{
		printf("addr_kernel bind() error\n");
		exit(1);
	}
	
	ret = listen(listenfd[0], 32);
	if(ret<0)
	{
		printf("listen error\n");
		exit(1);
	}
	
	//port 1820
	memset(&addr,0,sizeof(addr_r));
	addr_r.sin_family=AF_INET;
	addr_r.sin_addr.s_addr=htonl(INADDR_ANY);
	addr_r.sin_port=htons(1820);
	
	listenfd[1] = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd[1] < 0)
	{
		printf("socket() error\n");
		exit(1);
	}
	
	setsockopt(listenfd[1], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if ( bind(listenfd[1], (struct sockaddr*)&addr_r, sizeof(addr_r)) )
	{
		printf("addr_replay bind() error\n");
		exit(1);
	}
	
	ret = listen(listenfd[1], 32);
	if(ret<0)
	{
		printf("listen error\n");
		exit(1);
	}
	
//	return listenfd;
}

static int netmodel_at( int listenfd[2] )
{
	
	while(1)
	{
		int             connfd[2];
		pthread_t       tid;
		int             clilen;
		int	ret;
		struct sockaddr cliaddr;
		pthread_attr_t attr;
		fd_set          rset;
		fd_set          eset;
		
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
		
		clilen = sizeof(cliaddr);
		
		
		FD_ZERO( &rset );
		FD_ZERO( &eset );
		FD_SET( listenfd[0], &rset );
		FD_SET( listenfd[1], &rset );
		FD_SET( listenfd[0], &eset );
		FD_SET( listenfd[1], &eset );
		ret = select( 1+listenfd[1], &rset, 0, &eset, 0 );
		if(ret<0)
		{
			alog(LOG_ERR, "select error: %s",strerror(errno));
			return 0;
		}
		else if(FD_ISSET(listenfd[0], &rset))
		{
			connfd[0] = accept(listenfd[0], &cliaddr, &clilen);
			if(connfd[0]<0)
			{
				printf("accept error\n");
				continue;
			}

			pthread_mutex_init (&mutex,NULL);
			ret = pthread_create(&tid, &attr, &daemon_child, (void *) connfd[0]);
			if(ret)
			{
				printf("pthread_create error: %s\n",strerror(errno));
				close(connfd[0]);
			}
		}
		else if(FD_ISSET(listenfd[1], &rset))
		{
			connfd[1] = accept(listenfd[1], &cliaddr, &clilen);
			if(connfd[1]<0)
			{
				printf("accept error\n");
				continue;
			}
		
			ret = pthread_create(&tid, &attr, &daemon_child_r, (void *) connfd[1]);
			if(ret)
			{
				printf("pthread_create error: %s\n",strerror(errno));
				close(connfd[1]);
			}
		}
		else
		{
			alog(LOG_ERR, "select returns: %s",strerror(errno));
			return 0;
		}

		
		
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	int listenfd[2];
	
	init_arg();
	parse_arg(argc, argv);
	print_arg();
	authlog_open( param.logfile );
	
	signal(SIGINT, sig_int);
	
	tcp_listen(listenfd);
	
	if(!param.debug)
	{
		daemon( 0, 0 );
	}
	
	netmodel_at( listenfd );
	authlog_close();
	close(listenfd[0]);
	close(listenfd[1]);
	return 0;
}
