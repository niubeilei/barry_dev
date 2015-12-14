////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecureAuthCmd.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////



#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include <linux/module.h>


int sgAuthCmdSwitch;

int AosSecure_AuthCmdSetCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system busyloop <duration-in-sec> [<cpu-usage>]
	//
	char * tmp = parms->mStrings[0];
	int	ret=0;
	*length = 0;

    if (strcmp(tmp, "on") == 0)
	{
		aos_trace(KERN_INFO "to set authcmd status on \n");
		sgAuthCmdSwitch = 1;
	}
	else if (strcmp(tmp, "off") == 0)
	{
		aos_trace(KERN_INFO "to set authcmd status off \n");
		sgAuthCmdSwitch = 0;
	}
	else
	{
		sprintf(errmsg, "Invalid parameter! <on|off>");
		ret = -1;
	}
	
	return ret;
}

int AosSecure_AuthCmdShowCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{

	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);	// Retrieve the beginning of the return buffer
	unsigned int rsltIndex = 0;						// Return buffer index;
	unsigned int optlen = *length;
	if (sgAuthCmdSwitch)
	{
		sprintf(local, "Secure Auth Cmd Information\n---------------------------\n\t Status: on\n\n");
	}
	else
	{
		sprintf(local, "Secure Auth Cmd Information\n---------------------------\n\t Status: off\n\n");
	}
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;

	return 0;
}


static int AuthCmd_registerCli(void)
{
	int ret=0;
	
	ret |= OmnKernelApi_addCliCmd("secure_authcmd_set", AosSecure_AuthCmdSetCli);	
	ret |= OmnKernelApi_addCliCmd("secure_authcmd_show", AosSecure_AuthCmdShowCli);	

	return ret;
}

static int AuthCmd_unregisterCli(void)
{
	OmnKernelApi_delCliCmd("secure_authcmd_set");
	OmnKernelApi_delCliCmd("secure_authcmd_show");

	return 0;
}


static __init int 
AuthCmd_initModule(void)
{
	//init
	sgAuthCmdSwitch = 0;
	AuthCmd_registerCli();
	return 0;
}

static __exit void 
AuthCmd_exitModule(void)
{
	AuthCmd_unregisterCli();
}

module_init(AuthCmd_initModule);
module_exit(AuthCmd_exitModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kevin");


