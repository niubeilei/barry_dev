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
// 2014/12/30 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"
#include "Util/VarInt.h"

#include "BSON/BFGenericArray.h"
#include "BSON/BSON.h"
#include "BSON/BsonField.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFGenericArray_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFGenericArray(version);
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


AosBFGenericArray::AosBFGenericArray(const int version)
:
AosBsonField(version, eFieldTypeGenericArray)
{
}


AosBFGenericArray::~AosBFGenericArray()
{
}


AosJimoPtr 
AosBFGenericArray::cloneJimo() const
{
	return OmnNew AosBFGenericArray(*this);
}
/*
bool 
AosBFGenericArray::createVector(
		AosRundata *rdata,
		const i64  idx,
		const i64  pos 
		)
{
   arrayPos.push_back(pos);
   return true;
}
*/
bool
AosBFGenericArray::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
	//	const int idx,
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// "\x34" e_name u32(number of entries) arrayLength  array(VarValue) 
	// pos have pointed to the value  value = dataType + value

	const char *data = buff->data();
	i64 buffLen = buff->dataLen();
    aos_assert_rr(buffLen >= 4, rdata, false);
	//get the dataType
	u8 data_type =(u8)data[pos++];
	switch (data_type)
	{
	//case 1:
	case AosBsonField::eFieldTypeU8:
		{
	        //skip one byte value type
			aos_assert_rr(pos+1 <= buffLen, rdata, false);
			u8 vv = (u8)data[pos++];
			//pos+=1;
			value.setU64(vv);
		}
		break;

	//case 2:
	case AosBsonField::eFieldTypeU16:
		{
			aos_assert_rr(pos+2 <= buffLen, rdata, false);
			u16 vv;
			vv = (((u16)data[pos]) &0xff) +
				((((u16)data[pos+1]) << 8) &0xffff);
			pos+= sizeof(u16);
			value.setU64(vv);
		}
		break;

	//case 3:
	case AosBsonField::eFieldTypeU32:
		{
			aos_assert_rr(pos+4 <= buffLen, rdata, false);
			u32 vv;
			vv = (data[pos]&0xff) +
				((data[pos+1] << 8)&0xffff) +
				((data[pos+2] << 16)&0xffffff) +
				((data[pos+3] << 24)&0xffffffff);
			pos+= sizeof(u32);
			value.setU64(vv);
		}
		break;
	
	//case 4:
	case AosBsonField::eFieldTypeU64:
		{
			aos_assert_rr(pos+8 <= buffLen, rdata, false);
			u64 vv;
			vv = (((u64)data[pos])&0xff )+
				((((u64)data[pos+1]) << 8) & 0xffff)+
				((((u64)data[pos+2]) << 16)& 0xffffff) +
				((((u64)data[pos+3]) << 24)& 0xffffffff) +
				((((u64)data[pos+4]) << 32)& 0xffffffffff)+
				((((u64)data[pos+5]) << 40)& 0xffffffffffff) +
				((((u64)data[pos+6]) << 48)& 0xffffffffffffff) +
				((((u64)data[pos+7]) << 56)& 0xffffffffffffffff); 
			pos+= sizeof(u64);
			value.setU64(vv);
		}
		break;
	
	//case 5:
	case AosBsonField::eFieldTypeInt8:
		{
			aos_assert_rr(pos+1 <= buffLen, rdata, false);
			//char vv;
			//vv = (char)data[pos++];
			i8 vv;
			vv = data[pos++];
			//pos+=1;
			value.setI64(vv);
		}
		break;
   
	case AosBsonField::eFieldTypeChar:
		{
			aos_assert_rr(pos+1 <= buffLen, rdata, false);
			char vv;
			vv = data[pos++];
			value.setChar(vv);
		}
		break;
    
	//case 6:
	case AosBsonField::eFieldTypeInt16:
		{
			aos_assert_rr(pos+2 <= buffLen, rdata, false);
			i16 vv;
			vv = (data[pos]&0xff)+
				((data[pos+1] << 8)&0xffff);
			pos+= sizeof(i16);
		    value.setI64(vv);
		}
		break;

	case AosBsonField::eFieldTypeInt32:
		{
			aos_assert_rr(pos+4 <= buffLen, rdata, false);
			i32 vv;
			vv = (data[pos]&0xff) +
				((data[pos+1] << 8)&0xffff) +
				((data[pos+2] << 16)&0xffffff) +
				((data[pos+3] << 24)&0xffffffff);
			pos+= sizeof(i32);
		    value.setI64(vv);
		}
		break;
	
	case AosBsonField::eFieldTypeInt64:
		{
			aos_assert_rr(pos+8 <= buffLen, rdata, false);
			i64 vv;
			vv = (((u64)data[pos])&0xff )+
				((((u64)data[pos+1]) << 8) & 0xffff)+
				((((u64)data[pos+2]) << 16)& 0xffffff) +
				((((u64)data[pos+3]) << 24)& 0xffffffff) +
				((((u64)data[pos+4]) << 32)& 0xffffffffff)+
				((((u64)data[pos+5]) << 40)& 0xffffffffffff) +
				((((u64)data[pos+6]) << 48)& 0xffffffffffffff) +
				((((u64)data[pos+7]) << 56)& 0xffffffffffffffff); 
			pos+= sizeof(i64);
			value.setI64(vv);
		}
		break;
	
	case AosBsonField::eFieldTypeFloat:
	    {
		   aos_assert_rr(pos+sizeof(float) <= (u64)buffLen, rdata, false );
		   float vv;                                                                                                                          
	       char *vv_mem = (char*)&vv;
		   memcpy(vv_mem, &data[pos], sizeof(float));
		   pos += sizeof(float);
		   value.setDouble(vv);
		}
		break;
	
	case AosBsonField::eFieldTypeDouble:
	    {
		   aos_assert_rr(pos+sizeof(double) <= (u64)buffLen, rdata, false );
		   double vv;                                                                                                                          
	       char *vv_mem = (char*)&vv;
		   memcpy(vv_mem, &data[pos], sizeof(double));
		   pos += sizeof(double);
		   value.setDouble(vv);
		}
		break;

    case AosBsonField::eFieldTypeString:
		{
           AosValueRslt vv;
		   bool rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);
		   aos_assert_rr(rslt, rdata, false);
           u64 len;
		   len = vv.getU64();
		   aos_assert_rr(rslt, rdata, false);
		   aos_assert_rr((pos+len) <= buff->dataLen(), rdata, false);
		   OmnString str_value(&data[pos], (int)len);
		   value.setStr(str_value);
		   pos += len;
		//   return true;
		}
		break;
	
	default:
		return false;
	}
	return true;
}

