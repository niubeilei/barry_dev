

#include "module.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct file_info
{
	char * filename;
};

static int file_auth_password( struct storage_module *sm, char * name, char * resource )
{
	FILE * fp;
	char user[128];
	char password[128];
	char groups[128];
	int ret;	
		
	struct file_info * fi=(struct file_info*)(sm->pdata);

	alog(LOG_ERR, "file auth password ");
	fp = fopen( fi->filename, "r" );
	if(!fp)
	{
		alog(LOG_ERR, "open file %s error\n", fi->filename );
		goto fail;
	}

	printf("user:%s %s.\n",name,resource);
	while(1)
	{
		ret = fscanf( fp, "%s %s %s\n", user, password, groups );
		alog(LOG_ERR, "user file read:%s %s %s.\n",user,password,groups);
		if( ret<2 )
		{
			fclose(fp);
			goto fail;
		}

		if(strcmp(name,user)==0 && strcmp(resource,password)==0)
		{
			fclose(fp);
			goto success;
		}
	}
	
	fclose(fp);

fail:
	return 0;
success:
	return 1;
}

static int strcontain( char * str, char * sub )
{
	char * s;

	s = strtok( str, "," );
	while(s)
	{
		alog(LOG_ERR, "strtok:%s.\n", s);
		if(strcmp(s,sub)==0)
			return 1;
		s = strtok( 0, "," );
	}

	return 0; //false
}

static int file_auth_authorize( struct storage_module *sm , char * name, char * resource)
{
	FILE * fp;
	char user[64];
	char password[64];
	char resources[128];
	int ret;	
		
	struct file_info * fi=(struct file_info*)(sm->pdata);

	alog(LOG_ERR, "file auth authorize:%s,%s.",name,resource);
	fp = fopen( fi->filename, "r" );
	if(!fp)
	{
		alog(LOG_ERR, "open file %s error\n", fi->filename );
		goto fail;
	}

	while(1)
	{
		ret = fscanf( fp, "%s %s %s\n", user, password, resources);
		if( ret<2 )
		{
			goto close_fail;
		}

		alog(LOG_ERR, "file read:%s %s %s.\n", user,password,resources );

		if(strcmp(name,user)==0 && strcontain(resources,resource))
		{
			goto close_success;
		}
	}
	
close_fail:
	fclose(fp);
fail:
	return 0;

close_success:
	fclose(fp);
	return 1;
}

static int file_init( struct storage_module *sm , char * args )
{
	struct file_info * fi=(struct file_info*)malloc(sizeof(struct file_info));

	if( args == 0 )
	{
		puts("file module need a arg: filename");
		return -1;
	}

	if( access(args,R_OK) )
		return -1;

	fi->filename = args;
	sm->pdata = fi;

	return 0;
}

struct storage_module sm_file = 
{
	.init = file_init,
	.auth_password = file_auth_password,
	.auth_authorize = file_auth_authorize,
};

