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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryVars/Jimos/QueryVarSysDate.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosQueryVarSysDate_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQueryVarSysDate(version);
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


AosQueryVarSysDate::AosQueryVarSysDate(const int version)
:
AosQueryVar("sys_date", version),
mSysTime(0)
{
}


AosQueryVarSysDate::~AosQueryVarSysDate()
{
}


bool
AosQueryVarSysDate::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosQueryVarSysDate::eval(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def, 
		AosValueRslt &value)
{
	i64 v = OmnGetSecond();	
	value.setValue(v);
	return true;
}


AosJimoPtr
AosQueryVarSysDate::cloneJimo()  const
{
	try
	{
		return OmnNew AosQueryVarSysDate(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

