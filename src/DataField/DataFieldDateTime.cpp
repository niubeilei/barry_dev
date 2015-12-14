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
// 06/26/2015 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldDateTime.h"
#include <stdlib.h>


AosDataFieldDateTime::AosDataFieldDateTime(const bool reg)
:
AosDataField(AosDataFieldType::eDateTime, AOSDATAFIELDTYPE_DATETIME, reg)
{
}


AosDataFieldDateTime::AosDataFieldDateTime(const AosDataFieldDateTime &rhs)
:
AosDataField(rhs),
mFormat(rhs.mFormat)
{
}


AosDataFieldDateTime::~AosDataFieldDateTime()
{
}


bool
AosDataFieldDateTime::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFormat = def->getAttrStr("format");
	return true;
}


bool
AosDataFieldDateTime::serializeToXmlDoc(
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

	OmnString str(&data[start_pos], mFieldInfo.field_data_len);
	idx = start_pos + mFieldInfo.field_data_len;
	//str.normalizeWhiteSpace(true, true);  // trim

	// Question 
	docstr << "<" << mName << "><![CDATA[" << str << "]]></" << mName << ">";
	return true;
}


bool
AosDataFieldDateTime::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosDataFieldDateTime::getValueFromRecord(
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
	OmnString key(&data[mFieldInfo.field_offset], mFieldInfo.field_data_len);
	AosDateTime dt(key, mFormat);
	if (dt.isNotADateTime()) 
	{   
		OmnAlarm << "Current DateTime Object is invalid:" << key << "," << mFormat << "," 
			        << OmnString(data, len) << enderr;
		value.setNull();
		if (!mDftValue.isNull())
		{
			            value = mDftValue;
		}
		return true;
	}   

	value.setDateTime(dt);
	return true;
}


bool
AosDataFieldDateTime::setValueToRecord(
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


	const char *ch= value.getDateTime().toString(mFormat).data();
	mFieldInfo.field_len = strlen(ch); 

	memcpy(data + mFieldInfo.field_offset, ch, mFieldInfo.field_len);
	return true;
}


AosDataFieldObjPtr 
AosDataFieldDateTime::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldDateTime(*this);
}


AosDataFieldObjPtr
AosDataFieldDateTime::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldDateTime* field = OmnNew AosDataFieldDateTime(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

