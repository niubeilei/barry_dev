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
// 06/26/2015 created by Young
////////////////////////////////////////////////////////////////////////////
#include "DataField/FieldBinDateTime.h"
#include <stdlib.h>


AosFieldBinDateTime::AosFieldBinDateTime(const bool reg)
:
AosDataField(AosDataFieldType::eBinDateTime, AOSDATAFIELDTYPE_BIN_DATETIME, reg)
{
}


AosFieldBinDateTime::AosFieldBinDateTime(const AosFieldBinDateTime &rhs)
:
AosDataField(rhs)
{
}


AosFieldBinDateTime::~AosFieldBinDateTime()
{
}


bool
AosFieldBinDateTime::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mFormat = def->getAttrStr("format");
	mFieldInfo.field_len = sizeof(u64);
	mFieldInfo.field_data_len = sizeof(u64);
	return true;
}


bool
AosFieldBinDateTime::serializeToXmlDoc(
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
	aos_assert_r(idx >= 0 && datalen > 0, false);

	int start_pos = mFieldInfo.field_offset;
	if (start_pos < 0) return false;

	string str(&data[start_pos], mFieldInfo.field_data_len);
	idx = start_pos + mFieldInfo.field_data_len;
	AosDateTime vv(str, mFormat);
	if (vv.isNotADateTime()) 
	{   
		OmnAlarm << "Current DateTime Object is invalid" << enderr;
		return false;
	}   
	// Question 
	docstr << "<" << mName << "><![CDATA[" << str << "]]></" << mName << ">";
	return true;
}


bool
AosFieldBinDateTime::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosFieldBinDateTime::getValueFromRecord(
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

	int64_t epoch_second = *(int64_t*)(data + mFieldInfo.field_offset);
	AosDateTime dt(time_t(epoch_second), mFormat);
	if (dt.isNotADateTime()) 
	{   
		OmnAlarm << "Current DateTime Object is invalid:" << epoch_second << enderr;
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
AosFieldBinDateTime::setValueToRecord(
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

	i64 vv = value.getI64();
	*(i64 *)&data[mFieldInfo.field_offset] = vv;
	return true;
}


AosDataFieldObjPtr 
AosFieldBinDateTime::clone(AosRundata *rdata) const
{
	return OmnNew AosFieldBinDateTime(*this);
}


AosDataFieldObjPtr
AosFieldBinDateTime::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosFieldBinDateTime* field = OmnNew AosFieldBinDateTime(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

