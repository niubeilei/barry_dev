////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Comments:
// If one wants to have a thread to process something, one should
// derive a class from this class. The main function to override
// is "threadShellProc(...)". In this function, if it needs to 
// wait for receiving messages, it calls 
//
// Change History:
//	10/28/2004 - File Created
///////////////////////////////////////////////////////////////
#include "Thread/ThrdShellProc.h"

#include "Message/Req.h"
#include "Message/Resp.h"
#include "Thread/CondVar.h"
#include "Thread/ThrdTrans.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"



OmnThrdShellProc::OmnThrdShellProc(const OmnString &name)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mFinished(false),
mName(name)
{
	mPoison1 = 1234567890;
	mPoison2 = 2222222222;
	mPoison3 = 3333333333;
	mSem = 0;
}


OmnThrdShellProc::~OmnThrdShellProc()
{
}


bool
OmnThrdShellProc::isSuccessful() const
{
	if (!mTrans)
	{
		// The transaction wasn't started at all.
		return false;
	}

	return mTrans->isSuccess();
}


bool		
OmnThrdShellProc::waitUntilFinished()
{
	// 
	// This function will not return until the thread
	// finishes the processing. 
	//
	while (1)
	{
		mLock->lock();
		if (mFinished)
		{
			mLock->unlock();
			return true;
		}

		mCondVar->wait(mLock);
		mLock->unlock();
		continue;
	}

	return false;
}


bool
OmnThrdShellProc::threadFinished()
{
	// 
	// When the thread finished the processing, it should call
	// this function in case the caller is waiting for the
	// thread to finish (by calling "waitUntilFinished()"
	//
	mLock->lock();
	mCondVar->signal();
	mFinished = true;
	mLock->unlock();
	return true;
}
	

void 				
OmnThrdShellProc::setSem(const OmnSemPtr &sem)
{
	//cout << __FILE__ << ":" << __LINE__ << "=========== To set sem: " << sem.getPtr() << endl;
	mSem = sem;
}


void 				
OmnThrdShellProc::postSem()
{
	if (mSem) mSem->post();
}

