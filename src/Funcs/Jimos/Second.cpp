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
#include "Funcs/Jimos/Second.h"

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

AosJimoPtr AosCreateJimoFunc_AosSecond_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSecond(version);
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


AosSecond::AosSecond(const int version)
:
AosGenFunc("Second", version)
{
}

AosSecond::AosSecond()
:
AosGenFunc("Second", 1)
{
}



AosSecond::AosSecond(const AosSecond &rhs)
:
AosGenFunc(rhs)
{
}


AosSecond::~AosSecond()
{
}


bool
AosSecond::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosSecond::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}


bool
AosSecond::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	
	i64 second = -1;
	if (mValue.getType() == AosDataType::eDateTime) 
	{
		AosDateTime dt = mValue.getDateTime();
		second = dt.getPtime().time_of_day().seconds();
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
		char* flag = strptime(date_str.data(), date_type_str.data(), &tm_time); 
		if (flag != NULL)
		{
			second = tm_time.tm_sec;
		}
	}
	if (second < 0 || second > 59)
	{
		value.setNull();
	}
	else
	{
		value.setI64(second);
	}
	return true;
}

bool 
AosSecond::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "Second needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on
	return true;
}

AosJimoPtr
AosSecond::cloneJimo()  const
{
	try
	{
		return OmnNew AosSecond(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

