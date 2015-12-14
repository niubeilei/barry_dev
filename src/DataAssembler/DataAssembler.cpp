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
// 05/28/2012 Moved from Util/DataAssembler.cpp
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/DataAssembler.h"

#include "API/AosApi.h"
#include "DataAssembler/FileAssembler.h"
#include "DataAssembler/DataAssemblerSort.h"
#include "DataAssembler/DataAssemblerGroup.h"
#include "DataAssembler/DataAssemblerBuff.h"
#include "Job/Job.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/WriteCacherObj.h"
#include "Util/Ptrs.h"


int64_t AosDataAssembler::sgDftBuffLen = AosDataAssembler::eDftBuffLen;
int64_t AosDataAssembler::sgDftQueueSize = AosDataAssembler::eDftQueueSize;


AosDataAssembler::AosDataAssembler(
		const AosDataAssemblerType::E type, 
		const OmnString &name,
		const OmnString &asm_key,
		const u64 task_docid)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtrNoLock()),
mType(type),
mTypeName(name),
mAsmKey(asm_key),
mTaskDocid(task_docid)
{
	mName = asm_key;
}


AosDataAssembler::~AosDataAssembler()
{
}


bool
AosDataAssembler::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	// Ketty 2014/01/09
	AosXmlTagPtr rcd_xml = def->getFirstChild("datarecord");
	if(!rcd_xml)
	{
		AosSetErrorU(rdata, "missing output record cfg:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, mTaskDocid, rdata AosMemoryCheckerArgs);
	aos_assert_r(mOutputRecord, false);
	OmnString rcd_name = rcd_xml->getAttrStr(AOSTAG_NAME, "");

	if (rcd_name != "")
	{
		mOutputRecord->setRecordName(rcd_name);
	}

	return true;
}


u64
AosDataAssembler::getNextDocid(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return 0;
}
	

AosDataRecordObjPtr
AosDataAssembler::cloneDataRecord(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return 0;
}


/*
AosDataAssemblerObjPtr
AosDataAssembler::createIILAssembler(
		AosRundata *rdata,
		const OmnString &asm_key,
		const AosTaskObjPtr &task,
		const AosXmlTagPtr &def)
{
	return AosIILAssembler::createIILAssembler(asm_key, task, def, rdata);	
}


AosDataAssemblerObjPtr
AosDataAssembler::createDocAssembler(
		AosRundata *rdata,
		const OmnString &asm_key,
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &def)
{
	return AosDocAssembler::createDocAssembler(asm_key, task, def, rdata); 
}

	
AosDataAssemblerObjPtr
AosDataAssembler::createAssembler(
		const OmnString &asm_key,
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &data_col_def, 
		AosRundata *rdata)
{
	OmnString asm_type = data_col_def->getAttrStr("zky_type", "");
	aos_assert_r(asm_type != "", 0);
	if(asm_type == "doc")
	{
		AosXmlTagPtr doc_conf = data_col_def->getFirstChild();
		return AosDocAssembler::createDocAssembler(asm_key, task, doc_conf, rdata);
	}
	else if(asm_type == "iil")
	{
		return AosIILAssembler::createIILAssemblerV1(asm_key, task, data_col_def, rdata);
	}
	else
	{
		OmnNotImplementedYet;
	}
	return 0;
}
*/

	
AosDataAssemblerObjPtr
AosDataAssembler::createAssembler(
		const OmnString &asm_key,
		const u64 task_docid, 
		const AosXmlTagPtr &data_col_def, 
		AosRundata *rdata)
{
	OmnString asm_type = data_col_def->getAttrStr("type", "");
	if(asm_type == "doc")
	{
		OmnNotImplementedYet;
	}
	else if(asm_type == "file")
	{
		return AosFileAssembler::createFileAssemblerV1(asm_key, task_docid, data_col_def, rdata);
	}
	else if (asm_type == "sort")
	{
		return AosDataAssemblerSort::createAssemblerSort(asm_key, task_docid, data_col_def, rdata);
	}
	else if (asm_type == "group")
	{
		return AosDataAssemblerGroup::createAssembler(asm_key, task_docid, data_col_def, rdata);
	}
	else if (asm_type == "buff")
	{
		return AosDataAssemblerBuff::createAssemblerBuff(asm_key, task_docid, data_col_def, rdata);
	}
	else
	{
		OmnNotImplementedYet;
	}
	return 0;
}


/*
bool
AosDataAssembler::checkIILAsmConfigStatic(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	return AosIILAssembler::checkConfigStatic(def, rdata);
}
*/

bool
AosDataAssembler::checkConfigStatic(
		const AosXmlTagPtr &def,
		AosRundata* rdata)
{
	OmnString type = def->getAttrStr(AOSTAG_TYPE);
	switch(AosDataAssemblerType::toEnum(type))
	{
	case AosDataAssemblerType::eSort:
	 	 return AosDataAssemblerSort::checkConfigStatic(def, rdata);
	default:
		 OmnAlarm << "Unrecognized type: " << type << enderr;
		 return false;
	}
}

	
bool
AosDataAssembler::init(const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	AosXmlTagPtr tag = def->getFirstChild("data_assembler");
	if(!tag) return true;

	sgDftBuffLen = tag->getAttrInt64(AOSTAG_MAX_BUFFSIZE, -1);
	if (sgDftBuffLen <= eDftBuffMinLen) sgDftBuffLen = eDftBuffMinLen;
	if (sgDftBuffLen >= eDftBuffMaxLen) sgDftBuffLen = eDftBuffMaxLen; 
	return true;
}


bool 
AosDataAssembler::addEntry( 		
		AosRundata *rdata, 
		AosDataRecordObj *record, 
		const u64 docid)
{
	OmnShouldNeverComeHere;
	return false;
}
	
AosDataRecordObjPtr
AosDataAssembler::getOutputTemplate()
{
	return mOutputRecord;	
}

bool
AosDataAssembler::appendEntry(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataAssembler::appendEntry(
		const AosValueRslt &value,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}

//for log  by barry
//JIMODB-401
void
AosDataAssembler::showDataAssemblerInfo(
		const char *file,
		const int line,
		const OmnString &action)
{
	OmnString msg;
	msg << " DASMB:" << mAsmKey;
	report(file, line, action, msg);
}
