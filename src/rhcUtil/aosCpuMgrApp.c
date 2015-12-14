////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCpuMgrApp.cpp
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

// Our RHC System
#include "rhcUtil/aosCpuMgrApp.h"
#include "rhcUtil/aosResMgrToolkit.h"
#include "rhcUtil/aosResMgrDefines.h"
#include "aosUtil/Alarm.h"

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>

// Linux programmer time
#include <sys/time.h>

//struct CpuMgr_Alarm_Val
//{
//};
void reset_CpuMgr_Alarm_Val(struct CpuMgr_Alarm_Val * const ptr)
{
	ptr->m_nIdleRatio = 0;
	ptr->m_nAlarmFlag = 0;
	ptr->m_bIsOverload = FALSE;
}

//struct CpuMgr_App
//{
//};
void reset_config_CpuMgr_App(struct CpuMgr_App * const ptrApp)
{
	ptrApp->m_duration = RESMGR_SAMPL_DURATION_DEFAULT;
	ptrApp->m_interval = RESMGR_SAMPL_INTERVAL_DEFAULT;
	ptrApp->m_maxr 	   = ptrApp->m_duration / ptrApp->m_interval;
	// reset the resource manager normal configuration values
	reset_ResMgr_Config(&(ptrApp->m_resmgr_app));
}

void reset_CpuMgr_App(struct CpuMgr_App * const ptrApp)
{
	ptrApp->m_index    = 0;
	ptrApp->m_is_first_sample = TRUE;
	reset_CpuMgr_Alarm_Val(&(ptrApp->m_CpuMgrAlarm));

	ptrApp->m_duration = RESMGR_SAMPL_DURATION_DEFAULT;
	ptrApp->m_interval = RESMGR_SAMPL_INTERVAL_DEFAULT;
	ptrApp->m_maxr 	   = ptrApp->m_duration / ptrApp->m_interval;
	reset_ResMgr_App(&(ptrApp->m_resmgr_app));
}

// Note declare global variable here!!!
struct CpuMgr_App g_theCpuMgrApp;

// Encapsulate the operation of g_theCpuMgrApp
void get_CpuMgr_ResMgr_Threshold_App(ResMgr_Threshold_t * const ptr)
{
	if(ptr)
	{
		(*ptr) = g_theCpuMgrApp.m_resmgr_app.m_threshold_val;
	}
}

// Encapsulate the operation of g_theCpuMgrApp
void set_CpuMgr_ResMgr_Threshold_App(const ResMgr_Threshold_t * ptr)
{
	if(ptr)
	{
		if(g_theCpuMgrApp.m_resmgr_app.m_switch_on)
		{
			pthread_mutex_lock (&g_theCpuMgrApp.m_resmgr_app.m_mutex);
		}
		aos_resmgr_copy_threshold(&g_theCpuMgrApp.m_resmgr_app.m_threshold_val, ptr);
		if(g_theCpuMgrApp.m_resmgr_app.m_switch_on)
		{
			pthread_mutex_unlock (&g_theCpuMgrApp.m_resmgr_app.m_mutex);
		}
	}
}

// Translate input time string to formatted time string
// from "now", "-1H", "2M" to "yyyymmdd-hh:MM:SS"

// print help infomation and exit the program
void printCpuMgrHelp(const char *program_name)
{
	printf ("Usage: \n%s [Interval] [Duration]\n"
			"Description: \n\tshow CPU statistics daemon %s\n"
			"\t[Interval], Interger number, For setting the sampling time interval.\n"
			"\t[Duration], Interger number, For setting the sampling data time length.\n"
			,program_name, program_name);
}

// CPU statistics design:
// IdleRatio = (idleCur - idlePrev)/(Sigma(cpu_*Cur) - Sigma(cpu_*Prev))
int getIdleRatio(const CpuInfoRecordType *CpuInfoSnapCurr, const CpuInfoRecordType *CpuInfoSnapPrev)
{
	return ((CpuInfoSnapCurr->cpu_idle - CpuInfoSnapPrev->cpu_idle)*100/(getCpuUsageSum(CpuInfoSnapCurr,CpuInfoSnapPrev)));
}

// CPU busy checking
BOOL IsCPUOverload(const int nIdleRatio, int *nAlarmFlag)
{
	BOOL bStatus = FALSE;
	if(100 - nIdleRatio >= g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio)
		(*nAlarmFlag)++;
	else
		(*nAlarmFlag) = 0;
		
	bStatus = 
			(	(*nAlarmFlag) * g_theCpuMgrApp.m_interval
				>= g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime);

	(*nAlarmFlag) = (bStatus) ? 0 : (*nAlarmFlag);
	return bStatus;
}
// CPU normalize checking
BOOL IsCPUNormalload(const int nIdleRatio, int *nAlarmFlag)
{
	BOOL bStatus = FALSE;
	if(100 - nIdleRatio < g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio)
		(*nAlarmFlag)++;
	else
		(*nAlarmFlag) = 0;
		
	bStatus = 
			(	(*nAlarmFlag) * g_theCpuMgrApp.m_interval
				>= g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime);

	(*nAlarmFlag) = (bStatus) ? 0 : (*nAlarmFlag);
	return bStatus;
}

// running this function after each interval
void aos_cpu_mgr_repeat(int nSignal)
{
	getCpuInfo(&g_theCpuMgrApp.m_CpuInfoSnap);
	if(!g_theCpuMgrApp.m_is_first_sample)
	{
		// doing the statistics process
		g_theCpuMgrApp.m_CpuMgrAlarm.m_nIdleRatio = 
			getIdleRatio(&g_theCpuMgrApp.m_CpuInfoSnap, 
						 &g_theCpuMgrApp.m_CpuInfoSnapPrev[g_theCpuMgrApp.m_index%CPUMGR_PREV_STAT_MAX]);
		if(!g_theCpuMgrApp.m_CpuMgrAlarm.m_bIsOverload)
		{
			// doing the checking process
			g_theCpuMgrApp.m_CpuMgrAlarm.m_bIsOverload = 
						IsCPUOverload(g_theCpuMgrApp.m_CpuMgrAlarm.m_nIdleRatio, 
									  &g_theCpuMgrApp.m_CpuMgrAlarm.m_nAlarmFlag);
			if(g_theCpuMgrApp.m_CpuMgrAlarm.m_bIsOverload)
			{
				// Sending up to alarm state message!
				aos_alarm( eAosMD_AosUtil, eAosAlarm_General, 
					"CPU alarm: CPU is overload!");
#ifdef _DEBUG_ALARM_PRINT
				printf("\nCPU alarm: CPU is overload! FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
			//	write_alarm("CPU alarm: CPU is overload!");
			}
		}
		else
		{
			// doing the checking process
			g_theCpuMgrApp.m_CpuMgrAlarm.m_bIsOverload = 
						IsCPUNormalload(g_theCpuMgrApp.m_CpuMgrAlarm.m_nIdleRatio, 
									    &g_theCpuMgrApp.m_CpuMgrAlarm.m_nAlarmFlag);
			if(!g_theCpuMgrApp.m_CpuMgrAlarm.m_bIsOverload)
			{
				// Sending down to normal state message!
				aos_alarm( eAosMD_AosUtil, eAosAlarm_General, 
					"CPU alarm: CPU is normalload!");
#ifdef _DEBUG_ALARM_PRINT
				printf("CPU alarm: CPU is normalload! FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
			//	write_alarm("CPU alarm: CPU is normalload!");
			}
		}
	}
	else
	{
		g_theCpuMgrApp.m_is_first_sample = FALSE;
	}
#ifdef _DEBUG_
printf("Inside Cpu repeat function. FILE(%s) LINE(%d)\n", __FILE__, __LINE__ );
#endif
	saveCpuInfo(g_theCpuMgrApp.m_index, 
				g_theCpuMgrApp.m_maxr, g_theCpuMgrApp.m_interval, 
				g_theCpuMgrApp.m_duration, &g_theCpuMgrApp.m_CpuInfoSnap);

	// Doing the data copy and setting process
	++g_theCpuMgrApp.m_index;
	if(g_theCpuMgrApp.m_index >= g_theCpuMgrApp.m_maxr*2)
	{
		g_theCpuMgrApp.m_index = 0;
	}
	g_theCpuMgrApp.m_CpuInfoSnapPrev[g_theCpuMgrApp.m_index%CPUMGR_PREV_STAT_MAX] 
				=  g_theCpuMgrApp.m_CpuInfoSnap;
}

int aos_cpumgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen)
{
	return aos_resmgr_set_record_time(argc, argv, 
									zErrmsg, nErrorLen, 
									&g_theCpuMgrApp.m_maxr, &g_theCpuMgrApp.m_duration, 
									&g_theCpuMgrApp.m_interval);
}

// main --> refine to thread 
void cpuMgrThreadFunc(void * cpu_thread_input) //(res_thread_struct_t * cpu_thread_input)
{
	char zErrmsg[256] = "";
	int rc;

	// 1. Validate the input 
	if(strcasecmp(((res_thread_struct_t *)cpu_thread_input)->argv1, "--help") == 0)
	{
		printCpuMgrHelp(((res_thread_struct_t *)cpu_thread_input)->argv0);
		return ;
	}

	if(0 != getuid())
	{
		strcpy(zErrmsg, "Error: Authority not enough.");
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)cpu_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 2. Get the input value
	g_theCpuMgrApp.m_interval = getDuration(((res_thread_struct_t *)cpu_thread_input)->argv1);
	g_theCpuMgrApp.m_duration = getDuration(((res_thread_struct_t *)cpu_thread_input)->argv2);
	if(g_theCpuMgrApp.m_duration <= 0 || g_theCpuMgrApp.m_interval <= 0)
	{
		snprintf(zErrmsg, 256, "Error: Illegal input. \nargv0[%s]\nargv1[%s]\nargv2[%s]\n", 
								((res_thread_struct_t *)cpu_thread_input)->argv0, 
								((res_thread_struct_t *)cpu_thread_input)->argv1, 
								((res_thread_struct_t *)cpu_thread_input)->argv2);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)cpu_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}
	// 3. Get the key value 
	g_theCpuMgrApp.m_maxr = g_theCpuMgrApp.m_duration / g_theCpuMgrApp.m_interval;
	if(g_theCpuMgrApp.m_maxr > MAX_TIME_RATIO || g_theCpuMgrApp.m_maxr < MIN_TIME_RATIO)
	{
		// illegal input! 
		snprintf(zErrmsg, 256, "Error: Records number [%d] out of bound [%d,%d].", g_theCpuMgrApp.m_maxr, MIN_TIME_RATIO, MAX_TIME_RATIO);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)cpu_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 4.1 if the old thread is running 
	if(g_theCpuMgrApp.m_resmgr_app.m_status_on)
	{
		snprintf(zErrmsg, 256, "Error: system failure. The old manager thread is running.");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}
	g_theCpuMgrApp.m_resmgr_app.m_status_on = TRUE;

	// 4.2 move and cover the old file to *.1 file (keep some old record)
	if(!backupLastResFile(CPU_INFO_FILE_0, CPU_INFO_FILE_1))
	{
		sprintf(zErrmsg, "Error: system failure when backup the files [%s] and [%s].", CPU_INFO_FILE_0, CPU_INFO_FILE_1);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)cpu_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}
	// 4.3 registe the signal handler
	reset_reg_alarm_sig(&g_theCpuMgrApp.m_resmgr_app.m_resmgr_sigactions);

	// 4.4 set sleep time 
	struct timespec timeout;

	// 5. Always hold the recent maxr number records
	while (g_theCpuMgrApp.m_resmgr_app.m_switch_on)
	{
		pthread_mutex_lock (&g_theCpuMgrApp.m_resmgr_app.m_mutex);
		/* prepare timeout value.              */
		/* Note that we need an absolute time. */
		clock_gettime(CLOCK_REALTIME, &timeout);
		// Delaying interval 
		timeout.tv_sec += g_theCpuMgrApp.m_interval;
		rc = pthread_cond_timedwait(&g_theCpuMgrApp.m_resmgr_app.m_thread_kill, &g_theCpuMgrApp.m_resmgr_app.m_mutex, &timeout);
		pthread_mutex_unlock (&g_theCpuMgrApp.m_resmgr_app.m_mutex);
		if(g_theCpuMgrApp.m_resmgr_app.m_switch_on)
		{
			aos_cpu_mgr_repeat(0);
		}
	}
	g_theCpuMgrApp.m_resmgr_app.m_status_on = FALSE;
	// 6. Leave the current thread
	{
		pthread_mutex_lock (&g_theCpuMgrApp.m_resmgr_app.m_mutex);
		pthread_cond_signal(&g_theCpuMgrApp.m_resmgr_app.m_thread_stopped);
		pthread_mutex_unlock (&g_theCpuMgrApp.m_resmgr_app.m_mutex);
	}
}

