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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFU64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFU64_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFU64(version);
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



AosBFU64::AosBFU64(const int version)
:
AosBsonField(version, eFieldTypeU64)
{
}


AosBFU64::~AosBFU64()
{
}


AosJimoPtr 
AosBFU64::cloneJimo() const
{
	return OmnNew AosBFU64(*this);
}


bool
AosBFU64::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// 	"\x18" e_name u64
	// The u64 value is encoded as little endian.
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	u64 vv;
	char *data = buff->data();
	vv = (((u64)data[pos])&0xff )+
		 ((((u64)data[pos+1]) << 8) & 0xffff)+
		 ((((u64)data[pos+2]) << 16)& 0xffffff) +
		 ((((u64)data[pos+3]) << 24)& 0xffffffff) +
		 ((((u64)data[pos+4]) << 32)& 0xffffffffff)+
		 ((((u64)data[pos+5]) << 40)& 0xffffffffffff) +
		 ((((u64)data[pos+6]) << 48)& 0xffffffffffffff) +
		 ((((u64)data[pos+7]) << 56)& 0xffffffffffffffff); 
	pos += 8;
	value.setU64(vv);
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
AosBFU64::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	u64 vv = value.getU64();
	bool rslt = appendU64(vv, buff);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFU64::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	pos += sizeof(u64);
	return true;
}

bool 
AosBFU64::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeU64);
	return true;
}


bool
AosBFU64::setField(
		const OmnString &name, 
		const u64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeU64);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	buff_raw->gotoEnd();
	rslt = appendU64(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFU64::setField(
		const u32 name, 
		const u64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03(u8) + fieldname(varuint) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeU64);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	// Set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	
	// Set value as varuint
	buff_raw->gotoEnd();
	rslt = appendU64(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBFU64::appendU64(
				const u64 &value,
				AosBuff *buff_raw)
{
	//append (varuint)u64 to the buff.
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

