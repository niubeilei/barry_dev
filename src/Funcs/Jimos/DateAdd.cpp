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
// 2015/06/24 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/DateAdd.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time.hpp"
#include <time.h>
using namespace boost::gregorian;
using namespace boost::posix_time;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDateAdd_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDateAdd(version);
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


AosDateAdd::AosDateAdd(const int version)
:
AosGenFunc("DateAdd", version)
{
}

AosDateAdd::AosDateAdd()
:
AosGenFunc("DateAdd", 1)
{
}



AosDateAdd::AosDateAdd(const AosDateAdd &rhs)
:
AosGenFunc(rhs)
{
}


AosDateAdd::~AosDateAdd()
{
}


bool
AosDateAdd::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosDateAdd::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eDateTime;
}


bool
AosDateAdd::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{	
	// parms should has thress parametets
	
	// 1. date time
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	string date_str = mValue.getStr();

	// 2. Operating unit
	if (!AosGenFunc::getValue(rdata, 1, record)) return false;
	OmnString date_unit = mValue.getStr();

	// 3. interval
	if (!AosGenFunc::getValue(rdata, 2, record)) return false;
	int val = mValue.getI64();

	ptime pt;
	try
	{
		pt = time_from_string(date_str.data());
	}
	catch(...)
	{
		value.setNull();
		return true;
	}

	if (pt.is_not_a_date_time())
	{
		value.setNull();
		return true;
	}

	date_unit.toUpper();
	if (date_unit == "DAY")
	{
		pt = pt + days(val);
		AosDateTime dt(pt, "");
		value.setDateTime(dt);
	}
	else if (date_unit == "MONTH")
	{
		pt = pt + months(val);	
		AosDateTime dt(pt, "");
		value.setDateTime(dt);
	}
	else if (date_unit == "YEAR")
	{
		pt = pt + years(val);	
		AosDateTime dt(pt, "");
		value.setDateTime(dt);
	}
	//2015/8/12 xuqi JIMODB-375
	//2015-12-13 wumeng JIMODB-1403
	else if (date_unit == "HOUR")
	{                            
		pt = pt + hours(val);    
	    AosDateTime dt(pt, "");  
		value.setDateTime(dt);
	}
	else if (date_unit == "MINUTE")
	{                            
		pt = pt + minutes(val);    
		AosDateTime dt(pt, "");  
		value.setDateTime(dt);      
	}                            
	else if (date_unit == "SECOND")
	{                            
		pt = pt + seconds(val);    
		AosDateTime dt(pt, "");  
		value.setDateTime(dt);      
	}                            

	return true;
}


bool 
AosDateAdd::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 3)
	{
		errmsg << "DateAdd needs 3 parameters.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosDateAdd::cloneJimo()  const
{
	try
	{
		return OmnNew AosDateAdd(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

