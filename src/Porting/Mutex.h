////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Mutex.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Mutex_h
#define Omn_Porting_Mutex_h





#ifdef OMN_PLATFORM_UNIX



//
// The following was written for Unix. We will keep them commented
// so that when we need to port codes to Unix, we can reuse them.
//
//#include <synch.h>
//#define	OmnMutexType		mutex_t

//inline void OmnInitMutex(OmnMutexType &lock)
//{
//    mutex_init(&lock, USYNC_THREAD, 0);
//}



//inline void OmnMutexLock(OmnMutexType &lock)
//{
//	mutex_lock(&lock);
//}



//inline void OmnMutexUnlock(OmnMutexType &lock)
//{
//    mutex_unlock(&lock);
//}




//
// The following is the mutex implementation for Linux. 
// The include file for mutex is <pthread.h>
// The mutex type is pthread_mutex_t. In Linux implementation, there are
// three types of mutex (specified by mutex attributes: 
// 1. fast
// 2. recursive
// 3. error checking
// The default is "fast". See pthread_mutexattr_init(3) for more information.
// 



#include <pthread.h>



// #define OmnMutexType 	pthread_mutex_t
typedef pthread_mutex_t OmnMutexType;
typedef pthread_mutexattr_t OmnMutexAttr;

typedef pthread_mutex_t aos_lock_t;

inline OmnMutexAttr OmnInitMutexAttr(OmnMutexAttr &attr)
{
	pthread_mutexattr_init(&attr);
	return attr;
}

inline void OmnDestroyMutexAttr(OmnMutexAttr &attr)
{
	pthread_mutexattr_destroy(&attr);
}

inline bool OmnSetMutexAttrRecursive(OmnMutexAttr &attr)
{
	return pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != -1;
}

inline OmnMutexType OmnInitMutex(OmnMutexType &lock , OmnMutexAttr *attr = NULL)
{
	//
	// int pthread_mutex_init(pthread_mutex_t *mutex, 
	//			const pthread_mutexattr_t *mutexattr);
	// where "mutexattr" determines the type of the mutex. If
	// NULL is used, default attributes aer used. In our
	// implementation, we will use the default.
	// Currently, Linux supports only one mutex attribute: the
	// mutex kind, which is either PTHREAD_MUTEX_FAST_NP for 'fast',
	// PTHREAD_MUTEX_RECURSIVE_NP for 'recursive', and 
	// PTHREAD_MUTEX_ERRORCHECK_NP for 'error checking'. The
	// mutex kind determines what happens if a thread attempts to
	// lock a mutex it already owns with pthread_mutex_lock(3). 
	// If the mutex is of the 'fast' kind, pthread_mutex_lock(3)
	// simply suspends the calling thread forever. If the mutex is
	// of the 'error checking' kind, pthread_mutex_lock(3) returns 
	// immediately with teh error code EDEADLK. If the mutex
	// is of the 'recursive' kind, the call to pthread_mutex_lock(3)
	// returns immediately wtih a success return code. The number
	// of times the thread owning the mutex has locked it is 
	// reported in the mutex. The owning thread must call 
	// pthread_mutex_unlock(3) the same number of times before the 
	// mutex returns to the unlocked state.
	//
	// In our implementation, we will use 'fast', which is the default.
	//

	pthread_mutex_init(&lock, attr);
	return lock;
}


inline int OmnDestroyMutex(OmnMutexType &lock)
{
	return pthread_mutex_destroy(&lock);
}

// bool OmnMutexLock(OmnMutexType &lock);
inline bool OmnMutexLock(OmnMutexType &lock)
{
	return (pthread_mutex_lock(&lock) == 0);
}

inline bool OmnMutexTryLock(OmnMutexType &lock)
{
	return (pthread_mutex_trylock(&lock) == 0);
}

// void OmnMutexUnlock(OmnMutexType &lock);
inline bool OmnMutexUnlock(OmnMutexType &lock)
{
	return (pthread_mutex_unlock(&lock) == 0);
}


inline bool OmnMutexDestroy(OmnMutexType &lock)
{

	pthread_mutex_destroy(&lock);
	return true;
}



#elif defined(OMN_PLATFORM_MICROSOFT)
#include "Porting/GetErrno.h"
#include <winsock2.h>
#define OmnMutexType		HANDLE


inline OmnMutexType OmnInitMutex(OmnMutexType &lock)
{
    return lock = CreateMutex(NULL, FALSE, NULL);
}


inline int OmnDestroyMutex(OmnMutexType &lock)
{
	return CloseHandle(lock);
}


inline bool OmnMutexLock(OmnMutexType &lock)
{
    return (WaitForSingleObject(lock, INFINITE) != WAIT_FAILED);
}



inline bool OmnMutexUnlock(OmnMutexType &lock)
{
	
    if (!ReleaseMutex(lock))
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
#endif

#endif

