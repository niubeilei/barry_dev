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

#include "AppProxy/DenyPage.h"
#include "aos/KernelApiStruct.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosConfig.h"
#include "aos_core/aosSocket.h"
#include "aosUtil/Alarm.h"
#include "ssl/Ssl.h"

#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/timer.h>
#include <KernelSimu/jiffies.h>
#include <KernelSimu/uaccess.h>

#include "aosUtil/CharPtree.h"

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

    ret |= OmnKernelApi_addCliCmd("DenyPageAdd",        aos_deny_page_add);
    ret |= OmnKernelApi_addCliCmd("DenyPageRemove",         aos_deny_page_remove);
    ret |= OmnKernelApi_addCliCmd("DenyPageShow",       aos_deny_page_show);
    ret |= OmnKernelApi_addCliCmd("DenyPageAssign",         aos_deny_page_assign);
    ret |= OmnKernelApi_addCliCmd("DenyPageDeassign",   aos_deny_page_deassign);
    ret |= OmnKernelApi_addCliCmd("DenyPageSaveConfig", aos_deny_page_save_config);
    ret |= OmnKernelApi_addCliCmd("DenyPageClearAllConfig", aos_deny_page_clearall_config
);  

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


