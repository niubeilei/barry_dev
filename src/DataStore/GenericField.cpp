////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 12/22/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/GenericField.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"


AosGenericField::AosGenericField()
:
mDataType(eType_Invalid),
mDbDataType(eDbType_Invalid),
mNotNull(false),
mPrimaryKey(false),
mDftValueSet(false),
mValueSet(false)
{
	memset(&mValue, 0, sizeof(mValue));
}


AosGenericField::~AosGenericField()
{
	if (mDataType == eType_Str && mValue.m_str) aos_free(mValue.m_str);
}


bool	
AosGenericField::parse(const OmnString &name, 
				const AosGenericField::DataType type,
				const OmnString &value, 
				const OmnString &dft_value)
{
	mFieldName = name;
	switch (type)
	{
	case eType_Str:
		 aos_assert_r(setValue(value), false);
		 break;

	case eType_Int:
		 {
			 int vv = 0;
			 if (aos_atoi(value.data(), value.length(), &vv) == 0)
			 {
				 aos_assert_r(setValue(vv), false);
				 return true;
			 }
			 aos_assert_r(aos_atoi(dft_value.data(), dft_value.length(), 
					&vv) == 0, false);
			 setValue(vv);
			 break;
		 }

	case eType_Char:
	case eType_Bool:
	case eType_Int64:
	case eType_U32:
	case eType_U64:
	case eType_Double:
		 OmnNotImplementedYet;
		 break;

	default:
		 OmnAlarm << "Invalid type: " << type << enderr;
		 return false;
	}

	return true;
}


