////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/03/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_BlobSESnapShot_BlobSESnapShotMgr_h
#define AOS_BlobSESnapShot_BlobSESnapShotMgr_h

#include "BlobSE/BlobSESnapShot.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Thread/ThrdShellProc.h"

#include <map>
#include <hash_map>
#include <deque>
using namespace std;

class AosBlobSESnapshotMgr : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		//eFileGoodFlagPos = 0,
		//eCrtSaveSeqnoPos = eFileGoodFlagPos + 4,
		eCrtSaveSeqnoPos = 0,
		eCrtSnapshotRawFileIdPos = eCrtSaveSeqnoPos + 8,
		eSnapshotPos = eCrtSnapshotRawFileIdPos + 8,

		eFinishFlag = 0xffffffff,
		eFileGoodFlag = 0x38af6b5c,
	};
	
	struct AosProcReq 
	{
		AosBlobSESnapshotPtr	snap;
		AosSnapshotStatus::E 	opr;
		AosBlobSESnapshotPtr	snap2;
		
		AosProcReq(const AosBlobSESnapshotPtr &s1, AosSnapshotStatus::E opr)
		:
		snap(s1),
		opr(opr),
		snap2(0)
		{
		};
		
		AosProcReq(const AosBlobSESnapshotPtr &s1,
				AosSnapshotStatus::E opr, const AosBlobSESnapshotPtr &s2)
		:
		snap(s1),
		opr(opr),
		snap2(s2)
		{
		};
	};
	
	class ThrdProcer : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosRundataPtr				mRdata;
		AosBlobSESnapshotMgrPtr		mMgr;
		AosProcReq					mReq;
	
	public:
		ThrdProcer(const AosRundataPtr &rdata,
				const AosBlobSESnapshotMgrPtr &mgr,
				const AosProcReq &req)
		:
		OmnThrdShellProc("BlobSESnapshotProcer"),
		mRdata(rdata->clone(AosMemoryCheckerArgsBegin)),
		mMgr(mgr),
		mReq(req)
		{}

		virtual bool run();
		virtual bool procFinished();
	};

private:
	//////////////////////////////////////////
	// Basic members
	//////////////////////////////////////////
	bool					mShowLog;
	AosBlobSEPtr			mBlobSE;
	AosRawSEPtr				mRawSE;
	OmnRwLockPtr			mSnapshotsLock;
	OmnRwLockPtr			mCommittedDocidsLock;

	//////////////////////////////////////////
	//snapshot relevant data structure
	//////////////////////////////////////////

	//active snapshot map
	map<u64, AosBlobSESnapshotPtr>	mSnapshots;
	
	//cancelled snapshot list
	vector<AosBlobSESnapshotPtr>	mCanceledSnapshots;

	//committed snapshot list
	vector<AosBlobSESnapshotPtr>	mCommittedSnapshots;
	map<u64, AosBlobSESnapshotPtr>	mCommittedSnapshotsMap;

	// docid --> snapshot id list (containing the docid)
	// this is also called PDM
	//map<u64, vector<u64> >			mDocidSnapshotMap;
	hash_map<u64, vector<u64> >			mDocidSnapshotMap;

	typedef pair<u64, vector<u64> >		docid_pair;
	//////////////////////////////////////////
	//snapshot relevant file information
	//////////////////////////////////////////
	AosRawFilePtr			mMgrFile1;
	AosRawFilePtr			mMgrFile2;
	AosRawFilePtr			mCrtActiveFile;
	u64						mCrtSaveSeqno;
	u64						mCrtSnapshotRawFileId;
	bool					mIsSaving;
	//u32						mCrtActiveFileSeq;
	
	//////////////////////////////////////////
	// snapshot merging processing
	//////////////////////////////////////////
	deque<AosProcReq>		mProcReqs;
	OmnMutexPtr				mProcReqsLock;
	OmnMutexPtr				mLock;
	AosBlobSESnapshotPtr	mIsProcingSnap;

