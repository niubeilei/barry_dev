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
// Modification History:
// 08/08/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_DataType_h
#define Snt_Util_DataType_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/ValueDefs.h"

#define AOSDATATYPE_BOOL				"bool"
#define AOSDATATYPE_BUFF				"buff"
#define AOSDATATYPE_CHAR				"char"
#define AOSDATATYPE_CHARSTR_W_DOCID		"charstrwd"
#define AOSDATATYPE_CHARSTRING			"varchar"
#define AOSDATATYPE_DOUBLE				"double"
#define AOSDATATYPE_DATE				"date"
#define AOSDATATYPE_FLOAT				"float"
#define AOSDATATYPE_INT64				"int64"
#define AOSDATATYPE_INT32				"int32"
#define AOSDATATYPE_INT16				"int16"
#define AOSDATATYPE_INT8				"int8"
#define AOSDATATYPE_QUERYRSLT			"queryrslt"
#define AOSDATATYPE_OMNSTR_W_DOCID		"omnstrwd"
#define AOSDATATYPE_STRING				"string"
#define AOSDATATYPE_STR					"str"
#define AOSDATATYPE_STRING_W_DOCID		"wdstr"
#define AOSDATATYPE_U64					"u64"
#define AOSDATATYPE_U32					"u32"
#define AOSDATATYPE_U16					"u16"
#define AOSDATATYPE_U8					"u8"
#define AOSDATATYPE_XMLDOC				"xmldoc"
#define AOSDATATYPE_NUMBER				"number"
#define AOSDATATYPE_DATETIME			"datetime"
#define AOSDATATYPE_TIME_DURATION		"time_duration"

#define AOSDATATYPE_INVALID				"invalid"

#define AOSFILENAME_TYPE_I64            "__i64"
#define AOSFILENAME_TYPE_U64            "__u64"
#define AOSFILENAME_TYPE_DATETIME       "__dt"


#define DATE_SIZE						20

class AosDataType
{
public:
	enum E
	{
		eInvalid, 

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// The following datatype are basic types
		// Don't change it and the order. 
		// if you want change it .please ask to ice.
		eInt8,
		eU64, 
		eU32,
		eU16,
		eU8,
		eChar,
		eInt64,
		eInt32,
		eInt16,
 		eBool,
		eFloat,
		eDouble,
		eShortString, // lenghts < 15 
		eNull,
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		eLongString, 				
		eString, 				
		eVarInt	= 100, 			// Chen Ding, 2014/12/04
		eVarUint,				// Chen Ding, 2014/12/04
		eOmnStr,
		eLastFixedField,
		eXmlDoc,
		eQueryRslt,
		eCharStr,				// Chen Ding, 05/12/2012
		eCharStrWithDocid,		// Chen Ding, 07/08/2012
		eOmnStrWithDocid,		// Chen Ding, 07/08/2012
		eBuff,					// Chen Ding, 2013/03/17
		eUTF8Str,				// Chen Ding, 2014/01/02
		eArray,					// Chen Ding, 2014/01/02

		eBinary,
		eEmbeddedDoc,
		eDate,					// Andy zhang, 2014/09/03
		eNumber,				// Young, 2014/12/15
		eDateTime,				// Young, 2015/06/15
		eTimeDuration,			// Young, 2015/06/19

		eMaxDataType
	};

	static bool isValid(const E code)
	{
		return code > eInvalid && code < eMaxDataType;
	}

	static bool isPrimitiveType(const E code)
	{
		return code >= eU64 && code < eString;
	}

	static bool isNumericType(const E type)
	{
		switch (type)	
		{
		case eU64: 
		case eU32:
		case eU16:
		case eU8:
		case eInt64:
		case eInt32:
		case eInt16:
		case eInt8:
		case eBool:
		case eFloat:
		case eDouble:
		case eNumber:
		case eTimeDuration:
			return true;
		default:	
			return false;
		}
		return false;
	}

