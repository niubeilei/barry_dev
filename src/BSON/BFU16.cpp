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
// 2014/11/14 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFU16.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFU16_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFU16(version);
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


AosBFU16::AosBFU16(const int version)
:
AosBsonField(version, eFieldTypeU16)
{
}


AosBFU16::~AosBFU16()
{
}


AosJimoPtr 
AosBFU16::cloneJimo() const
{
	return OmnNew AosBFU16(*this);
}


bool
AosBFU16::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// 	"\x10" e_name u16
	// The u16 value is encoded as little endian.
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	u16 vv;
	char *data = buff->data();
	vv = (((u16)data[pos])&0xff )+
		 ((((u16)data[pos+1]) << 8) & 0xffff);

	pos += 2;
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
AosBFU16::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	u16 vv = value.getU64();
	
	bool rslt = appendU16(vv, buff);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFU16::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	pos += sizeof(u16);
	return true;
}


bool 
AosBFU16::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeU16);
	return true;
}


bool 
AosBFU16::setField(
		const u32 name, 
		const u16 value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03 eNumericFieldName(u8) + fieldname(varuint) + value(varuint)
	buff_raw->gotoEnd();
	bool rslt = buff_raw->setChar(AosBsonField::eFieldTypeU16);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	//Set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);

	//Set value as varuint
	rslt = appendU16(value, buff_raw);
	aos_assert_r(rslt , false);
	return true;
}


bool
AosBFU16::setField(
		const OmnString &name, 
		const u16 value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeU16);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	rslt = appendU16(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBFU16::appendU16(const u16 &value,
					AosBuff *buff_raw)
{
	buff_raw->gotoEnd();
	char c1 = (char)(value & 0xFF);
	char c2 = (char)((value >> 8) & 0xFF); 
	buff_raw->append(c1, c2);
	return true;
}

