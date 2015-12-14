////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: mysudo.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	char	arg[1024];
	int i;
	int p=0;

	arg[0]=0;

	for(i=1;i<argc; i++)
	{
		strncpy(&arg[p],argv[i],1020-p);
		p=strlen(arg);
		arg[p]=' ';
		arg[p+1]=0;
		p++;
	}

	setuid(0);
	seteuid(0);
	i = system(arg);

	if(i && 0==i%256)
	{
		i=255;
	}

	return i;
}
