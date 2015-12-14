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
// 06/17/2011	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CommandProc/AllCommandProcs.h"

#include "CommandProc/CommandProc.h"
#include "CommandProc/CmdProcRestartServer.h"
#include "CommandProc/CmdProcRestartServerFinished.h"

AosCommandProcPtr			 sgCommandProcs[AosCommandType::eMax];
AosStr2U32_t 				 sgNameMap;
AosAllCommandProcs 			 sgAosAllCommandProcs;

AosAllCommandProcs::AosAllCommandProcs()
{
	static AosCmdProcRestartServer					sgCmdProcRestartServer(true);
	static AosCmdProcRestartServerFinished			sgCmdProcRestartServerFinished(true);
}
#endif
