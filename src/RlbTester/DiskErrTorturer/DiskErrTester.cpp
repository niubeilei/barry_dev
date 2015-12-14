////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "RlbTester/DiskErrTorturer/DiskErrTester.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "RlbTester/DiskErrTorturer/TesterSvrInfo.h"
#include "Thread/Thread.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util/File.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "SysMsg/KillProcMsg.h"
#include "RlbTester/TesterTrans/RlbTesterStartProcMsg.h"
#include "RlbTester/TesterTrans/RlbTesterDamageDiskTrans.h"
#include "RlbTester/TesterTrans/RlbTesterIsProcDeathedTrans.h"
#include "RlbTester/TesterTrans/RlbTesterSwitchMFinishTrans.h"

AosDiskErrTester::AosDiskErrTester()
{
}


AosDiskErrTester::~AosDiskErrTester()
{
}


bool 
AosDiskErrTester::start()
{
	cout << "Start DiskErr Tester ..." << endl;
		
	AosDiskErrTesterPtr thisptr(this, false);
	mSvrs[0] = OmnNew AosTesterSvrInfo(thisptr, 1, 2, "../../DataSvr1/cube_data_data_0/Data");
	mSvrs[1] = OmnNew AosTesterSvrInfo(thisptr, 2, 2, "../../DataSvr2/bkpcube_data_data_0/Data");
	mConfigMasterIdx = 0;
	mCrtMasterIdx = 0;
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(1);
	
	u32 sleep_tm;
	while(1)
	{
		//sleep_tm = (rand() % 5 + 2) * 60;
		sleep_tm = (rand() % 5 + 1) * 60;
		OmnSleep(sleep_tm);
		
		basicTest();
	}
	return true;
}


bool 
AosDiskErrTester::basicTest()
{
	// 1. random determain the opr svr.
	AosTesterSvrInfoPtr svr = randGetSvr();
	aos_assert_r(svr, false);
	
	//return true;

	SvrOpr opr = svr->randGetOpr();	
	switch(opr)
	{
	case eDamageDisk:
		 return damageDisk(svr);

	case eHandlerDiskDamaging:
		 return handlerDiskDamaging(svr);

	case eKillProc:
		 return killProc(svr);

	case eStartProc:
		 return startProc(svr);
	
	case eNoOpr:
		return true;

	default:
		break;
	}
	return false;
}

bool
AosDiskErrTester::damageDisk(const AosTesterSvrInfoPtr &svr)
{
	aos_assert_r(svr, false);
	int svr_id = svr->getSvrId();
	u32 proc_id = svr->getProcId();
	aos_assert_r(svr_id >=0, false);
	
	OmnScreen << "DiskErrTester; damage the disk"
		<< "; svr_id:" << svr_id
		<< "; proc_id:" << proc_id
		<< endl;
	
	AosTransPtr trans = OmnNew AosRlbTesterDamageDiskTrans(svr_id, proc_id);
	bool rslt = AosSendTrans(mRdata, trans);
	aos_assert_r(rslt, false);
	svr->setDiskDamaging();
	return true;
}

bool
AosDiskErrTester::handlerDiskDamaging(const AosTesterSvrInfoPtr &svr)
{
	// This function checks whether the server reported disk errors.
	//
	aos_assert_r(svr, false);
	int svr_id = svr->getSvrId();
	u32 proc_id = svr->getProcId();
	aos_assert_r(svr_id >=0, false);
	
	OmnScreen << "DiskErrTester; handler the damaged disk"
		<< "; svr_id:" << svr_id
		<< "; proc_id:" << proc_id
		<< endl;

	// Create a trans and sends it to the server. If the server 
	// detected the disk error, the proxy should have killed the
	// server. 
	AosTransPtr trans = OmnNew AosRlbTesterIsProcDeathedTrans(svr_id, proc_id);
	bool svr_death;
	bool rslt = AosSendTrans(mRdata, trans);
	aos_assert_r(rslt, false);
	if(!trans->isSvrDeath())
	{
		// If the server is still up for 'too long', it is 
		// an error.
		return true;
	}
	
	// this proc has deathed.
	// then clean this damaged Disk.
	OmnString disk_path = svr->randGetDiskPath();
	aos_assert_r(disk_path != "", false);	
	OmnString rm_cmd = "rm ";
	rm_cmd << disk_path << " -rf";
	
	OmnScreen << "DiskErrTester; clean this damaged disk"
		<< "; svr_id:" << svr_id
		<< "; proc_id:" << proc_id
		<< "; disk_path:" << disk_path
		<< "; rm_cmd:" << rm_cmd 
		<< endl;
	//OmnAlarm << "alarm test" << enderr;
	int ss = system(rm_cmd.data());
	aos_assert_r(ss == 0, false);
	
	setProcDeathedPriv(svr);
	return true;
}


