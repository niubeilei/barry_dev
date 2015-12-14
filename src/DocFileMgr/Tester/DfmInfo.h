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
#ifndef Aos_DocFileMgr_Tester_DfmInfo_h
#define Aos_DocFileMgr_Tester_DfmInfo_h 

#include "Thread/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DocFileMgr/Tester/DocInfo.h"
#include "DocFileMgr/Tester/Ptrs.h"
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
	u64				mMaxDocid;
	AosDocFileMgrObjPtr mDfm;
	map<u32, AosSnapInfoPtr>	mSnaps;
	map<u64, AosDocInfoPtr> 	mDocInfo;
	u32				mTotalSnapNum;
	AosDfmDocPtr	mTmpDoc;

public:
	AosDfmInfo(
		const AosRundataPtr &rdata,
		AosDfmConfig &config);
	~AosDfmInfo();
	
	bool	addDoc(const AosRundataPtr &rdata);
	bool 	randModifyDoc(const AosRundataPtr &rdata);
	bool 	randDeleteDoc(const AosRundataPtr &rdata);
	bool 	randReadDoc(const AosRundataPtr &rdata);

	bool 	addDocToSnap(const AosRundataPtr &rdata);
	bool 	randModifyDocFromSnap(const AosRundataPtr &rdata);
	bool 	randDeleteDocFromSnap(const AosRundataPtr &rdata);
	bool 	randReadDocFromSnap(const AosRundataPtr &rdata);

	AosSnapInfoPtr randGetSnap();
	bool 	addSnapShot(const u32 snap_id);
	bool 	rollback(const AosSnapInfoPtr &snap);
	bool 	commit(const AosSnapInfoPtr &snap);
	bool 	merge(
				const AosSnapInfoPtr &target_snap,
				const AosSnapInfoPtr &merger_snap);
	
	u32		getSnapNum(){ return mSnaps.size(); };

	void 	snapWriteLock();
	void 	snapUnLock();
	
	bool 	hasMemory();
	bool 	sanitycheck(const AosRundataPtr &rdata);
	
	AosDocFileMgrObjPtr getDocFileMgr(){ return mDfm; };
	
	AosDocInfoPtr 	randGetDocInfo();

	bool 	serializeFrom(const AosBuffPtr &buff);
	bool 	serializeTo(const AosBuffPtr &buff);

};

#endif
