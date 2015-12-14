////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ssl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOS_SSL_H__
#define __AOS_SSL_H__

#include "ssl2/aosSslProc.h"

extern int aos_ssl_hardware_flag;

struct tcp_vs_conn;

enum
{
	eAosSsl_MaxSaveCmdLen = 3000
};

typedef struct AosCipherNameMap
{   
    char    name[eAosCipherNameLen];
    u32     value;
} AosCipherNameMap_t;


struct aosKernelApiParms;

extern int AosSsl_ClientSessionReuse;
extern int AosSsl_ServerSessionReuse;
extern ContextType gAosCaviumContextType;
extern KeyMaterialLocation gAosCaviumKeyMaterialLocation;
extern AosSslAcceleratorType gAosSslAcceleratorType;
extern n1_request_type gAosSslCaviumRequestType;

extern int AosSsl_Init(void);
extern int AosSsl_Start(struct tcp_vs_conn *conn, int front);
extern int AosSsl_setCiphersCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);
extern int AosSsl_showCiphersCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);
extern int AosSsl_showSupportedCiphersCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);
extern int AosSsl_setSessionReuseCli(
            char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen);
extern int AosSsl_saveConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);
extern int AosSsl_clearConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen);

extern int AosSsl_hardwareAcceleratorCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int AosSsl_sessionShowCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int AosSsl_sessionTimerCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int AosSsl_sessionLifeCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int AosSsl_showConfigCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

#endif

