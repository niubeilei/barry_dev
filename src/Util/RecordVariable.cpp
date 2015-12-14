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
// This class simulates the records of the following format:
// 		ddd$ddd$ddd
// where '$' is a substring that serves as the separator that separates
// fields in the record. The separator is configurable. 
//
// When data are set to this record, it separates the fields into mFields.
// There shall be an AosDataElem for each field. The AosDataElem is responsible
// for how to interpret the field.
//
// Modification History:
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataRecord directory
#include "Util/RecordVariable.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordVariable::AosRecordVariable(const bool flag)
:
AosDataRecord(AOSRECORDTYPE_VARIABLE, AosRecordType::eVariable, flag),
mRecordLen(0)
{
}


AosRecordVariable::AosRecordVariable(const AosXmlTagPtr &def, AosRundata *rdata)
:
AosDataRecord(AOSRECORDTYPE_VARIABLE, AosRecordType::eVariable, false),
mRecordLen(0)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosRecordVariable::~AosRecordVariable()
{
}


bool
AosRecordVariable::config(const AosXmlTagPtr &def, AosRundata *rdata)
{
	aos_assert_rr(def, rdata, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);
	
	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	if (mSep.length() <= 0)
	{
		AosSetErrorU(rdata, "missing_separator") << ": " << def->toString() << enderr;
		return false;
	}

	mIsCaseSensitive = def->getAttrBool(AOSTAG_CASE_SENSITIVE, true);
	return true;
}


AosDataRecordPtr 
AosRecordVariable::clone(const AosXmlTagPtr &def, AosRundata *rdata) const
{
	return OmnNew AosRecordVariable(def, rdata);
}


bool		
AosRecordVariable::setValue(
		const int idx, 
		const AosValueRslt &value, 
		AosRundata *rdata)
{
	switch (value.getType())
	{
	case AosDataType::eBool:
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


OmnString	
AosRecordVariable::getOmnStr(const int idx, const OmnString &dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];
	if (len <= 0) return dft;
	
	return mElems[idx]->getValueOmnStr(&mRawData[start_pos], len, dft);
}


const char *
AosRecordVariable::getCharStr(const int idx, int &len, const char *dft, const int dft_len)
{
	if (!mIsDataParsed) parseData();

	len = dft_len;
	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	len = mFieldLen[idx];
	return mElems[idx]->getValueCharStr(&mRawData[start_pos], len, len, dft, dft_len);
}


char		
AosRecordVariable::getChar(const int idx, const char dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];

	return mElems[idx]->getValueChar(&mRawData[start_pos], len, dft);
}


int			
AosRecordVariable::getInt(const int idx, const int dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];

	return mElems[idx]->getValueInt(&mRawData[start_pos], len, dft);
}


u32			
AosRecordVariable::getU32(const int idx, const u32 dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];

	return mElems[idx]->getValueU32(&mRawData[start_pos], len, dft);
}


int64_t		
AosRecordVariable::getInt64(const int idx, const int64_t &dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];

	return mElems[idx]->getValueInt64(&mRawData[start_pos], len, dft);
}


u64			
AosRecordVariable::getU64(const int idx, const u64 &dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];

	return mElems[idx]->getValueU64(&mRawData[start_pos], len, dft);
}


double		
AosRecordVariable::getDouble(const int idx, const double &dft) 
{
	if (!mIsDataParsed) parseData();

	aos_assert_r(idx >= 0 && (u32)idx < mStartPos.size(), dft);
	aos_assert_r(mElems[idx], dft);
	aos_assert_r(mRawData, dft);

	int start_pos = mStartPos[idx];
	int len = mFieldLen[idx];

	return mElems[idx]->getValueDouble(&mRawData[start_pos], len, dft);
}


bool		
AosRecordVariable::setCharStr(const int idx, const char *data, const int len)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData.append(data, len);
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setOmnStr(const int idx, const OmnString &data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setChar(const int idx, const char data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setInt(const int idx, const int data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setU32(const int idx, const u32 data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setInt64(const int idx, const int64_t &data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setU64(const int idx, const u64 &data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}


bool		
AosRecordVariable::setDouble(const int idx, const double &data)
{
	if (mData.length() > 0 && mSep.length() > 0) mData << mSep;
	mData << data;
	mRawData = (char *)mData.data();
	mDataLen = mData.length();
	return true;
}

	
bool
AosRecordVariable::reset(AosRundata *rdata)
{
	mData.setLength(0);
	mDataLen = 0;
	mRawData = (char *)mData.data();
	mDocid = 0;
	memset(mDocids, 0, sizeof(mDocids));
	return true;
}

	
OmnString
AosRecordVariable::toString() const
{
	OmnString ss;
	ss << "Type: " << (int)mType
		<< ", Docid: " << mDocid
		<< ", Docids: " << mDocids[0]
		<< ":" << mDocids[1]
		<< ":" << mDocids[2]
		<< ":" << mDocids[3] 
		<< ", Separator: " << mSep
		<< ", mNumElems: " << mNumElems
		<< ", RecordLen: " << mRecordLen;
	return ss;
}


bool 		
AosRecordVariable::setData(char *data, const int len, const bool need_copy)
{
	mIsDataParsed = false;
	if (!data || len <= 0)
	{
		mData = "";
		mDataLen = 0;
		mRawData = (char *)mData.data();
		return true;
	}

	if (need_copy)
	{
		mData.assign(data, len);
		mRawData = (char *)mData.data();
	}
	else
	{
		mRawData = data;
	}
	mDataLen = len;
	
	return true;
}


bool
AosRecordVariable::parseData()
{
	aos_assert_r(!mIsDataParsed, false);
	mIsDataParsed = true;
	mStartPos.clear();
	mFieldLen.clear();

	if (!mRawData || mDataLen <= 0) return true;

	return AosSeparateFields(mRawData, mDataLen, mSep.data(), mSep.length(),
			mIsCaseSensitive, mStartPos, mFieldLen);
}	
#endif
