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
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CommandProc/CmdProcRestartServerFinished.h"
#include "CommandProc/CommandNames.h"
#include "NetworkMgr/NetworkMgr.h"


AosCmdProcRestartServerFinished::AosCmdProcRestartServerFinished(const bool regflag)
:
AosCommandProc(AOSCOMMAND_RESTARTSERVER_FINISHED, AosCommandType::eRestartServerFinished, regflag)
{
}


bool 
AosCmdProcRestartServerFinished::procCommand(
		const AosXmlTagPtr &cmd,
		const AosRundataPtr &rdata)
{
	// This is the response of 'restart server'. It calls NetworkMgr's 
	// member function "transResponded(...)".
	aos_assert_rr(cmd, rdata, false);
	bool rslt = AosNetworkMgr::getSelf()->transFinished(cmd, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}
#endif
