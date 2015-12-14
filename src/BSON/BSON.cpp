////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BSON/BSON.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "BSON/BsonField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/VarUnInt.h"
#include "UtilComm/ConnBuff.h"

#include "BSON/BsonValue.h"
#include "BSON/BsonValueStrMap.h"
#include "BSON/BsonValueU64Map.h"
#include "BSON/BsonValueI64Map.h"
//#include "BSON/BsonValueMapItr.h"
//#include "BSON/BsonValueNull.h"
//#include "BSON/BsonValueArray.h"



//__thread AosBsonField **AosBSON::tsFields = 0;
AosBsonField **AosBSON::tsFields = 0;

//this constructor is used to build the BSON doc
//from the scratch
AosBSON::AosBSON()
:
mBuff(0),
mBuffRaw(0),
mDocLength(0),
mDefaultBuffSize(eDefaultBuffSize),
mParsed(false),
mIsGood(true),
mDealt(0),
mChanged(false)
{
	//the length can be defined in header file later on
	mBuff = OmnNew AosBuff(mDefaultBuffSize AosMemoryCheckerArgs);
	mBuffRaw = mBuff.getPtr();
	//init the BSON Document Length
	mBuffRaw->setU32(mDocLength);
	init();
}


//This constructor suppose data is from an buffer
//which need to be parsed firstly
/*
AosBSON::AosBSON(const AosBuffPtr &buff)
:
mBuff(buff),
mBuffRaw(buff.getPtr()),
mDocLength(buff->dataLen()),
mDealt(0)
{
	init();
}
*/

AosBSON::AosBSON(const AosBuffPtr &buff)
:
mBuff(buff),
mBuffRaw(buff.getPtr()),
mDocLength(buff->dataLen() - sizeof(u32)),
mDealt(0)
{
	init();
    build();
}

AosBSON::~AosBSON()
{
	mMap.clear();
	mU32Map.clear();
	//mDocMap.clear();
}


bool
AosBSON::init()
{
	if (!tsFields)
	{
		tsFields = AosBsonField::initFields();
	}
	//mDefaultBuffSize = eDefaultBuffSize;
	mMap.clear();
	mU32Map.clear();
	//mDocMap.clear();
	mParsed = false;
	return true;
}


void
AosBSON::setConnBuff(const OmnConnBuffPtr &buff)
{
	mConnBuff = buff;
	mBuff = OmnNew AosBuff(buff AosMemoryCheckerArgs);
	mBuffRaw = mBuff.getPtrNoLock();
	mDocLength = mBuff->dataLen() - sizeof(u32);
	init();
	build();
}


void
AosBSON::setBuff(const AosBuffPtr &buff)
{
	mBuff = buff;
	mBuffRaw = mBuff.getPtrNoLock();
	init();
	build();

	mBuffRaw->reset();
	mDocLength = mBuffRaw->getU32(0);
	mBuffRaw->incIndex(mDocLength);
}


AosBuffPtr
AosBSON::getBuff()
{
   bool rslt;
   if(!mChanged) return mBuff;
   // we assume that all special type is string
   itr_t itr = mMap.begin();
   for(; itr != mMap.end(); itr++)
   {
      AosBsonValuePtr mPtr = itr->second;
	  //if(mPtr->getPos() != -1) continue; 
	  switch(mPtr->getValueType())
	  {
		  case AosBsonField::eFieldTypeMap:
			   {
	               mBuffRaw->gotoEnd();
	               rslt = mPtr->getBuff(tsFields, mBuffRaw);
                   if(!rslt) return NULL;
				   break;
			   }
		  default:
			   break;
	  }
   }
   rslt = resetBuffLength();
   aos_assert_r(rslt, 0);
   return mBuff;
}


bool
AosBSON::clear()
{                                            
    // 1. Set Doc Length = 0;
    // 2. Add a 0x00 after length (u32) ????
    mMap.clear();
    mU32Map.clear();
    //mDocMap.clear();
    mParsed = false;

    mBuff->reset();
    mBuff->setU32(0);
    //mBuff->setChar(0);
    //mBuff->setDataLen(5);
    
    mBuff->setDataLen(4);
    mDocLength = 0;
    return true;
}


