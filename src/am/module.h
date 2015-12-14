#ifndef __MODULE_H
#define __MODULE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "syslog.h"
#define MAX_LINE 4096
//typedef unsigned long u32; 
//typedef uint64_t u64;


struct AM_input
{
	char *username;
	char *password;
	u32  client_ip;
	char *session_id;
	char *au_result;
	char *au_id;
};

struct AM_data
{
	char filename[128];	
	char *data[MAX_LINE];
};

struct AM_auth
{
	struct AM_data auth_data;
	char name[32];
	int (*init)(struct AM_data *auth_data);
	int (*authenticate)(struct AM_data *auth_data,struct AM_input *input);
	int (*authorize)(struct AM_data *auth_data,struct AM_input *input);
	int (*close)(struct AM_data *auth_data);
};

extern struct AM_auth aos_user;
extern struct AM_auth aos_ip;
extern struct AM_auth aos_author;
extern struct AM_auth aos_dbauthor;
extern struct AM_auth aos_webauth;

struct storage_module
{
	char * name;
	int (*init)( struct storage_module * sm, char * args );
	int (*auth_password)( struct storage_module * sm, char * user,  char * resource );
	int (*auth_authorize)( struct storage_module * sm , char * user, char * resource );


	void	*pdata;
};

extern struct storage_module sm_file;
extern struct storage_module sm_mysql;
#define alog syslog
#endif

