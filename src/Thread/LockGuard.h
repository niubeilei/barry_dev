////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	This class is a simple implementation of RAII(Resource Acquisition Is Initialization)
//
// Modification History:
// Dec 2, 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Thread_LockGuard_H_
#define Aos_Thread_LockGuard_H_

#define LockGuard(g,l)	AosLockGuard<OmnMutex>(g)((l))

template <typename LockType>
class AosLockGuard
{
private:
	LockType*	lock;
public:
	explicit AosLockGuard(LockType* l)
	:
	lock(l)
	{
		l->lock();
	}

	explicit AosLockGuard(LockType& l)
	:
	lock(l)
	{
		l.lock();
	}

	virtual ~AosLockGuard()
	{
		lock->unlock();
	}
};

#endif /* Aos_Thread_LockGuard_H_ */
