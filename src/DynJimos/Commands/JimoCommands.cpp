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
// 2013/06/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/Commands/JimoCommands.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DynJimos/Commands/JimoCommandsShell.h"
#include "DynJimos/Commands/CommandsImpl.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionType.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

AosJimoCommands::AosJimoCommands(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc)
{
	if (!init(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosJimoCommands::AosJimoCommands(
		const AosJimoCommands &rhs)
:
AosJimo(rhs)
{
}

AosJimoCommands::~AosJimoCommands()
{
}


bool
AosJimoCommands::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return config(rdata, worker_doc, jimo_doc);
}


bool
AosJimoCommands::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	registerMethod(AOSACTTYPE_CREATEDOC, sgFuncMap, 
	 		AosMethodId::eAosRundata_Worker,
	 		(void *)AosJimoCommands_CreateDoc);

	registerMethod(AOSACTTYPE_MODIFYATTR, sgFuncMap, 
	 		AosMethodId::eAosRundata_Worker,
	 		(void *)AosJimoCommands_ModifyDocAttr);

	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosJimoCommands::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


bool 
AosJimoCommands::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	// 'worker' is the doc to specify the command to run.
	if (!worker_doc)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}
	OmnString command_id = worker_doc->getAttrStr(AOSTAG_COMMAND_ID);
	if (command_id == "")
	{
		AosSetErrorUser(rdata, "missing_command_id") 
			<< ": " << worker_doc->toString() << enderr;
		return false;
	}

	AosMethodId::E method_id;
	void *func = AosJimo::getMethod(rdata, command_id, sgFuncMap, method_id);
	if (method_id != AosMethodId::eAosRundata_Worker)
	{
		AosSetErrorUser(rdata, "invalid_method_type") 
			<< method_id << enderr;
		return false;
	}

	AosMethodType_Rundata_WorkerDoc method = (AosMethodType_Rundata_WorkerDoc)func;
	return (*method)(rdata, worker_doc);
}


OmnString
AosJimoCommands::toString() const
{
	return AosJimo::toString();
}


bool 
AosJimoCommands::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoCommands::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoCommands::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosJimoCommands(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


void * 
AosJimoCommands::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}

