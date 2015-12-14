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
// Created: 2015-09-04 by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_ServiceProcess_h
#define AOS_SvrProxyMgr_ServiceProcess_h

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

class AosServiceProcess : public AosProcess 
{
public:
	AosServiceProcess(const bool reg_flag);
	AosServiceProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log);
	~AosServiceProcess();

	virtual AosProcessPtr clone(
				const AosProcessMgrPtr &proc_mgr,
				const AosSvrProxyPtr &proxy,
				const int logic_pid,
				const ProcInfo &proc_conf,
				const OmnString &tmp_dir,
				const bool auto_start,
				const bool show_log);

	//virtual bool setStorageDir(const AosXmlTagPtr &config);
	//virtual OmnString getStrBaseDir();
	virtual bool cleanResource();
private:
	OmnString getStrBaseDir(const AosProcessType::E type);

};
#endif
