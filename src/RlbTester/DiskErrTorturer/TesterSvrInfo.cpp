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
// 2013/10/31	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "RlbTester/DiskErrTorturer/TesterSvrInfo.h"

#include "alarm_c/alarm.c"

AosTesterSvrInfo::AosTesterSvrInfo(
		const AosDiskErrTesterPtr &mgr,	
		const int svr_id,
		const u32 proc_id,
		const OmnString &disk_path)
:
mMgr(mgr),
mSvrId(svr_id),
mProcId(proc_id),
mDiskPath(disk_path),
mDiskDamaged(false),
mSvrStatus(eProcActive)
{
}

AosDiskErrTester::SvrOpr
AosTesterSvrInfo::randGetOpr()
{
	if(mSvrStatus == eProcActive)	return procActiveGetNextOpr();
	if(mSvrStatus == eDiskDamaging)	return AosDiskErrTester::eHandlerDiskDamaging;;	
	if(mSvrStatus == eProcDeathed)	return procDeathedGetNextOpr(); 

	return AosDiskErrTester::eInvalid;
}


AosDiskErrTester::SvrOpr
AosTesterSvrInfo::procActiveGetNextOpr()
{
	bool can_be_killed;
	bool rslt = mMgr->killProcCheck(mSvrId, mProcId, can_be_killed);
	aos_assert_r(rslt, AosDiskErrTester::eInvalid);
	if (!can_be_killed) return AosDiskErrTester::eNoOpr;
	
	// Check whether the backup's disk was damaged.
	bool can = mMgr->canDamageDisk();
	if(!can)
	{
		// means some svr's disk has damaged. so this svr can't damaged.
		return AosDiskErrTester::eKillProc; 
	}

	return AosDiskErrTester::eDamageDisk;
	// or return AosDiskErrTester::eKillProc;
}

AosDiskErrTester::SvrOpr
AosTesterSvrInfo::procDeathedGetNextOpr()
{
	bool can_start = false;
	bool rslt = mMgr->startProcCheck(mSvrId, mProcId, can_start);
	aos_assert_r(rslt, AosDiskErrTester::eInvalid);
	if(!can_start)	return AosDiskErrTester::eNoOpr;

	return AosDiskErrTester::eStartProc;
}

