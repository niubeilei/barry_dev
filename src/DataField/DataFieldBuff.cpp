////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/29/2012 Created by Chen Ding
// 2014/02/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldBuff.h"


AosDataFieldBuff::AosDataFieldBuff(const bool reg)
:
AosDataField(AosDataFieldType::eBuff, AOSDATAFIELDTYPE_BUFF, reg)
{
}


AosDataFieldBuff::AosDataFieldBuff(const AosDataFieldBuff &rhs)
:
AosDataField(rhs)
{
}


AosDataFieldBuff::~AosDataFieldBuff()
{
}


bool
AosDataFieldBuff::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosDataFieldObjPtr 
AosDataFieldBuff::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldBuff(*this);
}


AosDataFieldObjPtr
AosDataFieldBuff::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldBuff * field = OmnNew AosDataFieldBuff(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}


bool	
AosDataFieldBuff::getValueFromRecord(
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
}


bool
AosDataFieldBuff::setValueToRecord(
		char * const data,
		const int data_len,
		const AosValueRslt &value,
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	if (mFieldInfo.field_offset + mFieldInfo.field_data_len > data_len)
	{
		outofmem = true;
		return true;
	}

	OmnString vv = value.getStr();
	mFieldInfo.field_len = vv.length();

	memcpy(data + mFieldInfo.field_offset, vv.data(), mFieldInfo.field_len);
	return true;
}

