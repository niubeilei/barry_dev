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
#include "DataField/FieldBinInt.h"


AosFieldBinInt::AosFieldBinInt(const bool reg)
:
AosDataField(AosDataFieldType::eBinInt, AOSDATAFIELDTYPE_BIN_INT, reg)
{
}


AosFieldBinInt::AosFieldBinInt(const AosFieldBinInt &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinInt::~AosFieldBinInt()
{
}


bool
AosFieldBinInt::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFieldInfo.field_len = sizeof(int);
	mFieldInfo.field_data_len = sizeof(int);
	return true;
}


bool
AosFieldBinInt::getValueFromRecord(
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
	int field_value = *(int*)(data + mFieldInfo.field_offset);
	value.setI64(field_value);
	idx = mFieldInfo.field_offset + mFieldInfo.field_data_len;
	return true;
}


bool
AosFieldBinInt::setValueToRecord(
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

	int vv = value.getI64();

	*(int *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinInt::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinInt(*this);
}


AosDataFieldObjPtr
AosFieldBinInt::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinInt * field = OmnNew AosFieldBinInt(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

