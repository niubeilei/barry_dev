////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ssl.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl/Ssl.h"

#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"

#include "aosUtil/Debug.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Slab.h"

#include "KernelSimu/string.h"
#include "KernelSimu/module.h"

#include "ssl/ReturnCode.h"
#include "ssl/SslSessionMgr.h"
#include "ssl/SslStatemachine.h"
#include "ssl/SslMisc.h"
#include "ssl/SslCommon.h"
#include "ssl/SslCipher.h"
#include "ssl/cavium.h"
#include "ssl/SslWrapper.h"

#include "PKCS/x509.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"


#include "JNS9054Card/lib_9054jmk.h"
// #include "Proxy/wrapper_ssl.h"

#ifdef __KERNEL__
#include "Ktcpvs/tcp_vs.h"
#endif

extern u32 sgServerCiphersSupported[eAosMaxCipher];
extern  u8 sgNumServerCiphersSupported;
extern u32 AosClientCiphersSupported[eAosMaxCipher];
extern  u8 AosNumClientCiphersSupported;
extern AosCipherNameMap_t aos_cipher_name_map[];

int aos_ssl_init_flag = 0;
int AosSsl_ClientSessionReuse = 1;
int AosSsl_ServerSessionReuse = 1;
int AosSsl_ClientAuth = 0;

ContextType gAosCaviumContextType = CONTEXT_SSL;
KeyMaterialLocation gAosCaviumKeyMaterialLocation = HOST_MEM;
AosSslAcceleratorType gAosSslAcceleratorType = eAosSslAcceleratorType_Soft;
n1_request_type gAosSslCaviumRequestType = CAVIUM_NON_BLOCKING;
struct AosSlab *gAosSlabContext = NULL;
struct AosSlab *gAosSlabRecord = NULL;

int AosSsl_Init(void)
{
	int ret;
	aos_enter_func(eFunc_AosSsl_Init);

	if ( aos_ssl_init_flag )
	{
		aos_min_log(eAosMD_SSL, "ssl already inited");
		return eAosRc_Success;
	}

	ret = AosSsl_SessionMgrInit();
	if ( eAosRc_Success !=  ret )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl session manager init failed");
		goto INIT_FAILED;
	}

	ret = AosSsl_cipherFunInit();
	if ( eAosRc_Success != ret )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl cipher function init failed");
		goto INIT_FAILED;
	}

	// init ssl context memory pool
	ret = AosSlab_constructor("SSL Context", 
							 sizeof(struct AosSslContext), 
							 SSL_CONTEXT_MAX_COUNTS, 
							 &gAosSlabContext);
	if (eAosRc_Success != ret || !gAosSlabContext)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl context slab create failed");
		goto INIT_FAILED;
	}
	
	// init ssl record memory pool
	ret = AosSlab_constructor("SSL Record", 
							 sizeof(struct AosSslRecord), 
							 SSL_RECORD_MAX_COUNTS, 
							 &gAosSlabRecord);
	if (eAosRc_Success != ret  || !gAosSlabRecord)
	{
		
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl record slab create failed");
		goto INIT_FAILED;
	}

	aos_ssl_init_flag = 1;

	return eAosRc_Success;
INIT_FAILED:
	AosSsl_Exit();
	
	return ret;
}

int AosSsl_Exit(void)
{
	// TBD:
	aos_trace("aos ssl module exit");
	return eAosRc_Success;
}

// Description:
// 	1.start ssl state machine, start front ssl or backend ssl
//	2.create ssl context for each ssl session
// Parameters:
//	Input:
//		svc: the virtual service
//		conn: the conn of this proxy
//		front: front ssl or backend ssl flags, eAosSSLFlag_Server 
//		stands for server, else stands for client
// Return:
//	0: successful
//   <0: failed
//
int AosSsl_Start(struct AosSslWrapperObject *swo, int server)
{
	int ret;
	aosSslSession_t *session = 0;
	
	aos_enter_func(eFunc_AosSsl_Start);

//	aos_trace("aos start ssl %s", (server & eAosSSLFlag_Server)?"server":"client");
	
	// start front ssl server
	if (server & eAosSSLFlag_Server)
	{
		ret = AosSslStm_startServer(
				swo, 
				swo->ssl_flags, 
				(int) swo->sk);
		return ret;
	}

	// start backend ssl client
	// Check whether there is a session to use
	//
	if ( swo->dest_addr && AosSsl_ClientSessionReuse )
	{
		session = AosSsl_getClientSession( swo->dest_addr );
	}

	if (session)
	{
		ret = AosSslStm_startClient(
			swo, 
			swo->ssl_flags, 
			0, 
			(char *)session->sessionid, 
			eAosSSL_SessionIdLen, 
			(char *)session->master_secret,
			MAX_RSA_MODULUS_LEN,
			(char*)session->client_random,
			(char*)session->server_random,
			(int)swo->sk);
	}
	else
	{
		ret = AosSslStm_startClient(
			swo, 
			swo->ssl_flags, 
			0, 
			0, 
			0, 
			0,	// session->master_secret
			0,	// master_secret_len
			0,	// session->client_random
			0,	// session->server_random
			(int)swo->sk);
	}

	if ( swo->context )
	{
		swo->context->flags = swo->ssl_flags;
	}
	return ret;
}

