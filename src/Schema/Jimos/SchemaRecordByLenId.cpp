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
#include "Schema/Jimos/SchemaRecordByLenId.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "MetaData/DocidMetaData.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaRecordByLenId_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSchemaRecordByLenId(version);
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


AosSchemaRecordByLenId::AosSchemaRecordByLenId(const int version)
:
AosSchema(AOSSCHEMA_RECORD, version),
mSchemaDocid(0),
mIsMultiRecord(false)
{
}

AosSchemaRecordByLenId::AosSchemaRecordByLenId(
		const AosSchemaRecordByLenId &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosSchema(AOSSCHEMA_RECORD,0)
{
	for (size_t i=0; i<rhs.mRecords.size(); i++)
	{
		AosDataRecordObjPtr record = rhs.mRecords[i]->clone(rdata AosMemoryCheckerFileLine);
		mRecords.push_back(record);
	}
	mRecord = rhs.mRecord->clone(rdata AosMemoryCheckerFileLine);
}

AosSchemaRecordByLenId::~AosSchemaRecordByLenId()
{
}


bool
AosSchemaRecordByLenId::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// The configure format:
	// 	<dataschema zky_name="xxx" jimo_objid="dataschema_record_bylenid_jimodoc_v0" >
	// 		<datarecord zky_datarecord_name="xxx" ..../>
	// 	</dataschema>
	aos_assert_r(worker_doc, false);
	AosXmlTagPtr rcd_conf = worker_doc->getFirstChild("datarecord");
	//aos_assert_r(rcd_conf, false);
	//AosXmlTagPtr child_rcd = rcd_conf->getFirstChild("datarecords");
	if (!rcd_conf)
	{
		mSchemaDocid = worker_doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(mSchemaDocid, false);
				// 2015.10.26
		AosXmlTagPtr dataschema = AosDocClientObj::getDocClient()->getDocByDocid(mSchemaDocid, rdata); 
		aos_assert_r(dataschema, false);

		rcd_conf = dataschema->getFirstChild("datarecord");
		aos_assert_r(rcd_conf, false);
		//mIsMultiRecord = true;
		//return true;
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
AosSchemaRecordByLenId::nextRecordset(
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
	
	if (mIsMultiRecord) 
	{
		return nextRecordsetByMulti(
				rdata, recordset, buffdata, dataset, contents_incomplete);
	}

	aos_assert_r(buffdata, false);
	aos_assert_r(mRecord, false);

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
	}
	else
	{
		recordset_raw = recordset.getPtr();
	}

	recordset->reset();
	recordset->setData(rdata, buffdata->getBuff());

	AosMetaDataPtr metaData = buffdata->getMetadata();
	recordset->setMetaData(rdata, metaData);

	//int num_rcds_to_add = recordset_raw->getNumRecordsToAdd();
	//aos_assert_r(num_rcds_to_add > 0, false);

	char * data = buffdata->data();
	
	//while (num_rcds_to_add && offset < data_len)
	while (offset < data_len)
	{
		u32 record_len = *(u32 *)(data+offset);
		if (record_len == 0 || offset + record_len > data_len)
		{
			contents_incomplete = true;
			break;
		}
		offset += sizeof(u32);

		offset += sizeof(u64);
/*
{
u64 schema_docid = *(u64*)(data+offset);
OmnString tmp(data+offset, record_len);
OmnScreen << "debug schema_docid: " << schema_docid << " len: " << record_len << " doc: " << tmp << endl;
}
*/

		recordset_raw->appendRecord(rdata, 0, offset, record_len, metaData);
		offset += record_len;
		//num_rcds_to_add--;
	}

	aos_assert_r(offset <= data_len, false);
	if (contents_incomplete && buffdata->isEOF())
	{
		contents_incomplete = false;
		dataset->addUnrecogContents(rdata, buffdata, buffdata->data(), offset, data_len-offset);
OmnScreen << "XXXXXXXXXXXXXXXXXXXXXXXXx" << endl;
	}

	buffdata->setCrtIdx(offset);
	return true;
}

bool
AosSchemaRecordByLenId::nextRecordsetByMulti(
		AosRundata *rdata, 
		AosRecordsetObjPtr &recordset, 
		const AosBuffDataPtr &buffdata,
		AosDatasetObj *dataset,
		bool &contents_incomplete)
{
	aos_assert_r(buffdata, false);
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
		//recordset_raw->holdRecord(rdata, mRecord);
	}
	else
	{
		recordset_raw = recordset.getPtr();
	}

	recordset->reset();
	recordset->setData(rdata, buffdata->getBuff());
	AosMetaDataPtr metaData = buffdata->getMetadata();
	recordset->setMetaData(rdata, metaData);

	//int num_rcds_to_add = recordset_raw->getNumRecordsToAdd();
	//aos_assert_r(num_rcds_to_add > 0, false);

	char * data = buffdata->data();
	int index = -1;
	
	while (offset < data_len)
	{
		u32 record_len = *(u32*)(data+offset);
		if (record_len == 0 || offset + record_len > data_len)
		{
			contents_incomplete = true;
			break;
		}
		offset += sizeof(u32);

		u64 schema_docid = *(u64*)(data+offset);
		aos_assert_r(schema_docid != 0, false);
		offset += sizeof(u64);

		index = getRecordIndex(rdata, mSchemaDocid, recordset_raw);
		aos_assert_r(index > -1, false);

		recordset_raw->appendRecord(rdata, index, offset, record_len, metaData);
		offset += record_len;
		//num_rcds_to_add--;
	}

	aos_assert_r(offset <= data_len, false);
	if (contents_incomplete && buffdata->isEOF())
	{
		contents_incomplete = false;
		dataset->addUnrecogContents(rdata, buffdata, buffdata->data(), offset, data_len-offset);
OmnScreen << "XXXXXXXXXXXXXXXXXXXXXXXXx" << endl;
	}

	buffdata->setCrtIdx(offset);
	return true;
}


