////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelApi.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosKernelApi.h"

#include "aos/KernelApiStruct.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosConfig.h"
#include "aos_core/aosSocket.h"
#include "aosUtil/Alarm.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"

#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/timer.h>
#include <KernelSimu/jiffies.h>
#include <KernelSimu/uaccess.h>

#include "aosUtil/CharPtree.h"

#include "AppProc/http_proc/ForwardTable.h"

static struct aosCharPtree *sg_cli_tree = 0;
static int    				sg_module_init_flag = 0;


int aosInitKernelApiCli(char *data,
						unsigned int *optlen,
						struct aosKernelApiParms *parms,
						char *errmsg,
						const int errlen)
{
	return 0;
}

extern int jmk_init(void);

int OmnKernelApi_init(void)
{
	if (sg_module_init_flag) return 0;

    sg_cli_tree = (struct aosCharPtree *)aosCharPtree_create();
	aos_assert1(sg_cli_tree);

	sg_module_init_flag = 1;
	return OmnKernelApi_addCliCmds();
}


static int aosKernelApi_convert(char *data,
						 unsigned int datalen,
						 struct aosKernelApiParms *parms)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	// Index counts from 1.
	//
	unsigned int i, ii, index;
	char *dd;
	int len = strlen(data);
	int *iv = (int *)&data[len+1];

	if ((ii = len + 1 + (iv[0] + 2) * 4) >datalen)
	{
		return eAosRc_IndexOutBound;
	}

	//
	// Retrieve the integers
	//
	parms->mNumIntegers = iv[0];
	parms->mNumStrings = iv[1];
	if (parms->mNumIntegers < 0 ||
		parms->mNumIntegers > eAosMaxKernelApiIntegers)
	{
		return eAosRc_IndexOutBound;
	}

	for (index = 0; index < parms->mNumIntegers; index++)
	{
		parms->mIntegers[index] = iv[index+2];
	}

	//
	// Retrieve the strings
	//

	dd = &data[ii];
	index = 0;
	for (i=0; i<parms->mNumStrings; i++)
	{
		parms->mStrings[index++] = &data[ii];

		ii += strlen(dd) + 1;
		if (ii > datalen)
		{
			return eAosRc_IndexOutBound;
		}
		dd = &data[ii];
	}

	return 0;
}

int OmnKernelApi_addTestCmds(void)
{
	int ret = 0;
	return ret;
}


int OmnKernelApi_addCliCmds(void)
{
	int ret = 0;

	ret = OmnKernelApi_addTestCmds();

	// Chen Ding, 09/24/2005
    ret |= OmnKernelApi_addCliCmd("cert_add", AosCertMgr_addCertCli);
    ret |= OmnKernelApi_addCliCmd("cert_del", AosCertMgr_delCertCli);
    ret |= OmnKernelApi_addCliCmd("cert_show", AosCertMgr_showCertCli);
    ret |= OmnKernelApi_addCliCmd("cert_clear", AosCertMgr_clearAllCli);
    ret |= OmnKernelApi_addCliCmd("cert_save", AosCertMgr_saveConfCli);
    ret |= OmnKernelApi_addCliCmd("import_cert", AosCertMgr_importCertCli);
    ret |= OmnKernelApi_addCliCmd("import_privkey", AosCertMgr_importPrivKeyCli);
    ret |= OmnKernelApi_addCliCmd("import_fprivkey", AosCertMgr_importFPrivKeyCli);
    ret |= OmnKernelApi_addCliCmd("import_fcert", AosCertMgr_importFCertCli);
//    ret |= OmnKernelApi_addCliCmd("set_ciphers", AosSsl_setCiphersCli);
//    ret |= OmnKernelApi_addCliCmd("show_ciphers", AosSsl_showCiphersCli);
//    ret |= OmnKernelApi_addCliCmd("show_supported_ciphers", AosSsl_showSupportedCiphersCli);
//    ret |= OmnKernelApi_addCliCmd("add_cert_chain", AosCertChain_addCli);
//    ret |= OmnKernelApi_addCliCmd("del_cert_chain", AosCertChain_delCli);
//    ret |= OmnKernelApi_addCliCmd("show_cert_chain", AosCertChain_showCli);
//    ret |= OmnKernelApi_addCliCmd("clear_cert_chain", AosCertChain_clearallCli);
//    ret |= OmnKernelApi_addCliCmd("save_cert_chain", AosCertChain_saveconfCli);

	ret |= OmnKernelApi_addCliCmd("fwdtbl_entry_add", aos_fwdtbl_entry_add_cli);
    ret |= OmnKernelApi_addCliCmd("fwdtbl_entry_del", aos_fwdtbl_entry_del_cli);
    ret |= OmnKernelApi_addCliCmd("fwdtbl_show", aos_fwdtbl_show_cli);
    ret |= OmnKernelApi_addCliCmd("fwdtbl_clearall", aos_fwdtbl_clearall_cli);
    ret |= OmnKernelApi_addCliCmd("fwdtbl_saveconf", aos_fwdtbl_saveconf_cli);

	return ret;
}

int OmnKernelApi_addCliCmd(char *id, aosCliFunc func)
{
	if (!sg_cli_tree)
	{
		return aos_progerr_d(("Kernel API tree is null"));
	}

	return aosCharPtree_insert(sg_cli_tree, id, strlen(id), (void *)func, 0);
}


int OmnProcKernelApi(char *data, 
					 unsigned int *optlen, 
					 char *errmsg, 
					 const int errlen)
{
	//
	// oprId
	// number of integers (integer)
	// number of strings (integer)
	// integer parm
	// integer parm
	// ...
	// string parm
	// string parm
	// ...
	//
	int ret = -1;
	aosCliFunc func;
	int index;
	struct aosKernelApiParms parms;

	memset(&parms, 0, sizeof(struct aosKernelApiParms));

	if (!sg_cli_tree)
	{
		*optlen = 0;
		strcpy(errmsg, "CLI Tree is null. System is not initialized");
		return eAosRc_CliTreeNull;
	}

	ret = aosCharPtree_get(sg_cli_tree, data, &index, (void **)&func);
	if (!func)
	{
		*optlen = 0;
		strcpy(errmsg, "Command not recognized in kernel");
		return eAosRc_CliCmdNotRecognized;
	}

	ret = aosKernelApi_convert(data, *optlen, &parms);
	if (ret)
	{
		*optlen = 0;
		strcpy(errmsg, "CD: Failed to parse parameters");
		return eAosRc_CliCmdParmError;
	}

	return func(data, optlen, &parms, errmsg, errlen);
}


int aosKernelApi_getMac(struct OmnKernelApi_GetMac *data)
{
//	return aosGetMacByName(data->mMac, data->mIpAddr, data->mDevName);
	return 0;
}


