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
#include "BSON/BsonField.h"
#include "BSON/BFString.h"
#include "BSON/BFU64.h"
#include "BSON/BFInt64.h"
#include "BSON/BFNull.h"
#include "BSON/BFDouble.h"
#include "BSON/BFFloat.h"
#include "BSON/BFU32.h"
#include "BSON/BFU16.h"
#include "BSON/BFU8.h"
#include "BSON/BFInt32.h"
#include "BSON/BFInt16.h"
#include "BSON/BFInt8.h"
#include "BSON/BFDocument.h"
#include "BSON/BFVarUint.h"
#include "BSON/BFArray.h"
#include "BSON/BFVarInt.h"
#include "BSON/BFGenericArray.h"
#include "BSON/BFChar.h"
#include "BSON/BFBool.h"
#include "BSON/BFBinary.h"
#include "BSON/BFMap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Util/VarUnInt.h"
#include "Util/VarInt.h"


OmnString AosBsonField::smClassnames[256];
static OmnMutex sgLock;
static bool sgInited = false;


AosBsonField::AosBsonField(const int version, const Type type)
:
AosJimo(AosJimoType::eBsonField, version),
mType(type)
{
	if (!sgInited)
	{
		sgLock.lock();
		if (!sgInited) 
		{
			init();
		}
		sgLock.unlock();
	}
}


AosBsonField::~AosBsonField()
{
}


bool
AosBsonField::init()
{
	smClassnames[eFieldTypeString] = "AosBFString";
	smClassnames[eFieldTypeU64] = "AosBFU64";
	smClassnames[eFieldTypeDouble] = "AosBFDouble";
	smClassnames[eFieldTypeFloat] = "AosBFFloat";
	smClassnames[eFieldTypeInt64] = "AosBFInt64";
	smClassnames[eFieldTypeNull] = "AosBFNull";
	smClassnames[eFieldTypeU32] = "AosBFU32";
	smClassnames[eFieldTypeU16] = "AosBFU16";
	smClassnames[eFieldTypeU8] = "AosBFU8";
	smClassnames[eFieldTypeInt32] = "AosBFInt32";
	smClassnames[eFieldTypeInt16] = "AosBFInt16";
	smClassnames[eFieldTypeInt8] = "AosBFInt8";
	smClassnames[eFieldTypeChar] = "AosBFChar";
	smClassnames[eFieldTypeDocument] = "AosBFDocument";
	smClassnames[eFieldTypeArray] = "AosBFArray";
	smClassnames[eFieldTypeVarUint] = "AosBFVarUint";
	smClassnames[eFieldTypeVarInt] = "AosBFVarInt";
	smClassnames[eFieldTypeGenericArray] = "AosBFGenericArray";
	smClassnames[eFieldTypeBool] = "AosBFBool";
	smClassnames[eFieldTypeBinary] = "AosBFBinary";
	smClassnames[eFieldTypeMap] = "AosBFMap";
	return true;
}


