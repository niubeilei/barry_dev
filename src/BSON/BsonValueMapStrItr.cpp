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
// 2014/08/17 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "BSON/BsonValueMapStrItr.h"
#include "Util/VarUnInt.h"
//#include "BSON/BSON.h"

AosBsonValueMapStrItr::AosBsonValueMapStrItr(
		AosBSON  *mptr,
		AosBsonField::Type type,
		const OmnString &name,
		AosBsonValueStrMap *mp,
		const AosValueRslt &Value)
:AosBsonValue(mptr, type, "", -1),
keyName(name),
mMap(mp),
mValue(Value)
{
}


AosBsonValueMapStrItr::~AosBsonValueMapStrItr()
{
   mMap = 0;
}


AosValueRslt &
AosBsonValueMapStrItr::getValue()
{
   return mValue;
}


AosBsonValue &
AosBsonValueMapStrItr::operator = (const AosValueRslt &value)
{
   mValue = value;
   mMap->setChanged(true);
   return *(AosBsonValue*)this;
}


AosBsonValue &
AosBsonValueMapStrItr::operator = (const OmnString &value)
{
   if(mValueType == AosBsonField::eFieldTypeString)
   {
       mValue = AosValueRslt(value);
       mMap->setChanged(true);
       return *(AosBsonValue*)this;
   }
   //error
   OmnThrowException("wrong value type");
   return *this;
}


AosBsonValue &
AosBsonValueMapStrItr::operator = (const u64 &value)
{
   if(mValueType == AosBsonField::eFieldTypeU64)
   {
       mValue = AosValueRslt(value);
       mMap->setChanged(true);
       return *(AosBsonValue*)this;
   }
   OmnThrowException("wrong value type");
   return *this;
}


AosBsonValue &
AosBsonValueMapStrItr::operator = (const i64 &value)
{
   if(mValueType == AosBsonField::eFieldTypeInt64)
   {
       mValue = AosValueRslt(value);
       mMap->setChanged(true);
	   return *(AosBsonValue*)this;
   }
   OmnThrowException("wrong value type");
   return *this;
}


AosBsonValue &
AosBsonValueMapStrItr::operator = (const double value)
{
   if(mValueType == AosBsonField::eFieldTypeDouble)
   {
       mValue = AosValueRslt(value);
       mMap->setChanged(true);
	   return *(AosBsonValue*)this;
   }
   OmnThrowException("wrong value type");
   return *this;
}


AosBsonValueMapStrItr::operator AosValueRslt ()
{
   return mValue;
}

AosBsonValueMapStrItr::operator u64 ()
{
   return mValue.getU64();
}


AosBsonValueMapStrItr::operator i64 ()
{
   return mValue.getI64();
}


AosBsonValueMapStrItr::operator OmnString ()
{
   return mValue.getStr();
}


AosBsonValueMapStrItr::operator double ()
{
   return mValue.getDouble();
}



