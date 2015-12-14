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
#include "DataFormat/DataElemStrRandom.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataFormat/DataFormatter.h"
#include "IILAssembler/IILAssembler.h"
#include "Rundata/Rundata.h"
#include "Random/RandomTypes.h"
#include "Util/DataRecord.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosDataElemStrRandom::AosDataElemStrRandom(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosDataElem(AosDataElemType::eStrRandom),
mCrtCount(0),
mPrevCount(0)
{
	bool rslt = config(def, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}


AosDataElemStrRandom::~AosDataElemStrRandom()
{
}


bool
AosDataElemStrRandom::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
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
	//  </dataelem>
	aos_assert_rr(def, rdata, false);
	
	bool rslt = AosDataElem::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	if (mIsConst) 
	{
		// mValue = def->getAttrStr(AOSTAG_VALUE, "");
		mValue = def->getNodeText();
		mDataLen = mValue.length();
	}
	else
	{
		mDataLen = def->getAttrInt(AOSTAG_LENGTH, -1);
	}
	aos_assert_r(mDataLen >= 0, false);
	
	mStrType = AosRandType::toEnum(def->getAttrStr(AOSTAG_STRTYPE, "normal"));
	aos_assert_r(AosRandType::isValid(mStrType), false);
	
	mMinLen = def->getAttrInt(AOSTAG_MINLEN, mDataLen);
	aos_assert_r(mMinLen >=0 && mMinLen <= mDataLen, false);

	mPadding = def->getAttrChar(AOSTAG_PADDING, ' ');
	mLeftAlgin = def->getAttrBool(AOSTAG_LEFTALGIN, false);

	if (mStrType == AosRandType::eCellPhoneNumber)
	{
		mPhoneNumLen = def->getAttrInt(AOSTAG_PHONENUMLEN, mDataLen);
		mWithCountryCode = def->getAttrBool(AOSTAG_WITHCOUNTRYCODE, true);
	}
	else if (mStrType == AosRandType::eDate)
	{
		//for date
		mMinYear =  def->getAttrInt(AOSTAG_MINYEAR, 2012);
		mMaxYear =  def->getAttrInt(AOSTAG_MAXYEAR, 2012);
	}
	else if(mStrType == AosRandType::eDigitString)
	{
		mIsUnique = def->getAttrBool(AOSTAG_UNIQUE, false);
	}
	return true;
}


