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
#include "Funcs/Jimos/BitwiseOr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBitwiseOr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBitwiseOr(version);
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


AosBitwiseOr::AosBitwiseOr(const int version)
:
AosGenFunc("AosBitwiseOr", version)
{
}

AosBitwiseOr::AosBitwiseOr()
:
AosGenFunc("AosBitwiseOr", 1)
{
}



AosBitwiseOr::AosBitwiseOr(const AosBitwiseOr &rhs)
:
AosGenFunc(rhs)
{
}


AosBitwiseOr::~AosBitwiseOr()
{
}


bool
AosBitwiseOr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosBitwiseOr::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosBitwiseOr::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d1;
	double d2;
	double d;
	for (u32 i=0; i<mSize; i++)
	{
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		if (i == 0)
		{
			d1 = mValue.getDouble();
			d=d1;
		}
		else 
		{
			d2 = mValue.getDouble();
			d=(int64_t)d|(int64_t)d2;
		}
	}
	value.setDouble(d);
	return true;
}

bool 
AosBitwiseOr::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize < 2)
	{
		errmsg << "Abs just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosBitwiseOr::cloneJimo()  const
{
	try
	{
		return OmnNew AosBitwiseOr(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

