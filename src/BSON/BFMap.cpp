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
// 2014/08/24 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFMap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFMap_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFMap(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		//AosLogError(rdata, false, "failed_creating_jimo") << enderr;
		OmnScreen << "failed_creating_jimo" << endl;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosBFMap::AosBFMap(const int version)
:
AosBsonField(version, eFieldTypeMap)
{
}


AosBFMap::~AosBFMap()
{
}


AosJimoPtr 
AosBFMap::cloneJimo() const
{
	return OmnNew AosBFMap(*this);
}


bool
AosBFMap::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
    return false;
}


//
//This method add length and data of the value
//to 'data' buffer. "pos" will be moved to the 
//end of 'data'. 
//
//'pos' is used for the caller to know the new
//data length
//
//Value is always in string format. It is up to
//the BSON field to parse the data into different
//types
//
bool
AosBFMap::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	return false;
}


bool 
AosBFMap::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	return false;
}


bool
AosBFMap::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeMap);
	return true;
}


// this function called by AosBsonValueMap to put map values into BSON buff 
bool
AosBFMap::setField(
		map<AosValueRslt, AosValueRslt> &values, 
		AosBuff *buff_raw)
{
	//this function only set key:value into the buff
	if(values.size() == 0) return true;
	bool rslt;
	map<AosValueRslt, AosValueRslt>::iterator itr;
	itr = values.begin();
	AosBsonField::Type keyType = getValueType(itr->first);
	AosBsonField::Type valueType = getValueType(itr->second);
	buff_raw->gotoEnd();
	buff_raw->setChar(keyType);
	buff_raw->setChar(valueType);
	for( ; itr != values.end(); itr++)
	{
	    buff_raw->gotoEnd();
	    //set name as string'varuint(length) + 'string'(contents)'
	    AosValueRslt key = itr->first;
		switch(keyType)
		{
		case AosBsonField::eFieldTypeString:
		     {
				OmnString name = key.getStr();  
		        rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	            aos_assert_r(rslt , false);
			    break;
			 }

		case AosBsonField::eFieldTypeU64:
			 {
				u64 name = key.getU64();
				rslt = buff_raw->setU64(name);
				aos_assert_r(rslt , false);
				break;
			 }

		case AosBsonField::eFieldTypeInt64: 
             {
			    i64 name = key.getI64();
				rslt = buff_raw->setI64(name);
				aos_assert_r(rslt , false);
				break;
			 }
		default:
		     return false;
		}
		buff_raw->gotoEnd();
		AosValueRslt data = itr->second;
		switch(valueType)
		{
		case AosBsonField::eFieldTypeString:
			 {
			    OmnString value = data.getStr();  
				rslt = AosBsonField::setFieldNameStr(value, buff_raw);
				aos_assert_r(rslt , false);
				break;
			 }

		case AosBsonField::eFieldTypeU64:
		     {
			    u64 value = data.getU64();
				rslt = buff_raw->setU64(value);
				aos_assert_r(rslt , false);
				break;
			 }

		case AosBsonField::eFieldTypeInt64: 
			 {
				i64 value = data.getI64();
				rslt = buff_raw->setI64(value);
				aos_assert_r(rslt , false);
				break;
			 }

		case AosBsonField::eFieldTypeDouble: 
             {
                double value = data.getDouble();
                rslt = buff_raw->setDouble(value);
                aos_assert_r(rslt , false);
                break;
             } 
		default:
		     return false;
		}
		//set value as varuint
		buff_raw->gotoEnd();
	}
	return true;
}

/*
bool 
AosBFMap::setField(
		const u32 name, 
		const i8 value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03(u8) + fieldname(varuint) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeInt8);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	// Set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	
	// Set value as varuint
	buff_raw->gotoEnd();
	buff_raw->append(char(value));
	return true;
}
*/

AosBsonField::Type                                                       
AosBFMap::getValueType(AosValueRslt value)                             
{                                                                        
    switch (value.getType())                                             
    {                                                                    
    case AosDataType::eFloat:   return AosBsonField::eFieldTypeFloat;    
                                                                         
    case AosDataType::eDouble:  return AosBsonField::eFieldTypeDouble;   
                                                                         
                                                                         
    case AosDataType::eString:  return AosBsonField::eFieldTypeString;   
                                                                         
    //case AosDataType::eNull:    return AosBsonField::eFieldTypeNull;   
                                                                         
    case AosDataType::eU8:      return AosBsonField::eFieldTypeU8;       
                                                                         
    case AosDataType::eU16:     return AosBsonField::eFieldTypeU16;      
                                                                         
    case AosDataType::eU32:     return AosBsonField::eFieldTypeU32;      
                                                                         
    case AosDataType::eU64:     return AosBsonField::eFieldTypeU64;      
                                                                         
    case AosDataType::eInt8:    return AosBsonField::eFieldTypeInt8;     
                                                                         
    case AosDataType::eInt16:   return AosBsonField::eFieldTypeInt16;    
                                                                         
    case AosDataType::eInt32:   return AosBsonField::eFieldTypeInt32;    
                                                                         
    case AosDataType::eInt64:   return AosBsonField::eFieldTypeInt64;    
                                                                         
    case AosDataType::eChar:    return AosBsonField::eFieldTypeChar;     
                                                                         
    case AosDataType::eVarInt:  return AosBsonField::eFieldTypeVarInt;   
                                                                         
    case AosDataType::eVarUint: return AosBsonField::eFieldTypeVarUint;  
                                                                         
    default:                    return AosBsonField::eInvalidFieldType;  
    }                                                                    
}                                                                        
