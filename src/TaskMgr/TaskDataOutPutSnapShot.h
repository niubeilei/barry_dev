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
#ifndef AOS_TaskMgr_TaskDataOutPutSnapShot_h
#define AOS_TaskMgr_TaskDataOutPutSnapShot_h

#include "TaskMgr/TaskData.h"
#include "UtilData/ModuleId.h"


class AosTaskDataOutPutSnapShot : public AosTaskData
{
	u32				mVirtualId;
	u32				mSnapShotId;

public:
	AosTaskDataOutPutSnapShot(const bool flag);
	AosTaskDataOutPutSnapShot(
			const u32 virtual_id,
			const u32 snapshot_id);
	~AosTaskDataOutPutSnapShot();

	virtual u64			getSnapShotId() { return mSnapShotId; }

	virtual bool		serializeTo(
							AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);
	virtual bool		serializeFrom(
							const AosXmlTagPtr &xml,
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
