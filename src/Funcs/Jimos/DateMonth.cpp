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
#include "Funcs/Jimos/DateMonth.h"

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

AosJimoPtr AosCreateJimoFunc_AosDateMonth_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateMonth(version);
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


AosDateMonth::AosDateMonth(const int version)
:
AosGenFunc("DateMonth", version)
{
}

AosDateMonth::AosDateMonth()
:
AosGenFunc("DateMonth", 1)
{
}



AosDateMonth::AosDateMonth(const AosDateMonth &rhs)
:
AosGenFunc(rhs)
{
}


AosDateMonth::~AosDateMonth()
{
}


bool
AosDateMonth::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateMonth::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDateMonth::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	aos_assert_r(mValue.getType()==AosDataType::eDateTime,false);
	OmnString date_str = mValue.getStr();
	OmnString date_type_str;
	if (mSize > 1)
	{
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		date_type_str = mValue.getStr();
		OmnString str = "";
		struct tm tm_time; 
		if(!strptime(date_str.data(), date_type_str.data(), &tm_time))
			return false; 
		date_str = "";
		date_str << tm_time.tm_year + 1900;
		date_str << "-";
		date_str << tm_time.tm_mon + 1 << "-01";
		date_type_str = "%Y-%m-%d";
		AosDateTime dt(date_str,date_type_str);
		if (dt.isNotADateTime()) 
		{   
			OmnAlarm << "Current DateTime Object is invalid" << enderr;
			return false;
		}
		value.setDateTime(dt);
		return true;
	}
	else 
	{
		//date_type_str = "%Y-%m-%d %H:%M:%S";
		date_type_str = "%Y-%m-%d";
	}

	AosDateTime dt = mValue.getDateTime();
	dt.setFormat(date_type_str);
	value.setDateTime(dt);
	return true;
}


bool 
AosDateMonth::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "DateMonth needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateMonth::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateMonth(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
