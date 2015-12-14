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
#include "DataField/DataFieldVirtual.h"


AosDataFieldVirtual::AosDataFieldVirtual(const bool reg)
:
AosDataField(AosDataFieldType::eVirtual , AOSDATAFIELDTYPE_VIRTUAL, reg)
{
}


AosDataFieldVirtual::AosDataFieldVirtual(const AosDataFieldVirtual &rhs)
:
AosDataField(rhs)
{
}


AosDataFieldVirtual::~AosDataFieldVirtual()
{
}


bool
AosDataFieldVirtual::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);
	mDataType = def->getAttrStr("data_type");
	aos_assert_r(mDataType != "", false);
	return true;
}


bool
AosDataFieldVirtual::serializeToXmlDoc(
		OmnString &docstr,
		int &idx,
		const char *data, 
		const int datalen,
		AosRundata* rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataFieldVirtual::createRandomValue(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataFieldVirtual::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	value = mDftValue;
	return true;
}


bool
AosDataFieldVirtual::setValueToRecord(
		char * const data,
		const int data_len,
		const AosValueRslt &value,
		bool &outofmem,
		AosRundata* rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosDataFieldObjPtr 
AosDataFieldVirtual::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldVirtual(*this);
}


AosDataFieldObjPtr
AosDataFieldVirtual::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldVirtual * field = OmnNew AosDataFieldVirtual(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

