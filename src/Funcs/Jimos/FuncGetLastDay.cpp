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
#include "Funcs/Jimos/FuncGetLastDay.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>
#include <boost/date_time/gregorian/gregorian.hpp>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFuncGetLastDay_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFuncGetLastDay(version);
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


AosFuncGetLastDay::AosFuncGetLastDay(const int version)
:
AosGenFunc("FuncGetLastDay", version)
{
}

AosFuncGetLastDay::AosFuncGetLastDay()
:
AosGenFunc("FuncGetLastDay", 1)
{
}



AosFuncGetLastDay::AosFuncGetLastDay(const AosFuncGetLastDay &rhs)
:
AosGenFunc(rhs)
{
}


AosFuncGetLastDay::~AosFuncGetLastDay()
{
}


bool
AosFuncGetLastDay::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosFuncGetLastDay::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	string ds = mValue.getStr();
	ds += "01";

	boost::gregorian::date d(boost::gregorian::from_undelimited_string(ds));
	if (d.is_not_a_date())
	{
		value.setNull();
	}
	else 
	{
		boost::gregorian::date dd = d.end_of_month();
		//string newdate = boost::gregorian::to_iso_string(dd);
		i64 newdate = dd.year() * 10000 + dd.month() * 100 + dd.day();
		value.setI64(newdate);
	}
	
	return true;
}

bool 
AosFuncGetLastDay::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "FuncGetLastDay just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosFuncGetLastDay::cloneJimo()  const
{
	try
	{
		return OmnNew AosFuncGetLastDay(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

