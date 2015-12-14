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
#include "Funcs/Jimos/DateSecond.h"

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

AosJimoPtr AosCreateJimoFunc_AosDateSecond_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateSecond(version);
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


AosDateSecond::AosDateSecond(const int version)
:
AosGenFunc("DateSecond", version)
{
}

AosDateSecond::AosDateSecond()
:
AosGenFunc("DateSecond", 1)
{
}



AosDateSecond::AosDateSecond(const AosDateSecond &rhs)
:
AosGenFunc(rhs)
{
}


AosDateSecond::~AosDateSecond()
{
}


bool
AosDateSecond::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateSecond::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDateSecond::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString date_str = mValue.getStr();

	OmnString date_type_str;
	if (mSize > 1)
	{
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		date_type_str = mValue.getStr();
	}
	else 
	{
		date_type_str = "%Y-%m-%d %H:%M:%S";
	}

	/*
	boost::local_time::local_time_input_facet *input_facet =new boost::local_time::local_time_input_facet();
	istringstream is(date_str.data());
	is.imbue(std::locale(std::cout.getloc(), input_facet));
	input_facet->format(date_type_str.data());
	ptime p1;
	is>>p1;
	DateSecond=p1.time_of_day().DateSeconds();
	*/
	OmnString str = "";
	struct tm tm_time; 
	strptime(date_str.data(), date_type_str.data(), &tm_time); 

	str << tm_time.tm_year + 1900;
	str << "-";
	str << tm_time.tm_mon + 1;
	str << "-";
	str << tm_time.tm_mday;
	str << " ";
	str << tm_time.tm_hour;
	str << ":";
	str << tm_time.tm_min;
	str << ":";
	str << tm_time.tm_sec;

	value.setStr(str);
	return true;
}


bool 
AosDateSecond::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "DateSecond needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateSecond::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateSecond(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