static int AosSsl_showCiphers(
	char* rsltBuff, 
	unsigned int *rsltIndex, 
	int length,
	char *errmsg,
    const int errlen)
{
    //
    // ssl show ciphers 
    //
    unsigned int optlen = length;
    char local[200];
	int i;

	aos_enter_func(eFunc_AosSsl_showCiphersCli);

	sprintf(local, "\nCipher Selected:");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "\n-----------------------------------");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "\nClient Ciphersuites Supported");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	if (AosNumClientCiphersSupported == 0)
	{
		sprintf(local, "No Client cipher suites defined");
    	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		for (i=0; i<AosNumClientCiphersSupported; i++)
		{
			sprintf(local, "\n    %s", AosSsl_getCipherName(
				AosClientCiphersSupported[i]));
    		aosCheckAndCopy(rsltBuff, rsltIndex, optlen, 
				local, strlen(local));
		}
	}

	sprintf(local, "\n\nServer Ciphersuites Supported");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	if (sgNumServerCiphersSupported == 0)
	{
		sprintf(local, "No Server cipher suites defined");
    	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		for (i=0; i<sgNumServerCiphersSupported; i++)
		{
			sprintf(local, "\n    %s", AosSsl_getCipherName(
				sgServerCiphersSupported[i]));
    		aosCheckAndCopy(rsltBuff, rsltIndex, optlen, 
				local, strlen(local));
		}
	}

	sprintf(local, "\n-----------------------------------\n");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
		
	return 0;
}

static int AosSsl_setCiphers(
			u16 num_ciphers,
            struct aosKernelApiParms *parms,
			u32 *target_ciphers,
			u8 *n_ciphers, 
			char *errmsg)
{
	u16 cipher_idx = 0;
	char *cipher_name;
	int index = 1;
	u32 ciphers[eAosMaxCipher], cipher;
	int ret;

	aos_enter_func(eFunc_AosSsl_setCiphers);
	while (num_ciphers-- && cipher_idx < eAosMaxCipher)
	{
		cipher_name = parms->mStrings[index++];
		if (!cipher_name)
		{
			sprintf(errmsg, "Missing cipher name: %d", index-1);
			return -eAosRc_SslCliErr;
		}
	
		ret = AosSsl_getCipher(cipher_name, &cipher);
		if (ret)
		{
			sprintf(errmsg, "Unrecognized cipher: %s", cipher_name);
			return -eAosRc_SslCliErr;
		}

		if (!AosSsl_isCipherSupported(cipher))	
		{
			sprintf(errmsg, "Cipher %s is not supported", cipher_name);
			return -eAosRc_SslCliErr;
		}

		ciphers[cipher_idx++] = cipher;
	}

	memcpy(target_ciphers, ciphers, cipher_idx*4);
	*n_ciphers = cipher_idx;
	return 0;
}


