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
#include "Funcs/Jimos/Strcmp.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStrcmp_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStrcmp(version);
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


AosStrcmp::AosStrcmp(const int version)
:
AosGenFunc("Strcmp", version)
{
}

AosStrcmp::AosStrcmp()
:
AosGenFunc("Strcmp", 1)
{
}



AosStrcmp::AosStrcmp(const AosStrcmp &rhs)
:
AosGenFunc(rhs)
{
}


AosStrcmp::~AosStrcmp()
{
}


bool
AosStrcmp::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosStrcmp::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eInt32;
}

	
bool
AosStrcmp::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString s1, s2;
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	s1 = mValue.getStr();
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	s2 = mValue.getStr();

	value.setI64(strcmp(s1.data(), s2.data()));
	return true;
}

bool 
AosStrcmp::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 2)
	{
		errmsg << "Strcmp needs 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosStrcmp::cloneJimo()  const
{
	try
	{
		return OmnNew AosStrcmp(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

