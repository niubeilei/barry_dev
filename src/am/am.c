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
#include "sqlite3.h"

#define AM_MAX_LEN 1024
#define AM_MIN_LEN 3
#define AM_auth_MAX 5

// secure cmd auth
//#include "Alarm/Alarm.h"
//#include "keyman/keyman.h"
#include <openssl/md5.h>

enum {
	CMD_AUTHENTICATE=1,
	CMD_AUTHORIZE=2,
	CMD_SECURECMDAUTH=3,
	CMD_WEBAUTHEN=5
};

// Kevin, 12/06/2006
int aos_Generate_MD5(const char* path, char* md5)
{
	/////////////// Function Description /////////////////
	// this function generates the MD5 code of a file.
	/////////////////////////////////////////////////////

	// 1. set system key
	// if none
	char* SystemKey = "12345678";
	//char* SystemKey = new char[33];
	//if(aos_keymagmt_get_key("SystemKey",SystemKey,33))
	//	return -2;   // fail to get system key

	// 2. open file
	//cout<<"1"<<endl;
	struct stat buff_stat;
	//cout<<"2"<<endl;
	//cout<<"path is :"<<path<<endl;
	if(stat(path,&buff_stat) < 0)
	{
		return -1;   // fail to get file
	}
	//cout<<"get stat"<<endl;
	if(!S_ISREG(buff_stat.st_mode)) 
	{
		return -1;   // path is not a file name
	}

	FILE* file = 0;
	file = fopen(path, "r");
	if(file == NULL) return -1; // error! fail to open the file

	// 3. call md5 function
	MD5_CTX c_md5;
	unsigned char md_md5[MD5_DIGEST_LENGTH];

	MD5_Init(&c_md5);
	while (!feof(file))
	{
#define bsize 1024
		char buff[bsize];
		unsigned long len;
		len = fread(buff, 1, bsize, file);		
		MD5_Update(&c_md5, buff, len);
	}

	MD5_Update(&c_md5, SystemKey, strlen(SystemKey));
	MD5_Final(md_md5, &c_md5);

	// 4. result conversion
	int i = 0;	
	for(i=0; i<MD5_DIGEST_LENGTH; i++)

		sprintf(md5+2*i,"%02x", md_md5[i]);
	md5[32]=0;	


	//cout<<"md5 code is : "<<md5<<endl;
	return 0;
}

