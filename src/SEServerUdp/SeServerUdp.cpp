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
// Modification History:
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEServerUdp/SeServerUdp.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"


OmnSingletonImpl(AosSeServerUdpSingleton,
                 AosSeServerUdp,
                 AosSeServerUdpSelf,
                "AosSeServerUdp");

static u64 sgReqId = 1000;

AosSeServerUdp::AosSeServerUdp()
:
mLock(OmnNew OmnMutex()),
mReqHead(0),
mProcIdx(-1)
{
}


AosSeServerUdp::~AosSeServerUdp()
{
}


bool
AosSeServerUdp::start()
{
	OmnCommListenerPtr thisptr(this, false);
	aos_assert_r(mComm, false);
	mComm->startReading(thisptr);

	OmnThreadedObjPtr pp(this, false);
	for (int i=0; i<mNumThreads; i++)
	{
	    OmnString name = "seserver_udp_";
	    name << i;
	    mThreads[i] = OmnNew OmnThread(thisPtr, name, i, true, true, __FILE__, __LINE__);
	}
	return true;
}


bool
AosSeServerUdp::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);

	AosXmlTagPtr tag = conf->getFirstChild("server_udp");
	aos_assert_r(tag, false);

	OmnString addr = tag->getAttrStr("local_addr");
	aos_assert_r(addr == "", false);
	OmnIpAddr ipaddr(addr);
	aos_assert_r(isaddr.isValid(), false);

	// Configure the request queue size.
	u32 queue_size = tag->getAttrU32("request_queue_size", eDftQueueSize);
	if (queue_size < eMinQueueSize) queue_size = eMinQueueSize;
	if (queue_size > eMaxQueueSize) queue_size = eMaxQueueSize;
	mRequests.resize(queue_size);

	int port = tag->getAttrInt("local_port", -1);
	aos_assert_r(port > 0, false);

	mComm = OmnNew OmnUdpComm(ipaddr, port, "seserver");

	return true;
}


bool
AosSeServerUdp::stop()
{
    return true;
}


bool
AosSeServerUdp::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosSeReqProcPtr req;
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mProcIdx != mReqHead)
		{
			// No more requests.
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		mProcIdx++;
		if (mProcIdx >= mMaxRequests) mProcIdx = 0;
		req = mRequests[mProcIdx];
		mLock->unlock();

		bool rslt = req->procRequest();

		// Need to move the following code to proc.
		if (!rslt)
		{
			req->sendResp();
			return true;
		}

		if (!req->needWait())
		{
			// Normally, 'req' should tell whether it has finished the processing.
			// But all the old requests did not set the finish flag. 
			// We use 'needWait()' to check. For all the old requests, it returns
			// false by default. If a request needs to wait, it should return true.
			req->sendResp();
			return true;
		}
	}

	OmnScreen << "Leaving SeServerUdp thread" << endl;
	return true;
}


bool		
AosSeServerUdp::msgRead(const OmnConnBuffPtr &buff)
{
	mLock->lock();
	u64 reqid = sgReqid++;
	AosSeReqProcPtr req = OmnNew AosSeReqProc(buff);
	req->setReqid(reqid);

	// Requests are added into a queue. The queue is implemented
	// as a fixed size array. 
	aos_assert_r(request, false);
	AosSeReqProcPtr old = mRequests[mReqHead];
	if (old && !old->isFinished())
	{
		logUnfinishedReq(old);
	}
	mRequests[mReqHead++] = request;
	if (mReqHead >= mMaxRequests) mReqHead = 0;

	mCondVar->signal();
	mLock->unlock();
	return true;
}


OmnString	
AosSeServerUdp::getCommListenerName() const
{
	return "seserver_udp";
}


void 		
AosSeServerUdp::readingFailed()
{
	return;
}


