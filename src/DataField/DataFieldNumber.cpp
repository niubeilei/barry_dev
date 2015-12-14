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
#include "DataField/DataFieldNumber.h"
#include <stdlib.h>


AosDataFieldNumber::AosDataFieldNumber(const bool reg)
:
AosDataField(AosDataFieldType::eNumber, AOSDATAFIELDTYPE_NUMBER, reg),
mPrecision(38),
mScale(0)
{
}


AosDataFieldNumber::AosDataFieldNumber(const AosDataFieldNumber &rhs)
:
AosDataField(rhs),
mPrecision(rhs.mPrecision),
mScale(rhs.mScale)
{
}


AosDataFieldNumber::~AosDataFieldNumber()
{
}


bool
AosDataFieldNumber::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mPrecision = def->getAttrU32("zky_precision", 38);
	mScale =  def->getAttrInt("zky_scale", 0);

	return true;
}


bool
AosDataFieldNumber::serializeToXmlDoc(
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
	double vv = atof(s.c_str());

	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldNumber::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	return true;
}


bool
AosDataFieldNumber::getValueFromRecord(
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

	//AosNumber num(mPrecision, mScale);	
	OmnString s(&data[mFieldInfo.field_offset], mFieldInfo.field_data_len);
	value.setDouble(atof(s.data()));
	return true;
}


bool
AosDataFieldNumber::setValueToRecord(
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

	char  ch[24];
	double d_value = value.getDouble();
	sprintf(ch, "%lf", d_value);
	mFieldInfo.field_len = strlen(ch); 

	memcpy(data + mFieldInfo.field_offset, ch, mFieldInfo.field_len);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldNumber::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldNumber(*this);
}


AosDataFieldObjPtr
AosDataFieldNumber::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldNumber* field = OmnNew AosDataFieldNumber(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}
