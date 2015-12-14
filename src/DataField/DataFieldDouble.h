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
// 07/17/2013 Created by Andy zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldDouble_h
#define Aos_DataField_DataFieldDouble_h

#include "DataField/DataField.h"


class AosDataFieldDouble : virtual public AosDataField
{
	OmnDefineRCObject;

	//double		mConstValue;
	//int 		mAccuracy;

public:
	AosDataFieldDouble(const bool reg);
	AosDataFieldDouble(const AosDataFieldDouble &rhs);
	~AosDataFieldDouble();

	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		return AosDataType::eDouble;
	}

	virtual bool	getDefaultIgnoreSerialize() {return false;}
	virtual bool	needValueFromField() {return false;}
	
	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;

	virtual bool	setValueToRecord(
						char * const data,
						const int data_len,
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

	virtual bool 	serializeToXmlDoc(
						OmnString &docstr,
						int &idx,
						const char *data,
						const int data_len,
						AosRundata* rdata);
	
	virtual bool	createRandomValue(
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const;

private:
	bool			config(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata);

	//double 			setAccuracy(double vv);

};

#endif

