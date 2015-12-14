////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThreadDef.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_ThreadDef_h
#define Omn_Porting_ThreadDef_h


#ifdef OMN_PLATFORM_UNIX
#include <pthread.h>

#define OmnThreadReturnType void*
#define OmnThreadParmType void*
#define OmnThreadFuncDef(x) void *(*x)(void *)
#define OmnThreadIdType pthread_t

inline OmnThreadIdType OmnGetCurrentThreadId()
{
	return pthread_self();
}

OmnThreadIdType OmnCreateThread(OmnThreadReturnType (*threadFunc)(OmnThreadParmType ),
				   void *parm,
				   const bool high = false);
int OmnCancelThread(OmnThreadIdType &t);
void OmnExitThread(void *retval);

#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>

#define OmnThreadReturnType DWORD WINAPI
#define OmnThreadParmType LPVOID
#define OmnThreadFuncDef(x) DWORD WINAPI x(LPVOID)
#define OmnThreadIdType DWORD

inline OmnThreadIdType OmnGetCurrentThreadId()
{
	return GetCurrentThreadId();
}

inline int OmnCreateThread(size_t stackSize, 
						   LPTHREAD_START_ROUTINE threadFunc,
						   void *parm,
						   unsigned long &thrdId, 
						   const bool high = false)
{
	HANDLE handle = CreateThread(NULL, 0, threadFunc, parm, 0, &thrdId);
	return (handle == NULL)?-1:0;
}


#endif
#endif
