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
// 03/03/2008 Copied from Porting
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_porting_time_of_day_h
#define Omn_porting_time_of_day_h

#ifdef __KERNEL__
#include <linux/time.h>
#include "aosUtil/Types.h"

static inline u32 aos_get_usec(void)
{
	struct timeval tp;
	do_gettimeofday(&tp);
	return tp.tv_usec;
}


static inline void aos_gettimeofday(struct timeval *tp)
{
	return do_gettimeofday(tp);
}

#elif OMN_PLATFORM_UNIX
#include <sys/time.h>
#include <time.h>
#include "aosUtil/Types.h"


// 
// Chen Ding, 02/10/2006
//
static inline u32 aos_get_usec(void)
{
	struct timeval tp;
	gettimeofday(&tp, 0);
	return tp.tv_usec;
}


static inline int aos_gettimeofday(struct timeval *tp) 
{
	return gettimeofday(tp, 0);
}


static inline int aos_system_relative_sec()
{
	struct timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec;
}


static inline u32 aos_get_sec()
{
	struct timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec;
}



#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>

inline i64 OmnSystemRelativeSec1()
{
	// 
	//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//							IMPORTANT
	//  This function returns the relative system seconds. It should
	//  be used as a reference point only. Note that the function:
	//	GetTickCount() will wrap back to 0 in about 49.7 days. 
	//
	DWORD tmpTime = GetTickCount();
	return tmpTime/1000;
}


inline bool 
OmnGetTimeOfDay(timeval *tp) 
{
	// 
	// The GetTickCount function retrieves the number of milliseconds that 
	// have elapsed since the system was started. It is limited to the 
	// resolution of the system timer. If you need a higher resolution timer, 
	// use amultimedia timer or a high-resolution timer. 
	//
	// Windows NT 3.5 and later		The system timer runs at approximately 10ms. 
	// Windows NT 3.1				The system timer runs at approximately 16ms. 
	// Windows 95 and later			The system timer runs at approximately 55ms. 
	// 
	// IMPORTANT: Do not call this function ligithly since this function 
	// uses '/' and '*'. It can be pretty slow.
	//

	DWORD tmpTime = GetTickCount();
	tp->tv_sec = tmpTime/1000;
	tp->tv_usec = (tmpTime % 1000)*1000;
	return true;
}





// BOOL QueryPerformanceCounter(          LARGE_INTEGER *lpPerformanceCount


inline unsigned int
OmnGetMsecSinceStart()
{
	// 
	//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//							IMPORTANT
	//  This function returns the relative system seconds. It should
	//  be used as a reference point only. Note that the function:
	//	GetTickCount() will wrap back to 0 in about 49.7 days. 
	//
	return (unsigned int)GetTickCount();
}

#endif
#endif

