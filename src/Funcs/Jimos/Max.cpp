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
#include "Funcs/Jimos/Max.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMax_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMax(version);
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


AosMax::AosMax(const int version)
:
AosGenFunc("Max", version)
{
}

AosMax::AosMax()
:
AosGenFunc("Max", 1)
{
}



AosMax::AosMax(const AosMax &rhs)
:
AosGenFunc(rhs)
{
}


AosMax::~AosMax()
{
}


bool
AosMax::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosMax::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosMax::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d;
	double d1;
	for (u32 i=0; i<mSize; i++)
	{
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		if (i == 0)
			d = mValue.getDouble();
		else {
			d1 = mValue.getDouble();
			if (d1 > d)
				d = d1;
		}
	}
	value.setDouble(d);
	return true;
}

bool 
AosMax::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "Max needs at least 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosMax::cloneJimo()  const
{
	try
	{
		return OmnNew AosMax(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