// Kevin, 12/06/2006
int aos_comman_Authenticate(const char* path)
{
	///////////   Function Description     //////////////////////
	// this function generates the MAC code of a file, then compares it with the 
	// Mac list in the database, and return a command by the result of the 
	// comparation.
	//

	// 1. open DB and compare with the list
	char path_DB[1024];
	strcpy(path_DB,path);
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char sql[1024];
	int nrow = 0, ncolumn = 0;
	char **azResult; 
	int rc;

	rc = sqlite3_open("/usr/local/rhc/aos_secure/securecmd.db", &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}

	sprintf(sql,"select * from cmdtable where filename =\"%s\";",path);
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg);

	if( nrow ) /* if path is in the DB list */ 
	{
		sprintf(sql,"select \"mac\" from cmdtable where filename =\"%s\";",path_DB);
		sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg );

		char md5_DB[33];
		strcpy(md5_DB, azResult[1]);
		char md5[33];

		sqlite3_free_table(azResult);
		sqlite3_close(db);

		// 2. MAC comparation
		//{

		if(aos_Generate_MD5(path,md5))
		{
			return -2; // fail to generate MD5
		}
		if(!strcmp(md5,md5_DB))
		{
			return 0;			// md5 matches!
		}
		else return -1;  // md5 doesn't match!
	}

	sprintf(sql,"select \"policy\" from policy;");
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg);

	char policy[10];
	strcpy(policy, azResult[1]);

	sqlite3_free_table(azResult);
	sqlite3_close(db);

	if(!strcmp(policy, "accept"))
	{
		return 0;
	}

	return -1;

	}

	extern int str_segment(char *str,char *item[],char seg);

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
	struct AM_auth *head[AM_auth_MAX];

	int authlogfd=0;
	int line=0;
	pthread_mutex_t mutex;

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
		//	param.listen_port = 1812;
		char *buf;
		char *item[32];
		FILE *fp;
		fp=fopen("am.conf","r");
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
		i=str_segment(buf,item,':');
		if(i<6)
		{
			printf("Read configure file error,Please check configure file\n");
			exit (1);
		}
		//	strcpy( aos_user.auth_data.filename, item[1] );
		//	strcpy( aos_ip.auth_data.filename, item[3] );
		//	strcpy( aos_author.auth_data.filename, item[5] );

		param.d_base=strdup(item[1]);
		param.logfile=strdup(item[3]);
		param.listen_port = atoi(item[5]);
		//	strcpy( aos_user.auth_data.filename, item[1] );
		//	strcpy( aos_ip.auth_data.filename, item[1] );
		//	strcpy( aos_author.auth_data.filename, item[1] );
		//	strcpy( aos_dbauthor.auth_data.filename, item[1] );

		free (buf);

	}

	static void usage(void)
	{
		puts("Usage: am [OPTION]...");
		puts("  -h\tHelp");
		puts("  -d\tDebug mode");
		puts("  -p\tThe port to listen on, default is 1812");
		//puts("  -m\tThe module to use, for instance: file, mysql, ...");
		//	puts("  -a\tSpecify userlist file name,default is user.conf");
		//	puts("  -i\tSpecify  ip_deny_list file name,default is ipdeny.conf");
		//	puts("  -z\tSpecify authorization file name,default is author.conf");
		puts("  -f\tSpecify log file name");
		puts("  -b\tSpecify database");
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
					/**********************************************************************
					  case 'a':
					  param.module_args = optarg;
					//			strcpy( aos_user.auth_data.filename, optarg );
					break;
					case 'i':
					param.module_args = optarg;
					//			strcpy( aos_ip.auth_data.filename, optarg );
					break;
					case 'z':
					param.module_args = optarg;
					strcpy( aos_author.auth_data.filename, optarg );
					break;
					 ***********************************************************************/

				case 'b':
					free(param.d_base);
					param.d_base = optarg;
					//			strcpy( aos_user.auth_data.filename, param.d_base );
					//			strcpy( aos_ip.auth_data.filename, param.d_base );
					//			strcpy( aos_author.auth_data.filename, param.d_base );
					//			strcpy( aos_dbauthor.auth_data.filename, param.d_base );
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
		printf("conf file: %s\n", aos_user.auth_data.filename);
		printf("log file: %s\n", param.logfile);

		return;
	}

	static void sig_int(int signo)
	{
		exit(0);
	}

	static void str_remove_rn( char * s)
	{
		if(!s)
			return ;
		while(*s)
		{
			if (*s=='\r'||*s=='\n')
			{
				*s=0;
				return ;
			}
			s++;
		}
	}

	void make_logpath(char *date_path)
	{
		char exec[256];
		time_t cur_time;
		struct tm *local;
		int year,month,day;

		cur_time=time(NULL)+5;
		local=localtime(&cur_time);
		year=local->tm_year+1900;
		month=local->tm_mon+1;
		day=local->tm_mday;
		sprintf(date_path,"%04d/%02d%02d", year,month,day);
		snprintf(exec,256,"mkdir -p /usr/local/keyou/Log/%s", date_path);
		system(exec);
	}

	static void write_dblog(struct AM_input *aos_input,int id)
	{
		time_t start;
		start = time(NULL);
		struct in_addr ipaddr;
		ipaddr.s_addr = aos_input->client_ip;

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
		snprintf(sql,512,"CREATE TABLE session(id primary key,appneme,username,starttime INTEGER,endtime INTEGER,clientIP,filepath);");
		sqlite3_exec( db , sql , 0 , 0 , &zErrMsg );

		char tmp_path[256];
		// make_logpath(tmp_path);
		snprintf(sql,512,"INSERT INTO session VALUES('%s','%s','%s',%d,%d,'%s','%s');",\
				aos_input->session_id,aos_input->password,aos_input->username,\
				(unsigned int)start,(unsigned int)id,inet_ntoa(ipaddr), tmp_path) ;
		sqlite3_exec( db , sql , 0 , 0 , &zErrMsg );

		sqlite3_close(db); 
	}

	static int do_auth(char *command) 
	{
		int cmd,num,au_result;
		//	char ipstr[16];
		struct AM_input aos_input;
		struct utsname name;
		char *item[32];
		char *dup_user;
		char *conn_t;

		printf("AM,do_auth->command:%s\n",command);
		//char *web_session;

		//	num=sscanf(command,"%d %s %s %s %s", &cmd,aos_input.username,aos_input.password,ipstr,aos_input.session_id );

		num=str_segment(command,item,' ');
		printf("AM,do_auth->command:%s:%d\n",command,num);

		// Kevin, 12/06/2006
		if (num == 3&&atoi(item[0])==3)
		{
			goto other;
		}

		if( (num != 5 && atoi(item[0])!=5) ||(atoi(item[0])==5 && num!=3 ) )
		{
			for(cmd=0;cmd<num;cmd++)
				alog(LOG_ERR,"%s ",item[cmd]);
			alog(LOG_ERR,"\n");
			goto fail;
		}
		if(num==5)
		{
			aos_input.username=item[1];
			aos_input.password=item[2];
			aos_input.client_ip=inet_addr(item[3]);
			aos_input.session_id=item[4];
			if(strlen(aos_input.username)>=20)
				dup_user = strdup(aos_input.username);
		}
		if(num==3)

		{ 
			aos_input.au_id=(unsigned long)item[1];
			aos_input.session_id=item[2];
		}

		if (uname(&name) == -1)
			printf("Error: Cannot get uname!\n");


other:

		cmd=atoi(item[0]);
		switch(cmd)
		{
			case CMD_AUTHENTICATE:

				for(num=0;num<1;num++)
				{
					if ( !head[num]->authenticate( &head[num]->auth_data, &aos_input))
					{
						//write_dblog(&aos_input,1);
						goto auth_fail;
					}
				}
				break;

			case CMD_AUTHORIZE:
				{
					//		for(num=1;num<=2;num++)
					//		{
					if (!( au_result=head[1]->authorize( &head[1]->auth_data, &aos_input)))
					{
						//write_dblog(&aos_input,2);
						goto auth_fail;
					}
					if ((au_result=head[2]->authorize( &head[2]->auth_data, &aos_input))<=2)
					{
						//write_dblog(&aos_input,3);
						goto auth_fail;
					}
					//		} 
					break;
				}
			case CMD_WEBAUTHEN:

				if ( !head[4]->authenticate( &head[4]->auth_data, &aos_input))
				{
					//write_dblog(&aos_input,5);
					goto auth_fail;
				}
				break;

				// Kevin, 12/08/2006
			case CMD_SECURECMDAUTH:
				{
					char * uid;
					char * path;
					uid = strdup(item[1]);
					path = strdup(item[2]);
					//printf("The 2 para is : %s\n", uid);
					//printf("The 3 para is : %s\n", path);
					int ret = aos_comman_Authenticate(path);
					if (ret == 0)
					{
						sprintf(command, "Yes");
						printf("The rst is : %s\n", command);
						authlog("%s '%d %s %s' YES\r\n", aos_LogChgTime2Str(), cmd, uid, command);
						return strlen( command );
					}
					else
					{
						sprintf(command, "no");
						printf("The rst is : %s\n", command);
						authlog("%s '%d %s %s' NO\r\n", aos_LogChgTime2Str(), cmd, uid, command);
						return strlen( command );
					}
					//
					//   added by Long 12/07/2006 , 
					//   but I don't know which parameter the caller should give.
					//
					break;
				};
			default:
				printf("Invalid access identifier:");
				for(num=0;num<5;num++)
				{
					printf("%s ",item[num]);
					alog(LOG_ERR,"%s ",item[num]);
				}
				printf("\n");
				alog(LOG_ERR,"\n");
				goto fail;
				break;				
		}

		if(cmd==5)
		{
			pthread_mutex_lock (&mutex);
			printf("HAHA am IS HERE\n");
			authlog("%s %s %s '%d %s' YES\r\n", aos_LogChgTime2Str(), name.nodename,\
					aos_input.session_id, cmd,\
					aos_input.username);
			pthread_mutex_unlock(&mutex);
			//		write_dblog(&aos_input,0);
			printf("Web Auth Success ! If other problem,Please go to kernel to check\n");
			snprintf( command, AM_MAX_LEN, "e:%s",aos_input.username );
			free(aos_input.username);
			return strlen( command );
		}
		if(cmd==1)
		{
			pthread_mutex_lock (&mutex);
			authlog("%s %s %s '%d %s' YES\r\n", aos_LogChgTime2Str(), name.nodename,\
					aos_input.session_id, cmd,\
					aos_input.username);
			pthread_mutex_unlock(&mutex);
			snprintf( command, AM_MAX_LEN, "a:Authenticate Success\r\n" );
			return strlen( command );
		}

		if(cmd==2)//author success and need db_author
		{
			pthread_mutex_lock (&mutex);
			authlog("%s %s %s '2 %s %s' YES\r\n", aos_LogChgTime2Str(), name.nodename,\
					aos_input.session_id,\
					aos_input.username,	item[3]);
			authlog("%s %s %s '3 %s %s' YES\r\n", aos_LogChgTime2Str(), name.nodename,\
					aos_input.session_id,\
					aos_input.username,	aos_input.password);
			pthread_mutex_unlock(&mutex);
			conn_t=strdup(aos_input.au_result);
			free(aos_input.au_result);	
			if(au_result==4)
			{

				if(strcmp(aos_input.username,"*")==0)
				{
					sprintf(aos_input.username,"%s",dup_user);
					free(dup_user);
				}

				au_result=head[3]->authorize( &head[3]->auth_data, &aos_input);

				if(au_result)
				{
					pthread_mutex_lock (&mutex);
					authlog("%s %s %s '4 %s %s %s' YES\r\n", aos_LogChgTime2Str(), name.nodename,\
							aos_input.session_id,\
							aos_input.username,	aos_input.password ,aos_input.au_result);
					pthread_mutex_unlock(&mutex);
					free(aos_input.au_result);
					//free(aos_input.au_id);
					//write_dblog(&aos_input,0);
					snprintf( command, AM_MAX_LEN, "c:%s\r\n",conn_t );
					free(conn_t);
					return strlen( command );
				}

				else
				{
					pthread_mutex_lock (&mutex);
					authlog("%s %s %s '4 %s %s %s' NO\r\n", aos_LogChgTime2Str(), name.nodename,\
							aos_input.session_id,\
							aos_input.username,aos_input.password,aos_input.au_result);
					free(aos_input.au_result);
					//free(aos_input.au_id);
					pthread_mutex_unlock(&mutex);
					//write_dblog(&aos_input,4);
					snprintf( command, AM_MAX_LEN, "4:Double_author don't allow this access\r\n" );
					return strlen( command );
				}

			}
			//write_dblog(&aos_input,0);
			snprintf( command, AM_MAX_LEN, "b:%s\r\n",conn_t );//author success and not need db_author//
			free(conn_t);
			return strlen( command );
		}

auth_fail:
		if(cmd==5)
		{
			pthread_mutex_lock (&mutex);
			printf("WEB fail  This is am\n");
			printf("WEB fail  This is am  :%s\n",aos_input.username);
			authlog("%s %s %s '5 %s' NO\r\n", aos_LogChgTime2Str(), name.nodename,\
					aos_input.session_id,\
					aos_input.username);
			pthread_mutex_unlock(&mutex);
			snprintf( command, AM_MAX_LEN, "9:No such web_auth_id\r\n" );
			free(aos_input.username);
			return strlen( command );
		}
		if(cmd==1)
		{
			pthread_mutex_lock (&mutex);
			authlog("%s %s %s '1 %s' NO\r\n", aos_LogChgTime2Str(), name.nodename,\
					aos_input.session_id,\
					aos_input.username);
			pthread_mutex_unlock(&mutex);
			snprintf( command, AM_MAX_LEN, "0:Invalid username or password\r\n" );
			return strlen( command );
		}
		if(cmd==2)
		{
			if(!au_result)
			{
				pthread_mutex_lock (&mutex);
				authlog("%s %s %s '2 %s %s' NO\r\n", aos_LogChgTime2Str(), name.nodename,\
						aos_input.session_id,\
						aos_input.username,	item[3]);
				pthread_mutex_unlock(&mutex);
				snprintf( command, AM_MAX_LEN, "2:This IP address is not permitted for this access\r\n" );
				return strlen( command );
			}
			else
			{
				pthread_mutex_lock (&mutex);
				authlog("%s %s %s '2 %s %s' YES\r\n", aos_LogChgTime2Str(), name.nodename,\
						aos_input.session_id,\
						aos_input.username,	item[3]);
				authlog("%s %s %s '3 %s %s' NO\r\n", aos_LogChgTime2Str(), name.nodename,\
						aos_input.session_id,\
						aos_input.username,	aos_input.password);
				pthread_mutex_unlock(&mutex);
				snprintf( command, AM_MAX_LEN, "3:Authorize don't allow this access\r\n" );
				return strlen( command );
			}
		}
fail:
		snprintf( command, AM_MAX_LEN, "1:Information from client is invalid\r\n" );
		return strlen( command );

	}

	static void* am_child( void * private )
	{
		fd_set          rset;
		fd_set          eset;
		int		ret;

		int connfd = (int)private;

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
				char buf[AM_MAX_LEN+1];
				int len;

				len = read( connfd, buf, AM_MAX_LEN );
				if( len < AM_MIN_LEN )
				{
					goto out;
				}
				buf[len] = 0;
				/*
				//for debug windows telnet
				int temp;
				for(temp=0;temp<len;temp++)
				printf("Am_Child  %d :%x\n",temp,buf[temp]);
				//OK
				*/
				str_remove_rn(buf);

				len = do_auth( buf );
				write( connfd, buf, len );
			}
			else
			{
				alog(LOG_ERR, "select returns: %s",strerror(errno));
				goto out;
			}

		}

