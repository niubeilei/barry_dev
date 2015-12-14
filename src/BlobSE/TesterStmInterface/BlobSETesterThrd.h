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
// 2015-11-20 Created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_TesterStmInterface_BlobSETester_h
#define Aos_BlobSE_TesterStmInterface_BlobSETester_h

#include "BlobSE/Ptrs.h"
#include "BlobSE/TesterStmInterface/Ptrs.h"
#include "Debug/Debug.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include <ext/hash_map>
#include <map>
#include <vector>
#include "aosUtil/Types.h"
#include "BlobSE/TesterStmInterface/BlobSETesterController.h"
#include "UtilData/BlobSEReqEntry.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosBlobSETesterThrd : virtual public OmnRCObject,
							public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum Opr
	{
		eCreate	= 0,
		eDelete	= 1,
		eModify	= 2,
		eRead	= 3,
		eApply	= 4
	};

	struct DocOpr
	{
	    int     seed;
	    int     repeat;
	    u64     timestamp;
	    u64     oprid;
	    Opr     opr;

	    bool operator ==(const u64 ullOprid)
		{
	    	return oprid == ullOprid;
		}
	};

	struct DocOprInfo
	{
		DocOprInfo()
		{
			mMinOprID = 0xFFFFFFFFFFFFFFFFULL;
		}
		u64				mMinOprID;
	    vector<DocOpr>  operations;

	    void addOpr(const DocOpr &s)
	    {
	    	if (s.oprid < mMinOprID)
			{
				mMinOprID = s.oprid;
			}
	    	operations.push_back(s);
	    }
	    bool pickOpridAndTimestamp(
	    		const u64	ullLastAppliedOprID,
	    		u64			&oprid,
				u64			&timestamp,
				Opr			&eOpr,
				bool		&bHasValidOpr)
	    {
	    	if (mMinOprID > ullLastAppliedOprID || 0 == ullLastAppliedOprID)
			{
	    		oprid = 0;
	    		timestamp = 0;
	    		bHasValidOpr = false;
	    		return true;
	    	}
	        aos_assert_r(!operations.empty(), false);
	        do {
				int idx = rand() % operations.size();
				oprid = operations[idx].oprid;
				timestamp = operations[idx].timestamp;
				eOpr = operations[idx].opr;
			} while (oprid > ullLastAppliedOprID);
	        return true;
	    }
	};

	typedef __gnu_cxx::hash_map<const u64, DocOprInfo, std::u64_hash, std::u64_cmp> map_t;
	typedef map_t::iterator itr_t;

private:
	u32		mThreadID;
	int		mCreateWeight;
	int		mDeleteWeight;
	int		mModifyWeight;
	int		mReadWeight;
	int		mApplyWeight;
	u32		mSeedCount;
	u32		mStartSec;
	u32		mTestDurationSec;
	u64		mLastCreatedDocid;
	map_t	mDocOprInfos;

	std::map<u32, OmnString>	mSeedMap;
	AosBlobSETesterController*	mController;
	AosRundataPtr				mRundata;
	AosBlobSEAPIPtr				mBlobSE;

public:
	AosBlobSETesterThrd(
			const u32					ulThreadID,
			AosBlobSEAPIPtr				&pBlobSE,
			AosBlobSETesterController*	pController);
	~AosBlobSETesterThrd();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);

private:
	bool	runOneTest();

	bool	testCreateDoc();

	bool	testModifyDoc();

	bool	testDeleteDoc();

	bool	testReadDoc();

	bool	applyChanges();

	//helper functions
	u64 createDocid();

	bool	loadSeed();

	u32		pickSeed();

	u64		pickDocid();

	bool	verifyData(
			const u64			ullDocid,
			const u64			ullOprid,
			const u64			ullTimestamp,
			const AosBuffPtr	&pBodyData);

	AosBuffPtr	createData(
			u32	&seed_id,
			u32 &repeat);

	bool saveDocOprInfo(
			const u64	ullDocid,
			const u64	ullOprid,
			const u32	ulSeed,
			const u32	ulRepeat,
			const u64	ullTimestamp,
			Opr			eOpr);

	bool genBlobSEBuff(
			const u64							ullDocid,
			AosBlobSEReqEntry::ReqTypeAndResult	eReqTypeAndResult,
			const AosBuffPtr					&pBodyData,
			AosBuffPtr							&pBuff,
			u64									&ullTimestamp);
};
#endif

