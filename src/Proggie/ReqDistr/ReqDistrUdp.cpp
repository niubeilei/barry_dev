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
// This class uses a UDP Comm to accepts network traffic. It starts 
// a number of threads. When a message is received, it puts the message
// into the queue and returns. 
//
// One of the thread picks up the message and processes it. All function
// calls are asynchronous.
//   
//
// Modification History:
// 2014/12/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Proggie/ReqDistr/ReqDistrUdp.h"

#include "alarm_c/alarm.h"
// #include "Packages/Frontend/Frontend.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqProcThrd.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ReqDistr/Ptrs.h"
#include "Proggie/ProggieUtil/TcpRequest.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "TinyXml/TinyXml.h"
#include "Util/OmnNew.h"
#include "util_c/strutil.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/CommListener.h"
#include "XmlUtil/XmlTag.h"



AosReqDistrUdp::AosReqDistrUdp(
		const int num_threads, 
		const AosNetReqProcPtr &proc)
:
mNumThreads(num_threads),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mProcessor(proc),
mIsBlocking(false),
mMaxQueueSize(eDftMaxQueueSize)
{
}


AosReqDistrUdp::~AosReqDistrUdp()
{
	OmnDelete mLock;
	OmnDelete mCondVar;
}


bool
AosReqDistrUdp::config(
		const OmnString &local_addr, 
		const OmnString &local_port,
		const OmnString &server_name,
		const OmnString &deli,
		const int max_proc_time,
		const int max_conns)
{
	// aos_assert_r(mLocalAddrDef.config(local_addr, "all"), false);
	// aos_assert_r(mLocalPorts.config(local_port, -1, -1), false);
	mServiceName = server_name;
	mMaxProcTime = max_proc_time;
	return true;
}


bool
AosReqDistrUdp::config(const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	// 'def' should be in the form:
	// 	<config local_addr="all|loopback|interface|addr"
	// 		local_port="nnn|$|nnn|$|..."
	// 		service_rcd="true|false"
	// 		service_name="xxx"
	// 		support_actions="true|false"
	// 		req_delinerator="newline|first_word_high|first_word_low|zero_term|xml"
	// 		max_proc_time="0|nnn"
	// 		max_conns="500|nnn">
	// 	</config>
	// aos_assert_r(mLocalAddrDef.config(def->getAttrStr("local_addr"), "all"), false);
	// aos_assert_r(mLocalPorts.config(def->getAttrStr("local_port"), -1, -1), false);
	mIsBlocking = def->getAttrBool("is_blocking", false);
	mServiceName = def->getAttrStr("service_name");
	mReqDelinerator = AosConvertTcpLenType(def->getAttrStr("req_delinerator"), eAosTLT_Xml);
	mMaxProcTime = def->getAttrInt("max_proc_time", 0);
	mNumThreads = def->getAttrInt("num_threads", 10);	
	aos_assert_r(init(), false);
	return true;
}


bool
AosReqDistrUdp::init()
{
	for (int i=0; i<mNumThreads; i++)
	{
		mFrontends[i] = OmnNew AosFrontend(this, mProcessor->clone(), i);
	}
	return true;
}


bool
AosReqDistrUdp::start()
{
	mServer = OmnNew OmnUdpComm(mLocalAddr, mLocalPort, "ReqDistrUdp");
	OmnCommListenerPtr thisptr(this, false);
	mServer->startReading(thisptr);
	return true;
}


bool
AosReqDistrUdp::stop()
{
	aos_assert_r(mServer, false);
	aos_assert_r(mThread, false);
	return true;
}


OmnString	
AosReqDistrUdp::getCommListenerName() const
{
	return "ReqDistrUdp";
}


void 		
AosReqDistrUdp::readingFailed()
{
}


bool		
AosReqDistrUdp::msgRead(const OmnConnBuffPtr &buff)
{
	aos_assert_r(buff, false);
	mLock->lock();
	while (mQueue.size() > mMaxQueueSize)
	{
		mCondVar->wait(mLock);
		continue;
	}

	mQueue.push(buff);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


OmnConnBuffPtr
AosReqDistrUdp::getRequest(const int index)
{
	// This function should be called by a thread. It tries to get a
	// request from the queue. If the queue is empty, it will block
	// until there is one.
	OmnConnBuffPtr req;
	while (1)
	{
		mLock->lock();
		if (mQueue.size() <= 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		req = mQueue.front();
		mQueue.pop();
		mLock->unlock();
		return req;
	}
}


bool
AosReqDistrUdp::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->broadcastSignal();
	mLock->unlock();
	return true;
}

/*
bool
AosReqDistrUdp::checkThread(OmnString &errmsg, const int thrdLogicId) const
{
	return mThreadStatus;
}
*/
#endif
