////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertMgr.cpp
// Description:
//	Certificate Chain:
//		A certificate chain contains one or more certificates.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/CertMgr.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/System.h"
#include "aosUtil/Types.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Mutex.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/List.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/StringUtil.h"
#include "PKCS/x509.h"
#include "PKCS/Base64.h"
#include "PKCS/ReturnCode.h"
#include "ssl/rsa.h"


static u8 						module_init_flag = 0;

static struct aos_list_head 	cert_list;
static aos_mutex_t 				cert_list_lock;

static X509_CERT_INFO *			sg_ocsp_cert = 0;
static X509_CERT_INFO *			sg_crl_cert = 0;
static X509_CERT_INFO *			sg_sys_cert = 0;
static AosRsaPrivateKey_t	*	sg_sys_priv_key = 0;

int AosCertMgr_init(void)
{
	if (!module_init_flag)
	{
		AOS_INIT_LIST_HEAD(&cert_list);
		AOS_INIT_MUTEX(cert_list_lock);
		
//		for (i=0; i<eAosCertMgr_MaxChains; i++)
//		{
//			sg_cert_chains.chains[i] = 0;
//		}
//
//		sg_cert_chains.num_chains = 0;

		module_init_flag = 1;
	}

	return 0;
}


// 
// The function retrieves a certificate by name. If found, the 
// certificate is passed back through 'cert'. This function does
// not lock the list. The caller should lock it before calling
// this function.
//
X509_CERT_INFO * AosCertMgr_getCertByName(const char *name)
{
	struct aos_list_head *entry;
	X509_CERT_INFO *cert;

	if (sg_ocsp_cert && strcmp(sg_ocsp_cert->name, name) == 0)
	{
		AosCert_hold(sg_ocsp_cert);
		return sg_ocsp_cert;
	}

	if (sg_crl_cert && strcmp(sg_crl_cert->name, name) == 0)
	{
		AosCert_hold(sg_crl_cert);
		return sg_crl_cert;
	}

	if (sg_sys_cert && strcmp(sg_sys_cert->name, name) == 0)
	{
		AosCert_hold(sg_sys_cert);
		return sg_sys_cert;
	}

	aos_list_for_each(entry, &cert_list)
	{
		if (strcmp(((X509_CERT_INFO *)entry)->name, name) == 0)
		{
			cert = (X509_CERT_INFO*)entry;
			AosCert_hold(cert);
			return cert;
		}
	}

	return 0;
}


// 
// The function retrieves the DER of the certificate associated
// with the server 'serverId'. If no certificate is associated
// with the server, it returns 0. Otherwise, it returns the 
// certificate DER and the length. 
//
// This function does not lock the list. The caller should lock
// it before calling this function.
//
X509_CERT_INFO * AosCertMgr_getCertByDer(const char *cert_der, u16 len)
{
	X509_CERT_INFO *cert;
	struct aos_list_head *entry;

	aos_list_for_each(entry, &cert_list)
	{
		cert = (X509_CERT_INFO *)entry;
		if (!cert->cert_der)
		{
			aos_alarm(eAosMD_PKCS, eAosAlarm_ProgErr, "cert_der is null");
		}
		else if (memcmp(cert->cert_der, cert_der, len) == 0)
		{
			return cert;
		}
	}

	return 0;
}


int AosCertMgr_isCertGood(const u16 serverId, X509_CERT_INFO *cert)
{
	return 1;
}


int AosCertMgr_associateCert(const u16 serverId, 
				X509_CERT_INFO *cert, 
				char *certDER, 
				const u16 certLen)
{
	return 0;
}


// 
// The function verifies whether the certificate 'cert' is a valid 
// certificate. If yes, it stores the decoded certificate with the
// server identified by 'serverId'. This decoded certificate may
// be reused in future function calls.
//
// Parameters:
//	'serverId': IN
//		The ID of the server with which the certificate is associated.
//
//  'certDER': IN
//		The DER of a certificate.
//
//	'certLen': IN
//		The length of 'certDER'.
//
// Function Return Values:
//	0:		Successful
//	Others:	Errors
//
int AosCertMgr_decode(char *cert_der, 
					  u16 der_len, 
					  X509_CERT_INFO **cert_decoded)
{
	int ret;
	X509_CERT_INFO *cert = 0;

	*cert_decoded = 0;
	AosCertMgr_init();

	AosCert_constructor(&cert);
	aos_assert1(cert);

	cert->cert_der = (char *)aos_malloc_atomic(der_len);
	if (!cert->cert_der)
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_MemErr, 
			"Failed to allocate memory");
		AosCert_put(cert);
		return -eAosAlarm_MemErr;
	}

	memcpy(cert->cert_der, cert_der, der_len);
	cert->cert_der_len = der_len;

	ret = x509_decode_certinfo(cert->cert_der, der_len, cert);
	if (ret)
	{
		AosCert_put(cert);
		return aos_alarm(eAosMD_Platform, eAosAlarm_CertDecodeErr,
			"Failed to decode cert: %d", ret);
	}

	(*cert_decoded) = cert;

	return 0;
}


