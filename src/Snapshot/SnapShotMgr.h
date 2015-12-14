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
#ifndef AOS_Snapshot_SnapShotMgr_h
#define AOS_Snapshot_SnapShotMgr_h

#include "DocFileMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Snapshot/Ptrs.h"
#include "Snapshot/SnapShot.h"
#include "Snapshot/SnapShotType.h"
#include "Snapshot/SnapShotInfo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"

#define AOS_SNAPSHOT_DIRNAME_PRE    "snap_"

class AosSnapShotMgr : virtual public OmnRCObject 
{
	OmnDefineRCObject;

	enum 
	{
		eEntryStart = 0, 
		eEntryMinSize = AosSnapShotInfo::eInfoMinSize,
		eEntryMaxSize = 1000, //1k
	};

	typedef map<u32, AosSnapShotInfoPtr> map_t;
	typedef map<u32, AosSnapShotInfoPtr> ::iterator mapitr_t;

private:
	u32					mEntrySize;
	OmnFilePtr			mCtrlFile;
	u32					mCrtSnapSeqno;

	map_t				mSnapShot;
	OmnMutexPtr			mLock;
	AosSnapShotType::E	mType;

	bool				mShowLog;	

public:
	AosSnapShotMgr(
		const AosSnapShotType::E &type,
		const u32 entry_size,
		const bool show_log);

	~AosSnapShotMgr();

	AosSnapShotPtr retrieveSnapShot(
			const AosRundataPtr &rdata,
			const u64 snap_id,
			const u32 virtual_id,
			const u32 dfm_id,
			AosSnapShotInfoPtr &info,
			const bool create_flag);

	bool addSnapShot(
			const AosRundataPtr &rdata,
			u64 &snap_id);

	bool updateSnapShotInfo(
			const AosRundataPtr &rdata,
			const AosSnapShotInfoPtr &info,
			const AosSnapShot::Status sts,
			const u32 virtual_id,
			const u32 dfm_id);

	bool rollBack(	
			const AosRundataPtr &rdata,
			const AosSnapShotInfoPtr &info,
			const AosSnapShot::Status sts,
			const u32 virtual_id,
			const u32 dfm_id);

	bool clearData();

	bool removeSnapShotInfo(const u32 snap_seqno);

	void updateSnapShotStatus(
		const AosSnapShot::Status sts,
		const AosSnapShotInfoPtr &info);

	static OmnString getSnapShotDirName(const u64 snap_id);

	AosBuffPtr 	readFromFile(const u32 snap_seqno);

	bool saveToFile(const u32 snap_seqno, const AosBuffPtr &buff);

	bool removeToFile(const u32 snap_seqno);

	u32 getEntrySize() {return mEntrySize;}

private:
	void  init();

	AosSnapShotPtr retrieveSnapShotLocked(
			const AosRundataPtr &rdata,
			const u32 snap_seqno,
			const u32 virtual_id,
			const u32 dfm_id,
			AosSnapShotInfoPtr &info,
			const bool create_flag);
};
#endif
