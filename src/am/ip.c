#include "module.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


static int do_authenticate( struct AM_data *auth_data, struct AM_input *input)
{

	return 1;
}


static int do_authorize( struct AM_auth *sm , char * name, char * resource)
{	
	return 1;
}

static int auth_init( struct AM_data *auth_data)
{
 	FILE *fp;
	fp=fopen(auth_data->filename,"r");
    auth_data->data=malloc(8192);
    fread(auth_data->data,8192,1,fp);
    fclose(fp);
    return 0;
}

static void auth_close( struct AM_data *auth_data)
{
	free(auth_data->data);
}

struct AM_auth aos_ip = 
{
	.auth_data.filename="ipdeny.conf",
	.name="ip",
	.init = auth_init,
	.authenticate = do_authenticate,
	.authorize = do_authorize,
	.close =auth_close,
};

