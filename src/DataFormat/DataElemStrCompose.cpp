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
#include "DataFormat/DataElemStrCompose.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataFormat/DataFormatter.h"
#include "IILAssembler/IILAssembler.h"
#include "Rundata/Rundata.h"
#include "Random/RandomTypes.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosDataElemStrCompose::AosDataElemStrCompose(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
:
AosDataElem(AosDataElemType::eStrCompose),
mCount(0)
{
	bool rslt = config(def, rdata);
	if (!rslt) OmnThrowException(rdata->getErrmsg());
}


AosDataElemStrCompose::~AosDataElemStrCompose()
{
}


bool
AosDataElemStrCompose::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosDataElemStrCompose::serializeFrom(
		OmnString &docstr,
		int &idx, 
		const char *data, 
		const int datalen, 
		const AosRundataPtr &rdata)
{
	// aos_assert_rr(data, rdata, false);
	// aos_assert_rr(mDataLen >= 0, rdata, false);
	// aos_assert_rr(idx >= 0 && datalen > 0 && (u32)idx + (u32)mDataLen <= (u32)datalen, rdata, false);
	// aos_assert_rr(mName != "", rdata, false);
	// 
	// OmnString vv(&data[idx], mDataLen);
	// idx += mDataLen;
	//	
	// docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	// return true;
	OmnNotImplementedYet;
	return false;
}


bool
AosDataElemStrCompose::serializeTo(
		const AosXmlTagPtr &doc, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	/*
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
	*/
	OmnNotImplementedYet;
	return false;
}


AosDataProcStatus::E
AosDataElemStrCompose::convertData(
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
		if (need_convert)
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
AosDataElemStrCompose::createRandomValue(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	/*
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
	*/
	OmnNotImplementedYet;
	return false;
}


/*
const char * 
AosDataElemStrCompose::getCharStr(const char *record, int &idx, int &len)
{
	OmnNotImplementedYet;
	return 0;
}


u64 
AosDataElemStrCompose::getU64(const char *record, const u64 &dft) const
{
	// if (mIsConst) 
	// {
	// 	char *ptr;
	// 	return strtoull(mConstValue.data(), &ptr, 10);
	// }

	// aos_assert_r(mOffset >= 0, dft);
	// switch (mRawFormat)
	// {
	// case eRawFormat_U64:
	 // 	 return *(u64*)&record[mOffset];

	// case eRawFormat_Str:
	// 	 {
	// 		 char *ptr;
	 // 		 return strtoull(&record[mOffset], &ptr, 10);
	// 	 }

	// default:
	 // 	 break;
	// }
	
	// OmnAlarm << "Unrecognized raw format: " << mRawFormat << enderr;
	// return dft;
	OmnNotImplementedYet;
	return false;
}
*/


bool
AosDataElemStrCompose::updateData(const AosDataRecordPtr &record, bool &continue_update)
{
	/*
	// It updates its value based on 'record'.
	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
		 if (!mHasValidValue)
		 {
			 setCrtValue(record);
			 mHasValidValue = true;
		 }
		 return true;

	case AosFieldOpr::eLast:
		 if (!mIsConst) setCrtValue(record);
		 return true;

	case AosFieldOpr::eSum:
	case AosFieldOpr::eAverage:
		 OmnShouldNeverComeHere;
		 return false;

	case AosFieldOpr::eMinimum:
		 if (!mHasValidValue)
		 {
			 setCrtValue(record);
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) return true;		// Should never happen
		 
		 // Note that this class assumes fixed length string.
		 if (strncmp(mCrtValue.data(), &record[mOffset], mDataLen) < 0)
		 {
			 mCrtValue.assign(&record[mOffset], mDataLen);
		 }
		 return true;

	case AosFieldOpr::eMaximum:
		 if (!mHasValidValue)
		 {
			 setCrtValue(record);
			 mHasValidValue = true;
			 return true;
		 }

		 if (mIsConst) return true;		// Should never happen
		 
		 // Note that this class assumes fixed length string.
		 if (strncmp(mCrtValue.data(), &record[mOffset], mDataLen) > 0)
		 {
			 mCrtValue.assign(&record[mOffset], mDataLen);
		 }
		 return true;

	case AosFieldOpr::eCount:
		 mCount++;
		 return true;

	default:
		 OmnAlarm << "Unrecognized field type: " << mFieldOpr << enderr;
		 return false;
	}
	*/

	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataElemStrCompose::saveAndClear()
{
	/*
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
	*/

	OmnShouldNeverComeHere;
	return false;
}
#endif
