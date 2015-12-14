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
//
// Modification History:
// 06/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_NetworkMgr_Testers_CommandRunnerTester_h
#define Aos_NetworkMgr_Testers_CommandRunnerTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/CommandRunnerObj.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosCommandRunnerTester : public AosCommandRunnerObj
{
	OmnDefineRCObject;

private:
	AosModuleId::E 	mModuleId;

public:
	AosCommandRunnerTester(const AosModuleId::E module_id);
	~AosCommandRunnerTester() {};

	virtual bool runCommand(const OmnString &args,
						const AosXmlTagPtr &root,
						const AosRundataPtr &rdata);

	virtual bool runCommand(
						const AosNetCommand::E command_id,
						const AosXmlTagPtr &msg,
						const AosRundataPtr &rdata);

};
#endif
