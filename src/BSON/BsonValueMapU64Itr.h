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
#ifndef Aos_BSON_BsonValueMapU64Itr_h
#define Aos_BSON_BsonValueMapU64Itr_h

#include "BSON/BsonValue.h"
#include "BSON/BsonValueU64Map.h"

class AosBsonValueMapU64Itr : virtual public AosBsonValue
{
protected:
    u64                 keyName;
	AosBsonValueU64Map  *mMap;
	AosValueRslt	    mValue;

public:
	AosBsonValueMapU64Itr(
			AosBSON  *mptr,
			AosBsonField::Type type,
			const u64 &name,
			AosBsonValueU64Map *mp, 
			const AosValueRslt &Value = AosValueRslt());
	
	~AosBsonValueMapU64Itr();

    //OmnString & getValue();

    virtual AosBsonValue & operator = (const OmnString &value); 
    virtual AosBsonValue & operator = (const u64 &value); 
    virtual AosBsonValue & operator = (const i64 &value); 
    virtual AosBsonValue & operator = (const double value); 

    AosValueRslt & getValue();
	virtual AosBsonValue & operator = (const AosValueRslt &value);
	virtual operator AosValueRslt ();
 	virtual operator OmnString ();
 	virtual operator u64 ();
 	virtual operator i64 ();
 	virtual operator double ();
};

#endif

