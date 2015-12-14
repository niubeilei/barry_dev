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
#include "Funcs/Jimos/Ceil.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCeil_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCeil(version);
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


AosCeil::AosCeil(const int version)
:
AosGenFunc("Ceiling", version)
{
}

AosCeil::AosCeil()
:
AosGenFunc("Ceiling", 1)
{
}



AosCeil::AosCeil(const AosCeil &rhs)
:
AosGenFunc(rhs)
{
}


AosCeil::~AosCeil()
{
}


bool
AosCeil::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCeil::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosCeil::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	double d = 0;
	if (!AosGenFunc::getValue(rdata, 0,  record)) return false;
	d = mValue.getDouble();
	value.setI64( ceil(d) );
	return true;
}

bool 
AosCeil::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Ceil need 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosCeil::cloneJimo()  const
{
	try
	{
		return OmnNew AosCeil(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

