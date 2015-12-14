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
// This is a fixed length string data element.
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to DataField. Chen Ding, 07/08/2012
#include "DataFormat/DataElemStr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataFormat/DataFormatter.h"
#include "IILAssembler/IILAssembler.h"
#include "Rundata/Rundata.h"
#include "Random/RandomTypes.h"
#include "Util/Buff.h"
#include "Util/ValueRslt.h"
#include "Util/DataRecord.h"
#include "Util/UtUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosDataElemStr::AosDataElemStr(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosDataElem(AosDataElemType::eStr),
mCount(0)
{
	bool rslt = config(def, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}


AosDataElemStr::~AosDataElemStr()
{
}


bool
AosDataElemStr::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//  <dataelem
	//      AOSTAG_TYPE=AOSDATAELEMTYPE_U64
	//      AOSTAG_NAME="xxx"					The field's name
	//      AOSTAG_OFFSET="xxx"					The field's position
	//      AOSTAG_ISCONST="xxx"				Whether this is a constant value.
	//      AOSTAG_FIXED_POSITION="true|false"	Whether the position is fixed or not.
	//      AOSTAG_RAW_FORMAT="u64|str|..."		How original data is expressed
	//      AOSTAG_VALUE="xxx" 					Needed only when AOSTAG_ISCONST is true
	//      AOSTAG_LENGTH="xxx"> 				Data Length
	//      <AOSTAG_CONSTANT>xxx</AOSTAG_CONSTANT>
	//      <CONDITION .../>
	//  </dataelem>
	aos_assert_rr(def, rdata, false);
	
	bool rslt = AosDataElem::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	if (mIsConst) 
	{
		// Chen Ding, 06/01/2012
		AosXmlTagPtr tag = def->getFirstChild(AOSTAG_CONSTANT);
		if (tag)
		{
			mConstValue = tag->getNodeText();
			mConstValue.normalizeWhiteSpace(true, true);  // trim
			mDataLen = mConstValue.length();
		}
	}
	else
	{
		mDataLen = def->getAttrInt(AOSTAG_LENGTH, -1);
	}
	aos_assert_r(mDataLen >= 0, false);
	
	mLeftAlignment = def->getAttrBool(AOSTAG_LEFTALGIN, false);
	mPadding = def->getAttrChar(AOSTAG_PADDING, ' ');
	
	return true;
}


