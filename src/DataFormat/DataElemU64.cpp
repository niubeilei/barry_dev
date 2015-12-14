////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataField. Chen Ding, 07/08/2012
#include "DataFormat/DataElemU64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataFormat/DataFormatter.h"
#include "IILAssembler/IILAssembler.h"
#include "Rundata/Rundata.h"
#include "Util/DataRecord.h"
#include "Util/Buff.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosDataElemU64::AosDataElemU64(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosDataElem(AosDataElemType::eU64),
mConstValue(0),
mCrtValue(0),
mPrevValue(0),
mCrtCount(0),
mPrevCount(0),
mStrValueLen(-1),
mFromFieldIdx(-1),
mToFieldIdx(-1)
{
	bool rslt = config(def, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}


AosDataElemU64::~AosDataElemU64()
{
}


bool
AosDataElemU64::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(def, rdata, false);
	bool rslt = AosDataElem::config(def, rdata);
	
	if (mIsConst)
	{
		// Chen Ding, 06/01/2012
		AosXmlTagPtr tag = def->getFirstChild(AOSTAG_CONSTANT);
		if (tag)
		{
			mConstValueStr = tag->getNodeText();
			mConstValue = mConstValueStr.toU64();
		}
	}
	mDataLen = sizeof(u64);
	aos_assert_rr(rslt, rdata, false);

	mFromFieldIdx = def->getAttrInt(AOSTAG_FROM_FIELD_IDX, -1);
	mToFieldIdx = def->getAttrInt(AOSTAG_TO_FIELD_IDX, -1);
	if (mFromFieldIdx >= 0)
	{
		AosXmlTagPtr tag = def->getFirstChild(AOSTAG_SEPARATOR);
		if (!tag)
		{
			AosSetErrorU(rdata, "missing_separator");
			AosLogError(rdata);
			return false;
		}
		mSeparator = tag->getNodeText();
	}

	// Chen Ding, 06/24/2012
	// if (mRawFormat == eRawFormat_Str)
	if (!mIsConst && mRawFormat == eRawFormat_Str)
	{
		mStrValueLen = def->getAttrInt(AOSTAG_STRVALUE_LEN, -1);

		if (mFromFieldIdx < 0)
		{
			if (mStrValueLen <= 0)
			{
				AosSetErrorU(rdata, "invalid_confi") << ": " << def->toString();
				AosLogError(rdata);
				return false;
			}
		}
	}

	return true;
}


