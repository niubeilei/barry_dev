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
#include "Value/ValueU16.h"

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
#include "Value/ValueU64.h"
#include "Value/ValueInt8.h"
#include "Value/ValueInt16.h"
#include "Value/ValueInt.h"
#include "Value/ValueInt64.h"
#include "Value/ValueFloat.h"
#include "Value/ValueDouble.h"
AosValueU16::~AosValueU16()
{
}

AosValueImp * 
AosValueU16::getAosValue(const double value)
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
			rslt = new AosValueU16(value);
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
AosValueU16::toString() const
{
	OmnString str;
	str << "u16 ";
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
AosValueU16::operator +(const AosValueImp &rhs)
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
AosValueU16::operator -(const AosValueImp &rhs)
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
AosValueU16::operator *(const AosValueImp &rhs)
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
AosValueU16::operator /(const AosValueImp &rhs)
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
AosValueU16::operator ==(const AosValueImp &rhs)
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
AosValueU16::operator !=(const AosValueImp &rhs)
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
AosValueU16::operator >(const AosValueImp &rhs)
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
AosValueU16::operator >=(const AosValueImp &rhs)
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
AosValueU16::operator <(const AosValueImp &rhs)
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
AosValueU16::operator <=(const AosValueImp &rhs)
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

ostream & operator<<(ostream &os,const AosValueU16 &rhs)
{
    os << rhs.mValue;
    return os;
}

istream & operator>>(istream &is,const AosValueU16 &rhs)
{
    is >> rhs.mValue;
    return is;
}


bool 
AosValueU16::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueU16::setU8(const u8 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU16::setU16(const u16 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU16::setInt8(const i8 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i8 to u16 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU16::setInt16(const i16 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i16 to u16 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU16::setFloat(const float vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting float to u16 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU16::setU64(const u64 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueU16::setU32(const u32 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU16::setInt64(const int64_t vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative to u16: "<< enderr; ;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU16::setInt(const int vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative to u16: " << enderr;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU16::setDouble(const double vv)
{
	if (vv < 0.0)
	{
		OmnAlarm << "Set negative to u16: " << enderr;
		return false;
	}
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU16::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data type imcompatible: OmnStr to u16" << enderr;
	return false;
}


bool 
AosValueU16::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data type imcompatible: char string to u16" << enderr;
	return false;
}

bool
AosValueU16::setChar(const char vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool 
AosValueU16::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Data type imcompatible: XML to u16" << enderr;
	return false;
}


bool 
AosValueU16::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Data type imcompatible: AosBuff to u16" << enderr;
	return false;
}


u8
AosValueU16::getU8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return (u8)mValue;
}

u16
AosValueU16::getU16()const
{
	if(mIsNull)
	{
		return 0;
	}
	return (u16)mValue;
}

i8
AosValueU16::getInt8()const
{
	if(mIsNull)
	{
		return -1;
	}
	return (i8)mValue;
}

i16
AosValueU16::getInt16()const
{
	if(mIsNull)
	{
		return -1;
	}
	return (i16)mValue;
}

float
AosValueU16::getFloat()const
{
	if(mIsNull)
	{
		return 0.0;
	}
	return (float)mValue;
}


OmnString 
AosValueU16::getOmnStr( ) const
{
	 
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueU16::getCharStr(int &len  ) const
{
	if (mIsNull) 
	{
		return 0;
	}
	return 0;
}


AosQueryRsltObjPtr 
AosValueU16::getQueryRslt(const AosRundataPtr &rdata  ) const
{
	if (mIsNull) 
	{
		return 0;
	}
	return 0;
}


AosXmlTagPtr 
AosValueU16::getXml( ) const
{
	if (mIsNull) 
	{
		return 0;
	}
	return 0;
}


double 	
AosValueU16::getDouble( ) const
{
	if (mIsNull) 
	{
		return 0.0;
	} 
	return mValue;
}


int 	
AosValueU16::getInt( ) const
{
	if (mIsNull) 
	{
		return -1;
	} 
	return mValue;
}


i64 	
AosValueU16::getInt64( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


u64 	
AosValueU16::getU64( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return (u64)mValue;
}


u32 	
AosValueU16::getU32( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return (u32)mValue;
}


char 	
AosValueU16::getChar( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}


bool 	
AosValueU16::getBool( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return mValue;
}
#endif