bool 
AosBsonField::skipEname(
		AosRundata *rdata, 
		AosBuff *buff, 
		i64 &pos)
{
	// 	id byte			one byte
	// 	nametype		one byte
	// 	varunint		variable
	// 	contents		variable
	aos_assert_rr(pos + 1 < buff->dataLen(), rdata, false);

	char *data = buff->data();
	bool rslt;
	AosValueRslt vv;
	u8 id = data[pos++];

	if (isEname(id)) 
	{
		if (data[pos] == 0x01)   //check the name type is string or U32
		{
			pos++;
			//get the 'varunint'
			rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);
			aos_assert_r(rslt, false);
			
			u32 len = vv.getU64();
			pos += len;
		}
		else if (data[pos] == 0x03) //prove the name type is U32
		{
		  // the name length become to varuint	
		  //	pos++;
	      //	pos+=sizeof(u32);
			pos++;
			//get the 'varunint'
			rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);
			aos_assert_r(rslt, false);
			
		//	u32 len = vv.getU32Value(rdata);
		//	pos += len;
		}
		return true;
	}

	if (isDname(id))
	{
		OmnNotImplementedYet;
		return false;
	}

	if (isNname(id))
	{
		OmnNotImplementedYet;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosBsonField::getName(
		AosRundata *rdata, 
		AosBuff *buff, 
		OmnString &name,
		i64 &pos) 
{
	//e_name: varUnint(lenght) contents(name)
	//the pos is point to the 'varunint'
	char *data = buff->data();
	AosValueRslt vv;

	//get the content's length and skip the varUnint
	bool rslt = AosVarUnInt::decode(data, buff->dataLen(), pos, vv);
	aos_assert_r(rslt, false);
	u32 len = vv.getU64();
	//get the 'contents'
	OmnString nn(&data[pos], len);
	name = nn;

	//skip the contents,now pos is point to the 'value'
	pos += len;
	return true;
}

//varuint function 2014/12/26  create by rain
bool
AosBsonField::getName(
		AosRundata *rdata, 
		AosBuff *buff, 
		u32 &name,
		i64 &pos) 
{
	//e_name: u32(name)
	char *data = buff->data();
    AosValueRslt vv;
	i64 len = buff->dataLen();
	//get the name
	//bool rslt = AosVarUnInt::decode();
	bool rslt = AosVarUnInt::decode(data,len,pos,vv);
	aos_assert_r(rslt,false);
	name = vv.getU64();
	return true;
}
/*
 * now all U32 name also must be encode to varuint length + data 2014/12/26 rain
bool
AosBsonField::getName(
		AosRundata *rdata, 
		AosBuff *buff, 
		u32 &name,
		i64 &pos) 
{
	//e_name: u32(name)
	char *data = buff->data();

	//skip the name,now pos is point to the 'value'
	name = (((u32)data[pos])&0xff )+
		 ((((u32)data[pos+1]) << 8) & 0xffff)+
		 ((((u32)data[pos+2]) << 16)& 0xffffff) +
		 ((((u32)data[pos+3]) << 24)& 0xffffffff);

	pos += sizeof(u32);
	return true;
}
*/


//Add different field handler jimos
//into the fields
AosBsonField **
AosBsonField::initFields()
{
	if (!sgInited)
	{
		//set jimo class name array
		smClassnames[eFieldTypeString] = "AosBFString";
		smClassnames[eFieldTypeU64] = "AosBFU64";
		smClassnames[eFieldTypeInt64] = "AosBFInt64";
		smClassnames[eFieldTypeNull] = "AosBFNull";
		smClassnames[eFieldTypeDouble] = "AosBFDouble";
		smClassnames[eFieldTypeU32] = "AosBFU32";
		smClassnames[eFieldTypeFloat] = "AosBFFloat";
		smClassnames[eFieldTypeU16] = "AosBFU16";
		smClassnames[eFieldTypeU8] = "AosBFU8";
		smClassnames[eFieldTypeInt32] = "AosBFInt32";
		smClassnames[eFieldTypeInt16] = "AosBFInt16";
		smClassnames[eFieldTypeInt8] = "AosBFInt8";
		smClassnames[eFieldTypeChar] = "AosBFChar";
		smClassnames[eFieldTypeDocument] = "AosBFDocument";
		smClassnames[eFieldTypeArray] = "AosBFArray";
	    smClassnames[eFieldTypeVarUint] = "AosBFVarUint";
	    smClassnames[eFieldTypeVarInt] = "AosBFVarInt";
	    smClassnames[eFieldTypeGenericArray] = "AosBFGenericArray";
	    smClassnames[eFieldTypeBool] = "AosBFBool";
	    smClassnames[eFieldTypeBinary] = "AosBFBinary";
	    smClassnames[eFieldTypeMap] = "AosBFMap";
		sgInited = true;
	}

	AosBsonField **fields = OmnNew AosBsonField*[256];
	memset(fields, 0, sizeof(AosBsonField*[256]));

	//set jimo object array
	//use jimos later on
	/*
	for (u32 i=0; i<256; i++)
	{
		if (smClassnames[i] != "")
		{
			AosJimoPtr jimo = AosCreateJimo(rdata, smClassnames[i], 1);
			aos_assert_rr(jimo, rdata, 0);
			aos_assert_rr(jimo->getJimoType() == AosJimoType::eBsonField, rdata, 0);
			jimo = jimo->cloneJimo();
			AosBsonField* field = dynamic_cast<AosBsonField *>(jimo.getPtr());
			aos_assert_rr(field, rdata, 0);
			fields[i] = field;
		}
	}
	*/

	int version = 1;
	fields[eFieldTypeString] = new AosBFString(version);
	fields[eFieldTypeU64] = new AosBFU64(version);
	fields[eFieldTypeInt64] = new AosBFInt64(version);
	fields[eFieldTypeNull] = new AosBFNull(version);
	fields[eFieldTypeDouble] = new AosBFDouble(version);
	fields[eFieldTypeU32] = new AosBFU32(version);
	fields[eFieldTypeFloat] = new AosBFFloat(version);
	fields[eFieldTypeU16] = new AosBFU16(version);
	fields[eFieldTypeU8] = new AosBFU8(version);
	fields[eFieldTypeInt32] = new AosBFInt32(version);
	fields[eFieldTypeInt16] = new AosBFInt16(version);
	fields[eFieldTypeInt8] = new AosBFInt8(version);
	fields[eFieldTypeChar] = new AosBFChar(version);
	fields[eFieldTypeDocument] = new AosBFDocument(version);
	fields[eFieldTypeArray] = new AosBFArray(version);
	fields[eFieldTypeVarUint] = new AosBFVarUint(version);
	fields[eFieldTypeVarInt] = new AosBFVarInt(version);
	fields[eFieldTypeGenericArray] = new AosBFGenericArray(version);
	fields[eFieldTypeBool] = new AosBFBool(version);
	fields[eFieldTypeBinary] = new AosBFBinary(version);
	fields[eFieldTypeMap] = new AosBFMap(version);
	
	cout << "BsonField init" << endl;
	return fields;
}


bool 
AosBsonField::xpathSetAttr(
		AosRundata *rdata, 
		AosBuffPtr buff,
		const vector<OmnString> &fnames, 
		const int crt_idx,
		const AosValueRslt &value, 
		const bool create)
{
	// This function assumes 'fnames[crt_idx]' is this field's name.
	// The field must be a document. Otherwise, it is an error.
	AosLogError(rdata, false, "internal_error")
		<< "Trying to set a field value to a non document field: " 
		<< fnames[crt_idx] 
		//<< ". Current field name: " << getFieldName() 
		<< enderr;
	return false;
}


bool 
AosBsonField::appendFieldValue(
		AosRundata *rdata,
		//const char data_type,
		const AosBsonField::Type data_type,
		const AosValueRslt &value,
		AosBuff *buff)
{
	// The field must be a Array. Otherwise, it is an error.
	AosLogError(rdata, false, "internal_error")
		<< "Trying to set a field value to a non array field: " 
		<< enderr;
	return false;
}


bool 
AosBsonField::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		const int idx,
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	AosLogError(rdata, false, "internal_error")
		<< "Trying to get a field value from a non array field: "
		<< enderr;
	return false;
}

