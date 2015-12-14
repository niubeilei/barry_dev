////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 2007-02-14 Created by CHK
////////////////////////////////////////////////////////////////////////////

// Our RHC System
#include "rhcUtil/aosMemMgrApp.h"

#include "rhcUtil/aosResMgrDefines.h"
#include "rhcUtil/aosResMgrToolkit.h"
#include "rhcUtil/aosMemMgrToolkit.h"
#include "aosUtil/Alarm.h"

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/utsname.h>

// Linux programmer
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

void reset_MemMgr_Alarm_Val(struct MemMgr_Alarm_Val * const ptr)
{
	ptr->m_nFreeRatio = 0;	//  = 0 /* the space ratio of free */
	ptr->m_nAlarmFlag = 0;	//  = 0 /*(m_nAlarmFlag*ninterval) time period to send alarm*/ 
	ptr->m_bIsOverload = FALSE;
}

void reset_config_MemMgr_App(struct MemMgr_App * const ptrApp)
{
	ptrApp->m_duration = RESMGR_SAMPL_DURATION_DEFAULT;
	ptrApp->m_interval = RESMGR_SAMPL_INTERVAL_DEFAULT;
	ptrApp->m_maxr 	   = ptrApp->m_duration / ptrApp->m_interval;
	reset_ResMgr_Config(&(ptrApp->m_resmgr_app));
}

void reset_MemMgr_App(struct MemMgr_App * const ptrApp)
{
	ptrApp->m_index    = 0;
	ptrApp->m_is_first_sample = TRUE;
	reset_MemMgr_Alarm_Val(&(ptrApp->m_MemMgrAlarm));

	ptrApp->m_duration = RESMGR_SAMPL_DURATION_DEFAULT;
	ptrApp->m_interval = RESMGR_SAMPL_INTERVAL_DEFAULT;
	ptrApp->m_maxr 	   = ptrApp->m_duration / ptrApp->m_interval;
	reset_ResMgr_App(&(ptrApp->m_resmgr_app));
}

// Note: declare global variable
struct MemMgr_App g_theMemMgrApp;

// Encapsulate the operation of g_theMemMgrApp
void get_MemMgr_ResMgr_Threshold_App(ResMgr_Threshold_t * const ptr)
{
	if(ptr)
	{
		(*ptr) = g_theMemMgrApp.m_resmgr_app.m_threshold_val;
	}
}

// Encapsulate the operation of g_theMemMgrApp
void set_MemMgr_ResMgr_Threshold_App(const ResMgr_Threshold_t * ptr)
{
	if(ptr)
	{
		if(g_theMemMgrApp.m_resmgr_app.m_switch_on)
		{
			pthread_mutex_lock (&g_theMemMgrApp.m_resmgr_app.m_mutex);
		}
		aos_resmgr_copy_threshold(&g_theMemMgrApp.m_resmgr_app.m_threshold_val, ptr);
		if(g_theMemMgrApp.m_resmgr_app.m_switch_on)
		{
			pthread_mutex_unlock (&g_theMemMgrApp.m_resmgr_app.m_mutex);
		}
	}
}

