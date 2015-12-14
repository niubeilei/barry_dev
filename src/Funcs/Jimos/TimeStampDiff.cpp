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
#include "Funcs/Jimos/TimeStampDiff.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time.hpp"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <string>
using namespace boost::gregorian;
using namespace boost::posix_time;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTimeStampDiff_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosTimeStampDiff(version);
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


AosTimeStampDiff::AosTimeStampDiff(const int version)
:
AosGenFunc("TimeStampDiff", version)
{
}

AosTimeStampDiff::AosTimeStampDiff()
:
AosGenFunc("TimeStampDiff", 1)
{
}



AosTimeStampDiff::AosTimeStampDiff(const AosTimeStampDiff &rhs)
:
AosGenFunc(rhs)
{
}


AosTimeStampDiff::~AosTimeStampDiff()
{
}


bool
AosTimeStampDiff::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosTimeStampDiff::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}

	
bool
AosTimeStampDiff::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (mSize != 3)
		return false;
    OmnString opr = mParms[0]->getValue(rdata);
	opr.toLower();
	if (opr == "")
	{
		value.setNull();
		return true;
	}

	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	if(mValue.getType() != AosDataType::eDateTime)
		return false;
	OmnString time1 = OmnString(mValue.getDateTime().toString("%Y-%m-%d %H:%M:%S"));
	if(time1 == "invalid")
		return false;
//	OmnString time1 = mValue.getStr();
	if(time1 == "")
	{
		value.setNull();
		return true;
	}
    
	if (!AosGenFunc::getValue(rdata, 2, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	if(mValue.getType() != AosDataType::eDateTime)
		return false;
	OmnString time2 = OmnString(mValue.getDateTime().toString("%Y-%m-%d %H:%M:%S"));	
//	OmnString time2 = mValue.getStr();
	if (time2 == "invalid")
		return false;
	if (time2 == "")
	{
		value.setNull();
		return true;
	}

	if (time1 == "" || time1 == "0") time1 = "1970-01-01 00:00:00";
	if (time2 == "" || time2 == "0") time2 = "1970-01-01 00:00:00";

	ptime p1, p2;
	try
	{
		p1 = time_from_string(time1.data());
		p2 = time_from_string(time2.data());
	}
	catch(...)
	{
		value.setNull();
		return true;
	}

	i64 year_p1  = p1.date().year();
	i64 year_p2  = p2.date().year();
	i64 month_p1 = p1.date().month();
	i64 month_p2 = p2.date().month();
	time_duration p3 = p2 - p1;

	double dif;
	i64 timedif = p3.total_milliseconds();
	//for the operation
	if (opr == "frac_second")
	{
		dif = timedif;
	}
	else if (opr == "second")
	{
		dif = timedif / 1000;
	}
	else if (opr == "minute")
	{
		dif = timedif / 1000 / 60;
	}
	else if (opr == "hour")
	{
		dif = timedif / 1000 / 60 / 60;
	}
	else if (opr == "day")
	{
		dif = (double)timedif / 1000 / 60 / 60 / 24;
	}
	else if (opr == "week")
	{
		dif = timedif / 1000 / 60 / 60 / 24 / 7;
	}
	else if (opr == "month")
	{
		dif = (year_p2 - year_p1) * 12 + (month_p2 - month_p1);
	}
	else if (opr == "quarter")
	{
		dif = ((year_p2 - year_p1) * 12 + (month_p2 - month_p1)) / 3;
	}
	else if (opr == "year")
	{
		dif = year_p2 - year_p1;
	}

	value.setI64(dif);
	return true;
}


bool 
AosTimeStampDiff::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize != 3)
	{
		errmsg << "Length need only 3 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosTimeStampDiff::cloneJimo()  const
{
	try
	{
		return OmnNew AosTimeStampDiff(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
