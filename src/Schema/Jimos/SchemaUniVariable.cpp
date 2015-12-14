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
#include "Schema/Jimos/SchemaUniVariable.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaUniVariable_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaUniVariable(version);
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


AosSchemaUniVariable::AosSchemaUniVariable(const int version)
:
AosSchema(AOSSCHEMA_UNIVARIABLE, version)
{
/*
	if (!config(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
*/
}


AosSchemaUniVariable::~AosSchemaUniVariable()
{
}


bool
AosSchemaUniVariable::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// The configure format:
	// 	<dataschema zky_name="xxx" jimo_objid="dataschema_univariable_jimodoc_v0">
	// 		<datarecord zky_datarecord_name="xxx" ..../>
	// 	</dataschema>
	aos_assert_r(worker_doc, false);
	
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
	
	mRowDelimiter = AosParseRowDelimiter(record->getAttrStr(AOSTAG_ROW_DELIMITER));
	aos_assert_r(mRowDelimiter!= "", false);

//	mRecordLen = mRecord->getDataLen();
//	aos_assert_r(mRecordLen > 0, false);
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
AosSchemaUniVariable::nextRecordset(
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

	contents_incomplete = false;

	int64_t offset = buffdata->getCrtIdx();
	aos_assert_r(offset >= 0, false);

	int64_t data_len = buffdata->dataLen();
	aos_assert_r(offset <= data_len, false);

//	int64_t remaining_records = (data_len - offset) / mRecordLen;
//	if (remaining_records == 0) 
//	{
//		// No more contents
//		return true;
//	}
	
	AosRecordsetObj *recordset_raw;
	if (!recordset)
	{
		recordset = dataset->cloneRecordset(rdata);
		aos_assert_r(recordset, false);
		recordset_raw = recordset.getPtr();
		recordset_raw->holdRecord(rdata, mRecord);
	}
	else
	{
		recordset_raw = recordset.getPtr();
	}

	recordset->reset();
	recordset->setData(rdata, buffdata->getBuff());

	//int num_records_to_add = recordset->getNumRecordsToAdd();
	//aos_assert_r(num_records_to_add > 0, false);

//	if (num_records_to_add > remaining_records) num_records_to_add = remaining_records;

	//AosDataRecordObjPtr new_record = recordset_raw->getRecord();
	//aos_assert_r(new_record, false);
//	AosDataRecordObj *raw_schema = new_record.getPtr();

	char * data = buffdata->data();

	//for (int i=0; i<num_records_to_add; i++)
	while (offset < data_len)
	{
		//if (offset >= data_len) break;
		const char * str1 = strstr(&data[offset], mRowDelimiter.data());
		int str_len = 0;
		if (str1 == 0) 
		{
			str_len = data_len - offset;
			if (!buffdata->isEOF())
			{
if(str_len > 1000000)
{
	OmnAlarm << "error" << enderr;
}
				contents_incomplete = true;
				break;
			}
		}
		else
		{
			str_len = str1 - &data[offset];
			if (str_len + offset + mRowDelimiter.length()> data_len)
			{
				str_len = data_len - offset;
				aos_assert_r(str_len + offset <= data_len, false);
				if (!buffdata->isEOF())
				{
if(str_len > 1000000)
{
	OmnAlarm << "error" << enderr;
}
					contents_incomplete = true;
					break;
				}
			}
			else
			{
				str_len += mRowDelimiter.length();
				aos_assert_r(str_len + offset <= data_len, false);
			}
		}
	
		aos_assert_r(str_len > 0, false);
		if (!contents_incomplete)
		{
			//recordset_raw->appendRecord(rdata, 0, offset, str_len, 0);
			//Jackie 2015/03/19
			AosMetaDataPtr metadata  = buffdata->getMetadata();
			recordset_raw->appendRecord(rdata, 0, offset, str_len, metadata);
			offset += str_len;
			aos_assert_r(offset <= data_len, false);
		}
	}

	aos_assert_r(offset <= data_len, false);
	buffdata->setCrtIdx(offset);
	return true;
}


bool
AosSchemaUniVariable::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
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
AosSchemaUniVariable::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mRecord, false);
	records.push_back(mRecord);
	return true;
}


void 
AosSchemaUniVariable::setTaskDocid(const u64 task_docid)
{
	mRecord->setTaskDocid(task_docid);
}


AosJimoPtr 
AosSchemaUniVariable::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaUniVariable(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

