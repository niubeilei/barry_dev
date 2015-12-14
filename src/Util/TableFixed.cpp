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
#include "Util/TableFixed.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataRecord/DataRecord.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/RecordFixBin.h"
#include "Util/Buff.h"


AosTableFixed::AosTableFixed(const bool flag)
:
AosDataTable(AOSTABLETYPE_FIXED, AosTableType::eFixed, flag),
mRecordLength(-1),
mData(0),
mDataLen(-1),
mMemCap(0),
mCursor(0)
{
}


/*
AosTableFixed::AosTableFixed(
		const int fieldlen1, 
		const int fieldlen2, 
		const int start_idx, 
		const int psize, 
		AosRundata *rdata)
:
AosDataTable(AOSTABLETYPE_FIXED, AosTableType::eFixed, false),
mRecordLength(fieldlen1+fieldlen2),
mData(0),
mDataLen(-1),
mMemCap(0),
mCursor(0)
{
	mRecord = OmnNew AosRecordFixBin(fieldlen2, fieldlen2, rdata);
	mDataLen = fieldlen1 + fieldlen2;
}
*/


AosTableFixed::AosTableFixed(const AosXmlTagPtr &def, AosRundata *rdata)
:
AosDataTable(AOSTABLETYPE_FIXED, AosTableType::eFixed, false),
mRecordLength(-1),
mData(0),
mDataLen(-1),
mMemCap(0),
mCursor(0)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosTableFixed::~AosTableFixed()
{
}


bool
AosTableFixed::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	aos_assert_rr(def, rdata, false);
	mRecordLength = def->getAttrInt(AOSTAG_RECORD_LENGTH, -1);
	if (mRecordLength <= 0)
	{
		AosSetErrorU(rdata, "invalid_record_length: ") << def->toString() << enderr;
		return false;
	}
	
	AosXmlTagPtr record = def->getFirstChild(AOSTAG_RECORD);
	if(!record)
	{
		AosSetErrorU(rdata, "invalid_record_length: ") << def->toString() << enderr;
		return false;
	}

	mRecord = AosDataRecordObj::createDataRecordStatic(record, 0, rdata AosMemoryCheckerArgs);
	if (!mRecord)
	{
	 	AosSetErrorU(rdata, "missing_record_def: ") << def->toString() << enderr;
	 	return false;
	}

	return true;
}


int 
AosTableFixed::getNumRecords() const
{
	aos_assert_r(mDataLen >= 0, -1);
	aos_assert_r(mRecordLength > 0, -1);
	return mDataLen / mRecordLength;
}


bool
AosTableFixed::clear()
{
	OmnNotImplementedYet;
	return false;
}
	

bool
AosTableFixed::copyData(const char *data, const int len, AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


int
AosTableFixed::getFieldLen(const int idx, AosRundata *rdata) const
{
	OmnNotImplementedYet;
	return false;
}


char *
AosTableFixed::getRecord(const int record_idx, int &record_len) const
{
	aos_assert_r(mData, 0);
	aos_assert_r(mRecordLength > 0, 0);
	aos_assert_r(record_idx >= 0, 0);
	int pos = record_idx * mRecordLength;
	if (pos + mRecordLength >= mDataLen) return 0;
	record_len = mRecordLength;
	return &mData[pos];
}


AosDataTablePtr
AosTableFixed::clone(const AosXmlTagPtr &def, AosRundata *rdata) const
{
	return OmnNew AosTableFixed(def, rdata);
}


int64_t
AosTableFixed::resize(const int64_t &num_records, AosRundata *rdata)
{
	aos_assert_rr(mRecordLength > 0, rdata, -1);
	mData = 0;

	try
	{
		mDataLen = num_records * mRecordLength;
		expandMemoryPriv(mDataLen);
		return mDataLen;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_alloc_mem: ") << mDataLen << enderr;
		return -1;
	}

	return mDataLen;
}


int64_t
AosTableFixed::setSize(const int64_t &size, AosRundata *rdata)
{
	aos_assert_rr(mRecordLength > 0, rdata, -1);
	aos_assert_rr(size > 0, rdata, -1);

	try
	{
		mDataLen = size;
		expandMemoryPriv(mDataLen);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_alloc_mem: ") << mDataLen << enderr;
		return -1;
	}

	return mDataLen;
}


bool 		
AosTableFixed::appendRecord(const AosDataRecordObjPtr &record, AosRundata *rdata)
{
	aos_assert_rr(record, rdata, false);

	OmnString d(record->getData(rdata), mRecordLength);
	//OmnScreen << "append record, record:" << d << ",len:" << mRecordLength << endl;

	if (mCursor + mRecordLength >= mMemCap) 
	{
	 	bool rslt = expandMemoryPriv();
	 	if (!rslt)
		{
			AosSetErrorU(rdata, "failed_alloc_memory") << mCursor
				<< ":" << mRecordLength << enderr;
			return false;
		}

	}

	memcpy(&mData[mCursor], record->getData(rdata), mRecordLength);
	mCursor += mRecordLength;
	return true;
}


bool
AosTableFixed::sort()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTableFixed::expandMemoryPriv()
{
	// This function expands the space. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = mMemCap + mMemCap;
	if (newsize == 0) newsize = 4000;
	AosBuffPtr buff = OmnNew AosBuff(newsize, 0 AosMemoryCheckerArgs);
	if (mData)
	{
		memcpy(buff->data(), mData, mMemCap);
	}

	mData = (char *)buff->data();
	mMemCap = newsize;
	mBuff = buff;
	return true;
}


bool
AosTableFixed::expandMemoryPriv(const u64 &size)
{
	// This function expands the space. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = size;
	if (newsize == 0) newsize = 4000;
	AosBuffPtr buff = OmnNew AosBuff(newsize, 0 AosMemoryCheckerArgs);
	if (mData)
	{
		memcpy(buff->data(), mData, mMemCap);
	}

	mData = (char *)buff->data();
	mMemCap = newsize;
	mBuff = buff;
	return true;
}


AosDataRecordObjPtr 
AosTableFixed::getRecord() const 
{ 
	return mRecord; 
}