AosBsonField::Type
AosBSON::getValueType(AosValueRslt value)
{
	switch (value.getType())
	{
	case AosDataType::eFloat:	return AosBsonField::eFieldTypeFloat;
	
	case AosDataType::eDouble:	return AosBsonField::eFieldTypeDouble;

	case AosDataType::eString:	return AosBsonField::eFieldTypeString;

	case AosDataType::eNull: 	return AosBsonField::eFieldTypeNull;

	case AosDataType::eU8: 	    return AosBsonField::eFieldTypeU8;
	
	case AosDataType::eU16: 	return AosBsonField::eFieldTypeU16;
	
	case AosDataType::eU32: 	return AosBsonField::eFieldTypeU32;

	case AosDataType::eU64:		return AosBsonField::eFieldTypeU64;

	case AosDataType::eInt8: 	return AosBsonField::eFieldTypeInt8;
	
	case AosDataType::eInt16: 	return AosBsonField::eFieldTypeInt16;
	
	case AosDataType::eInt32: 	return AosBsonField::eFieldTypeInt32;

	case AosDataType::eInt64: 	return AosBsonField::eFieldTypeInt64;

	case AosDataType::eChar: 	return AosBsonField::eFieldTypeChar;
	
	case AosDataType::eVarInt: 	return AosBsonField::eFieldTypeVarInt;
	
	case AosDataType::eVarUint: return AosBsonField::eFieldTypeVarUint;
	
	default:					return AosBsonField::eInvalidFieldType;
	}
}


bool 
AosBSON::setValue(const u32 name, const u64 &value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU64]; 
	//AosBsonField *field = tsFields[AosBsonField::eFieldTypeVarUint]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u64"
			<< ". Field Name: " << (int)name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU64, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const u32 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU32]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u32"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU32, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const u16 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU16]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u16"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU16, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const u8 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU8]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u8"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU8, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const i8 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt8]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i8"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt8, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const i16 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt16]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i16"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt16, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const i32 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt32]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i32"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt32, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const i64 &value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt64]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i64"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt64, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const float value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeFloat]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: float"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeFloat, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const double value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeDouble]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: double"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeDouble, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const bool value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeBool]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: bool"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeBool, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}

	
bool 
AosBSON::setValue(const u32 name)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeNull]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: NULL"
			<< ". Field Name: " << name << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(pos < 4)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeNull, name, pos); 
	bool rslt = field->setField(name, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const AosBuff* value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeBinary]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: NULL"
			<< ". Field Name: " << name << enderr;
		return false;
	}
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeBinary, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mU32Map[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const char *data, const int len)
{
	//change this to use AosBuff, by White 2015-11-03 17:08:33
	AosBuff buff(const_cast<char*>(data), len + 10, len, true AosMemoryCheckerArgs);	//MUST set copy flag to true!
					//FIXME:this const_cast is dangerous
	return setValue(name, &buff);
//	OmnString ss(data, len);
//	AosBsonField *field = tsFields[AosBsonField::eFieldTypeString];
//	if (!field)
//	{
//		OmnAlarm << "invalid_bson_field"
//			<< "Field Type: NULL"
//			<< ". Field Name: " << name << enderr;
//		return false;
//	}
//    u32itr_t itr = mU32Map.find(name);
//	if(itr != mU32Map.end())
//	{
//	   return false;
//	}
//
//	aos_assert_r(mBuffRaw, false);
//	mBuffRaw->gotoEnd();
//	u32 pos = getCrtPos();
//	if(!pos)
//	{
//	   return false;
//	}
//	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeString, name, pos);
//	bool rslt = field->setField(name, ss, mBuffRaw);
//	aos_assert_r(rslt, false);
//    mU32Map[name] = mPtr;
//	rslt = resetBuffLength();
//	aos_assert_r(rslt, false);
//    mChanged = true;
//	return true;
}


