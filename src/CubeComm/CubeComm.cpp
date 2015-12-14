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
// Each CubeComm is created by one JimoCaller. When it receives a message,
// it should inform the dialer. 
//
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CubeComm/CubeComm.h"


AosCubeComm::AosCubeComm(
		const OmnString &name, 
		const OmnCommListenerPtr &caller)
:
mName(name),
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mCondVar(OmnNew OmnCondVar()),
mCondVarRaw(mCondVar.getPtr())
{
	//thread
	mCaller = caller;
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "CubeCommThread", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosCubeComm::~AosCubeComm()
{
}


bool
AosCubeComm::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnConnBuffPtr connbuff;
    while (state == OmnThrdStatus::eActive)
    {
		mLockRaw->lock();
		if (mQueue.size() <= 0)
		{
			mCondVarRaw->wait(*mLockRaw);
			mLockRaw->unlock();
			continue;
		}

		connbuff = mQueue.front();
		mQueue.pop();
		mLockRaw->unlock();
		proc(connbuff);
	}

	return true;
}


bool		
AosCubeComm::msgRead(const OmnConnBuffPtr &connbuff)
{
	aos_assert_r(mCaller, false);

	mLockRaw->lock();
	mQueue.push(connbuff);
	mCondVar->signal();
	mLockRaw->unlock();
	return true;
}


OmnString	
AosCubeComm::getCommListenerName() const
{
	return mName;
}


void 		
AosCubeComm::readingFailed()
{
	return;
}


void
AosCubeComm::sendingFailed()
{
	return;
}


bool	
AosCubeComm::signal(const int threadLogicId)
{
	return true;
}

