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
// 2014/12/16 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Value/ValueU32.h"

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


AosValueU32::~AosValueU32()
{
}
AosValueImp *
AosValueU32::getAosValue(const double value)
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
		else if (value >= 0 && value <= ULONG_MAX)
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
AosValueU32::toString() const
{
	OmnString str;
	str << "u32 ";
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
AosValueU32::operator +(const AosValueImp &rhs)
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
AosValueU32::operator -(const AosValueImp &rhs)
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
AosValueU32::operator *(const AosValueImp &rhs)
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
AosValueU32::operator /(const AosValueImp &rhs)
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
AosValueU32::operator ==(const AosValueImp &rhs)
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
AosValueU32::operator !=(const AosValueImp &rhs)
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
AosValueU32::operator >(const AosValueImp &rhs)
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
AosValueU32::operator >=(const AosValueImp &rhs)
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
AosValueU32::operator <(const AosValueImp &rhs)
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
AosValueU32::operator <=(const AosValueImp &rhs)
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

ostream & operator<<(ostream &os,const AosValueU32 &rhs)
{
    os << rhs.mValue;
    return os;
}

istream & operator>>(istream &is,const AosValueU32 &rhs)
{
    is >> rhs.mValue;
    return is;
}

bool 
AosValueU32::check(const AosOpr opr, const AosValueImp &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueU32::setU8(const u8 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setU16(const u16 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setInt8(const i8 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i8 to u32 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setInt16(const i16 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i16 to u32 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setFloat(const float vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting float to u32 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}
bool
AosValueU32::setChar(const char vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}

bool
AosValueU32::setU64(const u64 vv)
{
	if (vv > UINT_MAX)
	{
		OmnAlarm << "Value overflow: " << vv << enderr;
	}

	mIsNull = false;
	mValue = vv;
	return true;
}


bool
AosValueU32::setU32(const u32 vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU32::setInt64(const int64_t vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative to u32: " << vv << enderr;
	}

	if (vv > UINT_MAX)
	{
		OmnAlarm << "Value overflow: " << vv << enderr;
	}

	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU32::setInt(const int vv)
{
	if (vv < 0)
	{
		OmnAlarm << "Set negative to u32: " << vv << enderr;
	}

	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU32::setDouble(const double vv)
{
	if (vv > UINT_MAX)
	{
		OmnAlarm << "Value overflow: " << vv << enderr;
	}
	else if (vv < 0.0)
	{
		OmnAlarm << "Set negative to u32: " << vv << enderr;
	}

	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueU32::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data type imcompatible: OmnStr to u32" << enderr;
	return false;
}


bool 
AosValueU32::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data type imcompatible: char string to u32" << enderr;
	return false;
}


bool 
AosValueU32::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Data type imcompatible: XML to u32" << enderr;
	return false;
}


bool 
AosValueU32::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Data type imcompatible: AosBuff to u32" << enderr;
	return false;
}
/*
bool
AosValueU32::setU8(const u8 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setU16(const u16 vv)
{
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setInt8(const i8 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i8 to u32 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setInt16(const i16 vv)
{
	if(vv < 0)
	{
		OmnAlarm << "Setting i16 to u32 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}

bool
AosValueU32::setFloat(const float vv)
{
	if(vv > U32MAX)
	{
		OmnAlarm << "Setting float to u32 positive overflow:" << enderr;
		return false;
	}
	if(vv < 0)
	{
		OmnAlarm << "Setting float to u32 negative overflow:" << enderr;
		return false;
	}
	mIsNull = false;
	mValue  = vv;
	return true;
}
*/
u8
AosValueU32::getU8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return (u8)mValue;
}

u16
AosValueU32::getU16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return (u16)mValue;
}

i8
AosValueU32::getInt8()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i8)mValue;
}

i16
AosValueU32::getInt16()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return (i16)mValue;
}

float
AosValueU32::getFloat()const
{
	if(mIsNull)
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}
	return (float)mValue;
}


OmnString 
AosValueU32::getOmnStr( ) const
{
	 
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueU32::getCharStr(int &len ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosQueryRsltObjPtr 
AosValueU32::getQueryRslt(const AosRundataPtr &rdata ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


AosXmlTagPtr 
AosValueU32::getXml( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return 0;
}


double 	
AosValueU32::getDouble( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0.0;
	}	 
	return mValue;
}


int 	
AosValueU32::getInt( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	} 
	return mValue;
}


i64 	
AosValueU32::getInt64( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return -1;
	}
	return mValue;
}


u64 	
AosValueU32::getU64( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	}
	return (u64)mValue;
}


u32 	
AosValueU32::getU32( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	} 
	return (u32)mValue;
}


char 	
AosValueU32::getChar( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	} 
	return mValue;
}


bool 	
AosValueU32::getBool( ) const
{
	if (mIsNull) 
	{
		OmnAlarm << "Value is null" << enderr;
		return 0;
	} 
	return mValue;
}
#endif
