////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimeOfDay.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_TimeOfDay_h
#define Omn_Porting_TimeOfDay_h

#include "Util/String.h"

#ifdef __KERNEL__
#include <linux/time.h>
#include "aosUtil/Types.h"

static inline u32 AosGetUsec(void)
{
	struct timeval tp;
	do_gettimeofday(&tp);
	return tp.tv_usec;
}


static inline void OmnGetTimeOfDay(struct timeval *tp)
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
static inline u32 AosGetUsec(void)
{
	struct timeval tp;
	gettimeofday(&tp, 0);
	return tp.tv_usec;
}


inline int 
OmnGetTimeOfDay(struct timeval *tp) 
{
	return gettimeofday(tp, 0);
}

inline i64
OmnSystemRelativeSec1()
{
	timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec;
}


inline u64 OmnGetSystemTime()
{
	timeval t;
	gettimeofday(&t, 0);
	// return (((u64)t.tv_sec) << 32) + (u32)t.tv_usec;
	u64 tt = (u64)t.tv_sec;
	tt *= 1000;
	tt += (t.tv_usec)/1000;
	return tt;
}


inline u64 OmnGetTimestamp()
{
	timeval t;
	gettimeofday(&t, 0);
	return (((u64)t.tv_sec) * 1000000) + (u32)t.tv_usec;
}


inline OmnString OmnGetTimebasedId()
{
	timeval t;
	gettimeofday(&t, 0);
	OmnString str;
	str << (u64)t.tv_sec << "_" << (u64)t.tv_usec;
	return str;
}


// Chen Ding, 2014/01/30
// inline u32
inline i64
OmnGetSecond()
{
	timeval t;
	gettimeofday(&t, 0);
	return t.tv_sec;
}


inline unsigned long
OmnGetUsec()
{
	timeval t;
	gettimeofday(&t, 0);
	return t.tv_usec;
}

inline int 
OmnGetTimeZoneOffset()
{
	//struct timezone{
	//int tz_minuteswest; /*和Greenwich 时间差了多少分钟*/
	//int tz_dsttime; /*日光节约时间的状态*/
	//};
	struct timezone tz;
	timeval t;
	gettimeofday (&t , &tz);
	// GMT + 8: -480 minutes
	return tz.tz_minuteswest;
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

