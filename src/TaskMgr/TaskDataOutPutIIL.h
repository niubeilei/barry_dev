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
#ifndef AOS_TaskMgr_TaskDataOutPutIIL_h
#define AOS_TaskMgr_TaskDataOutPutIIL_h

#include "TaskMgr/TaskData.h"


class AosTaskDataOutPutIIL : public AosTaskData
{
	int				mPhysicalId;
	OmnString		mDataColId;
	u64				mFileId;
	int				mLevel;
	int				mBlockIdx;
	bool			mLastMerge;

public:
	AosTaskDataOutPutIIL(const bool flag);
	AosTaskDataOutPutIIL(
			const int physical_id,
			const OmnString &datacol_id,
			const u64 &file_id,
			const int level,
			const int crt_block_idx,
			const bool last);
	~AosTaskDataOutPutIIL();

	virtual int			getPhysicalId() const {return mPhysicalId;}

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
