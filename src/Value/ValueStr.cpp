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
// 2013/12/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Value/ValueStr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>


extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosValueStr_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosValueStr(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosValueStr::~AosValueStr()
{
}

OmnString 
AosValueStr::toString() const
{
	OmnString str;
	str << "str ";
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
AosValueStr::setU64(const u64 vv)
{
	mIsNull = false;
	mValue = "";
	mValue << vv;
	return true;
}


bool 
AosValueStr::setU32(const u32 vv)
{
	mIsNull = false;
	mValue = "";
	mValue << vv;
	return true;
}

bool
AosValueStr::setInt64(const int64_t vv)
{
	mIsNull = false;
	mValue = "";
	mValue << vv;
	return true;
}


bool 
AosValueStr::setInt(const int vv)
{
	mIsNull = false;
	mValue = "";
	mValue << vv;
	return true;
}



bool 
AosValueStr::setDouble(const double vv)
{
	mIsNull = false;
	mValue = "";
	mValue << vv;
	return true;
}

bool 
AosValueStr::setStr(const OmnString vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueStr::setCharStr(const char *data, const int len)
{
	mIsNull = false;
	mValue.assign(data, len);
	return true;
}


bool 
AosValueStr::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Setting an XML to string!" << enderr;
	return false;
}


bool 
AosValueStr::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Setting an Buffer to string!" << enderr;
	return false;
}

bool
AosValueStr::getU64(u64 &value) const
{
	if (mIsNull) return false;
	value = mValue.toU64();
	return true;
}


bool		
AosValueStr::getU32(u32 &value) const
{
	if (mIsNull) return false;
	//value = mValue.toU32();
	return true;
}

bool
AosValueStr::getInt64(int64_t &value) const
{
	if (mIsNull) return false;
	//value = mValue.parseInt64();
	return true;
}

bool		
AosValueStr::getInt(int &value) const
{
	if (mIsNull) return false;
	//value = mValue.parseInt();
	return true;
}

bool
AosValueStr::getDouble(double &value) const
{
	if (mIsNull) return false;

	u32 len;
	if (!mValue.parseDouble(0, len, value)) 
		return false;

	return true;
}

bool
AosValueStr::getStr(OmnString &value) const
{
	if (mIsNull) return false;
	value = mValue;
	return true;
}


bool
AosValueStr::getCharStr(char *&data, int &len, const bool need_copy) const
{
	OmnNotImplementedYet;
	return false;
	/*
	if (mIsNull)
	{
		len = 0;
		return 0;
	}
	len = mValue.length();
	return mValue.data();
	*/
}


bool
AosValueStr::getXml(AosXmlTagPtr &value) const
{
	OmnAlarm << "To get XML from a string" << enderr;
	return false;
}


bool
AosValueStr::getBuff(AosBuffPtr &value) const
{
	OmnAlarm << "To get Buff from a string" << enderr;
	return false;
}



AosJimoPtr
AosValueStr::cloneJimo() const 
{
	try
	{
		return OmnNew AosValueStr(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}
#endif
