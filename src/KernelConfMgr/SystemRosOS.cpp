////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemQuotaMgr.cpp
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
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
//#include "XmlParser/XmlItem.h"

#include "rhcUtil/aosResMgrToolkit.h"

int rosOsSystemReboot(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;

	OmnCliSysCmd::doShell("reboot -f", rslt);

	*optlen = index;
	
	return 0;
}

int rosOsSystemShutdown(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;

	OmnCliSysCmd::doShell("shutdown -h now", rslt);

	*optlen = index;
	
	return 0;
}

int RosOS_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("system reboot", rosOsSystemReboot);
	ret |= CliUtil_regCliCmd("system shutdown", rosOsSystemShutdown);
	
	return ret;
}



