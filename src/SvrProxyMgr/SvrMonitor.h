////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_SvrMonitor_h
#define AOS_SvrProxyMgr_SvrMonitor_h

#include "alarm_c/alarm.h"
#include "SvrProxyMgr/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"

#include <map>
#include <vector>
#include <set>
using namespace std;

#include <netinet/in.h>

class AosSvrMonitor : public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eSendHeartBeatThrdId,
		eRecvHeartBeatThrdId,
		eSendSleepTime = 1,		// 1 sec.
		eHeartBeatTimeoutTimes = 5,	// 10 sec.
		eSockRecvTimeoutTimes = 10*60, //10 min
		eMaxMsgLen = 1024,

	};

	struct SvrStatusInfo
	{
		u32			mSvrId;
		OmnString	mStrAddr;
		u32		 	mPort;
		//Jozhi means the udp last recv time
		u64			mLastHeartbeatRecvTime;
		//Jozhi means the tcp sock last recv time
		u64			mSockLastRecvTime;
	};
	
	typedef SvrStatusInfo * SvrStatusInfoPtr;

	class RepaireConnectThrd: public OmnThrdShellProc
	{
		OmnDefineRCObject;
		private:
		SvrStatusInfoPtr		 	mSvrInfo;
		AosSvrMonitorPtr			mSvrMonitor;	
		public:
		RepaireConnectThrd(
				const AosSvrMonitorPtr &svr_monitor,
				const SvrStatusInfoPtr &svr_info)
		:
		OmnThrdShellProc("RepaireConnectThrd"),             
		mSvrInfo(svr_info),
		mSvrMonitor(svr_monitor)
		{
		}
		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};

private:
	AosNetSvrPtr				mNetSvr;
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr				mRecvHBThrd;
	OmnThreadPtr				mSendHBThrd;
	map<u32, SvrStatusInfoPtr>	mSvrStatus;
	vector<OmnThrdShellProcPtr> mRepaireConnect;
	int							mSock;
	bool						mShowLog;

public:
	AosSvrMonitor(const AosNetSvrPtr svr_proxy, const bool show_log);
	~AosSvrMonitor();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };
	
	bool 	start();
	bool 	recvHeartBeat(const int svr_id);
	bool	updateSockLastRecvTime(const int svr_id);
	bool	repaireConnect(const SvrStatusInfoPtr &svr_info);
	bool	repaireConnect(const int svr_id);

private:
	bool	addServers();
	bool 	initSvrInfo();
	bool 	initSock();
	bool 	bindSockToLocalAddr();
	bool 	initLocalAddr(sockaddr_in &addr);
	bool 	startThrd();

	bool 	sendHeartBeat(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	sendHeartBeatPriv(const AosBuffPtr &buff, sockaddr_in &addr_to);
	AosBuffPtr getSendMsg();
	bool 	isHeartBeatTimeout(const SvrStatusInfoPtr &svr_info);
	bool 	isSockRecvTimeout(const SvrStatusInfoPtr &svr_info);
	bool 	recvHeartBeatThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	procRecvedMsg(const AosBuffPtr &msg);


	SvrStatusInfoPtr newSvrStatus(const int svr_id);
	SvrStatusInfoPtr getSvrStatus(const int svr_id);
};

#endif
