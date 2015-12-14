////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BusyLoop.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include "aosUtil/BusyLoop.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/Memory.h"
#include "KernelUtil/KernelStat.h"

#include <linux/jiffies.h>
#include <linux/sched.h>

#ifdef CONFIG_AOS_TEST

// 1. Implement a CLI command
//    system busyloop <duration-in-sec>
//
// 2. When executing the above command, start a kernel thread.
//    while (1)
//    {
//     	if (jiffies >= expected)
//     	{
//     		stop the thread;
//     	}
//    }
//
//

static int sgDuration = 0;
static int sgCpuUsage = 0;
static int sgTimeout = 200;		// 50 ticks


static int AosSystem_BusyloopThreadFunc(void *data)
{
	struct aosSystemStat * currentSysStat;
	int currentCpuIdle=99+1;
	u64 expected = get_jiffies_64() + ((int)sgDuration) * HZ;
	
	while(1)
	{
		if (get_jiffies_64() >= expected) break;
		currentSysStat=AosSystemStat_get();
		currentCpuIdle=currentSysStat->cpu_idle_rate;
		if (currentCpuIdle <= 99 + 1 - sgCpuUsage)
		{	
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_timeout(sgTimeout);
		}
	}
	
	//printk("<0>finished \n");
	return 0;
}


int AosSystem_BusyLoopCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system busyloop <duration-in-sec> [<cpu-usage>]
	//

	sgDuration = parms->mIntegers[0];

	if (parms->mNumIntegers == 2)
	{
		sgCpuUsage = parms->mIntegers[1];
	}
	else
	{
		sgCpuUsage = 99+1;
	}
	
	*length = 0;
	if (kernel_thread(AosSystem_BusyloopThreadFunc,
		(void *)0, CLONE_VM | CLONE_FS | CLONE_FILES) < 0)
	{
		return 1;
	}
	
	return 0;
}


int AosSystem_BusyLoopinit()
{
	OmnKernelApi_addCliCmd("sys_busyloop", AosSystem_BusyLoopCli);	
	return 0;
}


#endif
#endif
