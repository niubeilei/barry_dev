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
#ifndef AOS_Snapshot_SnapShotIILTrans_h
#define AOS_Snapshot_SnapShotIILTrans_h

#include "DfmUtil/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Snapshot/SnapShot.h"
#include "Snapshot/Ptrs.h"
#include "Thread/ThrdShellProc.h"

struct AosSnapShotIILTrans : virtual public AosSnapShot 
{
private:
	u64						mSnapId;
	u32						mVirtualId;
	u32						mDfmId;
	AosDfmLogFilePtr		mDfmLogFile;	
	Status					mSts;
	bool					mShowLog;


	struct RollBackTrans : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosRundataPtr 			mRundata;
		AosSnapShotPtr			mCaller;

		RollBackTrans(const AosRundataPtr &rdata, const AosSnapShotPtr &caller)
		:
		OmnThrdShellProc("RollBackTrans"),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
		mCaller(caller)
		{
		}

		bool run()                
		{
			mCaller->rollBackFinishTransId(mRundata);
			return true;
		}

		bool procFinished()
		{
			return true;
		}
	};
public:
	AosSnapShotIILTrans(
			const u64 snap_id,
			const u32 virtual_id,
			const u32 dfm_id,
			const Status sts,
			const bool show_log);

	~AosSnapShotIILTrans();

	virtual bool addEntry(
			const AosRundataPtr &rdata,
			const AosDfmDocPtr &doc,
			vector<AosTransId> &trans_ids);

	virtual AosDfmDocPtr readDoc(
			const AosRundataPtr &rdata,
			const u64 &docid);

	virtual bool rollBack(const AosRundataPtr &rdata);

	virtual bool removeSnapShot(const AosRundataPtr &rdata);

	virtual bool commit(const AosRundataPtr &rdata, const AosDocFileMgrObjPtr &docFileMgr);

	virtual AosSnapShot::Status getStatus(){return mSts;}

	virtual bool clearData();

	virtual bool recoverCommit(const u32 siteid);

	virtual bool rollBackFinishTransId(const AosRundataPtr &rdata);

	virtual bool recoverRollBack(const u32 siteid);

private:
	bool getFileIdsBySnapShotDir(
			const u32 siteid,
			vector<u64> &file_ids);

	bool procRollBackTransId(const u32 siteid);
};
#endif
