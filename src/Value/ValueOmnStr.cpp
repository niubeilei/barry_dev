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
// 2013/03/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
/*
#include "Value/ValueOmnStr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>

AosValueOmnStr::~AosValueOmnStr()
{
}


OmnString 
AosValueOmnStr::toString() const
{
	OmnString str;
	str << "OmnStr ";
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


bool 
AosValueOmnStr::setOmnStr(const OmnString &vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


OmnString 
AosValueOmnStr::getOmnStr() const
{
	errcode.setOk();
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueOmnStr::getCharStr(int &len, ) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setOk();
	const char *data = mValue.data();
	len = mValue.length();
	return data;
}


AosQueryRsltObjPtr 
AosValueOmnStr::getQueryRslt(const AosRundataPtr &rdata, ) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


AosXmlTagPtr 
AosValueOmnStr::getXml() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}

i8	 	
AosValueOmnStr::getInt8() const
{
	if(mIsNull)
	{
		return -1;
	}
	return mValue; 
}
i16 	
AosValueOmnStr::getInt16() const
{
	
}
u8
AosValueOmnStr::getU8() const
{

}
u16
AosValueOmnStr::getU16() const
{

}
float
AosValueOmnStr::getFloat() const
{

}

double 	
AosValueOmnStr::getDouble() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0.0;
	}

	double vv;
	bool rslt = AosStr2Double(mValue.data(), mValue.length(), false, vv);
	if (!rslt)
	{
		errcode.setFailed(OmnErrId::eDataConversionError);
		return 0;
	}

	errcode.setOk();
	return vv;
}


int 	
AosValueOmnStr::getInt() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return -1;
	}

	i64 vv;
	bool rslt = AosStr2Int64(mValue.data(), mValue.length(), false, vv);
	if (!rslt)
	{
		errcode.setFailed(OmnErrId::eDataConversionError);
		return 0;
	}

	errcode.setOk();
	return vv;
}


i64 	
AosValueOmnStr::getInt64() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return -1;
	}

	i64 vv;
	bool rslt = AosStr2Int64(mValue.data(), mValue.length(), false, vv);
	if (!rslt)
	{
		errcode.setFailed(OmnErrId::eDataConversionError);
		return 0;
	}

	errcode.setOk();
	return vv;
}


u64 	
AosValueOmnStr::getU64() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	u64 vv;
	bool rslt = AosStr2U64(mValue.data(), mValue.length(), false, vv);
	if (!rslt)
	{
		errcode.setFailed(OmnErrId::eDataConversionError);
		return 0;
	}

	errcode.setOk();
	return vv;
}


u32 	
AosValueOmnStr::getU32() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	u64 vv;
	bool rslt = AosStr2U64(mValue.data(), mValue.length(), false, vv);
	if (!rslt)
	{
		errcode.setFailed(OmnErrId::eDataConversionError);
		return 0;
	}

	errcode.setOk();
	return vv;
}


char 	
AosValueOmnStr::getChar() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	if (mValue.length() == 1)
	{
		errcode.setOk();
		return mValue.data()[0];
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


bool 	
AosValueOmnStr::getBool() const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	if (mValue.length() == 4) 
	{
		const char *data = mValue.data();
		if ((data[0] == 'T' || data[0] == 't') &&
			(data[1] == 'R' || data[1] == 'r') &&
			(data[2] == 'U' || data[2] == 'u') &&
			(data[3] == 'E' || data[3] == 'e')) 
		{
			errcode.setOk();
			return true;
		}

		errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
		return false;
	}

	if (mValue.length() == 5) 
	{
		const char *data = mValue.data();
		if ((data[0] == 'F' || data[0] == 'f') &&
			(data[1] == 'A' || data[1] == 'a') &&
			(data[2] == 'L' || data[2] == 'l') &&
			(data[3] == 'S' || data[3] == 's') &&
			(data[4] == 'E' || data[3] == 'e')) 
		{
			errcode.setOk();
			return false;
		}

		errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
		return false;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


AosValue *
AosValueOmnStr::operator +(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


AosValue *
AosValueOmnStr::operator -(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


AosValue *
AosValueOmnStr::operator *(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


AosValue *
AosValueOmnStr::operator /(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosValueOmnStr::operator ==(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueOmnStr::operator !=(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueOmnStr::operator >(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueOmnStr::operator >=(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueOmnStr::operator <(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueOmnStr::operator <=(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueOmnStr::check(const AosOpr opr, const AosValueRslt &rhs) const
{
	OmnNotImplementedYet;
	return false;
}*/
