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
// 2012/10/16 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/TaskDataCreator.h"

#include "TaskMgr/TaskDataAuto.h"
#include "TaskMgr/TaskDataDir.h"
#include "TaskMgr/TaskDataDoc.h"
#include "TaskMgr/TaskDataDocIds.h"
#include "TaskMgr/TaskDataFile.h"
#include "TaskMgr/TaskDataVirtFile.h"

#include "TaskMgr/TaskDataOutPutIIL.h"
#include "TaskMgr/TaskDataOutPutDoc.h"
#include "TaskMgr/TaskDataOutPutDocid.h"
#include "TaskMgr/TaskDataOutPutSnapShot.h"
#include "TaskMgr/TaskDataIILSnapShot.h"
#include "TaskMgr/TaskDataDocSnapShot.h"

AosTaskDataCreator::AosTaskDataCreator()
{
}


AosTaskDataCreator::~AosTaskDataCreator()
{
}


AosTaskDataObjPtr
AosTaskDataCreator::createTaskDataStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	return AosTaskData::createTaskData(def, rdata);
}


AosTaskDataObjPtr
AosTaskDataCreator::createTaskDataOutPutDocid(
		const int physical_id,
		const OmnString &key,
		const u64 &start_docid,
		const int64_t &num)
{
	return OmnNew AosTaskDataOutPutDocid(physical_id, key, start_docid, num);
}

	
AosTaskDataObjPtr
AosTaskDataCreator::createTaskDataOutPutIIL(
		const int physical_id,
		const OmnString &datacol_id,
		const u64 &file_id,
		const int level,
		const int block_idx,
		const bool last_merge)
{
	return OmnNew AosTaskDataOutPutIIL(
		physical_id, datacol_id, file_id, level, block_idx, last_merge);
}
	

AosTaskDataObjPtr
AosTaskDataCreator::createTaskDataOutPutSnapShot(
		const u32 virtual_id,
		const u32 snapshot_id)
{
	return OmnNew AosTaskDataOutPutSnapShot(
		virtual_id, snapshot_id);	
}


AosTaskDataObjPtr 
AosTaskDataCreator::createTaskDataIILSnapShot(
		const u32 vir_id,
		const u64 &snapshot_id,
		const set<OmnString> &iilnames,
		const u64 &task_docid,
		const bool iscommit)
{
	return OmnNew AosTaskDataIILSnapShot(
			vir_id, snapshot_id, iilnames, task_docid, iscommit);
}

AosTaskDataObjPtr 
AosTaskDataCreator::createTaskDataDocSnapShot(
		const u32 vir_id,
		const u64 &snapshot_id,
		const AosDocType::E doctype,
		const u64 &task_docid,
		const bool iscommit)
{
	return OmnNew AosTaskDataDocSnapShot(
			vir_id, snapshot_id, doctype, task_docid, iscommit);
}

bool
AosTaskDataCreator::init()
{
	// Ketty 2014/01/02
	return AosTaskData::staticInit();
}