// 
// Description:
//	The function takes base64 encoded certificate and decode it
//  into a certificate as a structure. The function will allocate
//  memory for the DER of the certificate and the memory for
//  the certificate structure (X509_CERT_INFO). The caller should
//  release the certificate when finishing using it.
//
// Parameters:
//	'cert_base64': IN
//		The base64 encoded certificate
//
//  'cert_decoded': OUT
//		The decoded certificate. If failed, it is set to NULL.
//
//  'errmsg': OUT
//		If error occurs, this parameter holds the error message.
//
int AosCertMgr_decodeBase64(char *cert_base64, 
							X509_CERT_INFO **cert_decoded, 
							char *errmsg)
{
	char *cert_der = 0;
	int der_len;
	int ret;

	//
	// Decode the base64
	//
	*cert_decoded = 0;

	// 
	// "AosBase64_decode(...)" will allocate memory for cert_der. 
	// It is important to free the memory when finishing.
	//
	der_len = AosBase64_decode(cert_base64, strlen(cert_base64), &cert_der);
	if (der_len <= 0)
	{
		strcpy(errmsg, "Failed to decode the base64 of the certificate");
		return der_len;
	}

	aos_assert1(cert_der);

	// 
	// Decode the certificate
	//
	if ((ret = AosCertMgr_decode(cert_der, der_len, cert_decoded)))
	{
		aos_free(cert_der);
		sprintf(errmsg, "Failed to decode the certificate: %d", ret);
		return aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, errmsg);	
	}

	aos_free(cert_der);
	return 0;
}



static int AosCertMgr_importCert(
			char *name, 
			char *cert_base64, 
			X509_CERT_INFO **cert_created, 
			char *errmsg, 
			char add_to_list)
{
	X509_CERT_INFO *cert;
	int ret;

	if (!name)
	{
		strcpy(errmsg, "Missing name");
		return -eAosRc_CertErr;
	}

	if (strlen(name) >= eAosCertNameLen)
	{
		sprintf(errmsg, "Certificate name too long: %d. Maximum: %d", 
			strlen(name), eAosCertNameLen);
		return -eAosRc_CertErr;
	}

	if (!cert_base64)
	{
		strcpy(errmsg, "Missing certificate");
		return -eAosRc_CertErr;
	}

	aos_lock(cert_list_lock);
	if ((cert=AosCertMgr_getCertByName(name)) != NULL )
	{
		// 	
		// The certificate is already there. 
		//
		AosCert_put(cert);
		aos_unlock(cert_list_lock);
		strcpy(errmsg, 
			"Certificate name is used by another certificate");
		return -eAosRc_CertErr;
	}

	if ((ret = AosCertMgr_decodeBase64(cert_base64, cert_created, errmsg)))
	{
		aos_unlock(cert_list_lock);
		strcpy(errmsg, "Failed to decode the certificate");
		return -eAosRc_CertErr;
	}
	aos_assert1(*cert_created);

	strcpy((*cert_created)->name, name);

	if (add_to_list)
	{
		AosCert_hold(*cert_created);
		aos_list_add((struct aos_list_head *)(*cert_created), &cert_list);
	}
	aos_unlock(cert_list_lock);

	return 0;
}


int AosCertMgr_addCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // cert manager add cert <name> <certificate> 
    //
    char *name = parms->mStrings[0];
    char *cert_base64 = parms->mStrings[1];
	X509_CERT_INFO *cert;
	int ret;

	*length = 0;
	if (parms->mNumStrings != 2)
	{
		sprintf(errmsg, "Number of string parms mismatch: %d", 
			parms->mNumStrings);
		return -eAosRc_CertErr;
	}

	ret = AosCertMgr_importCert(name, cert_base64, &cert, errmsg, 1);
	if (cert) AosCert_put(cert);
	return ret;
}