int AosSsl_setCiphersCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
    //
    // ssl client set ciphers <name> [<cipher-suites>]*
    //
    char *name = parms->mStrings[0];
	unsigned int num_ciphers = parms->mNumStrings - 1;
	aos_enter_func(eFunc_AosSsl_setCiphersCli);

	*length = 0;

	if (num_ciphers < 1)
	{
		sprintf(errmsg, "String argument mismatch. Expect at least 2, "
			"but got: %d", parms->mNumStrings);
		return -eAosRc_SslCliErr;
	}

	if (num_ciphers >= eAosMaxCipher)
	{
		sprintf(errmsg, "Too many ciphers: %u. Maximum allowed: %d", 
			num_ciphers, eAosMaxCipher);
		return -eAosRc_SslCliErr;
	}

	AosKAPI_checkStr(name, "Missing client name", 
		-eAosRc_SslCliErr, errmsg);

	if (strcmp(name, "client") == 0)
	{
		return AosSsl_setCiphers(num_ciphers, parms, 
			AosClientCiphersSupported,
			&AosNumClientCiphersSupported, 
			errmsg);
	}

	if (strcmp(name, "server") == 0)
	{
		return AosSsl_setCiphers(num_ciphers, parms, 
			sgServerCiphersSupported,
			&sgNumServerCiphersSupported, 
			errmsg);
		return 0;
	}

	sprintf(errmsg, "Client not found: %s", name);
	return -eAosRc_SslCliErr;
}