//Add new data to the doc
//The data in mBuff is like follows
//  |\x12\x00\x00\x00|\x01|\x05\x00\x00\x00|hello|\x05\x00\x00\x00|world
//  19=1+4+5+4+5(4B)  type 5(4B)                  5(4B)          
bool 
AosBSON::setValue(const u32 name, const OmnString &value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeString]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: string"
			<< ". Field Name: " << (int)name
			<< ". Field value: " << value << enderr;
		return false;
	}
    
    u32itr_t itr = mU32Map.find(name);
	if(itr != mU32Map.end())
	{
	   return false;
	}
	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
    AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeString, name, pos);
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
	mU32Map[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


//this function called by AosBsonValue
bool 
AosBSON::setValue(const u32 name, const u64 &value, i64 &pos)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU64]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u64"
			<< ". Field Name: " << (int)name
			<< ". Field value: " << value << enderr;
		return false;
	}
	aos_assert_r(mBuffRaw, false);

	//append the value into the end of buff even if the name exist
	mBuffRaw->gotoEnd();
	pos = getCrtPos();
	if (pos < 0)
	{
	   return false;
	}

	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


//this function called by AosBsonValue
bool 
AosBSON::setValue(const OmnString &name, const u64 &value, i64 &pos)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU64]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u64"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
	aos_assert_r(mBuffRaw, false);

	//append the value into the end of buff even if the name exist
	mBuffRaw->gotoEnd();
	pos = getCrtPos();
	if (pos < 0)
	{
	   return false;
	}

	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const u32 name, const OmnString &value, i64 &pos)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeString]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: String"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
	aos_assert_r(mBuffRaw, false);

	//append the value into the end of buff even if the name exist
	mBuffRaw->gotoEnd();
	pos = getCrtPos();
	if (pos < 0)
	{
	   return false;
	}

	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const OmnString &value, i64 &pos)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeString]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: String"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
	aos_assert_r(mBuffRaw, false);

	//append the value into the end of buff even if the name exist
	mBuffRaw->gotoEnd();
	pos = getCrtPos();
	if (pos < 0)
	{
	   return false;
	}

	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const u64 &value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU64]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u64"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU64, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const u32 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU32]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u64"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU32, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const u16 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU16]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u16"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU16, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const u8 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeU8]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u8"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeU8, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const i8 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt8]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i8"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt8, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}

	
bool 
AosBSON::setValue(const OmnString &name, const i16 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt16]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i16"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt16, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}
	
	
bool 
AosBSON::setValue(const OmnString &name, const i32 value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt32]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i32"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt32, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}
	

bool 
AosBSON::setValue(const OmnString &name, const i64 &value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeInt64]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: i64"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeInt64, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}
	

bool 
AosBSON::setValue(const OmnString &name, const float value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeFloat]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: float"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeFloat, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const double value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeDouble]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: double"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeDouble, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(
		const OmnString &name, 
		const OmnString &value)
{
	bool rslt;
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeString]; 
	aos_assert_r(field, false);
    
	itr_t itr = mMap.find(name);
	if(itr != mMap.end())
	{
	   return false;
	}
	// appendField name, value to buff
	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
    AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeString, name, pos);
	if(!mPtr) return false; 
    rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const bool value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeBool]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: bool"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeBool, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const char value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeChar]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: char"
			<< ". Field Name: " << name
			<< ". Field value: " << value << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeChar, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString& name, const char *data, const int len)
{
	OmnString ss(data, len);
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeString]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: NULL"
			<< ". Field Name: " << name << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeString, name, pos); 
	bool rslt = field->setField(name, ss, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 
	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeNull]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: NULL"
			<< ". Field Name: " << name << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeNull, name, pos); 
	bool rslt = field->setField(name, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}


bool 
AosBSON::setValue(const OmnString &name, const AosBuff* value)
{
	AosBsonField *field = tsFields[AosBsonField::eFieldTypeBinary]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: Binary"
			<< ". Field Name: " << name << enderr;
		return false;
	}
    itr_t itr = mMap.find(name);
	if(itr != mMap.end()) 
	{
	   return false;
	}

	aos_assert_r(mBuffRaw, false);
	mBuffRaw->gotoEnd();
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeBinary, name, pos); 
	bool rslt = field->setField(name, value, mBuffRaw);
	aos_assert_r(rslt, false);
    mMap[name] = mPtr; 

	rslt = resetBuffLength();
	aos_assert_r(rslt, false);
    mChanged = true;
	return true;
}



