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
// 2012/12/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Groupby/GroupFieldU64.h"

#include "API/AosApi.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"

AosGroupFieldU64::AosGroupFieldU64()
:
AosGroupField(AosGroupField::eU64),
mHasValidValue(false),
mCrtValue(0),
mPrevValue(0)
{
}


AosGroupFieldU64::~AosGroupFieldU64()
{
}


bool
AosGroupFieldU64::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	bool rslt = AosGroupField::config(def, rdata);
	aos_assert_r(rslt, false);
	return true;
}
	

bool
AosGroupFieldU64::getU64Priv(
		u64 &value,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if (mFromDocidIdx >= 0)
	{
		value = record->getDocidByIdx(mFromDocidIdx);
		return true;
	}

	aos_assert_r(mFromFieldIdx >= 0, false);
			
	// This means that it needs to retrieve 'mFromFieldIdx' field.
	// Fields are separated by 'mSep'.
	const char *vv = 0;
	int vv_len = 0;
	bool rslt = AosGetField(vv, vv_len, record->getData(rdata.getPtr()), 
		record->getRecordLen(), mFromFieldIdx, mSep, 0);

	if (!rslt || !vv || vv_len <= 0) return false;
	
	if (mRawFormat == eRawFormat_Str)
	{
		OmnString v((char *)vv, vv_len);
		value = v.toU64();
		return true;
	}

	// The data raw format is u64.
	value = *(u64*)vv;
	return true;
}


bool
AosGroupFieldU64::updateData(
		const AosDataRecordObjPtr &record, 
		bool &continue_update, 
		const AosRundataPtr &rdata)
{
	// It updates its value based on 'record'. If this field wants to stop
	// further updating, it sets 'continue_update' to false. Otherwise, 
	// it sets it to 'true'.
	continue_update = false;
	
	u64 vv = 0;
	bool rslt = getU64Priv(vv, record, rdata);
	aos_assert_r(rslt, false);

	switch (mFieldOpr)
	{
	case AosFieldOpr::eFirst:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 mCrtValue = vv;
			 mHasValidValue = true;
		 }
		 return true;

	case AosFieldOpr::eLast:
		 continue_update = true;
		 mCrtValue = vv;
		 return true;

	case AosFieldOpr::eSum:
		 continue_update = true;
		 mCrtValue += vv;
		 return true;

	case AosFieldOpr::eAverage:
		 continue_update = true;
		 mCrtValue += vv;
		 mCount++;
		 return true;

	case AosFieldOpr::eMinimum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 mCrtValue = vv;
			 mHasValidValue = true;
			 return true;
		 }

		 if (vv < mCrtValue)
		 {
			 continue_update = true;
			 mCrtValue = vv;
		 }
		 return true;

	case AosFieldOpr::eMaximum:
		 if (!mHasValidValue)
		 {
			 continue_update = true;
			 mCrtValue = vv;
			 mHasValidValue = true;
			 return true;
		 }

		 if (vv > mCrtValue)
		 {
			 continue_update = true;
			 mCrtValue = vv;
		 }
		 return true;

	case AosFieldOpr::eCount:
		 continue_update = true;
		 mCount++;
		 return true;

	case AosFieldOpr::eField:
		 // It retrieves a value from 'record'. 
		 mCrtValue = vv;
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
AosGroupFieldU64::setRecordByPrevValue(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(record, false);
	if (mSetRecordDocid)
	{
		// This means we need to set the 'mToDocidIdx'-th docid to the 
		// record's docid.
		record->setDocid(mPrevValue);
		return true;
	}

	if (mToFieldIdx >= 0)
	{
		AosValueRslt value;
		value.setU64(mPrevValue);
		bool outofmem = false;
		return record->setFieldValue(mToFieldIdx, value, outofmem, rdata.getPtr());
	}

	return true;
}


bool
AosGroupFieldU64::saveAndClear()
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
		 mCount = 0;
		 mHasValidValue = false;
		 return true;

	case AosFieldOpr::eAverage:
		 if (mCount == 0)
		 {
			 mPrevValue = 0;
		 }
		 else
		 {
			 mPrevValue = mCrtValue / mCount;
		 }

		 mCrtValue = 0;
		 mCount = 0;
		 return true;

	case AosFieldOpr::eCount:
		 mPrevCount = mCount;
		 mCrtValue = 0;
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
AosGroupFieldU64::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosGroupFieldU64 * field = OmnNew AosGroupFieldU64();
	bool rslt = field->config(def, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

