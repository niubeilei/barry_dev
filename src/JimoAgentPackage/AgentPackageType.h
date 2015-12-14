////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgentPackage_AgentPackageType_h
#define Aos_JimoAgentPackage_AgentPackageType_h

#include "Util/String.h"

#define AOSTAG_AGENTPACKAGE_COMMAND 					"command"
#define AOSTAG_AGENTPACKAGE_COMMANDRESP					"command_resp"
#define AOSTAG_AGENTPACKAGE_FILETOREMOTE				"file_to_remote"
#define AOSTAG_AGENTPACKAGE_FILETOREMOTERESP			"file_to_remote_resp"
#define AOSTAG_AGENTPACKAGE_FILETOLOCAL					"file_to_local"
#define AOSTAG_AGENTPACKAGE_FILETOLOCALRESP				"file_to_local_resp"
#define AOSTAG_AGENTPACKAGE_INVALID 					"invalid"

class AosAgentPackageType
{
public:
	enum E
	{
		eInvalid,

		eCommand,
		eCommandResp,
		eFileToRemote,
		eFileToRemoteResp,
		eFileToLocal,
		eFileToLocalResp,

		eMax
	};

	static bool isValid(const E type)
	{
		return type > eInvalid && type < eMax; 
	}
	static E toEnum(const OmnString &str)
	{
		if (str == AOSTAG_AGENTPACKAGE_COMMAND) return eCommand;
		else if (str == AOSTAG_AGENTPACKAGE_COMMANDRESP) return eCommandResp;
		else if (str == AOSTAG_AGENTPACKAGE_FILETOREMOTE) return eFileToRemote;
		else if (str == AOSTAG_AGENTPACKAGE_FILETOREMOTERESP) return eFileToRemoteResp;
		else if (str == AOSTAG_AGENTPACKAGE_FILETOLOCAL) return eFileToLocal;
		else if (str == AOSTAG_AGENTPACKAGE_FILETOLOCALRESP) return eFileToLocalResp;
		else return eInvalid;
	}
	static OmnString toStr(const E type)
	{
		if (type == eCommand) return AOSTAG_AGENTPACKAGE_COMMAND;
		else if (type == eCommandResp) return AOSTAG_AGENTPACKAGE_COMMANDRESP;
		else if (type == eFileToRemote) return AOSTAG_AGENTPACKAGE_FILETOREMOTE;
		else if (type == eFileToRemoteResp) return AOSTAG_AGENTPACKAGE_FILETOREMOTERESP;
		else if (type == eFileToLocal) return AOSTAG_AGENTPACKAGE_FILETOLOCAL;
		else if (type == eFileToLocalResp) return AOSTAG_AGENTPACKAGE_FILETOLOCALRESP;
		else return AOSTAG_AGENTPACKAGE_INVALID;
	}
};
#endif