bool
AosGenericField::parse(
		const TiXmlElement *elem, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 'elem' should be in the form:
	// 	<field name="xxx"
	// 		datatype="xxx" 		xxx = 'bool|char|double|int|int64|str|u32|u64'
	// 		db_datatype="xxx"			optional(eDbType_Invalid)
	// 		not_null="true|false" 		optional(false)
	// 		primary_key="true|false"	optional(false)
	// 		dft_value="xxx"				optional(null)
	// 	/>
	errcode = eAosXmlInt_SyntaxError;
	errmsg = "Internal error";

	aos_assert_r(elem, false);

	const char *fname = elem->Attribute("name");
	const char *datatype = elem->Attribute("datatype");
	const char *db_datatype = elem->Attribute("db_datatype");

	// Parsing the field name
	if (!fname)
	{
		errmsg = "Field definition missing field name attribute!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mFieldName = fname;

	// Parsing the data type
	if (!datatype)
	{
		errmsg = "Field definition missing field data type attribute!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mDataType = convertDataType(datatype);
	if (mDataType == eType_Invalid)
	{
		errmsg = "Field data type incorrect: ";
		errmsg << datatype;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Parsing the db datatype
	mDbDataType = eDbType_Invalid;
	if (db_datatype)
	{
		mDbDataType = convertDbDatatype(db_datatype);
		if (mDbDataType == eDbType_Invalid)
		{
			errmsg = "Database data type incorrect: ";
			errmsg << db_datatype;
			OmnAlarm << errmsg << enderr;
			return false;
		}
	}

	// Parse 'not_null'
	const char *not_null = elem->Attribute("not_null");
	mNotNull = (not_null && strcmp(not_null, "true") == 0);

	// Parse 'primary_key'
	const char *primary_key = elem->Attribute("primary_key");
	mPrimaryKey = (primary_key && strcmp(primary_key, "true") == 0);

	// Parse 'dft_value'
	const char *dft_value = elem->Attribute("dft_value");
	mDftValueSet = false;
	if (dft_value)
	{
		mDftValue = dft_value;
		mDftValueSet = true;
	}

	return true;
}


AosGenericField::DbDataType
AosGenericField::convertDbDatatype(const char *db_datatype)
{
	return eDbType_Invalid;
}


bool
AosGenericField::setValue(const OmnString &value)
{
	// The data type of the field is 'str'.
	int len = value.length();
	if (len <= 0)
	{
		mValue.m_str = 0;
	}
	else
	{
		mValue.m_str = (char *)aos_malloc(len+1);
		aos_assert_r(mValue.m_str, false);
		strncpy(mValue.m_str, value.data(), len);
		mValue.m_str[len] = 0;
	}
	mDataType = eType_Str;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const char value)
{
	mValue.m_char = value;
	mDataType = eType_Char;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const bool value)
{
	mValue.m_bool = value;
	mDataType = eType_Bool;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const int value)
{
	mValue.m_int = value;
	mDataType = eType_Int;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const u32 value)
{
	mValue.m_u32 = value;
	mDataType = eType_U32;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const int64_t value)
{
	mValue.m_int64 = value;
	mDataType = eType_Int64;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const u64 value)
{
	mValue.m_u64 = value;
	mDataType = eType_U64;
	mValueSet = true;
	return true;
}


bool
AosGenericField::setValue(const double value)
{
	mValue.m_double = value;
	mDataType = eType_Double;
	mValueSet = true;
	return true;
}


OmnString	
AosGenericField::getValueStr() const
{
	OmnString str;
	switch (mDataType)
	{
	case eType_Str:
		 str = mValue.m_str;
		 return str;

	case eType_Char:
		 str << mValue.m_char;
		 return str;

	case eType_Bool:
		 return (mValue.m_bool)?"true":"false";

	case eType_Int:
		 str << mValue.m_int;
		 return str;

	case eType_Int64:
		 str << mValue.m_int64;
		 return str;

	case eType_U32:
		 str << mValue.m_u32;
		 return str;

	case eType_U64:
		 str << mValue.m_u64;
		 return str;

	case eType_Double:
		 str << mValue.m_double;
		 return str;

	default:
		 break;
	}

	OmnAlarm << "Unrecognized data type: " << mDataType << enderr;
	return "";
}


bool		
AosGenericField::isStrType() const
{
	return (mDataType == eType_Char ||
			mDataType == eType_Str);
}


AosGenericField::DataType
AosGenericField::convertDataType(const char *type)
{
	// 'type' should be one of the following:
	// 		'bool'
	// 		'char'
	// 		'double'
	// 		'int'
	// 		'int64'
	// 		'str'
	// 		'u32'
	// 		'u64'
	// Otherwise, it is an error.
	//
	aos_assert_r(type, eType_Invalid);
	int len = strlen(type);
	aos_assert_r(len >= 3, eType_Invalid);

	switch (type[0])
	{
	case 'b':
		 if (len == 4 && type[1] == 'o' && type[2] == 'o' && type[3] == 'l')
		 {
			 return eType_Bool;
		 }
		 break;

	case 'c':
		 if (len == 4 && type[1] == 'h' && type[2] == 'a' && type[3] == 'r')
		 {
			 return eType_Char;
		 }
		 break;

	case 'd':
		 if (strcmp(type, "double") == 0) return eType_Double;
		 break;

	case 'i':
		 if (type[1] == 'n' && type[2] == 't')
		 {
			 if (len == 3) return eType_Int;
			 if (len == 5 && type[3] == '6' && type[4] == '4') return eType_Int64;
		 }
		 break;

	case 's':
		 if (len == 3 && type[1] == 't' && type[2] == 'r') return eType_Str;
		 break;

	case 'u':
		 if (len == 3 && type[1] == '3' && type[2] == '2') return eType_U32;
		 if (len == 3 && type[1] == '6' && type[2] == '4') return eType_U64;
		 break;

	default:
		break;
	}

	OmnAlarm << "Invalid data type: " << type << enderr;
	return eType_Invalid;
}

bool		
AosGenericField::setValue(
		TiXmlElement *def, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// 'def' shall be in the form:
	// <tag name="fieldname" type="xxx">value</value>
	errcode = eAosXmlInt_SyntaxError;

	const char *txt = def->GetText();
	OmnString str;
	bool tt;
	u32 len;
	mValueSet = false;
	switch (mDataType)
	{
	case eType_Str:
		 if (!txt)
		 {
			 mValue.m_str = 0;
		 }
		 else
		 {
			 int len = strlen(txt);
			 mValue.m_str = (char *)aos_malloc(len+1);
			 if (!mValue.m_str)
			 {
				errmsg = "Failed to allocate memory";
				OmnAlarm << errmsg << enderr;
				return false;
			 }
			 strncpy(mValue.m_str, txt, len);
			 mValue.m_str[len] = 0;
		 }
		 mValueSet = true;
		 return true;

	case eType_Char:
		 if (!txt)
		 {
			 mValue.m_char = 0;
		 }
		 else
		 {
			 mValue.m_char = txt[0];
		 }
		 mValueSet = true;
		 return true;

	case eType_Bool:
		 mValue.m_bool = (txt && strcmp(txt, "true") == 0);
		 mValueSet = true;
		 return true;

	case eType_Int:
		 if (!txt)
		 {
			 mValue.m_int = 0;
			 mValueSet = true;
			 return true;
		 }
		 str = txt;
		 tt = str.parseInt(0, len, mValue.m_int);
		 if (!tt)
		 {
			 errcode = eAosXmlInt_SyntaxError;
			 errmsg = "Failed to parse the integer: ";
			 errmsg << str;
			 OmnAlarm << str << enderr;
			 return false;
		 }
		 mValueSet = true;
		 return true;

	case eType_U32:
		 if (!txt)
		 {
			 mValue.m_u32 = 0;
			 mValueSet = true;
			 return true;
		 }
		 str = txt;
		 tt = str.parseU32(0, len, mValue.m_u32);
		 if (!tt)
		 {
			 errcode = eAosXmlInt_SyntaxError;
			 errmsg = "Failed to parse the unsigned int: ";
			 errmsg << str;
			 OmnAlarm << str << enderr;
			 return false;
		 }
		 mValueSet = true;
		 return true;

	case eType_Int64:
		 if (!txt)
		 {
			 mValue.m_int64 = 0;
			 mValueSet = true;
			 return true;
		 }
		 str = txt;
		 tt = str.parseInt64(0, len, mValue.m_int64);
		 if (!tt)
		 {
			 errcode = eAosXmlInt_SyntaxError;
			 errmsg = "Failed to parse the int64_t: ";
			 errmsg << str;
			 OmnAlarm << str << enderr;
			 return false;
		 }
		 mValueSet = true;
		 return true;

	case eType_U64:
		 if (!txt)
		 {
			 mValue.m_u64 = 0;
			 mValueSet = true;
			 return true;
		 }
		 str = txt;
		 tt = str.parseU64(0, len, mValue.m_u64);
		 if (!tt)
		 {
			 errcode = eAosXmlInt_SyntaxError;
			 errmsg = "Failed to parse the u64: ";
			 errmsg << str;
			 OmnAlarm << str << enderr;
			 return false;
		 }
		 mValueSet = true;
		 return true;

	case eType_Double:
		 if (!txt)
		 {
			 mValue.m_double = 0;
			 mValueSet = true;
			 return true;
		 }
		 str = txt;
		 tt = str.parseDouble(0, len, mValue.m_double);
		 if (!tt)
		 {
			 errcode = eAosXmlInt_SyntaxError;
			 errmsg = "Failed to parse the double: ";
			 errmsg << str;
			 OmnAlarm << str << enderr;
			 return false;
		 }
		 mValueSet = true;
		 return true;

	default:
		 break;
	}	

	errmsg = "Unrecognized data type: ";
	errmsg << mDataType;
	return false;
}


bool
AosGenericField::getDftValue(OmnString &value)
{
	if (!mDftValueSet) return false;
	value = mDftValue;
	return true;
}


void
AosGenericField::resetValue()
{
	if (mDataType == eType_Str && mValue.m_str) aos_free(mValue.m_str);
	mValue.m_str = 0;
	mValueSet = false;
}
