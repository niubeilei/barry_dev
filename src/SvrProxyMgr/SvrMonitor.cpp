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
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/SvrMonitor.h"

#include "API/AosApi.h" 
#include "Thread/CondVar.h" 
#include "Thread/Mutex.h" 
#include "Thread/ThreadPool.h" 
#include "SvrProxyMgr/NetSvr.h"
#include "Porting/Sleep.h"

#include <unistd.h>
#include <fcntl.h>
#include <error.h>

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("AosSvrMonitor", __FILE__, __LINE__);

AosSvrMonitor::AosSvrMonitor(const AosNetSvrPtr svr_proxy, const bool show_log)
:
mNetSvr(svr_proxy),
mLock(OmnNew OmnMutex(true)),
mCondVar(OmnNew OmnCondVar()),
mShowLog(show_log)
{
}


AosSvrMonitor::~AosSvrMonitor()
{
}


bool
AosSvrMonitor::start()
{
	bool rslt = addServers();
	aos_assert_r(rslt, false);

	rslt = initSock();
	aos_assert_r(rslt, false);

	rslt = bindSockToLocalAddr();
	aos_assert_r(rslt, false);

	startThrd();
	return true;
}


bool
AosSvrMonitor::addServers()
{
	vector<u32> & svr_ids = AosGetServerIds();

	AosServerInfoPtr svr_info;
	SvrStatusInfoPtr svr_sts;
	mLock->lock();
	for(u32 i=0; i<svr_ids.size(); i++)
	{
		u32 svr_id = svr_ids[i];

		svr_sts = getSvrStatus(svr_id);
		if(!svr_sts)
		{
			svr_sts = newSvrStatus(svr_id); 
		}
	
		svr_info = AosGetSvrInfo(svr_id);
		aos_assert_r(svr_info, false);
		
		svr_sts->mStrAddr = svr_info->getAddr();
		svr_sts->mPort = svr_info->getPort();
		aos_assert_r(svr_sts->mStrAddr != "" && svr_sts->mPort> 0, false);
	}
	mLock->unlock();
	return true;
}


bool
AosSvrMonitor::initSock()
{
	mSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (mSock == -1)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	
	OmnScreen << "<ProxyMsg> SvrMonitor; init sock:" << mSock << endl;
	return true;
}


bool
AosSvrMonitor::bindSockToLocalAddr()
{
	aos_assert_r(mSock >= 0, false);
	
	struct sockaddr_in addr;
	bool rslt = initLocalAddr(addr);
	aos_assert_r(rslt, false);
	
	int rs = ::bind(mSock,(sockaddr *)&addr, sizeof(addr));
	if (rs == -1)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	return true;
}

bool
AosSvrMonitor::initLocalAddr(sockaddr_in &addr)
{
	int self_sid = AosGetSelfServerId();
	aos_assert_r(self_sid >=0, false);
	
	map<u32, SvrStatusInfoPtr>::iterator itr = mSvrStatus.find(self_sid);
	aos_assert_r(itr != mSvrStatus.end(), false);
	
	OmnString local_addr = (itr->second)->mStrAddr;
	u64	local_port = (itr->second)->mPort;
	aos_assert_r(local_addr != "" && local_port > 0, false);
	
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;  
	inet_aton(local_addr.data(),&(addr.sin_addr));
	addr.sin_port = htons(local_port);
	return true;
}


bool
AosSvrMonitor::startThrd()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mRecvHBThrd = OmnNew OmnThread(thisPtr, "RecvHeartBeatThrd",
			eRecvHeartBeatThrdId, true, true, __FILE__, __LINE__);
	mSendHBThrd = OmnNew OmnThread(thisPtr, "SendHeartBeatThrd",
			eSendHeartBeatThrdId, true, true, __FILE__, __LINE__);
	mRecvHBThrd->start();
	mSendHBThrd->start();
	return true;
}


bool
AosSvrMonitor::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u64 thrd_id = thread->getLogicId();
	
	if(thrd_id == eSendHeartBeatThrdId)
	{
		return sendHeartBeat(state, thread);
	}

	if(thrd_id == eRecvHeartBeatThrdId)
	{
		return recvHeartBeatThrdFunc(state, thread);
	}
	
	OmnAlarm << "Invalid thread logic id: " << thrd_id << enderr;
	return false;
}


