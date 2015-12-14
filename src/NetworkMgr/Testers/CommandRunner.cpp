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
#include "NetworkMgr/Testers/CommandRunner.h"

#include "Rundata/Rundata.h"
#include "Tester/TestMgr.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Buff.h"



AosCommandRunnerTester::AosCommandRunnerTester(const AosModuleId::E module_id)
:
mModuleId(module_id)
{
}


bool 
AosCommandRunnerTester::runCommand(
		const OmnString &args,
		const AosXmlTagPtr &root,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCommandRunnerTester::runCommand(
		const AosNetCommand::E command_id,
		const AosXmlTagPtr &msg,
		const AosRundataPtr &rdata)
{
	OmnScreen << "To run command: " << command_id << ": " << msg->toString() << endl;
	AosBuffPtr buff = msg->getNodeTextBinaryUnCopy();
	aos_assert_rr(buff, rdata, false);
	AosModuleId::E module_id = (AosModuleId::E)buff->getU32(0);
	AosNetCommand::E cid = (AosNetCommand::E)buff->getU32(0);
	aos_assert_rr(cid == command_id, rdata, false);
	aos_assert_rr(mModuleId == module_id, rdata, false);
	return true;
}


