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
#include "Funcs/Jimos/DateWeek.h"

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

AosJimoPtr AosCreateJimoFunc_AosDateWeek_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateWeek(version);
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


AosDateWeek::AosDateWeek(const int version)
:
AosGenFunc("DateWeek", version)
{
}

AosDateWeek::AosDateWeek()
:
AosGenFunc("DateWeek", 1)
{
}



AosDateWeek::AosDateWeek(const AosDateWeek &rhs)
:
AosGenFunc(rhs)
{
}


AosDateWeek::~AosDateWeek()
{
}


bool
AosDateWeek::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateWeek::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDateWeek::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	aos_assert_r(mValue.getType() == AosDataType::eDateTime,false);
	OmnString date_type_str;
/*	if (mSize > 1)
	{
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		date_type_str = mValue.getStr();
	}
	else 
	{
*/		date_type_str = "%G %V";
//	}
	AosDateTime dt = mValue.getDateTime();
	dt.setFormat(date_type_str);
	value.setDateTime(dt);
	/*
	OmnString date_str = value.getStr();

	OmnString str = "";
	struct tm tm_time; 
	strptime(date_str.data(), date_type_str.data(), &tm_time);
	int tm_yday = tm_time.tm_yday;
	int tm_mday = tm_time.tm_mday;
	char monthTime[100] = {0};
	strftime(monthTime, sizeof(monthTime)-1, "%V", &tm_time);
	int t1= atoi(monthTime);
	memset(monthTime, 0, 100);
	tm_time.tm_mday = 1;
	tm_time.tm_yday = tm_yday - tm_mday + 1;
	strftime(monthTime, sizeof(monthTime)-1, "%V", &tm_time);
	int t2 = atoi(monthTime);
	str << tm_time.tm_year + 1900;
	str << "-";
	str << tm_time.tm_mon + 1;
	str << " ";
	str << (t1 - t2 + 1);
	str << "week" << (tm_time.tm_mday / 7 + 1); */
	//OmnString nn(monthTime, sizeof(monthTime));
	//value.setStr(str);
	return true;
}


bool 
AosDateWeek::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "DateWeek needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateWeek::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateWeek(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

