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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/TaskAction.h"
#include "API/AosApi.h"
#include "JobTrans/NotifyTaskMgrCreateDataCol.h"
#include "DataAssembler/DataAssembler.h"
#include "TaskMgr/Task.h"

AosTaskAction::AosTaskAction(
		const OmnString &name, 
		const AosActionType::E type, 
		const bool reg)
:
AosActionObj(0)
{
	mType = type;
	AosActionObjPtr thisptr(this, false);
	if (reg) 
	{
		registerAction(thisptr, name);
	}
}


AosTaskAction::AosTaskAction(const AosActionType::E type)
:
AosActionObj(0)
{
	mType = type;
}


AosTaskAction::~AosTaskAction()
{
}


bool
AosTaskAction::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosTaskAction::run(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosTaskAction::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosTaskAction::run(const char *value, const int len, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosTaskAction::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	return true;
}

bool
AosTaskAction::checkConfig(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		set<OmnString> &data_col_ids)
{
	return true;
}


AosJimoPtr
AosTaskAction::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return 0;
}

bool
AosTaskAction::remoteCreateDataCol(
		const AosXmlTagPtr &output_xml,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr datacol_xml = output_xml->getFirstChild("datacollector");
	aos_assert_r(datacol_xml, false);

	OmnString xml_str = datacol_xml->toString();
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	OmnString datacol_id = task->getTaskId();
	u64 job_docid = task->getJobDocid();
	u64 task_docid = task->getTaskDocid();
	u32 svr_id = AosGetSelfServerId();

	//create datacollector
	AosTransPtr trans = OmnNew AosNotifyTaskMgrCreateDataCol(datacol_id, job_docid, task_docid, xml_str, svr_id);
	aos_assert_r(trans, false);

	AosBuffPtr resp;
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);
	aos_assert_r(!timeout, false);
	aos_assert_r(resp && resp->dataLen() >= 0, false);      
	aos_assert_r(resp->getBool(false), false);
	return true;
}
