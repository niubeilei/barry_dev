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
#include "DataField/DataFieldChar.h"


AosDataFieldChar::AosDataFieldChar(const bool reg)
:
AosDataField(AosDataFieldType::eChar, AOSDATAFIELDTYPE_CHAR, reg)
{
}


AosDataFieldChar::AosDataFieldChar(const AosDataFieldChar &rhs)
:
AosDataField(rhs)
{
}


AosDataFieldChar::~AosDataFieldChar()
{
}


bool
AosDataFieldChar::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataFieldChar::serializeToXmlDoc(
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

	char vv = data[start_pos];
	//OmnString str(&data[start_pos], mFieldInfo.field_data_len);
	//idx = start_pos + mFieldInfo.field_data_len;
	//str.normalizeWhiteSpace(true, true);  // trim
	//vv = str.toU64();
	
	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldChar::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	char vv = rand() % 128;
	buff->setChar(vv);
	return true;
}


bool
AosDataFieldChar::getValueFromRecord(
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

	char vv = data[mFieldInfo.field_offset];
	value.setChar(vv);
	return true;
}


bool
AosDataFieldChar::setValueToRecord(
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

	char ch = value.getChar();
	mFieldInfo.field_len = sizeof(char); 

	memcpy(data + mFieldInfo.field_offset, &ch, mFieldInfo.field_len);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldChar::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldChar(*this);
}


AosDataFieldObjPtr
AosDataFieldChar::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldChar * field = OmnNew AosDataFieldChar(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