bool
AosDataElemStrRandom::serializeFrom(
		OmnString &docstr,
		int &idx, 
		const char *data, 
		const int datalen, 
		const AosRundataPtr &rdata)
{
	// U64 is stored in binary, use 8 bytes.
	aos_assert_rr(data, rdata, false);
	aos_assert_rr(mDataLen >= 0, rdata, false);
	aos_assert_rr(idx >= 0 && datalen > 0 && (u32)idx + (u32)mDataLen <= (u32)datalen, rdata, false);
	aos_assert_rr(mName != "", rdata, false);

	OmnString vv(&data[idx], mDataLen);
	idx += mDataLen;
		
	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataElemStrRandom::serializeTo(
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
AosDataElemStrRandom::convertData(
		const char *record, 
		const int data_len, 
		int &idx,
		const bool need_convert,
		const AosBuffPtr &buff, 
		const bool need_value,
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	/*
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
		data = getCharStr(record, idx, len);
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
			value.setValue(vv);
		}
	}

	if (!mIsConst)
	{
		idx += mDataLen;
	}

	return AosDataProcStatus::eContinue;
	*/
	OmnNotImplementedYet;
	return AosDataProcStatus::eContinue;
}


bool
AosDataElemStrRandom::createRandomValue(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	if(mIsConst)
	{
		 buff->setBuff(mValue.data(), mDataLen);
		 return true;
	}
	
	aos_assert_r(mDataLen >= 0, false);

	switch (mStrType)
	{
	case AosRandType::eCellPhoneNumber:
		 mValue = OmnRandom::cellPhoneNumber(mDataLen, mPhoneNumLen, mWithCountryCode, mLeftAlgin, mPadding);
		 break;
	
	case AosRandType::eDate:
		 mValue = OmnRandom::dateStr(mMinYear, mMaxYear);
		 break;

	case AosRandType::eDigitString:
		 mValue = OmnRandom::digitStr(mDataLen, mLeftAlgin, mPadding, mIsUnique);
		 break;

	case AosRandType::eNormal :
		 mValue = OmnRandom::letterDigitStr(mDataLen, mMinLen, mLeftAlgin, mPadding);
		 break;

	default:
		 break;
	}

	buff->setBuff(mValue.data(), mDataLen);
	return true;
}


/*
const char * 
AosDataElemStrRandom::getCharStr(const char *record, int &idx, int &len)
{
	len = mDataLen;
	if (mIsConst)
	{
		return mValue.data();
	}
		
	int start_pos = mIsFixedPosition ? mOffset : idx;
	aos_assert_r(start_pos >= 0, 0);

	idx = start_pos + mDataLen;
	return &record[start_pos];
}


u64 
AosDataElemStrRandom::getU64(const char *record, const u64 &dft) const
{
	if (mIsConst) 
	{
		char *ptr;
		return strtoull(mValue.data(), &ptr, 10);
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
AosDataElemStrRandom::saveAndClear()
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
		 mCrtValue = 0;
		 mCrtCount = 0;
		 if (!mIsConst) mHasValidValue = false;
		 return true;

	case AosFieldOpr::eSum:
	case AosFieldOpr::eAverage:
		 OmnShouldNeverComeHere;
		 return false;

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


bool
AosDataElemStrRandom::updateData(const AosDataRecordPtr &record, bool &continue_update)
{
	/*
	// It updates its value based on 'record'.
	continue_update = true;
	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
		 if (!mHasValidValue)
		 {
			 setCrtValue(record->getData());
			 mHasValidValue = true;
		 }
		 return true;

	case AosFieldOpr::eLast:
		 if (!mIsConst) setCrtValue(record->getData());
		 return true;

	case AosFieldOpr::eSum:
	case AosFieldOpr::eAverage:
		 OmnShouldNeverComeHere;
		 return false;

	case AosFieldOpr::eMinimum:
		 if (!mHasValidValue)
		 {
			 setCrtValue(record->getData());
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) return true;		// Should never happen
		 
		 // Note that this class assumes fixed length string.
		 if (strncmp(mCrtValue.data(), &record->getData()[mOffset], mDataLen) < 0)
		 {
			 mCrtValue.assign(&record->getData()[mOffset], mDataLen);
		 }
		 return true;

	case AosFieldOpr::eMaximum:
		 if (!mHasValidValue)
		 {
			 setCrtValue(record->getData());
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) return true;		// Should never happen
		 
		 // Note that this class assumes fixed length string.
		 if (strncmp(mCrtValue.data(), &record->getData()[mOffset], mDataLen) > 0)
		 {
			 mCrtValue.assign(&record->getData()[mOffset], mDataLen);
		 }
		 return true;

	case AosFieldOpr::eCount:
		 mCrtCount++;
		 return true;

	// Chen Ding, 06/28/2012
	case AosFieldOpr::eField:
		 // It retrieves a value from 'record'. 
		 setCrtValue(record->getData());
		 continue_update = true;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << mFieldOpr << enderr;
		 return false;
	}
	*/

	OmnShouldNeverComeHere;
	return false;
}


/*
bool
AosDataElemStrRandom::setRecord(const AosDataRecordPtr &record)
{
	return setValue(record->getData(), record->getRecordLen());
}


bool 	
AosDataElemStrRandom::setRecord(char *record, const int record_len, const char value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemStrRandom::setRecord(char *record, const int record_len, const u32 value)
{
	OmnNotImplementedYet;
	return false; 
}


bool 	
AosDataElemStrRandom::setRecord(char *record, const int record_len, const int value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemStrRandom::setRecord(char *record, const int record_len, const u64 &value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemStrRandom::setRecord(char *record, const int record_len, const int64_t &value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemStrRandom::setRecord(char *record, const int record_len, const double &value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemStrRandom::setRecord(
		char *record, 
	 	const int record_len, 
	 	const char *data, 
	 	const int data_len)
{
	OmnNotImplementedYet;
	return false;
}


char   	
AosDataElemStrRandom::getValueChar(const char *rcd, const int rlen, const char v)
{
	OmnNotImplementedYet;
	return ' ';
}


int		
AosDataElemStrRandom::getValueInt(const char *rcd, const int rlen, const int v)
{
	OmnNotImplementedYet;
	return 0;
}


u32		
AosDataElemStrRandom::getValueU32(const char *rcd, const int rlen, const u32 v)
{
	OmnNotImplementedYet;
	return 0;
}


int64_t	
AosDataElemStrRandom::getValueInt64(const char *rcd, const int rlen, const int64_t &v)
{
	OmnNotImplementedYet;
	return 0;
}


u64		
AosDataElemStrRandom::getValueU64(const char *rcd, const int rlen, const u64 &v)
{
	OmnNotImplementedYet;
	return 0;
}


double
AosDataElemStrRandom::getValueDouble(const char *rcd, const int rlen, const double &v)
{
	OmnNotImplementedYet;
	return 0;
}


OmnString 
AosDataElemStrRandom::getValueOmnStr(const char *rcd, const int rlen, const OmnString &)
{
	OmnNotImplementedYet;
	return "";
}


const char *
AosDataElemStrRandom::getValueCharStr(
		const char *rcd, 
		const int rcd_len, 
		int &len, 
		const char *dft,
		const int dft_len)
{
	OmnNotImplementedYet;
	return 0;
}
*/
#endif
