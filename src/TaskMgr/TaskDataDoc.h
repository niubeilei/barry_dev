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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_TaskDataDoc_h
#define AOS_TaskMgr_TaskDataDoc_h

#include "TaskMgr/TaskData.h"


class AosTaskDataDoc : public AosTaskData
{
	int				mPhysicalId;
	OmnString		mDocScannerId;

public:
	AosTaskDataDoc(const bool flag);
	AosTaskDataDoc(
			const OmnString &scanner_id,
			const int physical_id);
	~AosTaskDataDoc();

	virtual int			getPhysicalId() const {return mPhysicalId;}

	virtual AosTaskDataObjPtr create() const;
	virtual AosTaskDataObjPtr create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) const;

	virtual AosDataCacherObjPtr createDataCacher(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif
