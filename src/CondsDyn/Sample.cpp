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
#include "CondsDyn/Sample.h"

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

AosDLLObjPtr AosCreateDLLCondSample(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc, 
 		const OmnString &libname,
 		const OmnString &version) 
{
	rdata->setDLLObj(0);
	try
	{
		AosDLLObjPtr obj = OmnNew AosDLLCondSample(rdata, 0, 
				libname, "AosCreateDLLCondSample", version);
		aos_assert_rr(obj, rdata, 0);
		rdata->setDLLObj(obj);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed running DLLCondSample DLL function");
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosDLLCondSample::AosDLLCondSample(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLCondObj("sample", libname, method, version)
{
	if (!config(rdata, sdoc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDLLCondSample::~AosDLLCondSample()
{
}


bool
AosDLLCondSample::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosDLLCondSample::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnScreen << "Evaluating condition: [def, rdata]" << endl;
	return true;
}


bool 
AosDLLCondSample::evalCond(const AosRundataPtr &rdata)
{
	OmnScreen << "Evaluating condition: [rdata]" << endl;
	return true;
}


bool 
AosDLLCondSample::evalCond(const char *record, const int len, const AosRundataPtr &rdata)
{
	OmnScreen << "Evaluating condition: [record, len, rdata]" << endl;
	return true;
}


bool 
AosDLLCondSample::evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnScreen << "Evaluating condition: [record, rdata]" << endl;
	return true;
}


bool 
AosDLLCondSample::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnScreen << "Evaluating condition: [value, rdata]" << endl;
	return true;
}

