////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertChain.cpp
// Description:
//	Certificate Chain:
//		A certificate chain contains one or more certificates.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/CertChain.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/System.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Mutex.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/List.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/StringUtil.h"
#include "PKCS/x509.h"
#include "PKCS/ReturnCode.h"
#include "PKCS/CertMgr.h"


static u8 						module_init_flag = 0;

static struct aos_list_head 	chain_list;
static aos_mutex_t 				chain_list_lock;


int AosCertChain_init(void)
{
	if (!module_init_flag)
	{
		AOS_INIT_LIST_HEAD(&chain_list);
		AOS_INIT_MUTEX(chain_list_lock);
		
		module_init_flag = 1;
	}

	return 0;
}


int AosCertChain_constructor(AosCertChain_t **chain)
{
	*chain = (AosCertChain_t *)aos_malloc(sizeof(AosCertChain_t));
	aos_assert1(*chain);

	memset(*chain, 0, sizeof(AosCertChain_t));
	return 0;
}


int AosCertChain_destructor(AosCertChain_t *chain)
{
	int i;
	
	chain->next = (struct aos_list_head *)AOS_POINTER_POISON;
	chain->prev = (struct aos_list_head *)AOS_POINTER_POISON;

	for (i=0; i<chain->num_certs; i++)
	{
		AosCert_put(chain->certs[i]);
		chain->certs[i] = (X509_CERT_INFO *)AOS_POINTER_POISON;
	}

	chain->num_certs = AOS_U8_POISON;
	aos_free(chain);
	return 0;
}


//
// IMPORTANT: This function does not lock the list. The caller should
// lock it before calling this funciton.
//
AosCertChain_t *
AosCertChain_getChainByName(const char *name)
{
	struct aos_list_head *entry;
	AosCertChain_t *chain;

	aos_list_for_each(entry, &chain_list)
	{
		chain = (AosCertChain_t *)entry;
		if (strcmp(chain->name, name) == 0)
		{
			return chain;
		}
	}

	return 0;
}


int AosCertChain_addCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert chain add <name> [<cert-name>]*
	//
	AosCertChain_t *chain;
	unsigned int i, j, num_certs;
	const char *name = parms->mStrings[0];
	char *cert_name;
	X509_CERT_INFO *certs[eAosCertChain_MaxMembers];

	*length = 0;

	if (parms->mNumStrings < 2)
	{
		strcpy(errmsg, "Invalid number of arguments. At least two"); 
		return -eAosRc_PkcsErr;
	}

	if (!name)
	{
		strcpy(errmsg, "Name is null");
		return -eAosRc_PkcsErr;
	}

	AosKAPI_checkStr(name, "Missing Chain Name", 
		-eAosRc_PkcsErr, errmsg);

	if (strlen(name) >= eAosCertChain_NameLen)
	{
		sprintf(errmsg, "Name too long: %d. Maximum allowed: %d", 
			strlen(name), eAosCertChain_NameLen);
		return -eAosRc_PkcsErr;
	}

	// 
	// Verify whether all the certificates are defined
	//
	for (i=1; i<parms->mNumStrings; i++)
	{
		cert_name = parms->mStrings[i];
		if (!cert_name)
		{
			sprintf(errmsg, "The %d-th certificate name is null", i);
			return -eAosRc_PkcsErr;
		}

		certs[i-1] = AosCertMgr_getCertByName(cert_name);
		if (!certs[i-1])
		{
			sprintf(errmsg, "The %u-th certificate is not found", i);

			for (j=0; j<i-1; j++)
			{
				AosCert_put(certs[j]);
			}
			return -eAosRc_PkcsErr;
		}
	}

	num_certs = parms->mNumStrings - 1;

	// 
	// We have retrieved all the certificates.
	//
	aos_lock(chain_list_lock);
	chain = AosCertChain_getChainByName(name);

	if (chain)
	{
		// 
		// The chain is defined. This command modifies the chain
		//
		for (i=0; i<chain->num_certs; i++)
		{
			AosCert_put(chain->certs[i]);
			chain->certs[i] = 0;
		}
		chain->num_certs = 0;
	}
	else
	{
		AosCertChain_constructor(&chain);
		aos_assert1(chain);
		strcpy(chain->name, name);
		aos_list_add_tail((struct aos_list_head *)chain, &chain_list);
	}

	// 
	// Add the certs
	//
	for (i=0; i<num_certs; i++)
	{
		chain->certs[i] = certs[i];
	}
	chain->num_certs = num_certs;

	aos_unlock(chain_list_lock);

	return 0;
}


int AosCertChain_delCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager cert chain remove <name>
	//
	struct aos_list_head *entry;
	AosCertChain_t *chain;
	const char *name = parms->mStrings[0];

	*length = 0;

	if (parms->mNumStrings != 1)
	{
		sprintf(errmsg, "Invalid number of arguments. Expect 1 but: %d", 
			parms->mNumStrings + parms->mNumIntegers); 
		return -eAosRc_PkcsErr;
	}

	if (!name)
	{
		strcpy(errmsg, "Name is null");
		return -eAosRc_PkcsErr;
	}

	// 
	// We have retrieved all the certificates.
	//
	aos_lock(chain_list_lock);
	aos_list_for_each(entry, &chain_list)
	{
		chain = (AosCertChain_t *)entry;
		if (strcmp(chain->name, name) == 0)
		{
			aos_list_del(entry);
			AosCertChain_destructor(chain);
			aos_unlock(chain_list_lock);
			return 0;
		}
	}

	aos_unlock(chain_list_lock);
	strcpy(errmsg, "Chain not found!");
	return 0;
}


