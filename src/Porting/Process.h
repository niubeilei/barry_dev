////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Process.h
// Description:
// Comments: This file contains functions relating to processes.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Porting_Process_h
#define Omn_Porting_Process_h


#ifdef OMN_PLATFORM_UNIX


#include <sys/types.h>
#include <unistd.h>

#define OmnProcessIdType pid_t

inline OmnProcessIdType OmnGetCrtPID()
{
	return getpid();
}

extern int AosGetPeakMem();






#elif OMN_PLATFORM_MICROSOFT

#include <Windows.h>

#define OmnProcessIdType DWORD
inline OmnProcessIdType OmnGetCrtPId()
{
	return GetCurrentProcessId(void);
}

#endif
#endif