bool 
AosBSON::setValue(
		const OmnString &name,
		const AosValueRslt &value)
{
    bool rslt;
	AosBsonField::Type type = getValueType(value);
	AosBsonField *field = tsFields[type]; 
	if (!field)
	{
		OmnAlarm << "invalid_bson_field"
			<< "Field Type: u64"
			<< ". Field Name: " << name << enderr;
		return false;
	}

	aos_assert_r(mBuffRaw, false);
	u32 pos = getCrtPos();
	if(!pos)
	{
	   return false;
	}
	switch(type)
	{
	case  AosBsonField::eFieldTypeFloat:
		  {
		      float vv = value.getDouble();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
		      break;
	      }

	case  AosBsonField::eFieldTypeDouble:
		  {
		      double vv = value.getDouble();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;
          }

	case  AosBsonField::eFieldTypeString:
		  {
		      OmnString vv = value.getStr();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
		      break;
          }
	
	case  AosBsonField::eFieldTypeNull:
          {
	          rslt = setValue(name);
	          aos_assert_r(rslt, false);
	          return true;
	      }    

	case  AosBsonField::eFieldTypeU8:
		  {
		      u8 vv = value.getU64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
              break;
		  }

	case  AosBsonField::eFieldTypeU16:
		  {
		      u16 vv = value.getU64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;
		  }

	case  AosBsonField::eFieldTypeU32:
		  {
		      u32 vv = value.getU64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;
		  }

	case  AosBsonField::eFieldTypeU64:
		  {
		      u64 vv = value.getU64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;    
		  }

	case  AosBsonField::eFieldTypeInt8:
		  {
		      i8 vv = value.getI64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;
		  }

	case  AosBsonField::eFieldTypeInt16:
		  {
		      i16 vv = value.getI64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;
		  }

	case  AosBsonField::eFieldTypeInt32:
		  {
		      i32 vv = value.getI64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
	          break;
		  }

	case  AosBsonField::eFieldTypeInt64:
	      {      
	          i64 vv = value.getI64();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
		      break;
		  }

	case  AosBsonField::eFieldTypeChar:
	      {
	          char vv = value.getChar();
	          rslt = setValue(name, vv);
	          aos_assert_r(rslt, false);
		      break;
		  }
    /*
	case  AosBsonField::eFieldTypeVarInt:
		  {
		      i64 vv = value.getInt64Value(rdata);
	          rslt = field->setField(name, vv, mBuffRaw);
	          aos_assert_r(rslt, false);
	          break;
		  }

	case  AosBsonField::eFieldTypeVarUint:
		  {
		      u64 vv = value.getU64Value(0);
	          rslt = field->setField(name, vv, mBuffRaw);
	          aos_assert_r(rslt, false);
	          break;
		  } */
    default: 
		  return false;
	}
	return true;
}

OmnString
AosBSON::getOmnStr(
		const u32 name,
		const OmnString &def)
{
	AosValueRslt value;
	OmnString returnValue;
	u32itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getStr();
	if (returnValue == "") return def;
	return returnValue;
}


OmnString
AosBSON::getOmnStr(
		const OmnString &name,
		const OmnString &def)
{
	AosValueRslt value;
	OmnString returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getStr();
	if (returnValue == "") return def;
	return returnValue;
}


u32
AosBSON::getU32(
		const OmnString &name,
		const u32 &def)
{
	AosValueRslt value;
	u32 returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}

u32
AosBSON::getU32(
		const u32 name,
		const u32 &def)
{
	AosValueRslt value;
	u32 returnValue;
	u32itr_t itr;
	
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}

u64
AosBSON::getU64(
		const OmnString &name,
		const u64 &def)
{
	AosValueRslt value;
	u64 returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}

u64
AosBSON::getU64(
		const u32 name,
		const u64 &def)
{
	AosValueRslt value;
	u64 returnValue;
	u32itr_t itr;
	
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}

int
AosBSON::getInt(
		const OmnString &name,
		const int &def)
{
	AosValueRslt value;
	int returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}

int
AosBSON::getInt(
		const u32 name,
		const int &def)
{
	AosValueRslt value;
	int returnValue;
	u32itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}


