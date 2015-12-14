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
#include "TaskMgr/TaskCreator.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionObj.h"
#include "TaskMgr/MapTask.h"
#include "TaskMgr/ReduceTask.h"
#include "TaskMgr/NormTask.h"
#include "Debug/Debug.h"


static AosTaskObjPtr	sgTasks[AosTaskType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;


AosTaskCreator::AosTaskCreator()
{
}


AosTaskCreator::~AosTaskCreator()
{
}

bool
AosTaskCreator::init()
{
	return staticInit();
}

bool
AosTaskCreator::staticInit()
{
	if(sgInited)	return true;

	static AosMapTask		sgMapTask(true);
	static AosReduceTask	sgReduceTask(true);
	static AosNormTask		sgNormTask(true);

	sgInited = true;
	return true;
}


bool
AosTaskCreator::registerTask(
		const AosTaskType::E type,
		const OmnString &name,
		const AosTaskObjPtr &task)
{
	if (!AosTaskType::isValid(type))
	{
		OmnAlarm << "Invalid job id: " << type << enderr;
		return false;
	}

	OmnTagFuncInfo << "create task with name: " << name << endl;
	sgLock.lock();
	if (sgTasks[type])
	{
		sgLock.unlock();
		OmnAlarm << "Task already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgTasks[type] = task;
	OmnString errmsg;
	bool rslt = AosTaskType::addName(name, type, errmsg);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add job name: " << errmsg << enderr;
	}
	return rslt;
}


AosTaskObjPtr 
AosTaskCreator::createTask(
		const AosXmlTagPtr &task_sdoc, 
		const AosJobInfo &jobinfo,
		const AosTaskDataObjPtr &task_data,
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	if (!sgInited)	staticInit();

	OmnTagFuncInfo << "create task with logic id: " << logic_id << endl;
	aos_assert_r(task_sdoc, 0);
	aos_assert_r(logic_id != "", 0);

	OmnString type = task_sdoc->getAttrStr(AOSTAG_TYPE);
	if (type == "")
	{
		AosSetErrorU(rdata, "missing_task_type:") << task_sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskType::E taskid = AosTaskType::toEnum(type);
	if (!AosTaskType::isValid(taskid))
	{
		AosSetErrorU(rdata, "invalid_task_id:") << task_sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskObjPtr task = sgTasks[taskid];
	if (!task)
	{
		AosSetErrorU(rdata, "task_not_defined:") << task_sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	
	AosTaskObjPtr tt = task->create(task_sdoc, jobinfo, task_data, logic_id, rdata);
	if (!tt)
	{
		AosSetErrorU(rdata, "failed_creating_task:") << task_sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return tt;
}


AosTaskObjPtr
AosTaskCreator::serializeFrom(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	if (!sgInited)	staticInit();
	aos_assert_r(doc, 0);

	AosTaskType::E type_id = AosTaskType::toEnum(doc->getAttrStr(AOSTAG_TASK_TYPE));	
	if (!AosTaskType::isValid(type_id))
	{
		OmnAlarm << "Invalid task id: " << type_id << enderr;
		return 0;
	}
	
	AosTaskObjPtr task = sgTasks[type_id];
	if (!task)
	{
		AosSetErrorU(rdata, "task_not_defined") << ": " << type_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskObjPtr tt = task->create();
	aos_assert_r(tt, 0);

	bool rslt = tt->serializeFrom(doc, rdata);
	if (!rslt)
	{
		OmnAlarm << "serialize from doc failed, may be config error" << enderr;
	}
	return tt;
}
	
bool
AosTaskCreator::checkConfig(
		const AosXmlTagPtr &def,
		map<OmnString, OmnString> &jobenv,
		const AosRundataPtr &rdata)
{
	if (!sgInited)	staticInit();

	aos_assert_r(def, 0);

	OmnString type = def->getAttrStr(AOSTAG_TYPE);
	if (type == "")
	{
		AosSetErrorU(rdata, "missing_task_type:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskType::E taskid = AosTaskType::toEnum(type);
	if (!AosTaskType::isValid(taskid))
	{
		AosSetErrorU(rdata, "invalid_task_id:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskObjPtr task = sgTasks[taskid];
	if (!task)
	{
		AosSetErrorU(rdata, "task_not_defined:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosTaskObjPtr tt = task->create();
	aos_assert_r(tt, 0);

	tt->setTaskENV(jobenv);
	bool rslt = tt->checkConfig(def, rdata);
	aos_assert_r(rslt, false);
	
	return true;
}


