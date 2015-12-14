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
#include "Funcs/Jimos/Coalesce.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCoalesce_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCoalesce(version);
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


AosCoalesce::AosCoalesce(const int version)
:
AosGenFunc("Coalesce", version)
{
}


AosCoalesce::AosCoalesce()
:
AosGenFunc("Coalesce", 1)
{
}


AosCoalesce::AosCoalesce(const AosCoalesce &rhs)
:
AosGenFunc(rhs)
{
}


AosCoalesce::~AosCoalesce()
{
}


bool
AosCoalesce::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCoalesce::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	OmnNotImplementedYet;	
	return AosDataType::eInvalid;
}


bool
AosCoalesce::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	value.setNull();
	for (size_t i=0; i<mSize; i++)
	{
		if (!(AosGenFunc::getValue(rdata, i, record))) return false;
		if (!mValue.isNull())
		{
			value = mValue;
			return true;
		}
	}

	return true;
}


bool 
AosCoalesce::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize <= 1)
	{
		errmsg << "Max needs at least 1 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosCoalesce::cloneJimo()  const
{
	try
	{
		return OmnNew AosCoalesce(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

