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
#ifndef AOS_SvrProxyMgr_Process_h
#define AOS_SvrProxyMgr_Process_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/ProcessType.h"
//#include "SEInterfaces/ProcInfo.h"
#include "SEInterfaces/ServerInfo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SvrProxyMgr/SockConn.h"
#include "SvrProxyUtil/Ptrs.h"

#include <vector>
using namespace std;

class AosProcess : public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	AosProcessType::E	mType;
	OmnString		mExePath;
	OmnString		mConfigPath;
	u32				mLogicPid;
	int				mChildPid;
	u32				mListenerPid;
	OmnString		mBaseDir;
	int					mListenerSvrId;
	
protected:
	AosIpcSvrInfoPtr mSvr; 
	AosSvrProxyPtr	mSvrProxy;
	AosProcessMgrPtr mProcMgr;
	bool			mProActiveKill;
	bool			mAutoStart;
	bool			mShowLog;
	char **			mArgs;
	u32				mArgsNum;
	u32				mCrtArgsIdx;
	OmnString		mTmpDir;

	bool			mNeedMonitor;
	bool			mDiskError;

public:
	AosProcess(
		const AosProcessType::E tp,
		const OmnString &name,
		const bool reg_flag);
	AosProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const AosProcessType::E proc_tp,
		const OmnString &exe_name,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log);

	~AosProcess();

	static AosProcessPtr createAdminProcStatic(
				const AosProcessMgrPtr &proc_mgr,
				const AosSvrProxyPtr &proxy,
				const int logic_pid,
				const bool auto_start,
				const OmnString tmp_dir,
				const bool show_log,
				int argc, 
				char **argv);
	static AosProcessPtr createProcStatic(
				const AosProcessMgrPtr &proc_mgr,
				const AosSvrProxyPtr &proxy,
				const int logic_pid,
				const ProcInfo &proc_conf,
				const bool auto_start,
				const OmnString tmp_dir,
				const bool show_log);

	virtual bool startProcess();
	//virtual OmnString getStrBaseDir();

	virtual AosProcessPtr clone(
				const AosProcessMgrPtr &proc_mgr,
				const AosSvrProxyPtr &proxy,
				const int logic_pid,
				const ProcInfo &proc_conf,
				const OmnString &tmp_dir,
				const bool auto_start,
				const bool show_log) = 0;
	virtual bool setNetworkConf(const AosXmlTagPtr &norm_config);
	
	virtual bool reboot(){ OmnShouldNeverComeHere; return false; };
	virtual bool hasCubeGrpId(const u32 cube_grp_id){ return false; };

	virtual bool setStorageDir(const AosXmlTagPtr &config);
	virtual bool cleanResource() { return true; }
	virtual bool handProcDeath();
	virtual bool handProcUp();

	bool 	start();
	bool 	stop(const int num);
	bool	restart();
	bool	handDiskDamage();

	AosProcessType::E  getType(){ return mType; };
	OmnString	getStrType(){ return AosProcessType::toStr(mType); };
	u32		getLogicPid(){ return mLogicPid; };
	int		getChildPid(){ return mChildPid; };
	bool 	recvNewConnSock(const OmnString &id, const int child_pid, const int sock_id);

	bool 	sendAppMsg(const AosAppMsgPtr &msg, bool &succ);
	bool 	sendTrans(const AosTransPtr &trans);
	bool 	isDeath();
	bool 	createConfig(const AosXmlTagPtr &norm_config);

	bool 	killProc(const int num = 9);
	bool 	setDeath();
	bool 	waitDeath();

	void	setArgs(const OmnString &args);
	bool	recvResendEnd(const u32 from_sid, const u32 from_pid);
	bool	isAutoStart(){ return mAutoStart; };
	
	void	setListenerPid(const u32 pid){ mListenerPid = pid; };
	void	setListenerSvrId(const int svr_id) { mListenerSvrId = svr_id; }
	OmnString getBaseDir() { return mBaseDir; }

	bool	isNeedMonitor(){ return mNeedMonitor; };
	void	setNeedMonitor(const bool ff){ mNeedMonitor = ff; };
	bool 	notifyListenerPidUp();
	bool 	procUpTriggerResend();

protected:
	virtual u32 getArgsNum(const u32 pp_args_num);
	virtual void addNewArgs();
	virtual bool resetArgs(){ return true;};
	
	bool 	setBaseDirConfig(const AosXmlTagPtr &config);
	OmnString getExeName();

	bool 	notifyListenerPidDeath();
	bool 	procDeathTriggerResend();

private:
	bool 	registerProc(const AosProcessPtr &proc, const OmnString &name);
	bool 	init();

	bool 	setIpcCltConf(const AosXmlTagPtr &norm_config);
	bool 	saveConfigToFile(const AosXmlTagPtr &norm_config);
	
	OmnString getArgs();
	
};
#endif