bool
AosDiskErrTester::killProc(const AosTesterSvrInfoPtr &svr)
{
	aos_assert_r(svr, false);
	int svr_id = svr->getSvrId();
	u32 proc_id = svr->getProcId();
	aos_assert_r(svr_id >=0, false);
	
	OmnScreen << "DiskErrTester; kill the proc"
		<< "; svr_id:" << svr_id
		<< "; proc_id:" << proc_id
		<< endl;
	
	AosAppMsgPtr msg = OmnNew AosKillProcMsg(svr_id, proc_id);
	bool rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);

	setProcDeathedPriv(svr);
	return true;
}

bool
AosDiskErrTester::setProcDeathedPriv(const AosTesterSvrInfoPtr &svr)
{
	aos_assert_r(svr, false);	
	
	svr->setProcDeathed();
	if(svr->getSvrId() != getCrtMasterSvrId())	return true;	
	
	// reset crt master.
	int old_idx = mCrtMasterIdx;
	int next_idx = (mCrtMasterIdx + 1) % eSvrNum;
	int death_num = 0;
	while(death_num < eSvrNum)
	{
		if(!mSvrs[next_idx] || mSvrs[next_idx]->isProcDeathed())
		{
			death_num += 1;
			next_idx = (next_idx + 1) % eSvrNum;
			continue;
		}
		
		mCrtMasterIdx = next_idx; 
		break;
	}
	
	if(death_num < eSvrNum)
	{
		aos_assert_r(mSvrs[mCrtMasterIdx], false);
		OmnScreen << "DiskErrTester; newMasterIdx"
			<< "; old_idx:" << old_idx 
			<< "; crt_master_idx:" << mCrtMasterIdx 
			<< endl;
	}
	else
	{
		OmnScreen << "DiskErrTester; all proc has deathed." << endl;
	}

	return true;
}

bool
AosDiskErrTester::startProc(const AosTesterSvrInfoPtr &svr)
{
	aos_assert_r(svr, false);
	int svr_id = svr->getSvrId();
	u32 proc_id = svr->getProcId();
	aos_assert_r(svr_id >=0, false);

	OmnScreen << "DiskErrTester; start the proc"
		<< "; svr_id:" << svr_id
		<< "; proc_id:" << proc_id
		<< endl;

	AosAppMsgPtr msg = OmnNew AosRlbTesterStartProcMsg(
		svr_id, AOSTAG_SVRPROXY_PID, proc_id);
	bool rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
	
	svr->setProcActive();
	if(svr_id != getConfigMasterSvrId())	return true;
	
	// this svr is the crt master svr_id
	for(u32 i=0; i<eSvrNum; i++)
	{
		if(!mSvrs[i])	continue;	
		if(mSvrs[i] != svr)	continue;
		
		mCrtMasterIdx = i;	
		OmnScreen << "DiskErrTester; newMasterIdx"
			<< "; crt_master_idx:" << mCrtMasterIdx 
			<< endl;
		break;
	}
	return true;
}

