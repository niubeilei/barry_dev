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
#include "Funcs/Jimos/Cot.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCot_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCot(version);
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


AosCot::AosCot(const int version)
:
AosGenFunc("Cot", version)
{
}

AosCot::AosCot()
:
AosGenFunc("Cot", 1)
{
}



AosCot::AosCot(const AosCot &rhs)
:
AosGenFunc(rhs)
{
}


AosCot::~AosCot()
{
}


bool
AosCot::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCot::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosCot::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d = 0;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	d = mValue.getDouble();
	value.setDouble(1/tan(d));
	return true;
}

bool 
AosCot::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Asin need 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosCot::cloneJimo()  const
{
	try
	{
		return OmnNew AosCot(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

