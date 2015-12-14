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
#include "Funcs/Jimos/SysDate.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSysDate_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSysDate(version);
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


AosSysDate::AosSysDate(const int version)
:
AosGenFunc("SysDate", version)
{
}

AosSysDate::AosSysDate()
:
AosGenFunc("SysDate", 1)
{
}



AosSysDate::AosSysDate(const AosSysDate &rhs)
:
AosGenFunc(rhs)
{
}


AosSysDate::~AosSysDate()
{
}


bool
AosSysDate::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosSysDate::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}

	
bool
AosSysDate::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	char time_str[30];
	time_t tt = time(NULL);
	struct tm *t = localtime(&tt);
	sprintf(time_str, "%d-%02u-%02u %02u:%02u:%02u", 
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, 
			t->tm_hour, t->tm_min, t->tm_sec);
	OmnString val(time_str, strlen(time_str));

	AosDateTime dt(val, "");
	if (dt.isNotADateTime()) 
	{   
		OmnAlarm << "Current DateTime Object is invalid" << enderr;
		return false;
	}   
	value.setDateTime(dt);
	return true;
}

bool 
AosSysDate::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 0)
	{
		errmsg << "SysDate donnot needs parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosSysDate::cloneJimo()  const
{
	try
	{
		return OmnNew AosSysDate(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

