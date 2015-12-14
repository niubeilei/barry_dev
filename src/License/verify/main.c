////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: verify\main.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "verify_license.h"


int main(int argc, char *argv[])
{
	int ret;
	uid_t user_id;

	if((user_id=getuid())!=0){
		printf("Please use root user!\n");
		exit(1);
	}
	
	ret=verify_license();
	if(ret){
		printf("license correct\n");
	}else{
		printf("license not correct\n");
	}
	return 0;
}