AosTesterSvrInfoPtr
AosDiskErrTester::randGetSvr()
{
	// 1. find whether has svr is in DiskDamaging sts.
	AosTesterSvrInfoPtr svr;
	for(u32 i=0; i<eSvrNum; i++)
	{
		svr = mSvrs[i];
		if(!svr)	continue;
		if(svr->isInDiskDamagingSts())	return svr;
	}
	
	// 2. check whether all svr has deathed.
	bool all_proc_deathed = true;
	for(u32 i=0; i<eSvrNum; i++)
	{
		svr = mSvrs[i];
		if(!svr)	continue;
		if(!svr->isProcDeathed())
		{
			all_proc_deathed = false;
			break;
		}
	}
	if(all_proc_deathed)
	{
		return mSvrs[mCrtMasterIdx]; 
	}

	// 3. rand determine the svr.
	bool proc_master = (OmnRandom::percent(eMasterWeight, eBkpWeight) == 0);
	if(proc_master)	return mSvrs[mConfigMasterIdx];

	// rand get bkp svr.	
	u32 nn = rand() % eBkpSvrNum;
	u32 idx = (mConfigMasterIdx + nn) % eSvrNum;
	return mSvrs[idx];
}


bool
AosDiskErrTester::startProcCheck(const int svr_id, const u32 proc_id, bool &can_start)
{
	can_start = true;
	if(svr_id != getConfigMasterSvrId())
	{
		// the bkp svr can always start.
		return true;
	}
	
	// check whether ths crt master switchToMaster finish. if not. wait.
	// because this svr is the true master. when this svr has up. will switchToMaster.
	bool finish;
	bool rslt = isSwitchToMasterFinish(getCrtMasterSvrId(), getCrtMasterProcId(), finish);
	aos_assert_r(rslt, false);
	if(finish)		return true;

	can_start = false;
	OmnScreen << "can't start this proc. crt master not switch to master finish."
		<< "; crt_master:" << getCrtMasterSvrId() 
		<< endl;
	return true;
}


bool
AosDiskErrTester::killProcCheck(const int svr_id, const u32 proc_id, bool &can_kill)
{
	// This function checks whether the server can be 'killed'. 
	// In the current implementations, a sever cannot be killed
	// if it is the master and the time since the master was 
	// started is not long enough. 
	can_kill = true;
	bool rslt;
	if(svr_id != getCrtMasterSvrId())	
	{
		// It is not the master. It can be killed.
		return true;
	}
	
	aos_assert_r(getCrtMasterProcId() == proc_id, false);
	bool finish;

	// Check whether the master is 'stable'
	rslt = isSwitchToMasterFinish(svr_id, proc_id, finish);
	aos_assert_r(rslt, false);
	if(finish)		
	{
		// The server is stable, i.e., it can be killed.
		return true;
	}

	// The server cannot be killed.
	can_kill = false;
	OmnScreen << "can't start this proc. crt master not switch to master finish."
		<< "; crt_master:" << svr_id 
		<< endl;
	return true;
}


bool
AosDiskErrTester::canDamageDisk()
{
	AosTesterSvrInfoPtr svr;
	u32 damaged_num = 0;
	for(u32 i=0; i<eSvrNum; i++)
	{
		svr = mSvrs[i];
		if(!svr)	continue;
		if(svr->isDiskDamaged()) damaged_num++;
	}
	return damaged_num < eSvrNum-1;
}


int
AosDiskErrTester::getConfigMasterSvrId()
{
	AosTesterSvrInfoPtr svr = mSvrs[mConfigMasterIdx];
	aos_assert_r(svr, false);
	
	return svr->getSvrId();
}


int
AosDiskErrTester::getCrtMasterSvrId()
{
	AosTesterSvrInfoPtr svr = mSvrs[mCrtMasterIdx];
	aos_assert_r(svr, false);
	
	return svr->getSvrId();
}

u32
AosDiskErrTester::getCrtMasterProcId()
{
	AosTesterSvrInfoPtr svr = mSvrs[mCrtMasterIdx];
	aos_assert_r(svr, false);
	
	return svr->getProcId();
}

bool
AosDiskErrTester::isSwitchToMasterFinish(const int svr_id, const u32 proc_id, bool &finish)
{
	AosBuffPtr resp;
	bool svr_death;
	AosTransPtr trans = OmnNew AosRlbTesterSwitchMFinishTrans(svr_id, proc_id);
	bool rslt = AosSendTrans(mRdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);
	aos_assert_r(!svr_death && resp, false);
	
	finish = resp->getU8(0);
	return true;
}

