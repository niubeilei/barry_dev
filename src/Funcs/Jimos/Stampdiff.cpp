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
#include "Funcs/Jimos/Stampdiff.h"
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

AosJimoPtr AosCreateJimoFunc_AosStampdiff_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStampdiff(version);
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


AosStampdiff::AosStampdiff(const int version)
:
AosGenFunc("Stampdiff", version)
{
}

AosStampdiff::AosStampdiff()
:
AosGenFunc("Stampdiff", 1)
{
}



AosStampdiff::AosStampdiff(const AosStampdiff &rhs)
:
AosGenFunc(rhs)
{
}


AosStampdiff::~AosStampdiff()
{
}


bool
AosStampdiff::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosStampdiff::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosStampdiff::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString str1,str2;	
	double timedif;
	if(mSize!=2)
	{
		return false;
	}
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	str1 = mValue.getStr();
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	str2 = mValue.getStr();

	if (str1 == "" || str1 == "0") str1 = "1970-01-01 00:00:00";
	if (str2 == "" || str2 == "0") str2 = "1970-01-01 00:00:00";

	ptime p1, p2; 
	try
	{
		p1 = time_from_string(str1.data());
		p2 = time_from_string(str2.data());
	}
	catch(...)
	{
		OmnAlarm << __func__ << enderr;
	}

	time_duration p3 = p1-p2;
	/*int difhours=p3.hours();
	int difmins1=p1.time_of_day().minutes();
	int difmins2=p2.time_of_day().minutes();
	int difseconds1=p1.time_of_day().seconds();
	int difseconds2=p2.time_of_day().seconds();
	//int difmillisecs2=p1.time_of_day().total_milliseconds();
	//int difmillisecs1=p2.time_of_day().total_milliseconds();
	p3.total_milliseconds()
	timedif=difhours*3600+(difmins2-difmins1)*60+(difseconds2-difseconds1);*/
	timedif=p3.total_milliseconds();
	value.setI64(timedif < 0 ? timedif*(-1):timedif);
	return true;
}


bool 
AosStampdiff::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize !=2)
	{
		errmsg << "Length need only 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosStampdiff::cloneJimo()  const
{
	try
	{
		return OmnNew AosStampdiff(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
