#include "module.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>


//static int line=0;

extern void SHA_Simple(void *p, int len, unsigned char *output);
extern void covert_hash_to_string( unsigned char *hash, char *hstr);
extern int str_segment(char *str,char *item[],char seg);
extern int authlog(const char *fmt,...);

extern struct parameter
{
	char	debug;
	int		listen_port;
	char	*module;
	char	*module_args;
	char	*logfile;
	char    *d_base;
}param;

/*****************************************************************
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
			printf("memory allot error\n");
			alog(LOG_ERR,"Memory allot error\n");
			i-=2;
			while(i>=0)
				free(p[i--]);
	        exit (1);
		}		
	}
	return i;
}
*******************************************************************/

int field_analy(char **p,char *field,int n)
{
	int i;
	for(i=0;i<n;i++)
		if(strcmp(p[i],field)==0)
			return i;
	return -1;
}

static int do_authenticate( struct AM_data *auth_data,struct AM_input *input)
{
//	char *buf;
	char user[128];
	char password[128];
	char hash[20];
//	char *item[32];
	int j=0,i=0;	
	alog(LOG_ERR, "file auth password ");
//	buf=auth_data->data;

	printf("user:%s %s.\n",input->username,input->password);
	
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
	strcpy(password,input->password);
	if(!j)
	{
		SHA_Simple(input->password,strlen(input->password),(unsigned char *)hash);
     	covert_hash_to_string( (unsigned char *)hash, password );
	}
	int nrow = 0, ncolumn = 0;
	char **azResult;
	char *sql = malloc(256);
	sprintf(sql,"SELECT * FROM user where password='%s'",password);
	sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg);
    free(sql);
	sqlite3_close(db);
	printf( "row:%d column=%d \n" , nrow , ncolumn );
	
	if(!nrow)
	{
		sqlite3_free_table( azResult );
		if(j)
			sprintf(input->username,"%s","[unknown]");
		return 0;
	}

	int field[8];
	field[0]=field_analy(azResult,"username",ncolumn);
	field[1]=field_analy(azResult,"password",ncolumn);
	
	for(i=1;i<=nrow;i++)
	{
		strcpy(user,azResult[ncolumn*i+field[0]]);
//		printf("form database:%s\n",user);
//		printf("input->username:%s\n",input->username);
		
		if(strcmp(user,"*")==0)
		{
			alog(LOG_ERR,"Invalid data in database.user,line %d:%s|%s\n",i,azResult[2*i],azResult[2*i+1]);
			alog(LOG_ERR,"Invalid data in database.user,line %d:the username is not allowed to be *",i);
			continue;
		}
		
		if(j)
		{
			SHA_Simple(user,strlen(user),(unsigned char *)hash);
			covert_hash_to_string( (unsigned char *)hash, user );
		}
		
		if(strcmp(user,input->username)==0)
		{
//			printf("HahA I reach here!\n");
			if(j)
				sprintf(input->username,"%s",azResult[2*i]);
			sqlite3_free_table( azResult );
			return 1;
		}
	}
		
	sqlite3_free_table( azResult );
	if(j)
		sprintf(input->username,"%s","[unknown]");
	return 0;
}


static int auth_init( struct AM_data *auth_data)
{

//	FILE *fp;
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

struct AM_auth aos_user = 
{
//	.auth_data.filename="user.conf",
	.name="user",
	.init = auth_init,
	.authenticate = do_authenticate,
//	.authorize = do_authorize,
	.close =auth_close,
};

