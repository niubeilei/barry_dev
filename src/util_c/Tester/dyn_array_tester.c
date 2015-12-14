////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 02/26/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/Tester/dyn_array_tester.h"

#include "util_c/memory.h"
#include <stdio.h>


int aos_dyn_array_tester()
{
	int i;
	int **aa;
	aa = aos_malloc(sizeof(int *) * 10);
	int bb[10];

	for (i=0; i<10; i++)
	{
		bb[i] = i;
		aa[i] = &bb[i];
	}

	for (i=0; i<10; i++)
	{
		printf("aa[i]: %d\n", *aa[i]);
	}
	return 0;
} 

