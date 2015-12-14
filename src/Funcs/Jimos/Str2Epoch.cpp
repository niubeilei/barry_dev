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
#include "Funcs/Jimos/Str2Epoch.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "UtilTime/TimeUtil.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStr2Epoch_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStr2Epoch(version);
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


AosStr2Epoch::AosStr2Epoch(const int version)
:
AosGenFunc("Str2Epoch", version)
{
}

AosStr2Epoch::AosStr2Epoch()
:
AosGenFunc("Str2Epoch", 1)
{
}



AosStr2Epoch::AosStr2Epoch(const AosStr2Epoch &rhs)
:
AosGenFunc(rhs)
{
}


AosStr2Epoch::~AosStr2Epoch()
{
}


bool
AosStr2Epoch::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosStr2Epoch::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}

	
bool
AosStr2Epoch::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString data_str = mValue.getStr();
	
	//int t_year, t_month, t_day, t_hour, t_minute, t_second;
	//sscanf(data_str.data(), "%d-%d-%d %d:%d:%d", 
			//&t_year, &t_month, &t_day, &t_hour, &t_minute, &t_second); 
	i64 epochTime = AosTimeUtil::str2EpochTime(data_str);
	value.setI64(epochTime);
	return true;
}

bool 
AosStr2Epoch::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Str2Epoch needs  1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosStr2Epoch::cloneJimo()  const
{
	try
	{
		return OmnNew AosStr2Epoch(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

