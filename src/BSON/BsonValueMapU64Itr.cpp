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
#include "BSON/BsonValueMapU64Itr.h"
#include "Util/VarUnInt.h"
//#include "BSON/BSON.h"

AosBsonValueMapU64Itr::AosBsonValueMapU64Itr(
		AosBSON  *mptr,
		AosBsonField::Type type,
		const u64 &name,
		AosBsonValueU64Map *mp,
		const AosValueRslt &Value)
:AosBsonValue(mptr, type, "", -1),
keyName(name),
mMap(mp),
mValue(Value)
{
}


AosBsonValueMapU64Itr::~AosBsonValueMapU64Itr()
{
   mMap = 0;
}


AosValueRslt &
AosBsonValueMapU64Itr::getValue()
{
   return mValue;
}


AosBsonValue &
AosBsonValueMapU64Itr::operator = (const AosValueRslt &value)
{
   mValue = value;
   mMap->setChanged(true);
   return *(AosBsonValue*)this;
}


AosBsonValueMapU64Itr::operator AosValueRslt ()
{
   return mValue;
}


AosBsonValue &
AosBsonValueMapU64Itr::operator = (const OmnString &value)
{
   if(mValueType == AosBsonField::eFieldTypeString)
   {
       mValue = AosValueRslt(value);
       mMap->setChanged(true);
       return *(AosBsonValue*)this;
   }
   OmnThrowException("wrong value type");
   return *this;                          
}


AosBsonValue &
AosBsonValueMapU64Itr::operator = (const u64 &value)
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
AosBsonValueMapU64Itr::operator = (const i64 &value)
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
AosBsonValueMapU64Itr::operator = (const double value)
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


AosBsonValueMapU64Itr::operator u64 ()
{
   return mValue.getU64();
}


AosBsonValueMapU64Itr::operator i64 ()
{
   return mValue.getI64();
}


AosBsonValueMapU64Itr::operator OmnString ()
{
   return mValue.getStr();
}


AosBsonValueMapU64Itr::operator double ()
{
   return mValue.getDouble();
}



