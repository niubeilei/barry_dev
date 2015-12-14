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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldInt32.h"

#include <stdlib.h>


AosDataFieldInt32::AosDataFieldInt32(const bool reg)
:
AosDataField(AosDataFieldType::eInt32, AOSDATAFIELDTYPE_INT32, reg),
mIsAbsolute(false)
{
}


AosDataFieldInt32::AosDataFieldInt32(const AosDataFieldInt32 &rhs)
:
AosDataField(rhs),
mIsAbsolute(rhs.mIsAbsolute)
{
}


AosDataFieldInt32::~AosDataFieldInt32()
{
}


bool
AosDataFieldInt32::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mIsAbsolute = def->getAttrBool("zky_is_absolute", false);
	return true;
}


bool
AosDataFieldInt32::serializeToXmlDoc(
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
	if (mIgnoreSerialize) return true;

	// Chen Ding, 2014/08/16
	if (mIsNull) return true;
	
	aos_assert_r(idx >= 0 && datalen > 0 && 
			(u32)idx + (u32)mFieldInfo.field_data_len <= (u32)datalen, false);

	int start_pos = mIsFixed ? mFieldInfo.field_offset : idx;
	if (start_pos < 0) return false;

	string s(&data[start_pos], mFieldInfo.field_data_len);

	docstr << "<" << mName << "><![CDATA[" << (i32)(atoll(s.c_str())) << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldInt32::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	i32 vv = rand() % 0xffffffff;
	buff->setI64(vv);

	return true;
}


bool
AosDataFieldInt32::getValueFromRecord(
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
	i32 vv = atoll(s.data());
	if (mIsAbsolute && (vv < 0)) vv = -(vv);

	value.setI64(vv);
	return true;
}


bool
AosDataFieldInt32::setValueToRecord(
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
AosDataFieldInt32::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldInt32(*this);
}


AosDataFieldObjPtr
AosDataFieldInt32::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldInt32 * field = OmnNew AosDataFieldInt32(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

