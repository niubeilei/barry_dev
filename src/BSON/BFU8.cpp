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
#include "BSON/BFU8.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFU8_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFU8(version);
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


AosBFU8::AosBFU8(const int version)
:
AosBsonField(version, eFieldTypeU8)
{
}


AosBFU8::~AosBFU8()
{
}


AosJimoPtr 
AosBFU8::cloneJimo() const
{
	return OmnNew AosBFU8(*this);
}


bool
AosBFU8::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// 	"\x0f" e_name u8
	// The u8 value is encoded as little endian.
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	buff->setCrtIdx(pos);
	u8 vv = buff->getU8(0);

	pos += sizeof(u8);
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
AosBFU8::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	u8 vv = value.getU64();
	buff->gotoEnd();
	bool rslt = buff->setU8(vv);
	aos_assert_r(rslt , false);
	return true;
}


bool 
AosBFU8::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	pos += sizeof(u8);
	return true;
}


bool 
AosBFU8::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeU8);
	return true;
}


bool 
AosBFU8::setField(
		const u32 name, 
		const u8 value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03 eNumericFieldName(u8) + fieldname(varuint) + value(varuint)
	buff_raw->gotoEnd();
	bool rslt = buff_raw->setChar(AosBsonField::eFieldTypeU8);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	//Set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);

	//Set value as varuint
	buff_raw->gotoEnd();
	rslt = buff_raw->setU8(value);
	aos_assert_r(rslt , false);
	return true;
}


bool
AosBFU8::setField(
		const OmnString &name, 
		const u8 value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeU8);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	buff_raw->gotoEnd();
	rslt = buff_raw->setU8(value);
	aos_assert_r(rslt, false);
	return true;
}

