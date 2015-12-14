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
#include "Funcs/Jimos/Truncate.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Number.h"

#include <cmath>


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTruncate_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosTruncate(version);
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


AosTruncate::AosTruncate(const int version)
:
AosGenFunc("Truncate", version)
{
}

AosTruncate::AosTruncate()
:
AosGenFunc("Truncate", 1)
{
}



AosTruncate::AosTruncate(const AosTruncate &rhs)
:
AosGenFunc(rhs)
{
}


AosTruncate::~AosTruncate()
{
}


bool
AosTruncate::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosTruncate::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosTruncate::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (mSize== 1)
	{
		OmnString date;
		string date_str, yy, mm, dd;

		//test: date_str:2014-11-07 16:04:32.0
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		date_str = mValue.getStr();
		if ( date_str == "sysdate")
		{
			time_t t = time(0); 
			char tmp[64]; 
			strftime( tmp, sizeof(tmp), "%Y-%m-%d ",localtime(&t) ); 
			date  = tmp;
		}
		else 
		{
			int len = date_str.size();
			aos_assert_r(len>=10, false);
			date = date_str.substr(0, 10);
		}
		value.setStr(date);
	}
	else if (mSize == 2)
	{
		if (!AosGenFunc::getValue(rdata, 0, record)) return false;
		if (mValue.isNull())
		{
			value.setNull();
			return true;
		}

		double d_val = mValue.getDouble();
		if (!AosGenFunc::getValue(rdata, 1, record)) return false;
		value.setDouble(d_val);
	}
	return true;
}


bool 
AosTruncate::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
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

	/*
	if (parms.size() != 2)
	{
		errmsg << "Truncate need 2 parameters.";
		return false;
	}
	*/

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosTruncate::cloneJimo()  const
{
	try
	{
		return OmnNew AosTruncate(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

