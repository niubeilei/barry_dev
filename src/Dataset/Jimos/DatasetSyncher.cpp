////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 2015/09/24 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetSyncher.h"
#include "NativeAPI/Util/Buff.h"
#include "Util/ValueRslt.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/SchemaObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/BuffData.h"
#include "Util/DiskStat.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetSyncher_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetSyncher(version);
		aos_assert_r(jimo, 0);
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


AosDatasetSyncher::AosDatasetSyncher(
		const OmnString &type,
		const int version)
:
AosDatasetSyncherObj(type, version),
mCrtOffset(0)
{
}


AosDatasetSyncher::AosDatasetSyncher(const int version)
:
AosDatasetSyncherObj(AOSDATASET_DATASETSYNCHER, version)
{
}


AosDatasetSyncher::~AosDatasetSyncher()
{
}


AosJimoPtr
AosDatasetSyncher::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetSyncher(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDatasetSyncher::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return config(rdata, worker_doc);
}


bool 
AosDatasetSyncher::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	// create Recordset
	AosXmlTagPtr recordset_conf = worker_doc->getFirstChild("recordset");
	if (!recordset_conf)
	{
		OmnString str = "<recordset />";
		AosXmlParser xmlparser;
		recordset_conf = xmlparser.parse(str, "" AosMemoryCheckerArgs);
	}
	aos_assert_r(recordset_conf, false);

	//mRecordset = AosRecordsetObj::createRecordsetStatic(rdata.getPtr(), recordset_conf);
	//aos_assert_r(mRecordset, false);
	
	AosXmlTagPtr rcd_conf = worker_doc->getFirstChild("datarecord");
	aos_assert_r(rcd_conf, false);
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
AosDatasetSyncher::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{
	if(recordset)
	{
		recordset->reset();
	}
    
	while (AosRecordsetObj::checkEmpty(recordset))
	{
		AosDiskStat disk_stat;
		mBuff->reset();
		AosBuffDataPtr buffdata = OmnNew AosBuffData(0, mBuff, disk_stat);
		aos_assert_r(buffdata, false);

		bool incomplete = false;
		bool rslt = nextRecordset(rdata.getPtr(), recordset, buffdata, this, incomplete);
        aos_assert_r(rslt, false);
		return true;
	}

    return false;
}


bool 
AosDatasetSyncher::nextRecordset(           
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
	AosDataRecordObjPtr record = mRecord->clone(rdata AosMemoryCheckerArgs);	
	aos_assert_r(record, false);

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
		recordset_raw->holdRecord(rdata, record);
	}
	else
	{
		recordset_raw = recordset.getPtr();
	}

	recordset->reset();
	recordset->setData(rdata, buffdata->getBuff());

	recordset->setMetaData(rdata, buffdata->getMetadata());

	char * data = buffdata->data();
	while (offset < data_len)
	{
		int record_len, status;
		record->determineRecordLen(&data[offset], data_len - offset, record_len, status);

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
		AosMetaDataPtr metadata = buffdata->getMetadata();
		recordset_raw->appendRecord(rdata, 0, offset, record_len, metadata);
		offset += record_len;
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
AosDatasetSyncher::appendRecord( 
		AosRundata *rdata,
		const int index,
		const int64_t offset,
		const int len,
		const AosMetaDataPtr &metaData)
{
	OmnNotImplementedYet;
	return true;
}


bool		
AosDatasetSyncher::setData(
		AosRundata *rdata,
		const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	mBuff = buff;
	//aos_assert_r(mRecordset, false);	
	//mRecordset->setData(rdata, buff);

	return true;
}


char*
AosDatasetSyncher::getData()
{
	return mBuff->data();
}


bool 		
AosDatasetSyncher::appendRecord(
		AosRundata *rdata,
		AosDataRecordObj *& record)
{
	bool rslt = false;
	char* data = record->getData(rdata);
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);

	AosDataRecordType::E recordtype = mRecord->getType();	
	if (recordtype == AosDataRecordType::eBuff)
	{
		rslt = mBuff->setEncodeCharStr(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	//else if (recordtype == AosDataRecordType::eFixedBinary)
	//{
	//	mRecordLen = rcd_len;
	//	rslt = mBuff->setBuff(data, rcd_len);
	//	aos_assert_r(rslt, false);
	//}
	else
	{
		OmnNotImplementedYet;
		return false;
	}
	return true;
}


bool 		
AosDatasetSyncher::nextRecord(
		AosRundata* rdata,
		AosDataRecordObj *& record)
{
	if (mCrtOffset == mBuff->dataLen())
	{
		record = 0;
		return true;
	}
	bool rslt = false;
	i64 offset = 0;
	int record_len = -1;
	char* data = mBuff->data();

	AosDataRecordType::E recordtype = mRecord->getType();
	//record = mRecord->clone(rdata AosMemoryCheckerArgs).getPtr();
	record = mRecord.getPtr();
	aos_assert_r(record, false);
	record->clear();

	AosMetaDataPtr metaData;
	if (recordtype == AosDataRecordType::eBuff)
	{
		record_len = *((int*)(data+mCrtOffset));
		rslt = AosBuff::decodeRecordBuffLength(record_len);
		aos_assert_r(rslt, false);
		offset = mCrtOffset + sizeof(int);
		aos_assert_r(record, false);
		mCrtOffset = offset + record_len;
	}
	//else if (recordtype == AosDataRecordType::eFixedBinary)
	//{
	//	record_len = mRecordLen;
	//	offset = mCrtOffset;
	//	mCrtOffset += record_len;
	//}
	else
	{
		OmnNotImplementedYet;
		return false;
	}

	int status;
	record->setData(&data[offset], record_len, metaData.getPtr(), status);
	return true;
}


AosRecordsetObjPtr
AosDatasetSyncher::cloneRecordset(AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	return NULL;
}
