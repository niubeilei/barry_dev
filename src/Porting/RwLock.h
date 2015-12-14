////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RwLock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_RwLock_h
#define Omn_Porting_RwLock_h





#ifdef OMN_PLATFORM_UNIX


#include <pthread.h>



typedef pthread_rwlock_t OmnRwLockType;



inline OmnRwLockType OmnInitRwLock(OmnRwLockType &lock)
{
	pthread_rwlock_init(&lock, NULL);
	return lock;
}


inline int OmnDestroyRwLock(OmnRwLockType &lock)
{
	return pthread_rwlock_destroy(&lock);
}

// bool OmnRwLockLock(OmnRwLockType &lock);
inline bool OmnRwLockReadLock(OmnRwLockType &lock)
{
	return (pthread_rwlock_rdlock(&lock) == 0);
}

inline bool OmnRwLockWriteLock(OmnRwLockType &lock)
{
	return (pthread_rwlock_wrlock(&lock) == 0);
}

inline bool OmnRwLockTryReadLock(OmnRwLockType &lock)
{
	return (pthread_rwlock_tryrdlock(&lock) == 0);
}

inline bool OmnRwLockTryWriteLock(OmnRwLockType &lock)
{
	return (pthread_rwlock_trywrlock(&lock) == 0);
}

inline bool OmnRwLockUnLock(OmnRwLockType &lock)
{
	return (pthread_rwlock_unlock(&lock) == 0);
}

inline bool OmnRwLockDestroy(OmnRwLockType &lock)
{

	pthread_rwlock_destroy(&lock);
	return true;
}


#elif defined(OMN_PLATFORM_MICROSOFT)
#include "Porting/GetErrno.h"
#include <winsock2.h>
#define OmnRwLockType		HANDLE

/*
inline OmnRwLockType OmnInitRwLock(OmnRwLockType &lock)
{
    return lock = CreateRwLock(NULL, FALSE, NULL);
}


inline int OmnDestroyRwLock(OmnRwLockType &lock)
{
	return CloseHandle(lock);
}


inline bool OmnRwLockLock(OmnRwLockType &lock)
{
    return (WaitForSingleObject(lock, INFINITE) != WAIT_FAILED);
}



inline bool OmnRwLockUnlock(OmnRwLockType &lock)
{
	
    if (!ReleaseRwLock(lock))
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