/*
static int AosCertMgr_delCert(
			X509_CERT_INFO **cert, 
			char *errmsg)
{
	if (*cert)
	{
		AosCert_put(*cert);
		*cert = 0;
		return 0;
	}
	
	sprintf(errmsg, "%s Certificate not set yet", msg);
	return -eAosRc_PkcsCliErr;
}
*/
	

int AosCertMgr_delCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // cert manager remove cert <name> 
    //
    char *name = parms->mStrings[0];
	struct aos_list_head *entry;

	AosKAPI_checkNumStrArgs(parms, 1, -eAosRc_PkcsCliErr, errmsg);
	AosKAPI_checkStr(name, "Missing certificate name", -eAosRc_PkcsCliErr, errmsg);

	*length = 0;

	if (sg_ocsp_cert && strcmp(sg_ocsp_cert->name, name) == 0)
	{
		AosCert_put(sg_ocsp_cert);
		sg_ocsp_cert = 0;
		return 0;
	}
		
	if (sg_crl_cert && strcmp(sg_crl_cert->name, name) == 0)
	{
		AosCert_put(sg_crl_cert);
		sg_crl_cert = 0;
		return 0;
	}

	if (sg_sys_cert && strcmp(sg_sys_cert->name, name) == 0)
	{
		AosCert_put(sg_sys_cert);
		sg_sys_cert = 0;
		return 0;
	}
		
	aos_lock(cert_list_lock);
	aos_list_for_each(entry, &cert_list)
	{
		if (strcmp(((X509_CERT_INFO *)entry)->name, name) == 0)
		{
			AosCert_put((X509_CERT_INFO *)entry);
			aos_list_del(entry);
			aos_unlock(cert_list_lock);
			return 0;
		}
	}

	// 	
	// The certificate is not there. 
	//
	aos_unlock(cert_list_lock);
	strcpy(errmsg, "Certificate not found");
	return -eAosRc_CertErr;
}

	
int AosCertMgr_listCert(char *data, 
						unsigned int *length,
						char *errmsg)
{
	// 
	// List all the certificates.
	//
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);
	char local[200];
	struct aos_list_head *entry;
	X509_CERT_INFO *cert;
	int ret;
	uint8 sn[50];
	int num_certs = 0;
	
	*length = 0;
    sprintf(local, "Type           Name               Serial         \n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
    sprintf(local, "----------------------------------------");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	if (sg_ocsp_cert)
	{
		ret = AosStrUtil_bin2HexStr(sg_ocsp_cert->serial_number.data, 
			sg_ocsp_cert->serial_number.len, sn, 50);

		//
    	// Hugo, 10/14/2005, Bug #24.
    	// change OSCP to OCSP.
    	//

		sprintf(local, "\nOCSP           %-15s    %s", 
			sg_ocsp_cert->name, sn);

    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		num_certs++;
	}

	if (sg_crl_cert)
	{
		ret = AosStrUtil_bin2HexStr(sg_crl_cert->serial_number.data, 
			sg_crl_cert->serial_number.len, sn, 50);
		sprintf(local, "\nCRL            %-15s    %s", 
			sg_crl_cert->name, sn);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		num_certs++;
	}

	if (sg_sys_cert)
	{
		ret = AosStrUtil_bin2HexStr(sg_sys_cert->serial_number.data, 
			sg_sys_cert->serial_number.len, sn, 50);
		sprintf(local, "\nSystem         %-15s    %s", 
			sg_sys_cert->name, sn);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		num_certs++;
	}

	aos_lock(cert_list_lock);
	aos_list_for_each(entry, &cert_list)
	{
		cert = (X509_CERT_INFO *)entry;

		ret = AosStrUtil_bin2HexStr(cert->serial_number.data, 
			cert->serial_number.len, sn, 50);
		if (ret <= 0)
		{
			sprintf(errmsg, "Certificate %s serial number invalid", 
				cert->name);
			return -eAosRc_CertErr;
		}

		sprintf(local, "\nDatabase       %-15s    %s", cert->name, sn);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		num_certs++;
	}

	aos_unlock(cert_list_lock);
	
	if (num_certs == 0)
	{
		sprintf(local, "\nNo certificates available");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	
    sprintf(local, "\n----------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}


int AosCertMgr_showCert(char *data, 
                        unsigned int *length,
						const char *name,
                        char *errmsg)
{
	struct aos_list_head *entry;
	X509_CERT_INFO *cert;
    unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIdx = 0;
	int found = 0;

	aos_lock(cert_list_lock);
	if (sg_ocsp_cert && strcmp(sg_ocsp_cert->name, name) == 0)
	{
		found = 1;
		rsltIdx += x509_cert2str(sg_ocsp_cert, rsltBuff, optlen);
	}
	else if (sg_crl_cert && strcmp(sg_crl_cert->name, name) == 0)
	{
		found = 1;
		rsltIdx += x509_cert2str(sg_crl_cert, rsltBuff, optlen);
	}
	else if (sg_sys_cert && strcmp(sg_sys_cert->name, name) == 0)
	{
		found = 1;
		rsltIdx += x509_cert2str(sg_sys_cert, rsltBuff, optlen);
	}
	else
	{
		aos_list_for_each(entry, &cert_list)
		{
			cert = (X509_CERT_INFO *)entry;
	
			if (strcmp(cert->name, name) == 0)
			{
				found = 1;
				rsltIdx += x509_cert2str(cert, rsltBuff, optlen);
				break;
			}
		}
	}
	aos_unlock(cert_list_lock);

	if (!found)
	{
		char local[50];
		sprintf(local, "Certificates %s not found", name);
    	aosCheckAndCopy(rsltBuff, &rsltIdx, optlen, local, strlen(local));
	}
	
	*length = rsltIdx;

	return 0;
}


int AosCertMgr_showCertCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // cert manager show cert [<name>]
    //
	// If <name> is not present, it is to list all the certificates 
	// (names only). Otherwise, it shows the named certificate.
	//
    char *name = parms->mStrings[0];

	if (parms->mNumStrings == 0)
	{
		return AosCertMgr_listCert(data, length, errmsg);
	} 
	else if (parms->mNumStrings == 1)
	{
		if (!name)
		{
			strcpy(errmsg, "Missing name");
			return -eAosRc_CertErr;
		}

		return AosCertMgr_showCert(data, length, name, errmsg);
	}
	
	strcpy(errmsg, "Incorrect number of arguments");
	return -eAosRc_CertErr;
}


int AosCertMgr_clearAllCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // cert manager clear config 
    //
	X509_CERT_INFO *cert;

	*length = 0;

	aos_lock(cert_list_lock);
	while (cert_list.next != &cert_list)
	{
		cert = (X509_CERT_INFO *)cert_list.next;
		aos_list_del(cert_list.next);
		AosCert_put(cert);
	}

	if (sg_ocsp_cert)
	{
		AosCert_put(sg_ocsp_cert);
		sg_ocsp_cert = 0;
	}

	if (sg_crl_cert)
	{
		AosCert_put(sg_crl_cert);
		sg_crl_cert = 0;
	}

	if (sg_sys_cert)
	{
		AosCert_put(sg_sys_cert);
		sg_sys_cert = 0;
	}

	if (sg_sys_priv_key)
	{
		//aos_free(sg_sys_priv_key);
		sg_sys_priv_key = NULL;
	}
	
	aos_unlock(cert_list_lock);
	return 0;
}


int AosCertMgr_saveConfCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager save config
	//
	struct aos_list_head *entry;
	X509_CERT_INFO *cert;
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);
	char *local = (char *)aos_malloc_atomic(eAosCertMgr_MaxAddCmdLen);
	int ret;

	if (sg_ocsp_cert)
	{
		// cert manager import filecert ocsp <cert-name> <filename>
		sprintf(local, 
			"<Cmd>cert manager import filecert "
			"ocsp %s %s/Certificates/%s</Cmd>\n", 
			sg_ocsp_cert->name, AosSystem_getDataDir(), 
			sg_ocsp_cert->name);
        ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, 
			local, strlen(local));
	}

	if (sg_crl_cert)
	{
		// cert manager import filecert crl <cert-name> <filename>
		sprintf(local, 
			"<Cmd>cert manager import filecert "
			"crl %s %s/Certificates/%s</Cmd>\n", 
			sg_crl_cert->name, AosSystem_getDataDir(), 
			sg_crl_cert->name);
        ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, 
			local, strlen(local));
	}

	if (sg_sys_cert)
	{
		// cert manager import filecert system <cert-name> <filename>
		sprintf(local, 
			"<Cmd>cert manager import filecert "
			"system %s %s/Certificates/%s</Cmd>\n", 
			sg_sys_cert->name, AosSystem_getDataDir(), 
			sg_sys_cert->name);
        ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, 
			local, strlen(local));
	}

	if (sg_sys_priv_key)
	{
		// cert manager import file private key <cert-name> <filename>
		sprintf(local, 
			"<Cmd>cert manager import file private key "
			"%s %s/Certificates/%s.key</Cmd>\n", 
			sg_sys_cert->name, AosSystem_getDataDir(), 
			sg_sys_cert->name);
        ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, 
			local, strlen(local));
	}

   	// cert manager add <name> <certb64>
	aos_lock(cert_list_lock);
	aos_list_for_each(entry, &cert_list)
	{
		cert = (X509_CERT_INFO *)entry;
		sprintf(local, 
			"<Cmd>cert manager import filecert "
			"db %s %s/Certificates/%s</Cmd>\n", 
			cert->name, 
			AosSystem_getDataDir(),
			cert->name);
        ret = aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, 
			local, strlen(local));
		if (ret)
		{
			break;
		}
    }

	aos_unlock(cert_list_lock);
	
	*length = rsltIndex;
	aos_free(local);

	return 0;
}


