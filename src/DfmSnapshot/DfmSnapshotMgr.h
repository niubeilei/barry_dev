////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2014/03/06 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmSnapshotMgr_h
#define AOS_DfmUtil_DfmSnapshotMgr_h

#include "DfmUtil/Ptrs.h"
#include "DfmSnapshot/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DfmSnapshotType.h"
#include "SEInterfaces/DfmSnapshotObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/TransId.h"

#include <map>
#include <deque>
using namespace std;

class AosDfmSnapshotMgr : public OmnThreadedObj
{
	OmnDefineRCObject;
	
	struct AosSnapReq 
	{
		AosRundataPtr rdata;
		AosDfmSnapshotObjPtr	snap;
		AosSnapshotSts::E 	snapOpr;
		AosDfmSnapshotObjPtr	snap2;
		
		AosSnapReq(const AosRundataPtr &r, const AosDfmSnapshotObjPtr &s1, AosSnapshotSts::E opr)
		:
		rdata(r->clone(AosMemoryCheckerArgsBegin)),
		snap(s1),
		snapOpr(opr),
		snap2(0)
		{
		};
		
		AosSnapReq(const AosRundataPtr &r, const AosDfmSnapshotObjPtr &s1,
				AosSnapshotSts::E opr, const AosDfmSnapshotObjPtr &s2)
		:
		rdata(r->clone(AosMemoryCheckerArgsBegin)),
		snap(s1),
		snapOpr(opr),
		snap2(s2)
		{
		};
	};

private:
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	OmnThreadPtr			mProcThrd;
	AosDocFileMgrPtr		mDfm;
	OmnString				mFileKey;
	AosReliableFilePtr		mMgrFile;
	//u64						mNextSnapId;
	map<u64, AosDfmSnapshotObjPtr>	mSnapshots;
	deque<AosSnapReq>		mSnapReqs;
	
public:
	AosDfmSnapshotMgr(const AosDocFileMgrPtr &dfm);
	~AosDfmSnapshotMgr();
	
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

	static AosDfmSnapshotMgrPtr createSnapshotMgrStatic(
				const AosRundataPtr &rdata,
				const AosDocFileMgrPtr &dfm);

	u64		createSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				const AosDfmSnapshotType::E snap_tp);

	bool 	commitSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);

	bool 	rollbackSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);

	bool 	mergeSnapshot(
				const AosRundataPtr &rdata,
				const u64 target_snap_id,
				const u64 merger_snap_id);

	bool 	readFromCommitingSnapshot(
				const AosRundataPtr &rdata,
				const u64 docid,
				bool &exist,
				AosDfmDocHeaderPtr &crt_header);

	bool 	readEntry(
				const AosRundataPtr &rdata,
				const u64 docid,
				const u64 snap_id,
				bool &exist,
				AosDfmDocHeaderPtr &crt_header);
	
	AosDfmSnapshotObjPtr getSnapshotById(const u64 snap_id);

	OmnString 	getFileKey(){ return mFileKey; };
	u64			getVirtualId();

private:	
	bool 	init(const AosRundataPtr &rdata);
	bool 	initFile(const AosRundataPtr &rdata);
	
	bool	initFromFile(const AosRundataPtr &rdata);

	bool 	startProcThrd();
	
	bool 	saveToFile(const AosRundataPtr &rdata);

	bool 	addSnapshotPriv(const AosDfmSnapshotObjPtr &snapshot);
	
	bool 	commitSnapshotPriv(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &snapshot);

	bool 	rollbackSnapshotPriv(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &snapshot);

	bool 	mergeSnapshotPriv(
				const AosRundataPtr &rdata,
				const AosDfmSnapshotObjPtr &target_snapshot,
				const AosDfmSnapshotObjPtr &merger_snapshot);

	bool 	readEntryPriv(
				const AosRundataPtr &rdata,
				const u64 docid,
				const AosDfmSnapshotObjPtr &crt_snapshot,
				bool &exist,
				AosDfmDocHeaderPtr &crt_header);

	AosDfmSnapshotObjPtr getSnapshotByIdPriv(const u64 snap_id);

};
#endif
