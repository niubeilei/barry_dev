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
#include "Value/ValueInt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>
#include "Value/ValueU8.h"
#include "Value/ValueU32.h"
#include "Value/ValueU16.h"
#include "Value/ValueU64.h"
#include "Value/ValueInt8.h"
#include "Value/ValueInt16.h"
#include "Value/ValueInt.h"
#include "Value/ValueInt64.h"
#include "Value/ValueFloat.h"
#include "Value/ValueDouble.h"

AosValueInt::~AosValueInt()
{
}

AosValueImp *
AosValueInt::getAosValue(const double value)
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
		else if(value >=0 && value <= USHRT_MAX)
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
AosValueInt::toString() const
{
	OmnString str;
	str << "int ";
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


/*
bool 
AosValueInt::getCharStr(char *data, int &len, const bool copy) const
{
	if (mIsNull) return false;

	if (!data)
	{
		OmnAlarm << "data is null" << enderr;
		len = 0;
		return false;
	}

	OmnString str;
	str << mValue;
	if (str.length() >= len)
	{
		OmnAlarm << "data too short: " << len << ":" << str.length() 
			<< ":" << mValue << enderr;
		return false;
	}

	strcpy(data, str.data());
	return true;
}

*/

bool
AosValueInt::setU8(const u8 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt::setU16(const u16 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt::setInt8(const i8 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt::setInt16(const i16 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueInt::setFloat(const float vv)
{
	if (vv > INT_MAX)
	{
		OmnAlarm << "Setting float to int positive overflow:" << enderr;
		return false;
	}
	if (vv < INT_MIN)
	{
		OmnAlarm << "Setting float to int negative overflow: " << enderr;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueInt::setChar(const char vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueInt::setU64(const u64 vv)
{
	if (vv > INT_MAX)
	{
		OmnAlarm << "Setting u64 to int overflow: " << vv << enderr;
		return false;
	}

	mIsNull = false;
	mValue = (int)vv;
	return true;
}


bool
AosValueInt::setU32(const u32 vv)
{
	if (vv > INT_MAX)
	{
		OmnAlarm << "Setting u32 to int overflow: " << vv << enderr;
		return false;
	}
	mIsNull = false;
	mValue = (int)vv;
	return true;
}


bool 
AosValueInt::setInt64(const int64_t vv)
{
	if (vv > INT_MAX)
	{
		OmnAlarm << "Setting int64 to int positive overflow: " << vv << enderr;
		return false;
	}

	if (vv < INT_MIN)
	{
		OmnAlarm << "Setting int64 to int negative overflow: " << vv << enderr;
		return false;
	}
	mIsNull = false;
	mValue = (int)vv;
	return true;
}


bool 
AosValueInt::setInt(const int vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueInt::setDouble(const double vv)
{
	if (vv > INT_MAX)
	{
		OmnAlarm << "Setting double to int positive overflow: " << vv << enderr;
		return false;
	}
	if (vv < INT_MIN)
	{
		OmnAlarm << "Setting double to int negative overflow: " << vv << enderr;
		return false;
	}

	mIsNull = false;
	mValue = (int)vv;
	return true;
}


bool 
AosValueInt::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data type imcompatible: char str to int" << enderr;
	return false;
}


bool 
AosValueInt::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data type imcompatible: char str to int" << enderr;
	return false;
	mIsNull = false;
	if (len <= 0 || !data)
	{
		mIsNull = true;
		mValue = 0;
		return true;
	}

	int64_t val;
	bool rslt = AosStr2Int64(data, len, false, val);
	aos_assert_r(rslt, false);
	if (val > INT_MAX)
	{
		OmnAlarm << "setInt overflow: " << val<< enderr;
		return false;
	}

	if (val < INT_MIN)
	{
		OmnAlarm << "setInt underflow: " << val<< enderr;
		return false;
	}

	mValue = val;
	return true;
}


bool 
AosValueInt::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Setting an XML to int not allowed!" << enderr;
	return false;
}


bool 
AosValueInt::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Setting an XML to int not allowed!" << enderr;
	return false;
}



u8
AosValueInt::getU8()const
{
	if(mIsNull)
	{
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
AosValueInt::getU16()const
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
AosValueInt::getInt8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i8)mValue;
}

i16
AosValueInt::getInt16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i16)mValue;
}

float
AosValueInt::getFloat()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return (float)mValue;
}

/*


bool		
AosValueInt::getU64() const
{
	if (mIsNull) return false;
	if (mValue < 0)
	{
		OmnAlarm << "Getting negative value to u64: " << mValue << enderr;
		return false;
	}

	value = mValue;
	return true;
}

		

bool		
AosValueInt::getU32(u32 &value) const
{
	if (mIsNull) return false;
	if (mValue < 0)
	{
		OmnAlarm << "Getting negative value to u32: " << mValue << enderr;
		return false;
	}
	value = mValue;
	return true;
}


bool
AosValueInt::getInt64(int64_t &value) const
{
	if (mIsNull) return false;
	value = mValue;
	return true;
}

bool
AosValueInt::getInt(int &value) const
{
	if (mIsNull) return false;

	value = mValue;
	return true;
		Aos
}

bool
AosValueInt::getDouble(double &value) const
{
	if (mIsNull) return false;

	value = mValue;
	return true;
}

bool
AosValueInt::getStr(OmnString &value) const
{
	if (mIsNull) return false;

	OmnString str;
	str << mValue;

	value = str;
	return true;
		Aos
}


bool		
AosValueInt::getCharStr(char *data, int &len, const bool copy)
{
	if (mIsNull)
	{
		if (len < 7)
		{
			OmnAlarm << "String too short (requiring at least 7): " << len << enderr;
			return false;
		}

		strcpy(data, "(null)");
		len = 6;
		return true;
	}

	OmnString str;
	str << mValue;
	if (len < str.length() + 1)
	{
		OmnAlarm << "String too short: " 
			<< len << "; requiring: " << str.length() << enderr;
		return false;
	}

	strcpy(data, str.data());
	len = str.length();
	return true;
}
*/


OmnString 
AosValueInt::getOmnStr() const
{
	 
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueInt::getCharStr(int &len ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosQueryRsltObjPtr 
AosValueInt::getQueryRslt(const AosRundataPtr &rdata) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosXmlTagPtr 
AosValueInt::getXml() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


double 	
AosValueInt::getDouble() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return (double)mValue;
}


int 	
AosValueInt::getInt() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


i64 	
AosValueInt::getInt64() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i64)mValue;
}


u64 	
AosValueInt::getU64() const
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
AosValueInt::getU32() const
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
AosValueInt::getChar() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}


bool 	
AosValueInt::getBool() const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}


AosValueImp&
AosValueInt::operator +(const AosValueImp &rhs)
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
AosValueInt::operator -(const AosValueImp &rhs)
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
AosValueInt::operator *(const AosValueImp &rhs)
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
AosValueInt::operator /(const AosValueImp &rhs)
{
	AosDataType::E dataType = rhs.getDataType();
	AosValueImp *rslt ;
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
AosValueInt::operator ==(const AosValueImp &rhs)
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
AosValueInt::operator !=(const AosValueImp &rhs)
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
AosValueInt::operator >(const AosValueImp &rhs)
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
AosValueInt::operator >=(const AosValueImp &rhs)
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
AosValueInt::operator <(const AosValueImp &rhs)
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
AosValueInt::operator <=(const AosValueImp &rhs)
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

ostream & operator<<(ostream &os, const AosValueInt &rhs)
{
	os << rhs.mValue;
	return os;
}

istream & operator>>(istream &is, const AosValueInt &rhs)
{
	is >> rhs.mValue;
	return is;
}


bool 
AosValueInt::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}
#endif
