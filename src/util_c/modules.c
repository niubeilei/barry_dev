////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Modules.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "util_c/modules.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/PlatformRc.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/string.h"

static int sgAosModuleInitFlag = 0;
char sgAosModuleNames[eAosModule_End+1][eAosModuleNameMaxLen];


// 
// It initializes the module names. Module names are declared in the 
// array 'sgAosModuleNames[eAosModule_End]'. This function
// simply goes over all the modules and uses the 'strcpy' to copy
// the module names to the corresponding elements.
//
// IMPORTANT: There is a maximum length for alarm module names. When 
// adding a new module name, MAKE SURE THE NAME IS NO LONGER THAN 
// THE MAXIMUM LENGTH.
//
// The function should never fail.
//
static void AosModule_initNames(void)
{
	strcpy(sgAosModuleNames[eAosMD_Platform], "Platform");
	strcpy(sgAosModuleNames[eAosMD_AosUtil], "AosUtil");
	strcpy(sgAosModuleNames[eAosMD_CLI], "CLI");
	strcpy(sgAosModuleNames[eAosMD_DenyPage], "DenyPage");
	strcpy(sgAosModuleNames[eAosMD_TcpProxy], "TcpProxy");
	strcpy(sgAosModuleNames[eAosMD_CertVerify], "CertVerify");
	strcpy(sgAosModuleNames[eAosMD_AMM], "AMM");
	strcpy(sgAosModuleNames[eAosMD_SSL], "SSL");
	strcpy(sgAosModuleNames[eAosMD_AppProc], "AppProc");
	strcpy(sgAosModuleNames[eAosMD_Tracer], "Tracer");
	strcpy(sgAosModuleNames[eAosMD_PKCS], "PKCS");
	strcpy(sgAosModuleNames[eAosMD_UtilComm], "UtilComm");
	strcpy(sgAosModuleNames[eAosMD_QoS], "QoS");
	strcpy(sgAosModuleNames[eAosMD_IAC], "IAC");
}


int AosModule_init(void)
{
	int i;

	if (sgAosModuleInitFlag) return 0;

	for (i=0; i<eAosModule_End; i++)
	{
		strcpy(sgAosModuleNames[i], "NotDefined");
	}

	AosModule_initNames();

	sgAosModuleInitFlag = 1;
	return 0;
};


int AosModule_getId(char *modulename)
{
	int i;

	if (!sgAosModuleInitFlag) AosModule_init();

	for (i=0; i<eAosModule_End; i++)
	{
		if (strcmp(sgAosModuleNames[i], modulename) == 0)
		{
			return i;
		}
	}

	return -eAosRc_UnrecognizedModuleName;
}


char *
aos_get_module_name(int code)
{
	if (!sgAosModuleInitFlag) AosModule_init();

	if (code <= eAosModule_Start || code >= eAosModule_End)
	{
		code = eAosModule_Start;
	}

	return sgAosModuleNames[code];
}


int AosModule_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
	// module show
	//
	int i;
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char local[200];

    *length = 0;
	if (sgAosModuleInitFlag == 0) AosModule_init();

    sprintf(local, "--------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	for (i=eAosModule_Start+1; i<eAosModule_End; i++)
	{
		sprintf(local, "%s\n", sgAosModuleNames[i]);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

    sprintf(local, "--------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}

