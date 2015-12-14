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
#include "DataField/DataFieldMetaData.h"


AosDataFieldMetaData::AosDataFieldMetaData(const bool reg)
:
AosDataField(AosDataFieldType::eMetaData, AOSDATAFIELDTYPE_METADATA, reg)
{
}


AosDataFieldMetaData::AosDataFieldMetaData(const AosDataFieldMetaData &rhs)
:
AosDataField(rhs),
mMetaDataType(rhs.mMetaDataType),
mMetaDataAttrName(rhs.mMetaDataAttrName)
{
}


AosDataFieldMetaData::~AosDataFieldMetaData()
{
}


bool
AosDataFieldMetaData::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	mMetaDataAttrName = def->getAttrStr(AOSTAG_METADATA_ATTRNAME);
	return true;
}


bool
AosDataFieldMetaData::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(record, false);

	AosMetaDataPtr metaData = record->getMetaData();
	aos_assert_r(metaData, false);

	OmnString v = metaData->getAttribute(mMetaDataAttrName);
	value.setStr(v);
	//OmnScreen << "metadata -------- attrname : " << mMetaDataAttrName << ", value : " << v << endl;
	return true;
}


AosDataFieldObjPtr 
AosDataFieldMetaData::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldMetaData(*this);
}


AosDataFieldObjPtr
AosDataFieldMetaData::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldMetaData * field = OmnNew AosDataFieldMetaData(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

