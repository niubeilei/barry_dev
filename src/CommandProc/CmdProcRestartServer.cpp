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
#include "CommandProc/CmdProcRestartServer.h"
#include "CommandProc/CommandNames.h"
#include "NetworkMgr/NetworkMgr.h"


AosCmdProcRestartServer::AosCmdProcRestartServer(const bool regflag)
:
AosCommandProc(AOSCOMMAND_RESTARTSERVER, AosCommandType::eRestartServer, regflag)
{
}


bool 
AosCmdProcRestartServer::procCommand(
		const AosXmlTagPtr &cmd,
		const AosRundataPtr &rdata)
{
	//<cmd zky_cmd_type="restartserver">
	// This function restart the server on which this class runs. 
	// It verifies the following:
	// 	1. IP address
	// 	2. Server Name
	aos_assert_rr(cmd, rdata, false);
	OmnString name = cmd->getAttrStr("name", "");
	OmnString addr = cmd->getAttrStr("addr", "");
	int port = cmd->getAttrInt("port", -1);
	bool rslt = AosNetworkMgr::getSelf()->verifyServer(name, addr, port, rdata);
	if (!rslt)
	{
		//AosNetworkMgr::getSelf()->sendErrorResponse(cmd, "", __FILE__, __LINE__, rdata);
		return true;
	}
	
	AosNetworkMgr::getSelf()->sendResponse("restartserver_finished", 
			cmd, "Restarting the server", rdata);
	OmnScreen << "Restarting the server ...." << endl;
	//OmnString cmd = "reboot";
	//system(cmd.data());
	return true;
}
#endif
