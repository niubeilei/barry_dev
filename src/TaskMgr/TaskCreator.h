////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TaskMgr_TaskCreator_h
#define Aos_TaskMgr_TaskCreator_h

#include "SEInterfaces/TaskType.h"
#include "SEInterfaces/TaskCreatorObj.h"


class AosTaskCreator : virtual public AosTaskCreatorObj
{
	OmnDefineRCObject;

public:
	AosTaskCreator();
	~AosTaskCreator();

	virtual bool registerTask(
					const AosTaskType::E type,
					const OmnString &name,
					const AosTaskObjPtr &task);

	virtual AosTaskObjPtr createTask(
					const AosXmlTagPtr &tag, 
					const AosJobInfo &jobinfo,
					const AosTaskDataObjPtr &task_data,
					const OmnString &logic_id,
					const AosRundataPtr &rdata);
	
	virtual AosTaskObjPtr serializeFrom(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata);

	virtual bool checkConfig(
					const AosXmlTagPtr &def,
					map<OmnString, OmnString> &jobenv,
					const AosRundataPtr &rdata);
	virtual bool init();
	
	static bool staticInit();

};
#endif

