////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 03/06/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/file.h"

#include "alarm_c/alarm.h"
#include "util_c/memory.h"
#include <stdio.h>

// 
// Description:
// This function reads an entire file into memory, stored in 
// 'contents'. It will allocate the memory for it. 
// If successful, it returns 0. 
//
int aos_file_read_file(
		const char * const fn, 
		char **contents, 
		int *len)
{
	aos_assert_r(fn, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(len, -1);

	FILE *file = fopen(fn, "r");
	aos_assert_rm(file, -1, "Failed to open file: %s", fn);

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	aos_assert_r(fsize >= 0, -1);
	fseek(file, 0, SEEK_SET);

	*contents = aos_malloc(fsize+1);
	aos_assert_r(*contents, -1);

	long n = fread(*contents, 1, fsize, file);
	(*contents)[fsize] = 0;
	aos_assert_rm(n == fsize, -1, "fsize: %d, read size: %d", fsize, n);
	*len = n;

	fclose(file);
	return 0;
}


// 
// Description
// It writes the contents to the file. If the file does not exist,
// it will create the file. If the file exists, it wipes out
// the existing contents. 
//
int aos_file_write_to_file(
		const char * const fn, 
		const char * const contents, 
		const int len)
{
	aos_assert_r(fn, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(len > 0, -1);

	FILE *file = fopen(fn, "w");
	aos_assert_rm(file, -1, "Failed to open file: %s", fn);

	size_t n = fwrite(contents, 1, len, file);
	aos_assert_r(n == len, -1);
	fclose(file);
	return 0;
}


// 
// Description
// It appends the contents to the file.
//
int aos_file_append_to_file(
		const char * const fn, 
		const char * const contents, 
		const int len)
{
	aos_assert_r(fn, -1);
	aos_assert_r(contents, -1);
	aos_assert_r(len > 0, -1);

	FILE *file = fopen(fn, "a");
	aos_assert_rm(file, -1, "Failed to open file: %s", fn);

	size_t n = fwrite(contents, 1, len, file);
	aos_assert_r(n == len, -1);
	fclose(file);
	return 0;
}

