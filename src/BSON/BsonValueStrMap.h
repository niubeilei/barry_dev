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
#ifndef Aos_BSON_BsonValueStrMap_h
#define Aos_BSON_BsonValueStrMap_h

#include "BSON/BsonValue.h"
//#include "BSON/BsonValueStrMapItr.h"
class AosBsonValueStrMapStrItr;

class AosBsonValueStrMap : public AosBsonValue
{
	map<OmnString, AosBsonValueMapStrItrPtr>	mValuesItr;
	AosBsonField::Type  mType;  //the value type in map
	bool    mParsed;
    bool    mChanged;

public:
	AosBsonValueStrMap(
			AosBSON  *mptr,
			OmnString &name,
			i64  pos,
			AosBsonField::Type type);

	~AosBsonValueStrMap();
 
	virtual AosBsonValue & operator [] (const OmnString &name);

	virtual AosValueRslt getValue(const OmnString &name);

   	virtual bool insert(OmnString &name, AosValueRslt &value);
    virtual bool getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw);
    void setChanged(bool flag){mChanged = flag;} 
    virtual bool parse();
};


#endif

