////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Sem.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Sem_h
#define Omn_Porting_Sem_h

#ifdef OMN_PLATFORM_UNIX


#include <semaphore.h>
#include <errno.h>

const long SEC_US=1000000;
const long SEC_NS=1000000000;


typedef sem_t OmnSemType;



inline int OmnInitSem(OmnSemType &sem, int value)
{
	return sem_init(&sem, 0, value);
}


inline int OmnDestroySem(OmnSemType &sem)
{
	return sem_destroy(&sem);
}

// bool OmnSemLock(OmnSemType &lock);
inline bool OmnSemPost(OmnSemType &sem)
{
	return (sem_post(&sem) == 0);
}

inline bool OmnSemWait(OmnSemType &sem)
{
	return (sem_wait(&sem) == 0);
}

inline bool OmnSemTryWait(OmnSemType &sem)
{
	return (sem_trywait(&sem) == 0);
}

inline bool OmnSemTimedWait(OmnSemType &sem, bool &timeout, int ms)
{
	/*
	timeval crtTime;
	OmnGetTimeOfDay(&crtTime);
	timespec t;

	int ss = 0;
	crtTime.tv_usec += ms;

	//Phil 2015/08/20
	//the usec could accumulated into seconds
	ss += crtTime.tv_usec / 1000000;
	crtTime.tv_usec = crtTime.tv_usec % 1000000;

	t.tv_nsec = crtTime.tv_usec*1000;
	t.tv_sec = crtTime.tv_sec + ss;
	*/
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += ms*SEC_US;
	if (ts.tv_nsec >= SEC_NS)
	{
		ts.tv_sec += ts.tv_nsec/SEC_NS;
		ts.tv_nsec = ts.tv_nsec%SEC_NS;
	}

	int rslt = sem_timedwait(&sem, &ts);
	if (rslt == -1)
	{
		if (errno == ETIMEDOUT)
			timeout = true;
	}
	return rslt == 0;
}


#elif defined(OMN_PLATFORM_MICROSOFT)
#include "Porting/GetErrno.h"
#include <winsock2.h>
#define OmnSemType		HANDLE

/*
inline OmnSemType OmnInitSem(OmnSemType &lock)
{
    return lock = CreateSem(NULL, FALSE, NULL);
}


inline int OmnDestroySem(OmnSemType &lock)
{
	return CloseHandle(lock);
}


inline bool OmnSemLock(OmnSemType &lock)
{
    return (WaitForSingleObject(lock, INFINITE) != WAIT_FAILED);
}



inline bool OmnSemUnlock(OmnSemType &lock)
{
	
    if (!ReleaseSem(lock))
	{
		int err = OmnGetErrno();
		if (err == 288)
		{
			return true;
		}

		return false;
	}

	return true;
}
*/
#endif

#endif

