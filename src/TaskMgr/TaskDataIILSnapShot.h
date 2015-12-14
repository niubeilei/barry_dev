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
// 2013/04/26 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_TaskDataIILSnapShot_h
#define AOS_TaskMgr_TaskDataIILSnapShot_h

#include "TaskMgr/TaskData.h"

class AosTaskDataIILSnapShot : public AosTaskData
{
	u32					mVirtualId;
	u64					mSnapShotId;
	u64					mTaskDocid;
	bool				mIsCommit;
	set<OmnString>		mIILNames;

public:
	AosTaskDataIILSnapShot(const bool flag);
	AosTaskDataIILSnapShot(
			const u32 virtual_id,
			const u64 snapshot_id,
			const set<OmnString> &iilnames,
			const u64 &task_docid,
			const bool iscommit);

	~AosTaskDataIILSnapShot();

	virtual u64			getSnapShotId() const  { return mSnapShotId; }

	virtual u32			getVirtualId() const { return mVirtualId; }

	virtual set<OmnString> getIILNames() const { return mIILNames; }

	virtual u64			getTaskDocid() const { return mTaskDocid; }
	
	virtual bool		getIsCommit() const { return mIsCommit; }

	virtual bool		serializeTo(
							AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);
	virtual bool		serializeFrom(
							const AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);

	virtual bool		serializeTo(
							AosBuffPtr &buff,
							const AosRundataPtr &rdata);
	virtual bool		serializeFrom(
							const AosBuffPtr &buff,
							const AosRundataPtr &rdata);

	virtual AosTaskDataObjPtr create() const;

	virtual AosTaskDataObjPtr create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) const;

	virtual bool isTheSameTaskData(
						const AosTaskDataObjPtr &task_data,
						const AosRundataPtr &rdata);

	virtual OmnString getStrKey(const AosRundataPtr &rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif
