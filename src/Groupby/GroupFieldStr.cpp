////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2012/12/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Groupby/GroupFieldStr.h"

#include "API/AosApi.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"


AosGroupFieldStr::AosGroupFieldStr()
:
AosGroupField(AosGroupField::eU64),
mHasValidValue(false)
{
}


AosGroupFieldStr::~AosGroupFieldStr()
{
}


bool
AosGroupFieldStr::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	bool rslt = AosGroupField::config(def, rdata);
	aos_assert_r(rslt, false);
	return true;
}

	
bool
AosGroupFieldStr::getStrPriv(
		OmnString &value,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	value = "";
	if (mFromDocidIdx >= 0)
	{
		value << record->getDocidByIdx(mFromDocidIdx);
		return true;
	}

	// This means that it needs to retrieve 'mFromFieldIdx' field.
	// Fields are separated by 'mSep'.
	const char *vv = 0;
	int vv_len = 0;
	bool rslt = AosGetField(vv, vv_len, record->getData(rdata.getPtr()), 
		record->getRecordLen(), mFromFieldIdx, mSep, 0);

	if (!rslt || !vv || vv_len <= 0) return false;
	
	if (mRawFormat == eRawFormat_Str)
	{
		value.assign((char *)vv, vv_len);
		return true;
	}

	// The data raw format is u64.
	value << *(u64*)vv;
	return true;
}


bool
AosGroupFieldStr::updateData(
		const AosDataRecordObjPtr &record, 
		bool &continue_update, 
		const AosRundataPtr &rdata)
{
	// It updates its value based on 'record'. Data fields are also used to collect
	// data during the time of processing records. When this function is called, 
	// the caller wants this field to collect the data based on the field definition.
	// The collected value is stored in this field.
	//
	// If this field wants to stop the further updating, it can set 'continue_update'
	// to false. Otherwise, it should set it to 'true'.
	continue_update = false;
	
	OmnString value;
	bool rslt = getStrPriv(value, record, rdata);
	aos_assert_r(rslt, false);

	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
		 // It collects the first value. 
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 mCrtValue = value;
			 mHasValidValue = true;
		 }
		 return true;

	case AosFieldOpr::eLast:
		 // It collects the last value
		 continue_update = true;
		 mCrtValue = value;
		 return true;

	case AosFieldOpr::eSum:
	case AosFieldOpr::eAverage:
		 OmnShouldNeverComeHere;
		 return false;

	case AosFieldOpr::eMinimum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 mCrtValue = value;
			 mHasValidValue = true;
			 return true;
		 }

		 // Note that this class assumes fixed length string.
		 if (mCrtValue > value)
		 {
			 continue_update = true;
			 mCrtValue = value;
		 }
		 return true;

	case AosFieldOpr::eMaximum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 mCrtValue = value;
			 mHasValidValue = true;
			 return true;
		 }

		 // Note that this class assumes fixed length string.
		 
		 if (mCrtValue < value)
		 {
			 continue_update = true;
			 mCrtValue = value;
		 }
		 return true;

	case AosFieldOpr::eCount:
		 continue_update = true;
		 mCount++;
		 return true;

	// Chen Ding, 06/28/2012
	case AosFieldOpr::eField:
		 // It retrieves a value from 'record'. 
		 mCrtValue = value;
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
AosGroupFieldStr::setRecordByPrevValue(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(record, false);
		
	if (mToFieldIdx >= 0)
	{
		AosValueRslt value;
		value.setStr(mPrevValue);
		bool outofmem = false;
		return record->setFieldValue(mToFieldIdx, value, outofmem, rdata.getPtr());
	}
	
	return true;
}


bool
AosGroupFieldStr::saveAndClear()
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
		 mHasValidValue = false;
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


AosGroupFieldPtr 
AosGroupFieldStr::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosGroupFieldStr * field = OmnNew AosGroupFieldStr();
	bool rslt = field->config(def, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

