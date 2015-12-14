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
#ifndef Omn_porting_cond_var_h
#define Omn_porting_cond_var_h


#ifdef OMN_PLATFORM_UNIX

#include "porting_c/mutex.h"
#include "porting_c/time_of_day.h"
#include <pthread.h>
#include <errno.h>

typedef pthread_cond_t aos_condvar_t;

static inline void aos_condvar_init(aos_condvar_t *var) 
{
    pthread_cond_init(var, 0);
}


static inline int aos_condvar_wait(
		aos_condvar_t *var, 
		aos_lock_t *lock) 
{
    pthread_cond_wait(var, lock);
	return 0;
}


static inline void aos_condvar_signal(aos_condvar_t *var)
{    
	pthread_cond_signal(var);
}


static inline int aos_condvar_timed_wait(
		aos_condvar_t *var, 
		aos_lock_t *lock,
		int *is_timedout, 
		const int sec)
{
	struct timeval crtTime;
	aos_gettimeofday(&crtTime);
	struct timespec t;
	t.tv_nsec = 0;
	t.tv_sec = crtTime.tv_sec + sec;

	int ret = pthread_cond_timedwait(var, lock, &t);
	*is_timedout = (ret == ETIMEDOUT);

	return 0;
}













//
// Microsoft Portion
//
#elif OMN_PLATFORM_MICROSOFT
#include "Alarm/alarm.h"
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#define  OmnCondVarType	HANDLE

inline bool OmnCondVarInit(OmnCondVarType &condVar, const OmnString &name)
{
	if (name.length() > 0)
	{
		condVar = CreateEvent(NULL, false, false, name);
	}
	else
	{
		condVar = CreateEvent(NULL, false, false, NULL);
	}

	if((long)condVar == ERROR_ALREADY_EXISTS)
	{
		OmnAlarm << "CondVar initialization error. Name = " << name << " Errno = "
			<< OmnGetErrno() << enderr;
		return false;
	}

	return true;
}

inline void OmnCondVarSignal(OmnCondVarType &condVar)
{
    if(SetEvent(condVar) == FALSE)
	{
		OmnAlarm << "Failed to signal. Errno = " << OmnGetErrno() << enderr;
	}
}


inline bool OmnCondVarWait(OmnCondVarType &condVar, OmnMutex &theMutex)
{
	theMutex.unlock();
	if (WaitForSingleObject(condVar, INFINITE) == WAIT_FAILED)
	{
		OmnAlarm << "Failed on CondVar wait. Errno = " << (int)GetLastError() << enderr;
		return false;
	}
	return true;
}


inline bool OmnCondVarWait(OmnCondVarType &condVar, 
						   OmnMutex &theMutex,
						   bool &isTimeout,
						   const int timerMsec)
{
    theMutex.unlock();
	if (timerMsec == -1)
	{
		isTimeout = false;
		if(WaitForSingleObject(condVar, INFINITE) == WAIT_FAILED)
		{
			OmnAlarmProgError << "Failed on CondVar wait. Errno = " 
				<< (int)GetLastError() << enderr;
			return false;
		}
		return true;
	}
	else
	{
		int r = WaitForSingleObject(condVar, timerMsec);
		if (r == WAIT_FAILED)
		{
			OmnAlarmProgError << "Failed on CondVar wait. Errno = " 
				<< (int)GetLastError() << enderr;
			isTimeout = false;
			return false;
		}
		else
		{
			//
			// It is successful. Check whether it is timeout.
			//
			isTimeout = (r == WAIT_TIMEOUT);
			return true;
		}
	}
    theMutex.lock();
}

#endif
#endif