int AosCertChain_list(char *data, 
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg)
{
	// 
	// Name      Cert      Cert       ...
	// -----------------------------------------
	//
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
    char local[200];
    struct aos_list_head *entry;
	AosCertChain_t *chain;
	int i;
	int count = 0;

    sprintf(local, "Name            Certificates ...       \n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
    sprintf(local, "----------------------------------------");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	aos_lock(chain_list_lock);
	aos_list_for_each(entry, &chain_list)
	{
		chain = (AosCertChain_t *)entry;
		sprintf(local, "\n%-16s", chain->name);

		for (i=0; i<chain->num_certs; i++)
		{
			sprintf(local + strlen(local), "%-15s", chain->certs[i]->name);
		}
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

		count++;
	}
	aos_unlock(chain_list_lock);

	if (count == 0)
	{
		sprintf(local, "\nNo certificate chains found");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

    sprintf(local, "\n----------------------------------------");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}


int AosCertChain_showCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager cert chain show 
	//
	if (parms->mNumStrings == 0)
	{
		return AosCertChain_list(data, length, parms, errmsg);
	}
	else
	{
		sprintf(errmsg, "Arguments mismatch. Expect 0 or 1 arguments but: %d", 
			parms->mNumStrings);
		return -eAosRc_PkcsErr;
	}

	return 0;
}


int AosCertChain_clearallCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager cert chain clear all
	//
	AosCertChain_t *chain;

	*length = 0;

	aos_lock(chain_list_lock);
	while (chain_list.next != &chain_list)
	{
		chain = (AosCertChain_t *)chain_list.next;
		aos_list_del((struct aos_list_head *)chain);
		AosCertChain_destructor(chain);
	}
	aos_unlock(chain_list_lock);
	return 0;
}


int AosCertChain_saveconfCli(char *data, 
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// cert manager cert chain save config
	//
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
	struct aos_list_head *entry;
	AosCertChain_t *chain;
	int i;
	char local[200];

	*length = 0;

	aos_lock(chain_list_lock);
	aos_list_for_each(entry, &chain_list)
	{
		chain = (AosCertChain_t *)entry;

		// 
		// The command "cert chain add <name> [<cert-name>]*
		// 
		sprintf(local, "<Cmd>cert chain add %s", chain->name);	
		for (i=0; i<chain->num_certs; i++)
		{
			sprintf(local + strlen(local), " %s", chain->certs[i]->name);
		}
		sprintf(local + strlen(local), "</Cmd>");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	aos_unlock(chain_list_lock);

	*length = rsltIndex;
	return 0;
}


AosCertChain_t *AosCertChain_getByName(const char *name)
{
	AosCertChain_t *chain = 0;
	struct aos_list_head *entry;

	aos_lock(chain_list_lock);
	aos_list_for_each(entry, &chain_list)
	{
		chain = (AosCertChain_t *)entry;
		if (strcmp(chain->name, name) == 0)
		{
			aos_unlock(chain_list_lock);
			return chain;
		}
	}

	aos_unlock(chain_list_lock);
	return 0;
}

// name :the name of chain 
// cert_name: the name of the cert added to the chain
int AosCertChain_add(char *name, char *cert_name)
{
	AosCertChain_t *chain;
	X509_CERT_INFO *certs;

	if (!name)
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_ProgErr, "the chain name is null");
		return -eAosRc_PkcsErr;
	}

	if (strlen(name) >= eAosCertChain_NameLen)
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_ProgErr, "Name too long: %d. Maximum allowed: %d", 
			strlen(name), eAosCertChain_NameLen);
		return -eAosRc_PkcsErr;
	}

	// 
	// Verify whether all the certificates are defined
	//
	if (!cert_name)
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_ProgErr, "The certificate name is null");
		return -eAosRc_PkcsErr;
	}

	certs = AosCertMgr_getCertByName(cert_name);
	if (!certs)
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_ProgErr, "The %s certificate is not found", cert_name);
		return -eAosRc_PkcsErr;
	}

	aos_lock(chain_list_lock);
	chain = AosCertChain_getChainByName(name);

	if (!chain)
	{
		AosCertChain_constructor(&chain);
		aos_assert1(chain);
		strcpy(chain->name, name);
		aos_list_add_tail((struct aos_list_head *)chain, &chain_list);
	}

	if (chain->num_certs >= eAosCertChain_MaxMembers)
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_ProgErr, "The number of certificates in chain reach maximum,"
				"you can not insert cert to chain %s any more", name);
		return -eAosRc_PkcsErr;
	}	
	// 
	// Add the certs
	//
	chain->certs[chain->num_certs] = certs;
	chain->num_certs++;

	aos_unlock(chain_list_lock);

	return 0;
}

int AosCertChain_del(char *chain_name)
{
	struct aos_list_head *entry;
	AosCertChain_t *chain;

	aos_assert1(chain_name);
	aos_lock(chain_list_lock);
	aos_list_for_each(entry, &chain_list)
	{
		chain = (AosCertChain_t *)entry;
		if (strcmp(chain->name, chain_name) == 0)
		{
			aos_list_del(entry);
			AosCertChain_destructor(chain);
			aos_unlock(chain_list_lock);
			return 0;
		}
	}
	aos_unlock(chain_list_lock);
	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( AosCertChain_getByName );
#endif

