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
#include "SvrProxyMgr/AdminProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/ProcessMgr.h"

#include <unistd.h>


AosAdminProcess::AosAdminProcess(const bool reg_flag)
:
AosProcess(AosProcessType::eAdmin, AOSPROCESS_ADMIN, reg_flag)
{
}

//AosAdminProcess::AosAdminProcess(
//		const AosProcessMgrPtr &proc_mgr,
//		const int logic_pid,
//		const OmnString &tmp_dir,
//		const bool auto_start,
//		const bool show_log)
//:
//AosProcess(proc_mgr, logic_pid, AosProcessType::eAdmin,
//		tmp_dir, auto_start, show_log)
//{
//}


AosAdminProcess::AosAdminProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		int argc,
		char **argv,
		const bool show_log)
:
AosProcess(proc_mgr, proxy, logic_pid, AosProcessType::eAdmin,
		"admin.exe", tmp_dir, auto_start, show_log)
{
	initAdminProcArgs(argc, argv);
}


AosAdminProcess::~AosAdminProcess()
{
}

	
AosProcessPtr 
AosAdminProcess::clone(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
{
	OmnShouldNeverComeHere;
	return 0;
	//return OmnNew AosAdminProcess(proc_mgr,
	//	logic_pid, tmp_dir, auto_start, show_log);
}


bool
AosAdminProcess::initAdminProcArgs(int argc, char **argv)
{
	mArgsNum = argc + 10;
	mArgs = OmnNew char*[mArgsNum];
	memset(mArgs, 0, sizeof(char*) * mArgsNum);
	
	OmnString exe_name = getExeName();
	aos_assert_r(exe_name != "", false);
	//mArgs[0] = (char *)exe_name.data();
	mArgs[0] = OmnNew char[exe_name.length() + 1];
	strcpy(mArgs[0], exe_name.data());

	mCrtArgsIdx = 1;
	for(int i = 1; i<argc; i++)
	{
		if(strcmp(argv[i], "-config") == 0)
		{
			// egnore config
			i++;
			continue;
		}
		if(strcmp(argv[i], "-rdcout") == 0)
		{
			continue;
		}
			
		//mArgs[mCrtArgsIdx] = (char*)argv[i];
		mArgs[mCrtArgsIdx] = OmnNew char[strlen(argv[i]) + 1];
		strcpy(mArgs[mCrtArgsIdx], argv[i]);
		mCrtArgsIdx++;
	}
	

	//mArgs[mCrtArgsIdx] = (char*)"-base_dir";
	OmnString tmp = "-base_dir";
	mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], tmp.data());
	mCrtArgsIdx++;
	
	OmnString basedir = OmnApp::getAppBaseDir();
	basedir.setLength(basedir.length()-1);
	basedir = basedir.subString(0, basedir.find('/', true));
	
	mArgs[mCrtArgsIdx] = OmnNew char[basedir.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], basedir.data());
	mCrtArgsIdx++;
	
	//mArgs[mCrtArgsIdx] = (char*)"-tmp_data_dir";
	tmp = "-tmp_data_dir";
	mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], tmp.data());
	mCrtArgsIdx++;
	
	mArgs[mCrtArgsIdx] = OmnNew char[mTmpDir.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], mTmpDir.data());
	mCrtArgsIdx++;
	
	//mArgs[mCrtArgsIdx] = (char *)("-config");
	tmp = "-config";
	mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], tmp.data());
	mCrtArgsIdx++;
	OmnString config_path;
	OmnString config_dir = OmnApp::getConfigDir();
	if (config_dir != "")
	{
		config_path << config_dir << "/config_admin.txt";
	}
	else
	{
		config_path << "config_admin.txt";
	}
	mArgs[mCrtArgsIdx] = OmnNew char[config_path.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], config_path.data()); 
	mCrtArgsIdx++;
	
	if(mAutoStart)
	{
		//mArgs[mCrtArgsIdx] = (char *)("-alarm");
		tmp = "-alarm";
		mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1]; 
		strcpy(mArgs[mCrtArgsIdx], tmp.data());
		mCrtArgsIdx++;
	}

	aos_assert_r(mCrtArgsIdx <= mArgsNum, false);
	mArgs[mCrtArgsIdx] = 0;
	return true;	
}

	
bool
AosAdminProcess::handProcDeath()
{
	setDeath();
	if(mProActiveKill)	return true;
	
	bool rslt = notifyListenerPidDeath();
	aos_assert_r(rslt, false);
	
	// this server even don't know who am i.
	if(!mSvrProxy->svrIsLoaded()) return true;
	
	OmnString reason = "admin proc death. svr_id:";
	reason << AosGetSelfServerId() << "; proc_id:" << getLogicPid();
	rslt = mProcMgr->procDeathTriggerResend(reason);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosAdminProcess::handProcUp()
{
	bool rslt = notifyListenerPidUp();
	aos_assert_r(rslt, false);

	// this server even don't know who am i.
	if(!mSvrProxy->svrIsLoaded())	return true;
	
	rslt = procUpTriggerResend(); 
	aos_assert_r(rslt, false);
	return true;
}


