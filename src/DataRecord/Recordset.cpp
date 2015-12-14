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
// Records are saved in the form:
// 	[AosDataRecordObj *, data, len]
// 	[AosDataRecordObj *, data, len]
// 	...
// 	[AosDataRecordObj *, data, len]
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/Recordset.h"

#include "DataRecord/RecordContainer.h"
#include "DataRecord/StreamRecordset.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/BuffArrayVar.h"
#include "Debug/Except.h"
#include "MetaData/MetaData.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"
#include <limits.h>


//int64_t AosRecordset::smMaxRecordsetSize = AosRecordset::eDftMaxRecordsetSize;

AosRecordset::AosRecordset()
:
mMaxSize(-1),
mNumEntries(0),
mReadIdx(0),
mWriteIdx(0),
mDataLen(0),
mOffset(0),
mEntries(0),
mData(0),
mEntryBuffRaw(0),
mDataBuffRaw(0)
{
	mMetaDataRaw = NULL;
}


AosRecordset::AosRecordset(
		const AosRecordset &rhs, 
		AosRundata *rdata)
:
mMaxSize(rhs.mMaxSize),
mNumEntries(rhs.mNumEntries),
mReadIdx(rhs.mReadIdx),
mWriteIdx(rhs.mWriteIdx),
mDataLen(rhs.mDataLen),
mOffset(rhs.mOffset)
{

	//2015-07-25 Phil
	//need to clone records object instead of using rhs's
	//datarecord object pointer directly
	mRecords.clear();
	mRawRecords.clear();
	mEntryBuffRaw = NULL;

	AosDataRecordObjPtr rcd = NULL;
	AosDataRecordObjPtr rcd1 = NULL;
	for (u32 i = 0; i < rhs.mRecords.size(); i++)
	{
		rcd = rhs.mRecords[i];
		aos_assert(rcd);

		rcd1 = rcd->clone(rdata AosMemoryCheckerArgs); 
		aos_assert(rcd1);
		mRecords.push_back(rcd1);

		aos_assert(rcd1.getPtr());
		mRawRecords.push_back(rcd1.getPtr());
	}

	if (rhs.mDataBuff)
	{
		mDataBuff = rhs.mDataBuff->clone(AosMemoryCheckerArgsBegin);
		mData = mDataBuff->data();
		aos_assert(mData == mDataBuff->data());
//OmnScreen << (void*)this << "," << (void*)mData << "," << (void*)mDataBuff->data() << endl;
	}
	else
	{
		mDataBuff = 0;
		mData = 0;
	}
	mDataBuffRaw = mDataBuff.getPtr();
	
	if (rhs.mEntryBuff)
	{
		mEntryBuff = rhs.mEntryBuff->clone(AosMemoryCheckerArgsBegin);
		mEntryBuffRaw = mEntryBuff.getPtr();

		mEntries = (Entry *)mEntryBuffRaw->data();
	}
	else
	{
		int64_t size = eDftRecordsetSize;
		if (mMaxSize > eDftRecordsetSize)
			size = mMaxSize;
		mEntryBuff = OmnNew AosBuff(size * sizeof(Entry) AosMemoryCheckerArgs);
		mEntryBuffRaw = mEntryBuff.getPtr();

		mEntries = (Entry *)mEntryBuffRaw->data();
	}
	mMetaData = rhs.mMetaData;
	mMetaDataRaw = mMetaData.getPtr();
}


// This constructor sets the record max size, initializes the memory
// for 'mEntries'. The caller should set the data later on.
AosRecordset::AosRecordset(
		AosRundata *rdata, 
		const int max_size)
:
mMaxSize(max_size),
mNumEntries(0),
mReadIdx(0),
mWriteIdx(0),
mOffset(0),
mEntries(0),
mData(0)
{
//	if (mMaxSize <= 0)
//	{
//		AosSetErrorU(rdata, "invalid_size") << mMaxSize << enderr;
//		OmnThrowException(rdata->getErrmsg());
//		return;
//	}

//	if (mMaxSize > smMaxRecordsetSize)
//	{
//		AosSetErrorU(rdata, "size_too_big") << mMaxSize << enderr;
//		OmnThrowException(rdata->getErrmsg());
//		return;
//	}

	int64_t size = eDftRecordsetSize;
	if (mMaxSize > eDftRecordsetSize)
		size = mMaxSize;
	mEntryBuff = OmnNew AosBuff(size * sizeof(Entry) AosMemoryCheckerArgs);
	mEntryBuffRaw = mEntryBuff.getPtr();

	mEntries = (Entry *)mEntryBuffRaw->data();
	mDataBuffRaw = NULL;
	mMetaDataRaw = NULL;
}


