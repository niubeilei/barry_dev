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
// Chen Ding, 2014/08/16
// Null Values
// -----------
//
// Modification History:
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldStr.h"


AosDataFieldStr::AosDataFieldStr(const bool reg)
:
AosDataField(AosDataFieldType::eStr, AOSDATAFIELDTYPE_STR, reg)
{
}


AosDataFieldStr::AosDataFieldStr(const AosDataFieldStr &rhs)
:
AosDataField(rhs)
{
}


AosDataFieldStr::~AosDataFieldStr()
{
}


bool
AosDataFieldStr::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataFieldStr::serializeToXmlDoc(
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

	OmnString vv;

	aos_assert_r(data, false);
	aos_assert_r(mFieldInfo.field_data_len >= 0, false);
	aos_assert_r(idx >= 0 && datalen > 0 && 
			(u32)idx + (u32)mFieldInfo.field_data_len <= (u32)datalen, false);

	int start_pos = mIsFixed ? mFieldInfo.field_offset : idx;
	if (start_pos < 0) return false;

	vv.assign(&data[start_pos], mFieldInfo.field_data_len);
	idx = start_pos + mFieldInfo.field_data_len;
	vv.normalizeWhiteSpace(true, true);  // trim

	docstr << "<" << mName << "><![CDATA[" << vv << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldStr::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(mFieldInfo.field_data_len >= 0, false);

	char data[mFieldInfo.field_data_len + 1];
	AosRandomLetterStr(mFieldInfo.field_data_len, data);
	
	buff->setBuff(data, mFieldInfo.field_data_len);
	return true;
}


bool
AosDataFieldStr::getValueFromRecord(
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

	value.setStr(OmnString(&data[mFieldInfo.field_offset], mFieldInfo.field_data_len));
	return true;
/*
	OmnString field_delimiter = record->getFieldDelimiter();
	aos_assert_r(field_delimiter != "", false);

	OmnString text_qualifier = record->getTextQualifier();

	if (text_qualifier == "")
	{
		const char * str1 = strstr(&data[idx], field_delimiter.data());
		int str_len = 0;
		if (str1 == 0) 
		{
			str_len = len - idx;
		}
		else
		{
			str_len = str1 - &data[idx];
			if (str_len + idx > len)
			{
				str_len = len - idx;
			}
		}

		value.setCharStr1(&data[idx], str_len, copy_flag);
		idx += str_len;
		return true;	
	}

	int qualifier_len = text_qualifier.length();
	int intRslt = strncmp(&data[idx], text_qualifier.data(), qualifier_len);
	if (intRslt != 0)
	{
		const char * str1 = strstr(&data[idx], field_delimiter.data());
		int str_len = 0;
		if (str1 == 0) 
		{
			str_len = len - idx;
		}
		else
		{
			str_len = str1 - &data[idx];
			if (str_len + idx > len)
			{
				str_len = len - idx;
			}
		}

		value.setCharStr1(&data[idx], str_len, copy_flag);
		idx += str_len;
		return true;	
	}
		
	idx += qualifier_len;

	OmnString sep = text_qualifier;
	sep << field_delimiter;

	int str_len = 0;
	const char* str1 = strstr(&data[idx], sep.data());
	if (str1 == 0)
	{
		intRslt = strncmp(&data[len - qualifier_len], text_qualifier.data(), qualifier_len);
		aos_assert_r(intRslt == 0, false);

		str_len = len - qualifier_len - idx;
	}
	else
	{
		str_len = str1 - &data[idx];
		if (str_len + idx + qualifier_len > len)
		{
			str_len = len - idx - qualifier_len;
		}
	}
	
	value.setCharStr1(&data[idx], str_len, copy_flag);
	idx += str_len;

	idx += qualifier_len;
		
	return true;
*/
}


bool
AosDataFieldStr::setValueToRecord(
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
AosDataFieldStr::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldStr(*this);
}


AosDataFieldObjPtr
AosDataFieldStr::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldStr * field = OmnNew AosDataFieldStr(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

