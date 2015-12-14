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
#ifndef AOS_TaskMgr_TaskDataVirtFile_h
#define AOS_TaskMgr_TaskDataVirtFile_h

#include "TaskMgr/TaskData.h"


class AosTaskDataVirtFile : public AosTaskData
{
	int						mPhysicalId;
	OmnString				mVirFileObjid;
	
public:
	AosTaskDataVirtFile(const bool flag);
	AosTaskDataVirtFile(
			const OmnString &virfile_objid,
			const int physical_id);
	~AosTaskDataVirtFile();

	virtual int			getPhysicalId() const {return mPhysicalId;}

	virtual AosTaskDataObjPtr create() const;
	virtual AosTaskDataObjPtr create(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) const;
							
	virtual bool isDataExist(const AosRundataPtr &rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
};
#endif
#endif
