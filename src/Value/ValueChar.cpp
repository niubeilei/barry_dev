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
// 2014/12/17 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Value/ValueChar.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>

AosValueChar::~AosValueChar()
{
}


OmnString 
AosValueChar::toString() const
{
	OmnString str;
	str << "char";
	if (mIsNull) 
	{
		str << "(null)";
	}
	else
	{
		str << mValue;
	}
	return str;
}


AosValueImp&
AosValueChar::operator +(AosValueImp *rhs)
{
	if (!rhs) return *this;

	OmnNotImplementedYet;
	return *this;
}


AosValueImp&
AosValueChar::operator -(const AosValueImp &rhs)
{
	OmnNotImplementedYet;
	return *this;
}


AosValueImp&
AosValueChar::operator *(const AosValueImp &rhs)
{
	OmnNotImplementedYet;
	return *this;
}


AosValueImp&
AosValueChar::operator /(const AosValueImp &rhs)
{
	OmnNotImplementedYet;
	return *this;
}


bool 
AosValueChar::operator ==(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
	case AosDataType::eChar:
	case AosDataType::eInt8:
	case AosDataType::eInt16:
	case AosDataType::eInt32:
	case AosDataType::eInt64:
		 return mValue == rhs.getInt();

	case AosDataType::eU64: 
	case AosDataType::eU32:
	case AosDataType::eU16:
	case AosDataType::eU8:
		 return mValue == rhs.getChar();

	default:
		 cout << "Datatype mismatch" << endl;
		 return 0;		
		eInvalid, 

		eBool,
		eFloat,
		eDouble,
		eVarInt,				// Chen Ding, 2014/12/04
		eVarUint,				// Chen Ding, 2014/12/04
		eOmnStr,
		eLastFixedField,

		// The following are variable length data types

		eString, 
		eXmlDoc,
		eQueryRslt,
		eCharStr,				// Chen Ding, 05/12/2012
		eCharStrWithDocid,		// Chen Ding, 07/08/2012
		eOmnStrWithDocid,		// Chen Ding, 07/08/2012
		eBuff,					// Chen Ding, 2013/03/17
		eUTF8Str,				// Chen Ding, 2014/01/02
		eArray,					// Chen Ding, 2014/01/02
		eNull,
		eBinary,
		eEmbeddedDoc,
		eDate,					// Andy zhang, 2014/09/03

		eMaxDataType
	}
}


bool 
AosValueChar::operator !=(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eChar:
			return mValue != rhs.getChar();
		default:
			cout << "Datatype mismatch" << endl;
			return 0;		
	}
}


bool 
AosValueChar::operator >(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eChar:
			return mValue > rhs.getChar();
		default:
			cout << "Datatype mismatch" << endl;
			return 0;		
	}
}


bool 
AosValueChar::operator >=(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eChar:
			return mValue >= rhs.getChar();
		default:
			cout << "Datatype mismatch" << endl;
			return 0;		
	}
}


bool 
AosValueChar::operator <(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eChar:
			return mValue < rhs.getChar();
		default:
			cout << "Datatype mismatch" << endl;
			return 0;		
	}
}


bool 
AosValueChar::operator <=(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eChar:
			return mValue <= rhs.getChar();
		default:
			cout << "Datatype mismatch" << endl;
			return 0;		
	}
}

ostream & operator<<(ostream &os,const AosValueChar &rhs)
{
    os << rhs.mValue;
    return os;
}

istream & operator>>(istream &is,const AosValueChar &rhs)
{
    is >> rhs.mValue;
    return is;
}

