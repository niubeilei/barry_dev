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
#ifndef Aos_CommandProc_CmdProcRestartServer_h
#define Aos_CommandProc_CmdProcRestartServer_h

#include "CommandProc/CommandProc.h"

class AosCmdProcRestartServer : public AosCommandProc
{
public:
	AosCmdProcRestartServer(const bool flag);
	~AosCmdProcRestartServer(){}

	virtual bool procCommand(
					const AosXmlTagPtr &cmd, 
					const AosRundataPtr &rdata);
};
#endif
#endif
