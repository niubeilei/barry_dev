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
#ifndef Aos_SEInterfaces_TaskCreatorObj_h
#define Aos_SEInterfaces_TaskCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskType.h"
#include "SEInterfaces/JobObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosTaskCreatorObj : virtual public OmnRCObject
{
public:
	virtual bool registerTask(
					const AosTaskType::E type,
					const OmnString &name,
					const AosTaskObjPtr &task) = 0;

	virtual AosTaskObjPtr createTask(
					const AosXmlTagPtr &tag,
					const AosJobInfo &jobinfo,
					const AosTaskDataObjPtr &task_data, 
					const OmnString &logic_id,
					const AosRundataPtr &rdata) = 0;

	virtual AosTaskObjPtr serializeFrom(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata) = 0;

	virtual bool checkConfig(
					const AosXmlTagPtr &def,
					map<OmnString, OmnString> &jobenv,
					const AosRundataPtr &rdata) = 0;
	
	virtual bool init() = 0;
};
#endif

