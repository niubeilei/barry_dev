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
// 09/04/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmSaveCacher_h
#define AOS_DocFileMgr_DfmSaveCacher_h

#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmDoc.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include <vector>
#include <map>
#include <set>
using namespace std;

class AosDfmSaveCacher: public OmnRCObject 
{
	OmnDefineRCObject;

public:

	enum
	{
		//eMaxCacheSize = 1000,
		eMaxCacheSize = 30,
		//eHeaderNumPerRead = 200,
		eCacheNum = 2,
	};

private:
	OmnRwLockPtr				mLock[eCacheNum];
	map<u64, AosDfmDocPtr>		mDocCache[eCacheNum];
	vector<u64>					mTransIds[eCacheNum];
	int							mCrtSlabFileSeqno[eCacheNum];
	AosDiskBlock2Ptr			mCrtDiskBlock[eCacheNum];
	u32							mCrtWriteIdx;

	u64							mVirtualId;
	u32							mDfmId;
	u32							mSiteid;
	AosDfmProcPtr				mProc;
	AosRundataPtr				mRdata;
	//AosDocFileMgrPtr			mDocFileMgr;

	bool						mShowLog;
public:
	AosDfmSaveCacher(const u32 virtual_id,
			const u32 dfm_id,
			const AosDfmProcPtr &proc);
	~AosDfmSaveCacher();

	bool	stop(const AosDocFileMgrPtr &dfm);

	bool 	addEntry(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				const vector<u64> &trans_ids,
				const AosDfmDocPtr &doc,
				const bool flushflag);

	AosDfmDocPtr readDoc(
				const AosRundataPtr &rdata,
				const u64 docid);

private:
	bool 	saveToFileLocked(
				const AosRundataPtr &rdata,
				const u32 crt_saveidx,
				const AosDocFileMgrPtr &dfm);
	bool 	readHeaderInfo(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				map<u64, AosDfmDocPtr> &cahceMap);
	bool 	saveHeaderToFile(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				set<AosDfmDocPtr, AosHeaderCmpFunc> &headerCache);

	bool 	saveBodyToFile(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				const u32 crt_saveidx,
				set<AosDfmDocPtr, AosBodyCmpFunc> &bodyCache,
				set<AosDfmDocPtr, AosHeaderCmpFunc> &headerCache);

	bool 	procSaveEachBody(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				const u32 crt_saveidx,
				const AosDfmDocPtr &doc, 
				bool &kickout);

	bool 	procModifyDiskBodyPriv(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				bool &kickout,
				const u32 fseqno,
				u64 foffset,
				const AosBuffPtr &body_data,
				const u32 crt_saveidx);

	bool 	procModifySlabBodyPriv(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				bool &kickout,
				const u32 fseqno,
				const u64 foffset,
				const AosBuffPtr &body_data,
				const u32 crt_saveidx);

	bool 	procDeleteEachBody(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				const u32 crt_saveidx,
				const AosDfmDocPtr &doc);
	
	bool	procDeleteDiskBodyPriv(
				const AosRundataPtr &rdata, 
				const AosDocFileMgrPtr &dfm,
				const u32 fseqno, 
				const u64 foffset,
				const u32 crt_saveidx);

	bool	procDeleteSlabBodyPriv(
				const AosRundataPtr &rdata, 
				const AosDocFileMgrPtr &dfm,
				const u32 fseqno, 
				const u64 foffset,
				const u32 crt_saveidx);

	bool 	procAddNewBodys(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				const u32 crt_saveidx,
				vector<AosDfmDocPtr> &kickoutCache,
				set<AosDfmDocPtr, AosHeaderCmpFunc> &headerCache);

	bool 	procAddDiskBodyPriv(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				u32 &new_fseqno,
				u64 &new_foffset, 
				const AosBuffPtr &body_data, 
				const u32 crt_saveidx);
	
	bool 	procAddSlabBodyPriv(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm,
				u32 &new_fseqno, 
				u64 &new_foffset, 
				const AosBuffPtr &body_data,
				const u32 crt_saveidx);
};

#endif
