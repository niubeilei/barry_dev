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
#include "Funcs/Jimos/FuncGetFirstDayOfMonth.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>
#include <boost/date_time/gregorian/gregorian.hpp>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosFuncGetFirstDayOfMonth_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosFuncGetFirstDayOfMonth(version);
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


AosFuncGetFirstDayOfMonth::AosFuncGetFirstDayOfMonth(const int version)
:
AosGenFunc("FuncGetFirstDayOfMonth", version)
{
}

AosFuncGetFirstDayOfMonth::AosFuncGetFirstDayOfMonth()
:
AosGenFunc("FuncGetFirstDayOfMonth", 1)
{
}



AosFuncGetFirstDayOfMonth::AosFuncGetFirstDayOfMonth(const AosFuncGetFirstDayOfMonth &rhs)
:
AosGenFunc(rhs)
{
}


AosFuncGetFirstDayOfMonth::~AosFuncGetFirstDayOfMonth()
{
}


bool
AosFuncGetFirstDayOfMonth::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosFuncGetFirstDayOfMonth::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	string ds = mValue.getStr();

	boost::gregorian::date d(boost::gregorian::from_undelimited_string(ds));
	if (d.is_not_a_date())
	{
		value.setNull();
	}
	else 
	{
		i64 newdate = d.year() * 10000 + d.month() * 100 + 1;
		value.setI64(newdate);
	}
	
	return true;
}

bool 
AosFuncGetFirstDayOfMonth::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "FuncGetFirstDayOfMonth just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosFuncGetFirstDayOfMonth::cloneJimo()  const
{
	try
	{
		return OmnNew AosFuncGetFirstDayOfMonth(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

