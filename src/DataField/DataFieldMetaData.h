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
// 2013/01/25 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldMetaData_h
#define Aos_DataField_DataFieldMetaData_h

#include "DataField/DataField.h"


class AosDataFieldMetaData : virtual public AosDataField
{
	OmnDefineRCObject;

	AosBuffData::MetaDataType	mMetaDataType;
	OmnString					mMetaDataAttrName;

public:
	AosDataFieldMetaData(const bool reg);
	AosDataFieldMetaData(const AosDataFieldMetaData &rhs);
	~AosDataFieldMetaData();
	
	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		return AosDataType::eString;
	}

	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;

	virtual bool	getValueFromRecord(
						AosDataRecordObj* record,
						const char * data,
						const int len,
						int &idx,
						AosValueRslt &value, 
						const bool copy_flag,
						AosRundata* rdata);

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const;

private:
	bool			config(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata);

};

#endif

