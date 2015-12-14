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
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/TaskProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

#include <unistd.h>


AosTaskProcess::AosTaskProcess(const bool reg_flag)
:
AosProcess(AosProcessType::eTask, AOSPROCESS_TASK, reg_flag)
{
}

AosTaskProcess::AosTaskProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
AosProcess(proc_mgr, proxy, logic_pid, AosProcessType::eTask, 
		AosGetProcExeName(AosProcessType::eTask), tmp_dir, auto_start, show_log)
{
}


AosTaskProcess::~AosTaskProcess()
{
}


AosProcessPtr
AosTaskProcess::clone(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
{
	return OmnNew AosTaskProcess(proc_mgr, proxy,
			logic_pid, tmp_dir, auto_start, show_log);
}

/*
bool
AosTaskProcess::setStorageDir(const AosXmlTagPtr &config)
{
	OmnString base_dir = config->getAttrStr("base_dir");
	base_dir = base_dir.subString(0, base_dir.find('/', true));
	OmnString storage_dir;
	storage_dir << base_dir << "/" 
				<< getStrBaseDir(AosProcessType::eFrontEnd) << "/Data";
OmnScreen << "***************" << storage_dir << endl;
	AosXmlTagPtr tag = config->getFirstChild("storage_mgr");
	aos_assert_r(tag, false);
	tag = tag->getFirstChild("storage_device");
	aos_assert_r(tag, false);
	tag = tag->getFirstChild("storage_partition");
	aos_assert_r(tag, false);
	tag->setAttr("userdir", storage_dir);
	return true;
}
*/

/*
OmnString
AosTaskProcess::getStrBaseDir()
{
	OmnString ss = AosProcess::getStrBaseDir(); 
	ss << "_" << getLogicPid();
	return ss;	
}
*/


OmnString
AosTaskProcess::getStrBaseDir(const AosProcessType::E type)
{
	OmnString ss = AosProcessType::toStr(type);
	ss << "_data";
	return ss;
}

bool
AosTaskProcess::cleanResource()
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