int AosSsl_showCiphersCli(
	char *data,
    unsigned int *length,
   	struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
    //
    // ssl show ciphers 
    //
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
	int ret;
	
	ret = AosSsl_showCiphers(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
		
	*length = rsltIndex;
	return ret;
}

int AosSsl_showSupportedCiphersCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{   
    //
    // ssl show supported ciphers 
    //
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
    int ret;
    
    ret = AosSsl_showSupportedCiphers(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
        
    *length = rsltIndex;
    return ret;
}

int AosSsl_clearConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
	// 
	// Reset the ciphers to their default values
	//
	*length=0;
	AosClientCiphersSupported[0] = TLS_RSA_WITH_RC4_128_MD5;
	AosClientCiphersSupported[1] = TLS_RSA_WITH_RC4_128_SHA;
	AosClientCiphersSupported[2] = TLS_RSA_WITH_DES_CBC_SHA,
	AosClientCiphersSupported[3] = JNSTLS_RSA_WITH_NULL_MD5;
	AosClientCiphersSupported[4] = JNSTLS_RSA_WITH_NULL_SHA;
	AosNumClientCiphersSupported = 5;

	sgServerCiphersSupported[0] = TLS_RSA_WITH_RC4_128_MD5;
	sgServerCiphersSupported[1] = TLS_RSA_WITH_RC4_128_SHA;
	sgServerCiphersSupported[2] = TLS_RSA_WITH_DES_CBC_SHA;
	sgServerCiphersSupported[3] = JNSTLS_RSA_WITH_NULL_MD5;
	sgServerCiphersSupported[4] = JNSTLS_RSA_WITH_NULL_SHA;

	sgNumServerCiphersSupported = 5;

	
	session_check_interval = 3600;    // 3600 minutes = one hour each time
	session_resume_life = 24*3600;    // 24*3600 minutes = 24 hours default

	AosSsl_ClientSessionReuse = 1;
	AosSsl_ServerSessionReuse = 1;

	gAosSslAcceleratorType = eAosSslAcceleratorType_Soft;
	//aos_ssl_hardware_flag = 0;
	return 0;
}


int AosSsl_saveConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
    //
    // ssl client save config
    //
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
 	char *local = (char *)aos_malloc_atomic(eAosSsl_MaxSaveCmdLen);
	unsigned int len;
	int i, ret;

	// <Cmd>ssl client set ciphers system [<cipher>]*
	sprintf(local, "<Cmd>ssl set ciphers client ");

	for (i=0; i< AosNumClientCiphersSupported; i++)
	{
		len = strlen(local);
		if (len + eAosCipherNameLen >= eAosSsl_MaxSaveCmdLen)
		{
			sprintf(errmsg, "Too many ciphers: %u", len);
			aos_free(local);
			return -eAosRc_SslCliErr;
		}

		sprintf(local + len, "%s ", 
			AosSsl_getCipherName(AosClientCiphersSupported[i]));
	}
	len = strlen(local);
	sprintf(local+len, "</Cmd>\n");
    ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "<Cmd>ssl set ciphers server ");
	for (i=0; i < sgNumServerCiphersSupported; i++)
	{
		len = strlen(local);
		if (len + eAosCipherNameLen >= eAosSsl_MaxSaveCmdLen)
		{
			sprintf(errmsg, "Too many ciphers: %u", len);
			return -eAosRc_SslCliErr;
		}

		sprintf(local + len, "%s ", 
			AosSsl_getCipherName(sgServerCiphersSupported[i]));
	}
	len = strlen(local);
	sprintf(local+len, "</Cmd>\n");
    ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	switch(gAosSslAcceleratorType)
	{
		case eAosSslAcceleratorType_517PM:
			sprintf(local, "<Cmd>ssl accelerator type 517PM</Cmd>\n");
			break;
		case eAosSslAcceleratorType_CaviumGen:
			sprintf(local, "<Cmd>ssl accelerator type caviumgen</Cmd>\n");
			break;
		case eAosSslAcceleratorType_CaviumPro:
			sprintf(local, "<Cmd>ssl accelerator type caviumpro</Cmd>\n");
			break;
		case eAosSslAcceleratorType_Soft:
		default:
			sprintf(local, "<Cmd>ssl accelerator type soft</Cmd>\n");
			break;
	}
 	
    ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

 	sprintf(local, "<Cmd>ssl session timer %d</Cmd>\n", (int)session_check_interval);
    ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
 	
	sprintf(local, "<Cmd>ssl session life %d</Cmd>\n", (int)session_resume_life);
    ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	if (AosSsl_ClientSessionReuse == 0)
	{
		sprintf(local, "<Cmd>ssl session reuse client off</Cmd>\n");
    	ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	if (AosSsl_ServerSessionReuse == 0)
	{
		sprintf(local, "<Cmd>ssl session reuse server off</Cmd>\n");
    	ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	aos_free(local);
	*length = rsltIndex;
	return ret;
}

int AosSsl_hardwareAcceleratorCli(
	char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
	char *type = parms->mStrings[0];

#ifdef __KERNEL__
	unsigned char *cert_der = NULL;
	int der_len = 0; 
	int ret = 0;
#endif

	*length = 0;

	if (strcmp(type, "soft") == 0)
	{
		aos_debug("ssl accelerator software on");
		gAosSslAcceleratorType = eAosSslAcceleratorType_Soft;
		AosSsl_cipherFunSet(gAosSslAcceleratorType);	
		return eAosRc_Success;
	}
	
#ifdef __KERNEL__

	if (strcmp(type, "caviumgen") == 0)
	{
		aos_debug("ssl accelerator cavium general on");
		gAosSslAcceleratorType = eAosSslAcceleratorType_CaviumGen;
		AosSsl_cipherFunSet(gAosSslAcceleratorType);	
		return eAosRc_Success;
	}

	if (strcmp(type, "caviumpro") == 0)
	{
		aos_debug("ssl accelerator cavium protocol on");
		gAosSslAcceleratorType = eAosSslAcceleratorType_CaviumPro;
		AosSsl_cipherFunSet(gAosSslAcceleratorType);	
		return eAosRc_Success;
	}

	if (strcmp(type, "caviumgen") != 0)
	{
		sprintf(errmsg, "unknown accelerator type\n");
		return -1;
	}

#ifdef CONFIG_JNS_JMK
	aos_debug("ssl accelerator  517 PM on");

	AosJNSJMK_getServerCertDer(&cert_der, &der_len);
	aos_assert1(cert_der);
	// save server certificate 
	ret = AosCertMgr_importSystemCertDer("system", cert_der, der_len);    
	if (ret)    
	{        
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "save cert from ic card into system cert db failed");
		sprintf(errmsg, "save cert from ic card into system cert db failed\n");
		return -1;    
	}    
	// get CA2 from IC CARD
	AosJNSJMK_getCA2CertDer(&cert_der, &der_len);
	if (der_len>0)
	{
		ret = AosCertMgr_importDbCertDer("JNS_CA2", cert_der, der_len);
		if (ret)    
		{        
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
						"save CA2 cert from ic card into system cert db failed");
			sprintf(errmsg, "save CA2 cert from ic card into system cert db failed\n");
			return -1;    
		}    
	}
	// get CA1 from IC CARD
	AosJNSJMK_getCA1CertDer(&cert_der, &der_len);
	if (der_len>0)
	{
		ret = AosCertMgr_importDbCertDer("JNS_CA1", cert_der, der_len);
		if (ret)    
		{        
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
						"save CA1 cert from ic card into system cert db failed");
			sprintf(errmsg, "save CA1 cert from ic card into system cert db failed\n");
			return -1;    
		}    
	}
	// get CA0 from IC CARD
	AosJNSJMK_getCA0CertDer(&cert_der, &der_len);
	if (der_len>0)
	{
		ret = AosCertMgr_importDbCertDer("JNS_CA0", cert_der, der_len);
		if (ret)    
		{        
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
						"save CA0 cert from ic card into system cert db failed");
			sprintf(errmsg, "save CA0 cert from ic card into system cert db failed\n");
			return -1;    
		}    
	}
	// get OCSP Cert from IC CARD
	AosJNSJMK_getOCSPCertDer(&cert_der, &der_len);
	if (der_len>0)
	{
		ret = AosCertMgr_importDbCertDer("JNS_OCSP", cert_der, der_len);
		if (ret)    
		{        
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
						"save OCSP cert from ic card into system cert db failed");
			sprintf(errmsg, "save OCSP cert from ic card into system cert db failed\n");
			return -1;    
		}    
	}
