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
// 2014/09/04 Created by Rain
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BSON_BFDocument_h
#define Aos_BSON_BFDocument_h

#include "BSON/BsonField.h"
#include "BSON/BSON.h"
#include "BSON/Ptrs.h"


class AosBFDocument : public AosBsonField
{
	OmnDefineRCObject;

private:
	AosBSONPtr	mBSON;

public:
	AosBFDocument(const int version);
	~AosBFDocument();

	virtual AosJimoPtr cloneJimo() const;

	virtual bool getFieldValue(
						AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos, 
						const bool copy_flag,
						AosValueRslt &value);

	virtual bool appendFieldValue(
						AosRundata *rdata, 
						const AosValueRslt &value,
						AosBuff *buff);

	virtual bool skipValue(AosRundata *rdata, 
						AosBuff *buff, 
						i64 &pos);
	
	virtual bool setType(AosRundata *rdata,
						AosBuff *buff);
/*
	virtual bool xpathSetAttr(
						AosRundata *rdata, 
						AosBuffPtr buff,
						const vector<OmnString> &fnames, 
						int crt_idx,
						const AosValueRslt &value, 
						const bool create);
*/
	virtual bool setField(
						const OmnString &name,
						const OmnString &value,
						AosBuff *buff){return false;}

	virtual bool setField(
						const u32 name,
						const OmnString &value,
						AosBuff *buff){return false;}
};

#endif
