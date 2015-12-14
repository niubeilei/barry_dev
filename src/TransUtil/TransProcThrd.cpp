////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/23/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/TransProcThrd.h"

#include "SEInterfaces/TransSvrObj.h"
#include "TransBasic/Trans.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"


AosTransProcThrd::AosTransProcThrd(const OmnString &name, const u32 num_thrd)
:
mName(name),
mNumThrds(num_thrd)
{
	for (u32 i = 0; i < mNumThrds; i++)
	{
		mLock[i] = OmnNew OmnMutex();
		mCondVar[i] = OmnNew OmnCondVar();

		OmnThreadedObjPtr thisptr(this, false);
		mThread[i] = OmnNew OmnThread(thisptr, "transproc", i, false, true, __FILE__, __LINE__);
		mThread[i]->start();
	}
}


AosTransProcThrd::~AosTransProcThrd()
{
}


bool
AosTransProcThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		u32 logicid = thread->getLogicId();
		mLock[logicid]->lock();
		if (mQueue[logicid].empty())
		{
			mCondVar[logicid]->wait(mLock[logicid]);
			mLock[logicid]->unlock();
			continue;
		}

		AosTransPtr trans = mQueue[logicid].front();
		mLock[logicid]->unlock();

		//OmnScreen << "ProcThrd proc!! trans_id:" << trans->getTransId().toString() << endl; 
		trans->proc();

		mLock[logicid]->lock();
		mQueue[logicid].pop(); 
		mLock[logicid]->unlock();
		AosTransSvrObj::getTransSvr()->resetCrtCacheSize(trans->getSize());
	}
	return true;
}


bool
AosTransProcThrd::signal(const int threadLogicId)
{
	return true;
}


bool
AosTransProcThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosTransProcThrd::addRequest(
		const u32 idx,
		const AosTransPtr &trans)
{
	aos_assert_r(idx < mNumThrds, false);

	mLock[idx]->lock();
	mQueue[idx].push(trans);	
	mCondVar[idx]->signal();
	mLock[idx]->unlock();
	return true;
}
