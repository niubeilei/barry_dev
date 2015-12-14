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
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_porting_thread_h
#define Omn_porting_thread_h


#ifdef OMN_PLATFORM_UNIX
#include <pthread.h>

struct aos_thread;

typedef void * aos_thread_return_type_t;
typedef void * aos_thread_parm_type_t;
typedef void (*aos_thread_func_t)(struct aos_thread *thread);
typedef void *(*aos_raw_thread_func_t)(void *);
typedef pthread_t aos_thread_id_t;

static inline aos_thread_id_t aos_get_crt_thread_id()
{
	return pthread_self();
}

extern int aos_create_thread(
		aos_raw_thread_func_t thread_func,		
		void *parm,
		pthread_t *thrdId, 
		const int high);

void aos_exit_thread(void *retval);

#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>

typedef DWORD WINAPI aos_thread_return_type_t;
typedef LPVOID aos_thread_parm_type_t;
typedef DWORD WINAPI (*x)(LPVOID);
typedef DWORD aos_thread_id_t;

inline aos_thread_it_type_t aos_get_crt_thread_id()
{
	return GetCurrentThreadId();
}

inline int aos_create_thread(
		size_t stackSize, 
		aos_thread_func_t thread_func,		
		void *parm,
		unsigned long &thrdId, 
		const bool high)
{
	HANDLE handle = CreateThread(NULL, 0, threadFunc, parm, 0, &thrdId);
	return (handle == NULL)?-1:0;
}


#endif
#endif
