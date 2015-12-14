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
#include "BSON/BFInt64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFInt64_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFInt64(version);
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


AosBFInt64::AosBFInt64(const int version)
:
AosBsonField(version, eFieldTypeInt64)
{
}


AosBFInt64::~AosBFInt64()
{
}


AosJimoPtr 
AosBFInt64::cloneJimo() const
{
	return OmnNew AosBFInt64(*this);
}


bool
AosBFInt64::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// "\x0c" e_name int64	
	// The i64 value is encoded as little endian.
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	i64 vv;
	char *data = buff->data();
	vv = (((i64)data[pos])&0xff )+
		 ((((i64)data[pos+1]) << 8) & 0xffff)+
		 ((((i64)data[pos+2]) << 16)& 0xffffff) +
		 ((((i64)data[pos+3]) << 24)& 0xffffffff) +
		 ((((i64)data[pos+4]) << 32)& 0xffffffffff)+
		 ((((i64)data[pos+5]) << 40)& 0xffffffffffff) +
		 ((((i64)data[pos+6]) << 48)& 0xffffffffffffff) +
		 ((((i64)data[pos+7]) << 56)& 0xffffffffffffffff); 
	pos += 8;
	value.setI64(vv);
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
AosBFInt64::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	i64 vv = value.getI64();
	bool rslt = appendInt64(vv, buff);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFInt64::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	pos += sizeof(i64);
	return true;
}


bool
AosBFInt64::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeInt64);
	return true;
}


bool
AosBFInt64::setField(
		const OmnString &name, 
		const i64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeInt64);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	buff_raw->gotoEnd();
	rslt = appendInt64(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFInt64::setField(
		const u32 name, 
		const i64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03(u8) + fieldname(varuint) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeInt64);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	// Set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	
	// Set value as varuint
	buff_raw->gotoEnd();
	rslt = appendInt64(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBFInt64::appendInt64(
				const i64 &value,
				AosBuff *buff_raw)
{
	//append (varuint)i64 to the buff.
	char c1 = (char)(value & 0xFF);
	char c2 = (char)((value >> 8) & 0xFF); 
	char c3 = (char)((value >> 16) & 0xFF); 
	char c4 = (char)((value >> 24) & 0xFF); 
	buff_raw->append(c1, c2, c3, c4);
	c1 = (char)((value >> 32) & 0xFF); 
	c2 = (char)((value >> 40) & 0xFF); 
	c3 = (char)((value >> 48) & 0xFF); 
	c4 = (char)((value >> 56) & 0xFF); 
	buff_raw->append(c1, c2, c3, c4);

	return true;
}

