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
#include "Funcs/Jimos/Repeat.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <algorithm>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRepeat_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRepeat(version);
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


AosRepeat::AosRepeat(const int version)
:
AosGenFunc("Repeat", version)
{
}

AosRepeat::AosRepeat()
:
AosGenFunc("Repeat", 1)
{
}



AosRepeat::AosRepeat(const AosRepeat &rhs)
:
AosGenFunc(rhs)
{
}


AosRepeat::~AosRepeat()
{
}


bool
AosRepeat::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosRepeat::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosRepeat::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString oldstr = mValue.getStr();
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	int times = mValue.getI64();
	OmnString newstr;
	for (int i=0; i<times; i++) newstr << oldstr;
	
	value.setStr(newstr);
	return true;
}

bool 
AosRepeat::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize != 2)
	{
		errmsg << "Repeat needs 3 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosRepeat::cloneJimo()  const
{
	try
	{
		return OmnNew AosRepeat(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