bool
AosSvrMonitor::sendHeartBeat(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	bool rslt;
	struct sockaddr_in addr_to;
	SvrStatusInfoPtr svr_sts;
	map<u32, SvrStatusInfoPtr>::iterator itr;
	while(state == OmnThrdStatus::eActive)
	{
		OmnSleep(eSendSleepTime);
		
		AosBuffPtr msg = getSendMsg();
		aos_assert_r(msg, false);
		
		mLock->lock();
		for(itr = mSvrStatus.begin(); itr != mSvrStatus.end(); itr++)
		{
			svr_sts = itr->second;
			if(itr->first == (u32)AosGetSelfServerId())	continue;
			if (isHeartBeatTimeout(svr_sts)) continue;

			isSockRecvTimeout(svr_sts);

			bzero(&addr_to, sizeof(addr_to));
			addr_to.sin_family = AF_INET;
			inet_aton((svr_sts->mStrAddr).data(),&(addr_to.sin_addr));
			addr_to.sin_port = htons(svr_sts->mPort);
		
			if (mShowLog)
			{
				OmnScreen << "============debug==========sendHeartBeat, svr_id: " << itr->first << endl;
			}
			rslt = sendHeartBeatPriv(msg, addr_to);
			if (!rslt)
			{
				OmnScreen << "******************Alarm*******************sendHeartBeat Failed, svr_id: " << itr->first << endl;
			}
		}
		//do to repaire all the failed connection
		sgThreadPool->procSync(mRepaireConnect);
		mRepaireConnect.clear();
		mLock->unlock();
	}
	
	return true;
}


bool
AosSvrMonitor::sendHeartBeatPriv(
		const AosBuffPtr &msg,
		sockaddr_in &addr_to)
{
	socklen_t addr_len = sizeof(sockaddr_in);
	int rslt = ::sendto(mSock, msg->data(), msg->dataLen(),
		0, (sockaddr*)&addr_to, addr_len);
	if(rslt < 0)
	{
		return false;
	}
	return true;
}


AosBuffPtr
AosSvrMonitor::getSendMsg()
{
	AosBuffPtr msg = OmnNew AosBuff(eMaxMsgLen AosMemoryCheckerArgs);
	
	int self_svr_id = AosGetSelfServerId();
	aos_assert_r(self_svr_id >=0, 0);
	msg->setInt(self_svr_id);
	aos_assert_r(msg->dataLen() <= eMaxMsgLen, 0);
	return msg;	
}

bool
AosSvrMonitor::isHeartBeatTimeout(const SvrStatusInfoPtr &svr_info)
{
	if(svr_info->mLastHeartbeatRecvTime == 0) return false;

	u64 time_sec = (OmnGetTimestamp() - svr_info->mLastHeartbeatRecvTime) / 1000 / 1000;
	if(time_sec < eHeartBeatTimeoutTimes)	return false;
	OmnScreen << "********************Alarm *****************HeartBeatTimeout, svr_id: " << svr_info->mSvrId << endl;

	svr_info->mLastHeartbeatRecvTime = OmnGetTimestamp();

	if (svr_info->mSvrId > AosGetSelfServerId())
	{
		AosSvrMonitorPtr thisPtr(this, false);
		OmnThrdShellProcPtr runner = OmnNew RepaireConnectThrd(thisPtr, svr_info);
		mRepaireConnect.push_back(runner);
	}
	return true;
}

bool
AosSvrMonitor::isSockRecvTimeout(const SvrStatusInfoPtr &svr_info)
{
	if (svr_info->mSockLastRecvTime == 0) return false;

	u64 time_sec = (OmnGetTimestamp() - svr_info->mSockLastRecvTime) / 1000 / 1000;
	if (time_sec < eSockRecvTimeoutTimes) return false;
	OmnScreen << "***********************Alarm******************SockRecvTimeout, svr_id: " << svr_info->mSvrId << endl;

	//svr_info->mSockLastRecvTime = OmnGetTimestamp();

	if (svr_info->mSvrId > AosGetSelfServerId())
	{
		AosSvrMonitorPtr thisPtr(this, false);
		OmnThrdShellProcPtr runner = OmnNew RepaireConnectThrd(thisPtr, svr_info);
		mRepaireConnect.push_back(runner);
	}
	return true;
}

