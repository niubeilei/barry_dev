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
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanobj/Tester/array_tester.h"

#include "semanobj/array_so.h"
#include "semantics/semantics.h"
#include <stdio.h>
#include <stdarg.h>

#define TESTIT(count, x...) testit(count, ##x)

void testit(int i, ...)
{
	va_list argptr;
	va_start(argptr, i);
	if ( i == 0 ) 
	{
		int n = va_arg( argptr, int );
		printf( "It is an integer: %d\n", n );
	} 
	else 
	{
		char *s = va_arg( argptr, char* );
		printf( "It is a string: %s\n", s);
	}
}

int 
main(int argc, char **argv)
{
	aos_semantics_init();

	aos_array_tester();
	return 0;
} 


