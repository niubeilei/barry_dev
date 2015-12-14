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
// Modification History:
// 01/30/2008: Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "porting_c/file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef OMN_PLATFORM_UNIX

// 
// Description:
// It retrieves the length of the named file. If the file is not
// found, it returns -1. Otherwise, it returns the file size. 
//
int64_t aos_file_get_size(const char * const filename)
{
	struct stat s;
	int ret = stat(filename, &s);
	if (ret) return -1;

	return s.st_size;
}

#elif defined(OMN_PLATFORM_MICROSOFT)
// 
// Microsoft portion
//
#endif // Platform

