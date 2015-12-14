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
#ifndef AOS_SvrSplitMgr_ProcessMgr_h
#define AOS_SvrSplitMgr_ProcessMgr_h

#include "alarm_c/alarm.h"
#include "SvrProxyMgr/Process.h"
#include "SysMsg/Ptrs.h"
#include "SEInterfaces/ProcessType.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <sys/epoll.h>
#include <map>
#include <queue>
using namespace std;


class AosProcessMgr : public OmnThreadedObj
{
	OmnDefineRCObject;
	enum
	{
		eMonitorThrdId = 1,
		eHandProcDeathThrdId = 2,
		eEntrySize = sizeof(u32) + sizeof(int) + sizeof(int),
	};

private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr				mMonitorThrd;
	OmnThreadPtr				mHandDeathProcThrd;

	OmnFilePtr					mPidTmpFile;

	AosSvrProxyPtr				mSvrProxy;
	AosIpcSvrPtr				mIpcSvr;
	OmnString					mTmpDir;
	bool						mAutoStartChild;
	bool						mAutoStartAdmin;
	AosXmlTagPtr				mChildNormConf;
	
	u32							mMaxLogicPid;
	map<u32, AosProcessPtr> 	mChildProcess;
	bool						mDiskDamagedHanded;
	bool						mShowLog;
	queue<AosProcessPtr>		mDeathedReqs;

public:
	explicit AosProcessMgr(
				const AosSvrProxyPtr &svr_proxy,
				const bool show_log);
	~AosProcessMgr();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    monitorThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };
	
	bool    config(const AosXmlTagPtr &conf);
	bool 	start(int argc, char **argv);
	bool    stopAll();
	bool    killAll();
	
	bool 	sendTrans(const AosTransPtr &trans);
	bool 	sendAppMsg(const AosAppMsgPtr &msg, bool &succ);
	bool 	recvNewConnSock(
				const OmnString &id,
				const u32 logic_pid,
				const int child_pid,
				const int conn_sock,
				const AosProcessType::E ptype);
	
	AosProcessType::E getProcType(const u32 logic_pid);
	
	bool	killProc(const u32 logic_pid);
	bool	startProc(const u32 logic_pid);
	u32 	startProc(const AosProcessType::E tp,
				const u32 listener_pid,
				const OmnString &args,
				const int svr_id);
	bool 	startDefProc();
	bool 	startDefProc(
				const u32 listener_pid,
				const OmnString &args,
				const AosXmlTagPtr &norm_config,
				const AosXmlTagPtr &cluster_config);

	bool 	isDeath(const u32 logic_pid);
	AosRecvEventHandlerPtr getRecvHandler();
	u32		getMaxCacheSize();
	
	int	 	getCrtMaster(const u32 cube_grp_id);

	bool 	notifyResendTrans(const AosTriggerResendMsgPtr &msg);
	bool 	recvResendEnd(
				const u32 from_sid,
				const u32 from_pid, 
				const u32 target_pid);
	
	bool	handDiskDamaged();
	bool	handSvrDeath(const int death_svr_id);

	//bool	localProcDeath(const AosProcessPtr &proc);
	bool 	stopProcExcludeAdmin(const int num);
	bool 	startJobSvr();
	bool	killAllTaskProcess();
	
// for cubeSvr start--------
public:
	// cubeProcess call following func.
	bool 	bkpCubeSvrDeath(
				const u32 cube_grp_id,
				const int from_svr_id, 
				const int crt_master);
	bool 	tryStartBkpCubeSvr(
				const u32 cube_grp_id,
				const int crt_master,
				bool &succ);
	bool	procDeathTriggerResend(const OmnString &reason);
	
	// SvrProxy call following func.
	bool	rebootCubeProc(const u32 cube_grp_id); 
	bool 	startBkpCubeSvr(
				const u32 cube_grp_id,
				const int crt_master);
	bool notifyProcIsUp(const int child_pid);
	bool	isTaskProcess(const AosProcessType::E type);

private:
	bool 	monitorCubeSvrsDeath(const int death_svr_id);
	bool 	cubeProcDeath(const u32 cube_grp_id, const int from_svr_id);
// for cubeSvr End----------.

private:
	bool 	monitorThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	monitorAutoStartChild();
	bool 	monitorNoAutoStartChild();
	bool 	monitorChildDeathPriv(const AosProcessPtr &proc);

	bool 	handProcDeathThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	bool 	cleanPidTmpFile();
	bool 	killProcessAndCleanSock(
				const u32 child_logic_pid,
				const int child_pid,
				const AosProcessType::E &type);
	bool 	cleanConfig(const u32 child_logic_pid);
	bool 	cleanProxySockFile();
	
	bool	saveToPidTmpFile(const u32 logic_pid,
				const int child_pid,
				const AosProcessType::E &type);

	AosProcessPtr 	createAdminProcess(int argc, char** argv);
	AosProcessPtr 	createProcess(const ProcInfo &proc_conf,
						const OmnString &args, 
						const bool auto_start);
	bool 	addProcessPriv(const u32 logic_pid, const AosProcessPtr &proc);
	u32 	allocLogicPid();

	bool	initChildNormConf(
				const AosXmlTagPtr &norm_config,
				const AosXmlTagPtr &cluster_config);
	AosXmlTagPtr 	getNetworkSvrsConf(const AosXmlTagPtr &cluster_config);

	AosProcessPtr 	getProcByAppMsg(const AosAppMsgPtr &msg);
	AosProcessPtr 	getProcByLogicPid(const u32 logic_pid);
	AosProcessPtr 	getProcByType(const AosProcessType::E tp);
	AosProcessPtr 	getCubeProcByCubeGrpId(const u32 cube_grp_id);
	AosProcessPtr 	getProcByChildPid(const int child_pid);
	
	//bool 	localProcUp(const AosProcessPtr &proc);

	bool 	startProcPriv(const AosProcessPtr &proc);
	bool 	needSaveToTmpFile(const u32 logic_pid);

	bool 	removeProc(const u32 logic_pid);

	AosProcessPtr tryGetProcByAppMsg(const AosAppMsgPtr &msg);
	bool 	contSendAppMsg(const AosProcessPtr &proc, 
				const AosAppMsgPtr &msg, 
				bool &succ);
};
#endif