// encapsulate the process of create a detached thread
BOOL cpuMgrSwitchOn()
{
	strcpy(g_theCpuMgrApp.m_resmgr_app.m_thread_input.argv0, "CpuMgr");
	sprintf(g_theCpuMgrApp.m_resmgr_app.m_thread_input.argv1, "%d", g_theCpuMgrApp.m_interval);
	sprintf(g_theCpuMgrApp.m_resmgr_app.m_thread_input.argv2, "%ld", g_theCpuMgrApp.m_duration);
	g_theCpuMgrApp.m_resmgr_app.m_switch_on = TRUE;
	// create the thread 
	aos_uland_thread_create(&g_theCpuMgrApp.m_resmgr_app.m_thread, 
							cpuMgrThreadFunc, 
							&g_theCpuMgrApp.m_resmgr_app.m_thread_input, 
							&g_theCpuMgrApp.m_resmgr_app.m_mutex, 
							&g_theCpuMgrApp.m_resmgr_app.m_thread_kill, 
							&g_theCpuMgrApp.m_resmgr_app.m_thread_stopped, 
							&g_theCpuMgrApp.m_resmgr_app.m_switch_on);
	return TRUE;
}

// encapsulate the process of kill a detached thread
BOOL cpuMgrSwitchOff()
{
	// if the old thread is not running, return 
	if(!g_theCpuMgrApp.m_resmgr_app.m_switch_on)
	{
		return TRUE;
	}

	// kill old thread
	g_theCpuMgrApp.m_resmgr_app.m_switch_on = FALSE;

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
//	ts.tv_sec += (g_theCpuMgrApp.m_interval*10);
	ts.tv_sec += (RESMGR_EXPIRED_TIME_DEFAULT);
	aos_uland_thread_destroy(&g_theCpuMgrApp.m_resmgr_app.m_thread, 
							 &g_theCpuMgrApp.m_resmgr_app.m_mutex, 
							 &g_theCpuMgrApp.m_resmgr_app.m_thread_kill, 
							 &g_theCpuMgrApp.m_resmgr_app.m_thread_stopped, 
							 &g_theCpuMgrApp.m_resmgr_app.m_switch_on);
	g_theCpuMgrApp.m_resmgr_app.m_status_on = FALSE;
	g_theCpuMgrApp.m_index 	= 0;
	g_theCpuMgrApp.m_is_first_sample= TRUE;

	return TRUE;
}
