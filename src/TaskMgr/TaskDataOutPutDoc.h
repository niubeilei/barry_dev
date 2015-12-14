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
#if 0
#ifndef AOS_TaskMgr_TaskDataOutPutDoc_h
#define AOS_TaskMgr_TaskDataOutPutDoc_h

#include "TaskMgr/TaskData.h"


class AosTaskDataOutPutDoc : public AosTaskData
{
	int				mPhysicalId;
	int				mServerId;
	//OmnString		mKey;
	OmnString		mDataColId;
	u64				mSizeid;
	int				mRecordLen;
	u64				mFileId;
	bool			mIsFixed;

public:
	AosTaskDataOutPutDoc(const bool flag);
	AosTaskDataOutPutDoc(
			const int physical_id,
			const int server_id,
			const OmnString &key,
			const u64 &sizeid,
			const int record_len,
			const u64 &file_id,
			const bool is_fixed);
	~AosTaskDataOutPutDoc();

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

#endif
