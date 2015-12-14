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
#include "Schema/Jimos/SchemaRecordByLength.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaRecordByLength_0( const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaRecordByLength(version);
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


AosSchemaRecordByLength::AosSchemaRecordByLength(const int version)
:
AosSchema(AOSSCHEMA_RECORDBYLENGTH, version)
{
}

AosSchemaRecordByLength::AosSchemaRecordByLength(
		const AosSchemaRecordByLength &rhs, 
		AosRundata *rdata AosMemoryCheckDecl)
:
AosSchema(AOSSCHEMA_RECORDBYLENGTH,0)
{
	mRecord = rhs.mRecord->clone(rdata  AosMemoryCheckerFileLine);
}

AosSchemaRecordByLength::~AosSchemaRecordByLength()
{
}


bool
AosSchemaRecordByLength::config(
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
	if (!rcd_conf)
	{
		AosSetErrorU(rdata, "missing_record : ") << worker_doc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mRecord = AosDataRecordObj::createDataRecordStatic(rcd_conf, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mRecord, false);
	OmnString rcd_name = rcd_conf->getAttrStr(AOSTAG_NAME, "");
	if (rcd_name != "")
	{
		mRecord->setRecordName(rcd_name);                 
	}
	
	return true;
}


bool
AosSchemaRecordByLength::nextRecordset(
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
	aos_assert_rr(data_len > 0, rdata, false);
	
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

	//int num_rcds_to_add = recordset_raw->getNumRecordsToAdd();
	//aos_assert_r(num_rcds_to_add > 0, false);
	char * data = buffdata->data();

	aos_assert_r((int64_t)(offset+sizeof(int)) < data_len, false);
	//while (num_rcds_to_add && offset < data_len)
	while (offset < data_len)
	{
		if (int64_t(offset + sizeof(int)) > data_len)
		{
			contents_incomplete = true;
			break;
		}
		int record_len = *((int*)(data+offset));
		bool rslt = AosBuff::decodeRecordBuffLength(record_len);
		if (!rslt)
		{
			OmnAlarm << "record_len decoding error. Schema name is: " 
				<< mSchemaName << ". record_len is: " << record_len << enderr;
			return false;
		}

		//aos_assert_r(rslt, false);
		if (int64_t(offset + record_len + sizeof(int)) > data_len)
		{
			contents_incomplete = true;
			break;
		}
		AosMetaDataPtr metadata  = buffdata->getMetadata();
		recordset_raw->appendRecord(rdata, 0, offset+sizeof(int), record_len, metadata);
		offset += record_len + sizeof(int);
		//num_rcds_to_add--;
	}

	aos_assert_r(offset <= data_len, false);

	if (contents_incomplete && buffdata->isEOF())
	{
		contents_incomplete = false;
		dataset->addUnrecogContents(rdata, buffdata, buffdata->data(), offset, data_len-offset);
		buffdata->setCrtIdx(data_len);
		return true;
	}
	
	buffdata->setCrtIdx(offset);
	return true;
}


bool
AosSchemaRecordByLength::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
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
AosSchemaRecordByLength::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mRecord, false);
	records.push_back(mRecord);
	return true;
}


void 
AosSchemaRecordByLength::setTaskDocid(const u64 task_docid)
{
	mRecord->setTaskDocid(task_docid);
}


AosJimoPtr 
AosSchemaRecordByLength::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaRecordByLength(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

// jimodb-1301
int 
AosSchemaRecordByLength::nextRecord(
		AosRundata *rdata,                                      
		char * data,
		int data_len,
		AosMetaData* meta,
		AosDataRecordObj *&rcd,
		int &parsed_len)
{
	int status = -2;
	
	aos_assert_r(mRecord,-2);

	if (data_len <= sizeof(int))
	{
		parsed_len = -1;
		return -1;
	}

	int rcdLen = *((int *)data);
	bool rslt = AosBuff::decodeRecordBuffLength(rcdLen);

	if (data_len < sizeof(int)+rcdLen)
	{
		parsed_len = -1;
		return -1;
	}

	data += sizeof(int);			// record's length(4 bytes) + value
	mRecord->clear();

	rslt = mRecord->setData(data,data_len-sizeof(int), meta, status);

	int rcd_len =  mRecord->getRecordLen(); 

	if (status == 0 && rcd_len != rcdLen)
	{
OmnScreen << "rcd_len is " << rcd_len << " rcdLen is " << rcdLen << endl;
mRecord->setData(data,data_len, meta, status);
	}

	if (meta && status == 0)
	{
		meta->moveToNext();
		meta->setRecordLength((int64_t&)rcd_len);
	}
	rcd = mRecord.getPtr();

	if (status != -1)
	{
		parsed_len = rcd_len+sizeof(int);
	}

	else
	{
		parsed_len = -1;
	}

	aos_assert_r(rslt,-2);
	return status;
}

AosSchemaObjPtr
AosSchemaRecordByLength::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosSchemaRecordByLength(*this, rdata AosMemoryCheckerFileLine);
	}
	catch(...)
	{
		OmnAlarm << "Failed creating object" << enderr; 
		return 0;                                       
	}
}
