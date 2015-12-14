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
#include "Funcs/Jimos/CurTime.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCurTime_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCurTime(version);
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


AosCurTime::AosCurTime(const int version)
:
AosGenFunc("CurTime", version)
{
}

AosCurTime::AosCurTime()
:
AosGenFunc("CurTime", 1)
{
}



AosCurTime::AosCurTime(const AosCurTime &rhs)
:
AosGenFunc(rhs)
{
}


AosCurTime::~AosCurTime()
{
}


bool
AosCurTime::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCurTime::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosCurTime::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	char time_str[30];
	time_t tt = time(NULL);
	struct tm *t = localtime(&tt);
	sprintf(time_str, "%02u:%02u:%02u", 
			t->tm_hour, t->tm_min, t->tm_sec);
	OmnString timestr(time_str, strlen(time_str));
	//value.setValue(time_str, strlen(time_str));
	value.setStr(timestr);
	return true;
}

bool 
AosCurTime::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 0)
	{
		errmsg << "CurTime donnot needs parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosCurTime::cloneJimo()  const
{
	try
	{
		return OmnNew AosCurTime(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