//Generic Array
bool 
AosBsonField::getFieldValue(
		AosRundata *rdata, 
		AosBuff *buff, 
		//const int idx,
		i64 &pos, 
		const bool copy_flag,
		AosValueRslt &value)
{
	AosLogError(rdata, false, "internal_error")
		<< "Trying to get a field value from a non array field: "
		<< enderr;
	return false;
}

bool 
AosBsonField::setField(const OmnString &name, const u64 &value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const u32 value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const u16 value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const u8 value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const i64 &value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const i32 value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const i16 value, AosBuff *buff_raw)
{
	return false;
}

	
bool 
AosBsonField::setField(const OmnString &name, const i8 value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const OmnString &name, const bool value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const char value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const float value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const OmnString &name, const AosBuff* value, AosBuff *buff_raw)
{
	return false;
}

bool 
AosBsonField::setField(const OmnString &name, const double value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, const OmnString &value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const OmnString &name, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(map<OmnString, OmnString> &values, AosBuff *buff_raw)
{
    return false;
}

bool
AosBsonField::setField(const OmnString &name, vector<AosValueRslt>& values, AosBuff *buff_raw)
{
    return false;
}

bool 
AosBsonField::setField(const u32 name, const u64 &value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const AosBuff* value, AosBuff *buff_raw)
{
	return false;
}

bool 
AosBsonField::setField(const u32 name, const u32 value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const u16 value, AosBuff *buff_raw)
{
	return false;
}

	
bool 
AosBsonField::setField(const u32 name, const u8 value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const i64 &value, AosBuff *buff_raw)
{
	return false;
}

	
bool 
AosBsonField::setField(const u32 name, const i32 value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const i16 value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const i8 value, AosBuff *buff_raw)
{
	return false;
}
	
bool 
AosBsonField::setField(const u32 name, const bool value, AosBuff *buff_raw)
{
	return false;
}

bool 
AosBsonField::setField(const u32 name, const char value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const u32 name, const float value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const double value, AosBuff *buff_raw)
{
	return false;
}
	

bool 
AosBsonField::setField(const u32 name, const OmnString &value, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setField(const u32 name, AosBuff *buff_raw)
{
	return false;
}


bool 
AosBsonField::setFieldNameStr(
		const OmnString &name, 
		AosBuff *buff_raw)
{
	const char *dd = name.data();
	int len = name.length();

	//append 'varunint (length)'
	buff_raw->gotoEnd();
	bool rslt = AosVarUnInt::encode((u32)len, buff_raw);
	aos_assert_r(rslt, false);

	//append 'string' (contents)
	buff_raw->gotoEnd();
	rslt = buff_raw->setBuff(dd, len);
	aos_assert_r(rslt, false);
	return true;
}

// now all U32 must be encodeed to varuint style   2014/12/26  Rain
bool 
AosBsonField::setFieldNameU32(
		const u32 name, 
		AosBuff *buff_raw)
{
	buff_raw->gotoEnd();
	//regard the name as length,and store it into the buff
    bool rslt = AosVarUnInt::encode(name,buff_raw);
	aos_assert_r(rslt,false);
	return true;
}

/* now all U32 must be encodeed to varuint style   2014/12/26  Rain
bool 
AosBsonField::setFieldNameU32(
		const u32 name, 
		AosBuff *buff_raw)
{
	buff_raw->gotoEnd();
	char c1 = (char)(name & 0xFF);
	char c2 = (char)((name >> 8) & 0xFF); 
	char c3 = (char)((name >> 16) & 0xFF); 
	char c4 = (char)((name >> 24) & 0xFF); 
	buff_raw->append(c1, c2, c3, c4);
	return true;
}*/

bool 
AosBsonField::isValid(const i8 id)
{
   if(id > AosBsonField::eMaxBsonFieldType || id < AosBsonField::eInvalidFieldType)
   {
      return false;
   }
   return true;
}
