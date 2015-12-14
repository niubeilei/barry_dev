////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: File.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_File_h
#define Omn_Porting_File_h

#ifdef OMN_PLATFORM_UNIX

#include "Alarm/Alarm.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


unsigned long long inline OmnGetFileLength(const char * fname)
{
	struct stat st;
	int rslt = stat(fname, &st);
	if (rslt == -1) return 0;
	return (unsigned long long)st.st_size;
}


unsigned long inline OmnGetFileLastModifyTime(const char * fname)
{
	struct stat st;
	int rslt = stat(fname, &st);
	if (rslt == -1) return 0;
	return (unsigned long)st.st_mtime;
}


int inline OmnFlushFile(const int fd)
{
	return fdatasync(fd);
}


int inline OmnFlushFile(FILE *f)
{
	if (fflush(f) == 0) return 0;

	OmnAlarm << "Failed to flush: " << errno << enderr;
	return errno;
}

#elif OMN_PLATFORM_MICROSOFT
#define OMN_E_RESOURCE_N_A EDEADLOCK

#include <iostream.h>
#include <stdio.h>
#include <Winsock2.h>
#include <Windows.h>

int inline OmnFlushFile(const int fd)
{
	//
	// Not implemented yet
	//
	cout << "********** " << __FILE__ << ":" << __LINE__ << ": "
		<< "Not implemented yet!" << endl;
	return 0;
}

int inline OmnFlushFile(FILE *f)
{
	//
	// Not implemented yet
	//
	return fflush(f);
}


// 
// OmnCreateDirectory(...)
//
#define OmnPathType LPCTSTR 
#define OmnFileSecurityAttrType LPSECURITY_ATTRIBUTES

bool inline OmnCreateDirectory(OmnPathType path,
							   OmnFileSecurityAttrType security)
{
	return CreateDirectory(path, security)?true:false;
}


#endif


#endif

