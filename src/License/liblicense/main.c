////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>

#include "verify.h"

int main(int argc, char *argv[])
{
	int ret;

	ret=verify_license();

	if(ret){
		printf("license is correct\n");
	}else{
		printf("license is not correct\n");
	}
	return 0;
}
	
