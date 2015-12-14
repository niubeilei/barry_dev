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
#include "../rhcApi.h"

#define MAX_RSLT_LEN 2048

int main(int argc, char **argv)
{	
	char rslt[MAX_RSLT_LEN];	

	fwIpsecPass("off", rslt);
	printf("The result is : %s\n", rslt);
	fwIpsecPass("on", rslt);
	printf("The result is : %s\n", rslt);
	
	fwL2tpPass("off", rslt);
	printf("The result is : %s\n", rslt);
	fwL2tpPass("on", rslt);
	printf("The result is : %s\n", rslt);
	
	fwPptpPass("off", rslt);
	printf("The result is : %s\n", rslt);
	fwPptpPass("on", rslt);
	printf("The result is : %s\n", rslt);

	return 0;
} 

