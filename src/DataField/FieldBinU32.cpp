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
#include "DataField/FieldBinU32.h"


AosFieldBinU32::AosFieldBinU32(const bool reg)
:
AosDataField(AosDataFieldType::eBinU32, AOSDATAFIELDTYPE_BIN_U32, reg)
{
}


AosFieldBinU32::AosFieldBinU32(const AosFieldBinU32 &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinU32::~AosFieldBinU32()
{
}


bool
AosFieldBinU32::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFieldInfo.field_len = sizeof(u32);
	mFieldInfo.field_data_len = sizeof(u32);
	return true;
}


bool
AosFieldBinU32::getValueFromRecord(
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
	u32 field_value = *(u32*)(data + mFieldInfo.field_offset);
	value.setU64(field_value);
	return true;
}


bool
AosFieldBinU32::setValueToRecord(
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

	u32 vv = value.getU64();
	*(u32 *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinU32::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinU32(*this);
}


AosDataFieldObjPtr
AosFieldBinU32::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinU32 * field = OmnNew AosFieldBinU32(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

