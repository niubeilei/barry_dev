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
#include "BSON/BFBinary.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"
#include "Util/ValueRslt.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBFBinary_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBFBinary(version);
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



AosBFBinary::AosBFBinary(const int version)
:
AosBsonField(version, eFieldTypeBinary)
{
}


AosBFBinary::~AosBFBinary()
{
}


AosJimoPtr 
AosBFBinary::cloneJimo() const
{
	return OmnNew AosBFBinary(*this);
}


bool
AosBFBinary::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	// eString e_name string
	// string ::= varUnInt + bytes* + 0x00
	
	bool rslt = skipEname(rdata, buff, pos);
	aos_assert_rr(rslt, rdata, false);

	aos_assert_rr(pos < buff->dataLen(), rdata, false);
	
	//skip string's 'varunint' (length)
	AosValueRslt vv;
	char *data = buff->data();
	rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);
	aos_assert_rr(rslt, rdata, false);

	u64 len = vv.getU64();
	
	//set mCrtIdx
	//i64 crtIdx = buff->getCrtIdx();
	//crtIdx = pos;
	buff->setCrtIdx(pos);
    AosBuffPtr bBuff = buff->getBuff(len, false AosMemoryCheckerArgs);
	aos_assert_rr(bBuff, rdata, NULL);
    // set value to valueRslt
    value.setBuff(bBuff);
	/*
	aos_assert_rr((pos+len) <= buff->dataLen(), rdata, false);

	//value.setValue(&data[pos], (int)len);
	value.setCharStr1(&data[pos], (int)len, copy_flag);
	pos += len; */
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
AosBFBinary::appendFieldValue(
				AosRundata *rdata, 
				const AosValueRslt &value,
				AosBuff *buff) 
{
	// This function appends 'varunint (length)' + 'string' (contents)
	// to 'buff'. 
	OmnString str = value.getStr();
	int len = str.length();

	//append 'varunint (length)'
	bool rslt = AosVarUnInt::encode((u32)len, buff);
	aos_assert_r(rslt, false);

	//append 'string' (contents)
	buff->gotoEnd();
	buff->setBuff(str.data(), len);
	return true;
}


bool 
AosBFBinary::skipValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	//varUnint value
	
	char *data = buff->data();
	AosValueRslt vv;
	u64 len;

	aos_assert_r(pos < buff->dataLen(), false);

	//get 'varunint' (length) and 'pos' point to the 'value'
	bool rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);
	aos_assert_r(rslt, false);
	len = vv.getU64();
	aos_assert_r(rslt, false);

	pos +=len;
	return true;
}

bool 
AosBFBinary::setType(
		AosRundata *rdata,
	    AosBuff *buff)
{
	buff->setChar(AosBsonField::eFieldTypeBinary);
	return true;
}


bool 
AosBFBinary::setField(
		const u32 name, 
		const AosBuff* value, 
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x07(u8) + fieldname + value(varuint)
	// 	value = length + value
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeBinary);
	aos_assert_r(rslt, false);
	rslt = buff_raw->setChar(AosBsonField::eNumericalFieldName);
	aos_assert_r(rslt, false);

	//set name as varuint
	rslt = AosBsonField::setFieldNameU32(name, buff_raw);
	aos_assert_r(rslt , false);
	
	// Set value as varuint
	buff_raw->gotoEnd();
	u64 dataLen = value->dataLen();
    rslt = AosVarUnInt::encode(dataLen, buff_raw);
	aos_assert_r(rslt , false);
	buff_raw->gotoEnd();
	rslt = buff_raw->setBuff(const_cast<AosBuff*>(value));
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBFBinary::setField(
		const OmnString &name, 
		const AosBuff *value,
		AosBuff *buff_raw)
{
	// The format is:
	// 	type(u8) + 0x01(u8) + fieldname(string) + value(varuint)
	bool rslt;
	buff_raw->gotoEnd();
	rslt = buff_raw->setChar(AosBsonField::eFieldTypeBinary);
	aos_assert_r(rslt , false);
	rslt = buff_raw->setChar(AosBsonField::eStringFieldName);
	aos_assert_r(rslt , false);

	//set name as string'varuint(length) + 'string'(contents)'
	rslt = AosBsonField::setFieldNameStr(name, buff_raw);
	aos_assert_r(rslt , false);
	
	//set value as varuint
	buff_raw->gotoEnd();
	u64 dataLen = value->dataLen();
    rslt = AosVarUnInt::encode(dataLen, buff_raw);
	aos_assert_r(rslt , false);
	buff_raw->gotoEnd();
	rslt = buff_raw->setBuff(const_cast<AosBuff*>(value));
	aos_assert_r(rslt , false);
	return true;
}


