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
#include "Funcs/Jimos/Rand.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRand_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRand(version);
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


AosRand::AosRand(const int version)
:
AosGenFunc("Rand", version)
{
}

AosRand::AosRand()
:
AosGenFunc("Rand", 1)
{
}



AosRand::AosRand(const AosRand &rhs)
:
AosGenFunc(rhs)
{
}


AosRand::~AosRand()
{
}


bool
AosRand::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosRand::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eInt32;
}


bool
AosRand::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	int number;
	//char randstr[30];
	if (1 == mSize)
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		double seedval = mValue.getDouble();
		srand((int)seedval);
		number=rand()%(int)seedval;
	}
	else
	{
		number=rand();
	}
	value.setI64(number);
	return true;
}

bool 
AosRand::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize > 2)
	{
		errmsg << "Rand need 0 or 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosRand::cloneJimo()  const
{
	try
	{
		return OmnNew AosRand(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

