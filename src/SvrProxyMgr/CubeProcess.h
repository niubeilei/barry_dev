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
#ifndef AOS_SvrProxyMgr_CubeProcess_h
#define AOS_SvrProxyMgr_CubeProcess_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "SvrProxyMgr/Process.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include <map>
using namespace std;

class AosCubeProcess : public AosProcess 
{

private:
	u32			mCubeGrpId;
	bool		mRebooting;

public:
	AosCubeProcess(const bool reg_flag);
	AosCubeProcess(
		const AosProcessType::E tp,
		const OmnString &name,
		const bool reg_flag);

	AosCubeProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const u32 cube_grp_id,
		const AosProcessType::E tp,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log);

	~AosCubeProcess();

	virtual AosProcessPtr clone(
				const AosProcessMgrPtr &proc_mgr,
				const AosSvrProxyPtr &proxy,
				const int logic_pid,
				const ProcInfo &proc_conf,
				const OmnString &tmp_dir,
				const bool auto_start,
				const bool show_log);

	virtual bool setNetworkConf(const AosXmlTagPtr &norm_config);
	//Jozhi 2015-04-20 remove _0,1,2
	//virtual OmnString getStrBaseDir();
	virtual u32 getArgsNum(const u32 pp_args_num);
	virtual void addNewArgs();
	virtual bool resetArgs();
	virtual bool reboot();
	virtual bool hasCubeGrpId(const u32 cube_grp_id){ return mCubeGrpId == cube_grp_id; };
	virtual bool handProcDeath();


	u32		getCubeGrpId(){ return mCubeGrpId; };

private:
	OmnString getVidStr();
	char * 	  getStrCrtMaster();
	bool 	handProcDeathPriv();

};
#endif
