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
#include "DataFormat/DataElemTime.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataFormat/DataFormatter.h"
#include "DataRecord/DataRecord.h"
#include "IILAssembler/IILAssembler.h"
#include "Rundata/Rundata.h"
#include "Random/RandomTypes.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"
#include "UtilTime/TimeFormat.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosDataElemTime::AosDataElemTime(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosDataElem(AosDataElemType::eTime),
mCount(0)
{
	bool rslt = config(def, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}


AosDataElemTime::~AosDataElemTime()
{
}


bool
AosDataElemTime::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//  <dataelem
	//      AOSTAG_TYPE=AOSDATAELEMTYPE_U64
	//      AOSTAG_NAME="xxx"					The field's name
	//      AOSTAG_OFFSET="xxx"					The field's position
	//      AOSTAG_ISCONST="xxx"				Whether this is a constant value.
	//      AOSTAG_FIXED_POSITION="true|false"	Whether the position is fixed or not.
	//      AOSTAG_RAW_FORMAT="u64|str|..."		How original data is expressed
	//      AOSTAG_VALUE="xxx" 					Needed only when AOSTAG_ISCONST is true
	//      AOSTAG_LENGTH="xxx" 				Data Length
	//      AOSTAG_TIMEGRAN="yer|mnt|wek|day|hur|mut|sec"
	//      AOSTAG_TIMEFORMAT="%Y%m%d%H:%M:%S|..."
	//  </dataelem>
	aos_assert_rr(def, rdata, false);
	
	bool rslt = AosDataElem::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);
	
	if (mIsConst) 
	{
		// mConstValue = def->getAttrStr(AOSTAG_VALUE, "");
		mConstValue = def->getNodeText();
		mDataLen = mConstValue.length();
	}
	else
	{
		mDataLen = def->getAttrInt(AOSTAG_LENGTH, -1);
	}
	aos_assert_r(mDataLen >= 0, false);
	
	mTimeFormat = AosTimeFormat::toEnum(def->getAttrStr(AOSTAG_TIME_FORMAT, ""));
	aos_assert_rr(AosTimeFormat::isValid(mTimeFormat), rdata, false);

	return true;
}


bool
AosDataElemTime::serializeFrom(
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
AosDataElemTime::serializeTo(
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
AosDataElemTime::convertData(
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
	// Convert the raw data (which is a string implementation of 
	// time) into AosUnitime, which is a u64 data. 
	aos_assert_rr(data_len >= 0, rdata, AosDataProcStatus::eError);
	aos_assert_rr(mDataLen >= 0, rdata, AosDataProcStatus::eError);
	
	int len;
	const char *data = 0;
	if (need_convert || need_value)
	{
		// It retrieves the string from the raw record.
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

	// It is not filtered. Need to convert the string into unitime.
	OmnString timeStr(data, len);
	// mTimeGran = AosTimeGran::eSecondly;
	// AosTime::TimeFormat format = AosTime::convertTimeFormatWithDft("2fmt");
	//20120525172134
	// OmnString mTimeFormatStr = "%Y%m%d%H:%M:%S";
	// u64 unitime = AosTime::getUniTime(mTimeFormat, mTimeGran, timeStr, mTimeFormatStr);
	
	u64 unitime = AosTime::getUniTime(timeStr, mTimeFormat);
	if (data && len > 0)
	{
		if (need_convert && buff)
		{
			buff->setU64(unitime);
		}

		if (need_value)
		{
			value.setValue(unitime);
		}
	}

	return AosDataProcStatus::eContinue;
}


bool
AosDataElemTime::createRandomValue(
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
AosDataElemTime::getCharStr(const char *record, int &idx, int &len)
{
	return getCharStrPriv(record, idx, len);
}


u64 
AosDataElemTime::getU64(const char *record, const u64 &dft) const
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
AosDataElemTime::updateData(const AosDataRecordPtr &record, bool &continue_update)
{
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
		 mCount++;
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

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataElemTime::saveAndClear()
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


/*
bool
AosDataElemTime::setRecord(const AosDataRecordPtr &record)
{
	return setValue(record->getData(), record->getRecordLen());
}


bool 	
AosDataElemTime::setRecord(char *record, const int record_len, const char value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemTime::setRecord(char *record, const int record_len, const u32 value)
{
	OmnNotImplementedYet;
	return false; 
}


bool 	
AosDataElemTime::setRecord(char *record, const int record_len, const int value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemTime::setRecord(char *record, const int record_len, const u64 &value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemTime::setRecord(char *record, const int record_len, const int64_t &value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemTime::setRecord(char *record, const int record_len, const double &value)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDataElemTime::setRecord(
		char *record, 
	 	const int record_len, 
	 	const char *data, 
	 	const int data_len)
{
	OmnNotImplementedYet;
	return false;
}


char   	
AosDataElemTime::getValueChar(const char *rcd, const int rlen, const char v)
{
	OmnNotImplementedYet;
	return ' ';
}


int		
AosDataElemTime::getValueInt(const char *rcd, const int rlen, const int v)
{
	OmnNotImplementedYet;
	return 0;
}


u32		
AosDataElemTime::getValueU32(const char *rcd, const int rlen, const u32 v)
{
	OmnNotImplementedYet;
	return 0;
}


int64_t	
AosDataElemTime::getValueInt64(const char *rcd, const int rlen, const int64_t &v)
{
	OmnNotImplementedYet;
	return 0;
}


u64		
AosDataElemTime::getValueU64(const char *rcd, const int rlen, const u64 &v)
{
	OmnNotImplementedYet;
	return 0;
}


double 	
AosDataElemTime::getValueDouble(const char *rcd, const int rlen, const double &)
{
	OmnNotImplementedYet;
	return 0;
}


OmnString 
AosDataElemTime::getValueOmnStr(const char *rcd, const int rlen, const OmnString &)
{
	OmnNotImplementedYet;
	return "";
}


const char *
AosDataElemTime::getValueCharStr(
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
