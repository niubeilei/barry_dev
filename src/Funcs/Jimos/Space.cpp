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
#include "Funcs/Jimos/Space.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSpace_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSpace(version);
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


AosSpace::AosSpace(const int version)
:
AosGenFunc("Space", version)
{
}

AosSpace::AosSpace()
:
AosGenFunc("Space", 1)
{
}



AosSpace::AosSpace(const AosSpace &rhs)
:
AosGenFunc(rhs)
{
}


AosSpace::~AosSpace()
{
}


bool
AosSpace::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosSpace::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosSpace::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString str;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	int times = mValue.getI64();
	for (int i=0; i<times; i++) str << " ";
	value.setStr(str);	
	
	return true;
}

bool 
AosSpace::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "Space just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosSpace::cloneJimo()  const
{
	try
	{
		return OmnNew AosSpace(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

