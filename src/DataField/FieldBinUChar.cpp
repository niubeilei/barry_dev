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

#include "DataField/FieldBinUChar.h"

AosFieldBinUChar::AosFieldBinUChar(const bool reg)
:
AosDataField(AosDataFieldType::eBinUChar, AOSDATAFIELDTYPE_BIN_UCHAR, reg)
{
}


AosFieldBinUChar::AosFieldBinUChar(const AosFieldBinUChar &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinUChar::~AosFieldBinUChar()
{
}


bool
AosFieldBinUChar::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFieldInfo.field_len = sizeof(char);
	mFieldInfo.field_data_len = sizeof(char);
	return true;
}


bool
AosFieldBinUChar::getValueFromRecord(
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
	unsigned char field_value = *(unsigned char*)(data + mFieldInfo.field_offset);
	value.setChar((char)field_value);
	idx = mFieldInfo.field_offset + mFieldInfo.field_data_len;
	return true;
}


bool
AosFieldBinUChar::setValueToRecord(
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

	unsigned char vv = value.getChar();
	*(unsigned char *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinUChar::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinUChar(*this);
}


AosDataFieldObjPtr
AosFieldBinUChar::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinUChar * field = OmnNew AosFieldBinUChar(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

