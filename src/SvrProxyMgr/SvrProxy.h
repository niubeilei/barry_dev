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
#ifndef AOS_SvrProxyMgr_SvrProxy_h
#define AOS_SvrProxyMgr_SvrProxy_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/SvrProxyObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SvrProxyMgr/ConnHandler.h"
#include "SvrProxyMgr/IpcSvr.h"
#include "SysMsg/Ptrs.h"
#include "TransBasic/ConnMsg.h"
#include "SvrProxyMgr/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <sys/epoll.h>
#include <map>
#include <list>

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


// use
// AosProxyLog << xxx << endlog;

#define  AosProxyLog {\
	    ostringstream os; \
		os << hex << "<" << __FILE__ <<  ":" << __LINE__ << "> " 

#define endlog \
		std::endl;\
		AosSvrProxy::log(os);\
}

class AosSvrProxy : public AosSvrProxyObj, 
					public OmnThreadedObj,
					public AosRecvEventHandler
{
	OmnDefineRCObject;
	
	enum
	{
		eInitThrdId = 1,
		eRecvThrdId = 2,
		eMaxRecvThrd = 20,
		eMaxTrys = 50,
	};

public:
	static map<OmnString, AosBuffPtr> smTestBuff;
	static map<OmnString, AosBuffPtr> smTestIOVBuff;

private:
	OmnMutexPtr			mLock;
	OmnMutexPtr			mInitLock;
	OmnMutexPtr			mSwitchLock;
	OmnCondVarPtr		mCondVar;
	OmnCondVarPtr 		mMastersCondVar;
	OmnCondVarPtr 		mClusterCondVar;

	AosProcessMgrPtr	mProcessMgr;
	AosNetSvrPtr		mNetSvr;
	AosTransHandlerPtr  mTransHandler;

	OmnThreadPtr		mInitThrd;
	u32					mRecvThrdNum;
	OmnThreadPtr		mRecvThrd[eMaxRecvThrd];
	AosSockConnPtr		mProcingConn[eMaxRecvThrd];

	//queue<AosSockConnPtr>	mEventConns;
	list<AosSockConnPtr>	mEventConns;

	u64					mMaxConnCacheSize;
	AosXmlTagPtr		mClusterConfig;
	AosXmlTagPtr		mNormConfig;
	OmnString			mArgs;
	bool				mSvrLoaded;
	bool				mClusterLoaded;
	bool				mMastersLoaded;
	
	bool				mShowLog;
	
	AosResendCheckThrdPtr mResendCheckThrd;
	
	static ofstream 	smLogFile;

public:
	explicit AosSvrProxy();
	~AosSvrProxy();

	// ThreadedObj Interface
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool signal(const int threadLogicId){ return true; };
	
	// Conn Event Handler Interface.
	virtual bool msgRecvEvent(const AosSockConnPtr &conn);

	virtual bool config(const AosXmlTagPtr &conf);
	virtual bool start(int argc, char **argv);
	virtual bool stop();
	virtual bool kill();

	virtual bool startServer();
	
	virtual bool addServers(
					const int crt_judger_sid,		
					const AosXmlTagPtr &svrs_conf,
					const AosXmlTagPtr &admin_conf);
	virtual bool addCluster(
					const u32 from_pid,
					const OmnString &args,
					const AosXmlTagPtr &master_config,
					const AosXmlTagPtr &norm_config); 
	virtual bool switchToMaster(const u32 cube_grp_id);
	virtual bool switchToMasterFinish(const u32 cube_grp_id, const u32 proc_id);

	virtual bool setNewMaster(const u32 cube_grp_id, const int new_master);
	virtual bool recvSvrStopped(const int stopped_sid);
	virtual bool stopProcExcludeAdmin(const int num);
	virtual bool diskDamaged();
	
	virtual bool killProc(const u32 logic_pid);
	virtual bool startProc(const u32 logic_pid);
	virtual bool startProc(
			const u32 from_pid, 
			const AosProcessType::E tp,
			const int listen_svr_id);
	
	virtual bool triggerResend(const AosTriggerResendMsgPtr &msg);
	virtual bool recvResendEnd(
					const int target_svr_id,
					const int target_proc_id,
					const bool is_ipc_conn,
					const int resend_from_sid,
					const u32 resend_from_pid);
	
	virtual bool recvSvrUp(const u32 svr_id);
	virtual bool recvSvrDeath(const int death_svr_id);
	virtual bool getCrtMastersBuff(const int to_svr_id);
	virtual bool getMastersFailed(const int from_sid);
	virtual bool recvMastersBuff(
					const int from_sid,
					const bool from_file,
					const AosBuffPtr &masters_buff);
	virtual bool getCluster(const int to_svr_id);
	virtual bool sendAppMsg(const AosAppMsgPtr &msg, bool &succ);
	
	virtual bool addWaitResendEnd(
					const int svr_id,
					const u32 proc_id,
					const bool wait_is_ipc_conn,
					const u32 wait_id);

	virtual bool removeWaitResendEnd(
					const int svr_id,
					const u32 proc_id,
					const bool wait_is_ipc_conn,
					const u32 wait_id);
	virtual bool 	selfIsJudgerSvr();

	virtual bool startJobSvr();
	
	u64		getMaxCacheSize() { return mMaxConnCacheSize; }
	int 	getCrtMaster(const u32 cube_grp_id);
	bool	broadcastMsg(const AosAppMsgPtr &msg);


	bool	monitorSvrDeath(const int svr_id);
	bool 	svrIsLoaded(){ return mSvrLoaded; };

	bool 	sendTrans(const AosTransPtr &trans);
	virtual bool notifyProcIsUp(const int child_pid);
	
	static void log(const ostringstream &data);

private:
	bool 	initLogFile();
	bool 	initCluster();
	bool 	initCrtMaster();

	bool 	configSvrProxy(const AosXmlTagPtr &svr_proxy_conf, 
				const AosXmlTagPtr &admin_conf);
	bool 	sendConfigToAdmin(const AosXmlTagPtr &svr_proxy_conf, 
				const AosXmlTagPtr &admin_conf);
	bool 	recvThrdFunc(
				OmnThrdStatus::E &state, 
				const OmnThreadPtr &thread,
				const u32 crt_thrdid);

	bool 	initThrdFunc(
				OmnThrdStatus::E &state,
				const OmnThreadPtr &thread);

	bool 	recvMsg(const AosAppMsgPtr &msg);

	bool 	sendAppMsgToNet(
				const AosAppMsgPtr &msg,
				bool &succ);
	bool 	sendAppMsgToChildProc(
				const AosAppMsgPtr &msg,
				bool &succ);
	bool 	isMsgToSvrProxy(const AosAppMsgPtr &msg);
	bool 	msgIsToSvrProxy(const AosAppMsgPtr &msg);
	
	bool 	isCubeGrpInSelfSvr(const u32 cube_grp_id);

	bool 	setNewMasterPriv(const u32 cube_grp_id, const int new_master);
	
};
#endif