// 
// This function imports a certificate into cert_list
//
static int AosCertMgr_importDbCert(
			char *name, 
			char *cert_base64, 
            char *errmsg)
{
	X509_CERT_INFO *cert;
	X509_CERT_INFO *cert_found;
	int ret;

	// 
	// Make sure the certificate is not in the system
	//
	if (NULL != (cert_found=AosCertMgr_getCertByName(name)))
	{
		AosCert_put(cert_found);
		sprintf(errmsg, "Certificate already exist");
		return -eAosRc_PkcsErr;
	}

	ret = AosCertMgr_importCert(name, cert_base64, &cert, errmsg, 1);
	if (ret)
	{
		sprintf(errmsg, "Failed to import certificate: %d", ret);
		return -eAosRc_PkcsErr;
	}
	AosCert_put(cert);
	return 0;
}


static int AosCertMgr_importSysCert(char *name, 
			char *cert_base64, 
            X509_CERT_INFO **cert,
            char *errmsg)
{
	// 
	// cert manager import [ocsp|crl|system|db] cert <name> <cert_base64>
	//
	X509_CERT_INFO *old_cert = 0;
	X509_CERT_INFO *cert_found = 0;
	int ret;

	// 
	// Make sure the certificate is not in the system
	//
	if (NULL != (cert_found = AosCertMgr_getCertByName(name)))
	{
		AosCert_put(cert_found);
		sprintf(errmsg, "Certificate already exist");
		return -eAosRc_PkcsErr;
	}

	if (*cert)
	{
		old_cert = *cert;
	}

	ret = AosCertMgr_importCert(name, cert_base64, cert, errmsg, 0);

	if (!*cert)
	{
		*cert = old_cert;
	}
	else
	{
		if (old_cert)
		{
			AosCert_put(old_cert);
		}
	}

	return ret;
}


