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
// 2015/01/28 Created by Rain
////////////////////////////////////////////////////////////////////////////
//
#ifndef Aos_BSON_BsonValue_h
#define Aos_BSON_BsonValue_h

#include "BSON/BsonField.h"
#include "BSON/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"

class AosBSON;

class AosBsonValue : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	enum                         
	{                            
	   eDefaultBuffSize = 5000, 
	};                           
    
	AosBSON*	   		mBSON;
	AosBsonField::Type	mValueType;	
	u32					mU32Name;
	OmnString           mStrName;
	i64					mPos; //use this pos to find normal value

public:
	AosBsonValue(
			AosBSON* Ptr, 
			const AosBsonField::Type type,
			const u32 name,
			const i64 &pos);
	
	AosBsonValue(
			AosBSON* Ptr, 
			const AosBsonField::Type type,
			const OmnString &name,
			const i64 &pos);
	
	virtual ~AosBsonValue();

  //Int asInt() const;
  //  UInt asUInt() const;
  //  double asDouble() const;
  //  bool asBool() const;
  //	string asString() const;

  //  bool isNull() const;
  //  bool isBool() const;
  //  bool isInt() const;
  //  bool isUInt() const;
  //  bool isDouble() const;
  //  bool isNumeric() const;
  //  bool isString() const;
  //  bool isArray() const;
  //  bool isObject() const;

 	virtual AosBsonValue & operator = (const u64 &value);
 	virtual AosBsonValue & operator = (const OmnString &value);
 	virtual AosBsonValue & operator = (const i64 &value);
 	virtual AosBsonValue & operator = (const double value);
	virtual AosBsonValue & operator = (const AosValueRslt &value);
     
    virtual AosBsonValue & operator [] (const OmnString &name); //OmnString map
    virtual AosBsonValue & operator [] (const u64 &name); //u64 map
    virtual AosBsonValue & operator [] (const i64 &name); //i64 map
    virtual AosBsonValue & operator [] (const u32 idx); // Array

    //setValue
    AosBsonValue & setValue (const u64 &value);
	AosBsonValue & setValue (const OmnString &value);
    AosBsonValue & setValue (const i64 &value);
    AosBsonValue & setValue (const double value);

	//getValue
    virtual AosValueRslt getValue(const OmnString &name); //override by map
    virtual AosValueRslt getValue(const u64 &name); //override by map
    virtual AosValueRslt getValue(const i64 &name); //override by map
    virtual AosValueRslt getValue(const i32 idx); //override by array

	virtual bool insert(OmnString &name, AosValueRslt &value);
    virtual bool insert(u64 &name, AosValueRslt &value);
    virtual bool insert(i64 &name, AosValueRslt &value);
    
 	//operator u64 ();
 	virtual operator OmnString ();
 	virtual operator u64 ();
 	virtual operator i64 ();
 	virtual operator double ();
	virtual operator AosValueRslt ();

	virtual bool parse();
	virtual bool getBuff(AosBsonField **tsFields, AosBuff* mBuffRaw);
	inline i64 getPos(){return mPos;}
    inline void setPos(i64 &pos){mPos = pos;} 
    inline AosBsonField::Type getValueType(){return mValueType;}
};

#endif