public:
	AosBlobSESnapshotMgr(
		const AosBlobSEPtr &blobSE,
		const AosRawSEPtr &rawSE);
	~AosBlobSESnapshotMgr();

	//////////////////////////////////////////
	// snapshot APIs
	//////////////////////////////////////////
	static AosBlobSESnapshotMgrPtr createSnapshotMgrStatic(
				const AosRundataPtr &rdata,
				const AosBlobSEPtr &blobSE,
				const AosRawSEPtr &rawSE);
	
	bool	createSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);

	bool 	commitSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);

	bool 	cancelSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);


	//not work for now
	bool 	preCommitSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);

	//merge: update header
	bool 	submitSnapshot(
				const AosRundataPtr &rdata,
				const u64 target_snap_id,
				const u64 submit_snap_id);
	
	//////////////////////////////////////////
	// Doc APIs
	//////////////////////////////////////////
	bool 	readDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id, 
				const u64 docid,
				AosBlobHeaderPtr &header,
				const u64 timestamp);

	bool 	saveDoc(
				const AosRundataPtr &rdata,
				const AosBlobHeaderPtr &new_header,
				bool &finish);

	/*
	bool 	deleteDoc(
				const AosRundataPtr &rdata,
				const u64 snap_id, 
				const u64 docid,
				bool &finish);
	*/

	bool 	saveToFile(const AosRundataPtr &rdata);
	
	AosBlobSESnapshotPtr getSnapshotById(
				const u64 snap_id);

	map<u64, AosBlobSESnapshotPtr>	getSnapshots() {return mSnapshots; }

	vector<AosBlobSESnapshotPtr>    getCanceledSnapshots() {return mCanceledSnapshots; }

	vector<AosBlobSESnapshotPtr>    getCommittedSnapshots() {return mCommittedSnapshots; }
	map<u64, AosBlobSESnapshotPtr>	getCommittedSnapshotsMap() {return mCommittedSnapshotsMap; }

	//map<u64, vector<u64> >          getDocidSnapshotMap() {return mDocidSnapshotMap; }
	hash_map<u64, vector<u64> >          getDocidSnapshotMap() {return mDocidSnapshotMap; }
	
	//is commited
	bool	isCommitted(const u64 snap_id);

public:
	// ProcThreadShell will call this func.
	bool 	procReq(
				const AosRundataPtr &rdata,
				const AosProcReq &req);
	bool 	oneReqProcFinished(const AosRundataPtr &rdata);

	// BlobSESnapshot will call this.
	AosBlobSESnapshotPtr retrieveSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);

private:	
	//////////////////////////////////////////
	// File APIs
	//////////////////////////////////////////
	bool 	init(const AosRundataPtr &rdata);

	bool 	initCrtActiveFile(
				const AosRundataPtr &rdata);

	bool 	initFromFile(
				const AosRundataPtr &rdata,
				const AosRawFilePtr &file);

	bool 	isFileGood(
				const AosRundataPtr &rdata,
				const AosRawFilePtr &file);

	u64 	readSaveSeqno(
				const AosRundataPtr &rdata,
				const AosRawFilePtr &file);

	OmnFilePtr getActiveMgrFile(const AosRundataPtr &rdata);

	AosRawFilePtr getRawFile(
				const AosRundataPtr &rdata, 
				const u64 raw_fileid, 
				const bool create_flag);

	//////////////////////////////////////////
	// Snapshot request processing APIs
	//////////////////////////////////////////
	bool 	initProcReq(
				const AosRundataPtr &rdata,
				const AosBlobSESnapshotPtr &snapshot);

	bool 	addProcReq(
				const AosRundataPtr &rdata,
				const AosProcReq &req);

	bool 	startProcReqLocked(
				const AosRundataPtr &rdata,
				const AosProcReq &req);

	bool 	procCommitReq(
				const AosRundataPtr &rdata,
				const AosBlobSESnapshotPtr &snapshot);

	bool 	procCancelReq(
				const AosRundataPtr &rdata,
				const AosBlobSESnapshotPtr &snapshot);

	bool 	procSubmitReq(
				const AosRundataPtr &rdata,
				const AosBlobSESnapshotPtr &target_snapshot,
				const AosBlobSESnapshotPtr &submit_snapshot);

	bool 	commitSnapshotPriv(
				const AosRundataPtr &rdata,
				const u64 snap_id,
				vector<AosBlobHeaderPtr> &vt_headers,
				u32 commit_time_stamp = 0);

	//////////////////////////////////////////
	// internal Snapshot APIs
	//////////////////////////////////////////
	bool 	addSnapshotLocked(
				const AosBlobSESnapshotPtr &snapshot);


	bool 	removeSnapshot(
				const AosBlobSESnapshotPtr &snapshot);

	//////////////////////////////////////////
	// internal Doc APIs
	//////////////////////////////////////////
	bool 	appendCommittedDocids(
				const AosRundataPtr &rdata, 
				const AosBlobSESnapshotPtr &snapshot);

	bool 	cleanCommittedDocids(
				const AosRundataPtr &rdata, 
				const AosBlobSESnapshotPtr &snapshot);

	bool 	readEntryFromSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id, 
				const u64 docid,
				AosBlobHeaderPtr &header);
	
	bool 	readEntryFromCommittedSnapshot(
				const AosRundataPtr &rdata,
				const u64 docid,
				AosBlobHeaderPtr &header);

	bool 	appendEntryToSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id, 
				const u64 docid,
				const AosBlobHeaderPtr &new_header);

	bool 	appendEntryToCommittedSnapshot(
				const AosRundataPtr &rdata,
				const u64 docid,
				const AosBlobHeaderPtr &new_header,
				bool &finish);
	
};
#endif