static int AosCertMgr_importCertFunc(
				char *type,
				char *name, 
				char *cert_base64, 
				char *errmsg)
{
	if (!type)
	{
		sprintf(errmsg, "Missing certificate type");
		return -eAosRc_PkcsErr;
	}

	if (!name)
	{
		sprintf(errmsg, "Missing certificate name");
		return -eAosRc_PkcsErr;
	}

	if (!cert_base64)
	{
		sprintf(errmsg, "Missing certificate");
		return -eAosRc_PkcsErr;
	}

	if (strcmp(type, "ocsp") == 0)
	{
		return AosCertMgr_importSysCert(name, cert_base64, 
			&sg_ocsp_cert, errmsg);
	}

	if (strcmp(type, "crl") == 0)
	{
		return AosCertMgr_importSysCert(name, cert_base64, 
			&sg_crl_cert, errmsg);
	}

	if (strcmp(type, "system") == 0)
	{
		return AosCertMgr_importSysCert(name, cert_base64, 
			&sg_sys_cert, errmsg);
	}

	if (strcmp(type, "db") == 0)
	{
		return AosCertMgr_importDbCert(name, cert_base64, errmsg);
	}

	sprintf(errmsg, "Invalid certificate type: %s", type);
	return -eAosRc_PkcsErr;
}




