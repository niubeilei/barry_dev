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
// Created: 2015-09-04 by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/ServiceProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

#include <unistd.h>


AosServiceProcess::AosServiceProcess(const bool reg_flag)
:
AosProcess(AosProcessType::eService, AOSPROCESS_SERVICE, reg_flag)
{
}

AosServiceProcess::AosServiceProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
AosProcess(proc_mgr, proxy, logic_pid, AosProcessType::eService, 
		AosGetProcExeName(AosProcessType::eService), tmp_dir, auto_start, show_log)
{
}


AosServiceProcess::~AosServiceProcess()
{
}


AosProcessPtr
AosServiceProcess::clone(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
{
	return OmnNew AosServiceProcess(proc_mgr, proxy,
			logic_pid, tmp_dir, auto_start, show_log);
}


OmnString
AosServiceProcess::getStrBaseDir(const AosProcessType::E type)
{
	OmnString ss = AosProcessType::toStr(type);
	ss << "_data";
	return ss;
}

bool
AosServiceProcess::cleanResource()
{
	OmnString base_dir = getBaseDir();
	OmnString cmd = "rm -r ";
	cmd << base_dir;
	system(cmd.data());
	OmnString sock_fname = mTmpDir;
	sock_fname << getChildPid() << "_" << getLogicPid() << "_" << AosProcessType::toStr(getType()) << ".sock";
OmnScreen << "===================sock name: " << sock_fname << endl;
	int rs = unlink(sock_fname.data());
	if(rs == -1)
	{
		OmnScreen << "maybe sock_fname not exist:" << sock_fname 
			<< "; errno:" << errno << endl;
	}
	return true;
}
