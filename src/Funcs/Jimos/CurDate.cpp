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
#include "Funcs/Jimos/CurDate.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCurDate_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCurDate(version);
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


AosCurDate::AosCurDate(const int version)
:
AosGenFunc("CurDate", version)
{
}

AosCurDate::AosCurDate()
:
AosGenFunc("CurDate", 1)
{
}



AosCurDate::AosCurDate(const AosCurDate &rhs)
:
AosGenFunc(rhs)
{
}


AosCurDate::~AosCurDate()
{
}


bool
AosCurDate::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCurDate::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosCurDate::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	char time_str[30];
	time_t tt = time(NULL);
	struct tm *t = localtime(&tt);
	sprintf(time_str, "%02u-%02u-%02u", 
			t->tm_year +1900, t->tm_mon+1, t->tm_mday);
	OmnString timestr(time_str, strlen(time_str));
	value.setStr(timestr);
	return true;
}

bool 
AosCurDate::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 0)
	{
		errmsg << "CurDate donnot needs parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosCurDate::cloneJimo()  const
{
	try
	{
		return OmnNew AosCurDate(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