int 
AosSchemaRecordByLenId::getRecordIndex(
	AosRundata *rdata, 
	const u64 &schema_docid, 
	AosRecordsetObj *recordset_raw)
{
	int index = -1;
	u64 record_docid  = 0; 

	if (mRecordDocids.count(record_docid) == 0)
	{
		AosXmlTagPtr schema_doc = AosGetDocByDocid(schema_docid, rdata);
		aos_assert_r(schema_doc, index);

		AosXmlTagPtr record_conf = schema_doc->getFirstChild(true);
		aos_assert_r(record_conf, index);

		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(record_conf, mTaskDocid, rdata AosMemoryCheckerArgs);
		aos_assert_r(record, index);
		mRecords.push_back(record);
		index = mRecords.size() -1;
		mRecordDocids[record_docid] = index;
		recordset_raw->holdRecord(rdata, record);
	}
	else                                        
	{                                           
		index = mRecordDocids[record_docid];    
	}                                           
	return index;
}


bool
AosSchemaRecordByLenId::getRecord(const OmnString &name, AosDataRecordObjPtr &record)
{
	record = NULL;
	OmnString key;
	if (mIsMultiRecord)
	{
		aos_assert_r(!mRecords.empty(), false);
		for (u32 i = 0; i < mRecords.size(); i++)
		{
			key = mRecords[i]->getRecordName();
			if (name == key)
			{
				record = mRecords[i];
			}
		}
		return true;
	}

	aos_assert_r(mRecord, false);
	key = mRecord->getRecordName();
	if (name == key)
	{
		record = mRecord;
	}
	return true;
}

	
bool
AosSchemaRecordByLenId::getRecords(vector<AosDataRecordObjPtr> &records)
{
	if (mIsMultiRecord)
	{
		aos_assert_r(!mRecords.empty(), false);
		records.insert(records.end(), mRecords.begin(), mRecords.end());
		return true;
	}
	aos_assert_r(mRecord, false);
	records.push_back(mRecord);
	return true;
}


void 
AosSchemaRecordByLenId::setTaskDocid(const u64 task_docid)
{
	mTaskDocid = task_docid;
	if (mIsMultiRecord)
	{
		for (u32 i = 0; i < mRecords.size(); i++)
		{
			mRecords[i]->setTaskDocid(task_docid);
		}
		return;
	}
	mRecord->setTaskDocid(task_docid);
	return;
}


AosJimoPtr 
AosSchemaRecordByLenId::cloneJimo()  const
{
	try
	{
		return OmnNew AosSchemaRecordByLenId(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

// jimodb-1301
int 
AosSchemaRecordByLenId::nextRecord(
		AosRundata *rdata,                                      
		char * data,
		int data_len,
		AosMetaData* meta,
		AosDataRecordObj *&rcd,
		int &parsed_len)
{
	int status = -2;
	int rcdLen;
	aos_assert_r(mRecord,status);
	
	if (data_len > sizeof(int))
	{
		rcdLen = *((int *)data);
	}
	else
	{
		parsed_len = -1;
		return -1;
	}
	if (data_len < sizeof(int) + sizeof(u64) + rcdLen)
	{
		parsed_len = -1;
		return -1;
	}
	
	mRecord->clear();
	data += (sizeof(int) + sizeof(u64));

	bool rslt = mRecord->setData(data, rcdLen, meta, status);

	int rcd_len =  mRecord->getRecordLen(); 
	if (meta && status == 0)
	{
		meta->moveToNext();
		meta->setRecordLength((int64_t&)rcdLen);
	}
	//if (status == 0)
	//	aos_assert_r(rcd_len == rcdLen, status); 

	rcd = mRecord.getPtr();
	parsed_len = rcd_len + sizeof(int) + sizeof(u64);
	aos_assert_r(rslt, -2);

	return status;
}

AosSchemaObjPtr
AosSchemaRecordByLenId::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosSchemaRecordByLenId(*this, rdata AosMemoryCheckerFileLine);
	}
	catch(...)
	{
		OmnAlarm << "Failed creating object" << enderr;   
		return 0;                                         
	}
}
