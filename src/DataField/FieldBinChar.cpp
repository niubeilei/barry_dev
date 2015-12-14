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

#include "DataField/FieldBinChar.h"

AosFieldBinChar::AosFieldBinChar(const bool reg)
:
AosDataField(AosDataFieldType::eBinChar, AOSDATAFIELDTYPE_BIN_CHAR, reg)
{
}


AosFieldBinChar::AosFieldBinChar(const AosFieldBinChar &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinChar::~AosFieldBinChar()
{
}


bool
AosFieldBinChar::config(
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
AosFieldBinChar::getValueFromRecord(
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
	char field_value = data[mFieldInfo.field_offset];
	value.setChar(field_value);
	idx = mFieldInfo.field_offset + mFieldInfo.field_data_len;
	return true;
}


bool
AosFieldBinChar::setValueToRecord(
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

	char vv = value.getChar();

	data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinChar::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinChar(*this);
}


AosDataFieldObjPtr
AosFieldBinChar::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinChar * field = OmnNew AosFieldBinChar(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

