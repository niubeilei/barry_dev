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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskObj.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskCreatorObj.h"


AosTaskCreatorObjPtr AosTaskObj::smTaskCreator;


bool
AosTaskObj::registerTask(
		const AosTaskType::E type,
		const OmnString &name,
		const AosTaskObjPtr &task)
{
	aos_assert_r(smTaskCreator, false);
	return smTaskCreator->registerTask(type, name, task);
}


AosTaskObjPtr
AosTaskObj::serializeFromStatic(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskCreator, 0);
	return smTaskCreator->serializeFrom(doc, rdata);
}


AosTaskObjPtr 
AosTaskObj::createTask(
		const AosXmlTagPtr &tag, 
		const AosJobInfo &jobinfo,
		const AosTaskDataObjPtr &task_data, 
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskCreator, 0);
	return smTaskCreator->createTask(tag, jobinfo, task_data, logic_id, rdata);
}

	
bool
AosTaskObj::checkConfigStatic(
		const AosXmlTagPtr &def,
		map<OmnString, OmnString> &jobenv,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskCreator, false);
	return smTaskCreator->checkConfig(def, jobenv, rdata);
}

	
void 
AosTaskObj::setCreator(const AosTaskCreatorObjPtr &d)
{
	smTaskCreator = d;
	smTaskCreator->init();		// Ketty 2013/12/27
}