	static bool isCharacterType(const E type)
	{
		switch (type)
		{
		case eChar: 
		case eString: 
		case eShortString: 
		case eLongString: 
		case eCharStr:
			return true;
		default:
			return false;
		}
		return false;
	}

	static E getFirstEntry() {return (E)(eInvalid+1);}
	static E getLastEntry() {return (E)(eMaxDataType-1);}
	static OmnString getTypeStr(const E type)
	{
		switch (type)
		{
		case eU64: 				return AOSDATATYPE_U64;
		case eU32: 				return AOSDATATYPE_U32;
		case eU16: 				return AOSDATATYPE_U16;
		case eU8: 				return AOSDATATYPE_U8;
		case eInt64: 			return AOSDATATYPE_INT64;
		case eInt32: 			return AOSDATATYPE_INT32;
		case eInt16: 			return AOSDATATYPE_INT16;
		case eInt8: 			return AOSDATATYPE_INT8;
		case eBool: 			return AOSDATATYPE_BOOL;
		case eFloat:			return AOSDATATYPE_FLOAT;
		case eDouble:			return AOSDATATYPE_DOUBLE;
		case eDate:				return AOSDATATYPE_DATE;
		case eString: 			return AOSDATATYPE_STRING;
		case eXmlDoc: 			return AOSDATATYPE_XMLDOC;
		case eNumber:			return AOSDATATYPE_NUMBER;
		case eDateTime:			return AOSDATATYPE_DATETIME;
		case eTimeDuration:		return AOSDATATYPE_TIME_DURATION;
		case eQueryRslt:		return AOSDATATYPE_QUERYRSLT;
		case eChar:				return AOSDATATYPE_CHAR; 
		case eCharStr:			return AOSDATATYPE_CHARSTRING; 	// Chen Ding, 05/12/2012
		case eCharStrWithDocid:	return AOSDATATYPE_CHARSTR_W_DOCID;
		case eOmnStrWithDocid:	return AOSDATATYPE_OMNSTR_W_DOCID;
		case eBuff:				return AOSDATATYPE_BUFF;
		default: 				return AOSDATATYPE_INVALID;
		}
		return AOSDATATYPE_INVALID;
	}

	static E toEnum(const OmnString &name)
	{
		if (name.length() < 2) return eInvalid;
		switch (name.data()[0])
		{
		case 'b':
			 if (name == AOSDATATYPE_BOOL)	return eBool;
			 if (name == AOSDATATYPE_BUFF)	return eBuff;
			 break;

		case 'c':
			 if (name == AOSDATATYPE_CHAR)				return eChar;
			 if (name == AOSDATATYPE_CHARSTRING)		return eCharStr;
			 if (name == AOSDATATYPE_CHARSTR_W_DOCID)	return eCharStrWithDocid;
			 break;

		case 'd':
			 if (name == AOSDATATYPE_DOUBLE)			return eDouble;	
			 if (name == AOSDATATYPE_DATE)				return eDate;	
			 if (name == AOSDATATYPE_DATETIME)			return eDateTime;	
			 break;

		case 'f':
			 if (name == AOSDATATYPE_FLOAT)				return eFloat;	
			 break;

		case 'i':
			 if (name.length() < 4) return eInvalid;
			 switch (name.data()[3])
			 {
			 case '6':
				  if (name == AOSDATATYPE_INT64)	return eInt64;
				  break;

			 case '3':
				  if (name == AOSDATATYPE_INT32) 	return eInt32;
				  break;

			 case '1':
				  if (name == AOSDATATYPE_INT16) 	return eInt16;
				  break;

			 default:
				  return eInvalid;
			 }
			 break;

		case 'n':
			 if (name == AOSDATATYPE_NUMBER) 		return eNumber;
			 break;

		case 's':
			 if (name == AOSDATATYPE_STR) 	return eString;
			 if (name == AOSDATATYPE_STRING) 	return eString;
			 if (name == AOSDATATYPE_STR) 	return eString;
			 break;

		case 'o':
		 	 if (name == AOSDATATYPE_OMNSTR_W_DOCID)	return eOmnStrWithDocid;
		 	 break;

		case 'u':
			 switch (name.data()[1])
			 {
			 case '6':
				  if (name == AOSDATATYPE_U64) 		return eU64;
				  break;

			 case '3':
				  if (name == AOSDATATYPE_U32) 		return eU32;
				  break;

			 case '1':
				  if (name == AOSDATATYPE_U16) 		return eU16;
				  break;

			 case '8':
				  if (name == AOSDATATYPE_U8) 		return eU8;
				  break;

			 default:
				  return eInvalid;
			 }
			 break;

		case 'x':
			 if (name == AOSDATATYPE_XMLDOC) 	return eXmlDoc;
			 break;

		case 'q':
		 	 if (name == AOSDATATYPE_QUERYRSLT)	return eQueryRslt;
		 	 break;

		case 'v':
		 	 if (name == AOSDATATYPE_CHARSTRING)	return eCharStr;
		 	 break;

		default:
			 break;
		}
		return eInvalid;
	}

