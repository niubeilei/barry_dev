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
#if 0
#include "Value/ValueConfig.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>


extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosValueConfig_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosValueConfig();
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

AosValueConfig::AosValueConfig()
:
AosConfigHandler("value")
{
	// This constructor is used to initialize 'Value' lib.
	AosConfigHandlerPtr thisptr(this, false);
	AosConfigMgr::registerHandler(thisptr);	
}


bool
AosValueConfig::configModule(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)
{
	return false;
}


OmnString 
AosValueConfig::toString() const
{
	return "";
}

bool
AosValueConfig::setU64(const u64 vv)
{
	return false;
}


bool
AosValueConfig::setU32(const u32 vv)
{
	return false;
}


bool 
AosValueConfig::setInt64(const int64_t vv)
{
	return false;
}


bool 
AosValueConfig::setInt(const int vv)
{
	return false;
}


bool 
AosValueConfig::setDouble(const double vv)
{
	return false;
}


bool 
AosValueConfig::setStr(const OmnString vv)
{
	return false;
}


bool 
AosValueConfig::setCharStr(const char *data, const int len)
{
	return false;
}


bool 
AosValueConfig::setXml(const AosXmlTagPtr &vv)
{
	return false;
}


bool 
AosValueConfig::setBuff(const AosBuffPtr &vv)
{
	return false;
}


bool		
AosValueConfig::getU64(u64 &value) const
{
	return false;
}


bool		
AosValueConfig::getU32(u32 &value) const
{
	return false;
}


bool
AosValueConfig::getInt64(int64_t &value) const
{
	return false;
}

bool
AosValueConfig::getInt(int &value) const
{
	return false;
}

bool
AosValueConfig::getDouble(double &value) const
{
	return false;
}

bool
AosValueConfig::getStr(OmnString &value) const
{
	return false;
}


/*
bool		
AosValueConfig::getCharStr(char *data, int &len, const bool copy)
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

bool
AosValueConfig::getCharStr(char *&data, int &len) const
{
	return false;
}

bool
AosValueConfig::getXml(AosXmlTagPtr &value) const
{
	return false;
}

bool
AosValueConfig::getBuff(AosBuffPtr &buff) const
{
	return false;
}


AosJimoPtr
AosValueConfig::cloneJimo() const 
{
	try
	{
		return OmnNew AosValueConfig(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}
#endif
