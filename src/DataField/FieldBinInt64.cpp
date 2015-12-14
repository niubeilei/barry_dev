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

#include "DataField/FieldBinInt64.h"


AosFieldBinInt64::AosFieldBinInt64(const bool reg)
:
AosDataField(AosDataFieldType::eBinInt64, AOSDATAFIELDTYPE_BIN_INT64, reg)
{
}


AosFieldBinInt64::AosFieldBinInt64(const AosFieldBinInt64 &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinInt64::~AosFieldBinInt64()
{
}


bool
AosFieldBinInt64::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFieldInfo.field_len = sizeof(i64);
	mFieldInfo.field_data_len = sizeof(i64);
	return true;
}


bool
AosFieldBinInt64::getValueFromRecord(
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

	int64_t field_value = *(int64_t*)(data + mFieldInfo.field_offset);
	value.setI64(field_value);
	idx = mFieldInfo.field_offset + mFieldInfo.field_data_len;
	return true;
}


bool
AosFieldBinInt64::setValueToRecord(
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

	int64_t vv = value.getI64();
	*(int64_t *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinInt64::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinInt64(*this);
}


AosDataFieldObjPtr
AosFieldBinInt64::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinInt64 * field = OmnNew AosFieldBinInt64(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

