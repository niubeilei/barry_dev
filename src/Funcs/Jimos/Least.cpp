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
#include "Funcs/Jimos/Least.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosLeast_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosLeast(version);
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


AosLeast::AosLeast(const int version)
:
AosGenFunc("Least", version)
{
}

AosLeast::AosLeast()
:
AosGenFunc("Least", 1)
{
}



AosLeast::AosLeast(const AosLeast &rhs)
:
AosGenFunc(rhs)
{
}


AosLeast::~AosLeast()
{
}


bool
AosLeast::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosLeast::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	OmnNotImplementedYet;		
	return AosDataType::eInvalid;
}


bool
AosLeast::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	bool found = false;
	value.setNull();
	for (u32 i=0; i<mSize; i++)
	{
		if (!AosGenFunc::getValue(rdata, i, record)) return false;
		if (mValue.isNull())
		{
			continue;
		}

		if (!found)
		{
			value = mValue;
			found = true;
		}
		else 
		{
			if (mValue < value) value = mValue;
		}
	}
	return true;
}


bool 
AosLeast::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "Least needs at least 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosLeast::cloneJimo()  const
{
	try
	{
		return OmnNew AosLeast(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