// load the system usage of memory
BOOL getSysMemInfo(struct MemInfoRecord *snap)
{
	FILE *fp = NULL;
	char tmpBuf[MAX_MEM_INFO];
//	char data[MEMMGR_MAX_LINE_LEN];
	char cTmp = 0;
	char *ptrData = NULL, *ptrTmp = NULL;
	int nCounter = 0;

	memset(tmpBuf, 0, MAX_MEM_INFO);

	fp = fopen(MEMMGR_PROC_MEM_FILE, "r");
	if(NULL == fp)
	{
		return FALSE;
	}
	while((cTmp = fgetc(fp)) != EOF)
	{
		strncat(tmpBuf, &cTmp, 1);
	}
	strncat(tmpBuf, &cTmp, 1);
	if(fp)
	{
		fclose(fp); 
		fp = NULL;
	}
	// 1. get the data from the system memory file 
	// 1.1. get total memory size
	if((ptrData = strcasestr(tmpBuf, "MemTotal:")) != NULL)
	{
		ptrData += strlen("MemTotal:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->total, ptrData, nCounter);
		snap->total[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->total);
	}
	// 1.2. get free memory size
	if((ptrData = strcasestr(tmpBuf, "MemFree:")) != NULL)
	{
		ptrData += strlen("MemTotal:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->free, ptrData, nCounter);
		snap->free[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->free);
	}
	// 1.3. get buffer memory size
	if((ptrData = strcasestr(tmpBuf, "Buffers:")) != NULL)
	{
		ptrData += strlen("MemTotal:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->buffer, ptrData, nCounter);
		snap->buffer[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->buffer);
	}
	// 1.4. get cached memory size
	if((ptrData = strcasestr(tmpBuf, "Cached:")) != NULL)
	{
		ptrData += strlen("Cached:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->cached, ptrData, nCounter);
		snap->cached[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->cached);
	}
	// 1.5. get SwapCached memory size
	if((ptrData = strcasestr(tmpBuf, "SwapCached:")) != NULL)
	{
		ptrData += strlen("SwapCached:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->swapcached, ptrData, nCounter);
		snap->swapcached[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->swapcached);
	}
	// 1.6. get SwapTotal memory size
	if((ptrData = strcasestr(tmpBuf, "SwapTotal:")) != NULL)
	{
		ptrData += strlen("SwapTotal:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->swaptotal, ptrData, nCounter);
		snap->swaptotal[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->swaptotal);
	}
	// 1.7. get SwapFree memory size
	if((ptrData = strcasestr(tmpBuf, "SwapFree:")) != NULL)
	{
		ptrData += strlen("SwapFree:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->swapfree, ptrData, nCounter);
		snap->swapfree[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->swapfree);
	}
	// 1.8. get active memory size
	if((ptrData = strcasestr(tmpBuf, "Active:")) != NULL)
	{
		ptrData += strlen("Active:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->active, ptrData, nCounter);
		snap->active[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->active);
	}
	// 1.9. get inactive memory size
	if((ptrData = strcasestr(tmpBuf, "Inactive:")) != NULL)
	{
		ptrData += strlen("Inactive:");
		while('\t' == (*ptrData) || ' ' == (*ptrData))
		{
			ptrData++;
		}
		ptrTmp = ptrData;
		nCounter = 0;
		while('\n' != (*ptrTmp) && nCounter < MEMMGR_MAX_LINE_LEN - 1)
		{
			ptrTmp++;
			nCounter++;
		}
		strncpy(snap->inactive, ptrData, nCounter);
		snap->inactive[nCounter] = '\0';	// set to be a null-terminated string
		eraseSpace(snap->inactive);
	}
	return TRUE;
}

// load the system usage of memory
BOOL getProcMemInfo(struct ProcessMemEntry *proc_mem_entry, const char * zFilePath)
{
	FILE *fp = NULL;

	fp = fopen(zFilePath, "r"); 
	if(NULL == fp)
	{
		return FALSE;
	}
	fscanf(fp, "%d %d %d %d %d %d %*d", \
		&proc_mem_entry->proc_mem_size, &proc_mem_entry->proc_mem_resident, 
		&proc_mem_entry->proc_mem_share, &proc_mem_entry->proc_mem_text, \
		&proc_mem_entry->proc_mem_lib, &proc_mem_entry->proc_mem_data);
	if(fp)
	{
		fclose(fp); 
		fp = NULL;
	}
	return TRUE;
}

// load the current status of the system memory
BOOL getMemInfo(struct MemInfoRecord *snap)
{
	DIR *dir = NULL;
	struct dirent *drp = NULL;
	int i = 0;
	char zTmp[64];

	if(NULL == snap)
	{
		return FALSE;
	}
	// 1.  Load system memory status infomation
	if(!getSysMemInfo(snap))
	{
		return FALSE;
	}

	// 2.  Load process memory status infomation
	// 2.1. Open /proc directory
	if ((dir = opendir(RESMGR_SYS_PROC)) == NULL) 
	{
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("error on opendir[%s]", RESMGR_SYS_PROC);
		}
#endif
		return FALSE;
	}
	// 2.2. Load process id infomation
	i = 0;
	while (((drp = readdir(dir)) != NULL) && (i < MAX_PROCESS_NUM))
	{
		if (drp->d_name[0] < '0' || drp->d_name[0] > '9')
		{
			continue;
		}
		snap->p[i].pid = atol(drp->d_name);
		i++;
	}
	if(dir)
	{
		closedir(dir);
		dir = NULL;
	}
	snap->pnum = i;

	// 2.3. Load process memory status infomation
	for (i = 0 ; i < snap->pnum; i++) 
	{
		sprintf(zTmp, MEMMGR_PID_STATM, snap->p[i].pid);
		if(!getProcMemInfo(&snap->p[i], zTmp))
		{
			reset_ProcessMemEntry(&snap->p[i]);
		}
	}

	return TRUE;
}

BOOL saveMemInfo(const long index, const int maxr, const int interval, const long duration, struct MemInfoRecord *snap)
{
	FILE *fp = NULL;
	char *zLocalTime = NULL;
	char process_info[MAX_PROCESS_INFO];
	char tmp[256];
	int i = 0;

	if (((index/maxr)%2)==0)
	{
		if((index%maxr)==0)
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if(NULL == zLocalTime || NULL == (fp = fopen(MEM_INFO_FILE_0, "w+")))
				return FALSE; 
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if((fp = fopen(MEM_INFO_FILE_0, "a")) == NULL)
				return FALSE; 
		}
	}
	else
	{
		if((index%maxr)==0)
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if(NULL == zLocalTime || NULL == (fp = fopen(MEM_INFO_FILE_1, "w+")))
				return FALSE; 
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if((fp = fopen(MEM_INFO_FILE_1, "a")) == NULL)
				return FALSE; 
		}
	}
	sprintf(process_info, "%d", snap->pnum);
	for(i = 0; i < snap->pnum; i++)
	{
		strcat(process_info, " ");
		sprintf(tmp, "%lu %d %d %d %d %d %d", snap->p[i].pid, 
				snap->p[i].proc_mem_size, snap->p[i].proc_mem_resident, 
				snap->p[i].proc_mem_share, snap->p[i].proc_mem_text, 
				snap->p[i].proc_mem_lib, snap->p[i].proc_mem_data);
		strcat(process_info, tmp);
	}
	fprintf(fp, "%lu %s %s %s %s %s %s %s %s %s %s\n", 
		time(NULL),
		snap->total,snap->free, snap->buffer, 
		snap->cached, snap->swapcached, 
		snap->active, snap->inactive, 
		snap->swaptotal, snap->swapfree, 
		process_info);

	if(fp)
	{
		fclose(fp); 
		fp = NULL;
	}

	return TRUE;
}

// print help infomation and exit the program
void printMemMgrHelp(const char *program_name)
{
	printf ("Usage: \n%s [Interval] [Duration]\n"
			"Description: \n\tshow memory statistics daemon %s\n" 
			"\t[Interval], Interger number, For setting the sampling time g_theMemMgrApp.m_interval.\n"
			"\t[Duration], Interger number, For setting the sampling data time length.\n"
			,program_name, program_name);
}

// Memory statistics design:
// FreeRatio = (freeSize)/(totalSize) * 100;
int getMemFreeRatio(const MemInfoRecordType &MemInfoSnapCurr)
{
	return (int)(100*(float)atol(MemInfoSnapCurr.free)/atol(MemInfoSnapCurr.total));
}

// Memory usage checking
BOOL IsMemOverload(const int nFreeRatio, int *nAlarmFlag)
{
	BOOL bStatus = FALSE;
	if(100 - nFreeRatio >= g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio)
		(*nAlarmFlag)++;
	else
		(*nAlarmFlag) = 0;
		
	bStatus = 
			(	(*nAlarmFlag) * g_theMemMgrApp.m_interval
				>= g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime);

	(*nAlarmFlag) = (bStatus) ? 0 : (*nAlarmFlag);
	return bStatus;
}

// Memory normalize checking
BOOL IsMemNormalload(const int nFreeRatio, int *nAlarmFlag)
{
	BOOL bStatus = FALSE;
	if(100 - nFreeRatio < g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio)
		(*nAlarmFlag)++;
	else
		(*nAlarmFlag) = 0;
		
	bStatus = 
			(	(*nAlarmFlag) * g_theMemMgrApp.m_interval
				>= g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime);

	(*nAlarmFlag) = (bStatus) ? 0 : (*nAlarmFlag);
	return bStatus;
}


// running this function after each g_theMemMgrApp.m_interval
void aos_mem_mgr_repeat(int nSignal)
{
	getMemInfo(&g_theMemMgrApp.m_MemInfoSnap);
	if(!g_theMemMgrApp.m_is_first_sample)
	{
		// doing the statistics process
		g_theMemMgrApp.m_MemMgrAlarm.m_nFreeRatio = 
			getMemFreeRatio(g_theMemMgrApp.m_MemInfoSnap);
		if(!g_theMemMgrApp.m_MemMgrAlarm.m_bIsOverload)
		{
			// doing the checking process
			g_theMemMgrApp.m_MemMgrAlarm.m_bIsOverload = 
						IsMemOverload( g_theMemMgrApp.m_MemMgrAlarm.m_nFreeRatio, 
									  &g_theMemMgrApp.m_MemMgrAlarm.m_nAlarmFlag);
			if(g_theMemMgrApp.m_MemMgrAlarm.m_bIsOverload)
			{
				// Sending up to alarm state message!
				aos_alarm( eAosMD_AosUtil, eAosAlarm_General, 
					"Memory alarm: Memory is overload!");
#ifdef _DEBUG_ALARM_PRINT
				printf("Memory alarm: Memory is overload! FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
			//	write_alarm("CPU alarm: CPU is overload!");
			}
		}
		else
		{
			// doing the checking process
			g_theMemMgrApp.m_MemMgrAlarm.m_bIsOverload = 
						IsMemNormalload(g_theMemMgrApp.m_MemMgrAlarm.m_nFreeRatio, 
										&g_theMemMgrApp.m_MemMgrAlarm.m_nAlarmFlag);
			if(!g_theMemMgrApp.m_MemMgrAlarm.m_bIsOverload)
			{
				// Sending down to normal state message!
				aos_alarm( eAosMD_AosUtil, eAosAlarm_General, 
					"Memory alarm: Memory is uderload!");
#ifdef _DEBUG_ALARM_PRINT
				printf("Memory alarm: Memory is uderload! FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
			//	write_alarm("Memory alarm: Memory is normalload!");
			}
		}
	}
	else
	{
		g_theMemMgrApp.m_is_first_sample = FALSE;
	}
#ifdef _DEBUG_
printf("Inside memory repeat function. FILE(%s) LINE(%d)\n", __FILE__, __LINE__ );
#endif
	saveMemInfo(g_theMemMgrApp.m_index, 
				g_theMemMgrApp.m_maxr, g_theMemMgrApp.m_interval, 
				g_theMemMgrApp.m_duration, &g_theMemMgrApp.m_MemInfoSnap);

	// Doing the data copy and setting process
	++g_theMemMgrApp.m_index;
	if(g_theMemMgrApp.m_index >= g_theMemMgrApp.m_maxr*2)
	{
		g_theMemMgrApp.m_index = 0;
	}
	g_theMemMgrApp.m_MemInfoSnapPrev[g_theMemMgrApp.m_index%MEMMGR_PREV_STAT_MAX] 
				=  g_theMemMgrApp.m_MemInfoSnap;
}


// set sampling record interval or duration
int aos_memmgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen)
{
	return aos_resmgr_set_record_time(argc, argv, 
									zErrmsg, nErrorLen, 
									&g_theMemMgrApp.m_maxr, &g_theMemMgrApp.m_duration, 
									&g_theMemMgrApp.m_interval);
}

// main --> refine to thread 
void memMgrThreadFunc(void * mem_thread_input) //(res_thread_struct_t * cpu_thread_input)
{
	char zErrmsg[256] = "";
	int rc;

	// 1. Validate the input 
	if(strcasecmp(((res_thread_struct_t *)mem_thread_input)->argv1, "--help") == 0)
	{
		printMemMgrHelp(((res_thread_struct_t *)mem_thread_input)->argv0);
		return ;
	}

	if(0 != getuid())
	{
		strcpy(zErrmsg, "Error: Authority not enough.");
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)mem_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 2. Get the input value
	g_theMemMgrApp.m_interval = getDuration(((res_thread_struct_t *)mem_thread_input)->argv1);
	g_theMemMgrApp.m_duration = getDuration(((res_thread_struct_t *)mem_thread_input)->argv2);
	if(g_theMemMgrApp.m_duration <= 0 || g_theMemMgrApp.m_interval <= 0)
	{
		snprintf(zErrmsg, 256, "Error: Illegal input. \nargv0[%s]\nargv1[%s]\nargv2[%s]\n", 
								((res_thread_struct_t *)mem_thread_input)->argv0, 
								((res_thread_struct_t *)mem_thread_input)->argv1, 
								((res_thread_struct_t *)mem_thread_input)->argv2);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}
	// 3. Get the key value 
	g_theMemMgrApp.m_maxr = g_theMemMgrApp.m_duration / g_theMemMgrApp.m_interval;
	if(g_theMemMgrApp.m_maxr > MAX_TIME_RATIO || g_theMemMgrApp.m_maxr < MIN_TIME_RATIO)
	{
		// illegal input! 
		snprintf(zErrmsg, 256, "Error: Records number [%d] out of bound [%d,%d].", g_theMemMgrApp.m_maxr, MIN_TIME_RATIO, MAX_TIME_RATIO);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)mem_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 4.1 if the old thread is running 
	if(g_theMemMgrApp.m_resmgr_app.m_status_on)
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
	g_theMemMgrApp.m_resmgr_app.m_status_on= TRUE;

	// 4.2 move and cover the old file to *.1 file (keep some old record)
	if(!backupLastResFile(MEM_INFO_FILE_0, MEM_INFO_FILE_1))
	{
		sprintf(zErrmsg, "Error: system failure when backup the files [%s] and [%s].", MEM_INFO_FILE_0, MEM_INFO_FILE_1);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)mem_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 4.3 registe the signal handler
	reset_reg_alarm_sig(&g_theMemMgrApp.m_resmgr_app.m_resmgr_sigactions);

	// 4.4 set sleep time 
	struct timespec timeout;

	// 5. Always hold the recent maxr number records
	while (g_theMemMgrApp.m_resmgr_app.m_switch_on)
	{
		pthread_mutex_lock (&g_theMemMgrApp.m_resmgr_app.m_mutex);
		/* prepare timeout value.              */
		/* Note that we need an absolute time. */
		clock_gettime(CLOCK_REALTIME, &timeout);
		// Delaying interval 
		timeout.tv_sec += g_theMemMgrApp.m_interval;
		rc = pthread_cond_timedwait(&g_theMemMgrApp.m_resmgr_app.m_thread_kill, &g_theMemMgrApp.m_resmgr_app.m_mutex, &timeout);
		pthread_mutex_unlock (&g_theMemMgrApp.m_resmgr_app.m_mutex);
		if(g_theMemMgrApp.m_resmgr_app.m_switch_on)
		{
			aos_mem_mgr_repeat(0);
		}
	}
	g_theMemMgrApp.m_resmgr_app.m_status_on= FALSE;
	// 6. Leave the current thread
	{
		pthread_mutex_lock (&g_theMemMgrApp.m_resmgr_app.m_mutex);
		pthread_cond_signal(&g_theMemMgrApp.m_resmgr_app.m_thread_stopped);
		pthread_mutex_unlock (&g_theMemMgrApp.m_resmgr_app.m_mutex);
	}
}

// encapsulate the process of create a detached thread
BOOL memMgrSwitchOn()
{
	strcpy(g_theMemMgrApp.m_resmgr_app.m_thread_input.argv0, "MemMgr");
	sprintf(g_theMemMgrApp.m_resmgr_app.m_thread_input.argv1, "%d", g_theMemMgrApp.m_interval);
	sprintf(g_theMemMgrApp.m_resmgr_app.m_thread_input.argv2, "%ld", g_theMemMgrApp.m_duration);
	g_theMemMgrApp.m_resmgr_app.m_switch_on= TRUE;

	// create the thread 
	aos_uland_thread_create(&g_theMemMgrApp.m_resmgr_app.m_thread, 
							memMgrThreadFunc, 
							&g_theMemMgrApp.m_resmgr_app.m_thread_input, 
							&g_theMemMgrApp.m_resmgr_app.m_mutex, 
							&g_theMemMgrApp.m_resmgr_app.m_thread_kill, 
							&g_theMemMgrApp.m_resmgr_app.m_thread_stopped, 
							&g_theMemMgrApp.m_resmgr_app.m_switch_on);

	return TRUE;
}

// encapsulate the process of kill a detached thread
BOOL memMgrSwitchOff()
{
	// if the old thread is not running, return 
	if(!g_theMemMgrApp.m_resmgr_app.m_switch_on)
	{
		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// special description by CHK 2007-02-25
	// here, we set the wait expiration time period as: g_theMemMgrApp.m_interval*10 
	// value 10 is our temporary setting value. just for protecting our own program not in wait jam
	//////////////////////////////////////////////////////////////////////////////////////////////////
//	struct timespec ts;
//	clock_gettime(CLOCK_REALTIME, &ts);
//	ts.tv_sec += (g_theMemMgrApp.m_interval*10);
//	ts.tv_sec += (RESMGR_EXPIRED_TIME_DEFAULT);
	// kill old thread
	g_theMemMgrApp.m_resmgr_app.m_switch_on = FALSE;
	aos_uland_thread_destroy(&g_theMemMgrApp.m_resmgr_app.m_thread, 
							 &g_theMemMgrApp.m_resmgr_app.m_mutex, 
							 &g_theMemMgrApp.m_resmgr_app.m_thread_kill, 
							 &g_theMemMgrApp.m_resmgr_app.m_thread_stopped, 
							 &g_theMemMgrApp.m_resmgr_app.m_switch_on);
	g_theMemMgrApp.m_resmgr_app.m_status_on = FALSE;
	g_theMemMgrApp.m_index 	= 0;
	g_theMemMgrApp.m_is_first_sample= TRUE;

	return TRUE;
}
