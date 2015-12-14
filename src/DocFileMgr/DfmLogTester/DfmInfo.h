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
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_DfmLogTester_DfmInfo_h
#define Aos_DocFileMgr_DfmLogTester_DfmInfo_h 

#include "Thread/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/DfmLogTester/DocInfo.h"
#include "DocFileMgr/DfmLogTester/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include "Thread/Mutex.h"

#include <set>
#include <vector>
using namespace std;


class AosDfmInfo : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eTotalMem = 5 * 1000 * 1000,		// 5G
		eMaxDocs = (eTotalMem / sizeof(AosDocInfo) ) * 1000,
	};

private:
	OmnMutexPtr		mLock;
	OmnRwLockPtr	mSnapLock;
	u32				mVId;
	u32				mSiteid;
	u32				mDfmId;
	AosDocFileMgrObjPtr mDfm;
	map<u32, AosSnapInfoPtr>	mSnaps;
	map<u64, AosDocInfoPtr> 	mDocInfo;
	u32			 	mCrtDocInfoNum;
	u32				mTotalSnapNum;

public:
	AosDfmInfo();
	AosDfmInfo(
		const AosRundataPtr &rdata,
		const u32 vid, 
		AosDfmConfig &config);
	~AosDfmInfo();
	
	AosDocInfoPtr addDoc(const AosRundataPtr &rdata, const u64 docid);
	bool 	randModifyDoc(const AosRundataPtr &rdata);
	bool 	randDeleteDoc(const AosRundataPtr &rdata);
	bool 	randReadDoc(const AosRundataPtr &rdata);

	AosSnapInfoPtr randGetSnap();
	bool 	addSnapShot(const u32 snap_id);
	bool 	removeSnapShot(const AosSnapInfoPtr &snap);
	bool 	rollBack(const AosSnapInfoPtr &snap);
	bool 	commit();

	void 	snapWriteLock();
	void 	snapUnLock();
	u32		getSnapNum(){ return mSnaps.size(); };
	bool 	canAddSnap(){ return mCrtDocInfoNum < eMaxDocs; }
	bool 	checkRollBack(const AosRundataPtr &rdata);
	
	AosDocFileMgrObjPtr getDocFileMgr(){ return mDfm; };

	bool 	serializeFrom(
				const AosBuffPtr &buff,
				AosDfmConfig &config);
	bool 	serializeTo(const AosBuffPtr &buff);

	u64		getDfmInfoId()
	{
		u64 dfm_info_id = mVId;
		dfm_info_id = (dfm_info_id << 32) + mSiteid;
	};

	u32		getVirtualId(){ return mVId; };
	u32		getDfmId(){ return mDfmId; };

	u64		getMaxDocid()
	{
		map<u64, AosDocInfoPtr>::reverse_iterator ritr = mDocInfo.rbegin();
		if(ritr == mDocInfo.rend())	return 0;
		
		return ritr->first;
	}
	void stop();

private:
	AosDocInfoPtr 	randGetDocInfo();

};

#endif
