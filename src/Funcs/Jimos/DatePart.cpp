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
#include "Funcs/Jimos/DatePart.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include <boost/xpressive/xpressive_dynamic.hpp>



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatePart_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatePart(version);
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


AosDatePart::AosDatePart(const int version)
:
AosGenFunc("DatePart", version)
{
}

AosDatePart::AosDatePart()
:
AosGenFunc("DatePart", 1)
{
}



AosDatePart::AosDatePart(const AosDatePart &rhs)
:
AosGenFunc(rhs)
{
}


AosDatePart::~AosDatePart()
{
}


bool
AosDatePart::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDatePart::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosDatePart::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	//aos_assert_r(record, false);
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString date_format = mValue.getStr();
	
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	OmnString data_str = mValue.getStr();

	int t_year, t_month, t_day, t_hour, t_minute, t_second;
	
	//2015/8/5 xuqi JIMODB-213
	bool isMatch = false;
	using namespace boost::xpressive;
	cregex reg_format = cregex::compile("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
	cregex reg_format1 = cregex::compile("[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9][\\s][0-9][0-9]:[0-9][0-9]:[0-9][0-9]");
	cregex reg_format2 = cregex::compile("[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]");
	 
	if(regex_match(data_str.data(),reg_format))
	{
		isMatch = true;
		sscanf(data_str.data(), "%4d%2d%2d%2d%2d%2d",
				&t_year, &t_month, &t_day, &t_hour, &t_minute, &t_second);
	}
	if(regex_match(data_str.data(),reg_format1))
    {
		isMatch = true;
		sscanf(data_str.data(), "%d-%d-%d %d:%d:%d",
				&t_year, &t_month, &t_day, &t_hour, &t_minute, &t_second);
	}
	if(regex_match(data_str.data(),reg_format2))                          
	{                                                                     
		isMatch = true;                                                   
	    sscanf(data_str.data(), "%4d%2d%2d",                      
				&t_year, &t_month, &t_day);
		t_hour = 0;
		t_minute = 0;
		t_second = 0;
 	}
	if(data_str.length() == 0 )
	{
		isMatch = true;
		t_year = 0;
		t_month = 0;
		t_day = 0;
		t_hour = 0;  
		t_minute = 0;
		t_second = 0;
	}
	if(!isMatch)
	{
		OmnString errmsg;
		errmsg<<"[ERR] : Date '"<<data_str.data()<<"' incorrect data format";
		rdata->setJqlMsg(errmsg);
		return false;
	}	

	// hard code by Young, 2014/09/15
	if (mSize >= 4)
	{
		if (!AosGenFunc::getValue(rdata, 2, record)) return false;
		OmnString opt_str = mValue.getStr();
		opt_str.toLower();

		if (!AosGenFunc::getValue(rdata, 3, record)) return false;
		int opt_num = mValue.getI64();

		if (opt_str == "y")
		{
			t_year += opt_num;
		}
		else if (opt_str == "m")
		{
			int tmp_year = (t_year * 12 + t_month + opt_num) / 12;
			int tmp_month = (t_year * 12 + t_month + opt_num) % 12;
			t_year = tmp_year;
			t_month = tmp_month;
		}
	}
	
	

	char strval[30];
	if (date_format == "yyyy")
	{
		sprintf(strval, "%d", t_year);
	}
	else if (date_format == "mm" || date_format == "m")
	{
		sprintf(strval, "%d", t_month);
	}
	else if (date_format == "dd" || date_format == "d")
	{
		sprintf(strval, "%d", t_day);
	}
	else if (date_format == "hh")
	{
		sprintf(strval, "%d", t_hour);
	}
	else if (date_format == "mi" || date_format == "n")
	{
		sprintf(strval, "%d", t_minute);
	}
	else if (date_format == "ss" || date_format == "s")
	{
		sprintf(strval, "%d", t_second);
	}
	else if (date_format == "yyyy-mm")
	{
		sprintf(strval, "%d-%02u", t_year, t_month);
	}
	else if (date_format == "yyyymmdd")
	{
		sprintf(strval, "%d%02u%02u", t_year, t_month, t_day);
	}
	else if (date_format == "mmddyyyy")
	{
		sprintf(strval, "%02u%02u%d", t_month, t_day, t_year);
	}
	else if (date_format == "yyyy-mm-dd")
	{
		sprintf(strval, "%d-%02u-%02u", t_year, t_month, t_day);
	}

	OmnString str(strval, strlen(strval));
	value.setStr(str);
	return true;
}

bool 
AosDatePart::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize < 2)
	{
		errmsg << "DatePart needs 2 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDatePart::cloneJimo()  const
{
	try
	{
		return OmnNew AosDatePart(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

