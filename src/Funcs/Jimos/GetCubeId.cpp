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
#include "Funcs/Jimos/GetCubeId.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGetCubeId_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGetCubeIdFunc(version);
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


AosGetCubeIdFunc::AosGetCubeIdFunc(const int version)
:
AosGenFunc("GetCubeIdFunc", version)
{
}

AosGetCubeIdFunc::AosGetCubeIdFunc()
:
AosGenFunc("GetCubeIdFunc", 1)
{
}



AosGetCubeIdFunc::AosGetCubeIdFunc(const AosGetCubeIdFunc &rhs)
:
AosGenFunc(rhs)
{
}


AosGetCubeIdFunc::~AosGetCubeIdFunc()
{
}


bool
AosGetCubeIdFunc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosGetCubeIdFunc::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eInt32;
}


bool
AosGetCubeIdFunc::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString vv;
	for (u32 i = 0; i < mSize; i++)
	{
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		if (!mValue.isNull())
		{
			vv <<mValue.getStr();
		}
	}
	int cube_id = AosGetCubeId(vv);
	value.setI64(cube_id);
	return true;
}

bool 
AosGetCubeIdFunc::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize > 0)
	{
		errmsg << "GetCubeIdFunc just needs 1 parameter";
		return false;
	}

	return true;
}

AosJimoPtr
AosGetCubeIdFunc::cloneJimo()  const
{
	try
	{
		return OmnNew AosGetCubeIdFunc(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

