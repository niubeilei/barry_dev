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
#include "Funcs/Jimos/Substring.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include <algorithm>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSubstring_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSubstring(version);
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


AosSubstring::AosSubstring(const int version)
:
AosGenFunc("Substring", version)
{
}

AosSubstring::AosSubstring()
:
AosGenFunc("Substring", 1)
{
}



AosSubstring::AosSubstring(const AosSubstring &rhs)
:
AosGenFunc(rhs)
{
}


AosSubstring::~AosSubstring()
{
}


bool
AosSubstring::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosSubstring::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}
	

bool
AosSubstring::getValue(
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
	OmnString oldstr = mValue.getStr();
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	if (mValue.isNull())
	{
		value.setNull();
		return true;
	}
	int pos = mValue.getI64();

	int len = 0;
	if (mSize == 2)
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

	string newstr;
	if (pos > 0 && pos <= (int)oldstr.length())
	{
		newstr = oldstr.subString(pos - 1, len);
	}
	else if (pos < 0 && abs(pos) <= oldstr.length())
	{
		pos = oldstr.length() - abs(pos);
		newstr = oldstr.subString(pos, len);
	}
	
	value.setStr(newstr);
	return true;
}


bool 
AosSubstring::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize < 2)
	{
		errmsg << "Substring needs at least 2 parameters";
		return false;
	}
	else if (mSize > 3)
	{
		errmsg << "Substring needs at most 3 parameters"; 
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosSubstring::cloneJimo()  const
{
	try
	{
		return OmnNew AosSubstring(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

