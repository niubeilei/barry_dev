///////////////////////////////////////////////////////////////////////////
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
// 2013/07/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SvrProxyObj_h
#define Aos_SEInterfaces_SvrProxyObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SvrProxyUtil/Ptrs.h"
#include "SvrProxyUtil/Ptrs.h"
#include "SysMsg/Ptrs.h"
//#include "SvrProxyMgr/ProcessMgr.h"
#include "TransUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "SEInterfaces/ProcessType.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosSvrProxyObj : virtual public OmnRCObject
{
protected:
	static AosSvrProxyObjPtr		smSvrProxy;

public:
	virtual bool config(const AosXmlTagPtr &app_conf) = 0;
	virtual bool start(int argc, char **argv) = 0;
	virtual bool stop() = 0;
	virtual bool kill() = 0;
	
	virtual bool startServer() = 0;
	virtual bool addServers(
					const int crt_judger_sid,		
					const AosXmlTagPtr &svr_proxy_conf,
					const AosXmlTagPtr &admin_conf) = 0;
	virtual bool addCluster(
					const u32 from_pid,
					const OmnString &args, 
					const AosXmlTagPtr &cluster_config,
					const AosXmlTagPtr &norm_config) = 0;
	
	virtual bool switchToMaster(const u32 cube_grp_id) = 0;
	virtual bool switchToMasterFinish(const u32 cube_grp_id, const u32 proc_id) = 0;
	virtual bool setNewMaster(const u32 cube_grp_id, const int new_master) = 0;
	virtual bool recvSvrStopped(const int stopped_sid) = 0;
	virtual bool stopProcExcludeAdmin(const int num) = 0;
	virtual bool diskDamaged() = 0;
	
	virtual bool killProc(const u32 logic_pid) = 0;
	virtual bool startProc(const u32 logic_pid) = 0;
	virtual bool startProc(
			const u32 from_pid, 
			const AosProcessType::E tp,
			const int listen_svr_id) = 0;
	
	virtual bool triggerResend(const AosTriggerResendMsgPtr &msg) = 0;
	virtual bool recvResendEnd(
					const int target_svr_id,
					const int target_proc_id,
					const bool is_ipc_conn,
					const int resend_from_sid,
					const u32 resend_from_pid) = 0;
	
	virtual bool recvSvrUp(const u32 up_svr_id) = 0;
	virtual bool recvSvrDeath(const int death_svr_id) = 0;
	virtual bool getCrtMastersBuff(const int to_svr_id) = 0;
	virtual bool recvMastersBuff(
					const int from_sid,
					const bool from_file,
					const AosBuffPtr &masters_buff) = 0;
	virtual bool getMastersFailed(const int from_sid) = 0;
	virtual bool getCluster(const int to_svr_id) = 0;
	virtual bool sendAppMsg(const AosAppMsgPtr &msg, bool &succ) = 0;
	
	virtual bool addWaitResendEnd(
					const int svr_id,
					const u32 proc_id,
					const bool wait_is_ipc_conn,
					const u32 wait_id) = 0;

	virtual bool removeWaitResendEnd(
					const int svr_id,
					const u32 proc_id,
					const bool wait_is_ipc_conn,
					const u32 wait_id) = 0;
	virtual bool 	selfIsJudgerSvr() = 0;
	virtual bool 	startJobSvr() = 0;
	virtual bool 	notifyProcIsUp(const int child_pid) = 0;
	
	static void setSvrProxy(const AosSvrProxyObjPtr &tc) {smSvrProxy = tc;}
	static AosSvrProxyObjPtr getSvrProxy() {return smSvrProxy;}

};

#endif
