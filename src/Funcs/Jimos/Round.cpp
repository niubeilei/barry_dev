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
#include "Funcs/Jimos/Round.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRound_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRound(version);
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


AosRound::AosRound(const int version)
:
AosGenFunc("Round", version)
{
}

AosRound::AosRound()
:
AosGenFunc("Round", 1)
{
}



AosRound::AosRound(const AosRound &rhs)
:
AosGenFunc(rhs)
{
}


AosRound::~AosRound()
{
}


bool
AosRound::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosRound::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosRound::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double k = 0, tmp = 1;
	double d1 = 0, d2 = 0;

	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	d1 = mValue.getDouble();

	if (mSize> 1)
	{
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		d2 = mValue.getDouble();
	}

	k = (int)d2;

	if (mSize == 1 || k == 0)
	{
		value.setI64( round(d1) );
		return true;
	}

	if(k > 0)
	{
		for (u32 i = 0 ; i < k; i++)
		{
			tmp = tmp * 10;
		} 
	}
	else 
	{
		for (i32 i = 0; i < k; i++)
		{
			tmp = tmp / 10;
		}
	}
	value.setI64( round(d1*tmp)/tmp);
	return true;
}


bool 
AosRound::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 1)
	{
		errmsg << "Round need 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosRound::cloneJimo()  const
{
	try
	{
		return OmnNew AosRound(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

