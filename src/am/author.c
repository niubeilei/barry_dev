#include "module.h"
#include <stdio.h>
#include <string.h>
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

/***************************************************************
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
*********************************************************************/


extern int field_analy(char **p,char *field,int n);

static int do_authorize( struct AM_data *auth_data, struct AM_input *input)
{
//	char *buf; //to copy the line read from the configure file
	char user[64]; //to store item[1] or the hash of item[1]
	char hash[20];
//	char *item[32];
	char *s;
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
	else printf("Authorize:You have opened a sqlite3 database named hac.db successfully!\n\
			Congratulations! Have fun ! ^-^ \n");

	int nrow = 0, ncolumn = 0;
	char **azResult;
	char *sql = malloc(256);
	
	sprintf(sql,"SELECT * FROM authorize where appname='%s' ",input->password);
	sqlite3_get_table( db , sql , &azResult , &nrow , &ncolumn , &zErrMsg);
    	free(sql);
	sqlite3_close(db);
//	printf( "row:%d column=%d \n" , nrow , ncolumn );
        
	if(strlen(input->username)>=20)
	{
		ishash=1;
	}
	if(!nrow)
	{
	    sqlite3_free_table( azResult );
	    if(ishash)
		sprintf(input->username,"%s","[unknown]");
	    return 2;
	}

	
	int field[20];
	field[0]=field_analy(azResult,"appname",ncolumn);
	field[1]=field_analy(azResult,"username",ncolumn);
	field[2]=field_analy(azResult,"time",ncolumn);
	field[3]=field_analy(azResult,"week",ncolumn);
	field[4]=field_analy(azResult,"day",ncolumn);
	field[5]=field_analy(azResult,"month",ncolumn);
	field[6]=field_analy(azResult,"year",ncolumn);
	field[7]=field_analy(azResult,"conn_time",ncolumn);
	field[8]=field_analy(azResult,"db_author",ncolumn);
	field[9]=field_analy(azResult,"authorize",ncolumn);
// add two field!!
    	field[10]=field_analy(azResult,"auth_id",ncolumn);	
    	field[11]=field_analy(azResult,"ip",ncolumn);	
	
	int hour,mint; //the current time of the computer
    	char wday[2],mday[3]; //the current date of the computer
	int h1,h2,m1,m2; //the time read from the conf
	char month[8],year[8];
	
	struct tm *local;
	time_t t;

    //if input is a hash name:

	// Get the current time and date from the computer:
	t=time(NULL); 
	local=localtime(&t);
	//get hour:
	hour=local->tm_hour;
	//get minute:
    	mint=local->tm_min;
	sprintf(month,"%d",local->tm_mon+1);
	sprintf(year,"%d",local->tm_year+1900);
	//get the day of week:
    	if(local->tm_wday==0) 
	{
		wday[0]='7'; 
		wday[1]=0; 
	}
    	else 
	{
		wday[0]='0'+local->tm_wday; 
		wday[1]=0; 
	}
	//get the day of month:
    	if(local->tm_mday < 10) 
	{
		mday[0]='0'+local->tm_mday; 
		mday[1]=0; 
	}
    	else 
	{
		mday[0]='0'+local->tm_mday/10; 
		mday[1]='0'+local->tm_mday%10; 
		mday[2]=0; 
	}

	alog(LOG_ERR, "file auth appname "); 
	printf("Authorize module!  user:%s %s.\n",input->username,input->password);


	// Match the lines in database one by one:
	for(i=1;i<=nrow;i++)
	{
	    alog(LOG_ERR, "database.authorize read:%s %s.\n",azResult[ncolumn*i+field[1]],azResult[ncolumn*i+field[0]]); 
	    strcpy(user,azResult[ncolumn*i+field[1]]);
	    if(ishash && strcmp(azResult[ncolumn*i+field[1]],"*")!=0)
	    {
		SHA_Simple(user,strlen(user),(unsigned char *)hash);
		covert_hash_to_string( (unsigned char *)hash, user );		
	    }

	//////Start to match: //////

        //Step 1: compare the username and get ready for write log!
	    if(strcmp(azResult[ncolumn*i+field[1]],"*")!=0)
	    {
	       if(strcmp(input->username,user)!=0)
			   continue;
	    }

	    //Step 2: compare the time
	    if(strcmp(azResult[ncolumn*i+field[2]],"*")!=0)
	    {
		sscanf(azResult[ncolumn*i+field[2]], "%d:%d-%d:%d", &h1,&m1,&h2,&m2);//get the time in the conf
		if(h1<0||h1>23||m1<0||m1>59 || h2<0||h2>23||m2<0||m2>59) 
		{
		    alog(LOG_ERR,"Invalid time in database.author,line %d:%s\n",i,azResult[ncolumn*i+field[2]]);
		    continue;
		}
	    	if(hour<h1 || hour>h2) 
		    continue;
	    	else if(hour==h1 && mint<m1) 
		    continue;
	    	else if(hour==h2 && mint>m2) 
		    continue;
	    }

	    //Step 3: compare the day of the week
	    if(strcmp(azResult[ncolumn*i+field[3]],"*")!=0)
	    {   
		s = strtok(azResult[ncolumn*i+field[3]], ",");
	        while(s)// search wday in item[3]
	  	{
		    if(strcmp(s,wday)!=0) 
			s = strtok( 0, "," );
		    else break;
	        } 
		if(!s) 
		    continue;
	    } 

	    //Step 4: compare the day of the month
	    if(strcmp(azResult[ncolumn*i+field[4]],"*")!=0)
	    {	
		s = strtok(azResult[ncolumn*i+field[4]], ",");
		while(s)
		{//search mday in item[4]
		    if(strcmp(s,mday)!=0) 
			s = strtok( 0, "," );
		    else break;
	        } 
	        if(!s) 
		    continue;
	    } 
	  
	
	    //Step 5: month
	    if(strcmp(azResult[ncolumn*i+field[5]],"*")!=0)
	    {	
		s = strtok(azResult[ncolumn*i+field[5]], ",");
		while(s)
		{
		    if(strcmp(s,month)!=0) 
			s = strtok( 0, "," );
		    else break;
	        } 
	        if(!s) 
		    continue;
	    } 
	    //Step 6: year and ip
	    if(strcmp(azResult[ncolumn*i+field[6]],"*")!=0)
	    {
		if(strcmp(azResult[ncolumn*i+field[6]],year)!=0)
		    continue;
	    } 
		
	    if(strcmp(azResult[ncolumn*i+field[11]],"*")!=0)
	    {
		printf("%s:%d\n",azResult[ncolumn*i+field[11]],inet_addr(azResult[ncolumn*i+field[11]]));
		if(input->client_ip!=inet_addr(azResult[ncolumn*i+field[11]]))
		    continue;
	    }
	    //Step 7: Arriving here indicates that the first 7 filds are matched successfully
		
	    if(strcmp(azResult[ncolumn*i+field[8]],"yes")!=0&&strcmp(azResult[ncolumn*i+field[8]],"no")!=0)
	    {
		alog(LOG_ERR,"Invalid data in database.authorize ,db_author field,line %d:the invalid field is %s\n",i,azResult[ncolumn*i+field[8]]);
		continue;
	    }
	    
	    if(strcmp(azResult[ncolumn*i+field[9]],"allow")!=0&&strcmp(azResult[ncolumn*i+field[9]],"deny")!=0)
	    {
		alog(LOG_ERR,"Invalid data in database.authorize ,authorize field,line %d:the invalid field is %s\n",i,azResult[ncolumn*i+field[9]]);
		continue;
	    }
		
	    if(strcmp(azResult[ncolumn*i+field[9]],"deny")==0) 
	    {
//			free(buf); 
		if(ishash)
		    sprintf (input->username,"%s",azResult[ncolumn*i+field[1]]);
		return 2;
	    }

		
/*************************************************************************		
		int conn_time=0;
		if(strcmp(azResult[10*i+7],"*")!=0)
		{
			conn_time=atoi(azResult[10*i+7]);
			if(conn_time<=0)
			{
				alog(LOG_ERR,"Invalid data in database.authorize,conn_time field,line %d:%s\n",i,azResult[10*i+7]);
				continue;
			}
		}
*************************************************************************/
		
		
	    //Step 8: to see authorize is "allow" or 'deny" :
	    if(strcmp(azResult[ncolumn*i+field[8]],"no")==0) 
	    {
//			free(buf); 
		if(ishash)
		    sprintf (input->username,"%s",azResult[ncolumn*i+field[1]]);
		input->au_result=strdup(azResult[ncolumn*i+field[7]]);
		return 3;
	    }
	    if(strcmp(azResult[ncolumn*i+field[8]],"yes")==0) 
	    {
//			free(buf); 
			
		if(ishash)
		    sprintf (input->username,"%s",azResult[ncolumn*i+field[1]]);
		input->au_result=strdup(azResult[ncolumn*i+field[7]]);
		input->au_id=strdup(azResult[ncolumn*i+field[10]]);
		return 4;
	    }

	}

	//Arriving here indicates that NO line is matched; 

	
	if(ishash)//if it is a hash name
	{
//		authlog("%s %s'","[unkown]",input->password);
	    sprintf(input->username,"%s","[unknown]");
	}
	
	return 2; //matching failed!

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

//      line=get_fileline(auth_data->data,fp);
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

struct AM_auth aos_author = 
{
	.auth_data.filename="author.conf",
	.name="author",
	.init = auth_init,
//	.authenticate = do_authenticate,
	.authorize = do_authorize,
	.close =auth_close,
};

