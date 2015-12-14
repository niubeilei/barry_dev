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
// This class is used as the Data Sync Client. When a requester has something
// that needs to be synched with a remote site, it calls its function. 
//
// To ensure the reliability, each transaction must be responded. Otherwise, 
// it may re-send the request. On the receiving side, it should ignore the
// re-transmitted requests. 
//
// A client may connect to multiple sites. It may sync with one or more
// remote sites. If one site fails, it may sync with the backup.
//
// Modification History:
// 10/12/2010 Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataSync/DataSyncClt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataSync/DsTrans.h"
#include "HealthCheck/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h" 
#include "Thread/CondVar.h" 
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCommSvr.h"
#include "UtilComm/CommTypes.h"
#include "Util1/Wait.h"
#include "Util/File.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Porting/Sleep.h"
#include "Thread/ThreadMgr.h"
#include "TransUtil/TransFileMgr.h"
#include "SeSysLog/SeSysLog.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


static u64 sgSentTime = 0;

OmnSingletonImpl(AosDataSyncCltSingleton,
				 AosDataSyncClt,
				 AosDataSyncCltSelf,
				 "AosDataSyncClt");


AosDataSyncClt::AosDataSyncClt()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mFinishFlag(true),
mStopPush(false),
mLogid(0),
mTransFile(0),
mClt(0),
mTid(0)
{
}


AosDataSyncClt::~AosDataSyncClt()
{
	if (mReqThread) mReqThread->stop();
	OmnThreadMgr::getSelf()->removeThread(mReqThread);
	mReqThread = 0;

	if (mMonitorThread) mMonitorThread->stop();
	OmnThreadMgr::getSelf()->removeThread(mMonitorThread);
	mMonitorThread = 0;
}


//API
bool
AosDataSyncClt::start(const AosXmlTagPtr &def)
{
	if(NULL == def)
	{
		OmnAlarm << "DataSync's config is not find!" << enderr;
		return false;
	}
	mClientId = def->getAttrStr(AOSMSG_CLIENTID);
	aos_assert_r(mClientId != "", false);

	mAddr = def->getAttrStr(AOSMSG_ADDR);
	aos_assert_r(mAddr!="", false);
	
	mPort = def->getAttrInt(AOSMSG_PORT, -1);
	aos_assert_r(mPort > 0, false);
	
	aos_assert_r(createConnection(), false);
	mTransFile = AosSeSysLog::getTransFile();
	aos_assert_r(mTransFile, false);

	OmnThreadedObjPtr thisPtr(this, false);
	mReqThread = OmnNew OmnThread(thisPtr, "ds", 0, true, true, __FILE__, __LINE__);
	mMonitorThread = OmnNew OmnThread(thisPtr, "ds", 1, true, true, __FILE__, __LINE__);
	
	return true;
}


bool    
AosDataSyncClt::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	switch (thread->getLogicId())
	{
	case 0:
		 return reqThreadFunc(state, thread);

	case 1:
		 return monitorThreadFunc(state, thread);

	default:
		 OmnAlarm << "Incorrect thread id: " << thread->getLogicId() << enderr;
		 return false;
	}

	return false;
}


bool
AosDataSyncClt::reqThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnString request;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mReqQueue.empty() || !mFinishFlag)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		mFinishFlag = false;
		request	 = mReqQueue.front();
		mReqQueue.pop();
		aos_assert_rl(syncData(request), mLock, false);
		mLock->unlock();
	}
	return true;
}


bool
AosDataSyncClt::monitorThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		u64 time = OmnGetSecond();
		if (time-sgSentTime > eRespTimer && sgSentTime != 0 && mTran)
		{
			// This means an oustanding request failed receiving response. 
			// This can be the case of failing sending the request, or 
			// the remote site failed responding. 
			// Will resend.
			u32 logid;
			sgSentTime = OmnGetSecond();
			aos_assert_rl(mTran->sendRequest(logid), mLock, false);
		}
		else
		{
			if (mReqQueue.empty() && !mTran && mFinishFlag)
			{
				u32 logid;
				map<u32, OmnString> trans;
				mTransFile->recoverTrans(trans, logid, mTid);
				OmnScreen << "*****Read_LogFile*****" << "logid:" << logid << " mTid:" << mTid << endl;
				map<u32, OmnString>::iterator item = trans.begin();
				mStopPush = false;
				while (item!= trans.end())
				{
					addRequest(item->second);
					item++;
				}
				trans.clear();
			}
		}
		mLock->unlock();
		OmnWait::getSelf()->wait(eMonitorFreqSec, 0);
	}
	return true;
}


bool
AosDataSyncClt::createConnection()
{
	OmnString errmsg;
	mConn = OmnNew OmnTcpCommSvr(OmnIpAddr(mAddr), mPort, eNumPorts, "DataSyncServer");
	if (!mConn->connect(errmsg))
	{
		OmnAlarm << "Fail To start datasync_server" << enderr;
		return false;
	}

	OmnTcpListenerPtr thisPtr(this, false);
	mConn->startReading(thisPtr);
	
	return true;
}


