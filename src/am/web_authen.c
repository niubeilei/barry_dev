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


extern int str_segment(char *str,char *item[],char seg);
extern int authlog( const char * fmt, ... );

extern struct parameter
{
	char	debug;
	int	listen_port;
	char	*module;
	char	*module_args;
	char	*logfile;
	char    *d_base;
}param;



extern int field_analy(char **p,char *field,int n);

static int do_authenticate( struct AM_data *auth_data, struct AM_input *input)
{
	int i=0;
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	int rc;
	
	rc = sqlite3_open(param.d_base, &db);
	
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}
	
	else 
		printf("You have opened a sqlite3 database named hac.db successfully!\n\
			Congratulations! Have fun ! ^-^ \n");

	int nrow = 0, ncolumn = 0;
	char **azResult;
	char *sql = malloc(256);
	
	
	sprintf(sql,"SELECT * FROM webauth where session_id='%s' ",input->au_id);
	printf("Input->au_id:%s\n",input->au_id);
	sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg);
    free(sql);
	sqlite3_close(db);
	printf( "webauth:row:%d column=%d \n" , nrow , ncolumn );
        
	
	if(!nrow)
	{
		sqlite3_free_table( azResult );
		input->username=strdup("[unknown]");
		return 0;
	}
    
	int field[10];
	field[0]=field_analy(azResult,"session_id",ncolumn);
	field[1]=field_analy(azResult,"username",ncolumn);
	field[2]=field_analy(azResult,"valid_time",ncolumn);
	
	time_t now_t,cur_t;
	now_t=time(NULL); 
	
	
	for(i=1;i<=nrow;i++)
	{
	    cur_t=atoi(azResult[ncolumn*i+field[2]]);
	    if(now_t>cur_t&&strcmp(azResult[ncolumn*i+field[2]],"0")!=0)
			continue;
//	    input->username=strdup(azResult[ncolumn*i+field[1]]);
		input->username=strdup(azResult[ncolumn*i+field[1]]);
	    return 1;
		
	}
	
	input->username=strdup("[unknown]");
	return 0; 

}


static int auth_init( struct AM_data *auth_data)
{

	return 0;
}

static int auth_close( struct AM_data *auth_data)
{
	return 0;
}

struct AM_auth aos_webauth = 
{
//	.auth_data.filename="db_author.conf",
	.name="web_authen",
	.init = auth_init,
	.authenticate = do_authenticate,
//	.authorize = do_authorize,
	.close =auth_close,
};

