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
// 2014/09/12 Created by Barry Niu 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BSON_BFArray_h
#define Aos_BSON_BFArray_h

#include "BSON/BsonField.h"



class AosBFArray : public AosBsonField
{
	OmnDefineRCObject;

private:

public:
	AosBFArray(const int version);
	~AosBFArray();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool getFieldValue(
			            AosRundata *rdata,
						AosBuff *buff, 
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);

	virtual bool getFieldValue(
						AosRundata *rdata,
			            AosBuff *buff, 
						const int idx,
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);

	virtual bool appendFieldValue(
						AosRundata *rdata,
			            const AosValueRslt &value,
						AosBuff *buff);

	virtual bool appendFieldValue(
			            AosRundata *rdata,  
						//const char data_type, 
						const AosBsonField::Type data_type,
						const AosValueRslt &value,
						AosBuff *buff);

	virtual bool skipValue(
						AosRundata *rdata,
			            AosBuff *buff, 
						i64 &pos);

	virtual bool setType(
			         AosRundata *rdata,
			         AosBuff *buff);
	
 /*  	virtual bool setField(
						const OmnString &name,
						const OmnString &value,
						AosBuff *buff){return false;} */

	virtual bool setField(
			            const OmnString &name,
						vector<AosValueRslt> &values,
						AosBuff *buff);

	virtual bool setField(
						const u32 name,
						const OmnString &value,
						AosBuff *buff){return false;}
private:
    AosBsonField::Type getValueType(AosValueRslt value);
};

#endif

