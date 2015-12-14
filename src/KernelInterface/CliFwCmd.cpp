////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliFwCmd.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/CliCmd.h"

#include "Alarm/Alarm.h"
#include "CliClient/ModuleCliServer.h"
#include "KernelInterface/ReturnCode.h"
#include "KernelInterface/Cli.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/Ptrs.h"
#include "Util/StrParser.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlParser/XmlItem.h"


//kevin 07/04/06
bool
OmnCliCmd::saveFwConfig(const OmnString &parms, OmnString &rslt)
{
	OmnStrParser parser(parms);
	int intv;
	bool stop;

	mParms.reset();
	OmnString fn;
	while (mParms.hasMore())
	{
		OmnCliParmPtr parm = mParms.next();
		if (!parm->parseArg(parser, intv, fn, stop, rslt))
		{
			return false;
		}

		if (stop)
		{
			// No name provided. User the default config name.
			break;
		}

		if (parm->isInteger())
		{
			rslt = "Command Incorrect";
			return false;
		}

		break;
	}
							
	AosCli cli(fn);
	int ret = cli.saveOnlyFwConfig(rslt);
	return ret == 0;	
}


//kevin 07/04/06
bool
OmnCliCmd::loadFwConfig(const OmnString &parms, OmnString &rslt)
{
	OmnStrParser parser(parms);
	int intv;
	bool stop;

	mParms.reset();
	OmnString fn;
	while (mParms.hasMore())
	{
		OmnCliParmPtr parm = mParms.next();
		if (!parm->parseArg(parser, intv, fn, stop, rslt))
		{
			return false;
		}

		if (stop)
		{
			// No name provided. User the default config name.
			break;
		}

		if (parm->isInteger())
		{
			rslt = "Command Incorrect";
			return false;
		}

		break;
	}
							
	AosCli cli(fn);
	int ret = cli.loadFwConfig(rslt);
	return ret == 0;	
}


//kevin 07/04/06
bool
OmnCliCmd::fwRules(const OmnString &parms, OmnString &rslt)
{
/*	OmnStrParser parser(parms);
	int intv;
	bool stop;

	mParms.reset();
	OmnString fn;
	while (mParms.hasMore())
	{
		OmnCliParmPtr parm = mParms.next();
		if (!parm->parseArg(parser, intv, fn, stop, rslt))
		{
			return false;
		}

		if (stop)
		{
			// No name provided. User the default config name.
			break;
		}

		if (parm->isInteger())
		{
			rslt = "Command Incorrect";
			return false;
		}

		break;
	}
*/							
	AosCli cli(parms);
	int ret = cli.fwRules(rslt);
	return ret == 0;	
}


//kevin 07/10/06
bool
OmnCliCmd::fwIpfilAll(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilAll(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilTcp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilTcp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilUdp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilUdp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilIcmp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilIcmp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilStateNew(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilStateNew(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilStateAck(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilStateAck(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilAll(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilAll(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilTcp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilTcp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilUdp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilUdp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilIcmp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilIcmp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatSnat(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatSnat(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDnatPort(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDnatPort(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDnatIP(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDnatIP(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatMasq(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatMasq(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatRedi(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatRedi(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilDelAll(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilDelAll(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilDelTcp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilDelTcp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilDelUdp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilDelUdp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilDelIcmp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilDelIcmp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilDelStateNew(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilDelStateNew(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwIpfilDelStateAck(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwIpfilDelStateAck(rslt);
	return ret == 0;	
}




bool
OmnCliCmd::fwMacfilDelAll(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilDelAll(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilDelTcp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilDelTcp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilDelUdp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilDelUdp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwMacfilDelIcmp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwMacfilDelIcmp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDelSnat(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDelSnat(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDelDnatPort(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDelDnatPort(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDelDnatIP(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDelDnatIP(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDelMasq(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDelMasq(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwNatDelRedi(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwNatDelRedi(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwAntiSyn(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwAntiSyn(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwAntiIcmp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwAntiIcmp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwAntiDelSyn(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwAntiDelSyn(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwAntiDelIcmp(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwAntiDelIcmp(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwBlacklistLoadFile(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwBlacklistLoadFile(rslt);
	return ret == 0;	
}

	
bool
OmnCliCmd::fwBlacklistLoadUrl(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwBlacklistLoadUrl(rslt);
	return ret == 0;	
}


//BinGong 11/14/06
bool
OmnCliCmd::fwBlacklistRemoveFile(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwBlacklistRemoveFile(rslt);
	return ret == 0;	
}


//BinGong 11/14/06
bool
OmnCliCmd::fwBlacklistRemoveUrl(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwBlacklistRemoveUrl(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwTimefil(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwTimefil(rslt);
	return ret == 0;	
}


bool
OmnCliCmd::fwDelTimefil(const OmnString &parms, OmnString &rslt)
{
	AosCli cli(parms);
	int ret = cli.fwDelTimefil(rslt);
	return ret == 0;	
}
#endif

