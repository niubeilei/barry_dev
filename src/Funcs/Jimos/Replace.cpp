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
#include "Funcs/Jimos/Replace.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <algorithm>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosReplace_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosReplace(version);
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


AosReplace::AosReplace(const int version)
:
AosGenFunc("Replace", version)
{
}

AosReplace::AosReplace()
:
AosGenFunc("Replace", 1)
{
}



AosReplace::AosReplace(const AosReplace &rhs)
:
AosGenFunc(rhs)
{
}


AosReplace::~AosReplace()
{
}


bool
AosReplace::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosReplace::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosReplace::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString str = mValue.getStr();
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	OmnString from_str = mValue.getStr();
	if (!AosGenFunc::getValue(rdata, 2, record)) return false;
	OmnString to_str = mValue.getStr();
	str.replace(from_str, to_str, true);
	
	value.setStr(str);
	return true;
}

bool 
AosReplace::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 3)
	{
		errmsg << "Replace needs 3 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosReplace::cloneJimo()  const
{
	try
	{
		return OmnNew AosReplace(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

