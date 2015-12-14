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
// 2013/12/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Jimos/ActRunDataEngine.h"


#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/DataEngineObj.h"
#include "SEInterfaces/TaskDataObj.h"


AosActRunDataEngine::AosActRunDataEngine(
		const AosRundataPtr &rdata, 
		const OmnString &version)
:
AosSdocAction("run_data_engine", version)
{
}


AosActRunDataEngine::~AosActRunDataEngine()
{
}


bool
AosActRunDataEngine::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &worker_doc,
		const AosRundataPtr &rdata)
{
	if (!worker_doc)
	{
		AosSetErrorUser3(rdata, "actrundataengine_missing_worker_doc", "Syntax Error") << enderr;
		return false;
	}

	if (!task)
	{
		AosSetError(rdata, "internal_erro") << enderr;
		return false;
	}

	mTask = task;

	AosDataEngineObjPtr data_engine = createDataEngine(rdata, worker_doc);
	if (!data_engine)
	{
		actionFinished(rdata, false, 0);
		return false;
	}

	data_engine->setTask(rdata, task);
	if (!data_engine->startEngine(rdata))
	{
		actionFinished(rdata, false, 0);
		return false;
	}
	
	actionFinished(rdata, true, 0);
	return true;
}


AosDataEngineObjPtr
AosActRunDataEngine::createDataEngine(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo)
	{
		AosSetErrorUser3(rdata, "actrundataengine_001", "Config Error") 
			<< worker_doc << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AOS_JIMOTYPE_DATA_ENGINE)
	{
		AosSetErrorUser3(rdata, "actrundataengine_002", "Config Error") 
			<< worker_doc << enderr;
		return 0;
	}

	AosDataEngineObjPtr data_engine = dynamic_cast<AosDataEngineObj*>(jimo.getPtr());
	if (!data_engine)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return data_engine;
}


bool
AosActRunDataEngine::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	AosDataEngineObjPtr engine = createDataEngine(rdata, def);
	if (!engine) return false;

	return engine->checkConfig(rdata, def, task);
}


AosJimoPtr
AosActRunDataEngine::cloneJimo() const
{
	try
	{
		return OmnNew AosActRunDataEngine(*this);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Internal error" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


AosActionObjPtr
AosActRunDataEngine::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActRunDataEngine(*this);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


void 
AosActRunDataEngine::actionFinished(
		const AosRundataPtr &rdata,
		const bool rslt, 
		const AosXmlTagPtr &status_doc)
{
	OmnScreen << "Data Engine finished: " << rslt << endl;
	if (!mTask)
	{
		OmnScreen << "data finished: " << rslt << endl;
		return;
	}

	// if (!rslt)
	// {
	// 	actionFailed(rdata);
	// }
	// else
	// {
	// 	actionFinished(rdata);
	// }
	mTask = 0;
}

