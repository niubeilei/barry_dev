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
#include <KernelSimu/netdevice.h>
#include <KernelSimu/dev.h>
#include <KernelSimu/route.h>
#include <KernelSimu/string.h>
#include <KernelSimu/aosKernelDebug.h>
#include "KernelUtil/MgdObj.h"
#include "PKCS/CertMgr.h"
#include "Servers/AppServer.h"
#include "Servers/ServerGroup.h"
#include "aosUtil/Tracer.h"

#ifdef __KERNEL__
#include "KernelUtil/aostcpapi.h"
#include "AppProxy/DenyPage.h"
#endif

static int sgAosCoreInitFlag = 0;

int aosCore_moduleInit(void)
{
    int ret = 0;

	ret |= aos_log_init(); 
	ret |= aosMgdObj_moduleInit();
    ret |= AosCertMgr_init();


#ifdef AOS_KERNEL_SIMULATE
	ret |= dev_init();
    ret |= aosInitRtTable();
#endif

	return ret;
}


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

	ret = aosCore_moduleInit() || OmnKernelApi_init();
	if (ret)
	{
		strcpy(errmsg, "Failed to init AOS");
		return ret;
	}

#ifdef __KERNEL__
	ret = aos_ktcpvs_init();
	if (ret)
	{
		strcpy(errmsg, "Failed to init ktcpvs");
		return ret;
	}
#endif

	return ret;
}

