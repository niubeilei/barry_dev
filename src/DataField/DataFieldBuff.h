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
// 2014/02/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldBuff_h
#define Aos_DataField_DataFieldBuff_h

#include "DataField/DataField.h"


class AosDataFieldBuff : virtual public AosDataField
{
	OmnDefineRCObject;

public:
	AosDataFieldBuff(const bool reg);
	AosDataFieldBuff(const AosDataFieldBuff &rhs);
	~AosDataFieldBuff();
	
	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		return AosDataType::eBuff;
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

	bool setValueToRecord(
						char * const data,
						const int data_len,
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