/*
bool
AosBFGenericArray::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	return true;
}
*/

bool
AosBFGenericArray::appendFieldValue(
		AosRundata *rdata, 
		const AosValueRslt &value,
		AosBuff *buff)
{
	return true;
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
AosBFGenericArray::appendFieldValue(
				AosRundata *rdata, 
			    const AosBsonField::Type data_type,
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	// "\x04" e_name data_type u32(number of entries) array(value, value, value, ....., value)
	bool rslt;
	buff->gotoEnd();
	switch(data_type)
	{
	//case 1:
	case AosBsonField::eFieldTypeU8:
		{
			u32 vv = value.getU64();
			rslt = buff->setU8((u8)vv);
			aos_assert_rr(rslt, rdata, false);
		}
		break;

	//case 2:
	case AosBsonField::eFieldTypeU16:
		{
			u16 vv = value.getU64();
			char c1 = (char)(vv & 0xFF);
			char c2 = (char) ((vv >>8) & 0xFF);
			buff->append(c1, c2);
		}
		break;

	//case 3:
	case AosBsonField::eFieldTypeU32:
		{
			u32 vv = value.getU64();
			char c1 = (char) (vv & 0xFF);
			char c2 = (char) ((vv >> 8) & 0xFF);
			char c3 = (char) ((vv >> 16) & 0xFF);
			char c4 = (char) ((vv >> 24) & 0xFF);
			buff->append(c1, c2, c3, c4);
		}
		break;

	//case 4:
	case AosBsonField::eFieldTypeU64:
		{
			u64 vv = value.getU64();
			char c1 = (char) (vv & 0xFF);
			char c2 = (char) ((vv >> 8) & 0xFF);
			char c3 = (char) ((vv >> 16) & 0xFF);
			char c4 = (char) ((vv >> 24) & 0xFF);
			buff->append(c1, c2, c3, c4);
			char c5 = (char) ((vv >> 32) & 0xFF);
			char c6 = (char) ((vv >> 40) & 0xFF);
			char c7 = (char) ((vv >> 48) & 0xFF);
			char c8 = (char) ((vv >> 56) & 0xFF);
			buff->append(c5, c6, c7, c8);
		}
		break;

	//case 5:
	case AosBsonField::eFieldTypeInt8:
		{
			i8 vv = value.getI64();
			rslt = buff->setChar(vv);
			aos_assert_rr(rslt, rdata, false);
		}
		break;

	case AosBsonField::eFieldTypeChar:
		{
			char vv = value.getChar();
			rslt = buff->setChar(vv);
			aos_assert_rr(rslt, rdata, false);
		}
		break;
	
	//case 6:
	case AosBsonField::eFieldTypeInt16:
		{
			int16_t vv = value.getI64();
			char c1 = (char)(vv & 0xFF);
			char c2 = (char) ((vv >>8) & 0xFF);
			buff->append(c1, c2);
		}
		break;
	
	case AosBsonField::eFieldTypeInt32:
		{
			int32_t vv = value.getI64();
			char c1 = (char)(vv & 0xFF);
			char c2 = (char) ((vv >>8) & 0xFF);
			char c3 = (char) ((vv >> 16)& 0xFF);
			char c4 = (char) ((vv >> 24)& 0xFF);
			buff->append(c1, c2, c3, c4);
		}
		break;

	case AosBsonField::eFieldTypeInt64:
		{
			int64_t vv = value.getI64();
			char c1 = (char)(vv & 0xFF);
			char c2 = (char) ((vv >>8) & 0xFF);
			char c3 = (char) ((vv >> 16)& 0xFF);
			char c4 = (char) ((vv >> 24)& 0xFF);
			buff->append(c1, c2, c3, c4);
			char c5 = (char) ((vv >> 32) & 0xFF);
			char c6 = (char) ((vv >> 40) & 0xFF);
			char c7 = (char) ((vv >> 48) & 0xFF);
			char c8 = (char) ((vv >> 56) & 0xFF);
			buff->append(c5, c6, c7, c8);
		}
		break;

	case AosBsonField::eFieldTypeFloat:
		{
			float vv = value.getDouble();
			char* data = (char*)&vv;
			buff->append(data[0], data[1], data[2], data[3]);
		}
		break;
	
	case AosBsonField::eFieldTypeDouble:
		{
			double vv = value.getDouble();
			char* data = (char*)&vv;
			buff->append(data[0], data[1], data[2], data[3]);
			buff->append(data[4], data[5], data[6], data[7]);
		}
		break;
	
	case AosBsonField::eFieldTypeString:
		{
           OmnString str = value.getStr();
		   int len = str.length();
		   bool rslt = AosVarUnInt::encode((u32)len, buff);
		   aos_assert_r(rslt, false);
		   buff->gotoEnd();
		   buff->setBuff(str.data(), len);
		   return true;
		}
	
	default: 
		return false;
	}
	return true;
}


bool 
AosBFGenericArray::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	// "\x34" e_name  u32(number of entries) (u32)dataLen  array 
	char *data = buff->data();
	//skip the four byte dataNumbers
	//u32 num_entries = *((u32*)(&data[pos]));
	pos +=sizeof(u32);
	//skip the dataLen
	u32 dataLen = *((u32*)(&data[pos]));
	pos = pos + sizeof(u32) + dataLen;
	return true;
}

