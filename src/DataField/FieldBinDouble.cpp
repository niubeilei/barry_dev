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
#include "DataField/FieldBinDouble.h"


AosFieldBinDouble::AosFieldBinDouble(const bool reg)
:
AosDataField(AosDataFieldType::eBinDouble, AOSDATAFIELDTYPE_BIN_DOUBLE, reg)
{
}


AosFieldBinDouble::AosFieldBinDouble(const AosFieldBinDouble &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinDouble::~AosFieldBinDouble()
{
}


bool
AosFieldBinDouble::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFieldInfo.field_data_len = sizeof(double);
	mFieldInfo.field_len = sizeof(double);
	return true;
}


bool
AosFieldBinDouble::getValueFromRecord(
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
	double field_value = *(double*)(data + mFieldInfo.field_offset);
	value.setDouble(field_value);
	return true;
}


bool
AosFieldBinDouble::setValueToRecord(
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

	double vv = value.getDouble();
	*(double *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinDouble::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinDouble(*this);
}


AosDataFieldObjPtr
AosFieldBinDouble::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinDouble * field = OmnNew AosFieldBinDouble(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

