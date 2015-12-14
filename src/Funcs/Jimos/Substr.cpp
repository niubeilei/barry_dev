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
#include "Funcs/Jimos/Substr.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <algorithm>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSubstr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSubstr(version);
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


AosSubstr::AosSubstr(const int version)
:
AosGenFunc("Substr", version)
{
}

AosSubstr::AosSubstr()
:
AosGenFunc("Substr", 1)
{
}



AosSubstr::AosSubstr(const AosSubstr &rhs)
:
AosGenFunc(rhs)
{
}


AosSubstr::~AosSubstr()
{
}


bool
AosSubstr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosSubstr::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}

	
bool
AosSubstr::getValue(
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
	string oldstr = mValue.getStr();

	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	int pos = mValue.getI64();

	int len = 0;
	if (mSize  == 2)
	{
		len = oldstr.length() + 1 - pos;    	
	}
	else if (mSize == 3)
	{
		if (!AosGenFunc::getValue(rdata, 2, record)) return false;
		if (mValue.isNull())
		{
			value.setNull();
			return true;
		}

		len = mValue.getI64();
	}

//code just write for oracle
//if in mysql please use substing.
	string newstr;
	if (pos >= 0 && pos <= (int)oldstr.length())
	{
		if (pos == 0)
		{
			pos = 1;
		}
		newstr = oldstr.substr(pos - 1, len);
	}
	else if (pos < 0 && abs(pos) <= oldstr.length())
	{
		pos = oldstr.length() - abs(pos);
		newstr = oldstr.substr(pos, len);
	}
	
	value.setStr(newstr);
	return true;
}


bool 
AosSubstr::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize < 2)
	{
		errmsg << "Substr needs at least 2 parameters";
		return false;
	}
	else if (mSize > 3)
	{
		errmsg << "Substr needs at most 3 parameters"; 
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosSubstr::cloneJimo()  const
{
	try
	{
		return OmnNew AosSubstr(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

