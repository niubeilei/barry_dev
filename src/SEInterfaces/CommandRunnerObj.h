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
#ifndef Aos_SEInterfaces_CommandRunnerObj_h
#define Aos_SEInterfaces_CommandRunnerObj_h

#include "alarm_c/alarm.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosCommandRunnerObj : virtual public OmnRCObject
{
public:
	static bool registerCommandRunner(
					const AosModuleId::E module_id,
					const AosCommandRunnerObjPtr &caller);

	static AosCommandRunnerObjPtr getCommandRunner(const AosModuleId::E module_id);

	virtual bool runCommand(
					const OmnString &args, 
					const AosXmlTagPtr &root, 
					const AosRundataPtr &rdata) = 0;

	virtual bool runCommand(
					const AosNetCommand::E command_id,
					const AosXmlTagPtr &msg,
					const AosRundataPtr &rdata);

	static bool runCommand(const AosRundataPtr &rdata);

	static bool runCommandStatic(
			const AosModuleId::E module_id,
			const OmnString &args,
			const AosXmlTagPtr &root,
			const AosRundataPtr &rdata);
};
#endif
