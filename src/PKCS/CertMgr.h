////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PKCS_CertMgr_h
#define Omn_PKCS_CertMgr_h

#include "aosUtil/Types.h"
#include "PKCS/x509.h"
#include "aosUtil/List.h"
#include "PKCS/PKCSPriv.h"

/*
enum
{
	eAosCertMgr_MaxChains = 128,
	eAosCertMgr_ChainSize = 10,
	eAosCertMgr_NameLen = 16,

};

typedef struct AosCertChain
{
	char				name[eAosCertMgr_NameLen+1];
	X509_CERT_INFO *	chain[eAosCertMgr_ChainSize];
} AosCertChain_t;


typedef struct AosCertChains
{
	AosCertChain_t	*chains[eAosCertMgr_MaxChains];	
	u32				 num_chains;
} AosCertChains_t;
*/


extern X509_CERT_INFO * AosCertMgr_getCertByName(const char *name);
extern int AosCertMgr_decode(char *certDER, 
			const u16 certLen,
			X509_CERT_INFO **certDecoded);
extern int AosCertMgr_decodeBase64(char *cert_base64,
                            X509_CERT_INFO **cert_decoded,
                            char *errmsg);
struct aosKernelApiParms;


extern int AosCertMgr_init(void);
extern int AosCertMgr_addCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_delCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_showCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_clearAllCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_saveConfCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_importCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_importPrivKeyCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_importFPrivKeyCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosCertMgr_importFCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosCertMgr_setCertTypeCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern X509_CERT_INFO * AosCertMgr_getOcspCert(const u32 vs_id);
extern int AosCertMgr_setOcspCert(char *cert_der, int cert_der_len);
extern X509_CERT_INFO * AosCertMgr_getCrlCert(const u32 vs_id);
extern X509_CERT_INFO * AosCertMgr_getSystemCert(void);
extern struct AosRsaPrivateKey * AosCertMgr_getSystemPrivKey(void);

extern int AosCertMgr_importSystemCertDer(char *name, char *cert_der, u32 der_len);
extern int AosCertMgr_importDbCertDer(char *name, char *cert_der, u32 der_len);

#endif