bool 
AosValueChar::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueChar::setU64(const u64 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueChar::setU32(const u32 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueChar::setInt64(const int64_t vv)
{	
	if (vv < 0)
	{
		cout << "Setting a negative to unsigned char " << vv << endl;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueChar::setInt(const int vv)
{
	if (vv < 0)
	{
		cout << "Setting a negative to unsigned char " << vv << endl;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueChar::setDouble(const double vv)
{
	if (vv < 0)
	{
		cout << "Setting a negative to unsigned char " << vv << endl;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueChar::setOmnStr(const OmnString &vv)
{
	cout << "Data types imcompatible: OmnString to char" << endl;
	return false;
}

bool 
AosValueChar::setChar(const char vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueChar::setCharStr(const char *data, const int len, const bool copy_flag)
{
	cout << "Data types imcompatible: char string to char" << endl;
	return false;
}


bool 
AosValueChar::setXml(const AosXmlTagPtr &vv)
{
	cout << "Setting an XML to char not allowed!" << endl;
	return false;
}


bool 
AosValueChar::setBuff(const AosBuffPtr &vv)
{
	cout << "Setting an XML to char not allowed!" << endl;
	return false;
}

bool
AosValueChar::setU8(const u8 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueChar::setU16(const u16 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueChar::setInt8(const i8 vv)
{
	if (vv < 0)
	{
		cout << "Setting a negative to unsigned char " << vv << endl;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueChar::setInt16(const i16 vv)
{
	if (vv < 0)
	{
		cout << "Setting a negative to unsigned char " << vv << endl;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueChar::setFloat(const float vv)
{
	if (vv < 0)
	{
		cout << "Setting a negative to unsigned char " << vv << endl;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

u8
AosValueChar::getU8()const
{
	if(mIsNull)
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return (u8)mValue;
}

u16
AosValueChar::getU16()const
{
	if(mIsNull)
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return (u16)mValue;
}

i8
AosValueChar::getInt8()const
{
	if(mIsNull)
	{
		cout << "Value is null" << endl;
		return -1;
	}
	if (mValue < 0) 
	{
		cout << "Getting negative value to char: " << mValue << endl;
		return 0;
	}
	return (i8)mValue;
}

i16
AosValueChar::getInt16()const
{
	if(mIsNull)
	{
		cout << "Value is null" << endl;
		return -1;
	}
	if (mValue < 0) 
	{
		cout << "Getting negative value to unsigned char: " << mValue << endl;
		return 0;
	}
	return (i16)mValue;
}

float
AosValueChar::getFloat()const
{
	if(mIsNull)
	{
		cout << "Value is null" << endl;
		return 0.0;
	}
	if (mValue < 0) 
	{
		cout << "Getting negative value to unsigned char: " << mValue << endl;
		return 0;
	}
	return (float)mValue;
}




OmnString 
AosValueChar::getOmnStr() const
{
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueChar::getCharStr(int &len ) const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return 0;
}


AosQueryRsltObjPtr 
AosValueChar::getQueryRslt(const AosRundataPtr &rdata) const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return 0;
}


AosXmlTagPtr 
AosValueChar::getXml() const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return 0;
	}

	return 0;
}


double 	
AosValueChar::getDouble() const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return 0.0;
	}
	if (mValue < 0) 
	{
		cout << "Getting negative value to unsigned char: " << mValue << endl;
		return 0;
	}
	return mValue;
}


int 	
AosValueChar::getInt() const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return -1;
	}
	if (mValue < 0) 
	{
		cout << "Getting negative value to unsigned char: " << mValue << endl;
		return 0;
	}
	return mValue;
}


i64 	
AosValueChar::getInt64() const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return -1;
	}
	if (mValue < 0) 
	{
		cout << "Getting negative value to unsigned char: " << mValue << endl;
		return 0;
	}
	return mValue;
}


u64 	
AosValueChar::getU64() const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return (u64)mValue;
}


u32 	
AosValueChar::getU32() const
{
	if (mIsNull) 
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return (u32)mValue;
}


char 	
AosValueChar::getChar() const
{
	if (mIsNull)
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return mValue;
}


bool 	
AosValueChar::getBool() const
{
	if (mIsNull)
	{
		cout << "Value is null" << endl;
		return 0;
	}
	return mValue;
}
#endif
