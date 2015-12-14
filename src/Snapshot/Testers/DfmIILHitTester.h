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
// Modification History:
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Snapshot_Testers_DfmIILHitTester_h
#define Aos_Snapshot_Testers_DfmIILHitTester_h

#include "DocFileMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Snapshot/Testers/DfmTester.h"
#include "Util/TransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"

class AosDfmIILHitTester : virtual public AosDfmTester 
{
	enum E
	{
		eInvalid,	
		eCommit,
		eRollBack,
		eNormal
	};

	struct Status
	{
		u64		start;
		u64		end;
		E		sts;	
	};

private:
	u32					mLogicid;
	OmnString			mIILName;
	u64					mTaskDocid;
	u64					mSnapId;
	int					mVirtualId;
	AosRundataPtr		mRdata;
	u64					mValue;
	int					mBatchAddNum;
	map<int, Status>	mSts;
	u64					mTotalNumDocs;
	u64					mNumDocs;
	map<u64, int>		mVerify;
	

public:
	AosDfmIILHitTester();
	AosDfmIILHitTester(
		const AosRundataPtr &rdata);
	AosDfmIILHitTester(
		const u32 logicid,
		const int num,
		const AosRundataPtr &rdata);
	~AosDfmIILHitTester();

	bool createSnapshot();
	bool rollbackSnapshot();
	bool commitSnapShot();
	bool hitBatchAdd();
	virtual bool basicTest();
	bool runQuery();
	bool verify();

	bool serializeFrom(const AosBuffPtr &buff);
	bool serializeTo(const AosBuffPtr &buff);
};
#endif
