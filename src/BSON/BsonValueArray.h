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
// 2015/02/01 Created by Rain
////////////////////////////////////////////////////////////////////////////
//
#ifndef Aos_BSON_BsonValueArray_h
#define Aos_BSON_BsonValueArray_h

#include "BSON/BsonField.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "BSON/Ptrs.h"

#include "BSON/BsonValue.h"
class AosBsonValueArray;


class AosBsonValueArrayItr : virtual public AosBsonValue
{
protected:
    AosBsonValueArray  *mArray;
    AosValueRslt       mValue;

public:                                                         
    AosBsonValueArrayItr(
            AosBSON  *mptr,
            AosBsonField::Type type,
			const OmnString &name,
            AosBsonValueArray *mp, 
            const AosValueRslt &Value);
    
    ~AosBsonValueArrayItr();

    AosValueRslt & getValue();

    virtual AosBsonValue & operator = (const AosValueRslt &value); 

    virtual operator OmnString ();
};

class AosBsonValueArray : virtual public AosBsonValue
{
	vector<AosBsonValueArrayItrPtr>	 mValues;
    AosBsonField::Type mType;

	//AosBsonField::Type  mType;  //element type
public:
	//normal Array
    AosBsonValueArray(
			AosBSON *ptr, 
			AosBsonField::Type type, 
			OmnString &name, 
			AosBsonField::Type dataType,
			i64 pos);

	//Generic Array
    AosBsonValueArray(
			AosBSON *ptr, 
			AosBsonField::Type type, 
			OmnString &name, 
            i64 pos);

    ~AosBsonValueArray();
    virtual AosValueRslt getValue(const i32 idx);
    
//	virtual bool setValue(vector<AosValueRslt> &values);
	bool isNull() const {return !mValues.size();}
	bool isArray() const {return true;}
    virtual AosBsonValue & operator [] (const u32 idx);
    virtual bool parse(); 

	virtual bool getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw);
private:
	bool parseArray(
		    AosBsonField::Type type,
			AosBuff*   mBuff,
			i64  &pos);

/*	bool getNormalValue(
			AosRundata  *rdata,
			AosBsonField** &tsFields, 
			OmnString  &name,
			AosBuff* &mBuffRaw);

    bool getGenericValue(
			AosRundata  *rdata,
			AosBsonField** &tsFields,
			OmnString  &name,
			AosBuff* &mBuffRaw);
*/
    AosBsonField::Type getType(AosValueRslt value);

};


#endif

