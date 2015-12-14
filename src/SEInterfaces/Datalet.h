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
// 2013/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_Datalet_h
#define Aos_SEInterfaces_Datalet_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/DataletType.h"
#include "SEInterfaces/DataRecordObj.h"


class AosDatalet : public AosJimo
{
protected:
	AosDataletType::E		mDataletType;

public:
	AosDatalet(const AosDataletType::E type, const u32 version);
	~AosDatalet();

	virtual bool		isFixed() const = 0;
	virtual int			getDataletLen() const = 0;
	virtual void 		clear() = 0;

	virtual bool 		setData(
							char *data, 
							const int len, 
							const AosBuffDataPtr &metaData, 
							const int64_t offset,
							const bool need_copy) = 0;

	virtual AosDataletPtr clone() const = 0;

	virtual bool setFieldValue( AosRundata *rdata,
								const OmnString &field_name, 
								const AosValueRslt &value) = 0;

	virtual bool setFieldValue( AosRundata *rdata,
								const int field_idx, 
								const AosValueRslt &value) = 0; 

	virtual bool setFieldValue( AosRundata *rdata, 
								const char * const field_name, 
								const AosValueRslt &value) = 0; 

	virtual bool setFieldValueWPath( 
								AosRundata *rdata,
								const char * const name, 
								const AosValueRslt &value) = 0;

	virtual bool getFieldValue( AosRundata *rdata, 
								const char *field_name, 
								AosValueRslt &value) = 0;

	virtual bool getFieldValue( AosRundata *rdata, 
								const int field_idx, 
								AosValueRslt &value) = 0;

	virtual bool getFieldValue( AosRundata *rdata, 
								const char *data,
								const int data_len,
								const char *field_name,
								AosValueRslt &value) = 0;

	virtual bool getFieldValue( AosRundata *rdata, 
								const char *data,
								const int data_len,
								const int field_idx,
								AosValueRslt &value) = 0;

	virtual bool getFieldValueWPath(
								AosRundata *rdata, 
								const char * const name, 
								AosValueRslt &value) = 0;

	virtual bool serializeToBuff(AosRundata *rdata, AosBuff *buff) = 0;

	virtual AosDataletType::E getType() const = 0;
};

#endif
