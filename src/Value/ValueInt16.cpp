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
// 2014/12/15 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Value/ValueInt16.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>
#include "Value/ValueU8.h"
#include "Value/ValueU16.h"
#include "Value/ValueU32.h"
#include "Value/ValueU64.h"
#include "Value/ValueInt8.h"
#include "Value/ValueInt16.h"
#include "Value/ValueInt.h"
#include "Value/ValueInt64.h"
#include "Value/ValueFloat.h"
#include "Value/ValueDouble.h"
AosValueInt16::~AosValueInt16()
{
}

AosValueImp*
AosValueInt16::getAosValue(const double value)
{
	long long int iValue = value;
	AosValueImp *rslt;
	if (iValue == value)
	{
		if (value >= CHAR_MIN && value <= CHAR_MAX)
		{
			rslt = new AosValueInt8(value);
		}
		else if (value >= 0 && value <= UCHAR_MAX)
		{
			rslt = new AosValueU8(value);
		}
		else if (value >= SHRT_MIN && value <= SHRT_MAX)
		{
			rslt = new AosValueInt16(value);
		}
		else if (value >= 0 && value <= USHRT_MAX)
		{
			rslt = new AosValueU16(value);
		}
		else if (value >= INT_MIN && value <= INT_MAX)
		{
			rslt = new AosValueInt(value);
		}
		else if (value >= 0 && value <= UINT_MAX)
		{
			rslt = new AosValueU32(value);
		}
		else if (value >= LLONG_MIN && value <= LLONG_MAX)
		{
			rslt = new AosValueInt8(value);
		}
		else if (value >= 0 && value <= ULLONG_MAX)
		{
			rslt = new AosValueU64(value);
		}
	}
	else
	{
		rslt = new AosValueDouble(value);
	}
	return rslt;
}

OmnString 
AosValueInt16::toString() const
{
	OmnString str;
	str << "int16 ";
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
AosValueInt16::operator +(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	AosValueImp *rslt;
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
		{
			double value = (double)mValue + rhs.getDouble();
			rslt = getAosValue(value);
			break;
		}
		default:
		{
			OmnAlarm << "Data type mismatch" << enderr;
		}
	}
	return *rslt;
}


AosValueImp&
AosValueInt16::operator -(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	AosValueImp *rslt;
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
		{
			double value = (double)mValue - rhs.getDouble();
			rslt = getAosValue(value);
			break;
		}
		default:
		{
			OmnAlarm << "Data type mismatch" << enderr;
		}
	}
	return *rslt;
}


AosValueImp&
AosValueInt16::operator *(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	AosValueImp *rslt;
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
		{
			double value = (double)mValue * rhs.getDouble();
			rslt = getAosValue(value);
			break;
		}
		default:
		{
			OmnAlarm << "Data type mismatch" << enderr;
		}
	}
	return *rslt;
}


AosValueImp&
AosValueInt16::operator /(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	AosValueImp *rslt;
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
		{
			double value = (double)mValue / rhs.getDouble();
			rslt = getAosValue(value);
			break;
		}
		default:
		{
			OmnAlarm << "Data type mismatch" << enderr;
		}
	}
	return *rslt;
}


bool 
AosValueInt16::operator ==(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
			return(double)mValue == rhs.getDouble();
		default:
			OmnAlarm << "Datatype mismatch" << enderr;
			return false ;		
	}
}


bool 
AosValueInt16::operator !=(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
			return(double)mValue != rhs.getDouble();
		default:
			OmnAlarm << "Datatype mismatch" << enderr;
			return false ;		
	}
}


bool 
AosValueInt16::operator >(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
			return(double)mValue > rhs.getDouble();
		default:
			OmnAlarm << "Datatype mismatch" << enderr;
			return false;		
	}
}


bool 
AosValueInt16::operator >=(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
			return(double)mValue >= rhs.getDouble();
		default:
			OmnAlarm << "Datatype mismatch" << enderr;
			return false ;		
	}
}


bool 
AosValueInt16::operator <(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
			return(double)mValue < rhs.getDouble();
		default:
			OmnAlarm << "Datatype mismatch" << enderr;
			return false;		
	}
}


bool 
AosValueInt16::operator <=(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	switch(dataType)
	{
		case AosDataType::eDouble:
		case AosDataType::eFloat:
		case AosDataType::eInt8:
		case AosDataType::eInt16:
		case AosDataType::eInt32:
		case AosDataType::eInt64:
		case AosDataType::eU8:
		case AosDataType::eU16:
		case AosDataType::eU32:
		case AosDataType::eU64:
			return(double)mValue <= rhs.getDouble();
		default:
			OmnAlarm << "Datatype mismatch" << enderr;
			return false ;		
	}
}

ostream & operator<<(ostream &os, const AosValueInt16 &rhs)
{
	os << rhs.mValue;
	return os;
}

istream & operator>>(istream &is, const AosValueInt16 &rhs)
{
	is >> rhs.mValue;
	return is;
}

bool
AosValueInt16::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueInt16::setU8(const u8 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt16::setU16(const u16 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt16::setInt8(const i8 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt16::setInt16(const i16 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt16::setFloat(const float vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueInt16::setU64(const u64 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueInt16::setU32(const u32 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueInt16::setInt64(const int64_t vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 	
AosValueInt16::setInt(const int vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueInt16::setDouble(const double vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueInt16::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data types imcompatible: OmnString to int64" << enderr;
	return false;
}

bool
AosValueInt16::setChar(const char vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool 
AosValueInt16::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data types imcompatible: char string to int64" << enderr;
	return false;
}


bool 
AosValueInt16::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Setting an XML to int not allowed!" << enderr;
	return false;
}


bool 
AosValueInt16::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Setting an XML to int not allowed!" << enderr;
	return false;
}

u8
AosValueInt16::getU8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	if (mValue < 0)
	{
		OmnAlarm << "Getting negative value to u8: " << mValue << enderr;
		return 0;
	}
	return (u8)mValue;
}

u16
AosValueInt16::getU16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	if (mValue < 0)
	{
		OmnAlarm << "Value is null" << enderr;
		OmnAlarm << "Getting negative value to u16: " << mValue << enderr;
		return 0;
	}
	return (u16)mValue;
}

i8
AosValueInt16::getInt8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i8)mValue;
}

i16
AosValueInt16::getInt16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i16)mValue;
}

float
AosValueInt16::getFloat()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return (float)mValue;
}


OmnString 
AosValueInt16::getOmnStr() const
{
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueInt16::getCharStr(int &len ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosQueryRsltObjPtr 
AosValueInt16::getQueryRslt(const AosRundataPtr &rdata ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosXmlTagPtr 
AosValueInt16::getXml() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


double 	
AosValueInt16::getDouble() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return mValue;
}


int 	
AosValueInt16::getInt() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


i64 	
AosValueInt16::getInt64() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


u64 	
AosValueInt16::getU64() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	if (mValue < 0) 
	{
		OmnAlarm << "Getting negative value to u64: " << mValue << enderr;
		return 0;
	}
	return (u64)mValue;
}


u32 	
AosValueInt16::getU32() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	if (mValue < 0) 
	{
		OmnAlarm << "Getting negative value to u32: " << mValue << enderr;
		return 0;
	}
	return (u32)mValue;
}


char 	
AosValueInt16::getChar() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}


bool 	
AosValueInt16::getBool() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}

#endif
