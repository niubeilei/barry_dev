////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 4/5/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "AccessManager/AmThread.h"

#include "AccessManager/AccessMgr.h"
#include "AmUtil/AmTrans.h"
#include "AmUtil/AmExternalSvr.h"
#include "AmUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"


AosAmThread::AosAmThread(const AosAccessMgrPtr &accessMgrPtr)
:
mLock(OmnNew OmnMutex()), 
mCondVar(OmnNew OmnCondVar()),
mAccessMgr(accessMgrPtr)
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "AmThread", 0, false, true, __FILE__, __LINE__);
	mThread->start();
}


AosAmThread::~AosAmThread()
{
	mThread->stop();
	while(1)
	{
		if(mThread->isStopped())
		{
			break;
		}
		else
		{
			sleep(1);
		}
	}
}


bool
AosAmThread::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosAmThreadPtr thisPtr(this, false);

    while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (!mTrans)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		mLock->unlock();

		//
		// Process the request
		//
		procTrans();
		mTrans = 0;

		mAccessMgr->returnThread(thisPtr);
	}

	return true;
}


bool    
AosAmThread::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool    
AosAmThread::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosAmThread::process(const AosAmTransPtr &trans)
{
	// mTrans must be 0 now!
	if(mTrans)
	{
		OmnAlarm << "AosAmThread resource conflict!" << enderr;
		return false;
	}

	mTrans = trans;
	// wake up the thread
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosAmThread::procTrans()
{
	// 
	// Process the transaction
	// 1. Parse the transaction
	// 2. determine Request Type
	// 3. Look up the Request Table
	//
	
	// 1. Parse the transaction
	AosAmMsgPtr msg = OmnNew AosAmMsg(*mTrans->getMsg());
	// msg->parse(); has already parsed when create the message
	if(!msg)
	{
		OmnAlarm << "Server not recieve message before processing." << enderr;
		return false;
	}
	msg->parse(); // may has already parsed when create the message
	AosAmTransPtr trans;
	int ret;

	// 2. determine Request Type
//	// AosAmRequestSvrTypeId::E requestTypeId;
//	u16 requestTypeId;
//
//	if(!msg->getTag(requestTypeId, (u8)AosAmTagId::eTransFuncId))
//	{
//		OmnAlarm << "Server failed to load process function pointer!" << enderr;
//		return false;
//	}

	// 2. get message Id
	u8 msgId = msg->getMsgId();
	OmnVList<AosAmExternalSvrPtr> server_array;

	// 3. load server array
	mAccessMgr->getMsgSvrArray(msgId, server_array);
	server_array.reset();
	while(server_array.entries() > 0)
	{
		trans = OmnNew AosAmTrans(mTrans);
		AosAmExternalSvrPtr server = server_array.popFront();

		// 4.1 Set each server information 
		trans->setExternalSvr(server);
		AosAmProcFunc transFunc = mAccessMgr->lookupTransFunc((AosAmRequestSvrTypeId::E)server->getServerTypeId());
		if(!transFunc)
		{
			OmnAlarm << "Server request process function not defined yet!" << enderr;
			return false;
		}

		OmnTrace << "Server request process tracing! " << server->getServerTypeId() << endl;
		// 4.2 Process each server request 
		ret = transFunc(trans, mAccessMgr);
		OmnTrace << "Server request process tracing! AFTER transfunction" << endl;
		if(ret == 0 && trans->isRespPositive())
		{
			break;
		}
	}

	if(trans)
	{
		mTrans = trans;
	}

/*
	// 3. Look up the Request Table
	AosAmProcFunc transFunc = mAccessMgr->lookupTransFunc((AosAmRequestSvrTypeId::E)requestTypeId);
	if(!transFunc)
	{
		OmnAlarm << "Server request process function not defined yet!" << enderr;
		return false;
	}

	// 4. Process the request 
	int ret;
	// 4.1 return 0 means success otherwise return value < 0 to represent the errercode
	ret = transFunc(mTrans, mAccessMgr);
 */


	// 5. put the response to the send queue
	if(mTrans->hasResponse())
	{
		mAccessMgr->sendResponse(mTrans);
	}
	mAccessMgr->returnThread(AosAmThreadPtr(this));

	return true;
}
#endif