bool
AosDataElemU64::serializeFrom(
		OmnString &docstr,
		int &idx, 
		const char *data, 
		const int datalen, 
		const AosRundataPtr &rdata)
{
	// U64 is stored in binary, use 8 bytes.
	aos_assert_rr(mName != "", rdata, false);
	
	u64 vv = 0;
	if(mIsConst)
	{
		vv = mConstValue; 
	}
	else
	{
		aos_assert_rr(data, rdata, false);
		aos_assert_rr(idx >= 0 && datalen > 0
			&& (u32)idx + (u32)mDataLen <= (u32)datalen, rdata, false);

		vv = *(u64*)&data[idx];
		idx += mDataLen;
	}
	
	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataElemU64::serializeTo(
		const AosXmlTagPtr &doc, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// This function sets 'buff' from 'doc' based on its definition.
	// If 'mOffset' is set, it will set the contents to 'mOffset'
	// (eight bytes long). Otherwise, it appends the contents to
	// 'buff'.
	AosXmlTagPtr sub = doc->getFirstChild(mName);
	aos_assert_r(sub, false);

	OmnString value = sub->getNodeText();
	u64 vv = atoll(value.data());
	if (mOffset >= 0)
	{
		buff->setCrtIdx(mOffset);
	}
	buff->setU64(vv);
	return true;
}


AosDataProcStatus::E
AosDataElemU64::convertData(
		const char *record, 
		const int data_len, 
		int &idx,
		const bool need_convert,
		const AosBuffPtr &buff, 
		const bool need_value,
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	// This function does the following:
	// 1. Convert data from 'record' to 'buff' as needed. 
	// 2. Create IIL entries.
	// If the data is not big enough, it fails the operation. 'data_too_short' is set
	// to true, and 'false' is returned.
	aos_assert_rr(data_len >= 0, rdata, AosDataProcStatus::eError);
	
	u64 vv = 0;
	if (need_convert || need_value)
	{
		vv = getU64Priv(record, 0);
	}

	if (need_convert && buff)
	{
		buff->setU64(vv);
	}

	if (need_value)
	{
		value.setValue(vv);
	}

	if (!mIsConst)
	{
		idx += sizeof(u64);
	}

	return AosDataProcStatus::eContinue;
}


bool
AosDataElemU64::createRandomValue(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	if(mIsConst)
	{
		buff->setU64(mConstValue);
	}
	else
	{
		u64 vv = rand() % 0xffffffff;
		buff->setU64(vv);
	}
	return true;
}


/*
const char *
AosDataElemU64::getCharStr(const char *record, int &idx, int &len)
{
	aos_assert_r(!mIsConst, 0);
	aos_assert_r(mOffset >= 0, 0);
	len = mDataLen; 
	return &record[mOffset];
}


u64 
AosDataElemU64::getU64(const char *record, const u64 &dft) const
{
	return getU64Priv(record, dft);
}
*/


bool
AosDataElemU64::updateData(const AosDataRecordPtr &record, bool &continue_update)
{
	// It updates its value based on 'record'. If this field wants to stop
	// further updating, it sets 'continue_update' to false. Otherwise, 
	// it sets it to 'true'.
	continue_update = false;
	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 setCrtValue(record);
			 mHasValidValue = true;
		 }
		 return true;

	case AosFieldOpr::eLast:
		 continue_update = true;
		 if (!mIsConst) setCrtValue(record);
		 return true;

	case AosFieldOpr::eSum:
		 continue_update = true;
		 mCrtValue += getU64Priv(record->getData(), 0);
		 return true;

	case AosFieldOpr::eAverage:
		 continue_update = true;
		 mCrtValue += getU64Priv(record->getData(), 0);
		 mCrtCount++;
		 return true;

	case AosFieldOpr::eMinimum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 setCrtValue(record);
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) 
		 {
			 return true;		// Should never happen
		 }
		 else
		 {
			 u64 vv = getU64Priv(record, 0);
			 if (vv < mCrtValue)
			 {
				 continue_update = true;
				 mCrtValue = vv;
			 }
		 }
		 return true;

	case AosFieldOpr::eMaximum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 setCrtValue(record);
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) 
		 {
			 return true;		// Should never happen
		 }
		 else
		 {
			 u64 vv = getU64Priv(record, 0);
			 if (vv > mCrtValue)
			 {
				 continue_update = true;
				 mCrtValue = vv;
			 }
		 }
		 return true;

	case AosFieldOpr::eCount:
		 continue_update = true;
		 mCrtCount++;
		 return true;

	case AosFieldOpr::eField:
		 // It retrieves a value from 'record'. 
		 setCrtValue(record);
		 continue_update = true;
		 return true;

	case AosFieldOpr::eNone:		// Chen Ding, 06/28/2012
		 // It does nothing.
		 continue_update = true;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << mFieldOpr << enderr;
		 continue_update = true;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataElemU64::saveAndClear()
{
	// This function saves the current value to previous value, and then clear
	// the current values.
	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
	case AosFieldOpr::eLast:
	case AosFieldOpr::eSum:
	case AosFieldOpr::eMinimum:
	case AosFieldOpr::eMaximum:
	case AosFieldOpr::eField:
		 mPrevValue = mCrtValue;
		 mCrtValue = 0;
		 mCrtCount = 0;
		 if (!mIsConst) mHasValidValue = false;
		 return true;

	case AosFieldOpr::eAverage:
		 if (mCrtCount == 0)
		 {
			 mPrevValue = 0;
		 }
		 else
		 {
			 mPrevValue = mCrtValue / mCrtCount;
		 }

		 mCrtValue = 0;
		 mCrtCount = 0;
		 return true;

	case AosFieldOpr::eCount:
		 mPrevCount = mCrtCount;
		 mCrtValue = 0;
		 mCrtCount = 0;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << mFieldOpr << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


/*
bool
AosDataElemU64::setRecord(const AosDataRecordPtr &record)
{
	if (mSetRecordDocid)
	{
		// This means we need to set the 'mToDocidIdx'-th docid to the 
		// record's docid.
		record->setDocid(mPrevValue);
		return true;
	}

	if(mToFieldIdx >= 0)
	{
		return setValue(record->getData(), mPrevValue);
	}

	return true;
}


bool 	
AosDataElemU64::setRecord(char *record, const int record_len, const char value)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0, false);
	aos_assert_r(mOffset + sizeof(u64) <= (u32)record_len, false);
	*(u64*)&record[mOffset] = value;
	return true;
}


bool 	
AosDataElemU64::setRecord(char *record, const int record_len, const u32 value)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0 && mOffset + sizeof(u64) <= (u32)record_len, false);
	*(u64*)&record[mOffset] = value;
	return true;
}


bool 	
AosDataElemU64::setRecord(char *record, const int record_len, const int value)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0 && mOffset + sizeof(u64) <= (u32)record_len, false);
	*(u64*)&record[mOffset] = (u64)value;
	return true;
}


bool 	
AosDataElemU64::setRecord(char *record, const int record_len, const u64 &value)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0 && mOffset + sizeof(u64) <= (u32)record_len, false);
	*(u64*)&record[mOffset] = value;
	return true;
}


bool 	
AosDataElemU64::setRecord(char *record, const int record_len, const int64_t &value)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0 && mOffset + sizeof(u64) <= (u32)record_len, false);
	*(u64*)&record[mOffset] = (u64)value;
	return true;
}


bool 	
AosDataElemU64::setRecord(char *record, const int record_len, const double &value)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0 && mOffset + sizeof(u64) <= (u32)record_len, false);
	*(u64*)&record[mOffset] = (u64)value;
	return true;
}


bool 	
AosDataElemU64::setRecord(
	 	char *record, 
	 	const int record_len, 
	 	const char *data, 
	 	const int data_len)
{
	if (mIsConst) return true;
	aos_assert_r(record && record_len >= 0, false);
	aos_assert_r(mOffset >= 0 && mOffset + sizeof(u64) <= (u32)record_len, false);

	OmnString ss(data, data_len);
	*(u64*)&record[mOffset] = ss.parseU64(0);
	return true;	
}


char   	
AosDataElemU64::getValueChar(const char *record, const int record_len, const char dft)
{
	return (char)getU64Priv(record, dft);
}


int		
AosDataElemU64::getValueInt(const char *record, const int rlen, const int dft)
{
	return (int)getU64Priv(record, dft);
}


u32		
AosDataElemU64::getValueU32(const char *record, const int rlen, const u32 dft)
{
	return getU64Priv(record, dft);
}


int64_t	
AosDataElemU64::getValueInt64(const char *record, const int rlen, const int64_t &dft)
{
	return (int64_t)getU64Priv(record, dft);
}


u64		
AosDataElemU64::getValueU64(const char *record, const int rlen, const u64 &dft)
{
	return getU64Priv(record, dft);
}


double 	
AosDataElemU64::getValueDouble(const char *record, const int rlen, const double &dft)
{
	return (double)getU64Priv(record, dft);
}


OmnString 
AosDataElemU64::getValueOmnStr(const char *record, const int rlen, const OmnString &dft)
{
	OmnString ss;
	ss << getU64Priv(record, 0);
	return ss;
}


const char *
AosDataElemU64::getValueCharStr(
		const char *record, 
		const int rcd_len, 
		int &len, 
		const char *dft, 
		const int dft_len)
{
	if (mIsConst) return mConstValueStr.data();
	if (mRawFormat == eRawFormat_Str)
	{
		return &record[mOffset];
	}

	OmnShouldNeverComeHere;
	return 0;
}
*/
#endif
