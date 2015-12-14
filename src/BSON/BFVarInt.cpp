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
// 2014/12/29 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFVarInt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFVarInt_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFVarInt(version);
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



AosBFVarInt::AosBFVarInt(const int version)
:
AosBsonField(version, eFieldTypeVarInt)
{
}


AosBFVarInt::~AosBFVarInt()
{
}


AosJimoPtr 
AosBFVarInt::cloneJimo() const
{
	return OmnNew AosBFVarInt(*this);
}


bool
AosBFVarInt::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// 	"\x11" e_name i64(varInt) 
	// The i64 value is encoded as little endian.
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	AosValueRslt vv;
	char *data = buff->data();
	rslt = AosVarInt::decode(data, buff->dataLen(), pos, vv);                                                                                                 
	aos_assert_r(rslt, false);
	i64 val = vv.getI64();
	value.setI64(val);
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
AosBFVarInt::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	i64 vv;
	vv = value.getI64();
	//set value as varInt
	buff->gotoEnd();
  //	bool rslt = AosBsonField::setFieldNameU32(vv, buff);
	bool rslt = AosVarInt::encode(vv,buff);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFVarInt::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	//pos += sizeof(i64);
	//now the value has become to varInt style
	//the pos had pointed to the begin of Value
	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	char* data = buff->data();
	AosValueRslt vv;
	bool rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);                                                                                                 
	aos_assert_r(rslt, false);        
	//i64 len = vv.getu64Value(rdata);rslt	
	return true;
}

bool 
AosBFVarInt::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeVarInt);
	return true;
}


bool 
AosBFVarInt::setField(
		const u32 name, 
		const i64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	valueType(u8) + nameType + nameValue(u8) + (i64)value(varInt)
	buff_raw->gotoEnd();
	bool rslt = buff_raw->setChar(AosBsonField::eFieldTypeVarInt);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	//Set name as varInt
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	//buff->setu64(name);

	//Set value as varInt
	rslt = AosVarInt::encode(value, buff_raw);
	aos_assert_r(rslt , false);
	return true;
}


bool
AosBFVarInt::setField(
		const OmnString &name, 
		const i64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	valuetype(u8) + (u8)nametype + fieldname(string) + value(varInt)
	// 	string = stringlength + stringValue
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeVarInt);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varInt(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varInt
	buff_raw->gotoEnd();
	//rslt = AosBsonField::setFieldNameU32(value, buff_raw);
	rslt = AosVarInt::encode(value, buff_raw);
	aos_assert_r(rslt , false);
	return true;
}