int AosCertMgr_importPrivKey(
			const char *cert_name, 
			const char *key_base64, 
			char *errmsg)
{
	X509_CERT_INFO *cert;
	AosRsaPrivateKey_t *prikey;
	int ret = 0;

	cert = AosCertMgr_getCertByName(cert_name);
	if(!cert)
	{
		sprintf(errmsg, "Cert '%s' is not found", cert_name);
		return -eAosRc_PkcsErr;
	}
		

	prikey = (AosRsaPrivateKey_t *)aos_malloc_atomic(sizeof(AosRsaPrivateKey_t));
	if (!prikey)
	{
		sprintf(errmsg, "Failed to allocate memory");
		AosCert_put(cert);
		return -eAosRc_PkcsErr;
	}

	ret = AosCert_decodePrivKey(key_base64, prikey);
	if (ret)
	{
		AosCert_put(cert);
		sprintf(errmsg, "Failed to decode the private key");
		aos_free(prikey);
		return -eAosRc_PkcsErr;
	}

	cert->prikey = prikey;

	// 
	// set system private key. 
	//
	if (cert == sg_sys_cert)
	{
		sg_sys_priv_key = prikey;
	}

	AosCert_put(cert);
	return 0;
}


int AosCertMgr_importPrivKeyCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager import private key <name> <key>
	//
	char *name = parms->mStrings[0];
	char *key_base64 = parms->mStrings[1];

	*length = 0;
	if (parms->mNumStrings != 2)
	{
		sprintf(errmsg, "Number of string parms mismatch: %d", 
			parms->mNumStrings);
		return -eAosRc_PkcsErr;
	}

	return AosCertMgr_importPrivKey(name, key_base64, errmsg);
}

int AosCertMgr_importFCertCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager import filecert [ocsp|crl|system|db] <name> 
	//		<filename> <cert_base64>
	//
	char *type = parms->mStrings[0];
	char *name = parms->mStrings[1];
	char *cert_base64 = parms->mStrings[3];
	
	*length = 0;
	if (parms->mNumStrings != 4)
	{
		sprintf(errmsg, "Number of string parms mismatch: %d", 
			parms->mNumStrings);
		return -eAosRc_PkcsErr;
	}

	return AosCertMgr_importCertFunc(type, name, cert_base64, errmsg);
}


int AosCertMgr_importFPrivKeyCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager import file private key <cert_name> 
	//		<filename> <key_base64>
	//
	char *cert_name = parms->mStrings[0];
	char *key_base64 = parms->mStrings[2];
	
	*length = 0;
	if (parms->mNumStrings != 3)
	{
		sprintf(errmsg, "Number of string parameters mismatch: %d", 
			parms->mNumStrings);
		return -eAosRc_PkcsErr;
	}

	return AosCertMgr_importPrivKey(cert_name, key_base64, errmsg);
}

	
int AosCertMgr_importCertCli(
				char *data, 
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
	// 
	// cert manager import cert [ocsp|crl|system] <name> <cert_base64>
	//
	char *type = parms->mStrings[0];
	char *name = parms->mStrings[1];
	char *cert_base64 = parms->mStrings[2];
	
	*length = 0;
	if (parms->mNumStrings != 3)
	{
		sprintf(errmsg, "Number of string parms mismatch: %d", 
			parms->mNumStrings);
		return -eAosRc_PkcsErr;
	}

	return AosCertMgr_importCertFunc(type, name, cert_base64, errmsg);
}

int AosCertMgr_setCertTypeCli(
	char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
    // 
    // cert manager setcert <type> <name> 
    // 
    char *type = parms->mStrings[0];
    char *name = parms->mStrings[1];
    X509_CERT_INFO *cert;

    *length = 0;
   	if(!type)
	{
		sprintf(errmsg, "Cert type error: type is null");
		return -eAosRc_PkcsErr;
	}
	 
   	if(!name)
	{
		sprintf(errmsg, "Cert name error: name is null");
		return -eAosRc_PkcsErr;
	}

	if(strcmp(type, "system") != 0)
	{
		sprintf(errmsg, "Cert type should only be 'system'");
		return -eAosRc_PkcsErr;
	}

	cert = AosCertMgr_getCertByName(name);
	if(!cert)
	{
		sprintf(errmsg, "The cert named '%s' is not exist in CERT DB", name);
		return -eAosRc_PkcsErr;
	}


	sg_sys_cert = cert;
	sg_sys_priv_key = cert->prikey;
	
	AosCert_put(cert);	
    return eAosRc_Success;
}


