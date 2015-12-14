#include "module.h"
#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

//#define N 9 //the legal fild total in a line

//static int line=0;

extern void SHA_Simple(void *p, int len, unsigned char *output);
extern void covert_hash_to_string( unsigned char *hash, char *hstr);
extern int str_segment(char *str,char *item[],char seg);
extern int authlog( const char * fmt, ... );
extern struct parameter
{
	char	debug;
	int		listen_port;
	char	*module;
	char	*module_args;
	char	*logfile;
	char    *d_base;
}param;

/****************************************************************
static int get_fileline(char *p[],FILE *fp)
{
	int i=0;
	char buf[1024];
	while(1)
	{
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

extern int field_analy(char **p,char *field,int n);

static int do_authorize( struct AM_data *auth_data, struct AM_input *input)
{
//	char *buf; //to copy the line read from the configure file
	char user[64]; //to store item[1] or the hash of item[1]
	char hash[20];
//	char *item[32];
//	char *s;
	int i=0;
    int ishash=0;

	
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

	int nrow = 0, ncolumn = 0;
	char **azResult;
	char *sql = malloc(256);
	
	
	sprintf(sql,"SELECT * FROM db_author where appname='%s' and auth_id='%s' ",input->password,input->au_id);
	printf("Input->au_id:%s\n",input->au_id);
	sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg);
    free(sql);
	sqlite3_close(db);
	printf( "db_author:row:%d column=%d \n" , nrow , ncolumn );
        
	if(strlen(input->username)>=20)
     	    ishash=1;
	
	if(!nrow)
	{
		sqlite3_free_table( azResult );
		if(ishash)
			sprintf(input->username,"%s","[unknown]");
		input->au_result=strdup("[NONE]");
		return 0;
	}
    
	int field[10];
	field[0]=field_analy(azResult,"appname",ncolumn);
	field[1]=field_analy(azResult,"username",ncolumn);
	field[2]=field_analy(azResult,"valid_time",ncolumn);
	field[3]=field_analy(azResult,"man_author",ncolumn);
	field[4]=field_analy(azResult,"authorize",ncolumn);
	field[5]=field_analy(azResult,"auth_id",ncolumn);
	
//	struct tm *local;
	time_t now_t,cur_t;
	now_t=time(NULL); 
	
	alog(LOG_ERR, "Double authorize:file auth appname "); 
	printf("user:%s %s.\n",input->username,input->password);


//	alog(LOG_ERR, "user file read:%s %s.\n",item[1],item[0]); 
	
	for(i=1;i<=nrow;i++)
	{

		cur_t=atoi(azResult[ncolumn*i+field[2]]);
		if(now_t>cur_t&&strcmp(azResult[ncolumn*i+field[2]],"0")!=0)
			continue;
	    strcpy(user,azResult[ncolumn*i+field[1]]);
		if(strcmp(azResult[ncolumn*i+field[1]],"*")!=0&&ishash)
		{
			SHA_Simple(user,strlen(user),(unsigned char *)hash);
			covert_hash_to_string( (unsigned char *)hash, user );
		}

		if(strcmp(azResult[ncolumn*i+field[1]],"*")!=0&&strcmp(user,input->username)!=0)
		   continue;
        
//		printf("Here 1!\n");
//		printf("Here 2!\n");
		
		if(strcmp(azResult[ncolumn*i+field[4]],"deny")==0)
		{	
			if(ishash)
				sprintf (input->username,"%s",azResult[ncolumn*i+field[1]]);
			input->au_result=strdup(azResult[ncolumn*i+field[3]]);
			return 0;
		}
		else if(strcmp(azResult[ncolumn*i+field[4]],"allow")==0)
		{	
			if(ishash)
				sprintf (input->username,"%s",azResult[ncolumn*i+field[1]]);
			input->au_result=strdup(azResult[ncolumn*i+field[3]]);
			return 1;
		}
		else
		{	
			alog(LOG_ERR,"Invalid data in database.db_auth,authorize field,line %d:The invalid field data is %s\n",i,azResult[ncolumn*i+field[4]]);
			continue;
		}

	}

	if(ishash)
		sprintf (input->username,"%s","[unknown]");
	input->au_result=strdup("[NONE]");
	return 0; //matching failed!

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
//
//  line=get_fileline(auth_data->data,fp);
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

struct AM_auth aos_dbauthor = 
{
//	.auth_data.filename="db_author.conf",
	.name="db_author",
	.init = auth_init,
//	.authenticate = do_authenticate,
	.authorize = do_authorize,
	.close =auth_close,
};