bool
AosDataElemStr::serializeFrom(
		OmnString &docstr,
		int &idx, 
		const char *data, 
		const int datalen, 
		const AosRundataPtr &rdata)
{
	// U64 is stored in binary, use 8 bytes.
	aos_assert_rr(mName != "", rdata, false);
	
	OmnString vv;
	if(mIsConst)
	{
		vv = mConstValue;
	}
	else
	{
		aos_assert_rr(data, rdata, false);
		aos_assert_rr(mDataLen >= 0, rdata, false);
		aos_assert_rr(idx >= 0 && datalen > 0 && (u32)idx + (u32)mDataLen <= (u32)datalen, rdata, false);
		vv.assign(&data[idx], mDataLen);
		vv.normalizeWhiteSpace(true, true);  // trim
		idx += mDataLen;
	}
		
	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataElemStr::serializeTo(
		const AosXmlTagPtr &doc, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// This function sets 'buff' from 'doc' based on its definition.
	// If 'mOffset' is set, it will set the contents to 'mOffset'
	// (eight bytes long). Otherwise, it appends the contents to
	// 'buff'.
	aos_assert_rr(mName != "", rdata, false);
	aos_assert_rr(mDataLen >= 0, rdata, false);
	
	AosXmlTagPtr sub = doc->getFirstChild(mName);
	aos_assert_r(sub, false);

	OmnString value = sub->getNodeText();
	if (mOffset >= 0)
	{
		buff->setCrtIdx(mOffset);
	}
	buff->setBuff(value.data(), mDataLen);
	return true;
}


AosDataProcStatus::E
AosDataElemStr::convertData(
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
	// If the data is not big enough, it fails the operation. It returns eDataTooShort 
	aos_assert_rr(data_len >= 0, rdata, AosDataProcStatus::eError);
	aos_assert_rr(mDataLen >= 0, rdata, AosDataProcStatus::eError);
	
	int len;
	const char *data = 0;
	if (need_convert || need_value)
	{
		data = getCharStrPriv(record, idx, len);
	}

	if (!data)
	{
		// This means the record is too short. 
		return AosDataProcStatus::eDataTooShort;
	}
	
	// Chen Ding, 05/06/2012
	// Check whether it needs to filter the data. 
	if (mFilter)
	{
		if (!mFilter->evalCond(data, len, rdata))
		{
			// It is filtered out.
			return AosDataProcStatus::eRecordFiltered;
		}
	}

	if (data && len > 0)
	{
		if (need_convert && buff)
		{
			buff->setBuff(data, mDataLen);
		}

		if (need_value)
		{
			OmnString vv(data, len);
			vv.normalizeWhiteSpace(true, true);  // trim
			value.setValue(vv);
		}
	}

	return AosDataProcStatus::eContinue;
}


bool
AosDataElemStr::createRandomValue(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	if (mIsConst)
	{
		 buff->setBuff(mConstValue.data(), mDataLen);
		 return true;
	}
	
	aos_assert_r(mDataLen >= 0, false);

	char data[mDataLen + 1];
	AosRandomLetterStr(mDataLen, data);
	
	buff->setBuff(data, mDataLen);
	return true;
}


/*
const char * 
AosDataElemStr::getCharStr(const char *record, int &idx, int &len)
{
	return getCharStrPriv(record, idx, len);
}


u64 
AosDataElemStr::getU64(const char *record, const u64 &dft) const
{
	if (mIsConst) 
	{
		char *ptr;
		return strtoull(mConstValue.data(), &ptr, 10);
	}

	aos_assert_r(mOffset >= 0, dft);
	switch (mRawFormat)
	{
	case eRawFormat_U64:
	 	 return *(u64*)&record[mOffset];

	case eRawFormat_Str:
		 {
			 char *ptr;
	 		 return strtoull(&record[mOffset], &ptr, 10);
		 }

	default:
	 	 break;
	}
	
	OmnAlarm << "Unrecognized raw format: " << mRawFormat << enderr;
	return dft;
}
*/


bool
AosDataElemStr::updateData(const AosDataRecordPtr &record, bool &continue_update)
{
	// It updates its value based on 'record'.
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
		 if (!mIsConst) 
		 {
			 continue_update = true;
			 setCrtValue(record);
		 }
		 return true;

	case AosFieldOpr::eSum:
	case AosFieldOpr::eAverage:
		 OmnShouldNeverComeHere;
		 return false;

	case AosFieldOpr::eMinimum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 setCrtValue(record);
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) return true;		// Should never happen
		 
		 // Note that this class assumes fixed length string.
		 if (strncmp(mCrtValue.data(), &record->getData()[mOffset], mDataLen) < 0)
		 {
			 continue_update = true;
			 mCrtValue.assign(&record->getData()[mOffset], mDataLen);
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

		 if (mIsConst) return true;		// Should never happen
		 
		 // Note that this class assumes fixed length string.
		 if (strncmp(mCrtValue.data(), &record->getData()[mOffset], mDataLen) > 0)
		 {
			 continue_update = true;
			 mCrtValue.assign(&record->getData()[mOffset], mDataLen);
		 }
		 return true;

	case AosFieldOpr::eCount:
		 continue_update = true;
		 mCount++;
		 return true;

	// Chen Ding, 06/28/2012
	case AosFieldOpr::eField:
		 // It retrieves a value from 'record'. 
		 setCrtValue(record);
		 continue_update = true;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << mFieldOpr << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataElemStr::saveAndClear()
{
	// This function saves the current value to previous value, and then clear
	// the current values.
	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
	case AosFieldOpr::eLast:
	case AosFieldOpr::eMinimum:
	case AosFieldOpr::eMaximum:
		 mPrevValue = mCrtValue;
		 mCrtValue = "";
		 mCount = 0;
		 if (!mIsConst) mHasValidValue = false;
		 return true;

	case AosFieldOpr::eSum:
	case AosFieldOpr::eAverage:
		 OmnShouldNeverComeHere;
		 return false;

	case AosFieldOpr::eCount:
		 mPrevCount = mCount;
		 mCrtValue = "";
		 mCount = 0;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << mFieldOpr << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataElemStr::getValueFromRecord(
		const AosDataRecord *record,
		AosValueRslt &value, 
		const bool copy_flag,
		const AosRundataPtr &rdata)
{
	// This function retrieves the value from the record 'record'. 
	aos_assert_rr(record, rdata, false);
	if (mIsConst) 
	{
		return value.setCharStr1(mConstValue.data(), mConstValue.length(), copy_flag);
	}

	int record_len = record->getRecordLen();
	char *data = (char *)record->getData();
	if (!data || record_len <= 0)
	{
		return value.setCharStr1(0, 0, false);
	}

	aos_assert_rr(mOffset >= 0 && mOffset + mDataLen <= record_len, rdata, false);
	return value.setCharStr1(&data[mOffset], mDataLen, copy_flag);
}


bool
AosDataElemStr::setValueToRecord(
		const AosDataRecord *record,
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	// This function sets 'value' to the record 'record'.
	// The format in 'record' is a string at the position 'mOffset'
	// with the length 'mDataLen'. 
	aos_assert_rr(record, rdata, false);
	char *data = (char *)record->getData();
	int record_len = record->getRecordLen();
	aos_assert_rr(mOffset >= 0 && mOffset + mDataLen <= record_len, rdata, false);

	const char *vv = 0;
	int vv_len = 0;
	if (mIsConst)
	{
		vv = mConstValue.data();
		vv_len = mConstValue.length();
	}
	else
	{
		vv = ((AosValueRslt *)&value)->getCharStr(vv_len);
	}

	if (!vv || vv_len <= 0)
	{
		memset(&data[mOffset], mPadding, mDataLen);
		goto setTrailingNull;
	}

	if (vv_len > mDataLen)
	{
		switch (mDataTooLongPolicy)
		{
		case AosDataTooLongPolicy::eTreatAsError:
			 AosSetErrorU(rdata, "data_too_long") << enderr;
			 memset(&data[mOffset], mPadding, mDataLen);
			 goto setTrailingNull;

		case AosDataTooLongPolicy::eTrimLeft:
			 memcpy(&data[mOffset], &vv[vv_len - mDataLen], mDataLen);
			 goto setTrailingNull;

		case AosDataTooLongPolicy::eIgnoreSilently:
			 memset(&data[mOffset], mPadding, mDataLen);
			 goto setTrailingNull;
			 
		case AosDataTooLongPolicy::eTrimRight:
		default:
			 memcpy(&data[mOffset], vv, mDataLen);
			 goto setTrailingNull;
		}
	}
	else if (vv_len < mDataLen)
	{
		int delta = mDataLen - vv_len;
		if (mLeftAlignment)
		{
			memcpy(&data[mOffset], vv, vv_len);
			memset(&data[mOffset+vv_len], mPadding, delta);
			goto setTrailingNull;
		}
		else
		{
			memset(&data[mOffset], mLeadPadding, delta);
			memcpy(&data[mOffset+delta], vv, vv_len);
			goto setTrailingNull;
		}
	}
	else
	{
		// The same length as 'mDataLen'. 
		memcpy(&data[mOffset], vv, vv_len);
		goto setTrailingNull;
	}

setTrailingNull:
	if (mSetTrailingNull) data[mOffset + mDataLen] = 0;
	return true;
}


/*
bool
AosDataElemStr::setRecord(const AosDataRecordPtr &record)
{
	return setValue(record->getData(), record->getRecordLen());
}


bool 	
AosDataElemStr::setRecord(char *record, const int record_len, const char value)
{
	if (mIsConst) return true;
	aos_assert_r(mOffset >= 0 && mDataLen >= 0 && mOffset + mDataLen <= record_len, false);
	aos_assert_r(record, false);
	if (mDataLen == 0) return true;
	record[mOffset] = value;
	return true;
}


bool 	
AosDataElemStr::setRecord(char *record, const int record_len, const u32 value)
{
	if (mIsConst) return true;
	aos_assert_r(mOffset >= 0 && mDataLen >= 0 && mOffset + mDataLen <= record_len, false);
	aos_assert_r(record, false);
	if (mDataLen == 0) return true;

	OmnString ss;
	ss << value;
	return setRecord(record, record_len, ss.data(), ss.length());
}


bool 	
AosDataElemStr::setRecord(char *record, const int record_len, const int value)
{
	OmnString ss;
	ss << value;
	return setRecord(record, record_len, ss.data(), ss.length());
}


bool 	
AosDataElemStr::setRecord(char *record, const int record_len, const u64 &value)
{
	OmnString ss;
	ss << value;
	return setRecord(record, record_len, ss.data(), ss.length());
}


bool 	
AosDataElemStr::setRecord(char *record, const int record_len, const int64_t &value)
{
	OmnString ss;
	ss << value;
	return setRecord(record, record_len, ss.data(), ss.length());
}


bool 	
AosDataElemStr::setRecord(char *record, const int record_len, const double &value)
{
	OmnString ss;
	ss << value;
	return setRecord(record, record_len, ss.data(), ss.length());
}


bool 	
AosDataElemStr::setRecord(
	 	char *record, 
	 	const int record_len, 
	 	const char *data, 
	 	const int data_len)
{
	aos_assert_r(record, false);
	if (!data || data_len <= 0) return true;

	if (data_len >= mDataLen)
	{
		strncpy(&record[mOffset], data, mDataLen);
		return true;
	}

	if (mLeftAlignment)
	{
		strncpy(&record[mOffset], data, data_len);
		memset(&record[mOffset+data_len], mPadding, mDataLen - data_len);
		return true;
	}
	
	int delta = mDataLen - data_len;
	memset(&record[mOffset], mPadding, delta);
	strncpy(&record[mOffset + delta], data, data_len);
	return true;
}


char   	
AosDataElemStr::getValueChar(const char *record, const int rlen, const char dft)
{
	if (mIsConst)
	{
		if (mConstValue.length() <= 0) return dft;
		return mConstValue.data()[0];
	}

	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	return record[mOffset];
}


int		
AosDataElemStr::getValueInt(const char *rcd, const int rlen, const int dft)
{
	if (mIsConst)
	{
		if (mConstValue.length() <= 0) return dft;
		return atoi(mConstValue.data());
	}

	char *record = (char*)rcd;
	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	char c = record[mOffset + mDataLen];
	record[mOffset + mDataLen] = 0;	
	int vv = atoi(&record[mOffset]);
	record[mOffset + mDataLen] = c;	
	return vv;
}


u32		
AosDataElemStr::getValueU32(const char *rcd, const int rlen, const u32 dft)
{
	char *endptr;
	if (mIsConst)
	{
		if (mConstValue.length() <= 0) return dft;
		return strtoull(mConstValue.data(), &endptr, 10);
	}

	char *record = (char*)rcd;
	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	char c = record[mOffset + mDataLen];
	record[mOffset + mDataLen] = 0;	

	u32 vv = strtoull(&record[mOffset], &endptr, 10);
	record[mOffset + mDataLen] = c;	
	return vv;
}


int64_t	
AosDataElemStr::getValueInt64(const char *rcd, const int rlen, const int64_t &dft)
{
	char *endptr;
	if (mIsConst)
	{
		if (mConstValue.length() <= 0) return dft;
		return strtoll(mConstValue.data(), &endptr, 10);
	}

	char *record = (char*)rcd;
	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	char c = record[mOffset + mDataLen];
	record[mOffset + mDataLen] = 0;	

	int64_t vv = strtoll(&record[mOffset], &endptr, 10);
	record[mOffset + mDataLen] = c;	
	return vv;
}


u64		
AosDataElemStr::getValueU64(const char *rcd, const int rlen, const u64 &dft)
{
	char *endptr;
	if (mIsConst)
	{
		if (mConstValue.length() <= 0) return dft;
		return strtoull(mConstValue.data(), &endptr, 10);
	}

	char *record = (char*)rcd;
	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	char c = record[mOffset + mDataLen];
	record[mOffset + mDataLen] = 0;	

	u64 vv = strtoull(&record[mOffset], &endptr, 10);
	record[mOffset + mDataLen] = c;	
	return vv;
}


double 	
AosDataElemStr::getValueDouble(const char *rcd, const int rlen, const double &dft)
{
	char *endptr;
	if (mIsConst)
	{
		if (mConstValue.length() <= 0) return dft;
		return strtod(mConstValue.data(), &endptr);
	}

	char *record = (char*)rcd;
	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	char c = record[mOffset + mDataLen];
	record[mOffset + mDataLen] = 0;	

	u64 vv = strtod(&record[mOffset], &endptr);
	record[mOffset + mDataLen] = c;	
	return vv;
}


OmnString 
AosDataElemStr::getValueOmnStr(const char *rcd, const int rlen, const OmnString &dft)
{
	if (mIsConst) return mConstValue;
	char *record = (char*)rcd;
	aos_assert_r(record && rlen > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rlen, dft);
	OmnString ss(&record[mOffset], mDataLen);
	return ss;
}


const char *
AosDataElemStr::getValueCharStr(
		const char *rcd, 
		const int rcd_len, 
		int &len, 
		const char *dft, 
		const int dft_len)
{
	if (mIsConst) return mConstValue.data();
	char *record = (char*)rcd;
	len = dft_len;
	aos_assert_r(record && rcd_len > 0, dft);
	aos_assert_r(mOffset >= 0 && mOffset + mDataLen <= rcd_len, dft);
	len = mDataLen;
	return &record[mOffset];
}
*/

#endif
