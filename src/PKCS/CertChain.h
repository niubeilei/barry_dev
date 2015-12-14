////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertChain.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_CertChain_h
#define Omn_PKCS_CertChain_h

#include "aosUtil/Types.h"
#include "aosUtil/List.h"

struct _X509_CERT_INFO;

enum
{
    eAosCertChain_MaxMembers = 10,
	eAosCertChain_NameLen = 15,
};

typedef struct AosCertChain
{
    struct aos_list_head    *next;
    struct aos_list_head    *prev;
    char                     name[eAosCertChain_NameLen];
    struct _X509_CERT_INFO  *certs[eAosCertChain_MaxMembers];
    u8                       num_certs;
} AosCertChain_t;

struct aosKernelApiParms;
extern AosCertChain_t *AosCertChain_getByName(const char *name);
extern int AosCertChain_init(void);
extern int AosCertChain_addCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertChain_delCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertChain_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertChain_clearallCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertChain_saveconfCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertChain_add(char *name, char *cert_name);

extern int AosCertChain_del(char *chain_name);
#endif

