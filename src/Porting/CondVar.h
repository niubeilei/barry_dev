////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CondVar.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_CondVar_h
#define Omn_Porting_CondVar_h

#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/GetErrno.h"
#include "Porting/Mutex.h"
#include "Porting/TimeOfDay.h"
#include "Thread/Mutex.h"



#ifdef OMN_PLATFORM_UNIX

//#include <synch.h>
#include <pthread.h>
#define  OmnCondVarType pthread_cond_t	

inline void OmnCondVarInit(OmnCondVarType &condVar, const OmnString &name)
{
    //cond_init(&mCondVar, USYNC_THREAD, 0);
    pthread_cond_init(&condVar, 0);
}

inline bool OmnCondVarWait(OmnCondVarType &condVar, 
						   OmnMutex &theMutex)
{
    //cond_wait(&condVar, &theMutex.getLock());
    pthread_cond_wait(&condVar, &theMutex.getLock());
	return true;
}

inline void OmnCondVarSignal(OmnCondVarType &condVar)
{    
	//cond_signal(&condVar);
	pthread_cond_signal(&condVar);
}


inline void OmnCondVarBroadSignal(OmnCondVarType &condVar)
{    
	//cond_broadcast(&condVar);
	pthread_cond_broadcast(&condVar);
}


inline int OmnCondVarWait(OmnCondVarType &condVar, 
						   OmnMutex &theMutex,
						   bool &isTimeout,
						   const int sec, 
						   const int usec)
{
	timeval crtTime;
	OmnGetTimeOfDay(&crtTime);
	timespec t;

	int ss = sec;
	crtTime.tv_usec += usec;
	if (crtTime.tv_usec > 1000000)
	{
		ss++;
		crtTime.tv_usec -= 1000000;
	}

	t.tv_nsec = crtTime.tv_usec*1000;
	t.tv_sec = crtTime.tv_sec + ss;

	// cout << __FILE__ << ":" << __LINE__ << ":-----" << t.tv_sec - crtTime.tv_sec 
	// 		<< ":" << t.tv_nsec - crtTime.tv_usec << ":" << sec << ":" << usec << endl;

	int ret = pthread_cond_timedwait(&condVar, &theMutex.getLock(), &t);
	isTimeout = (ret == ETIMEDOUT);
	if (ret == EINVAL)
	{
	    cout << "=============================" << endl;
		cout << " crt_sec:" << crtTime.tv_sec << endl;
		cout << " crt_usec:" << crtTime.tv_usec << endl;
		cout << " sec:" << sec << endl;
		cout << " usec:" << usec << endl;
		cout << " t.tv_sec:" << t.tv_sec << endl;
		cout << " t.tv_nsec:" << t.tv_nsec << endl;
	    cout << "=============================" << endl;
	}
	return ret;
}













//
// Microsoft Portion
//
#elif OMN_PLATFORM_MICROSOFT
#include "Alarm/Alarm.h"
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



