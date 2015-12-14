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
#ifndef Aos_RlbTester_DiskErr_DiskErrTester_h
#define Aos_RlbTester_DiskErr_DiskErrTester_h 

#include "Rundata/Ptrs.h"
#include "RlbTester/DiskErrTorturer/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosDiskErrTester: public OmnRCObject
{
	OmnDefineRCObject;
	
	enum
	{
		eSvrNum = 2,
		eBkpSvrNum = 1,
		
		eMasterWeight = 70,
		eBkpWeight = 30,
	};

public:
	enum SvrOpr
	{
		eInvalid,

		eDamageDisk,
		eHandlerDiskDamaging,
		eKillProc,
		eStartProc,
		eNoOpr,
	};

private:
	AosRundataPtr	mRdata;
	AosTesterSvrInfoPtr		mSvrs[eSvrNum];
	u32		mConfigMasterIdx;
	u32		mCrtMasterIdx;

public:
	AosDiskErrTester();
	~AosDiskErrTester();

	bool	start();
	bool 	killProcCheck(const int svr_id, const u32 proc_id, bool &can_kill);
	bool 	canDamageDisk();
	bool 	startProcCheck(const int svr_id, const u32 proc_id, bool &can_start);
	
private:
	bool 	basicTest();
	bool 	damageDisk(const AosTesterSvrInfoPtr &svr);
	bool 	handlerDiskDamaging(const AosTesterSvrInfoPtr &svr);
	bool 	killProc(const AosTesterSvrInfoPtr &svr);
	bool 	startProc(const AosTesterSvrInfoPtr &svr);

	bool 	setProcDeathedPriv(const AosTesterSvrInfoPtr &svr);
	AosTesterSvrInfoPtr randGetSvr();
	int 	getConfigMasterSvrId();
	int 	getCrtMasterSvrId();
	u32 	getCrtMasterProcId();
	bool 	isSwitchToMasterFinish(const int svr_id, const u32 proc_id, bool &finish);

};
#endif
