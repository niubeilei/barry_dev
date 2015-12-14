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
// Created: 04/25/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_NetSvr_h
#define AOS_SvrProxyMgr_NetSvr_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SvrProxyUtil/Ptrs.h"
#include "SvrProxyMgr/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

#include <sys/epoll.h>
#include <map>
#include <set>
using namespace std;


class AosNetSvr: public OmnThreadedObj
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxConnTrys = 5,
		eReConnTime = 5,

		eAcceptConnThrdId = 1,
		eCheckConnThrdId = 2,
	};

private:
	OmnMutexPtr		mLock;
	AosSvrProxyPtr	mSvrProxy;
	int				mListenSock;
	OmnThreadPtr	mConnThrd;
	OmnThreadPtr	mCheckConnThrd;
	AosSvrMonitorPtr  mSvrMonitor;
	
	AosXmlTagPtr	mSvrsConfig;	
	AosXmlTagPtr	mAdminConfig;
	u32				mCrtJudgerSvrId;

	bool			mShowLog;
	bool			mStopping;
	map<int, AosNetSvrInfoPtr>	mSvrs; 
	OmnString		mLocalAddr;
	int 			mLocalPort;

public:
	AosNetSvr(
			const AosSvrProxyPtr &svr_proxy,
			const bool showlog);
	~AosNetSvr();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

	bool	config(const AosXmlTagPtr &conf);
	bool	startListen();
	
	bool 	init(
				const AosXmlTagPtr &svrs_config,
				const AosXmlTagPtr &admin_config,
				const int crt_judger_sid);
	bool	start();
	bool 	notifyIsStopping();
	bool	stop();

	bool 	recvSvrReady(const int svr_id);
	bool 	recvSvrDeath(const int svr_id);
	bool 	recvSvrStopping(const int svr_id);

	bool 	sendTrans(const int to_svr_id, const AosTransPtr &trans);
	bool 	sendAppMsg(
				const int to_svr_id,
				const AosAppMsgPtr &msg,
				bool &succ);
	bool 	recvResendEnd(const u32 from_sid, const u32 from_pid, const int target_sid);
	
	bool 	selfIsJudgerSvr();
	int 	getJudgerSvrId(){ return mCrtJudgerSvrId; };

	bool 	broadcastMsg(const AosAppMsgPtr &msg);

public:
	// NetSvrInfo using following func.
	//bool 	connFailed(const int svr_id, const int sock);

public:	
	// SvrMonitor using following func.
	bool 	monitorSvrDeath(const int svr_id);
	int 	calcuNextJudgerSvrId(const int crt_judger_sid);
	bool 	sendSvrDeathMsg(const int to_sid, const int death_sid);
	bool	setConnFailed(const int svr_id);
	bool    stop(const int svr_id);
	bool	repaireConnect(const int svr_id);
private:	
	bool	connectSvr(const AosNetSvrInfoPtr &svr, int trys);
	void 	startCheckConnThrd();
	void 	startConnThrd();
	void 	setNewJudgerSvrId(const int judger_sid);

	bool 	acceptConnThrdFunc(
				OmnThrdStatus::E &state,
				const OmnThreadPtr &thread);
	bool 	checkConnThrdFunc(
				OmnThrdStatus::E &state,
				const OmnThreadPtr &thread);

	bool 	bindSelfConnSock(const int sock_id);
	u32 	randomGetPort(const int svr_id, const int trys);
	int 	getSvrIdByPort(const int clt_port);

	bool 	sendSvrReadyMsg(const AosNetSvrInfoPtr &svr);
	bool    checkAndSendSvrReadyMsg(const bool accept);

	AosNetSvrInfoPtr getSvr(const int svr_id, bool create = false);
	
	bool 	sendAddServerMsg(const AosNetSvrInfoPtr &svr);


	bool 	sendAppMsgPriv(
				const AosNetSvrInfoPtr &svr,
				const AosAppMsgPtr &msg,
				bool &succ);
};
#endif
