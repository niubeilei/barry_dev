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
#include "Value/ValueFloat.h"

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
#include "Value/ValueInt64.h"
#include "Value/ValueInt16.h"
#include "Value/ValueInt.h"
#include "Value/ValueFloat.h"
#include "Value/ValueDouble.h"

#define FLOAT_MAX
#define FLOAT_MIN

AosValueFloat::~AosValueFloat()
{
}


AosValueImp * 
AosValueFloat::getAosValue(const double value)
{
	long long int iValue = value;
	AosValueImp *rslt;
	if(iValue==value)
	{
		if(value>= CHAR_MIN && value<=CHAR_MAX)
		{
			rslt = new AosValueInt8(value);
		}
		else if(value >= 0 && value <= UCHAR_MAX)
		{
			rslt = new AosValueU8(value);
		}
		else if(value >= SHRT_MIN && value <= SHRT_MAX)
		{
			rslt = new AosValueInt16(value);
		}
		else if(value >= 0 && value <= USHRT_MAX)
		{
			rslt = new AosValueU16(value);
		}
		else if(value >= INT_MIN && value <= INT_MAX)
		{
			rslt = new AosValueInt(value);
		}
		else if(value >= 0 && value <= UINT_MAX)
		{
			rslt = new AosValueU32(value);
		}
		else if(value >= LLONG_MIN && value <= LLONG_MAX)
		{
			rslt = new AosValueInt64(value);
		}
		else if(value >= 0 && value <= ULLONG_MAX)
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
AosValueFloat::toString() const
{
	OmnString str;
	str << "double ";
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
AosValueFloat::operator +(const AosValueImp &rhs)
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
			return *this;
		}
	}
	return *rslt;
}


AosValueImp&
AosValueFloat::operator -(const AosValueImp &rhs)
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
			return *this;
		}
	}
	return *rslt;
}


AosValueImp&
AosValueFloat::operator *(const AosValueImp &rhs)
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
			return *this;
		}
	}
	return *rslt;
}


AosValueImp&
AosValueFloat::operator /(const AosValueImp &rhs)
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
			return *this;
		}
	}
	return *rslt;
}


bool 
AosValueFloat::operator ==(const AosValueImp &rhs)
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
			return false;		
	}
}


bool 
AosValueFloat::operator !=(const AosValueImp &rhs)
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
			return false;		
	}
}


bool 
AosValueFloat::operator >(const AosValueImp &rhs)
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
AosValueFloat::operator >=(const AosValueImp &rhs)
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
			return false;		
	}
}


bool 
AosValueFloat::operator <(const AosValueImp &rhs)
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
AosValueFloat::operator <=(const AosValueImp &rhs)
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
			return false;		
	}
}

ostream & operator<<(ostream &os,const AosValueFloat &rhs)
{
    os << rhs.mValue;
    return os;
}

istream & operator>>(istream &is,const AosValueFloat &rhs)
{
    is >> rhs.mValue;
    return is;
}

bool 
AosValueFloat::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueFloat::setU8(const u8 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueFloat::setU16(const u16 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueFloat::setInt8(const i8 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueFloat::setInt16(const i16 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueFloat::setFloat(const float vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueFloat::setU64(const u64 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueFloat::setU32(const u32 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueFloat::setInt64(const int64_t vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i64 to float negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueFloat::setInt(const int vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueFloat::setDouble(const double vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool 
AosValueFloat::setChar(const char vv)
{
	OmnAlarm << "Data types imcompatible ::char to float" << enderr;
	return true;
}


bool 
AosValueFloat::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data types imcompatible: OmnStr to float" << enderr;
	return false;
}


bool 
AosValueFloat::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data types imcompatible: char str to double" << enderr;
	return false;
}


bool 
AosValueFloat::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Setting an XML to int not allowed!" << enderr;
	return false;
}


bool 
AosValueFloat::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Setting an XML to int not allowed!" << enderr;
	return false;
}



u8
AosValueFloat::getU8()const
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
AosValueFloat::getU16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	if (mValue < 0)
	{
		OmnAlarm << "Getting negative value to u16: " << mValue << enderr;
		return 0;
	}
	return (u16)mValue;
}

i8
AosValueFloat::getInt8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i8)mValue;
}

i16
AosValueFloat::getInt16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i16)mValue;
}

float
AosValueFloat::getFloat()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return (float)mValue;
}


OmnString 
AosValueFloat::getOmnStr() const
{
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueFloat::getCharStr(int &len) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosQueryRsltObjPtr 
AosValueFloat::getQueryRslt(const AosRundataPtr &rdata) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosXmlTagPtr 
AosValueFloat::getXml() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


double 	
AosValueFloat::getDouble() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return mValue;
}


int 	
AosValueFloat::getInt() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


i64 	
AosValueFloat::getInt64() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


u64 	
AosValueFloat::getU64() const
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
AosValueFloat::getU32() const
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
AosValueFloat::getChar() const
{
	if (mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}


bool 	
AosValueFloat::getBool() const
{
	if (mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}

#endif

