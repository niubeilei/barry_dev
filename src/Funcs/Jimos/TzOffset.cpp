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
#include "Funcs/Jimos/TzOffset.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTzOffset_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosTzOffset(version);
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


AosTzOffset::AosTzOffset(const int version)
:
AosGenFunc("ToChar", version)
{
}

AosTzOffset::AosTzOffset()
:
AosGenFunc("ToChar", 1)
{
}



AosTzOffset::AosTzOffset(const AosTzOffset &rhs)
:
AosGenFunc(rhs)
{
}


AosTzOffset::~AosTzOffset()
{
}


bool
AosTzOffset::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosTzOffset::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosTzOffset::getValue(
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
	OmnString data_str = mValue.getStr();

	OmnString time, time_str;
	int hour = 0;
	int min = 0;
	if(data_str == "SESSIONTIMEZONE" || data_str == "sessiontimezone" || data_str == "DBTIMEZONE" || data_str == "dbtimezone")
	{
		hour = OmnGetTimeZoneOffset() / 60;
		min = OmnGetTimeZoneOffset() % 60;
		if(min == 0)
		{
			time_str << ":"
				<< "00";
			if(hour < -10)
			{
				time << "+" 
					<< abs(hour)
					<< time_str;
			}
			else if(hour < 0 && hour > -10)
			{
				time << "+" 
					<< "0" 
					<< abs(hour)
					<< time_str;
			}
			else if(hour > 0 && hour < 10)
			{
				time << "-" 
					<< "0" 
					<< abs(hour)
					<< time_str;
			}
			else if(hour > 10 )
			{
				time << "-" 
					<< abs(hour)
					<< time_str;
			}
			else
			{
				time << "00:00";
			}
		}
		else
		{
			if(hour > 0 && hour < 10)
			{
				time << "-"
					<< "0" 
					<< hour
					<< ":"
					<< min;
				aos_assert_r(time != 0, false);
			}
			else if(hour > 10)
			{	
				time << "-"
					<< hour
					<< ":"
					<< min;
				aos_assert_r(time != 0, false);

			}
			else if(hour < 0 && hour > -10)
			{	
				time << "+"
					<< "0"
					<<hour
					<< ":"
					<< min;
				aos_assert_r(time != 0, false);

			}
			else if(hour < -10)
			{	
				time << "+"
					<<hour
					<< ":"
					<< min;
				aos_assert_r(time != 0, false);

			}	
			else
			{
				time << "00:00";
			}
		}
	}
	value.setStr(time);
	return true;
}

bool 
AosTzOffset::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	
	if (mSize != 1)
	{
		errmsg << "ToChar needs at 1 parameter.";
		return false;
	}
	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosTzOffset::cloneJimo()  const
{
	try
	{
		return OmnNew AosTzOffset(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

