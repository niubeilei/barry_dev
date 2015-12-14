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
#include "ActionDyn/Sample.h"

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

AosDLLObjPtr AosCreateDLLActionSample(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc, 
 		const OmnString &libname,
 		const OmnString &version) 
{
	rdata->setDLLObj(0);
	try
	{
		AosDLLObjPtr obj = OmnNew AosDLLActionSample(rdata, 0, 
				libname, "AosCreateDLLActionSample", version);
		aos_assert_rr(obj, rdata, 0);
		rdata->setDLLObj(obj);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed running DLLActionSample DLL function");
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosDLLActionSample::AosDLLActionSample(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLActionObj("sample", libname, method, version)
{
	if (!config(rdata, sdoc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDLLActionSample::~AosDLLActionSample()
{
}


bool
AosDLLActionSample::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosDLLActionSample::serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnScreen << "run(buff, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnScreen << "run(buff, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::abortAction(const AosRundataPtr &rdata)
{
	OmnScreen << "run(rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const AosXmlTagPtr &def, const AosRundataPtr &rundata)
{
	OmnScreen << "run(def, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnScreen << "run(value, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnScreen << "run(record, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const AosDataBlobObjPtr &blob, const AosRundataPtr &rdata)
{
	OmnScreen << "run(blob, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const char *data, const int len, const AosRundataPtr &rdata)
{
	OmnScreen << "run(data, len, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnScreen << "run(buff, rdata)" << endl;
	return true;
}


bool 
AosDLLActionSample::run(const AosTaskObjPtr &task, 
					const AosXmlTagPtr &sdoc,
					const AosTaskDataObjPtr &def, 
					const AosRundataPtr &rundata)
{
	OmnScreen << "run(task, sdoc, def, rdata)" << endl;
	return true;
}


