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
// Record schema desinged by extract records to the streaming data. 
// it used the record object to analyze the stream, 
// then the record object can tell the length that the parsed record has.
// example:
// <dataschema zky_name="example1_task002_schema_rhs" jimo_objid="dataschema_record_jimodoc_v0">
// 		<datarecord type="iil" zky_name="example1_task002_rcd_rhs_iil" zky_length="128">
// 			<datafields>
// 				<datafield type="str" zky_name="value" zky_offset="0" zky_length="120" zky_datatooshortplc="cstr"/>
// 				<datafield type="bin_u64" zky_name="docid" zky_offset="120" zky_length="8"/>
// 			</datafields>
//		</datarecord>
// </dataschema>
//
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/Jimos/SchemaRecord.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaRecord_0( const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaRecord(version);
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


AosSchemaRecord::AosSchemaRecord(const int version)
:
AosSchema(AOSSCHEMA_RECORD, version),
mRecord(0)
{
}

AosSchemaRecord::AosSchemaRecord(
		const AosSchemaRecord &rhs, 
		AosRundata *rdata AosMemoryCheckDecl)
:
AosSchema(AOSSCHEMA_RECORD,0)
{
	mRecordPtr = rhs.mRecordPtr->clone(rdata AosMemoryCheckerFileLine);
	mRecord = mRecordPtr.getPtr();
}

AosSchemaRecord::~AosSchemaRecord()
{
}


bool
AosSchemaRecord::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// The configure format:
	// 	<dataschema zky_name="xxx" jimo_objid="dataschema_record_jimodoc_v0" >
	// 		<datarecord zky_datarecord_name="xxx" ..../>
	// 	</dataschema>
	aos_assert_r(worker_doc, false);
	AosXmlTagPtr rcd_conf = worker_doc->getFirstChild("datarecord");
	//Jozhi this case is the multi record
	if (!rcd_conf)
	{
		u64 schema_docid = worker_doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(schema_docid != 0, false);
		AosXmlTagPtr schema_doc = AosGetDocByDocid(schema_docid, rdata);
		aos_assert_r(schema_doc, false);
		rcd_conf = schema_doc->getFirstChild("datarecord");
	}
	aos_assert_r(rcd_conf, false);
	//if (!rcd_conf)
	//{
	//	AosSetErrorU(rdata, "missing_record : ") << worker_doc->toString();
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}

	mRecordPtr = AosDataRecordObj::createDataRecordStatic(rcd_conf, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mRecordPtr, false);
	mRecord = mRecordPtr.getPtr();
	OmnString rcd_name = rcd_conf->getAttrStr(AOSTAG_NAME, "");
	if (rcd_name != "")
	{
		mRecord->setRecordName(rcd_name);                 
	}
	
	return true;
}


bool
AosSchemaRecord::nextRecordset(
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
	aos_assert_rr(offset >= 0, rdata, false);

	int64_t data_len = buffdata->dataLen();
	aos_assert_rr(offset <= data_len, rdata, false);
	
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

	recordset->setMetaData(rdata, buffdata->getMetadata());

	//AosDataRecordObjPtr new_record = recordset_raw->getRecord();
	//aos_assert_r(new_record, false);
//	AosDataRecordObj *raw_schema = new_record.getPtr();
	
	//int num_rcds_to_add = recordset_raw->getNumRecordsToAdd();
	//aos_assert_r(num_rcds_to_add > 0, false);

	char * data = buffdata->data();
	
	//while (num_rcds_to_add && offset < data_len)
	while (offset < data_len)
	{
		//mRecord->setData(&data[offset], -1);

		int record_len, status;
		mRecord->determineRecordLen(&data[offset], data_len - offset, record_len, status);

		if (record_len == -1)
		{
			contents_incomplete = true;
			aos_assert_r(offset <= data_len, false);
			break;
		}

		if (status == -2)
		{
			//invalid record
			dataset->addUnrecogContents(rdata, buffdata, buffdata->data(), offset, record_len);
OmnString rcd_str(&data[offset], record_len);    
OmnScreen << "XXXXXXXXXX invalid Record : " << rcd_str << endl;
			offset += record_len;
			aos_assert_r(offset <= data_len, false);
			continue;
		}
//OmnString rcd_str(&data[offset], record_len);    
//OmnScreen << "XXXXXXXXXX correct Record : " << rcd_str << endl;

		AosMetaDataPtr metadata = buffdata->getMetadata();
		recordset_raw->appendRecord(rdata, 0, offset, record_len, metadata);
		offset += record_len;
		//num_rcds_to_add--;
		aos_assert_r(offset <= data_len, false);
	}

	if (contents_incomplete && buffdata->isEOF())
	{
		contents_incomplete = false;
		dataset->addUnrecogContents(rdata, buffdata, buffdata->data(), offset, data_len-offset);
OmnScreen << "XXXXXXXXXXXXXXXXXXXXXXXXx" << endl;
OmnString rcd_str(&data[offset], data_len-offset);    
OmnScreen << "XXXXXXXXXX incomplete Record : " << rcd_str << endl;
		buffdata->setCrtIdx(data_len);
		return true;
	}
	
	buffdata->setCrtIdx(offset);
	return true;
}


bool
AosSchemaRecord::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
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
AosSchemaRecord::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mRecord, false);
	records.push_back(mRecord);
	return true;
}


void 
AosSchemaRecord::setTaskDocid(const u64 task_docid)
{
	mRecord->setTaskDocid(task_docid);
}


AosJimoPtr 
AosSchemaRecord::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaRecord(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

// jimodb-1301
int 
AosSchemaRecord::nextRecord(
		AosRundata *rdata,                                      
		char * data,
		int data_len,
		AosMetaData* meta,
		AosDataRecordObj *&rcd,
		int &parsed_len)
{
	// Use 'mRecord' to parse the next record. It may return one of 
	// the three states:
	// 1. Good record
	// 2. Invalid but complete record
	// 3. Incomplete record
	int status = -2;

	aos_assert_r(mRecord,status);
	mRecord->clear();

	// 'setData(...)' will parse the record in order to determine
	// the record length.
	bool rslt = mRecord->setData(data,data_len, meta, status);
	int rcd_len =  mRecord->getRecordLen(); 
	
	if (meta && status == 0)
	{
		meta->moveToNext();
		meta->setRecordLength((int64_t&)rcd_len);
	}
	rcd = mRecord;
	parsed_len = rcd_len;
	
	aos_assert_r(rslt, -2);
	return status;
}

AosSchemaObjPtr
AosSchemaRecord::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosSchemaRecord(*this, rdata AosMemoryCheckerFileLine);
	}
	catch(...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}
