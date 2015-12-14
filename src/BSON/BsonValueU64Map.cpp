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
#include "BSON/BsonValueU64Map.h"
#include "Util/VarUnInt.h"
#include "BSON/BSON.h"
#include "BSON/BsonValueMapU64Itr.h"
#include "BSON/BFMap.h"

AosBsonValueU64Map::AosBsonValueU64Map(
		AosBSON *mptr,
		OmnString &name,
		i64 pos,
		AosBsonField::Type type)
:
AosBsonValue(mptr, AosBsonField::eFieldTypeMap, name, pos),
mType(type),
mParsed(true),
mChanged(true)
{
	if (pos != -1)
	{
	    mParsed = false;
		mChanged = false;
	}
}


AosBsonValueU64Map::~AosBsonValueU64Map()
{
}

	
bool
AosBsonValueU64Map::parse()
{
	if (mPos == -1)
	{
		mParsed = true;
		return true;
	}
	//parse the buff into object map
	//now the pos has pointed to the begin of the mapObject's name in buff
	bool rslt;
	AosValueRslt vv;
	u64 len;
	AosBuff *mBuff = mBSON->getCrtBuff();
	char* data = mBuff->data();
	i64 pos = mPos;
	rslt = AosBsonField::skipEname(0, mBuff, pos);
	aos_assert_r(rslt, false);
	mBuff->setCrtIdx(pos);
	u32 numbers = mBuff->getU32(0); 
	pos += 4;  
	pos += 4;
	pos += 1; //skip the key type
	AosBsonField::Type valueType = static_cast<AosBsonField::Type>(data[pos]);
	pos++;
	for (unsigned int i = 0 ; i < numbers; i++)
	{
		u64 key = mBuff->getU64(0);
		pos += sizeof(u64);
        
		switch(valueType)
		{
			case AosBsonField::eFieldTypeString:
		    {
			    rslt = AosVarUnInt::decode(data, mBuff->dataLen(), pos, vv);
		        aos_assert_r(rslt, false);
		        len = vv.getU64();
		        OmnString tt(data+pos,len);
		        vv.setStr(tt);
		        pos+=len;
                break;
			}
			case AosBsonField::eFieldTypeU64:
			{
			    u64 value = mBuff->getU64(0);
				vv.setU64(value);
				break;
			}
			case AosBsonField::eFieldTypeInt64:
			{
			    i64 value = mBuff->getI64(0);
				vv.setI64(value);
				break;
			}
			case AosBsonField::eFieldTypeDouble:
			{
				double value = mBuff->getDouble(0);
			    vv.setDouble(value);
				break;
			}
			default:
                return false;
		}
		map<u64, AosBsonValueMapU64ItrPtr>::iterator itr;
		itr = mValuesItr.find(key);
		if(itr != mValuesItr.end())
		{
		   *(itr->second) =  vv;  
		}
		else
		{
		   mValuesItr[key] = OmnNew AosBsonValueMapU64Itr(mBSON, valueType, key, this, vv);
		}
	}
	mBuff->setCrtIdx(pos);
	mParsed = true;
    return true;
}


AosBsonValue & 
AosBsonValueU64Map::operator [] (const u64 &name)
{	
	if (!mParsed) parse();
	map<u64, AosBsonValueMapU64ItrPtr>::iterator itr;
	itr = mValuesItr.find(name);
	if(itr != mValuesItr.end()) 
	{
		return *((itr->second).getPtr());
	}
	else
	{
	    AosBsonValueMapU64ItrPtr itrPtr;
		itrPtr = OmnNew AosBsonValueMapU64Itr(mBSON, mType, name, this);
		mValuesItr[name] = itrPtr;  // for a short time
	    return *(itrPtr.getPtr());
    }
}


bool
AosBsonValueU64Map::insert(
		     u64 &name, 
		     AosValueRslt &value)
{
   if (!mParsed) parse();
   map<u64, AosBsonValueMapU64ItrPtr>::iterator itr = mValuesItr.find(name);
   if (itr != mValuesItr.end()) return false;
   
   AosBsonField::Type valueType = mBSON->getValueType(value);
   AosBsonValueMapU64ItrPtr itrPtr;
   if(mType != valueType) return false;
   itrPtr = OmnNew AosBsonValueMapU64Itr(mBSON, mType, name, this, value);
   mValuesItr[name] = itrPtr;
   //mPos = -1;
   mChanged = true;
   return true;
}


AosValueRslt 
AosBsonValueU64Map::getValue(const u64 &name)
{
   if (!mParsed) parse();
   map<u64, AosBsonValueMapU64ItrPtr>::iterator itr = mValuesItr.find(name);
   if(itr == mValuesItr.end()) 
   {
	   OmnThrowException("name don't exist");
	   return AosValueRslt();
   }
   AosBsonValueMapU64ItrPtr ptr = mValuesItr[name];
   return ptr->getValue();
}

bool
AosBsonValueU64Map::getBuff(AosBsonField **tsFields, AosBuff* buff_raw)
{
   bool rslt;	
   if (!mChanged && mPos != -1)
   {
	   return true;
   }
   map<AosValueRslt,AosValueRslt>  values;
   // style valueType + nameType + name + numbers + length + values
   // values = key + value
   // key = keyType + keyValue
   // value = valueType + valueData
   buff_raw->gotoEnd();
   rslt = buff_raw->setChar(AosBsonField::eFieldTypeMap);
   aos_assert_r(rslt , false);
   rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
   aos_assert_r(rslt , false);
   rslt = AosBsonField::setFieldNameStr(mStrName, buff_raw);
   aos_assert_r(rslt , false);
   buff_raw->setU32(mValuesItr.size()); //numbers of value
   i64 tempPos = buff_raw->getCrtIdx();
   buff_raw->setU32(0);  //for length
   //set value
   map<u64, AosBsonValueMapU64ItrPtr>::iterator itr = mValuesItr.begin();
   for(; itr != mValuesItr.end(); itr++)
   {
      u64 keyName = itr->first;
      AosValueRslt tempName;
	  tempName.setU64(keyName);
	  AosValueRslt tempValue = (itr->second)->getValue();
      values.insert(make_pair(tempName, tempValue));   
   }
   AosBFMap *field = dynamic_cast<AosBFMap*>(tsFields[AosBsonField::eFieldTypeMap]);
   aos_assert_r(field, false);
   
   field->setField(values, buff_raw);
   buff_raw->gotoEnd();
   i64 length = buff_raw->dataLen();
   rslt = buff_raw->setCrtIdx(tempPos);
   aos_assert_r(rslt , false);
   rslt = buff_raw->setU32(length - tempPos - 4 - 2);
   aos_assert_r(rslt , false);
   return true;
}



