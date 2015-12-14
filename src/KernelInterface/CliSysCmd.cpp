////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliSysCmd.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/CliSysCmd.h"

#include <sys/wait.h>
#include "Alarm/Alarm.h"
#include "KernelInterface/ReturnCode.h"
// #include "KernelInterface/Cli.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/Ptrs.h"
#include "Util/StrParser.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"

OmnCliSysCmd::OmnCliSysCmd(OmnString prefix)
{
	mPrefix = prefix;
	mUsage = "Call system() directly";
	mBufsize = eSysCmdBuffSize;
	mOprId = "excute_syscmd";
	mModId = "syscmd";
		
	return;
}


bool
OmnCliSysCmd::checkDef(OmnString &rslt)
{
	if (mBufsize < 0)
	{
		rslt = "Bufsize is negative: ";
		rslt << mBufsize;
		return false;
	}
	return true;
}


bool 
OmnCliSysCmd::run(const OmnString &cmd,const int parmPos, OmnString &rslt,const AosModuleCliServerPtr server)
{
	if(cmd.length() < parmPos)
	{
		return false;
	}

	OmnString parms(cmd.getBuffer()+ parmPos , cmd.length() - parmPos);
	
	parms.getBuffer()[parms.length()] = 0;
/*	int stat = system(parms);
	if(0 == WIFEXITED(stat))
	{
		rslt = "command abnormal";
		return false;
	}
	if(WIFSTOPPED(stat))
	{
		rslt = "process stopped";
		return false;
	}
	if(WCOREDUMP(stat))
	{
		rslt = "core dump";
		return false;
	}
	rslt = "command excuted";
*/

	doShell(parms, rslt);
	return true;
}


int 
OmnCliSysCmd::doShell(const OmnString &cmd,OmnString &msg)
{
    FILE *fp;
    char line[2048];

    fp=popen(cmd.getBuffer(),"r");
    while(fgets(line,2048,fp))
    {
        msg << line;
    }
    pclose(fp);
    return 0;
}

bool
OmnCliSysCmd::saveConfig(const OmnString &parms, OmnString &rslt)
{
	return false;	
}


bool
OmnCliSysCmd::loadConfig(const OmnString &parms, OmnString &rslt)
{
	return false;	
}


bool		
OmnCliSysCmd::runKernel(const OmnString &parms, OmnString &rslt)
{
	return false;	
}




