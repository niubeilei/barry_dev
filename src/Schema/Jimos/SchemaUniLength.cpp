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
// All records use the same length and the schema never changes.
//
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/Jimos/SchemaUniLength.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaUniLength_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaUniLength(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosSchemaUniLength::AosSchemaUniLength(const int version)
:
AosSchema(AOSSCHEMA_UNILENGTH, version)
{
/*
	if (!config(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
*/
}


AosSchemaUniLength::~AosSchemaUniLength()
{
}


bool
AosSchemaUniLength::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// The configure format:
	// 	<dataschema zky_name="xxx" jimo_objid="dataschema_unilength_jimodoc_v0" >
	// 		<datarecord zky_datarecord_name="xxx" ..../>
	// 	</dataschema>
	aos_assert_r(worker_doc, false);
	
	mSchemaName = worker_doc->getAttrStr("zky_name");

	AosXmlTagPtr record = getDataRecordConf(worker_doc, rdata.getPtr());
	aos_assert_r(record, false);

	mRecord = AosDataRecordObj::createDataRecordStatic(record, 0, rdata.getPtr() AosMemoryCheckerArgs);
	if (!mRecord)
	{
		AosSetEntityError(rdata, "schemaunilen_invalid_record_def", "Schema", mSchemaName)
			<< record->toString() << enderr;
		return false;
	}

	OmnString rcd_name = record->getAttrStr(AOSTAG_NAME, "");
	if (rcd_name != "")
	{
		mRecord->setRecordName(rcd_name);                 
	}

	mRecordLen = mRecord->getRecordLen();
	if (mRecordLen <= 0)
	{
		AosSetEntityError(rdata, "schemaunilen_invalid_record_len", "Schema", mSchemaName)
			<< record->toString() << enderr;
		return false;
	}
/*
	mFilter = 0;
	AosXmlTagPtr filter_tag = worker_doc->getFirstChild(AOSTAG_FILTER);
	if (filter_tag)
	{
	    mFilter = AosConditionObj::getConditionStatic(filter_tag, rdata);
		if (!mFilter)
		{
			AosSetEntityError(rdata, "schemafixlen_faild_create_filter", "schema", schema_name)
				<< tag << enderr;
			return false;
		}
	}                                                                 
*/
	return true;
}


bool
AosSchemaUniLength::nextRecordset(
		AosRundata *rdata, 
		AosRecordsetObjPtr &recordset, 
		const AosBuffDataPtr &buffdata,
		AosDatasetObj *dataset,
		bool &contents_incomplete)
{
	// This function retrieves a recordset from 'buffdata' at the position
	// 'offset'. The input 'buffdata' is a sequence of 'records' with the
	// same length and same schema. This function parses the number of
	// records requested by 'recordset' from 'buffdata'. Upon success, 
	// 'offset' moved to the first unconsumed byte, if any, or the end
	// of 'buffdata', 'recordset' holds the parsed records. 

	aos_assert_r(buffdata, false);
	aos_assert_r(mRecord, false);
	aos_assert_r(mRecordLen > 0, false);

	contents_incomplete = false;

	int64_t offset = buffdata->getCrtIdx();
	aos_assert_r(offset >= 0, false);

	int64_t data_len = buffdata->dataLen();
	aos_assert_r(offset <= data_len, false);

	AosRecordsetObj *recordset_raw;
	if (!recordset)
	{
		recordset = dataset->cloneRecordset(rdata);
		aos_assert_r(recordset, false);
		recordset_raw = recordset.getPtr();
		recordset_raw->holdRecord(rdata, mRecord);
//OmnScreen << " 111111111111111111 holdRecord 22222222222222222 " << endl;
	}
	else
	{
		recordset_raw = recordset.getPtr();
	}

	recordset->reset();
	recordset->setData(rdata, buffdata->getBuff());

	//int num_records_to_add = recordset->getNumRecordsToAdd();
	//aos_assert_r(num_records_to_add > 0, false);

	int64_t remaining_records = (data_len - offset) % mRecordLen;
	int64_t complete_records = (data_len - offset) / mRecordLen;
	//if (remaining_records && complete_records < num_records_to_add)
	if (remaining_records)
	{
		contents_incomplete = true;
	}

	if (complete_records == 0) 
	{
		// No more contents
		return true;
	}
	
	//if (num_records_to_add > complete_records) num_records_to_add = complete_records;

//	char *crt_data = buffdata->data();	
	//AosDataRecordObj *raw_schema = mRecord.getPtr();
	//AosDataRecordObjPtr new_record = recordset_raw->getRecord();
	//aos_assert_r(new_record, false);
//	AosDataRecordObj *raw_schema = new_record.getPtr();

	//for (int i=0; i<num_records_to_add; i++)
	for (int i=0; i<complete_records; i++)
	{
//		OmnString rcd_str(&crt_data[offset], mRecordLen);
//OmnScreen << " ##### append Record : " << rcd_str << endl;
		AosMetaDataPtr metadata  = buffdata->getMetadata();
		recordset_raw->appendRecord(rdata, 0, offset, mRecordLen, metadata);
		offset += mRecordLen;
//	int intRslt = strncmp(&crt_data[offset-4], "$$$$", 4);
//	aos_assert_r(!intRslt, false);
	}

	if (contents_incomplete && buffdata->isEOF())
	{
		dataset->addUnrecogContents(rdata, buffdata, buffdata->data(), offset, data_len-offset);
		contents_incomplete = false;
		buffdata->setCrtIdx(data_len);
		return true;
	}

	buffdata->setCrtIdx(offset);
	return true;
}


bool
AosSchemaUniLength::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
{
	record = 0;
	aos_assert_r(mRecord, false);
	OmnString key = mRecord->getRecordName();
	if (name == key)
	{
		record = mRecord;
	}
	return true;
}

	
bool
AosSchemaUniLength::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mRecord, false);
	records.push_back(mRecord);
	return true;
}


void 
AosSchemaUniLength::setTaskDocid(const u64 task_docid)
{
	mRecord->setTaskDocid(task_docid);
}


AosJimoPtr 
AosSchemaUniLength::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaUniLength(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

