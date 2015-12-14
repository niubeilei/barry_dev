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
#ifndef Aos_BSON_BsonValueU64Map_h
#define Aos_BSON_BsonValueU64Map_h

#include "BSON/BsonValue.h"
//#include "BSON/BsonValueU64MapItr.h"
class AosBsonValueMapU64Itr;

class AosBsonValueU64Map : public AosBsonValue
{
	map<u64, AosBsonValueMapU64ItrPtr>	mValuesItr;
	AosBsonField::Type mType;
	bool    mParsed;
    bool    mChanged;

public:
	AosBsonValueU64Map(
			AosBSON  *mptr,
			OmnString &name,
			i64  pos,
			AosBsonField::Type type);

	~AosBsonValueU64Map();
 
	virtual AosBsonValue & operator [] (const u64 &name);

	virtual AosValueRslt getValue(const u64 &name);

   	virtual bool insert(u64 &name, AosValueRslt &value);
    virtual bool getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw);
    void setChanged(bool flag){mChanged = flag;} 
    virtual bool parse();
};


#endif

