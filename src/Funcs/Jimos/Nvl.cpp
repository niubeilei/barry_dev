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
#include "Funcs/Jimos/Nvl.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosNvl_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosNvl(version);
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


AosNvl::AosNvl(const int version)
:
AosGenFunc("Nvl", version)
{
}

AosNvl::AosNvl()
:
AosGenFunc("Nvl", 1)
{
}



AosNvl::AosNvl(const AosNvl &rhs)
:
AosGenFunc(rhs)
{
}


AosNvl::~AosNvl()
{
}


bool
AosNvl::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosNvl::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	mDataType = mParms[0]->getDataType(rdata, record);
	aos_assert_r(mDataType != AosDataType::eInvalid, AosDataType::eInvalid);

	return mDataType;
}


bool
AosNvl::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	AosValueRslt vv = mValue;
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (vv.isNull())
	{
		value = mValue;
	}
	else 
	{
		value = vv;
	}
	return true;
}


bool 
AosNvl::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize != 2)
	{
		errmsg << "Nvl needs 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosNvl::cloneJimo()  const
{
	try
	{
		return OmnNew AosNvl(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