i64
AosBSON::getI64(
		const OmnString &name,
		const i64 &def)
{
	AosValueRslt value;
	i64 returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getI64();
	return returnValue;
}

i64
AosBSON::getI64(
		const u32 name,
		const i64 &def)
{
	AosValueRslt value;
	i64 returnValue;
	u32itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getI64();
	return returnValue;
}


bool
AosBSON::getBool(
		const u32 name,
		const bool &def)
{
	AosValueRslt value;
	bool returnValue;
	u32itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}

bool
AosBSON::getBool(
		const OmnString &name,
		const bool &def)
{
	AosValueRslt value;
	bool returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getU64();
	return returnValue;
}


AosBuffPtr
AosBSON::getBuff(const u32 name, const AosBuffPtr &def)
{
	AosValueRslt value;
	AosBuffPtr returnValue;
	u32itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getBuff();
	return returnValue;
}


AosBuffPtr
AosBSON::getBuff(const OmnString &name, const AosBuffPtr &def)
{
	AosValueRslt value;
	AosBuffPtr returnValue;
	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return def;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return def; 
    }
	//mean name exist
	AosBSON::getValue(name,value);
   	returnValue = value.getBuff();

	return returnValue;
}


//u32 name ,there is no special type
bool
AosBSON::getValue(
		const u32 name,
		AosValueRslt &value)
{
	value.setNull();

	u32itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return false;
	}
	
	itr = mU32Map.find(name);
	if(itr == mU32Map.end()) 
	{
		return false; 
    }
	//find the name.
	AosBsonValuePtr mPtr  = itr->second;
	i64 pos = mPtr->getPos();
	char *data = mBuffRaw->data();
	AosBsonField * field = tsFields[(u8)data[pos]]; 
	if (!field)
	{
		AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		AosLogError(rdata, true, "invalid_bson_field") << (u8)data[pos] 
			<< ". Position: " << pos << ". Name: " << name << enderr;
		return false;
	}
	return field->getFieldValue(0, mBuffRaw, pos, true, value);
}


bool
AosBSON::getValue(
		const OmnString &name,
		AosValueRslt &value)
{
	value.setNull();

	itr_t itr;
	if (!mBuffRaw || mDocLength == 0) 
	{
	    return false;
	}
	
	itr = mMap.find(name);
	if(itr == mMap.end()) 
	{
		return false; 
    }
	//find the name.
	AosBsonValuePtr mPtr  = itr->second;
	i64 pos = mPtr->getPos();
	char *data = mBuffRaw->data();
	AosBsonField * field = tsFields[(u8)data[pos]]; 
	if (!field)
	{
	    AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		AosLogError(rdata, true, "invalid_bson_field") << (u8)data[pos] 
			<< ". Position: " << pos << ". Name: " << name << enderr;
		return false;
	}
	return field->getFieldValue(0, mBuffRaw, pos, true, value);
}

//"operator" operate
AosBsonValue & 
AosBSON::operator [] (const OmnString &name)
{
   //this [] only for special type
   itr_t  itr = mMap.find(name);
   if (itr != mMap.end())
   {
      AosBsonValuePtr mp = itr->second;
	  return *(mp.getPtr());   
   }
		
   AosBsonValuePtr mp = OmnNew AosBsonValue(this, AosBsonField::eFieldTypeNull, name, -1);
   mMap[name] = mp;
   return *(mp.getPtr());   
}


//the keyType of this map is AosBsonField::Type
bool 
AosBSON::createMap(OmnString &name, AosBsonField::Type keyType, AosBsonField::Type valueType)
{
   itr_t itr = mMap.find(name);
   if(itr != mMap.end()) return false;
   AosBsonValuePtr mPtr;
   switch(keyType)
   {
	  case AosBsonField::eFieldTypeString:
		   {
		       mPtr = OmnNew AosBsonValueStrMap(this, name, (i64)(-1), valueType);
               break;
		   }
	  case AosBsonField::eFieldTypeU64:
		   {
		       mPtr = OmnNew AosBsonValueU64Map(this, name, (i64)(-1), valueType);
		       break;
		   }
	  case AosBsonField::eFieldTypeInt64:
		   {
		       mPtr = OmnNew AosBsonValueI64Map(this, name, (i64)(-1), valueType);
		       break;
		   }
	  default:
		   return false;
   }
   if(!mPtr) return false;
   mMap[name] = mPtr;
   mChanged = true;
   return true;
}


