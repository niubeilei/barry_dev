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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskDataObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosTaskDataCreatorObjPtr AosTaskDataObj::smCreator;

	
AosTaskDataObjPtr
AosTaskDataObj::createTaskDataStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createTaskDataStatic(def, rdata);
}

AosTaskDataObjPtr
AosTaskDataObj::createTaskDataOutPutDocid(
		const int physical_id,
		const OmnString &key,
		const u64 &start_docid,
		const int64_t &num)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createTaskDataOutPutDocid(physical_id, key, start_docid, num);
}

	
AosTaskDataObjPtr
AosTaskDataObj::createTaskDataOutPutIIL(
		const int physical_id,
		const OmnString &datacol_id,
		const u64 &file_id,
		const int level,
		const int block_idx,
		const bool last_merge)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createTaskDataOutPutIIL(physical_id, datacol_id, file_id, level, block_idx, last_merge);
}
	

AosTaskDataObjPtr
AosTaskDataObj::createTaskDataOutPutSnapShot(
		const u32 virtual_id,
		const u32 snapshot_id)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createTaskDataOutPutSnapShot(virtual_id, snapshot_id);
}

AosTaskDataObjPtr 
AosTaskDataObj::createTaskDataIILSnapShot(
		const u32 vir_id,
		const u64 &snapshot_id,
		const set<OmnString> &iilnames,
		const u64 &task_docid,
		const bool iscommit)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createTaskDataIILSnapShot(vir_id, snapshot_id, iilnames, task_docid, iscommit);
}

AosTaskDataObjPtr 
AosTaskDataObj::createTaskDataDocSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const AosDocType::E doctype,
				const u64 &task_docid,
				const bool iscommit)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createTaskDataDocSnapShot(vir_id, snapshot_id, doctype, task_docid, iscommit);
}

	
void
AosTaskDataObj::setCreator(const AosTaskDataCreatorObjPtr &creator)
{
	smCreator = creator;
	smCreator->init();
}
