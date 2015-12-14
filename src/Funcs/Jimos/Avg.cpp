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
#include "Funcs/Jimos/Avg.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAvg_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAvg(version);
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


AosAvg::AosAvg(const int version)
:
AosGenFunc("Avg", version)
{
}

AosAvg::AosAvg()
:
AosGenFunc("Avg", 1)
{
}



AosAvg::AosAvg(const AosAvg &rhs)
:
AosGenFunc(rhs)
{
}


AosAvg::~AosAvg()
{
}


bool
AosAvg::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosAvg::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosAvg::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d = 0;
	for (u32 i=0; i < mSize; i++)
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		d += mValue.getDouble();
	}
	value.setDouble(d/mSize);
	return true;
}

bool 
AosAvg::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1)
	{
		errmsg << "Avg needs at least 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosAvg::cloneJimo()  const
{
	try
	{
		return OmnNew AosAvg(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