bool
AosSvrMonitor::repaireConnect(const SvrStatusInfoPtr &svr_info)
{
	OmnScreen << "============debug==========stop connect, svr_id: " << svr_info->mSvrId << endl;
	mNetSvr->stop(svr_info->mSvrId);
	if (svr_info->mSvrId > AosGetSelfServerId())
	{
		OmnScreen << "============debug==========connect to, svr_id: " << svr_info->mSvrId << endl;
		mNetSvr->repaireConnect(svr_info->mSvrId);
	}
	return true;
}

bool
AosSvrMonitor::repaireConnect(const int svr_id)
{
	mLock->lock();
	SvrStatusInfoPtr svr_info = getSvrStatus(svr_id);	
	aos_assert_rl(svr_info, mLock, false);
	bool rslt = repaireConnect(svr_info);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}


bool
AosSvrMonitor::recvHeartBeatThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	struct sockaddr_in clt_addr;
	socklen_t addr_len = sizeof(sockaddr_in);
	
	AosBuffPtr recv_msg = OmnNew AosBuff(eMaxMsgLen AosMemoryCheckerArgs);
	while(state == OmnThrdStatus::eActive)
	{
		int bytes_recv = ::recvfrom(mSock, recv_msg->data(), eMaxMsgLen,
				0, (sockaddr*)&clt_addr, &addr_len); 
		if(bytes_recv <= 0)
		{
			OmnAlarm << "read error...." << enderr;  
		}
		
		recv_msg->setDataLen(bytes_recv);
		recv_msg->setCrtIdx(0);
		procRecvedMsg(recv_msg);
	}
	
	return true;
}


bool
AosSvrMonitor::procRecvedMsg(const AosBuffPtr &msg)
{
	aos_assert_r(msg, false);
	int svr_id = msg->getInt(-1);	
	aos_assert_r(svr_id >=0, false);
	
	recvHeartBeat(svr_id);
	return true;
}


bool
AosSvrMonitor::recvHeartBeat(const int svr_id)
{
	mLock->lock();
	SvrStatusInfoPtr svr_info = getSvrStatus(svr_id);	
	aos_assert_rl(svr_info, mLock, false);
	if (mShowLog)
	{
		OmnScreen << "============debug==========recvHeartBeat update time, svr_id: " << svr_id << endl;
	}
	svr_info->mLastHeartbeatRecvTime = OmnGetTimestamp();
	mLock->unlock();
	return true;
}

bool
AosSvrMonitor::updateSockLastRecvTime(const int svr_id)
{
	mLock->lock();
	SvrStatusInfoPtr svr_info = getSvrStatus(svr_id);	
	aos_assert_rl(svr_info, mLock, false);

	if (mShowLog)
	{
		OmnScreen << "============debug==========updateSockLastRecvTime update time, svr_id: " << svr_id << endl;
	}
	svr_info->mSockLastRecvTime = OmnGetTimestamp();
	mLock->unlock();
	return true;
}


AosSvrMonitor::SvrStatusInfoPtr
AosSvrMonitor::getSvrStatus(const int svr_id)
{
	map<u32, SvrStatusInfoPtr>::iterator itr = mSvrStatus.find(svr_id);
	if(itr == mSvrStatus.end())	return 0;
	
	return itr->second;
}


AosSvrMonitor::SvrStatusInfoPtr
AosSvrMonitor::newSvrStatus(const int svr_id)
{
	SvrStatusInfoPtr svr_sts = getSvrStatus(svr_id);
	aos_assert_r(!svr_sts, 0);

	svr_sts = OmnNew SvrStatusInfo();
	svr_sts->mLastHeartbeatRecvTime = 0;
	svr_sts->mSockLastRecvTime = 0;
	svr_sts->mSvrId = svr_id;

	mSvrStatus.insert(make_pair(svr_id, svr_sts));
	OmnScreen << "ssssss new svr status! svr_id:" << svr_id << endl;

	return svr_sts;
}

bool
AosSvrMonitor::RepaireConnectThrd::run()
{
	mSvrMonitor->repaireConnect(mSvrInfo);
	return true;
}
