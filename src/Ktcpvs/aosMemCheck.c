////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMemCheck.c
// Description:
//		Add Memory Check Function in this file, the Memory Checking will 
// be run as a thread  
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/mm.h>
#include <linux/time.h>
#include <linux/smp_lock.h>
#include <asm/page.h>

#include "KernelPorting/Daemonize.h"
#include "KernelPorting/Current.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"

char gaos_mem_check_start = 0;
unsigned long gaos_mem_check_timeout = 5*HZ;

#define KK(x) ((x) << (PAGE_SHIFT - 10))
int aos_mem_check_thread(void *param)
{
	struct sysinfo info;
	struct timeval check_time;
	
	lock_kernel();
	aosDaemonize("AosMemCheckThread");
	// Block all signals except SIGKILL and SIGSTOP
	spin_lock_irq(&aosCurrent_Siglock(current));
	siginitsetinv(&current->blocked, sigmask(SIGKILL) | sigmask(SIGSTOP));
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));

	while(gaos_mem_check_start)
	{
		si_meminfo(&info);
		OmnGetTimeOfDay(&check_time);
		aos_min_log(eAosMD_TcpProxy, "MemCheckDate: %lu, MemFree: %8luKB", check_time.tv_sec, KK(info.freeram));
		__set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(gaos_mem_check_timeout);
		__set_current_state(TASK_RUNNING);
	}
	return 0;
}

int aosMem_checkStartCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
    char *start = parms->mStrings[0];
	*length = 0;
	if (strcmp(start,"on") == 0)
	{
		if (!gaos_mem_check_start)
		{
			gaos_mem_check_start = 1;
			if (kernel_thread(aos_mem_check_thread, NULL, CLONE_VM | CLONE_FS | CLONE_FILES) < 0)
			{
				gaos_mem_check_start = 0;
				strcpy(errmsg, "Aos Memory Checking Thread start failed\n");
				return -1;
			}
		}
	}

	if (strcmp(start, "off") == 0)
	{
		gaos_mem_check_start = 0;
	}
	return eAosRc_Success;
}

int aosMem_checkTimeoutCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen)
{
    int  timeout = parms->mIntegers[0];
	*length = 0;
	
	aos_assert1((timeout > 0) && (timeout < 65536));
	
	gaos_mem_check_timeout = timeout*HZ;
	
	return eAosRc_Success;
}