u32
AosBSON::getCrtPos()
{
   mBuffRaw->gotoEnd();
   i64 pos = mBuffRaw->getCrtIdx(); 
   if(pos < 4)
   {
     return 0;
   }
   return (u32)pos; 
}

	
bool
AosBSON::resetBuffLength()
{
	mDocLength = mBuffRaw->dataLen() - sizeof(u32);
	mBuffRaw->reset();
	mBuffRaw->setU32(mDocLength);
	mParsed = false;
	return true;
}

bool 
AosBSON::build()
{
	aos_assert_r(mBuffRaw, false);

	i64 pos = sizeof(u32);
	bool rslt;
	itr_t itr;
	u32itr_t u32itr;

	//get the docLength
	mBuffRaw->reset();
	mDocLength = mBuffRaw->getU32(0);
	if(!mDocLength) return true;
//	aos_assert_r(mDocLength, false);
	char *data = mBuffRaw->data();

	mMap.clear();
	mU32Map.clear();
	while (pos < i64(mDocLength + sizeof(u32)))
	{
		i64 tempPos = pos;
		//get the field
		AosBsonField* field = getField(data[pos]);
		aos_assert_r(field, false);
        AosBsonField::Type valueType = field->getFieldType();
		pos++;
		if (data[pos] == 0x01) //string use 0x01
		{
			//name type is 'string'
		    pos++;
			OmnString name;
			rslt = field->getName(0, mBuffRaw, name, pos);
			//aos_assert_rr(rslt, rdata, false);
            aos_assert_r(rslt, false);

			itr = mMap.find(name);
			if (itr != mMap.end())
			{
			    //if the field has existed ,wo cover it
				switch(valueType)                                                                
				{
					case AosBsonField::eFieldTypeMap:
						 {
							 AosBsonValuePtr mPtr = itr->second;
							 mPtr->setPos(tempPos);
							 //mPtr->parse();
							 //pos = mBuffRaw->getCrtIdx();
						     pos += 4;
							 mBuffRaw->setCrtIdx(pos);         
							 i64 length = mBuffRaw->getU32(0);
							 pos = pos + 4 + length;
							 break;
						 }
					case AosBsonField::eFieldTypeDocument:
						 {
							 //should be implementd
						     break;
						 }
					default:
						 {
                             AosBsonValuePtr mPtr = itr->second;
                             mPtr->setPos(tempPos); 
	                         rslt = field->skipValue(0, mBuffRaw, pos);		
						 }
				}
			}
			else
			{
				//rslt = createBsonValue(this, valueType, name, tempPos, field);
			    //aos_assert_r(rslt, false);
				switch(valueType)                                                                
				{
					case AosBsonField::eFieldTypeMap:
						 {
							 pos += 8; 
							 if(data[pos] == AosBsonField::eFieldTypeString)
							 {
								  pos++;
								  AosBsonField::Type type = static_cast<AosBsonField::Type>(data[pos--]);
							      AosBsonValuePtr mPtr = OmnNew AosBsonValueStrMap(this, name, tempPos, type);
							      mMap[name] = mPtr;
							 }
							 else if(data[pos] == AosBsonField::eFieldTypeU64)
							 {
								  pos++;
								  AosBsonField::Type type = static_cast<AosBsonField::Type>(data[pos--]);
							      AosBsonValuePtr mPtr = OmnNew AosBsonValueU64Map(this, name, tempPos, type);
								  mMap[name] = mPtr;
							 }
							 else
							 {
								  AosBsonField::Type type = static_cast<AosBsonField::Type>(data[++pos]);
							      AosBsonValuePtr mPtr = OmnNew AosBsonValueI64Map(this, name, tempPos, type);
								  mMap[name] = mPtr;
								  pos--;
							 }
							 pos = pos - 4;
							 mBuffRaw->setCrtIdx(pos);
							 i64 length = mBuffRaw->getU32(0);
							 pos = pos + 4 + 2 + length;
							 break;
						 }
					case AosBsonField::eFieldTypeDocument:
						 {
							 //should be implementd
						     break;
						 }
					default:
						 {
							 AosBsonValuePtr mPtr = OmnNew AosBsonValue(this, valueType, name, tempPos);
							 mMap[name] = mPtr;
							// i64 crtPos = mBuffRaw->getCrtIdx();  
							 rslt = field->skipValue(0, mBuffRaw, pos);
							 aos_assert_r(rslt, false);
						 }
				}
			}
		}
		else if (data[pos] == 0x03) 
		{
			pos++;
			//name type is 'u32'
			u32 name;
			rslt = field->getName(0, mBuffRaw, name, pos);
			aos_assert_r(rslt, false);

			//skip the value 
			rslt = field->skipValue(0, mBuffRaw, pos);
			aos_assert_r(rslt, false);

			u32itr = mU32Map.find(name);
			if (u32itr != mU32Map.end())
			{
			   AosBsonValuePtr mPtr = itr->second;
			   mPtr->setPos(tempPos); 
			}
			else
			{
				mU32Map[name] = OmnNew AosBsonValue(this, valueType, name, tempPos);
			}
		}
		else
		{
			AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
			//invalid name type
			AosLogError(rdata, true, AosErrmsgId::eInvalidBSONFieldNameType) 
				<< "FieldNameType:" << data[pos] << enderr;
			return false;
		}
	}
	return true;
}


