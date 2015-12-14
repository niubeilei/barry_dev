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
#include "Funcs/Jimos/Addsec.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time.hpp"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <string>
using namespace boost::gregorian;
using namespace boost::posix_time;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAddsec_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAddsec(version);
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


AosAddsec::AosAddsec(const int version)
:
AosGenFunc("Addsec", version)
{
}

AosAddsec::AosAddsec()
:
AosGenFunc("Addsec", 1)
{
}



AosAddsec::AosAddsec(const AosAddsec &rhs)
:
AosGenFunc(rhs)
{
}


AosAddsec::~AosAddsec()
{
}


bool
AosAddsec::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosAddsec::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;		
}


bool
AosAddsec::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString str1, s;
	double n;
	if(mSize != 2)
	{
		return false;
	}
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;

	str1 = mValue.getStr();
	if (str1 == "" || str1 == "0") str1 = "1970-01-01 00:00:00";
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	n = mValue.getDouble();

	try
	{	
		if(str1.data()[4] != '-')
		{
			OmnString str;
			const char* p_ch = str1.data();
			str.append(p_ch,4)
			   .appendChar('-')
			   .append(p_ch+4,2)
			   .appendChar('-')
			   .append(p_ch+6,2)
			   .appendChar(' ')
			   .append(p_ch+8,2)
			   .appendChar(':')
			   .append(p_ch+10,2)
			   .appendChar(':')
			   .append(p_ch+12,2);
			str1 = str;
		}
		ptime p1 = time_from_string(str1.data());
		ptime p2 = p1 + seconds(n);
		s = to_simple_string(p2);
	}
	catch(...)
	{
		OmnAlarm << "error ....." << enderr;
	}
	value.setStr(s);
	return true;
}


bool 
AosAddsec::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 2)
	{
		errmsg << "Length need only 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}


AosJimoPtr
AosAddsec::cloneJimo()  const
{
	try
	{
		return OmnNew AosAddsec(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
