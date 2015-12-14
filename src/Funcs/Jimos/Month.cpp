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
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/Month.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time.hpp"
#include <time.h>

using namespace boost::gregorian;
using namespace boost::posix_time;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMonth_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMonth(version);
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


AosMonth::AosMonth(const int version)
:
AosGenFunc("Month", version)
{
}

AosMonth::AosMonth()
:
AosGenFunc("Month", 1)
{
}



AosMonth::AosMonth(const AosMonth &rhs)
:
AosGenFunc(rhs)
{
}


AosMonth::~AosMonth()
{
}


bool
AosMonth::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosMonth::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}


bool
AosMonth::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;

	i64 month = 0;
	if (mValue.getType() == AosDataType::eDateTime) 
	{
		AosDateTime dt = mValue.getDateTime();
		month = dt.getPtime().date().month();
	}
	else
	{
		OmnString date_str = mValue.getStr();
		OmnString date_type_str = "%Y-%m-%d %H:%M:%S";
		if (mSize > 1)
		{
			if (!AosGenFunc::getValue(rdata, 1, record)) return false;
			date_type_str = mValue.getStr();
		}
		struct tm tm_time; 
		strptime(date_str.data(), date_type_str.data(), &tm_time); 
		month = tm_time.tm_mon + 1;
	}
	if (month < 1 || month > 12)
	{
		value.setNull();
	}
	else
	{
		value.setI64(month);
	}
	return true;
}

bool 
AosMonth::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "Month needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on
	return true;
}

AosJimoPtr
AosMonth::cloneJimo()  const
{
	try
	{
		return OmnNew AosMonth(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

