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
// 2015/9/11 Created by xuqi
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Jimos/DatasetMatrix.h"
#include "NativeAPI/Util/Buff.h"
#include "Util/ValueRslt.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/SchemaObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/BuffData.h"
#include "Util/DiskStat.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetMatrix_0(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetMatrix(version);
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


AosDatasetMatrix::AosDatasetMatrix(
		const OmnString &type,
		const int version)
:
AosDatasetMatrixObj(type, version)
{


}


AosDatasetMatrix::AosDatasetMatrix(const int version)
:
AosDatasetMatrixObj(AOSDATASET_DATAMATRIX, version)
{


}


AosDatasetMatrix::~AosDatasetMatrix()
{


}


AosJimoPtr
AosDatasetMatrix::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatasetMatrix(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}
	OmnShouldNeverComeHere;
	return 0;
}



bool
AosDatasetMatrix::initBuff()
{
	//1.create AosBuff by Rows and Columns
	//2.set all memory with 0
	if( mRowsNum > 0 && mColumnsNum > 0)
	{
		int buff_len = (mRowsNum * mColumnsNum * sizeof(i64)) + mRowsNum * sizeof(int);
		mBuff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
		for (u32 i=0; i<mRowsNum; i++)                                           
		{
			buff_len = sizeof(i64) * mColumnsNum;                         
			AosBuffPtr buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
			char* data = buff->data();                                        
			memset(data, 0, buff_len);                                        
			mBuff->setEncodeCharStr(data, buff_len);
		}

		return true;
	}

	return false;
}
	

bool 
AosDatasetMatrix::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	
	//1.create schema by schema_conf 
	//2.init buff
	AosXmlTagPtr schema_conf = worker_doc->getFirstChild("dataschema");
    aos_assert_r(schema_conf, false);

	// create Recordset
	AosXmlTagPtr recordset_conf = worker_doc->getFirstChild("recordset");
	if (!recordset_conf)
	{
		OmnString str = "<recordset />";
		AosXmlParser xmlparser;
		recordset_conf = xmlparser.parse(str, "" AosMemoryCheckerArgs);
	}
	aos_assert_r(recordset_conf, false);

	mRecordset = AosRecordsetObj::createRecordsetStatic(rdata.getPtr(), recordset_conf);
	aos_assert_r(mRecordset, false);
	
	mSchema = AosSchemaObj::createSchemaStatic(rdata.getPtr(), schema_conf);
    aos_assert_r(mSchema, false);

	mRowsNum = worker_doc->getAttrU32("yrange", 0);
    mColumnsNum = worker_doc->getAttrU32("xrange", 0);
    
	bool rslt = initBuff();
	aos_assert_r(rslt, false);

	return true;

}
	
bool
AosDatasetMatrix::setCellValue(
		AosRundata *rdata,
		const u32 &curColumnsNum,
		const u32 &curRowsNum,
		const AosValueRslt &value)
{
	//find the index and set value in Buff
	aos_assert_r(curRowsNum < mRowsNum && curColumnsNum < mColumnsNum, false);
	
	i64 buffIndex = (curColumnsNum + curRowsNum * mColumnsNum) * sizeof(i64) + sizeof(int) * (curRowsNum+1);

	mBuff->setCrtIdx(buffIndex);
	mBuff->setI64(value.getI64());
	return true;

}


bool
AosDatasetMatrix::nextRecordset(
		const AosRundataPtr &rdata,
		AosRecordsetObjPtr &recordset)
{

	//get recordset by using mSchema->nextRecordset()

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
		bool rslt = mSchema->nextRecordset(rdata.getPtr(), recordset, buffdata, this, incomplete);
        aos_assert_r(rslt, false);
		return true;
	}

    return false;
}


bool
AosDatasetMatrix::getRecords(vector<AosDataRecordObjPtr> &records)
{
	aos_assert_r(mSchema, false);
	return mSchema->getRecords(records);
}