AosRecordset::~AosRecordset()
{
}


bool
AosRecordset::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	OmnTagFuncInfo << endl;
	//mMaxSize = def->getAttrInt64("recordset_size", eDftRecordsetSize);

//	if (mMaxSize > smMaxRecordsetSize)
//	{
//		AosSetErrorUser(rdata, "size_too_big") << mMaxSize << enderr;
//		return false;
//	}

	//int64_t size = eDftRecordsetSize;
	//if (mMaxSize > eDftRecordsetSize)
		//size = mMaxSize;
	mEntryBuff = OmnNew AosBuff(eDftRecordsetSize* sizeof(Entry) AosMemoryCheckerArgs);
	mEntryBuffRaw = mEntryBuff.getPtr();

	mEntries = (Entry *)mEntryBuffRaw->data();
	return true;	
}


AosRecordsetObjPtr
AosRecordset::clone(AosRundata *rdata) const
{
	try
	{
		return OmnNew AosRecordset(*this, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

AosRecordsetObjPtr
AosRecordset::createStreamRecordset(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	AosStreamRecordset *streamRcdset = OmnNew AosStreamRecordset();
	aos_assert_r(streamRcdset, 0);
	AosRecordsetObjPtr rcdset = streamRcdset->createStreamRecordset(rdata, record);
	aos_assert_r(rcdset, 0);
	OmnDelete streamRcdset;
	return rcdset;
}

	
AosRecordsetObjPtr 
AosRecordset::createRecordset(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	OmnTagFuncInfo << endl;
	AosRecordset * recordset = OmnNew AosRecordset();
	// AosRecordContainer* rcd = (AosRecordContainer *)(record.getPtr());
	AosRecordContainer* rcd = dynamic_cast<AosRecordContainer *>(record.getPtr());
	aos_assert_rr(rcd, rdata, 0);
	
	recordset->mMaxSize = rcd->mNumRecord;
	recordset->mReadIdx = 0;
	recordset->mOffset = 0;;
	
	int64_t size = eDftRecordsetSize;
	if (recordset->mMaxSize > eDftRecordsetSize)
		size = recordset->mMaxSize;
	recordset->mEntryBuff = OmnNew AosBuff(size * sizeof(Entry) AosMemoryCheckerArgs);
	recordset->mEntryBuffRaw = recordset->mEntryBuff.getPtr();
	recordset->mEntries = (Entry *)recordset->mEntryBuff->data();

	//felicia,2015/08/18 
	//recordset->setData(rdata, rcd->mBuff);
	AosBuffPtr data_buff = (rcd->mBuff)->clone(AosMemoryCheckerArgsBegin);
	recordset->setData(rdata, data_buff);
	
	recordset->holdRecord(rdata, rcd->mRecord);

	int64_t offset = 0;
	for(u32 i=0; i<rcd->mRecordLens.size(); i++)
	{
		recordset->appendRecord(rdata, 0, offset, rcd->mRecordLens[i], 0);	
		offset += rcd->mRecordLens[i];
	}
	//aos_assert_r(recordset->mWriteIdx == recordset->mMaxSize, 0);

	return recordset;
}


bool
AosRecordset::swap(
		AosRundata *rdata,
		const AosDataRecordObjPtr &record)
{
	AosRecordContainer* rcd = dynamic_cast<AosRecordContainer *>(record.getPtr());
	aos_assert_r(rcd, false);
	
	mMaxSize = rcd->mNumRecord;
	mReadIdx = 0;
	mOffset = 0;;
	
	int64_t size = eDftRecordsetSize;
	if (mMaxSize > eDftRecordsetSize)
		size = mMaxSize;
	mEntryBuff = OmnNew AosBuff(size* sizeof(Entry) AosMemoryCheckerArgs);
	mEntryBuffRaw = mEntryBuff.getPtr();
	mEntries = (Entry *)mEntryBuffRaw->data();
	
	setData(rdata, rcd->mBuff);
	rcd->mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	rcd->mBuffRaw = rcd->mBuff.getPtr();	
	holdRecord(rdata, rcd->mRecord);

	int64_t offset = 0;
	for(u32 i=0; i<rcd->mRecordLens.size(); i++)
	{
//OmnScreen << "############### Recordset swap " << this << " offset : " << offset << " rcdlen : " << rcd->mRecordLens[i] << endl;
		appendRecord(rdata, 0, offset, rcd->mRecordLens[i], 0);
		offset += rcd->mRecordLens[i];
	}
	//aos_assert_r(mWriteIdx == mMaxSize, 0);
	return true;
}


AosRecordsetObjPtr 
AosRecordset::createRecordset(
		AosRundata *rdata,
		const AosXmlTagPtr &def)
{
	OmnTagFuncInfo << endl;
//OmnScreen << " ======================================== " << endl;
//OmnScreen << " @@@@@@@@@@@############# createRecordset " << endl;
//OmnScreen << " ======================================== " << endl;
	
	AosRecordset * recordset = OmnNew AosRecordset();
	bool rslt = recordset->config(def, rdata);
	aos_assert_r(rslt, 0);
	return recordset;
}

char*
AosRecordset::getData()
{
	//OmnScreen << " #### recordset getData " << (void*)this << "," << (void*)mData << "," << mDataBuff->data() << endl;
	return mData;
}

bool
AosRecordset::setData(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, false);
	mDataBuff = buff;
	mDataBuffRaw = mDataBuff.getPtrNoLock();
	mData = mDataBuffRaw->data();
	mDataLen = mDataBuffRaw->dataLen();
	aos_assert_r(mData == mDataBuffRaw->data(), false);

//OmnScreen << " #### recordset setData " << (void*)this << "," << (void*)mData << "," << (void*)mDataBuff->data() << endl;
	return true;
}


AosDataRecordObjPtr
AosRecordset::getRecord()
{
	if (mRecords.empty()) return 0;
	return mRecords[0];
}

AosDataRecordObj *
AosRecordset::getRawRecord(const int idx)
{
	aos_assert_r(idx < (int)mRawRecords.size(), 0);
	return mRawRecords[idx];
}

bool
AosRecordset::holdRecord(
		AosRundata *rdata, 
		const AosDataRecordObjPtr &record)
{
	AosDataRecordObjPtr new_record = record->clone(rdata AosMemoryCheckerArgs);	
	//aos_assert_r(new_record->getRecordLen() > 0, false);
	aos_assert_r(new_record, false);
	mRecords.push_back(new_record);
	mRawRecords.push_back(new_record.getPtr());
//OmnScreen << "########## holdRecord Recordset : " << this << " new_reocrd : " << new_record.getPtr() << " old record : " << record.getPtr() << " ########" << endl; 
	return true;
}

	
bool
AosRecordset::holdRecords(
		AosRundata *rdata, 
		const vector<AosDataRecordObjPtr> &records)
{
	AosDataRecordObjPtr new_record;
	for(size_t i=0; i<records.size(); i++)
	{
		new_record = records[i]->clone(rdata AosMemoryCheckerArgs);
		aos_assert_r(new_record, false);
		mRecords.push_back(new_record);
		mRawRecords.push_back(new_record.getPtr());
	}
	return true;
}


void
AosRecordset::setMetaData(
		AosRundata *rdata,  
		const AosMetaDataPtr &metadata)
{
	mMetaData = metadata;
	mMetaDataRaw = metadata.getPtr();
}


bool
AosRecordset::nextRecord(
		AosRundata* rdata, 
		AosDataRecordObj * &record)
{
	record = 0;
	if (!mData || !mEntries) return true;

	do {
		if (mReadIdx >= mWriteIdx)
		{
			return true;
		}

		int index = mEntries[mReadIdx].rcd_idx;
		aos_assert_r(index < (int)mRawRecords.size(), false);

		record = mRawRecords[index];
		aos_assert_rr(record, rdata, false);
		record->clear();
		if (mData != mDataBuffRaw->data())
		{
			mData = mDataBuffRaw->data();
		}

	//print by levi
	//OmnScreen << (void*)this << "," << (void*)mData << "," << (void*)mDataBuff->data() << endl;
	//OmnScreen << " ######### Recordset nextRecord : " << record << " mReadIdx : " << mReadIdx << " offset : " << mEntries[mReadIdx].offset << " len : " << mEntries[mReadIdx].len << endl;

		int64_t len = mEntries[mReadIdx].len;
		//if (mReadIdx != 0)
		//{
		if (mMetaDataRaw)
		{
			mMetaDataRaw->moveToNext();
			mMetaDataRaw->setRecordLength(len);
		}
		//}
		
		int status;	
		i64 offset = mEntries[mReadIdx].offset;
		bool rslt = record->setData(&mData[offset],
				mEntries[mReadIdx].len, mMetaDataRaw, status);//mOffset + offset);

		if (!rslt) return false;

		if (!record->isValid())
		{
			mReadIdx++;
			record = 0;
		}
	}
	while(!record);

	mReadIdx++;
	return true;
}


bool
AosRecordset::appendRecord(
		AosRundata *rdata, 
		const int index,
		const int64_t offset, 
		const int len,
		const AosMetaDataPtr &metaData)
{
	OmnTagFuncInfo << endl;
	//aos_assert_rr(mWriteIdx < mMaxSize, rdata, false);
	aos_assert_r(index < (int)mRawRecords.size(), false);
	//mEntries[mWriteIdx].record = record;
	
	// Ken Lee, 2015/04/22
	if ((i64)(mEntryBuffRaw->dataLen() + sizeof(Entry)) > mEntryBuffRaw->buffLen())
	{
		int64_t new_buff_len = (mEntryBuffRaw->buffLen()) * 2;
		aos_assert_r(new_buff_len <= INT_MAX, false);
		AosBuff *new_buff = OmnNew AosBuff(new_buff_len AosMemoryCheckerArgs);   
		char * data = mEntryBuffRaw->data();
		new_buff->setBuff(data, mEntryBuffRaw->dataLen());
		
		mEntryBuff = new_buff;
		mEntryBuffRaw = new_buff;
		mEntries = (Entry *)mEntryBuffRaw->data();
	}

	mEntries[mWriteIdx].rcd_idx = index;
	mEntries[mWriteIdx].offset = offset;
	mEntries[mWriteIdx].len = len;
	mWriteIdx++;

	// Ken Lee, 2015/04/22
	mEntryBuffRaw->setDataLen(mEntryBuffRaw->dataLen() + sizeof(Entry));
//print by levi
//OmnScreen << " #### recordset appendrecord " << (void*)this << "," << (void*)mData << "," << (void*)mDataBuff->data() << endl;

	return true;
}


//
//1. this method assume the record set has only a unique
//DataRecord, or data schema
//
//2. this method will not only append mEntry, but also
//append real data into the data buff
//
//3. This method assumes that the recordset has already
//held one data record, therefore no need to append new
//record in the same format
//
//4. This method assumes that the record is in type 
//   AosRecordContainer
//
bool
AosRecordset::appendRecord(
		AosRundata *rdata, 
		AosDataRecordObjPtr &rcd,
		AosBuffDataPtr &metaData)
{
	//aos_assert_rr(mWriteIdx < mMaxSize, rdata, false);
	char *data;
	int len;

	if (rcd->getType() == AosDataRecordType::eContainer)
	{
		AosRecordContainer* rcdContainer = dynamic_cast<AosRecordContainer *>(rcd.getPtr());
		len = rcdContainer->getInsideRecord()->getRecordLen();
		data = rcdContainer->getData(rdata);
	}
	else
	{
		len = rcd->getRecordLen();
		data = rcd->getData(rdata);
	}

	if (mWriteIdx == 0)
	{
		//keep the record, assume there is only one data schema
		//holdRecord(rdata, rcd);
		//aos_assert_r(mRecords.size() == 1, false);

		//allocate data buff
		AosBuffPtr buff = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
		setData(rdata, buff);
		
		//allocate entry buff
		mMaxSize = eDftRecordsetSize;
		mEntryBuff = OmnNew AosBuff(mMaxSize * sizeof(Entry) AosMemoryCheckerArgs);
		mEntryBuffRaw = mEntryBuff.getPtr();
		mEntries = (Entry *)mEntryBuffRaw->data();

		//set entry value for the the first record
		mEntries[mWriteIdx].rcd_idx = 0;
		mEntries[mWriteIdx].offset = 0;
		mEntries[mWriteIdx].len = len;
	}
	else
	{
		int64_t idx = mWriteIdx - 1;
	
		// Ken Lee, 2015/04/22
		if ((i64)(mEntryBuff->dataLen() + sizeof(Entry)) > mEntryBuff->buffLen())
		{
			int64_t new_buff_len = (mEntryBuff->buffLen()) * 2;
			aos_assert_r(new_buff_len <= INT_MAX, false);
			AosBuffPtr new_buff = OmnNew AosBuff(new_buff_len AosMemoryCheckerArgs);   
			char * data = mEntryBuff->data();
			new_buff->setBuff(data, mEntryBuff->dataLen());

			mEntryBuff = new_buff;
			mEntries = (Entry *)mEntryBuff->data();
		}

		//set current entry value
		mEntries[mWriteIdx].rcd_idx = 0;
		mEntries[mWriteIdx].offset = mEntries[idx].offset + mEntries[idx].len;
		mEntries[mWriteIdx].len = len;
	}

	//adjust entry buff's datalen
	mEntryBuffRaw->setDataLen(mEntryBuffRaw->dataLen() + sizeof(Entry));

	//common operations
	//if (metaData) mMetaDatas[mWriteIdx] = metaData;
	mWriteIdx++;

	//append data to databuff
	mDataBuffRaw->addBuff(data, len);

	return true;
}

//
// Phil 2015/04/01
bool
AosRecordset::sort(const AosRundataPtr &rdata, const AosCompareFunPtr &comp)
{
	AosDataRecordType::E type = mRecords[0]->getType();
	aos_assert_r(type, false);
	if (type == AosDataRecordType::eBuff)
	{
		//for buff
		AosBuffArrayVarPtr arrayVar = AosBuffArrayVar::create(comp, false, rdata);
		aos_assert_r(arrayVar, false);
		
		//const AosBuffPtr buff = mDataBuff;
		//arrayVar->setBodyBuff(buff, size, 0);
		AosDataRecordObj* record;
		AosRundata *rundata = rdata.getPtr();
		for(int i=0; i<mWriteIdx; i++)
		{
			nextRecord(rundata, record);
			aos_assert_r(record, false);
			arrayVar->appendEntry(record, rundata);
		}
		
		//this sort will change data in recordset's data buff
		bool rslt = arrayVar->sort();
		aos_assert_r(rslt, false);

		//get sorted bodybuff
		AosBuffPtr bodyBuff;
		set<i64> bodyAddr;
		AosBuffPtr headBuff = arrayVar->getHeadBuff();
		arrayVar->procHeaderBuff(comp, headBuff, bodyBuff, bodyAddr, true);

		const AosBuffPtr cBodyBuff = bodyBuff;
		char *data = bodyBuff->data();
		int64_t data_len = bodyBuff->dataLen();
		this->reset();
		this->setData(rundata, cBodyBuff);
		AosRecordsetObj *recordset_raw = this;

		//aos_assert_r(num_rcds_to_add > 0, false);
		int64_t offset = 0;

		aos_assert_r((int64_t)(offset+sizeof(int)) < data_len, false);
		//while (num_rcds_to_add && offset < data_len)
		while (offset < data_len)
		{
			bool contents_incomplete = false;
			if (int64_t(offset + sizeof(int)) > data_len)
			{
				contents_incomplete = true;
				break;
			}
			int record_len = *((int*)(data+offset));
			bool rslt = AosBuff::decodeRecordBuffLength(record_len);
			aos_assert_r(rslt, false);
			if (int64_t(offset + record_len + sizeof(int)) > data_len)
			{
				contents_incomplete = true;
				break;
			}
			const AosMetaDataPtr metadata = NULL;
			recordset_raw->appendRecord(rundata, 0, offset+sizeof(int), record_len, metadata);
			offset += record_len + sizeof(int);
		}

		//get the merge record number
		mWriteIdx = arrayVar->getNumEntries();

	}
	else
	{
		//for fixbin
		AosBuffArrayPtr buffArray = OmnNew AosBuffArray(mDataBuff, comp.getPtr(), false);

		//this sort will change data in recordset's data buff
		buffArray->sort(AosDataColOpr::eNormal);
		//get the merge record number
		mWriteIdx = buffArray->getNumEntries();
	}

	return true;
}

bool
AosRecordset::reset()
{
	mNumEntries = 0;
	mReadIdx = 0;
	mWriteIdx = 0;
	mDataLen = 0;
	mOffset = 0;
	mEntryBuffRaw->reset();
	mEntryBuffRaw->setDataLen(0);
//	mEntries = (Entry *)mEntryBuff->data();
	mData = 0;
	mNumEntries = 0;
	mDataBuff = 0;
	mDataBuffRaw = NULL;
//print by levi
//OmnScreen << " #### recordset reset " << (void*)this << "," << (void*)mData << "," << &mDataBuff << endl;
	return true;
}


//int64_t
//AosRecordset::getNumRecordsToAdd()
//{
//	// The recordset expects 'mMaxSize' records, and it has
//	// already had 'mNumEntries'. 
//	return mMaxSize - mWriteIdx;
//}


//
//Phil, 2015/04/03
//Save record information to a buffer to be re-constructed
//later on
//
bool 
AosRecordset::serializeTo(AosRundata *rdata,
		                  AosBuff *buff)
{
	aos_assert_r(rdata, false);

	//save members
	buff->setI64(mMaxSize);
	buff->setI64(mNumEntries);
	buff->setI64(mReadIdx);
	buff->setI64(mWriteIdx);
	buff->setI64(mDataLen);
	buff->setI64(mOffset);
	
	//save record xml 
	buff->setU32(mRawRecords.size());
	AosXmlTagPtr doc; 
	for (u32 i = 0; i < mRawRecords.size(); i++)
	{
		doc = (mRawRecords[i])->getRecordDoc();
		buff->setOmnStr(doc->toString());
	}
	
	//save databuff
	buff->setU32(mDataBuff->dataLen());
	buff->setBuff(mDataBuff);    
	
	//save entry buff
	buff->setU32(mEntryBuffRaw->dataLen());
	buff->setBuff(mEntryBuffRaw);    

	return true;
}

//
//reconstruct a record set from a buffer
//
bool 
AosRecordset::serializeFrom(AosRundata *rdata,
		                    AosBuff *buff)
{
	aos_assert_r(rdata, false);

	//get members
	i64 dft = 0;
	mMaxSize = buff->getI64(dft);
	mNumEntries = buff->getI64(dft);
	mReadIdx = buff->getI64(dft);
	mWriteIdx = buff->getI64(dft);
	mDataLen = buff->getI64(dft);
	mOffset = buff->getI64(dft);
	
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

		rcd = AosDataRecordObj::
			createDataRecordStatic(doc, 0, rdata AosMemoryCheckerArgs);
		aos_assert_r(rcd, false);

		holdRecord(rdata, rcd);
	}
	
	//construct databuff
	u32 len = buff->getU32(0);
	mDataBuff = buff->getBuff(len, true AosMemoryCheckerArgs); 
	aos_assert_r(mDataBuff, false);
	mDataBuffRaw = mDataBuff.getPtrNoLock();

	mData = mDataBuff->data();
	
	//construct entry buff
	len = buff->getU32(0);
	mEntryBuff = buff->getBuff(len, true AosMemoryCheckerArgs); 
	mEntryBuffRaw = mEntryBuff.getPtr();
	aos_assert_r(mEntryBuffRaw, false);

	mEntries = (Entry *)mEntryBuffRaw->data(); 

	return true;
}


//Jozhi for streaming
bool
AosRecordset::replaceRawRecord(AosDataRecordObj* record)
{
	aos_assert_r(record, false);
	record->clear();
	int idx = mReadIdx;
	if (idx != 0)
	{
		idx = mReadIdx - 1;
	}
	int index = mEntries[idx].rcd_idx;
	mRawRecords[index] = record;
	return true;
}