//
// Returns the certificate for the virtual service 'vs_id'.
// Different virtual services may use different OCSP servers,
// therefore, different OCSP certificates. Virtual services
// may also use the system OCSP certificate.
//
// If no certificate is associated with the specified virtual
// service, the function returns NULL.
//
X509_CERT_INFO * AosCertMgr_getOcspCert(const u32 vs_id)
{
    //
    // In the current implementation, all virtual services use
    // the system OCSP certificate.
    //
    return sg_ocsp_cert;
}


//
// Returns the CRL certificate for the virtual service 'vs_id'.
// Different virtual services may use different CRL servers,
// therefore, different CRL certificates. Virtual services
// may also use the system CRL certificate.
//
// If no certificate is associated with the specified virtual
// service, the function returns NULL.
//
X509_CERT_INFO * AosCertMgr_getCrlCert(const u32 vs_id)
{
    //
    // In the current implementation, all virtual services use
    // the system CRL certificate.
    //
    return sg_crl_cert;
}


//
// Returns this unit's certificate, called System Certificate.
// If the certificate was not configured yet, it returns NULL.
//
X509_CERT_INFO * AosCertMgr_getSystemCert(void)
{
    return sg_sys_cert;
}


//
// Returns this unit's private key, called System Private Key.
// If the private key was not configured yet, it returns NULL.
//
AosRsaPrivateKey_t * AosCertMgr_getSystemPrivKey(void)
{
    return sg_sys_priv_key;
}

static int AosCertMgr_importCertDer(
            char *name,
            char *cert_der,
            u32 der_len,
            X509_CERT_INFO **cert_created,
            char add_to_list)
{
    int ret;
    X509_CERT_INFO *cert_found;
	
	*cert_created = 0;
    //
    // Make sure the certificate is not in the system
    //
    aos_lock(cert_list_lock);
    if (NULL != (cert_found=AosCertMgr_getCertByName(name)))
    {
    	 AosCert_put(cert_found);
        aos_unlock(cert_list_lock);
        aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, "Certificate already exist");
        return -eAosRc_PkcsErr;
    }

    if (!cert_der)
    {
        aos_unlock(cert_list_lock);
        aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, "Missing certificate");
        return -eAosRc_CertErr;
    }

    //
    // Decode the certificate
    //
    if ((ret = AosCertMgr_decode(cert_der, der_len, cert_created)))
    {
        aos_unlock(cert_list_lock);
        aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, "Failed to decode cert: %d", ret);
        return -eAosRc_CertErr;
    }

    strncpy((*cert_created)->name, name, eAosCertNameLen);

    if (add_to_list)
    {
		AosCert_hold(*cert_created);
        aos_list_add((struct aos_list_head *)(*cert_created), &cert_list);
    }
    aos_unlock(cert_list_lock);

    return 0;
}

int AosCertMgr_importSystemCertDer(
	char *name,
    char *cert_der,
    u32 der_len)
{
	X509_CERT_INFO *cert;
	int ret;
	
	aos_assert1(name);

 	ret = AosCertMgr_importCertDer(name, cert_der, der_len, &cert, 0);
    if (ret)
    {
        aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, "import der format certificate failed");
        return -1;
    }

    if (sg_sys_cert)
    {
        AosCert_put(sg_sys_cert);
    }
    sg_sys_cert = cert;
	
	return 0;
}

int AosCertMgr_importDbCertDer(
	char *name,
	char *cert_der,
	u32 der_len)
{
	X509_CERT_INFO *cert;
	X509_CERT_INFO *cert_found;
	int ret;
	
	aos_assert1(name);
	aos_assert1(cert_der);
	// 
	// Make sure the certificate is not in the system
	//
	if (NULL != (cert_found=AosCertMgr_getCertByName(name)))
	{
		AosCert_put(cert_found);
		aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, "%s Certificate already exist", name);
		return -eAosRc_PkcsErr;
	}

	ret = AosCertMgr_importCertDer(name, cert_der, der_len, &cert, 1);
	if (ret)
    	{
        	aos_alarm(eAosMD_SSL, eAosAlarm_PkcsErr, "import %s der format certificate failed", name);
        	return -1;
    	}
	AosCert_put(cert);
	return 0;
}


