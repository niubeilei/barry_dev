////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Modules.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_util_modules_h
#define Omn_util_modules_h


enum
{
	eAosModuleNameMaxLen = 30
};

typedef enum AosModules
{
	eAosModule_Start = 0,

	eAosMD_Platform,
	eAosMD_AosUtil,
	eAosMD_CLI,
	eAosMD_JNSCard,
	eAosMD_DenyPage,
	eAosMD_TcpProxy,
	eAosMD_CertVerify,
	eAosMD_AMM,
	eAosMD_SSL,
	eAosMD_AppProc,
	eAosMD_Tracer,
	eAosMD_PKCS,
	eAosMD_UtilComm,
	eAosMD_WebWall,
	eAosMD_QoS,
	eAosMD_IAC,

	eAosModule_End
} aos_module_id_e;

static inline int aos_module_id_check(aos_module_id_e id)
{
	return (id > eAosModule_Start &&
			id < eAosModule_End)?1:0;
}


// extern char sgAosModuleNames[eAosModule_End][eAosModuleNameMaxLen];

struct aosKernelApiParms;

#ifdef __cplusplus
extern "C" {
#endif

extern char *aos_get_module_name(int id);
extern int   AosModule_init(void);
extern int   AosModule_getId(char *modulename);
extern int   AosModule_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
#ifdef __cplusplus
}
#endif

#endif

