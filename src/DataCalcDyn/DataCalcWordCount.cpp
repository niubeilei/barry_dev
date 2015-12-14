////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCalcDyn/DataCalcWordCount.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataCalcDyn/DataCalcNames.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosDLLObjPtr AosCreateDataCalcWordCount(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc, 
 		const OmnString &libname,
 		const OmnString &version) 
{
	rdata->setDLLObj(0);
	try
	{
		AosDLLObjPtr obj = OmnNew AosDataCalcWordCount(rdata, 0, 
				libname, "AosCreateDataCalcWordCount", version);
		aos_assert_rr(obj, rdata, 0);
		rdata->setDLLObj(obj);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed running DataCalcWordCount DLL function");
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosDataCalcWordCount::AosDataCalcWordCount(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDataCalc(AOSDATACALC_WORDCOUNT, libname, method, version)
{
	if (!config(rdata, sdoc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataCalcWordCount::~AosDataCalcWordCount()
{
}


bool
AosDataCalcWordCount::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	if (!def) return true;

	return true;
}


bool
AosDataCalcWordCount::run(
		const AosRundataPtr &rdata,
		const char *field,
		const int len,
		AosValueRslt &output)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataCalcWordCount::run(
		const AosRundataPtr &rdata,
		AosValueRslt &input,
		AosValueRslt &output)
{
	OmnNotImplementedYet;
	return false;
}

