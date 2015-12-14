////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliSysCmd.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelInterface_CliSysCmd_h
#define Omn_KernelInterface_CliSysCmd_h

#include "KernelInterface/CliCmd.h"

class OmnCliSysCmd : public OmnCliCmd
{
	enum
	{
		eSysCmdBuffSize = 100
	};

public:
	OmnCliSysCmd(OmnString prefix);
	virtual ~OmnCliSysCmd() {}

    bool    	run(const OmnString &cmd,const int parmPos, OmnString &rslt,const AosModuleCliServerPtr server);
	bool		checkDef(OmnString &rslt);
	static int 		doShell(const OmnString &cmd,OmnString &msg);
	
private:
	bool		saveConfig(const OmnString &parms, OmnString &rslt);
	bool		loadConfig(const OmnString &parms, OmnString &rslt);
	bool		runKernel(const OmnString &parms, OmnString &rslt);
};

#endif

