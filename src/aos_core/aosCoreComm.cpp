////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCoreComm.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosCoreComm.h"

#include "aos_core/aosMacCtlr.h"
#include "aos_core/aosMacRule.h"
#include "aos_core/aosRule.h"
#include "aos_core/aosSockCtl.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/CharPtree.h"
#include "AAA/AaaServer.h"
#include "AppProxy/AppProxy.h"
#include <KernelSimu/netdevice.h>
#include <KernelSimu/dev.h>
#include <KernelSimu/route.h>
#include <KernelSimu/string.h>
#include <KernelSimu/aosKernelDebug.h>
#include "KernelUtil/MgdObj.h"
#include "Servers/AppServer.h"
#include "Servers/ServerGroup.h"
#include "aosUtil/Tracer.h"

#ifdef __KERNEL__
#include "KernelUtil/aostcpapi.h"
#include "KernelUtil/KernelStat.h"
#include "AppProxy/DenyPage.h"
#endif

static int sgAosCoreInitFlag = 0;

int aosCore_moduleInit(void)
{
    int ret = 0;

printk("<0>To init modules (Chen Ding, 03/17/2006)\n");

	ret |= aos_log_init(); 
	ret |= aosMgdObj_moduleInit();


#ifdef __KERNEL__
#ifdef CONFIG_TCPVS
	ret |= aosAppProxy_init();
	ret |= aos_deny_page_init();
#endif


    ret |= aosRule_init();
	ret |= aosServerGroup_init();
    ret |= aosMacCtlr_init();
	ret |= aosAppServer_init();
#ifdef CONFIG_AOS_AAA
	ret |= aosAaaServer_init();
#endif

#ifdef CONFIG_AOS_TCPAPI
	ret |= aos_tcpapi_start();
#endif

#ifdef CONFIG_AOS_SSL
	ret |= AosSsl_Init();
#endif

#ifdef CONFIG_CERT_VERIFY
	ret |= AosCertMgr_init();
	ret |= AosCertChain_init();
	ret |= AosOcsp_setRespModeinit();
#endif

	//ret |= AosSystemStat_init();
	ret |= AosSystemArp_init();

#ifdef CONFIG_AOS_TEST
	ret |= AosSystem_BusyLoopinit();
#endif
#endif

#ifdef CONFIG_AOS_APP_HTTP_PROC
	ret |= aos_http_fwdtbl_init();
#endif
	
#ifdef AOS_KERNEL_SIMULATE
	ret |= dev_init();
    ret |= aosInitRtTable();
#endif

	return ret;
}


/* Chen Ding, 2013/05/20
int aosCoreInit(char *errmsg, const int length)
{
	int ret = eAosRc_Success;

	errmsg[0] = 0;
	if (sgAosCoreInitFlag)
	{
		// If a userland program re-runs, it's possible to init
		// aosCoreInit multiple times. Simply ignore.
		return 0;
	}

	sgAosCoreInitFlag = 1;

	// 
	// Chen Ding, 03/17/2006
	// We need to init aosKernelApi first. 
	//
	// ret = aosCore_moduleInit() || OmnKernelApi_init();
	ret = OmnKernelApi_init() || aosCore_moduleInit();
	if (ret)
	{
		strcpy(errmsg, "Failed to init AOS");
		return ret;
	}

#ifdef __KERNEL__
	// by zzh. 20060504
	// ret = aos_ktcpvs_init();
	// if (ret)
	// {
	// 	strcpy(errmsg, "Failed to init ktcpvs");
	// 	return ret;
	// }
	//
#endif

	return ret;
}
*/

