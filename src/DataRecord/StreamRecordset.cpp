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
// A record set holds a number of records. It is used by a consumer. 
// For performance considerations, record sets are not thread safe. 
// If the caller wants to make a record set thread safe, it must be
// protected using locks before using the record set.
//
// Record sets returns raw pointers instead of smart pointers. Users
// should never delete the pointers. 
//
// 2015/08/06 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/StreamRecordset.h"

#include "DataRecord/RecordContainer.h"
#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/BuffArrayVar.h"
#include "Debug/Except.h"
#include "Thread/Mutex.h"
#include "MetaData/MetaData.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
//#include "Sorter/MergeFileSorter.h"

AosStreamRecordset::AosStreamRecordset()
{
	mLock = OmnNew OmnMutex();
	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mBuffRaw = mBuff.getPtr();
	mSize = 0;
	mWriteIdx = 0;
	mCrtOffset = 0;
}


AosStreamRecordset::AosStreamRecordset(
		const AosStreamRecordset &rhs,
		AosRundata *rdata)
:
mLock(OmnNew OmnMutex()),
mSize(rhs.mSize),
mWriteIdx(rhs.mWriteIdx),
mCrtOffset(0)
{
	mRecords.clear();

	AosDataRecordObjPtr rcd = NULL;
	AosDataRecordObjPtr rcd1 = NULL;
	for (u32 i = 0; i < rhs.mRecords.size(); i++)
	{
		rcd = rhs.mRecords[i];
		aos_assert(rcd);

		rcd1 = rcd->clone(rdata AosMemoryCheckerArgs); 
		aos_assert(rcd1);
		mRecords.push_back(rcd1);
	}

	//levi 2015/10/4
	//OmnScreen<<"streamrecordset mRecord.size : " << mRecords.size() <<endl;

	if (rhs.mRecord)
	{
		mRecord = rhs.mRecord->clone(rdata AosMemoryCheckerArgs);
		aos_assert(mRecord);

		mRecordRaw = mRecord.getPtr();
	}
	mRecordType = rhs.mRecordType;

	if(rhs.mBuff)
	{
		mBuff = rhs.mBuff->clone(AosMemoryCheckerArgsBegin);
		mBuffRaw = mBuff.getPtr();
	}
}


AosStreamRecordset::~AosStreamRecordset()
{
}


bool
AosStreamRecordset::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	return true;	
}


