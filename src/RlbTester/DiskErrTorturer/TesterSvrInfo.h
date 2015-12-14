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
#ifndef Aos_RlbTester_DiskErr_TesterSvrInfo_h
#define Aos_RlbTester_DiskErr_TesterSvrInfo_h 

#include "Rundata/Ptrs.h"
#include "RlbTester/DiskErrTorturer/DiskErrTester.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

class AosTesterSvrInfo: public OmnRCObject
{
	OmnDefineRCObject;

	enum Sts
	{
		eInvalid,

		eProcActive,
		eProcDeathed,
		eDiskDamaging,

		eSvrDeath,
	};
	
private:
	AosDiskErrTesterPtr mMgr;
	int			mSvrId;
	u32			mProcId;
	OmnString	mDiskPath;
	bool		mDiskDamaged;
	Sts	 		mSvrStatus;

public:
	AosTesterSvrInfo(
			const AosDiskErrTesterPtr &mgr,	
			const int svr_id,
			const u32 proc_id,
			const OmnString &disk_path);
	
	Sts		getStatus(){ return mSvrStatus; };
	int		getSvrId(){ return mSvrId; };
	u32		getProcId(){ return mProcId; };
	bool	isDiskDamaged(){ return mDiskDamaged; };

	AosDiskErrTester::SvrOpr randGetOpr();
	void 	setDiskDamaging(){ mSvrStatus = eDiskDamaging; mDiskDamaged = true; };
	void 	setProcDeathed(){ mSvrStatus = eProcDeathed; };
	void 	setProcActive(){ mSvrStatus = eProcActive; mDiskDamaged = false; };
	bool	isInDiskDamagingSts(){ return mSvrStatus == eDiskDamaging; };

	OmnString randGetDiskPath(){ return mDiskPath; };
	bool	isProcDeathed(){ return mSvrStatus == eSvrDeath || mSvrStatus == eProcDeathed; };

private:
	AosDiskErrTester::SvrOpr	procActiveGetNextOpr();
	AosDiskErrTester::SvrOpr	procDeathedGetNextOpr();

};
#endif
