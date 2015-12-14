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
#include "Funcs/Jimos/DateDay.h"

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

AosJimoPtr AosCreateJimoFunc_AosDateDay_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateDay(version);
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


AosDateDay::AosDateDay(const int version)
:
AosGenFunc("DateDay", version)
{
}

AosDateDay::AosDateDay()
:
AosGenFunc("DateDay", 1)
{
}



AosDateDay::AosDateDay(const AosDateDay &rhs)
:
AosGenFunc(rhs)
{
}


AosDateDay::~AosDateDay()
{
}


bool
AosDateDay::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateDay::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDateDay::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	AosValueRslt vv;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	aos_assert_r(mValue.getType() == AosDataType::eDateTime,false);
	vv = mValue;
	OmnString date_type_str;
	if (mSize > 1)
	{
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		date_type_str = mValue.getStr();
	}
	else 
	{
		date_type_str = "%Y-%m-%d";
	}

	AosDateTime dt = vv.getDateTime();
	dt.setFormat(date_type_str);
	value.setDateTime(dt);
	return true;
}


bool 
AosDateDay::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1 || mSize > 2)
	{
		errmsg << "DateDay needs 1 or 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateDay::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateDay(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

