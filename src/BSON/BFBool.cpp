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
// 2015/01/9 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFBool.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFBool_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFBool(version);
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



AosBFBool::AosBFBool(const int version)
:
AosBsonField(version, eFieldTypeBool)
{
}


AosBFBool::~AosBFBool()
{
}


AosJimoPtr 
AosBFBool::cloneJimo() const
{
	return OmnNew AosBFBool(*this);
}


bool
AosBFBool::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// "\x06" e_name + value	
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	char *data = buff->data();
	bool vv = (bool)(data[pos]);
	value.setBool(vv);
	pos += sizeof(bool);
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
AosBFBool::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	//	The format for null value is:
	bool vv = value.getBool();
	buff->append((char)vv);
	return true;
}


bool 
AosBFBool::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	pos += sizeof(bool);
	return true;
}

bool
AosBFBool::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeBool);
	return true;
}


bool 
AosBFBool::setField(
		const OmnString &name, 
		const bool value,
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) 
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeBool);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	buff_raw->gotoEnd();
    buff_raw->append(char(value));
	return true;
}


bool 
AosBFBool::setField(
		const u32 name, 
		const bool value,
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03(u8) + fieldname(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeBool);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	// Set name as varuint
	AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	buff_raw->gotoEnd();
	buff_raw->append(char(value));
	return true;
}

