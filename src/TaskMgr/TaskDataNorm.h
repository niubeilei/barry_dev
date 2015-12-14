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
#ifndef AOS_TaskMgr_TaskDataNorm_h
#define AOS_TaskMgr_TaskDataNorm_h

#include "TaskMgr/TaskData.h"


class AosTaskDataNorm : public AosTaskData
{
	int				mPhysicalId;
	
public:
	AosTaskDataNorm(const bool flag);
	~AosTaskDataNorm();

	virtual int			getPhysicalId() const {return mPhysicalId;}
	
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

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif

#endif
