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
#include "Funcs/Jimos/DateYear.h"

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

AosJimoPtr AosCreateJimoFunc_AosDateYear_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateYear(version);
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


AosDateYear::AosDateYear(const int version)
:
AosGenFunc("DateYear", version)
{
}

AosDateYear::AosDateYear()
:
AosGenFunc("DateYear", 1)
{
}



AosDateYear::AosDateYear(const AosDateYear &rhs)
:
AosGenFunc(rhs)
{
}


AosDateYear::~AosDateYear()
{
}


bool
AosDateYear::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateYear::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}


bool
AosDateYear::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString date_str = mValue.getStr();
	OmnString date_type_str;

	if (mSize > 1 )
	{
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		date_type_str = mValue.getStr();
		int year = 0;
		struct tm tm_time; 
		strptime(date_str.data(), date_type_str.data(), &tm_time); 

		year = tm_time.tm_year + 1900;
		date_str = "";
		date_str << year << "-01-01";
		date_type_str = "%Y-%m-%d";
	}
	else
	{
		//date_type_str = "%Y-%m-%d %H:%M:%S";
		date_type_str = "%Y-%m-%d";
	}
	AosDateTime dt(date_str, date_type_str);\
	value.setDateTime(dt);
	return true;
}

bool 
AosDateYear::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "DateYear needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateYear::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateYear(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
