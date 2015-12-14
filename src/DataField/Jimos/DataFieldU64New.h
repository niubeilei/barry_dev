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
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_Jimos_DataFieldU64New_h
#define Aos_DataField_Jimos_DataFieldU64New_h

#include "DataField/DataField.h"


class AosDataFieldU64New : public AosDataField
{
	OmnDefineRCObject;

private:
	bool	mParsed;
	u64		mValue;
	AosDataType::E mSourceDataType;

public:
	AosDataFieldU64New(const int version);
	AosDataFieldU64New(const AosDataFieldU64New &rhs);
	~AosDataFieldU64New();
	
	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;

	virtual bool	getValueFromRecord(
						AosDataRecordObj* record,
						const char * data,
						const int len,
						int &idx,
						AosValueRslt &value, 
						const bool copy_flag,
						AosRundata* rdata);

	virtual bool	createRandomValue(
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const;

	bool serializeToXmlDoc(
						OmnString &docstr,
						int &idx,
						const char *data, 
						const int datalen,
						AosRundata* rdata);

	bool setValueToRecord(
						AosDataRecordObj* record,
						const AosValueRslt &value,
						bool &outofmem,
						AosRundata* rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata);

};

#endif

