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
#ifndef Aos_SEInterfaces_TaskDataObj_h
#define Aos_SEInterfaces_TaskDataObj_h


#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataCreatorObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "Util/CodeConvertion.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/Ptrs.h"


class AosTaskDataObj : virtual public OmnRCObject
{
protected:
	static AosTaskDataCreatorObjPtr	smCreator;

public:
	virtual AosTaskDataType::E	getTaskDataType() const = 0;
	virtual int					getPhysicalId() const = 0;
	virtual OmnString			getFileName() const = 0;

	virtual u64					getSnapShotId() const = 0;
	virtual u32					getVirtualId() const = 0;
	virtual set<OmnString>		getIILNames() const = 0;
	virtual AosDocType::E		getDocType() const = 0;
	virtual u64					getTaskDocid() const = 0;
	virtual bool				getIsCommit() const = 0;

	virtual bool	isDataExist(const AosRundataPtr &rdata) = 0;
	virtual AosTaskDataObjPtr create() const = 0;
	virtual AosTaskDataObjPtr create(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) const = 0;
	virtual bool serializeTo(
				AosXmlTagPtr &xml,
				const AosRundataPtr &rdata) = 0;
	virtual bool serializeFrom(
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata) = 0;
	virtual AosDataCacherObjPtr createDataCacher(
				const AosDataRecordObjPtr &record,
				const AosRundataPtr &rdata) = 0;
	
	virtual AosDataScannerObjPtr createDataScanner(const AosRundataPtr &rdata) = 0;

	virtual bool isTheSameTaskData(
				const AosTaskDataObjPtr &task_data,
				const AosRundataPtr &rdata) = 0;

	virtual OmnString getStrKey(const AosRundataPtr &rdata) = 0;

	virtual bool serializeTo(
				AosBuffPtr &buff,
				const AosRundataPtr &rdata) = 0;

	virtual bool serializeFrom(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata) = 0;


	static AosTaskDataObjPtr createTaskDataStatic(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	static AosTaskDataObjPtr createTaskDataOutPutIIL(
				const int physical_id,
				const OmnString &datacol_id,
				const u64 &file_id,
				const int level,
				const int block_idx = 0,
				const bool last_merge = false);

	static AosTaskDataObjPtr createTaskDataOutPutDocid(
				const int physical_id,
				const OmnString &key,
				const u64 &start_docid,
				const int64_t &num);
	static AosTaskDataObjPtr createTaskDataOutPutSnapShot(
				const u32 virtual_id,
				const u32 snapshot_id);

	static AosTaskDataObjPtr createTaskDataIILSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const set<OmnString> &iilnames,
				const u64 &task_docid,
				const bool iscommit);

	static AosTaskDataObjPtr createTaskDataDocSnapShot(
				const u32 vir_id,
				const u64 &snapshot_id,
				const AosDocType::E doctype,
				const u64 &task_docid,
				const bool iscommit);

private:
	virtual bool config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) = 0;

public:
	static void setCreator(const AosTaskDataCreatorObjPtr &creator);
	static AosTaskDataCreatorObjPtr getCreator() {return smCreator;}
};

#endif

