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
// 2014/05/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldXpath.h"

#include "DataRecord/RecordXml.h"


AosDataFieldXpath::AosDataFieldXpath(const bool reg)
:
AosDataField(AosDataFieldType::eXpath, AOSDATAFIELDTYPE_XPATH, reg),
mValueType(AosDataType::eString),
mPrecision(38),
mScale(0)
{
}


AosDataFieldXpath::AosDataFieldXpath(const AosDataFieldXpath &rhs)
:
AosDataField(rhs),
mXpath(rhs.mXpath),
mValueType(rhs.mValueType),
mPrecision(rhs.mPrecision),
mScale(rhs.mScale)
{
}


AosDataFieldXpath::~AosDataFieldXpath()
{
}


bool
AosDataFieldXpath::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mXpath = def->getAttrStr(AOSTAG_XPATH);
	aos_assert_r(mXpath != "", false);

	mValueType = AosDataType::toEnum(def->getAttrStr(AOSTAG_VALUE_TYPE));
	if (!AosDataType::isValid(mValueType))
	{
		mValueType = AosDataType::eString;
	}

	if (mValueType == AosDataType::eNumber)
	{
		mPrecision = def->getAttrU32("zky_precision", 38);
		mScale = def->getAttrInt("zky_scale", 0);
		aos_assert_r(mPrecision > 0, false);
	}

	return true;
}


bool
AosDataFieldXpath::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(mXpath != "", false);
	aos_assert_r(record && record->getType() == AosDataRecordType::eXml, false);

	AosRecordXml * rcd = dynamic_cast<AosRecordXml *>(record);
	OmnString vv = rcd->xpathQuery(mXpath, rdata);
	vv.normalizeWhiteSpace(true, true);

	if (vv == "NULL")
	{
		value.setNull();	
	}
	else
	{
		//value.setValue(vv);
		value.setStr(vv);
	}

	return true;
}


bool
AosDataFieldXpath::setValueToRecord(
		AosDataRecordObj* record,
		const AosValueRslt &value,
		bool &outofmem,
		AosRundata* rdata)
{
	outofmem = false;
	aos_assert_r(mXpath != "", false);
	aos_assert_r(record && record->getType() == AosDataRecordType::eXml, false);

	OmnString str = "";
	if (value.isNull()) 
	{
		str = "NULL";
	}
	else 
	{
		str = value.getStr();
		if (str == "\b")
		{
			str = "NULL";
		}
	}

	AosRecordXml * rcd = dynamic_cast<AosRecordXml *>(record);
	return rcd->xpathSetAttr(mXpath, str, rdata);
}


AosDataFieldObjPtr 
AosDataFieldXpath::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldXpath(*this);
}


AosDataFieldObjPtr
AosDataFieldXpath::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldXpath * field = OmnNew AosDataFieldXpath(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

