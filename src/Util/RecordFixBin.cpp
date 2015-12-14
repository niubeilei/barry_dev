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
// This is binary fixed length record:
// 	String		fixed length
// 	char *		fixed length
// 	bool		one byte
// 	char		one byte
// 	int			sizeof(int)
// 	int64_t		sizeof(int64_t)
// 	u32			sizeof(u32)
// 	u64			sizeof(u64)
// 	double		sizeof(double)
//
// Modification History:
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataRecord directory
#include "Util/RecordFixBin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordFixBin::AosRecordFixBin(const bool flag)
:
AosDataRecord(AOSRECORDTYPE_FIXBIN, AosRecordType::eFixedBinary, flag),
mDataLen(-1),
mRecordLen(0)
{
}


AosRecordFixBin::AosRecordFixBin(const AosXmlTagPtr &def, AosRundata *rdata)
:
AosDataRecord(AOSRECORDTYPE_FIXBIN, AosRecordType::eFixedBinary, false),
mDataLen(-1),
mRecordLen(0)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRecordFixBin::~AosRecordFixBin()
{
}


bool
AosRecordFixBin::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	aos_assert_rr(def, rdata, false);
	if (!AosDataRecord::config(def, rdata)) return false;

	mRecordLen = 0;
	for (u32 i=0; i<mElems.size(); i++)
	{
		mRecordLen += mElems[i]->getDataLen();
	}

	mInternalData.setLength(mRecordLen);
	mData = (char *)mInternalData.data();
	mDataLen = mRecordLen;
	memset(mData, 0, mRecordLen + 1);
	return true;
}


OmnString	
AosRecordFixBin::getOmnStr(const int idx, const OmnString &dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueOmnStr(mData, mRecordLen, dft);
}


const char *
AosRecordFixBin::getCharStr(const int idx, int &len, const char *dft, const int dft_len)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueCharStr(mData, mRecordLen, len, dft, dft_len);
}


char		
AosRecordFixBin::getChar(const int idx, const char dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueChar(mData, mRecordLen, dft);
}


bool		
AosRecordFixBin::getBool(const int idx, const bool dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueChar(mData, mRecordLen, dft);
}


int			
AosRecordFixBin::getInt(const int idx, const int dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueInt(mData, mRecordLen, dft);
}


u32			
AosRecordFixBin::getU32(const int idx, const u32 dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueU32(mData, mRecordLen, dft);
}


int64_t		
AosRecordFixBin::getInt64(const int idx, const int64_t &dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueInt64(mData, mRecordLen, dft);
}


u64			
AosRecordFixBin::getU64(const int idx, const u64 &dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueU64(mData, mRecordLen, dft);
}


double		
AosRecordFixBin::getDouble(const int idx, const double &dft)
{
	aos_assert_r(mData, dft);
	aos_assert_r(idx >= 0 && idx < mNumElems, dft);
	return mElems[idx]->getValueDouble(mData, mRecordLen, dft);
}


OmnString	
AosRecordFixBin::toString() const
{
	OmnNotImplementedYet;
	return "";
}


AosDataRecordPtr 
AosRecordFixBin::clone(const AosXmlTagPtr &def, AosRundata *rdata) const
{
	return OmnNew AosRecordFixBin(def, rdata);
}


bool
AosRecordFixBin::setChar(const int idx, const char data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, data);
	return true;
}


bool		
AosRecordFixBin::setBool(const int idx, const bool data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, (char)data);
	return true;
}


bool
AosRecordFixBin::setInt(const int idx, const int data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, data);
	return true;
}


bool
AosRecordFixBin::setU32(const int idx, const u32 data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, data);
	return true;
}


bool
AosRecordFixBin::setInt64(const int idx, const int64_t &data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, data);
	return true;
}


bool
AosRecordFixBin::setU64(const int idx, const u64 &data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, data);
	return true;
}


bool
AosRecordFixBin::setDouble(const int idx, const double &data)
{
	aos_assert_r(mData, false);
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	mElems[idx]->setRecord(mData, mRecordLen, data);
	return true;
}


bool		
AosRecordFixBin::setValue(
		const int idx, 
		const AosValueRslt &value, 
		AosRundata *rdata)
{
	aos_assert_rr(idx >= 0 && idx < mNumElems, rdata, false);
	switch (value.getType())
	{
	case AosDataType::eBool:
		 return setBool(idx, value.getChar(rdata));

	case AosDataType::eChar:
		 return setChar(idx, value.getChar(rdata));

	case AosDataType::eU32:
		 return setU32(idx, value.getU32Value(rdata));

	case AosDataType::eU64:
		 return setU64(idx, value.getU64Value(rdata));

	case AosDataType::eInt32:
		 return setInt(idx, value.getIntValue(rdata));

	case AosDataType::eInt64:
		 return setInt64(idx, value.getInt64Value(rdata));

	case AosDataType::eDouble:
		 return setDouble(idx, value.getDoubleValue(rdata));

	case AosDataType::eString:
	case AosDataType::eCharStr:
		 {
			 int len;
			 const char *data = value.getCharStr(len);
			 if (data) return setCharStr(idx, data, len);
			 AosSetErrorU(rdata, "internal_error");
			 AosLogError(rdata);
			 return false;
		 }

	default:
		 break;
	}

	AosSetErrorU(rdata, "invalid_data_type") << ": " << value.getType();
	AosLogError(rdata);
	return false;
}


bool		
AosRecordFixBin::setOmnStr(const int idx, const OmnString &data)
{
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	return mElems[idx]->setRecord(mData, mRecordLen, data.data(), data.length());
}

bool		
AosRecordFixBin::setCharStr(const int idx, const char *data, const int len)
{
	aos_assert_r(idx >= 0 && idx < mNumElems, false);
	return mElems[idx]->setRecord(mData, mRecordLen, data, len);
}


bool
AosRecordFixBin::reset(AosRundata *rdata)
{
	aos_assert_rr(mData, rdata, false);
	memset(mData, 0, mRecordLen);
	mDocid = 0;
	memset(mDocids, 0, sizeof(mDocids));
	return true;
}


bool 		
AosRecordFixBin::setData(char *data, const int len, const bool need_copy)
{
	if (need_copy)
	{
		mInternalData.assign(data, len);
		mData = (char *)mInternalData.data();
		mDataLen = len;
		return true;
	}

	mData = data;
	mDataLen = len;
	return true;
}
#endif
