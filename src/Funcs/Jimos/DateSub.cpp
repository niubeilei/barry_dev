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
#include "Funcs/Jimos/DateSub.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApiS.h"

#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/exception/all.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDateSub_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateSub(version);
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


AosDateSub::AosDateSub(const int version)
:
AosGenFunc("DateSub", version)
{
}

AosDateSub::AosDateSub()
:
AosGenFunc("DateSub", 1)
{
}



AosDateSub::AosDateSub(const AosDateSub &rhs)
:
AosGenFunc(rhs)
{
}


AosDateSub::~AosDateSub()
{
}


bool
AosDateSub::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateSub::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDateSub::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString date_str = mValue.getStr();

	//2015/8/3 xuqi JIMODB-214
	using namespace boost::xpressive;
	cregex reg_format = cregex::compile("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9][\\s][0-9][0-9]:[0-9][0-9]:[0-9][0-9]");
	if(!regex_match(date_str.data(),reg_format))
	{
		rdata->setJqlMsg("Error: incorrect date format. correct format:'year-mounth-day hour:minute:second'.");
		return false;
	}

	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	OmnString opr_str = mValue.getStr();

	ptime p1;
	try
	{
		p1 = time_from_string(date_str.data());
	}
	catch (boost::exception &e)
	{
		OmnScreen << diagnostic_information(e) << endl;
	    //2015/8/3 xuqi JIMODB-213
		rdata->setJqlMsg("Error: incorrect date format. correct format:'year-mounth-day hour:minute:second'.");
		return false;
	}

	opr_str.replaceStrWithSpace();
	vector<OmnString> values;
	int n = AosSplitStr(opr_str, " ", values, 4);
	if (n != 3 && values.size() != 3)
	{
		rdata->setJqlMsg("Error: incorrect INTERVAL expr format.  correct format: 'INTERVAL [number] [time Key word]'.");
		return false;
	}
	
	int num = values[1].toInt();
	OmnString type = values[2];
	type.toUpper();

    ptime p2;
	if(type == "SECOND")
	{
		p2 = p1 - seconds(num);
	}
	else if(type == "MINUTE")
	{
		p2 = p1 - minutes(num);
	}
	else if(type == "HOUR")
	{
		p2 = p1 - hours(num);
	}
	else if(type == "DAY")
	{
		p2 = p1 - days(num);
	}
	else if(type == "MONTH")
	{
		p2 = p1 - months(num);
	}
	else if(type == "YEAR")
	{
		p2 = p1 - years(num);
	}
	else
	{
		OmnString msg = "Error: unrecognized INTERVAL expr type '";
		msg << type << "'.";
		rdata->setJqlMsg(msg);
		return false;
	}

	//string strval = to_simple_string(p2);
	AosDateTime dt(p2, "");
	if (dt.isNotADateTime()) 
	{   
		OmnAlarm << "Current DateTime Object is invalid" << enderr;
		return false;
	}   
	value.setDateTime(dt);
	return true;
}

bool 
AosDateSub::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 2)
	{
		errmsg << "DateSub needs 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateSub::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateSub(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
