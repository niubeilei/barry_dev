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
// 2014/08/24 Created by Barry Niu 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BSON_BFInt16_h
#define Aos_BSON_BFInt16_h

#include "BSON/BsonField.h"



class AosBFInt16 : public AosBsonField
{
	OmnDefineRCObject;

private:

public:
	AosBFInt16(const int version);
	~AosBFInt16();

	virtual AosJimoPtr cloneJimo() const;
	virtual bool getFieldValue(AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);

	virtual bool appendFieldValue(AosRundata *rdata, 
						const AosValueRslt &value,
						AosBuff *buff);

	virtual bool skipValue(AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos);

	virtual bool setType(AosRundata *rdata, 
						AosBuff *buff);

	virtual bool setField(const OmnString &name, 
						const i16 value, 
						AosBuff *buff_raw);

	virtual bool setField(const u32 name, 
						const i16 value, 
						AosBuff *buff_raw);

private:
	bool appendInt16(const i16 &value, AosBuff *buff_raw);

};

#endif

