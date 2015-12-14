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
#include "Funcs/Jimos/Char.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosChar_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosChar(version);
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


AosChar::AosChar(const int version)
:
AosGenFunc("Char", version)
{
}

AosChar::AosChar()
:
AosGenFunc("Char", 1)
{
}



AosChar::AosChar(const AosChar &rhs)
:
AosGenFunc(rhs)
{
}


AosChar::~AosChar()
{
}


bool
AosChar::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosChar::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosChar::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	char *s=new char[mSize+1];
	int j=0;
	for(u32 i=0;i<mSize;i++)
	{
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		double d = mValue.getDouble();
		if(d>32&&d<127)
		{
			char c=(int)d;
			s[j]=c;
			j++;
		}
	}
	s[j]='\0';
	string str(s,j);
	value.setStr(str);	
	return true;
}

bool 
AosChar::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize  < 1)
	{
		errmsg << "Abs just needs at least 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosChar::cloneJimo()  const
{
	try
	{
		return OmnNew AosChar(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

