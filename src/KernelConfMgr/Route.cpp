////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Cli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Global.h"

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
//#include "Util/IpAddr.h"
//#include "Util/OmnNew.h"
//#include "XmlParser/XmlItem.h"

#include <string.h>



int systemDevRoute(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnString dip;
	OmnString dmask;
	OmnString dev;
	
	dip = parms->mStrings[0];
	dmask = parms->mStrings[1];
	dev = parms->mStrings[2];

	if(dev == "incard")
	{
		dev = "br0";
	}

	else if(dev == "outcard")
	{
		dev = "eth0";
	}
	
	else
	{
		dev ="eth0";
	}
	
	OmnString systemCmd;
	systemCmd << "/sbin/route add -net " << dip <<" netmask " << dmask <<" dev " << dev;
    OmnCliSysCmd::doShell(systemCmd, rslt);

	*optlen = index;
	return 0;
}

int systemDelDevRoute(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnString dip;
	OmnString dmask;
	OmnString dev;
	dip = parms->mStrings[0];
	dmask = parms->mStrings[1];
	dev = parms->mStrings[2];

	if(dev == "incard")
	{
		dev = "br0";
	}

	else if(dev == "outcard")
	{
		dev = "eth0";
	}
	
	else
	{
		dev ="eth0";
	}
	
	OmnString systemCmd;
	systemCmd << "/sbin/route del -net " << dip <<" netmask " << dmask <<" dev " << dev;
    OmnCliSysCmd::doShell(systemCmd, rslt);

	*optlen = index;
	return 0;
}
