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
#include "Funcs/Jimos/Epoch2Str.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosEpoch2Str_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosEpoch2Str(version);
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


AosEpoch2Str::AosEpoch2Str(const int version)
:
AosGenFunc("Epoch2Str", version)
{
}

AosEpoch2Str::AosEpoch2Str()
:
AosGenFunc("Epoch2Str", 1)
{
}



AosEpoch2Str::AosEpoch2Str(const AosEpoch2Str &rhs)
:
AosGenFunc(rhs)
{
}


AosEpoch2Str::~AosEpoch2Str()
{
}


bool
AosEpoch2Str::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosEpoch2Str::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}


bool
AosEpoch2Str::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	i64 epoch_time = mValue.getI64();
	
	OmnString date_str = AosTimeUtil::epochToStrNew("%Y-%m-%d %H:%M:%S", epoch_time);
	value.setStr(date_str);
	return true;
}


bool 
AosEpoch2Str::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "Epoch2Str needs  1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosEpoch2Str::cloneJimo()  const
{
	try
	{
		return OmnNew AosEpoch2Str(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

