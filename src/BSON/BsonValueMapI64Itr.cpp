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
#include "BSON/BsonValueMapI64Itr.h"
#include "Util/VarUnInt.h"
//#include "BSON/BSON.h"

AosBsonValueMapI64Itr::AosBsonValueMapI64Itr(
		AosBSON  *mptr,
		AosBsonField::Type type,
		const i64 &name,
		AosBsonValueI64Map *mp,
		const AosValueRslt &Value)
:AosBsonValue(mptr, type, "", -1),
keyName(name),
mMap(mp),
mValue(Value)
{
}


AosBsonValueMapI64Itr::~AosBsonValueMapI64Itr()
{
   mMap = 0;
}


AosValueRslt &
AosBsonValueMapI64Itr::getValue()
{
   return mValue;
}


AosBsonValue &
AosBsonValueMapI64Itr::operator = (const AosValueRslt &value)
{
   mValue = value;
   mMap->setChanged(true);
   return *(AosBsonValue*)this;
}


AosBsonValue &
AosBsonValueMapI64Itr::operator = (const OmnString &value)
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
AosBsonValueMapI64Itr::operator = (const u64 &value)
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
AosBsonValueMapI64Itr::operator = (const i64 &value)
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
AosBsonValueMapI64Itr::operator = (const double value)
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


AosBsonValueMapI64Itr::operator AosValueRslt ()
{
   return mValue;
}


AosBsonValueMapI64Itr::operator u64 ()
{
   return mValue.getU64();
}


AosBsonValueMapI64Itr::operator i64 ()
{
   return mValue.getI64();
}


AosBsonValueMapI64Itr::operator OmnString ()
{
   return mValue.getStr();
}


AosBsonValueMapI64Itr::operator double ()
{
   return mValue.getDouble();
}



