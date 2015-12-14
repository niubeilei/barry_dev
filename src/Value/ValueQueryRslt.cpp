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
#include "Value/ValueQueryRslt.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>


AosValueQueryRslt::~AosValueQueryRslt()
{
}


OmnString 
AosValueQueryRslt::toString() const
{
	OmnString str;
	str << "QueryRslt: ";
	if (mIsNull) 
	{
		str << "(null)";
	}
	return str;
}


bool
AosValueQueryRslt::setU64(const u64 vv)
{
	OmnAlarm << "Data type imcompatible: u64 to QueryRslt" << vv << enderr;
	return false;
}


bool
AosValueQueryRslt::setU32(const u32 vv)
{
	OmnAlarm << "Data type imcompatible: u32 to QueryRslt" << vv << enderr;
	return false;
}


bool 
AosValueQueryRslt::setInt64(const int64_t vv)
{
	OmnAlarm << "Data type imcompatible: int64 to QueryRslt" << vv << enderr;
	return false;
}


bool 
AosValueQueryRslt::setInt(const int vv)
{
	OmnAlarm << "Data type imcompatible: int to QueryRslt" << vv << enderr;
	return false;
}


bool 
AosValueQueryRslt::setDouble(const double vv)
{
	OmnAlarm << "Data type imcompatible: double to QueryRslt" << vv << enderr;
	return false;
}


bool 
AosValueQueryRslt::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data type imcompatible: OmnString to QueryRslt" << vv << enderr;
	return false;
}


bool 
AosValueQueryRslt::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data type imcompatible: char string to QueryRslt" << enderr;
	return false;
}


bool 
AosValueQueryRslt::setXml(const AosXmlTagPtr &vv)
{
	OmnAlarm << "Data type imcompatible: XML to QueryRslt" << vv << enderr;
	return false;
}


bool 
AosValueQueryRslt::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Data type imcompatible: AosBuff to QueryRslt" << vv << enderr;
	return false;
}


OmnString 
AosValueQueryRslt::getOmnStr(AosErrCode &errcode) const
{
	errcode.setOk();
	OmnString ss;
	ss << mValue;
	return ss;
}


const char *
AosValueQueryRslt::getCharStr(int &len, AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


AosQueryRsltObjPtr 
AosValueQueryRslt::getQueryRslt(const AosRundataPtr &rdata, AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setOk();
	return mValue;
}


AosXmlTagPtr 
AosValueQueryRslt::getXml(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


double 	
AosValueQueryRslt::getDouble(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0.0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


int 	
AosValueQueryRslt::getInt(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return -1;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


i64 	
AosValueQueryRslt::getInt64(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return -1;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


u64 	
AosValueQueryRslt::getU64(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


u32 	
AosValueQueryRslt::getU32(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


char 	
AosValueQueryRslt::getChar(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


bool 	
AosValueQueryRslt::getBool(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


bool
AosValueQueryRslt::setQueryRslt(const AosQueryRsltObjPtr &vv)
{
	mIsNull = false;
	mValue = vv;
	return true;
}
*/
