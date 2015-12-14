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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BSON_BsonValueI64Map_h
#define Aos_BSON_BsonValueI64Map_h

#include "BSON/BsonValue.h"
//#include "BSON/BsonValueI64MapItr.h"
class AosBsonValueMapI64Itr;

class AosBsonValueI64Map : public AosBsonValue
{
	map<i64, AosBsonValueMapI64ItrPtr>	mValuesItr;
	AosBsonField::Type mType;
	bool    mParsed;
    bool    mChanged;

public:
	AosBsonValueI64Map(
			AosBSON  *mptr,
			OmnString &name,
			i64  pos,
			AosBsonField::Type type);

	~AosBsonValueI64Map();
 
	virtual AosBsonValue & operator [] (const i64 &name);

	virtual AosValueRslt getValue(const i64 &name);

   	virtual bool insert(i64 &name, AosValueRslt &value);
    virtual bool getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw);
    void setChanged(bool flag){mChanged = flag;} 
    virtual bool parse();
};


#endif

