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
#include "Proggie/ReqDistr/ReqDistr.h"

#include "alarm_c/alarm.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqProcThrd.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ReqDistr/ConnListener.h"
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
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpServerEpoll.h"
#include "UtilComm/TcpListener.h"
#include "XmlUtil/XmlTag.h"



AosReqDistr::AosReqDistr(const AosNetReqProcPtr &proc)
:
mNumThreads(eInitNumThreads),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mProcessor(proc),
mIsBlocking(false)
{
}


/*
bool
AosReqDistr::config(const OmnString &def)
{
	// This constructor takes the configuration 'def' to configure this
	// instance. 'def' must be an XML tag. 
	TiXmlDocument thedoc;
	thedoc.Parse(def);
	TiXmlNode *root = thedoc.FirstChild();
	aos_assert_r(root, false);
	TiXmlElement *ee = root->ToElement();
	aos_assert_r(ee, false);
	aos_assert_r(config(ee), false);
	aos_assert_r(init(), false);
	return true;
}
*/


AosReqDistr::~AosReqDistr()
{
}


/*
 * Chen Ding, 02/11/2012
bool
AosReqDistr::config(TiXmlElement *def)
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
	aos_assert_r(mLocalAddrDef.config(def->Attribute("local_addr"), "all"), false);
	aos_assert_r(mLocalPorts.config(def->Attribute("local_port"), -1, -1), false);
	
	//ken 2011/12/21
	//mServiceRcd = aos_a2bool(def->Attribute("service_rcd"), true);
	//mSupportActions = aos_a2bool(def->Attribute("support_actions"), true);
	mServiceName = def->Attribute("service_name");
	mReqDelinerator = AosConvertTcpLenType(def->Attribute("req_delinerator"), eAosTLT_Xml);
	mMaxProcTime = aos_atoi_dft(def->Attribute("max_proc_time"), 0);
	mMaxConns = aos_atoi_dft(def->Attribute("max_conns"), eDftMaxConns);
	return true;
}
*/


bool
AosReqDistr::config(
		const OmnString &local_addr, 
		const OmnString &local_port,
		const OmnString &server_name,
		const OmnString &deli,
		const int max_proc_time,
		const int max_conns)
{
	aos_assert_r(mLocalAddrDef.config(local_addr, "all"), false);
	aos_assert_r(mLocalPorts.config(local_port, -1, -1), false);
	mServiceName = server_name;
	mReqDelinerator = AosConvertTcpLenType(deli, eAosTLT_Xml);
	mMaxProcTime = max_proc_time;
	mMaxConns = max_conns;
	mNumThreads = 10;
	aos_assert_r(init(), false);
	return true;
}


bool
AosReqDistr::config(const AosXmlTagPtr &def)
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
	aos_assert_r(mLocalAddrDef.config(def->getAttrStr("local_addr"), "all"), false);
	aos_assert_r(mLocalPorts.config(def->getAttrStr("local_port"), -1, -1), false);
	mIsBlocking = def->getAttrBool("is_blocking", false);
	mServiceName = def->getAttrStr("service_name");
	mReqDelinerator = AosConvertTcpLenType(def->getAttrStr("req_delinerator"), eAosTLT_Xml);
	mMaxProcTime = def->getAttrInt("max_proc_time", 0);
	mMaxConns = def->getAttrInt("max_conns", eDftMaxConns);
	mNumThreads = def->getAttrInt("num_threads", 10);	
	aos_assert_r(init(), false);
	return true;
}


bool
AosReqDistr::init()
{
	AosReqDistrPtr thisPtr(this, false);
	for (int i=0; i<mNumThreads; i++)
	{
		mProcThreads[i] = OmnNew AosReqProcThrd(thisPtr, mProcessor->clone(), i);
	}
	return true;
}


bool
AosReqDistr::start()
{
	mServer = OmnNew OmnTcpServerEpoll(mLocalAddrDef, mLocalPorts,
			mServiceName, mReqDelinerator, mMaxConns);
	mServer->setBlocking(mIsBlocking);
	OmnTcpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
	mServer->startReading();
	return true;
}


bool
AosReqDistr::stop()
{
	aos_assert_r(mServer, false);
	aos_assert_r(mThread, false);
	mServer->stopReading();
	mThread->stop();
	return true;
}


OmnString   
AosReqDistr::getTcpListenerName() const
{
	return "WebProcMgr";
}


void        
AosReqDistr::msgRecved(
		const OmnConnBuffPtr &buff,
		const OmnTcpClientPtr &conn)
{
	aos_assert(buff);
	aos_assert(conn);

	buff->setConn(conn);
	mLock->lock();
	if (!mReqTail)
	{
		buff->setPrev(0);
		buff->setNext(0);
		mReqHead = mReqTail = buff;
	}
	else
	{
		if (!mReqHead)
		{
			OmnAlarm << "Internal error: " << enderr;
			mReqHead = mReqTail;
		}
		mReqTail->setNext(buff);
		buff->setPrev(mReqTail);
		buff->setNext(0);
		mReqTail = buff;
	}
	mCondVar->signal();
	mLock->unlock();
	return;
}


OmnConnBuffPtr
AosReqDistr::getRequest(const int index)
{
	// This function should be called by a thread. It tries to get a
	// request from the queue. If the queue is empty, it will block
	// until there is one.
	OmnConnBuffPtr req;
	while (1)
	{
		// if (mRequests.size() == 0)
		mLock->lock();
		if (!mReqHead)
		{
			mCondVar->wait(mLock);

			//felicia, 2013/06/19
			aos_assert_rl(index < mNumThreads, mLock, 0);

			//jozhi 2014/11/10 may be this thread no started
			if (!mProcThreads[index])
			{
				mLock->unlock();
				continue;
			}
			mProcThreads[index]->setThreadStatus();
			mLock->unlock();
			continue;
		}

		// req = mRequests.front();
		// mRequests.pop_front();
		req = mReqHead;
//		req->setNext(0);
		if (mReqHead == mReqTail)
		{
			mReqHead = 0;
			mReqTail = 0;
		}
		else
		{
			mReqHead = mReqHead->getNext();
			if (mReqHead)
			{
				mReqHead->setPrev(0);
			}
			else
			{
				OmnShouldNeverComeHere;
				mReqTail = 0;
			}
		}
		req->setNext(0);
		mLock->unlock();
		return req;
	}
}


bool
AosReqDistr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thrd)
{
	// This is the monitoring thead. 
	while (state == OmnThrdStatus::eActive)
	{
		// Currently do nothing.
		mThreadStatus = true;
		OmnSleep(1);
	}

	return true;
}


bool
AosReqDistr::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->broadcastSignal();
	mLock->unlock();
	return true;
}

/*
bool
AosReqDistr::checkThread(OmnString &errmsg, const int thrdLogicId) const
{
	return mThreadStatus;
}
*/
