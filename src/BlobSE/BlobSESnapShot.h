////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/03/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_BlobSESnapShot_BlobSESnapShot_h
#define AOS_BlobSESnapShot_BlobSESnapShot_h

#include "BlobSE/BlobSE.h"
#include "BlobSE/Ptrs.h"
#include "Util/String.h"
#include "Thread/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include <map>
#include <set>
#include <vector>
using namespace std;


struct AosSnapshotStatus
{
	enum E
	{
		eActive,
		ePreCommitted,
		eCommitted,
		//eRollbacked,
		eCanceled,
		eSubmitted,
	};
	
	static OmnString toStr(const E &sts)
	{
		switch(sts)
		{
		case eActive:
			return "Active";

		case ePreCommitted:
			return "PreCommitted";
	
		case eCommitted:
			return "Committed";
		
		case eCanceled:
			return "Canceled";
		
		case eSubmitted:
			return "Submitted";

		default:
			break;
		}
			
		return "";
	}
};

class AosBlobSESnapshot : public OmnRCObject
{
	OmnDefineRCObject;
	
private:
	//////////////////////////////////////////
	// Basic members
	//////////////////////////////////////////
	u64						mSnapId;
	//struct AosBlobSE::Config		mBlobSEConf;
	OmnRwLockPtr			mStatusLock;
	AosSnapshotStatus::E	mStatus;
	u32						mCommittedTimeStamp;
	OmnRwLockPtr			mHeadersLock;
	OmnRwLockPtr			mExtraHeadersLock;
	bool					mExtraEntryCommitted;
	OmnMutexPtr				mChildSnapsLock;
	u32						mParentSnapId;

	////////////////////////////////////////// 
	// header relevant info
	////////////////////////////////////////// 
	//docid --> header
	map<u64, AosBlobHeaderPtr>	mDocidHeaderMap;
	set<u64>				mAllDocids;
	vector<AosBlobHeaderPtr> mDeletedHeaders;

	map<u64, u32>			mExtraDocids;
	vector<AosBlobHeaderPtr> mExtraHeaders;
	map<u32, AosBlobSESnapshotPtr> mChildSnaps;

	//////////////////////////////////////////            
	////snapshot relevant file information                  
	////////////////////////////////////////////            
	AosRawFilePtr			mLogFile1;
	AosRawFilePtr			mLogFile2;
	AosRawFilePtr			mCrtActiveFile;

public:
	//////////////////////////////////////////
	// constructors/destructors 
	//////////////////////////////////////////
	AosBlobSESnapshot(const u64 snap_id);
	//		const struct AosBlobSE::Config &blobSE_conf);
	~AosBlobSESnapshot();
	
	static AosBlobSESnapshotPtr createSnapshotStatic(
				const AosRundataPtr &rdata,
				const struct AosBlobSE::Config &blobSE_conf,
				const AosRawSEPtr &rawSE,
				const u64 snap_id,
				const u64 raw_fileid_1,
				const u64 raw_fileid_2);

	static AosBlobSESnapshotPtr createSnapshotStatic(
				const AosRundataPtr &rdata,
				const struct AosBlobSE::Config &blobSE_conf,
				const AosRawSEPtr &rawSE,
				const AosBuffPtr &buff);

	//////////////////////////////////////////
	// getters/setters 
	//////////////////////////////////////////
	u32		getSnapId(){ return mSnapId; };
	u32		getCommittedTimeStamp(){ return mCommittedTimeStamp; };
	AosSnapshotStatus::E getStatus(){ return mStatus; };

	bool	setCommitted(
				const AosRundataPtr &rdata,
				const u32 commit_time_stamp);

	bool 	setPreCommitted(const AosRundataPtr &rdata);
	
	bool 	setCanceled(const AosRundataPtr &rdata);

	bool 	setSubmitted(
				const AosRundataPtr &rdata,
				const u32 p_snap_id);

	bool	releaseDeletedHeaders();
	//////////////////////////////////////////
	// Doc/header APIs
	//////////////////////////////////////////
	bool	readEntry(
				const AosRundataPtr &rdata,
				const u64 docid,
				AosBlobHeaderPtr &header);

	bool 	appendEntryCheckStatus(
				const AosRundataPtr &rdata,
				const u64 docid,
				const AosBlobHeaderPtr &new_header,
				AosBlobHeaderPtr &old_header);
	
	bool	appendEntry(
				const AosRundataPtr &rdata,
				const u64 docid,
				const AosBlobHeaderPtr &new_header,
				AosBlobHeaderPtr &old_header);

	bool 	readCommittedEntry(
				const AosRundataPtr &rdata,
				const u64 docid,
				AosBlobHeaderPtr &header);

	bool 	readAllDocids(
				const AosRundataPtr &rdata,
				set<u64> &docid_set);

	bool	readAllEntrys(
				const AosRundataPtr &rdata,
				vector<AosBlobHeaderPtr> &vt_headers);

	bool 	serializeMetaInfo(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff);


	//////////////////////////////////////////
	// Extra Doc/header APIs
	//////////////////////////////////////////
	
	void 	startCommitExtraEntry();

	bool	readAllExtraEntrys(
				const AosRundataPtr &rdata,
				vector<AosBlobHeaderPtr> &vt_headers);

	void 	commitExtraEntryFinish();
	
	bool	appendExtraEntry(
				const AosRundataPtr &rdata,
				const u64 docid,
				const AosBlobHeaderPtr &new_header,
				bool &succ);

	 map<u64, AosBlobHeaderPtr> getHeaderMap() { return mDocidHeaderMap; }
	 vector<AosBlobHeaderPtr> getDeletedHeaderList() { return mDeletedHeaders; }

	//////////////////////////////////////////
	// parent/child snapshots
	//////////////////////////////////////////
	bool 	addChildSnapshot(
				const AosRundataPtr &rdata,
				const AosBlobSESnapshotPtr &submit_snap);
	bool	removeChildSnapshot(
				const AosRundataPtr &rdata,
				const u64 snap_id);
	
	u32		getParentSnapId(){ return mParentSnapId; };

	//////////////////////////////////////////
	//// File APIs                              
	////////////////////////////////////////////
	bool	saveToFile(const AosRundataPtr &rdata);

	static bool removeFilesStatic(
				const AosRundataPtr &rdata,
				const struct AosBlobSE::Config &blobSE_conf,
				const AosRawSEPtr &rawSE,
				const AosBuffPtr &buff,
				u64 &snap_id);

	bool 	removeFiles(
				const AosRundataPtr &rdata,
				const struct AosBlobSE::Config &blobSE_conf,
				const AosRawSEPtr &rawSE);

private:
	//////////////////////////////////////////
	// internal File APIs
	//////////////////////////////////////////
	bool 	init(
				const AosRundataPtr &rdata,
				const struct AosBlobSE::Config &blobSE_conf,
				const AosRawSEPtr &rawSE,
				const u64 raw_fileid_1,
				const u64 raw_fileid_2,
				const u64 crt_active_fileid);

	bool 	init(
				const AosRundataPtr &rdata,
				const struct AosBlobSE::Config &blobSE_conf,
				const AosRawSEPtr &rawSE,
				const u64 raw_fileid_1,
				const u64 raw_fileid_2);

	AosRawFilePtr getRawFile(
				const AosRundataPtr &rdata, 
				const AosRawSEPtr &rawSE,
				const struct AosBlobSE::Config &blobSE_conf,
				const u64 raw_fileid, 
				const bool create_flag);

	bool 	initFromFile(
				const AosRundataPtr &rdata,
				const AosRawFilePtr &raw_file);

};
#endif
