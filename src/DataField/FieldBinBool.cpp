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
#include "DataField/FieldBinBool.h"


AosFieldBinBool::AosFieldBinBool(const bool reg)
:
AosDataField(AosDataFieldType::eBinBool, AOSDATAFIELDTYPE_BIN_BOOL, reg)
{
}


AosFieldBinBool::AosFieldBinBool(const AosFieldBinBool &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinBool::~AosFieldBinBool()
{
}


bool
AosFieldBinBool::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFieldInfo.field_len = sizeof(bool); 
	mFieldInfo.field_data_len = sizeof(bool); 
	return true;
}


bool
AosFieldBinBool::getValueFromRecord(
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
	bool field_value = *(bool*)(data + mFieldInfo.field_offset);
	value.setBool(field_value);
	idx = mFieldInfo.field_offset + sizeof(bool);
	return true;
}


bool
AosFieldBinBool::setValueToRecord(
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

	bool vv = value.getBool();

	*(bool*)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinBool::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinBool(*this);
}


AosDataFieldObjPtr
AosFieldBinBool::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinBool * field = OmnNew AosFieldBinBool(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