#endif

	AosCertChain_del("system");
	if(AosCertChain_add("system", "JNS_CA2"))
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "add CA2 cert into system chain failed");
		sprintf(errmsg, "add CA2 cert into system chain failed\n");
		return -1;    
	}
	if(AosCertChain_add("system", "JNS_CA1"))
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "add CA1 cert into system chain failed");
		sprintf(errmsg, "add CA1 cert into system chain failed\n");
		return -1;    
	}
	if(AosCertChain_add("system", "JNS_CA0"))
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "add CA0 cert into system chain failed");
		sprintf(errmsg, "add CA0 cert into system chain failed\n");
		return -1;    
	}
	if(AosCertChain_add("system", "JNS_OCSP"))
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "add OCSP cert into system chain failed");
		sprintf(errmsg, "add OCSP cert into system chain failed\n");
		return -1;    
	}



	//aos_ssl_hardware_flag = 1;
	gAosSslAcceleratorType = eAosSslAcceleratorType_517PM;


	// register encryption function 
	AosSsl_cipherFunSet(gAosSslAcceleratorType);	
		
#endif
	
	return eAosRc_Success;

}

int AosSsl_sessionTimerCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
    int interval = parms->mIntegers[0];

    *length = 0;
	
	// we use minutes unit for interval
	if (interval < 60 )
	{
		sprintf(errmsg, "resumed session expired checking interval should be large than 60 seconds\n");
		return -1;
	}

	AosSsl_SessionSetTimer(interval);

	return eAosRc_Success;
}

int AosSsl_sessionLifeCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
    int life = parms->mIntegers[0];

    *length = 0;

    // we use minutes unit for interval
    if (life < 60 )
    {
        sprintf(errmsg, "resumed session expired checking interval should be large than 60 seconds\n");
        return -1;
    }
    session_resume_life = life; // seconds is the uint

    return eAosRc_Success;
}

static int AosSsl_sessionShowConfig(
	char* rsltBuff,
    unsigned int *rsltIndex,
    int length,
    char *errmsg,
    const int errlen)
{
    unsigned int optlen = length;
 	char *local = (char *)aos_malloc_atomic(eAosSsl_MaxSaveCmdLen);

	aos_assert1(local);

	sprintf(local, "\nSession Configuration:\n");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

    sprintf(local, "ssl session resume\n"
					"\tcheck time: %d seconds\n" 
					"\t life time: %d seconds\n", 
					(int)session_check_interval,
					(int)session_resume_life);
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Client Session Reuse: %s\n", AosSsl_ClientSessionReuse?"on":"off");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Server Session Reuse: %s\n", AosSsl_ServerSessionReuse?"on":"off");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	aos_free(local);

	return eAosRc_Success;
}

