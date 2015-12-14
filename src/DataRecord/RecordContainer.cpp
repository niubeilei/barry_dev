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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordContainer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosRecordContainer::AosRecordContainer(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eContainer, 
		AOSRECORDTYPE_CTNR, flag AosMemoryCheckerFileLine),
mBuff(0),
mBuffRaw(0),
mCrtRecordIdx(0),
mNumRecord(0),
mRecordRaw(0),
mAssembler(0)
{
}


AosRecordContainer::AosRecordContainer(
		const AosRecordContainer &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mCrtRecordIdx(rhs.mCrtRecordIdx),
mNumRecord(rhs.mNumRecord)
{
	if (rhs.mBuff)
	{
		mBuff = rhs.mBuff->clone(AosMemoryCheckerArgsBegin);
		mMemory = mBuff->data();
	}
	mBuffRaw = mBuff.getPtr();

	if (rhs.mRecord)
	{
		mRecord = rhs.mRecord->clone(rdata AosMemoryCheckerArgs);
	}
	mRecordRaw = mRecord.getPtr();

	mAssembler = rhs.mAssembler;
}


AosRecordContainer::~AosRecordContainer()
{
}


bool
AosRecordContainer::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr record_tag = def->getFirstChild("datarecord");
	aos_assert_r(record_tag, false);

	mRecord = AosDataRecordObj::createDataRecordStatic(
				record_tag, mTaskDocid, rdata AosMemoryCheckerArgs);
	if (!mRecord)
	{
		AosSetEntityError(rdata, "record_container_invalid_record_def", 
				"RecordContainer", "RecordContainer")
			<< record_tag->toString() << enderr;
		return false;
	}
	mRecordRaw = mRecord.getPtr();

	mBuff = OmnNew AosBuff(eDftBuffSize AosMemoryCheckerArgs);
	mBuffRaw = mBuff.getPtr();

	mMemory = mBuff->data();
	memset(mMemory, 0, eDftBuffSize);
	mMemLen = eDftBuffSize;
	mCrtRecordIdx = 0;
	mNumRecord = 0;

	mRecordRaw->setMemory(mMemory, mMemLen);
	return true;
}


AosDataRecordObjPtr 
AosRecordContainer::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordContainer(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr 
AosRecordContainer::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordContainer(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}



AosDataRecordObjPtr 
AosRecordContainer::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordContainer * record = OmnNew AosRecordContainer(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordContainer::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata) 
{
	//aos_assert_r(mRecord, false);
	//return mRecord->getFieldValue(idx, value, rdata);
	return mRecordRaw->getFieldValue(idx, value, copy_flag, rdata);
}



int
AosRecordContainer::getFieldIdx(
		const OmnString &name,
		AosRundata *rdata)
{
	//aos_assert_r(mRecord, -1);
	//return mRecord->getFieldIdx(name, rdata);
	return mRecordRaw->getFieldIdx(name, rdata);
}


bool		
AosRecordContainer::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	//aos_assert_r(mRecord, false);
	outofmem = false;
	//aos_assert_r(idx >= 0 && idx < mRecord->getNumFields(), false);
	aos_assert_r(idx >= 0 && idx < mRecordRaw->getNumFields(), false);
	mIsDirty = true;
	//AosDataRecord::setFieldValue(idx, value, outofmem, rdata);  

	bool rslt;
	aos_assert_r(mMemLen-mCrtRecordIdx >= 0, false);

	if (mMemLen-mCrtRecordIdx > 0)
	{
		mMemory = &mBuffRaw->data()[mCrtRecordIdx];
		mRecordRaw->setMemory(mMemory, mMemLen-mCrtRecordIdx);
		rslt = mRecordRaw->setFieldValue(idx, value, outofmem, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		outofmem = true;
	}
	while (outofmem)
	{
		rslt = increaseMemory();
		aos_assert_r(rslt, false);

		mRecordRaw->setMemory(mMemory, mMemLen-mCrtRecordIdx);
		rslt = mRecordRaw->setFieldValue(idx, value, outofmem, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosRecordContainer::increaseMemory()
{
	int64_t old_len = mBuffRaw->buffLen();
	int64_t crt_pos = mCrtRecordIdx + (mMemLen - mCrtRecordIdx);
		
	mBuffRaw->setDataLen(mMemLen);
	//	mMemLen += 2 * old_len;
	if (mMemLen > eDftIncreaseSize)
	{
		mMemLen += eDftIncreaseSize/5;
	}
	else
	{
		mMemLen += eDftIncreaseSize/2;
	}

	aos_assert_r(mMemLen < eMaxBuffSize, false);
	int64_t new_len = mBuffRaw->resize(mMemLen);
	aos_assert_r(mMemLen <= new_len, false);

	mMemory = mBuffRaw->data();
	int64_t left_size = new_len - old_len;
	memset(&mMemory[crt_pos], 0, left_size);
	mBuffRaw->setDataLen(mMemLen);

	aos_assert_r(mMemLen-mCrtRecordIdx > 0, false);
	mMemory = &mBuffRaw->data()[mCrtRecordIdx];

	return true;
}

	
bool
AosRecordContainer::setFieldValue(
		const OmnString &field_name, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	//aos_assert_r(mRecord, false);
	//int idx = mRecord->getFieldIdx(field_name, rdata);
	int idx = mRecordRaw->getFieldIdx(field_name, rdata);
	return setFieldValue(idx, value, outofmem, rdata);
}


void
AosRecordContainer::clear()
{
	AosDataRecord::clear();

	mBuffRaw->reset();
	mMemLen = mBuffRaw->buffLen();
	mCrtRecordIdx = 0;
	mNumRecord = 0;
	mRecordLens.clear();
	mRecordRaw->clear();
}

void		
AosRecordContainer::flushRecord(AosRundata *rdata_raw) 
{ 
	if (mIsDirty)
	{
		//aos_assert(mRecord);
	//	if (!((AosDataRecord*)mRecordRaw)->isDirty())
	//	{
	//		OmnAlarm << enderr;
	//	}
		mNumRecord = 0; 
		mRecordRaw->flush();
		mIsDirty = false;
		//Jackie 2015/01/29
		aos_assert(mAssembler);
		mAssembler->appendEntry(rdata_raw, mRecordRaw);
	}
}



bool 		
AosRecordContainer::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	//OmnShouldNeverComeHere;
	//return false;
	//arvin 2015.11.17
	//Jimodb-1087
	//the data just have one record,use in DataProcGroupBy.cpp
	mIsDirty =  true;
	mRecordRaw->setData(data,len,metaData,status);
	return true;
}

AosXmlTagPtr
AosRecordContainer::serializeToXmlDoc(
		const char *data, 
		const int data_len,
		AosRundata* rdata)
{
	OmnString docStr = "";
	AosXmlTagPtr insideDoc = mRecordRaw->serializeToXmlDoc(data, data_len, rdata);

	docStr << "<datarecord type=\"ctnr\" zky_name=\"" << mName << "\">";
	docStr << insideDoc->toString();
	docStr << "</datarecord>";

	AosXmlTagPtr doc = AosXmlParser::parse(docStr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	return doc;
}
