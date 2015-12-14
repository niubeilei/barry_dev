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
#include "Funcs/Jimos/BitwiseAnd.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBitwiseAnd_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosBitwiseAnd(version);
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


AosBitwiseAnd::AosBitwiseAnd(const int version)
:
AosGenFunc("AosBitwiseAnd", version)
{
}

AosBitwiseAnd::AosBitwiseAnd()
:
AosGenFunc("AosBitwiseAnd", 1)
{
}



AosBitwiseAnd::AosBitwiseAnd(const AosBitwiseAnd &rhs)
:
AosGenFunc(rhs)
{
}


AosBitwiseAnd::~AosBitwiseAnd()
{
}


bool
AosBitwiseAnd::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosBitwiseAnd::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eDouble;
}


bool
AosBitwiseAnd::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	//u32 nums=parms->size();
	double d1;
	double d2;
	double d;
	for (u32 i=0; i < mSize; i++)
	{
		AosGenFunc::getValue(rdata, i, record);
		//if (!(*parms)[i]->getValue(rdata, record, vv)) return false;
		if (i == 0)
		{
			d1 = mValue.getDouble();
			d = d1;
		}
		else 
		{
			d2 = mValue.getDouble();
			d = (int64_t)d&(int64_t)d2;
		}
	}
	value.setDouble(d);
	return true;
}

bool 
AosBitwiseAnd::syntaxCheck(
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
AosBitwiseAnd::cloneJimo()  const
{
	try
	{
		return OmnNew AosBitwiseAnd(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

