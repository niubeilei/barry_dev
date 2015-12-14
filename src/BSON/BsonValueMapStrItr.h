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
#ifndef Aos_BSON_BsonValueMapStrItr_h
#define Aos_BSON_BsonValueMapStrItr_h

#include "BSON/BsonValue.h"
#include "BSON/BsonValueStrMap.h"

class AosBsonValueMapStrItr : virtual public AosBsonValue
{
protected:
    OmnString        keyName;
	AosBsonValueStrMap  *mMap;
	AosValueRslt	 mValue;

public:
	AosBsonValueMapStrItr(
			AosBSON  *mptr,
			AosBsonField::Type type,
			const OmnString &name,
			AosBsonValueStrMap *mp, 
			const AosValueRslt &Value = AosValueRslt());
	
	~AosBsonValueMapStrItr();


    virtual AosBsonValue & operator = (const OmnString &value); 
    virtual AosBsonValue & operator = (const u64 &value); 
    virtual AosBsonValue & operator = (const i64 &value); 
    virtual AosBsonValue & operator = (const double value); 

 	virtual operator OmnString ();
 	virtual operator u64 ();
 	virtual operator i64 ();
 	virtual operator double ();
    AosValueRslt & getValue();
	virtual AosBsonValue & operator = (const AosValueRslt &value);
	virtual operator AosValueRslt ();
};

#endif