	static int getValueSize(const E datatype)
	{
		switch (datatype)
		{
		case eU64: 		return sizeof(u64);
		case eU32: 		return sizeof(u32);
		case eU16: 		return sizeof(u16);
		case eU8: 		return sizeof(u8);
		case eInt64: 	return sizeof(int64_t);
		case eInt32: 	return sizeof(int32_t);
		case eInt16: 	return sizeof(int16_t);
		case eBool: 	return sizeof(bool);
		case eChar: 	return sizeof(char);
		case eFloat: 	return sizeof(double);
		case eDouble: 	return sizeof(double);
		case eNumber: 	return sizeof(double);
		case eDate: 	return DATE_SIZE;
		default: 		return -1;
		}
		return -1;
	}

	static OmnString getFieldName(const OmnString name, const OmnString type)
	{
		OmnString str = name;
		if (type == AOSDATATYPE_U64)
			str << AOSFILENAME_TYPE_U64;
		if (type == AOSDATATYPE_DATE)
			str << AOSFILENAME_TYPE_DATETIME;
		return str;                                                            
	}

	static bool inline isNumerical(const E code)
	{
		switch (code)
		{
		case eInt8:
		case eU64: 
		case eU32:
		case eU16:
		case eU8:
		case eChar:
		case eInt64:
		case eInt32:
		case eInt16:
		case eFloat:
		case eDouble:
		case eVarInt:
		case eVarUint:
			 return true;

		default:
			 return false;
		}
	}

	static E autoTypeConvert(const E t1, const E t2)
	{
		if (t1 == t2) 
		{
			if (t1 == eDateTime) return eInt64;
			return t1;		
		}

		if (isNumericType(t1) && isNumericType(t2))
		{
			AosDataType::E tt1 = typePromotion(t1);
			AosDataType::E tt2 = typePromotion(t2);
			if (tt1 == eDouble || tt2 == eDouble) return eDouble;
			else if (tt1 == eInt64 || tt2 == eInt64) return eInt64;
			else if (tt1 == eU64 || tt2 == eU64) return eU64;
		}
		else if (isCharacterType(t1) && isCharacterType(t2))
		{
			return eString;
		}

		return eInvalid;
	}

	static E typePromotion(const E t)
	{
		switch (t)
		{
		case eFloat:
		case eDouble:
		case eNumber:
			return AosDataType::eDouble;

		case eU64: 
		case eU32:
		case eU16:
		case eU8:
			return AosDataType::eU64;

		case eInt64:
		case eInt32:
		case eInt16:
		case eInt8:
			return AosDataType::eInt64;

		default:
			break;
		}
		return eInvalid;
	}

};

#endif
