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
#ifndef Aos_TaskMgr_TaskDataCreator_h
#define Aos_TaskMgr_TaskDataCreator_h

#include "SEInterfaces/TaskDataCreatorObj.h"
#include "vector"

class AosTaskDataCreator : virtual public AosTaskDataCreatorObj
{
	OmnDefineRCObject;

public:
	AosTaskDataCreator();
	~AosTaskDataCreator();

	virtual AosTaskDataObjPtr createTaskDataStatic(
			    const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	
	/*
	virtual AosTaskDataObjPtr createTaskDataAuto(
				const OmnString &file_name,
				const int physical_id);
	virtual AosTaskDataObjPtr createTaskDataDir(
				vector<OmnString> &dir_names,
				const int physical_id,
				const bool recursion,
				const bool ignore_head,
				const OmnString &character_type,
				const OmnString &row_delimiter);
	virtual AosTaskDataObjPtr createTaskDataDir(
				vector<AosFileInfo> &fileinfos,
				const int physical_id,
				const bool ignore_head,
				const OmnString &character_type,
				const OmnString &row_delimiter,
				const bool isUnicomFile);
	virtual AosTaskDataObjPtr createTaskDataDoc(
				const OmnString &scanner_id,
				const int physical_id);
	virtual AosTaskDataObjPtr createTaskDataDocids(
				const vector<u64> &docids,
				const int physical_id,
				const OmnString &row_delimiter);
	virtual AosTaskDataObjPtr createTaskDataFile(
				const OmnString &file_name,
				const int physical_id,
				const bool ignore_head,
				const OmnString &character_type,
				const OmnString &row_delimiter);
	virtual AosTaskDataObjPtr createTaskDataVirtualFile(
				const OmnString &file_name,
				const int physical_id);
	virtual AosTaskDataObjPtr createTaskDataOutPutDoc(
				const int physical_id,
				const int server_id,
				const OmnString &key,
				const u64 &sizeid,
				const int record_len,
				const u64 &file_id,
				const bool is_fixed);
	*/
	virtual AosTaskDataObjPtr createTaskDataOutPutDocid(
				const int physical_id,
				const OmnString &key,
				const u64 &start_docid,
				const int64_t &num);
	virtual AosTaskDataObjPtr createTaskDataOutPutIIL(
				const int physical_id,
				const OmnString &datacol_id,
				const u64 &file_id,
				const int level,
				const int block_idx,
				const bool last_merge);
	virtual AosTaskDataObjPtr createTaskDataOutPutSnapShot(
				const u32 virtual_id,
				const u32 snapshot_id);

	virtual AosTaskDataObjPtr createTaskDataIILSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const set<OmnString> &iilnames,
				const u64 &task_docid,
				const bool iscommit);

	virtual AosTaskDataObjPtr createTaskDataDocSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const AosDocType::E doctype,
				const u64 &task_docid,
				const bool iscommit);
	
	//virtual AosTaskDataObjPtr createTaskDataBuff(
	//			const AosTaskObjPtr task);

	virtual bool init();

};
#endif

