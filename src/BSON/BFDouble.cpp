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
#include "BSON/BFDouble.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFDouble_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFDouble(version);
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


AosBFDouble::AosBFDouble(const int version)
:
AosBsonField(version, eFieldTypeDouble)
{
}


AosBFDouble::~AosBFDouble()
{
}


AosJimoPtr 
AosBFDouble::cloneJimo() const
{
	return OmnNew AosBFDouble(*this);
}


bool
AosBFDouble::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// "0x01" e_name double	
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	double vv;
	char *vv_mem = (char *)&vv;
	char *data = buff->data();
	memcpy(vv_mem, &data[pos], sizeof(double));

	pos += sizeof(double);
	value.setDouble(vv);
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
AosBFDouble::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	//double vv = value.getU64Value(0);
	double vv = value.getDouble();
	bool rslt = appendDouble(vv, buff);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosBFDouble::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	pos += sizeof(double);
	return true;
}


bool 
AosBFDouble::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeDouble);
	return true;
}


bool
AosBFDouble::setField(
		const OmnString &name, 
		const double value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeDouble);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	buff_raw->gotoEnd();
	rslt = appendDouble(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFDouble::setField(
		const u32 name, 
		const double value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x03(u8) + fieldname(varuint) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeDouble);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	// Set name as varuint
	//AosBsonField *field = tsFields[AosBsonField::eFieldTypeDouble];
	//rslt = field->setFieldNameU32(name, buff_raw);
	AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	
	// Set value as varuint
	buff_raw->gotoEnd();
	rslt = appendDouble(value, buff_raw);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBFDouble::appendDouble(
		const double value,
		AosBuff *buff_raw)
{
	//append (varuint)double to the buff.
	char *data = (char *)&value;
	
	buff_raw->append(data[0], data[1], data[2], data[3]);
	buff_raw->append(data[4], data[5], data[6], data[7]);

	return true;
}

