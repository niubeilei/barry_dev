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
#ifndef AOS_TaskMgr_TaskDataOutPutDocid_h
#define AOS_TaskMgr_TaskDataOutPutDocid_h

#include "TaskMgr/TaskData.h"


class AosTaskDataOutPutDocid : public AosTaskData
{
	int				mPhysicalId;
	OmnString		mKey;
	u64				mStartDocid;
	int64_t			mNum;

public:
	AosTaskDataOutPutDocid(const bool flag);
	AosTaskDataOutPutDocid(
			const int physical_id,
			const OmnString &key,
			const u64 &start_docid,
			const int64_t &num);
	~AosTaskDataOutPutDocid();

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
