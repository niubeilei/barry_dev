////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2015/01/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Dataset/Jimos/DatasetAsOutput.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"


AosDatasetAsOutput::AosDatasetAsOutput(
		const OmnString &type,
		const int version)
:
AosDataset(type, version)
{
}


AosDatasetAsOutput::~AosDatasetAsOutput()
{
}


bool 
AosDatasetAsOutput::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	//	<conf ...>
	//		<dataset .../>
	//		<data_collector .../>
	//		<data_assembler .../>
	//	</conf>
	AosXmlTagPtr tags = worker_doc->getFirstChild("dataset");
	if (!tags)
	{
		AosLogUserError(rdata, "missing_dataset_conf") << enderr;
		return false;
	}
	mDataset = AosDatasetObj::createDatasetStatic(rdata, tags);
	if (!mDataset)
	{
		AosLogUserError(rdata, "failed_creating_dataset") << enderr;
		return false;
	}
	mDatasetRaw = mDataset.getPtr();

	tags = worker_doc->getFirstChild("data_collector");
	if (tags)
	{
		mDataCollector = AosDataCollectorObj::createDataCollectorStatic(rdata, tags);
	}

	tags = worker_doc->getFirstChild("data_assembler");
	if (tags)
	{
		mDataAssembler = AosDataAssembler::createDataAssemblerStatic(rdata, mTask, tags);
	}

	return true;
}


bool 
AosDatasetAsOutput::sendStart(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->sendStart(rdata);
}


bool 
AosDatasetAsOutput::sendFinish(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->sendFinish(rdata);
}


bool 
AosDatasetAsOutput::reset(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->reset(rdata);
}


int64_t 
AosDatasetAsOutput::getNumEntries()
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->getNumEntries();
}


int64_t 
AosDatasetAsOutput::getTotalSize() const
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->getTotalSize();
}


void 
AosDatasetAsOutput::setTask(const AosTaskObjPtr &task)
{
	mTask = task;
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->setTask(task);
}


AosRecordsetObjPtr 
AosDatasetAsOutput::getRecordset()
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->getRecordset();
}


AosRecordsetObjPtr 
AosDatasetAsOutput::cloneRecordset()
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->cloneRecordset();
}


bool 
AosDatasetAsOutput::getRecord(
		const OmnString &name, 
		AosDataRecordObjPtr &record)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->getRecord(name, record();
}


bool 
AosDatasetAsOutput::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->getRecords(records);
}


bool 
AosDatasetAsOutput::nextRecordset(
		const AosRundataPtr &rdata, 
		AosRecordsetObjPtr &recordset)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->nextRecordset(rdata, recordset);
}


bool 
AosDatasetAsOutput::addDataCube(
		const AosRundataPtr &rdata, 
	 	const AosDataCubeObjPtr &data_cube)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->addDataCube(rdata, data_cube);
}


bool 
AosDatasetAsOutput::addUnrecogContents(
		const AosRundataPtr &rdata,
		const AosBuffDataPtr &buff_data,
		const char *data,
		const int64_t &start_pos,
		const int64_t &length)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->addUnrecogContents(rdata, buff_data, data, start_pos, length);
}


bool 
AosDatasetAsOutput::addInvalidContents(
		const AosRundataPtr &rdata,
		AosDataRecordObj * record)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->addInvalidContents(rdata, record);
}


bool 
AosDatasetAsOutput::addFilterContents(
		const AosRundataPtr &rdata,
		AosDataRecordObj * record)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->addFilterContents(rdata, record);
}


bool 
AosDatasetAsOutput::addContents(
		AosRundata *rdata, 
		AosBuff *buff)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->addContents(rdata, buff);
}



bool 
AosDatasetAsOutput::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->getRsltInfor(infor, rdata);
}


bool 
AosDatasetAsOutput::setValueBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mDatasetRaw, rdata, false);
	return mDatasetRaw->setValueBuff(buff, rdata);
}
#endif
