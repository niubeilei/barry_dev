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
// 2014/05/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldXpath_h
#define Aos_DataField_DataFieldXpath_h

#include "DataField/DataField.h"


class AosDataFieldXpath : virtual public AosDataField
{
	OmnDefineRCObject;

	OmnString		mXpath;

	AosDataType::E	mValueType;
	u32				mPrecision;
	int				mScale;

public:
	AosDataFieldXpath(const bool reg);
	AosDataFieldXpath(const AosDataFieldXpath &rhs);
	~AosDataFieldXpath();

	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		return AosDataType::eString;
	}

	virtual bool	getDefaultIgnoreSerialize() {return false;}
	virtual bool	needValueFromField() {return false;}
	
	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;

	virtual bool	setValueToRecord(
						AosDataRecordObj* record,
						const AosValueRslt &value, 
						bool &outofmem,
						AosRundata* rdata);

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