bool
AosDataSyncClt::syncData(const OmnString &request)
{
	if (mTran)
	{
		OmnAlarm << "Transaction not null" << enderr;
	}
	
	AosDataSyncCltPtr thisPtr(this, false);
	mTran = OmnNew AosDsTrans(thisPtr, request);
	aos_assert_r(mTran, false);

	u32 id = mLogid;
	sgSentTime = OmnGetSecond();
	mTran->sendRequest(mLogid);

	if(id != mLogid-1 && id !=0)
	{
		OmnAlarm << "******Error****id:" << id << " mLogid:" << mLogid << enderr;
		return false;
	}
	return true;
}


bool    
AosDataSyncClt::signal(const int threadLogicId)
{
	return true;
}


bool    
AosDataSyncClt::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosDataSyncClt::procMsg(const OmnConnBuffPtr &buff)
{
	aos_assert_r(buff, false);
	OmnString bf = buff->getString();
	aos_assert_r(bf.length(), false);

	OmnString resp = "<response>";
	resp << buff->getData() << "</response>";
	OmnString rep = "<response></response>";
	if(strcmp(resp, rep)==0)
	{
		OmnAlarm << "buff is empty" << enderr;
		return false;
	}
	
	AosXmlParser parser;
	AosXmlTagPtr msg = parser.parse(resp, "" AosMemoryCheckerArgs);
	AosXmlTagPtr child;
	if (!msg || !(child = msg->getFirstChild()))
	{
		OmnAlarm << "Failed to parse the request!***" << resp << enderr;
		return false;
	}

	return procResp(msg);
}


bool
AosDataSyncClt::procResp(const AosXmlTagPtr &msg)
{
	mLock->lock();
	
	AosXmlTagPtr Opr = msg->getFirstChild(AOSMSG_OPR);
	if (NULL == Opr)
	{
		OmnAlarm << "Fail To Find Opr***" << msg->toString() << enderr;
		mLock->unlock();
		return false;
	}

	OmnString opr_str = Opr->getAttrStr(AOSMSG_TYPE);
	if (opr_str == "Link")
	{
		OmnString req = "<request ";
		req << AOSMSG_CLIENTID << "=\"" << mClientId << "\" type=\"Connection\">"
			<< "</request>";
		mClt->smartSend(req.data(), req.length());
		
		beginSync();
		mLock->unlock();
		return true;
	}

	if (opr_str == "Result")
	{
		AosXmlTagPtr contents = msg->getFirstChild(AOSMSG_CONTENTS);
		if(NULL != contents)
		{
			u64 trans_id = contents->getAttrU64(AOSMSG_TRANSID, 0);
			aos_assert_r(trans_id > 0, false);

			u32 logid = contents->getAttrU32("logid", 0);
			if(logid == mLogid)
			{
				OmnScreen << "~~~have_finished_logid~~~~" << logid << endl;
				mTransFile->finishEntry(logid);
				mFinishFlag = true;
				mTran = 0;
				mTid = logid;
				mCondVar->signal();
			}
		}
	}

	mLock->unlock();
	return true;
}


OmnTcpClientPtr
AosDataSyncClt::getConn()
{
	return mClt;
}


bool
AosDataSyncClt::addRequest(const OmnString &data)
{
	// The request data is 'data'. It creates a request and adds
	// the request to the queue.
	if(mStopPush)
	{
		return true;
	}

	OmnString req = "<request ";
	req << AOSMSG_CLIENTID << "=\"" << mClientId << "\">"
		<< "<objdef>" << data << "</objdef></request>";

	mReqQueue.push(req);
	
	if(mReqQueue.size() > eQueueLength)
	{
		mStopPush = true;
	}
	
	if (mReqThread->getStatus() != OmnThrdStatus::eActive)
	{
		mReqThread->start();
	}

	if (mMonitorThread->getStatus() != OmnThrdStatus::eActive)
	{
		mMonitorThread->start();
	}

	mCondVar->signal();
	return true;
}


bool
AosDataSyncClt::beginSync()
{
	if (!mReqQueue.empty() || !mFinishFlag)
	{
		return true;
	}

	OmnScreen << "********Start DataSync************" << endl;
	u32 logid;
	map<u32, OmnString> trans;
	mTransFile->recoverTrans(trans, logid, mTid);
	map<u32, OmnString>::iterator item = trans.begin();
	mStopPush = false;

	while (item!= trans.end())
	{
		addRequest(item->second);
		item++;
	}
	trans.clear();

	if (mReqThread->getStatus() != OmnThrdStatus::eActive)
	{
		mReqThread->start();
	}

	if (mMonitorThread->getStatus() != OmnThrdStatus::eActive)
	{
		mMonitorThread->start();
	}

	return true;
}


void
AosDataSyncClt::msgRecved(const OmnConnBuffPtr &buff,
		const OmnTcpClientPtr &client)
{
	aos_assert(client);

	mClt = client;
	aos_assert(procMsg(buff));
	
	return;
}
#endif
