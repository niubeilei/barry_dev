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
// 2014/12/27 Created by Rain
////////////////////////////////////////////////////////////////////////////
#include "BSON/BFVarUint.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFVarUint_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFVarUint(version);
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



AosBFVarUint::AosBFVarUint(const int version)
:
AosBsonField(version, eFieldTypeVarUint)
{
}


AosBFVarUint::~AosBFVarUint()
{
}


AosJimoPtr 
AosBFVarUint::cloneJimo() const
{
	return OmnNew AosBFVarUint(*this);
}


bool
AosBFVarUint::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// Its format is:
	// 	"\x11" e_name u64(varuint) 
	// The u64 value is encoded as little endian.
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	AosValueRslt vv;
	char *data = buff->data();
	rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);                                                                                                 
	aos_assert_r(rslt, false);
	u64 val;
	val = vv.getU64();
	value.setU64(val);
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
AosBFVarUint::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff)
{
	// The class assumes 'field_type' and 'e_name' has already
	// beein appended.
	u64 vv =  value.getU64();
	//set value as varuint
	buff->gotoEnd();
  //	bool rslt = AosBsonField::setFieldNameU32(vv, buff);
	bool rslt = AosVarUnInt::encode(vv,buff);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosBFVarUint::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	//pos += sizeof(u64);
	//now the value has become to varUint style
	//the pos had pointed to the begin of Value
	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	char* data = buff->data();
	AosValueRslt vv;
	bool rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);                                                                                                 
	aos_assert_r(rslt, false);        
	//u64 len = vv.getu64Value(rdata);rslt	
	return true;
}

bool 
AosBFVarUint::setType(
		AosRundata *rdata,
		AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeVarUint);
	return true;
}


bool 
AosBFVarUint::setField(
		const u32 name, 
		const u64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	valueType(u8) + nameType + nameValue(u8) + (u64)value(varuint)
	buff_raw->gotoEnd();
	bool rslt = buff_raw->setChar(AosBsonField::eFieldTypeVarUint);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt , false);

	//Set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	//buff->setu64(name);

	//Set value as varuint
	rslt = AosVarUnInt::encode(value, buff_raw);
	aos_assert_r(rslt , false);
	return true;
}


bool
AosBFVarUint::setField(
		const OmnString &name, 
		const u64 &value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	valuetype(u8) + (u8)nametype + fieldname(string) + value(varuint)
	// 	string = stringlength + stringValue
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeVarUint);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	buff_raw->gotoEnd();
	//rslt = AosBsonField::setFieldNameU32(value, buff_raw);
	rslt = AosVarUnInt::encode(value, buff_raw);
	aos_assert_r(rslt , false);
	return true;
}

