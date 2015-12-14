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
#include "Value/ValueXml.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Config/ConfigMgr.h"
#include "Util/Buff.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include <limits.h>

AosValueXml::~AosValueXml()
{
}


OmnString 
AosValueXml::toString() const
{
	OmnString str;
	str << "XML: ";
	if (mIsNull || !mValue) 
	{
		str << "(null)";
	}
	else
	{
		str << mValue->toString();
	}
	return str;
}


AosValue *
AosValueXml::operator +(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


AosValue *
AosValueXml::operator -(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


AosValue *
AosValueXml::operator *(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


AosValue *
AosValueXml::operator /(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosValueXml::operator ==(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueXml::operator !=(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueXml::operator >(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueXml::operator >=(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueXml::operator <(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueXml::operator <=(const AosValueRslt &rhs)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosValueXml::check(const AosOpr opr, const AosValueRslt &rhs) const
{
	OmnNotImplementedYet;
	return false;
}


bool
AosValueXml::setU64(const u64 vv)
{
	OmnAlarm << "Data type imcompatible: u64 to XML: " << vv << enderr;
	return false;
}


bool
AosValueXml::setU32(const u32 vv)
{
	OmnAlarm << "Data type imcompatible: u32 to XML: " << vv << enderr;
	return false;
}


bool 
AosValueXml::setInt64(const int64_t vv)
{
	OmnAlarm << "Data type imcompatible: int64 to XML: " << vv << enderr;
	return false;
}


bool 
AosValueXml::setInt(const int vv)
{
	OmnAlarm << "Data type imcompatible: int to XML: " << vv << enderr;
	return false;
}


bool 
AosValueXml::setDouble(const double vv)
{
	OmnAlarm << "Data type imcompatible: double to XML: " << vv << enderr;
	return false;
}


bool 
AosValueXml::setOmnStr(const OmnString &vv)
{
	OmnAlarm << "Data type imcompatible: OmnStr to XML: " << vv << enderr;
	return false;
}


bool 
AosValueXml::setCharStr(const char *data, const int len, const bool copy_flag)
{
	OmnAlarm << "Data type imcompatible: char string to XML: " << enderr;
	return false;
}


bool 
AosValueXml::setXml(const AosXmlTagPtr &vv)
{
	if (!vv)
	{
		mIsNull = true;
		mValue = 0;
		return true;
	}

	mIsNull = false;
	mValue = vv;
	return true;
}


bool 
AosValueXml::setBuff(const AosBuffPtr &vv)
{
	OmnAlarm << "Data type imcompatible: AosBuff to XML: " << vv << enderr;
	return false;
}


OmnString 
AosValueXml::getOmnStr(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return "";
	}

	OmnAlarm << "Data type imcompatible: XML to OmnStr" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return "";
}


const char *
AosValueXml::getCharStr(int &len, AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to CharStr" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


AosQueryRsltObjPtr 
AosValueXml::getQueryRslt(const AosRundataPtr &rdata, AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to QueryRslt" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return 0;
}


AosXmlTagPtr 
AosValueXml::getXml(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	errcode.setOk();
	return mValue;
}


double 	
AosValueXml::getDouble(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to double" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


int 	
AosValueXml::getInt(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to int" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


i64 	
AosValueXml::getInt64(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to int64" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


u64 	
AosValueXml::getU64(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to u64" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


u32 	
AosValueXml::getU32(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to u32" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


char 	
AosValueXml::getChar(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to char" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}


bool 	
AosValueXml::getBool(AosErrCode &errcode) const
{
	if (mIsNull) 
	{
		errcode.setFailed(OmnErrId::eNullValue);
		return 0;
	}

	OmnAlarm << "Data type imcompatible: XML to bool" << enderr;
	errcode.setFailed(OmnErrId::eImcompatibleDataTypes);
	return false;
}

*/
