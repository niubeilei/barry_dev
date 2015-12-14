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
#ifndef Aos_Snapshot_Testers_DfmNormalDocTester_h
#define Aos_Snapshot_Testers_DfmNormalDocTester_h

#include "DocFileMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Snapshot/Testers/DfmTester.h"
#include "Snapshot/Testers/Ptrs.h"
#include "Util/TransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"

class AosDfmNormalDocTester : virtual public AosDfmTester 
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
	u64					mTaskDocid;
	map<int, u64>		mSnapIds;
	AosRundataPtr		mRdata;
	u64					mTotalNumDocs;
	u64					mNumDocs;
	u64					mBatchAddNum;
	map<int, Status>	mSts;

	vector<u32> 		mVirtualIds;
	AosNormalDocPtr mNormalObj;
	u64					mValue;
	

public:
	AosDfmNormalDocTester();
	AosDfmNormalDocTester(
		const AosRundataPtr &rdata);
	AosDfmNormalDocTester(
		const u32 logicid,
		const AosRundataPtr &rdata);
	~AosDfmNormalDocTester();

	bool createSnapshot();
	bool rollbackSnapshot();
	bool commitSnapShot();
	bool createDocs();
	bool verify();
	virtual bool basicTest();

	bool serializeFrom(const AosBuffPtr &buff);
	bool serializeTo(const AosBuffPtr &buff);
};
#endif