out:
		alog(LOG_ERR, "exit child thread pid:%d, tid:%d\n", getpid(), (int)pthread_self() );
		close( connfd );
		return 0;
	}


	static int tcp_listen(int port)
	{
		int	ret;
		int listenfd;
		const int		on = 1;
		struct sockaddr_in addr;

		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
		addr.sin_port=htons(port);

		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd < 0)
		{
			printf("socket() error\n");
			exit(1);
		}

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if ( bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) )
		{
			printf("bind() error\n");
			exit(1);
		}

		ret = listen(listenfd, 32);
		if(ret<0)
		{
			printf("listen error\n");
			exit(1);
		}

		return listenfd;
	}

	static int netmodel_at( int listenfd )
	{

		while(1)
		{
			int             connfd;
			pthread_t       tid;
			int             clilen;
			int	ret;
			struct sockaddr cliaddr;
			pthread_attr_t attr;

			pthread_attr_init( &attr );
			pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, &cliaddr, &clilen);
			if(connfd<0)
			{
				printf("accept error\n");
				continue;
			}

			pthread_mutex_init (&mutex,NULL);
			ret = pthread_create(&tid, &attr, &am_child, (void *) connfd);
			if(ret)
			{
				printf("pthread_create error: %s\n",strerror(errno));
				close(connfd);
			}
		}

		return 0;
	}

	static int load_storage_module(void)
	{

		int i;
		//	if( param.module == 0 )
		//	{
		//		usage();
		//	}

		head[0] = &aos_user;
		head[1] = &aos_ip;
		head[2] = &aos_author;
		head[3] = &aos_dbauthor;
		head[4] = &aos_webauth;
		for(i=0;i<AM_auth_MAX;i++)
			head[i]->init(&head[i]->auth_data);
		return 0;
	}

	int main(int argc, char **argv)
	{
		int listenfd,i;

		init_arg();
		parse_arg(argc, argv);
		print_arg();
		load_storage_module();

		authlog_open( param.logfile );

		signal(SIGINT, sig_int);

		listenfd = tcp_listen(param.listen_port);

		if(!param.debug)
		{
			daemon( 0, 0 );
		}

		netmodel_at( listenfd );
		authlog_close();
		close(listenfd);
		for(i=0;i<AM_auth_MAX;i++)
			head[i]->close(&head[i]->auth_data);
		return 0;
	}