AosRecordsetObjPtr
AosStreamRecordset::clone(AosRundata *rdata) const
{
	try
	{
		mLock->lock();
		AosRecordsetObjPtr streamRcdset = OmnNew AosStreamRecordset(*this, rdata);
		mLock->unlock();

		return streamRcdset;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

	
AosRecordsetObjPtr 
AosStreamRecordset::createStreamRecordset(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	AosStreamRecordset* recordset = OmnNew AosStreamRecordset();
	if (record->getType() == AosDataRecordType::eContainer)
	{
		AosRecordContainer* rcd = dynamic_cast<AosRecordContainer *>(record.getPtr());
		aos_assert_rr(rcd, rdata, 0);

		recordset->mRecordType = (rcd->mRecord)->getType();
		recordset->holdRecord(rdata, rcd->mRecord);
	}
	else
	{
		recordset->mRecordType = record->getType();
		recordset->holdRecord(rdata, record);
	}
	//recordset->holdRecord(rdata, rcd->mRecord);
//	recordset->mRecordType = (rcd->mRecord)->getType();
//	recordset->mRecord = record;
//	recordset->mRecordRaw = record.getPtr();

	/*
	//get Record Type
	AosXmlTagPtr recordXml = rcd->getRecordDoc();
	AosXmlTagPtr rcdXml = recordXml->getFirstChild("datarecord");
	OmnString type = rcdXml->getAttrStr("type");

	recordset->mRecordType = AosDataRecordType::toEnum(type);
	recordset->mRecordRaw = (recordset->mRecord).getPtr();
	*/
	return recordset;
}


bool
AosStreamRecordset::swap(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	OmnNotImplementedYet;
	return false;
}


AosRecordsetObjPtr 
AosStreamRecordset::createRecordset(
		AosRundata *rdata,
		const AosXmlTagPtr &def)
{
	OmnNotImplementedYet;
	return 0;
}

AosRecordsetObjPtr 
AosStreamRecordset::createRecordset(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosStreamRecordset::nextRecord(
		AosRundata* rdata, 
		AosDataRecordObj * &record)
{
	if (mCrtOffset == mBuffRaw->dataLen())
	{
		record = 0;
		return true;
	}
	bool rslt = false;
	i64 offset = 0;
	int record_len = -1;
	char* data = mBuffRaw->data();

	//mRecordRaw = mRecords[mWriteIdx].getPtr();
	mRecordRaw = mRecords[0].getPtr();
	aos_assert_r(mRecordRaw, false);
	record = mRecordRaw;
	record->clear();

	AosMetaDataPtr metaData;
	if (mRecordType == AosDataRecordType::eBuff)
	{
		record_len = *((int*)(data+mCrtOffset));
		rslt = AosBuff::decodeRecordBuffLength(record_len);
		aos_assert_r(rslt, false);
		offset = mCrtOffset + sizeof(int);
		aos_assert_r(record, false);
		mCrtOffset = offset + record_len;
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		record_len = mRecordLen;
		offset = mCrtOffset;
		mCrtOffset += record_len;
	}
	else
	{
		record_len = *(int*)((data+mCrtOffset));
		offset = mCrtOffset + sizeof(int);
		mCrtOffset = offset + record_len;
	}
	int status;
	record->setData(&data[offset], record_len, metaData.getPtr(), status);
	mWriteIdx++;
	return true;
}

bool
AosStreamRecordset::appendRecord(
		AosRundata *rdata, 
		const int index,
		const int64_t offset, 
		const int len,
		const AosMetaDataPtr &metaData)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosStreamRecordset::sort(const AosRundataPtr &rdata, const AosCompareFunPtr &comp)
{
	if (mRecordType == AosDataRecordType::eBuff)
	{
		AosBuffArrayVarPtr array = OmnNew AosBuffArrayVar(mBuff, comp, false);
		aos_assert_r(array, false);
		array->sort();
		//felicia, 2015/08/28
		if(comp->mAosAgrs.size() > 0)
		{
			array->mergeData();
		}
		AosBuffPtr headBuff = array->getHeadBuff();
		set<i64> bodyAddrSet;
		AosBuffPtr bodyBuff;
		AosBuffArrayVar::procHeaderBuff(comp, headBuff, bodyBuff, bodyAddrSet);
//AosMergeFileSorter::sanitycheck_var(comp.getPtr(), bodyBuff->data(), bodyBuff->dataLen());
		mBuff = bodyBuff;
		mBuffRaw = mBuff.getPtr();
		return true;
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		AosBuffArrayPtr array = OmnNew AosBuffArray(mBuff, comp, false);
		array->sort();
		//felicia, 2015/08/28
		if(comp->mAosAgrs.size() > 0)
		{
			array->mergeData();
		}
		mBuff = array->getBuff();
		mBuffRaw = mBuff.getPtr();
		return true;
	}
	else
	{
		OmnNotImplementedYet;
	}
	return false;
}


bool 
AosStreamRecordset::serializeTo(AosRundata *rdata,
		                  AosBuff *buff)
{
	i64 len = mBuffRaw->dataLen();
	aos_assert_r(len > 0, false);
	buff->setI64(len);
	buff->setBuff(mBuffRaw);
	buff->setInt(mSize);
	buff->setU32((u32)mRecordType);
	
	//save record xml 
	buff->setU32(mRecords.size());
	AosXmlTagPtr doc; 
	for (u32 i = 0; i < mRecords.size(); i++)
	{
		doc = (mRecords[i])->getRecordDoc();

		buff->setOmnStr(doc->toString());
	}
	
	return true;
}

bool 
AosStreamRecordset::serializeFrom(AosRundata *rdata,
		                    AosBuff *buff)
{
	i64 len = buff->getI64(-1);
	aos_assert_r(len > 0, false);
	mBuff = buff->getBuff(len, true AosMemoryCheckerArgs);
	aos_assert_r(mBuff, false);
	mBuffRaw = mBuff.getPtr();
	mSize = buff->getInt(0);
	mRecordType = (AosDataRecordType::E)buff->getU32(0);
	
	//construct records and hold
	u32 numRecords;
	numRecords = buff->getU32(0);
	AosXmlTagPtr doc; 
	OmnString strDoc;
	AosDataRecordObjPtr rcd;
	for (u32 i = 0; i < numRecords; i++)
	{
		strDoc = buff->getOmnStr("");
		doc = AosStr2Xml(rdata, strDoc AosMemoryCheckerArgs);
		aos_assert_r(doc, false);

		rcd = AosDataRecordObj::createDataRecordStatic(doc, NULL, rdata AosMemoryCheckerArgs);
		aos_assert_r(rcd, false);

		holdRecord(rdata, rcd);
	}
	
	return true;
}


char*
AosStreamRecordset::getData()
{
	return mBuff->data();
}

bool
AosStreamRecordset::setData(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return true;
}


AosDataRecordObjPtr
AosStreamRecordset::getRecord()
{
	OmnNotImplementedYet;
	return 0;
}


vector<AosDataRecordObjPtr> 
AosStreamRecordset::getRecords()
{
	return mRecords;
}

AosDataRecordObj *
AosStreamRecordset::getRawRecord(const int idx)
{
	OmnNotImplementedYet;
	return 0;
}

bool
AosStreamRecordset::holdRecord(
		AosRundata *rdata, 
		const AosDataRecordObjPtr &record)
{
	AosDataRecordObjPtr new_record = record->clone(rdata AosMemoryCheckerArgs);	
	aos_assert_r(new_record, false);
	mRecords.push_back(new_record);

	return true;
}

	
bool
AosStreamRecordset::holdRecords(
		AosRundata *rdata, 
		const vector<AosDataRecordObjPtr> &records)
{
	OmnNotImplementedYet;
	return false;
}


void
AosStreamRecordset::setMetaData(
		AosRundata *rdata,  
		const AosMetaDataPtr &metadata)
{
	OmnNotImplementedYet;
}

bool
AosStreamRecordset::reset()
{
	//OmnNotImplementedYet;
	mBuff = 0;
	mBuffRaw = NULL;
	mRecords.clear();
	mRecordRaw = NULL;
	mCrtOffset = 0;
	mRecordLen = 0;
	mSize = 0;
	mWriteIdx = 0;
	return true;
}


bool
AosStreamRecordset::appendRecord(
		AosRundata *rdata, 
		AosDataRecordObjPtr &record,
		AosBuffDataPtr &metaData)
{
	mSize++;
	bool rslt = false;
	char* data = record->getData(rdata);
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);

//	AosRecordContainer* rcdContainer = dynamic_cast<AosRecordContainer *>(record.getPtr());

//	mRecordType = (rcdContainer->mRecord)->getType();
//	holdRecord(rdata, rcdContainer->mRecord);

	//mRecords.push_back(record);

	if (mRecordType == AosDataRecordType::eBuff)
	{
		rslt = mBuff->setEncodeCharStr(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		mRecordLen = rcd_len;
		rslt = mBuff->setBuff(data, rcd_len);
		aos_assert_r(rslt, false);
	}
	else if (mRecordType == AosDataRecordType::eCSV)
	{
		rslt = mBuff->setCharStr(data, rcd_len);
		aos_assert_r(rslt,false);
		return true;
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}
	return true;
}


void 
AosStreamRecordset::resetReadIdx()
{
	mCrtOffset = 0;
	mWriteIdx = 0;
}

bool
AosStreamRecordset::resetOffset()
{
	mCrtOffset = 0;
	return true;
}

bool 
AosStreamRecordset::replaceRawRecord(AosDataRecordObj* record)
{
	aos_assert_r(record, false)
	mRecords[0] = record;
	return true;
}

bool 
AosStreamRecordset::setDataBuff(                
		AosRundata *rdata,       
		AosBuffPtr buffdata)
{
	mBuff = buffdata;
	mBuffRaw=mBuff.getPtr();
	return true;
}
