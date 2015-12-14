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
// 07/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TaskDataCreatorObj_h
#define Aos_SEInterfaces_TaskDataCreatorObj_h

#include "Rundata/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "Util/CodeConvertion.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
using namespace std;


class AosTaskDataCreatorObj : virtual public OmnRCObject
{
public:
	virtual AosTaskDataObjPtr createTaskDataStatic(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) = 0;
	
	virtual AosTaskDataObjPtr createTaskDataOutPutDocid(
				const int physical_id,
				const OmnString &key,
				const u64 &start_docid,
				const int64_t &num) = 0;
	virtual AosTaskDataObjPtr createTaskDataOutPutIIL(
				const int physical_id,
				const OmnString &datacol_id,
				const u64 &file_id,
				const int level,
				const int block_idx,
				const bool last_merge) = 0;
	virtual AosTaskDataObjPtr createTaskDataOutPutSnapShot(
				const u32 virtual_id,
				const u32 snapshot_id) = 0;

	virtual AosTaskDataObjPtr createTaskDataIILSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const set<OmnString> &iilnames,
				const u64 &task_docid,
				const bool iscommit) = 0;

	virtual AosTaskDataObjPtr createTaskDataDocSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const AosDocType::E doctype,
				const u64 &task_docid,
				const bool iscommit) = 0;
	
	virtual bool init() = 0;

};

#endif

