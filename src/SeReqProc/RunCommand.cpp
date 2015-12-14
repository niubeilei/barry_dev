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
// The torturer is in SengTorturer/TesterRunCommandNew.cpp
//   
//
// Modification History:
// 05/18/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/RunCommand.h"

#include "SEInterfaces/CommandRunnerObj.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"


AosRunCommand::AosRunCommand(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_RUNCOMMAND, AosSeReqid::eRunCommand, rflag)
{
}


bool 
AosRunCommand::proc(const AosRundataPtr &rdata)
{
	return AosCommandRunnerObj::runCommand(rdata);
}

