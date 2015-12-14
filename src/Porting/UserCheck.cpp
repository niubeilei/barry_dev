////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Uid.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Porting/UserCheck.h"

#ifdef OMN_PLATFORM_UNIX

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

int read_passwd(char *username,char *passwd);
int get_salt(char *passwd,char *salt);

bool	OmnCheckSystemUser(char* uname,char* pword)
{
	bool rslt = false;
	if ( !getpwnam(uname) )
	{
		return false;
	}

	char passwd[100];
	read_passwd(uname,passwd);

	char salt[100];
	get_salt(passwd,salt);

	char *block1 = (char*)malloc(sizeof(char*)*34);
	char *password =block1;
	password=(char*)crypt(pword,salt);

	if(!strcmp(passwd,password))
	{
		rslt = true;
	}
	else
	{
		rslt = false;
	}
	free(block1);
	return rslt;
}

int read_passwd(char *username,char *passwd)
{
	int i,j,n;
	
	FILE * fd;

	char buf[1024];

	fd=fopen("/etc/shadow","r");

	while (fgets(buf,1024,fd)!=NULL){

		if(strstr(buf,username))//bug

			break;

	}
	
	n=0;
	for(i=0,j=0;i<1024;i++)
	{
		if( ':' == buf[i]) 
			n++;
		if(2 == n)
			break;
		if( 1 == n ){

			passwd[j]=buf[i+1];
			j++;
		}
	}
	passwd[j-1]='\0';

	fclose(fd);
	return 0;
}
		
int get_salt(char *passwd,char *salt)
{
	int i,n=0;
	char *block2 = (char *)malloc(100);
	char *tmp = block2;
	if(strlen(tmp) < strlen(passwd))
	{
		return 0;
	}
	strcpy(tmp,passwd);

	for(i=0;i<100;i++)
	{
		if ( *tmp == '$' )
			n++;
		salt[i]=*tmp;
		if ( 3 == n )
			break;
		tmp++;
	}
	salt[i+1]='\0';
	free(block2);	
	return 0;
}



#endif