// 
// Yang Tao, 02/21/2006, #83
//
int AosCertMgr_setOcspCert(char *cert_der, int cert_der_len)
{
	if (sg_ocsp_cert)
	{
		aos_free(sg_ocsp_cert);
	}

	//sg_ocsp_cert = aos_malloc_atomic(sizeof(struct X509_CERT_INFO));
	sg_ocsp_cert = (X509_CERT_INFO*)aos_malloc_atomic(sizeof(X509_CERT_INFO));
	if (!sg_ocsp_cert)
	{
		// 
		// No memory. Raise alarm
		//
		aos_alarm(eAosMD_PKCS, eAosAlarm_MemErr, 
				"Failed to allocate memory for OCSP Server Certificate!!!");
		return -eAosAlarm_MemErr;
	}

	if (AosCertMgr_decode(cert_der, cert_der_len, &sg_ocsp_cert))
	{
		// 
		// Failed to decodde the certificate. Raise alarm
		//
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
				"Failed to decode OCSP Server Certificate!!!");
	}

	// 
	// OCSP certificate is successfully decoded.
	//
	return 0;
}


/*
int AosCertMgr_releaseChain(AosCertChain_t *chain)
{
	int i;
	aos_assert1(chain);

	i = 0;
	while (chain->chain[i])
	{
		AosCert_put(chain->chain[i]);
		chain->chain[i] = 0;
	}

	chain->name[0] = 0;
	return 0;
}


AosCertChain_t *
AosCertMgr_getChainByName(const char *name)
{
	int i = 0;

	while (sg_cert_chains.chains[i])
	{
		if (strcmp((sg_cert_chains.chains[i])->name, name) == 0)
		{
			return sg_cert_chains.chains[i];
		}

		i++;
	}

	return 0;
}
			
		
int AosCertMgr_addCertChainCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // cert manager add cert chain <name> [<certificate>]*
    //
    char *name = parms->mStrings[0];
	AosCertChain chain, *the_chain;
	unsigned int i;
	char *cert_name;
	X509_CERT_INFO *cert;

	AosKAPI_checkStr(name, "Missing chain name", 
		-eAosRc_PkcsErr, errmsg);
	if (strlen(name) > eAosCertMgr_NameLen)
	{
		sprintf(errmsg, "Name too long: %d. Maximum: %d", 
			strlen(name), eAosCertMgr_NameLen);
		return -eAosRc_PkcsErr;
	}

	if (parms->mNumStrings <= 1)
	{
		sprintf(errmsg, "Expecting at least two parameters but got "
			"only: %d parameters", parms->mNumStrings);
		return -eAosRc_PkcsErr;
	}

	if (parms->mNumStrings - 1 > eAosCertMgr_ChainSize)
	{
		sprintf(errmsg, "Too many certificates in a chain: %d. "
			"Maximum allowed: %d", parms->mNumStrings-1, 
			eAosCertMgr_ChainSize);
		return -eAosRc_PkcsErr;
	}

	memset(&chain, 0, sizeof(AosCertChain_t));
	for (i=1; i<parms->mNumStrings; i++)
	{
		cert_name = parms->mStrings[i];
		if (!cert_name)
		{
			sprintf(errmsg, "The %d-th name is empty!", i);
			return -eAosRc_PkcsErr;
		}

		cert = AosCertMgr_getCertByName(cert_name);
		if (!cert)
		{
			sprintf(errmsg, "Certificate %s not found", cert_name);
			return -eAosRc_PkcsErr;
		}

		chain.chain[i] = cert;
	}

	the_chain = AosCertMgr_getChainByName(name);
	if (!the_chain)
	{
		// 
		// It is to add a new chain. 
		//
		if (sg_cert_chains.num_chains >= eAosCertMgr_MaxChains)
		{
			sprintf(errmsg, "Too many chains: %d", 
				(unsigned int)sg_cert_chains.num_chains);
			AosCertMgr_releaseChain(&chain);
			return -eAosRc_PkcsErr;
		}

		the_chain = 
			(AosCertChain_t *)aos_malloc_atomic(sizeof(AosCertChain_t));
		if (!the_chain)
		{
			aos_alarm(eAosMD_PKCS, eAosAlarm_MemErr, 
				"Failed to allocate memory");
			AosCertMgr_releaseChain(&chain);
			return -eAosRc_PkcsErr;
		}
	}

	memcpy(the_chain, &chain, sizeof(AosCertChain_t));
	sg_cert_chains.chains[sg_cert_chains.num_chains++] = the_chain;
	return 0;
}

*/

#ifdef __KERNEL__
EXPORT_SYMBOL( AosCertMgr_getSystemPrivKey );
EXPORT_SYMBOL( AosCertMgr_getSystemCert );
#endif

