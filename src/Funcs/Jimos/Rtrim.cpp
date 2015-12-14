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
#include "Funcs/Jimos/Rtrim.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <cmath>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosRtrim_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosRtrim(version);
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


AosRtrim::AosRtrim(const int version)
:
AosGenFunc("Rtrim", version)
{
}

AosRtrim::AosRtrim()
:
AosGenFunc("Rtrim", 1)
{
}


AosRtrim::AosRtrim(const AosRtrim &rhs)
:
AosGenFunc(rhs)
{
}


AosRtrim::~AosRtrim()
{
}


bool
AosRtrim::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosRtrim::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosRtrim::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	aos_assert_r(mSize>= 1 && mSize <=2, false);

	if (mSize == 1)
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		OmnString ss = mValue.getStr();
		ss.removeTailWhiteSpace();
		value.setStr(ss);	
	}
	else
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		OmnString s1 = mValue.getStr();
		s1.removeTailWhiteSpace();

		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		OmnString s2 = mValue.getStr();

		int len1 = s1.length();
		int len2 = s2.length();
		if (len2 == 0 || len2 > len1)
		{
			value.setStr(s1);	
		}
		if (len2 == 1)
		{
			i32 i = len1 - 1;
			while (i >= 0)
			{
				if (s1.data()[i] == *s2.data()) 
					break;
				i--;
			}
			if (i == 1)
			{
				OmnString temp = "";
				temp << s1.data()[0];
				s1 = temp;
			}
			if (i > 1)
			{
				s1 = s1.substr(0, i - 1);
			}
			value.setStr(s1);	
		}
		value.setStr(s1);	
	}
	return true;
}


bool 
AosRtrim::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize < 1)
	{
		errmsg << "Substr needs at least 1 parameters";
		return false;
	}
	else if (mSize > 2)
	{
		errmsg << "Substr needs at most 2 parameters"; 
		return false;
	}



	return true;
}


AosJimoPtr
AosRtrim::cloneJimo()  const
{
	try
	{
		return OmnNew AosRtrim(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

