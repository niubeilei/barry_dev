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

#ifndef Omn_Thread_CondVar_h
#define Omn_Thread_CondVar_h

#include "Porting/CondVar.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/SPtr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class OmnMutex;

class OmnCondVar : public OmnRCObject
{
	OmnDefineRCObject;

    OmnCondVarType		mCondVar;
    OmnString			mName;

    //
	// Do not use the following
    //
    OmnCondVar & operator = (const OmnCondVar &);
    OmnCondVar(const OmnCondVar &);

public:
	OmnCondVar() {init();}
    OmnCondVar(const OmnString &name)
		:
	mName(name)
	{
		init();
	}
	~OmnCondVar() {}

	inline bool wait(OmnMutex &mutex)
	{
		return OmnCondVarWait(mCondVar, mutex);
	}

	inline bool wait(const OmnMutexPtr &mutex)
	{
		return OmnCondVarWait(mCondVar, *(mutex.getPtr()));
	}

	inline int timedWait(
			OmnMutex &mutex, 
			bool &isTimeout, 
			const int sec, 
			const int usec = 0)
	{
		isTimeout = false;
		if (sec == -1) return OmnCondVarWait(mCondVar, mutex);

		return OmnCondVarWait(mCondVar, mutex, isTimeout, sec, usec);
	}

	inline int timedWait(
			const OmnMutexPtr &mutex, 
			bool &isTimeout, 
			const int sec, 
			const int usec = 0)
	{
		// If timer is -1, it will wait until something is received. 
		// If it is a positive integer, it means to wait until either something
		// has been received, or the timer expires (in miniseconds). 
		isTimeout = false;
		if (sec == -1) return OmnCondVarWait(mCondVar, *(mutex.getPtr()));

		return OmnCondVarWait(mCondVar, *(mutex.getPtr()), isTimeout, sec, usec);
	}

    inline void signal()
	{
		OmnCondVarSignal(mCondVar);
	}

	inline void broadcastSignal()
	{
		OmnCondVarBroadSignal(mCondVar);
	}

    inline OmnString getName() const {return mName;}

private:
	inline void	init()
	{
		OmnCondVarInit(mCondVar, mName);
	}
};
#endif
