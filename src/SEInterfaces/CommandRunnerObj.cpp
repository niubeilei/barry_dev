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
// 05/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/CommandRunnerObj.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
//#include "TransUtil/ModuleId.h"


static AosCommandRunnerObjPtr sgCommandRunners[AosModuleId::eMax];
static OmnMutex sgLock;

bool 
AosCommandRunnerObj::registerCommandRunner(
		const AosModuleId::E module_id,
		const AosCommandRunnerObjPtr &caller) 
{
	aos_assert_r(AosModuleId::isValid(module_id), false);
	sgCommandRunners[module_id] = caller;
	return true;
}


AosCommandRunnerObjPtr
AosCommandRunnerObj::getCommandRunner(const AosModuleId::E module_id)
{
	aos_assert_r(AosModuleId::isValid(module_id), 0);
	return sgCommandRunners[module_id];
}


bool
AosCommandRunnerObj::runCommand(const AosRundataPtr &rdata)
{
	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		AosSetErrorUser(rdata, "Missing request") << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	//Retrieve args 
	OmnString args = root->getChildTextByAttr("name", "args");
	if (args == "")
	{
		AosSetErrorUser(rdata, "missing_args") << root->toString() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString module_id = AosParseArgs(args, "mid");
	if (module_id == "")
	{
		AosSetErrorUser(rdata, "missing_module_id") << root->toString() << enderr;
		return false;
	}

	AosModuleId::E mid = AosModuleId::toEnum(module_id);
	sgLock.lock();
	AosCommandRunnerObjPtr runner = sgCommandRunners[mid];
	sgLock.unlock();
	if (!runner)
	{
		AosSetErrorUser(rdata, "missing_command_runner") << root->toString() << enderr;
		return false;
	}

	return runner->runCommand(args, root, rdata);
}


bool
AosCommandRunnerObj::runCommand(
		const AosNetCommand::E commandId,
		const AosXmlTagPtr &msg, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosCommandRunnerObj::runCommandStatic(
			const AosModuleId::E module_id,
			const OmnString &args,
			const AosXmlTagPtr &root,
			const AosRundataPtr &rdata)
{
	AosCommandRunnerObjPtr runner = getCommandRunner(module_id);
	aos_assert_r(runner, false);
	return runner->runCommand(args, root, rdata);
}