int AosSsl_sessionShowCli(
	char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
	unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
 	char *local = (char *)aos_malloc_atomic(eAosSsl_MaxSaveCmdLen);
	int ret;

    *length = 0;
	aos_assert1(local);
	ret = AosSsl_sessionShowConfig(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
	aos_free(local);
	*length = rsltIndex;
	
	return ret;
}

int AosSsl_setSessionReuseCli(
			char *data,
    		unsigned int *length,
    		struct aosKernelApiParms *parms,
    		char *errmsg,
    		const int errlen)
{
	// 
	// ssl session reuse <client|server> <on|off>
	//
	char *cs = parms->mStrings[0];
	char *status = parms->mStrings[1];

	*length = 0;
	if (parms->mNumStrings != 2)
	{
		sprintf(errmsg, "Parameters mismatch. Expect two but got %d", 
			parms->mNumStrings);
		return -eAosRc_SslCliErr;
	}

	if (!cs || !status)
	{
		sprintf(errmsg, "Missing parameters");
		return -eAosRc_SslCliErr;
	}

	if (strcmp(cs, "client") == 0)
	{
		if (strcmp(status, "on") == 0)
		{
			AosSsl_ClientSessionReuse = 1;
			return 0;
		}
		
		if (strcmp(status, "off") == 0)
		{
			AosSsl_ClientSessionReuse = 0;
			return 0;
		}
		
		sprintf(errmsg, "Incorrect status: %s", status);
		return -eAosRc_SslCliErr;
	}

	if (strcmp(cs, "server") == 0)
	{
		if (strcmp(status, "on") == 0)
		{
			AosSsl_ServerSessionReuse = 1;
			return 0;
		}
		
		if (strcmp(status, "off") == 0)
		{
			AosSsl_ServerSessionReuse = 0;
			return 0;
		}
		
		sprintf(errmsg, "Incorrect status: %s", status);
		return -eAosRc_SslCliErr;
	}

	sprintf(errmsg, "Incorrect argument: %s", cs);
	return -eAosRc_SslCliErr;
}

int AosSsl_setClientAuthCli(
	char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
	// 
	// ssl clientauth <on|off>
	//
	char *status = parms->mStrings[0];

	*length = 0;

	if ( !status )
	{
		sprintf(errmsg, "Missing parameters");
		return -eAosRc_SslCliErr;
	}

	if (strcmp(status, "on") == 0)
	{
		AosSsl_ClientAuth = 1;
		return 0;
	}
		
	if (strcmp(status, "off") == 0)
	{
		AosSsl_ClientAuth = 0;
		return 0;
	}
		
	sprintf(errmsg, "Incorrect argument: %s", status);
	return -eAosRc_SslCliErr;
}

int AosSsl_showConfigCli(
            char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen)
{
	unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
	char local[200];
	int ret = 0;
	
	*length = 0;

	switch ( gAosSslAcceleratorType )
	{
		case eAosSslAcceleratorType_517PM:
			sprintf(local, "\nSSL Accelerator 517PM Card\n");
			break;
		case eAosSslAcceleratorType_CaviumGen:
			sprintf(local, "\nSSL Accelerator Cavium General API Card\n");
			break;
		case eAosSslAcceleratorType_CaviumPro:
			sprintf(local, "\nSSL Accelerator Cavium Protocol API Card\n");
			break;
		case eAosSslAcceleratorType_Soft:			
		default:
			sprintf(local, "\nSSL Accelerator Pure Soft\n");
			break;
	}
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf( local, "\nSSL Client Authentication is %s", AosSsl_ClientAuth?"On":"Off" );
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	ret = AosSsl_sessionShowConfig(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
	if (ret != 0)
	{
		return ret;
	}

	ret = AosSsl_showCiphers(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
	if (ret != 0)
	{
		return ret;
	}

	ret = AosSsl_showSupportedCiphers(rsltBuff, &rsltIndex, optlen, errmsg, errlen);
	if (ret != 0)
	{
		return ret;
	}

	*length = rsltIndex;
	return ret;
}

int AosSsl_showSupportedCiphers(
    char* rsltBuff, 
    unsigned int *rsltIndex, 
    int length,
    char *errmsg,
    const int errlen)
{
    //
    // ssl client save config
    //
    unsigned int optlen = length;
 	char local[200]; 
	int i;
	struct AosSslContext;
	u16 supported = 0;
	
	sprintf(local, "\nCipher Supported:");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "\n-----------------------------------");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	i = 0;
	while (aos_cipher_name_map[i].value != 0)
	{
		if (AosSsl_isCipherSupported(aos_cipher_name_map[i].value))
		{
			supported++;
			sprintf(local, "\n    %s", aos_cipher_name_map[i].name);
    		aosCheckAndCopy(rsltBuff, rsltIndex, optlen, 
				local, strlen(local));
		}
		i++;
	}

	if (supported == 0)			
	{
		sprintf(local, "\nNo supported ciphers");
    	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	}

	sprintf(local, "\n-----------------------------------\n");
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
		
	return 0;
}

EXPORT_SYMBOL( AosSsl_Start );
EXPORT_SYMBOL( AosSsl_ClientAuth );
EXPORT_SYMBOL( gAosSslAcceleratorType );
