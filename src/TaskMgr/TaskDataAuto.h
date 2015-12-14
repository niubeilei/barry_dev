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
#if 0
#ifndef AOS_TaskMgr_TaskDataAuto_h
#define AOS_TaskMgr_TaskDataAuto_h

#include "TaskMgr/TaskData.h"


class AosTaskDataAuto : public AosTaskData
{
	int				mPhysicalId;
	OmnString		mFileName;
	
public:
	AosTaskDataAuto(const bool flag);
	AosTaskDataAuto(
			const OmnString &file_name,
			const int physical_id);
	~AosTaskDataAuto();

	virtual int			getPhysicalId() const {return mPhysicalId;}
	virtual OmnString	getFileName() const {return mFileName;}
	
	virtual bool serializeTo(
				AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);
	virtual bool serializeFrom(
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	virtual AosTaskDataObjPtr create() const;
	virtual AosTaskDataObjPtr create(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) const;

	virtual AosDataCacherObjPtr createDataCacher(
				const AosDataRecordObjPtr &record,
				const AosRundataPtr &rdata);
							
	virtual bool isDataExist(const AosRundataPtr &rdata);
	
private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif

#endif