AosBsonField *
AosBSON::getField(const i8 id)
{
	bool rslt;
	rslt = AosBsonField::isValid(id);
	aos_assert_r(rslt, NULL);
	return tsFields[id];
}


bool
AosBSON::isMember(const OmnString &name)
{
    if (!mBuffRaw || mDocLength == 0) return false;
    itr_t itr = mMap.find(name); 
    if (itr == mMap.end())       
    {                            
	    return false;             
    }              
    return true;
}


bool
AosBSON::isMember(const u32 name)
{
	if (!mBuffRaw || mDocLength == 0) return false;
	u32itr_t itr = mU32Map.find(name); 
	if (itr == mU32Map.end())       
	{                            
	    return false;             
	}              
	return true;
}


bool
AosBSON::swap(AosBSON& rhs)
{
   if(!(rhs.mBuff) || !mBuff)                
      return false;                         
//swap the datas in buff                   
   AosBuffPtr tempBuff = rhs.mBuff;        
   rhs.mBuff = mBuff;                      
   mBuff = tempBuff;                       
// swap mBuffRaw                           
   AosBuff* tempBuffRaw = rhs.mBuffRaw;    
   rhs.mBuffRaw = mBuffRaw;                
   mBuffRaw = tempBuffRaw;                 
//swap Map                                 
    mMap.swap(rhs.mMap);                   
    mU32Map.swap(rhs.mU32Map);             
                                           
//swap mDocLength                          
   u32 len = rhs.mDocLength;               
   rhs.mDocLength = mDocLength;            
   mDocLength = len;                       
//swap mDefaultBuffSize                    
   len = rhs.mDefaultBuffSize;             
   rhs.mDefaultBuffSize = mDefaultBuffSize;
   mDefaultBuffSize = len;                 
//swap mParsed                             
   bool tempP = rhs.mParsed;               
   rhs.mParsed = mParsed;                  
   mParsed = tempP;                        
//swap mIsGood                             
   bool tempG = rhs.mIsGood;               
   rhs.mIsGood = mIsGood;                  
   mIsGood = tempG;                        
//swap mDealt                              
   i64 tempD = rhs.mDealt;                 
   rhs.mDealt = mDealt;                    
   mDealt = tempD;                         
//swap mChanged
   bool tempCH = rhs.mChanged;
   rhs.mChanged = mChanged;   
   mChanged = tempCH;         
//swap mConnBuff                           
   OmnConnBuffPtr tempCBuff = rhs.mConnBuff;
   rhs.mConnBuff = mConnBuff;               
   mConnBuff = tempCBuff;                   
   return true;                             
}
