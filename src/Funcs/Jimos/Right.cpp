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
#include "Funcs/Jimos/Right.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <string>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRight_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRight(version);
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


AosRight::AosRight(const int version)
:
AosGenFunc("Right", version)
{
}

AosRight::AosRight()
:
AosGenFunc("Right", 1)
{
}



AosRight::AosRight(const AosRight &rhs)
:
AosGenFunc(rhs)
{
}


AosRight::~AosRight()
{
}


bool
AosRight::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosRight::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosRight::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	OmnString str = mValue.getStr();
	if (str == "")
	{
		value.setStr(str);
		return true;
	}

	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	i64 len = mValue.getI64();
	OmnString sub_str;
	if (len > 0)
	{
		int start = 0;
		if (len > str.length()) 
		{
			len = str.length();
		}
		else
		{
			start = str.length() - len;
		}
		sub_str = str.subString(start, len);
	}
	value.setStr(sub_str);
	return true;
}


bool 
AosRight::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize !=2)
	{
		errmsg << "Length need only 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosRight::cloneJimo()  const
{
	try
	{
		return OmnNew AosRight(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

