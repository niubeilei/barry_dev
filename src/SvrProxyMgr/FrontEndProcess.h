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
#ifndef AOS_SvrProxyMgr_FrontEndProcess_h
#define AOS_SvrProxyMgr_FrontEndProcess_h

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

class AosFrontEndProcess : public AosProcess 
{
public:
	AosFrontEndProcess(const bool reg_flag);
	AosFrontEndProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log);
	~AosFrontEndProcess();

	virtual AosProcessPtr clone(
				const AosProcessMgrPtr &proc_mgr,
				const AosSvrProxyPtr &proxy,
				const int logic_pid,
				const ProcInfo &proc_conf,
				const OmnString &tmp_dir,
				const bool auto_start,
				const bool show_log);

	virtual bool handProcDeath();
	//virtual bool handProcUp();

};
#endif
