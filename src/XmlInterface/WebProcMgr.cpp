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
//
// Modification History:
// 	03/03/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/WebProcMgr.h"

#include "alarm_c/alarm.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpServerGrp.h"
#include "XmlInterface/WebProcThread.h"
#include "XmlInterface/WebProcReq.h"




AosWebProcMgr::AosWebProcMgr(
		const OmnString &addr, 
		const int port, 
		const int numPorts)
:
mLocalAddr(OmnIpAddr(addr)), 
mLocalPort(port),
mNumPorts(numPorts),
mTotalThreads(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	for (int i=0; i<eNumThreads; i++)
	{
		mProcThreads[i] = OmnNew AosWebProcThread();
	}
}


AosWebProcMgr::~AosWebProcMgr()
{
}


bool
AosWebProcMgr::start()
{
	mServer = OmnNew OmnTcpServerGrp(mLocalAddr, mLocalPort, mNumPorts, 
			"WebProcMgr", eMaxConns, eAosTLT_FirstWordHigh);
	OmnString errmsg;
	if (!mServer->connect(errmsg))
	{
		OmnAlarm << "Failed to connect. Check the local address: "
			<< errmsg << ". Local address: "
			<< mLocalAddr.toString() << ":" << mLocalPort << enderr;
		return false;
	}

	OmnTcpCltGrpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
	mServer->startReading();
	return true;
}


bool
AosWebProcMgr::stop()
{
	aos_assert_r(mServer, false);
	aos_assert_r(mThread, false);
	mServer->stopReading();
	mThread->stop();
	return true;
}


OmnString   
AosWebProcMgr::getTcpListenerName() const
{
	return "WebProcMgr";
}


void        
AosWebProcMgr::msgRecved(
		const OmnTcpCltGrpPtr &group,
		const OmnConnBuffPtr &buff,
		const OmnTcpClientPtr &conn)
{
	aos_assert(buff);
	aos_assert(conn);

OmnTrace << "!!!!!!!!MsgRecved: " << buff->getData() << endl;
	AosWebProcReqPtr req = OmnNew AosWebProcReq(conn, buff);
	aos_assert(req);
	mLock->lock();
	mRequests.push_back(req);
	mCondVar->signal();
	mLock->unlock();
	return;
}


/*
void
AosWebProcMgr::returnThread(const AosWebProcThreadPtr &thread)
{
	mLock->lock();
	mIdleThreads.push_back(thread);
	mCondVar->signal();
	mLock->unlock();
	return;
}
*/


void        
AosWebProcMgr::newConn(const OmnTcpClientPtr &conn)
{
}


bool        
AosWebProcMgr::connClosed(
		const OmnTcpCltGrpPtr &,
		const OmnTcpClientPtr &client)
{
	return true;
}


AosWebProcReqPtr 
AosWebProcMgr::getRequest()
{
	AosWebProcReqPtr req;
	mLock->lock();
	while (1)
	{
OmnTrace << "To get request" << endl;
		if (mRequests.size() == 0)
		{
OmnTrace << "To wait!" << endl;
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		req = mRequests.front();
		mRequests.pop_front();
		mLock->unlock();
OmnTrace << "Got it" << endl;
		return req;
	}
}


bool
AosWebProcMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thrd)
{
	/*
	AosWebProcThreadPtr thread;
	AosWebProcReqPtr req;
	AosWebProcMgrPtr thisPtr(this, false);
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		mThreadStatus = true;
		thread = 0;

		// Check whether there is any pending request. If not, wait.
		if (mRequests.size() == 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		// There are some pending requests. Check whether there are idle threads.
		if (mIdleThreads.size() == 0)
		{
			// No idle threads. Check whether we can create more. 
			if (mTotalThreads >= eMaxNumThreads)
			{
				// Cannot create more. Need to wait until some threads finish
				// their current processing. 
				mCondVar->wait(mLock);
				mLock->unlock();
				continue;
			}

			// We can create more threads. 
			thread = OmnNew AosWebProcThread();
			if (!thread)
			{
				OmnAlarm << "Server run out of memory!" << enderr;
				mCondVar->wait(mLock);
				mLock->unlock();
				continue;
			}
			mTotalThreads++;
		}

		req = mRequests.front();
		mRequests.pop_front();
		if (!thread) 
		{
			thread = mIdleThreads.front();
			mIdleThreads.front();
		}
		mLock->unlock();

		thread->procReq(req, thisPtr);
	}
	*/

	return true;
}


bool
AosWebProcMgr::signal(const int threadLogicId)
{
	mLock->lock();
	mThreadStatus = false;
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosWebProcMgr::checkThread(OmnString &errmsg, const int thrdLogicId) const
{
	return mThreadStatus;
}

