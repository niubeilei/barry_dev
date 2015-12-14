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
// 01/16/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DataField/FieldBinU64.h"


AosFieldBinU64::AosFieldBinU64(const bool reg)
:
AosDataField(AosDataFieldType::eBinU64, AOSDATAFIELDTYPE_BIN_U64, reg)
{
}


AosFieldBinU64::AosFieldBinU64(const AosFieldBinU64 &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinU64::~AosFieldBinU64()
{
}


bool
AosFieldBinU64::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(mValueFromFieldIdx < 0, false);

	mFieldInfo.field_len = sizeof(u64);
	mFieldInfo.field_data_len = sizeof(u64);
	return true;
}


bool
AosFieldBinU64::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	if (mIsNull)
	{
		value.setNull();
		if (!mDftValue.isNull())
		{
			value = mDftValue;
		}
		return true;
	}

	u64 field_value = *(u64*)(data + mFieldInfo.field_offset);
	value.setU64(field_value);
	idx = mFieldInfo.field_offset + mFieldInfo.field_data_len;
	return true;
}


bool
AosFieldBinU64::setValueToRecord(
		char * const data,
		const int data_len,
		const AosValueRslt &value,
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;

	if (data_len < mFieldInfo.field_offset + mFieldInfo.field_data_len)
	{
		outofmem = true;
		return true;
	}

	u64 vv = value.getU64();
	
	*(u64 *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinU64::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinU64(*this);
}


AosDataFieldObjPtr
AosFieldBinU64::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinU64 * field = OmnNew AosFieldBinU64(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

