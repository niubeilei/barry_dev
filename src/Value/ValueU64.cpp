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
// 2014/12/14 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#include "Value/ValueU64.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>
#include "Value/Value.h"
// #include "Value/ValueU16.h"
// #include "Value/ValueU32.h"
// #include "Value/ValueU64.h"
// #include "Value/ValueInt8.h"
// #include "Value/ValueInt16.h"
// #include "Value/ValueInt.h"
// #include "Value/ValueInt64.h"
// #include "Value/ValueDouble.h"
// #include "Value/ValueFloat.h"



AosValueU64::~AosValueU64()
{
}


/*
AosValueImp *
AosValueU64::getAosValue(const double value)
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
			rslt = new AosValueInt64(value);
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
*/


OmnString 
AosValueU64::toString(const bool is_null) const
{
	OmnString str;
	str << "u64 ";
	if (is_null) 
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
AosValueU64::add(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 u64 vv = rhs.getU64();
		 mValue += vv;
		 return *this;
	}

	OmnAlarm << "Dividing u64 by: " << AosDataType::getTypeStr(dataType) << enderr;
	return *this;
}


AosValueImp &
AosValueU64::minus(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 u64 vv = rhs.getU64();
		 mValue -= vv;
		 return *this;
	}

	OmnAlarm << "Dividing u64 by: " << AosDataType::getTypeStr(dataType) << enderr;
	return *this;
}


AosValueImp&
AosValueU64::multiply(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 u64 vv = rhs.getU64();
		 mValue *= vv;
		 return *this;
	}

	OmnAlarm << "Dividing u64 by: " << AosDataType::getTypeStr(dataType) << enderr;
	return *this;
}


AosValueImp&
AosValueU64::divide(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 u64 vv = rhs.getU64();
		 if (vv == 0)
		 {
			 OmnAlarm << "dividing by zero" << enderr;
			 return *this;
		 }
		 mValue /= vv;
		 return *this;
	}

	OmnAlarm << "Dividing u64 by: " << AosDataType::getTypeStr(dataType) << enderr;
	return *this;
}


bool 
AosValueU64::checkEQ(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 return(double)mValue == rhs.getDouble();
	}
	return false;		
}


bool 
AosValueU64::checkNE(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 return(double)mValue != rhs.getDouble();
	}
	return true;
}


bool 
AosValueU64::checkGT(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 return(double)mValue > rhs.getDouble();
	}
	return false;		
}


bool 
AosValueU64::checkGE(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 return(double)mValue >= rhs.getDouble();
	}
	return false;		
}


bool 
AosValueU64::checkLT(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 return(double)mValue < rhs.getDouble();
	}
	return false;		
}


bool 
AosValueU64::checkLE(const AosValue &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	if (AosDataType::isNumerical(dataType))
	{
		 return(double)mValue <= rhs.getDouble();
	}
	return false;		
}


ostream & operator<<(ostream &os,const AosValueU64 &rhs)
{
    os << rhs.mValue;
    return os;
}


istream & operator>>(istream &is,const AosValueU64 &rhs)
{
    is >> rhs.mValue;
    return is;
}


bool 
AosValueU64::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueU64::setU8(const u8 vv)
{
	mValue  = vv;
	return true;
}
bool
AosValueU64::setU16(const u16 vv)
{
	mValue  = vv;
	return true;
}

bool
AosValueU64::setInt8(const i8 vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Setting i8 to u64 negative overflow:" << enderr;
		return false;
	}
	mValue  = vv;
	return true;
}

bool
AosValueU64::setInt16(const i16 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i16 to u64 negative overflow:" << enderr;
		return false;
	}
	mValue  = vv;
	return true;
}

bool
AosValueU64::setFloat(const float vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting float to u64 negative overflow:" << enderr;
		return false;
	}
	mValue  = vv;
	return true;
}


bool
AosValueU64::setU64(const u64 vv)
{
	mValue = vv;
	return true;
}


bool
AosValueU64::setChar(const char vv)
{
	mValue = vv;
	return true;
}


bool
AosValueU64::setU32(const u32 vv)
{
	mValue = vv;
	return true;
}


bool 
AosValueU64::setInt64(const int64_t vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative value to u64" << enderr;
	}

	mValue = vv;
	return true;
}


bool 
AosValueU64::setInt(const int vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative value to u64" << enderr;
	}
	mValue = vv;
	return true;
}


bool 
AosValueU64::setDouble(const double vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative value to u64" << enderr;
	}

	mValue = vv;
	return true;
}


bool 
AosValueU64::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data types mismatch: OmnString to u64" << enderr;
	return false;
}


bool 
AosValueU64::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data types mismatch: CharStr to u64" << enderr;
	return false;
}


bool 
AosValueU64::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Setting an XML to u64 not allowed!" << enderr;
	return false;
}


bool 
AosValueU64::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Setting an XML to u64 not allowed!" << enderr;
	return false;
}


u8
AosValueU64::getU8()const
{
	return (u8)mValue;
}


u16
AosValueU64::getU16()const
{
	return (u16)mValue;
}


i8
AosValueU64::getInt8()const
{
	return (i8)mValue;
}


i16
AosValueU64::getInt16()const
{
	return (i16)mValue;
}

float
AosValueU64::getFloat()const
{
	return (float)mValue;
}


OmnString 
AosValueU64::getOmnStr() const
{
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueU64::getCharStr(int &len) const
{
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr 
AosValueU64::getXml() const
{
	OmnShouldNeverComeHere;
	return 0;
}


double 	
AosValueU64::getDouble() const
{
	return mValue;
}


int 	
AosValueU64::getInt() const
{
	return mValue;
}


i64 	
AosValueU64::getInt64() const
{
	return mValue;
}


u64 	
AosValueU64::getU64() const
{
	return mValue;
}


u32 	
AosValueU64::getU32() const
{
	return (u32)mValue;
}


char 	
AosValueU64::getChar() const
{
	return mValue;
}


bool 	
AosValueU64::getBool() const
{
	return mValue;
}


AosValueImpPtr
AosValueU64::clone()
{
	return OmnNew AosValueU64(mValue);
}