bool
AosBFGenericArray::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeGenericArray);
	return true;
}


bool
AosBFGenericArray::setField(
		const OmnString &name,
		vector<AosValueRslt> &values,
		AosBuff *mBuffRaw)
{
    // The format is:
	//  valueType(0x34) + nameType +name + values
	//  values = u32(number of entries)+ arrayLength + array(value1, value2, value3, ....., value)
	bool rslt;
	mBuffRaw->gotoEnd();
	rslt = mBuffRaw->setChar(AosBsonField::eFieldTypeGenericArray);
	aos_assert_r(rslt , false);
	rslt = mBuffRaw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, mBuffRaw);
	aos_assert_r(rslt , false);

	//set the numbers of array into buff
	mBuffRaw->gotoEnd();
	u32 number = values.size();
	rslt = mBuffRaw->setU32(number);
	aos_assert_r(rslt,false);

	//before set arrayLength wo should set value first ,so wo can 
	//get arrayLength    aw->gotoEnd();                                                              
    i64 tempIdx = mBuffRaw->getCrtIdx();
	mBuffRaw->setU32(0); //use for store arrayLength

	//store values
	//first:get the type of the data
	for(u32 i = 0; i < values.size(); i++)
	{
		//get the element data type
		AosBsonField::Type data_type = getValueType(values[i]); 
		rslt = mBuffRaw->setChar(data_type);
		aos_assert_r(rslt,false);

		//set Value
		rslt = appendFieldValue(0, data_type, values[i], mBuffRaw);
		aos_assert_r(rslt, false);
    }
	u32 mDocLength = mBuffRaw->dataLen();
	// set array length
	// set the pointer to the begin of the ArrayLength
	mBuffRaw->setCrtIdx(tempIdx);
	mBuffRaw->setU32(mDocLength - tempIdx - 4);
	mBuffRaw->reset();
	mBuffRaw->setU32(mDocLength);
	return true;
}

	
AosBsonField::Type
AosBFGenericArray::getValueType(AosValueRslt value)
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

