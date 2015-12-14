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
// 2013/07/18 Created by Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldBool.h"

#include <algorithm>
#include <cstring>

AosDataFieldBool::AosDataFieldBool(const bool reg)
:
AosDataField(AosDataFieldType::eBool, AOSDATAFIELDTYPE_BOOL, reg),
mDefault(false)
{
}


AosDataFieldBool::AosDataFieldBool(const AosDataFieldBool &rhs)
:
AosDataField(rhs),
mDefault(rhs.mDefault)
{
}


AosDataFieldBool::~AosDataFieldBool()
{
}


bool
AosDataFieldBool::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr dft = def->getFirstChild("default");
	if (dft)
	{
		OmnString strDef = dft->getNodeText();
		strDef.normalizeWhiteSpace(true, true);  // trim
		if (strDef.toLower() == "true") mDefault = true;
		else if (strDef.toLower() != "") mDefault = false;
	}
	return true;
}


bool
AosDataFieldBool::getValueFromRecord(
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

	string key(&data[mFieldInfo.field_offset], mFieldInfo.field_data_len);
	transform(key.begin(), key.end(), key.begin(), ::tolower);
	key.erase(0, key.find_first_not_of(" \t\n\r"));
	key.erase(key.find_last_not_of(" \t\n\r")+1);

	if (strcmp(key.data(), "true") == 0){
		value.setBool(true);
	} else {
		value.setBool(mDefault);
	}
	return true;
}


bool
AosDataFieldBool::setValueToRecord(
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
AosDataFieldBool::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldBool(*this);
}


AosDataFieldObjPtr
AosDataFieldBool::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldBool * field = OmnNew AosDataFieldBool(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}
