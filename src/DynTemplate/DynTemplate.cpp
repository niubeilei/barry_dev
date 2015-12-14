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
// 2013/05/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynTemplate/DynTemplate.h"

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

AosDLLObjPtr AosCreateDLLActionDynTemplate(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc, 
 		const OmnString &libname,
 		const OmnString &version) 
{
	rdata->setDLLObj(0);
	try
	{
		OmnScreen << "To create DLL: " << libname << ":" << version << endl;
		AosDLLObjPtr obj = OmnNew AosDLLActionDynTemplate(rdata, sdoc, 
				libname, "AosCreateDLLActionDynTemplate", version);
		aos_assert_rr(obj, rdata, 0);
		rdata->setDLLObj(obj);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed running DLLActionDynTemplate DLL function");
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosDLLActionDynTemplate::AosDLLActionDynTemplate(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLActionObj("dyn_template", libname, method, version)
{
	if (!config(rdata, sdoc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDLLActionDynTemplate::~AosDLLActionDynTemplate()
{
}


bool
AosDLLActionDynTemplate::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	OmnScreen << "To configure AosDLLActionDynTemplate" << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::serializeTo: " << rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::serializeFrom: " << rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::abortAction(const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::abortAction: " << rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(def, rdata): " 
		<< rdata->getSiteid() << endl;
	if (def) 
	{
		OmnScreen << "def: " << def->toString() << endl;
	}
	return true;
}


bool 
AosDLLActionDynTemplate::run(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(value, rdata): " 
		<< rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::run(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(record, rdata): " 
		<< rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::run(const AosDataBlobObjPtr &blob, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(blob, rdata): " 
		<< rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::run(const char *data, const int len, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(data, len, rdata): " 
		<< rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::run(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(buff, rdata): " 
		<< rdata->getSiteid() << endl;
	return true;
}


bool 
AosDLLActionDynTemplate::run(const AosTaskObjPtr &task, 
					const AosXmlTagPtr &sdoc,
					const AosTaskDataObjPtr &def, 
					const AosRundataPtr &rdata)
{
	OmnScreen << "AosDLLActionDynTemplate::run(task, sdoc, def, rdata): " 
		<< rdata->getSiteid() << endl;
	return true;
}

