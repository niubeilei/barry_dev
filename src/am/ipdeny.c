#include "module.h"
#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//static int line=0;

extern void SHA_Simple(void *p, int len, unsigned char *output);
extern void covert_hash_to_string( unsigned char *hash, char *hstr);
extern int str_segment(char *str,char *item[],char seg);
extern struct parameter
{
	char	debug;
	int		listen_port;
	char	*module;
	char	*module_args;
	char	*logfile;
	char    *d_base;
}param;

/************************************************************
static int get_fileline(char *p[],FILE *fp)
{
	int i=0;
	char buf[1024];
	while(1)
	{
//		p[0]=malloc(1024);
		if(fgets(buf,1024,fp)==NULL)
			break;
		if((p[i++]=strdup(buf))==NULL)
		{
			printf("memory alloc error\n");
			alog(LOG_ERR,"Memory alloc error\n");
			i-=2;
			while(i>=0)
				free(p[i--]);
	        exit (1);
		}		
	}

	return i;
}
*****************************************************************/

extern int field_analy(char **p,char *field,int n);

static int do_authorize( struct AM_data *auth_data,struct AM_input *input)
{
//	char *buf;
	char *item[32];
	char user[128];
//	char password[128];
	char hash[20];
	int ret,i=0,j=0;
 	u32 ipaddr;	
	alog(LOG_ERR, "file auth password ");

	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	int rc;
//	rc = sqlite3_open(auth_data->filename, &db);
	rc = sqlite3_open(param.d_base, &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}
	else printf("You have opened a sqlite3 database named hac.db successfully!\n\
			Congratulations! Have fun ! ^-^ \n");
	
	if(strlen(input->username)>=20)
		j=1;
//	strcpy(password,input->password);
	
	int nrow = 0, ncolumn = 0;
	char **azResult;
	char *sql = malloc(256);
	sprintf(sql,"SELECT * FROM ipdeny where appname='*' or appname='%s'",input->password);
	sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg);
    free(sql);
	sqlite3_close(db);
	if(!nrow)
	{
		sqlite3_free_table( azResult );
		return 1;
	}
	
	int field[8];
	field[0]=field_analy(azResult,"ip",ncolumn);
	field[1]=field_analy(azResult,"username",ncolumn);
	field[2]=field_analy(azResult,"appname",ncolumn);
	for(i=1;i<=nrow;i++)
	{
		strcpy(user,azResult[ncolumn*i+field[1]]);
		if(j&&strcmp(user,"*")!=0)
		{
			SHA_Simple(user,strlen(user),(unsigned char *)hash);
			covert_hash_to_string( (unsigned char *)hash, user );
		}
		ret = str_segment(azResult[ncolumn*i+field[0]],item,'/');
		if(ret==1)
		    ipaddr=input->client_ip;
		else if(ret==2)
		    ipaddr=input->client_ip&inet_addr(item[1]);
		else
		{	
		    alog(LOG_ERR,"Invalid dada in database.ipdeny,line%d:%s|%s|%s\n",i,azResult[ncolumn*i+field[0]],azResult[ncolumn*i+field[1]],azResult[ncolumn*i+field[2]]);
			continue;
		}
		if((strcmp(user,input->username)==0||strcmp(user,"*")==0)&&ipaddr==inet_addr(item[0]))
		{
			sqlite3_free_table( azResult );
			return 0;
		}
	}
		
	sqlite3_free_table( azResult );
	return 1;
}


static int auth_init( struct AM_data *auth_data)
{

//	FILE *fp;
//	long filesize;
//	fp=fopen(auth_data->filename,"r");
//	if(!fp)
//	{
//	   	alog(LOG_ERR, "open file %s error\n", auth_data->filename);
//		exit (1);
//	}   
//   line=get_fileline(auth_data->data,fp);
//	fclose(fp);
	return 0;
}

static int auth_close( struct AM_data *auth_data)
{
//	int i;
//	for(i=0;i<line;i++)
//	{
//		free(auth_data->data[i]);
//	}	
    return 0;
}

struct AM_auth aos_ip = 
{
//	.auth_data.filename="ipdeny.conf",
	.name="ip",
	.init = auth_init,
//	.authenticate = do_authenticate,
	.authorize = do_authorize,
	.close =auth_close,
};

