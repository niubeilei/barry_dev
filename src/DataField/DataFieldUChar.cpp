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
// 06/30/2015 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldUChar.h"


AosDataFieldUChar::AosDataFieldUChar(const bool reg)
:
AosDataField(AosDataFieldType::eUChar, AOSDATAFIELDTYPE_UCHAR, reg)
{
}


AosDataFieldUChar::AosDataFieldUChar(const AosDataFieldUChar &rhs)
:
AosDataField(rhs)
{
}


AosDataFieldUChar::~AosDataFieldUChar()
{
}


bool
AosDataFieldUChar::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataFieldUChar::serializeToXmlDoc(
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
	
	aos_assert_r(data, false);
	aos_assert_r(mFieldInfo.field_data_len >= 0, false);
	aos_assert_r(idx >= 0 && datalen > 0 && 
			(u32)idx + (u32)mFieldInfo.field_data_len <= (u32)datalen, false);

	int start_pos = mIsFixed ? mFieldInfo.field_offset : idx;
	if (start_pos < 0) return false;

	unsigned char vv = data[start_pos];
	//OmnString str(&data[start_pos], mFieldInfo.field_data_len);
	//idx = start_pos + mFieldInfo.field_data_len;
	//str.normalizeWhiteSpace(true, true);  // trim
	//vv = str.toU64();
	
	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldUChar::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	unsigned char vv = rand() % 256;
	buff->setChar(vv);
	return true;
}


bool
AosDataFieldUChar::getValueFromRecord(
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

	unsigned char vv = data[mFieldInfo.field_offset];
	value.setChar(char(vv));
	return true;
}


bool
AosDataFieldUChar::setValueToRecord(
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

	unsigned char ch = value.getChar();
	mFieldInfo.field_len = sizeof(unsigned char); 

	memcpy(data + mFieldInfo.field_offset, &ch, mFieldInfo.field_len);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldUChar::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldUChar(*this);
}


AosDataFieldObjPtr
AosDataFieldUChar::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldUChar * field = OmnNew AosDataFieldUChar(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

