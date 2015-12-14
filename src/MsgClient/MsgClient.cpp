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
// This is a utility to select docs.
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "MsgClient/MsgClient.h"

#include "Util/Buff.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "JobTrans/ProcMsgTrans.h"
#include "SysMsg/StartDynamicProcMsg.h"
#include "Porting/Sleep.h"
#include "MsgService/MsgService.h"
#include "API/AosApi.h"

OmnSingletonImpl(AosMsgClientSingleton,
                 AosMsgClient,
                 AosMsgClientSelf,
                 "AosMsgClient");


AosMsgClient::AosMsgClient()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mIsStart(false)
{
}


AosMsgClient::~AosMsgClient()
{
}

bool
AosMsgClient::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr msg_config = config->getFirstChild("msgclient");
	if (msg_config)
	{
		mIsStart = msg_config->getAttrBool("is_start", false);
		AosXmlTagPtr msg_svr_tag = msg_config->getFirstChild();
		while(msg_svr_tag)
		{
			OmnString url = msg_svr_tag->getAttrStr("url", "");
			mMsgSvrUrl.push_back(url);
			OmnScreen << "++++++++++++++: " << url << endl;
			msg_svr_tag = msg_config->getNextChild();
		}
	}
	return true;
}



bool
AosMsgClient::start(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosMsgClient::start()
{
	if (!mIsStart)
	{
		return true;
	}
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "MsgClientThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	mLock->lock();
	if (mWaitProcs.empty())
	{
OmnScreen << "======================send start msg process: " << endl;
		AosAppMsgPtr msg = OmnNew AosStartDynamicProcMsg(AosProcessType::eMsg);
		aos_assert_rl(msg, mLock, false);
		AosSendMsg(msg);
	}
	mLock->unlock();
	return true;
}


bool
AosMsgClient::stop()
{
	return true;
}

OmnString 
AosMsgClient::getMsgSvrUrl()
{
	if (mMsgSvrUrl.empty())
	{
		return "";
	}
	return mMsgSvrUrl[0];
}

bool
AosMsgClient::procMsgSvrRequest(
		const OmnString &request,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mWaitProcs.empty())
	{
		RequestInfo info;
		info.mRequest = request;
		info.mRundata = rdata;
		mWaitRequest.push_back(info);
		AosAppMsgPtr msg = OmnNew AosStartDynamicProcMsg(AosProcessType::eMsg);
		aos_assert_rl(msg, mLock, false);
		AosSendMsg(msg);
		mCondVar->signal();
	}
	else
	{
		u32 logic_pid = mWaitProcs.front();
		AosTransPtr trans = OmnNew AosProcMsgTrans(request, AosGetSelfServerId(), logic_pid, false);
		bool rslt = AosSendTrans(rdata, trans);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}

bool    
AosMsgClient::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 logic_pid = 0;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mWaitRequest.empty() || mWaitProcs.empty())
		{
			mCondVar->wait(mLock);
			mThreadStatus = true;
			mLock->unlock();
			continue;
		}

		if (!mWaitRequest.empty() && !mWaitProcs.empty())
		{
			logic_pid = mWaitProcs.front();
			RequestInfo info = mWaitRequest.front();
			mWaitRequest.pop_front();
			AosTransPtr trans = OmnNew AosProcMsgTrans(info.mRequest, AosGetSelfServerId(), logic_pid, false);
			AosSendTrans(info.mRundata, trans);
		}
		mLock->unlock();
	}
	return true;
}

bool    
AosMsgClient::signal(const int threadLogicId)
{
	return true;
}

bool
AosMsgClient::stopProcCb(const u32 logic_pid)
{
	mLock->lock();
	deque<u32>::iterator itr = mWaitProcs.begin();
	while(itr != mWaitProcs.end())
	{
		if ((*itr) == logic_pid)
		{
			mWaitProcs.erase(itr);
			break;
		}
		itr++;
	}
	aos_assert_rl(mWaitProcs.empty(), mLock, false);
	mLock->unlock();
	return true;
}

bool
AosMsgClient::startProcCb(const u32 logic_pid)
{
OmnScreen << "jozhi==========================start msg callback" << endl;
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	mLock->lock();
	mWaitProcs.push_back(logic_pid);
	aos_assert_rl(mWaitProcs.size() == 1, mLock, false);
	mCondVar->signal();
	mLock->unlock();
	//Jozhi test send msg
	/*
	OmnSleep(120);
	while(true)
	{
		OmnSleep(1);
		AosXmlTagPtr contents = AosXmlParser::parse("<Contents to=\"10000075\" from=\"xxx\">hello word!!!!</Contents>" AosMemoryCheckerArgs);
		AosMsgService::getSelf()->sendMsgToUser(contents, rdata);
	}
	*/
	return true;
}
