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
// 07/17/2013 Created by Andy zhang 
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldDouble.h"
#include <stdlib.h>


AosDataFieldDouble::AosDataFieldDouble(const bool reg)
:
AosDataField(AosDataFieldType::eDouble, AOSDATAFIELDTYPE_DOUBLE, reg)
{
}


AosDataFieldDouble::AosDataFieldDouble(const AosDataFieldDouble &rhs)
:
AosDataField(rhs)
{
}


AosDataFieldDouble::~AosDataFieldDouble()
{
}


bool
AosDataFieldDouble::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDataFieldDouble::serializeToXmlDoc(
		OmnString &docstr,
		int &idx,
		const char *data, 
		const int datalen,
		AosRundata* rdata)
{
	// It converts the data from 'data' to 'docstr'. The value in 'data'
	// starts at 'idx', and its length is 'mFieldInfo.field_data_len'. 
	// If needed, it will do the data conversion (not supported yet)
	// from the original form to the target form.
	if(mIgnoreSerialize) return true;

	// Chen Ding, 2014/08/16
	if (mIsNull) return true;
	
	aos_assert_r(data, false);
	aos_assert_r(mFieldInfo.field_data_len >= 0, false);
	aos_assert_r(idx >= 0 && datalen > 0 && 
			(u32)idx + (u32)mFieldInfo.field_data_len <= (u32)datalen, false);

	int start_pos = mIsFixed ? mFieldInfo.field_offset : idx;
	if (start_pos < 0) return false;

	string s(&data[start_pos], mFieldInfo.field_data_len);
	idx = start_pos + mFieldInfo.field_data_len;
	docstr << "<" << mName << "><![CDATA[" << atof(s.c_str()) << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldDouble::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	double vv = rand() % 0xffffffff;
	buff->setDouble(vv);
	return true;
}


bool
AosDataFieldDouble::getValueFromRecord(
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
	OmnString s(&data[mFieldInfo.field_offset], mFieldInfo.field_data_len);
	double vv = atof(s.data());
	value.setDouble(vv);
	return true;
}


bool
AosDataFieldDouble::setValueToRecord(
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

	OmnString vv = value.getStr();
	mFieldInfo.field_len = vv.length();

	memcpy(data + mFieldInfo.field_offset, vv.data(), mFieldInfo.field_len);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldDouble::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldDouble(*this);
}


AosDataFieldObjPtr
AosDataFieldDouble::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldDouble* field = OmnNew AosDataFieldDouble(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

/*
double
AosDataFieldDouble::setAccuracy(double vv)
{
	aos_assert_r(0 <= mAccuracy && mAccuracy <= 10, false);
	int tem_num = 1;
	for (int i = 0; i < mAccuracy; i++)
	{
		tem_num = tem_num * 10;
	}

	return double(i64(vv * tem_num + 0.5))/tem_num;
}
*/

