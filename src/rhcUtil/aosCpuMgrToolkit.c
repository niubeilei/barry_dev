////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCpuMgrToolkit.c
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "rhcUtil/aosCpuMgrToolkit.h"
#include "rhcUtil/aosResMgrToolkit.h"
#include "rhcUtil/aos_resmgr_xml.h"

#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/utsname.h>

// Linux programmer
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//===========================================================================================================
// Tool functions for CPU Manager 
//===========================================================================================================
// get the cpu snap info from the Linux system
BOOL getCpuInfo(struct CpuInfoRecord *snap)
{	
	FILE *fp = NULL, *pidfp = NULL;
	DIR *dir = NULL;
	struct dirent *drp;
	char tmp[64];
	int i = 0;
	
	fp = fopen(PROC_STAT_FILE, "r"); 
	
	fscanf(fp, "%*s %lu %lu %lu %lu %lu %lu %lu", \
		&snap->cpu_user, &snap->cpu_nice, &snap->cpu_system, &snap->cpu_idle, \
		&snap->cpu_iowait, &snap->cpu_irq, &snap->cpu_sirq);
	
	fclose(fp); 

	if ((dir = opendir(RESMGR_SYS_PROC)) == NULL) 
	{
		// perror("opendir");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("error on opendir[%s]", RESMGR_SYS_PROC);
		}
#endif
		// exit(1);
		return FALSE;
	}
	i = 0;
	while (((drp = readdir(dir)) != NULL) && (i < MAX_PROCESS_NUM))
	{
		if (drp->d_name[0] >= '0' && drp->d_name[0] <= '9')
		{
			snap->p[i].pid = atol(drp->d_name);
			i++;
		}
	}
	
	snap->pnum = i;
	if(dir)
	{
		closedir(dir);
		dir = NULL;
	}

	for (i = 0 ; i < snap->pnum; i++) 
	{
		sprintf(tmp, PID_STAT, snap->p[i].pid);
		//printf("now open the proc file %s\n", tmp);
		pidfp = fopen(tmp, "r");
		if (NULL == pidfp)
		{
			// error!!!
			continue;
		}
		fscanf(pidfp, "%*d %s %*s %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*u %*u %*d %*d %*u %*u %*d %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u\n", 
		// fscanf(pidfp, "%d %s %*u %*u\n",
			 snap->p[i].name, &snap->p[i].utime, &snap->p[i].stime);
		 //if (max_pid < pid_stats[pid]->pid)
		 //	 max_pid = pid_stats[pid]->pid;
		if(pidfp)
		{
			fclose(pidfp);
			pidfp = NULL;
		}
	}

	return TRUE;
}

// save the cpu snap info to the data file
BOOL saveCpuInfo(const long index, const int maxr, const int interval, const long duration, struct CpuInfoRecord *snap)
{
	FILE *fp = NULL;
	char process_info[MAX_PROCESS_INFO];
	char tmp[64];
	int i = 0;
	char *zLocalTime = NULL;

	if (((index/maxr)%2)==0)
	{
		if((index%maxr)==0)
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if(NULL == zLocalTime || (fp = fopen(CPU_INFO_FILE_0, "w+")) == NULL)
				return FALSE; 
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
//			if(zLocalTime)
//			{
//				// delete zLocalTime;
//				free(zLocalTime);
//			}
//			zLocalTime = NULL;
		}
		else
		{
			if((fp = fopen(CPU_INFO_FILE_0, "a")) == NULL)
				return FALSE; 
		}
	}
	else
	{
		if((index%maxr)==0)
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if(NULL == zLocalTime || (fp = fopen(CPU_INFO_FILE_1, "w+")) == NULL)
				return FALSE; 
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if((fp = fopen(CPU_INFO_FILE_1, "a")) == NULL)
				return FALSE; 
		}

	}
	sprintf(process_info, "%d", snap->pnum);
	for(i = 0; i < snap->pnum; i++)
	{
		strcat(process_info, " ");
		sprintf(tmp, "%lu %s %lu %lu", snap->p[i].pid, snap->p[i].name, snap->p[i].utime, snap->p[i].stime);
		strcat(process_info, tmp);
	}
   	fprintf(fp, "%lu %lu %lu %lu %lu %lu %lu %lu %s\n", time(NULL), \
		snap->cpu_user, snap->cpu_nice, snap->cpu_system, snap->cpu_idle, \
   		snap->cpu_iowait, snap->cpu_irq, snap->cpu_sirq, \
		process_info);

	fclose(fp); 

	return TRUE;
}

// get the cpu snap data from the data log file 
BOOL getCpuSnapData(CpuInfoRecordType *cpuSnap, const char *line)
{
	const char *zFormat = "%ld %lu %lu %lu %lu %lu %lu %lu %*s";
	int nRslt = 0;
	nRslt = sscanf(line, zFormat, &cpuSnap->index, \
					&cpuSnap->cpu_user, &cpuSnap->cpu_nice, &cpuSnap->cpu_system, &cpuSnap->cpu_idle, \
			   		&cpuSnap->cpu_iowait, &cpuSnap->cpu_irq, &cpuSnap->cpu_sirq);
	if(EOF == nRslt)	// assert(-1 == EOF)
	{
		return 0;
	}
	else if(nRslt <= 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

// get the cpu process snap data from the data log file 
BOOL getProcSnapData(CpuInfoRecordType * const cpuSnap, const char *line)
{
	int nRslt = 0;
	// const char *zCpuFormat = "%lu %lu %lu %lu %lu %lu %lu %lu %*s"
	char *str1, *token = NULL;
	char *saveptr = NULL;
	char strLine[CPUMGR_LOCALBUFFER_MAX];
	int i = 0;
	const char strDelim[3] = " \n";

	nRslt = sscanf(line, "%lu %lu %lu %lu %lu %lu %lu %lu %d %*s", &cpuSnap->index, \
					&cpuSnap->cpu_user, &cpuSnap->cpu_nice, &cpuSnap->cpu_system, &cpuSnap->cpu_idle, \
			   		&cpuSnap->cpu_iowait, &cpuSnap->cpu_irq, &cpuSnap->cpu_sirq, &cpuSnap->pnum);
	if(nRslt <= 0)
	{
		return 0;
	}
	strcpy(strLine, line);
	for(i = 0, str1 = strLine; i < 9; i++, str1 = NULL)
	{
		token = strtok_r(str1, " ", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return 0;
		}
	}
	for(i = 0; i < cpuSnap->pnum && i < MAX_PROCESS_NUM; i++)
	{
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		cpuSnap->p[i].pid = atol(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		strncpy(cpuSnap->p[i].name, token, 16);
		cpuSnap->p[i].name[15] = 0;
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		cpuSnap->p[i].utime = atol(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		cpuSnap->p[i].stime = atol(token);
	}
	if(nRslt <= 0 || (EOF == nRslt))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

BOOL loadCpuSnapDataLine(FILE *fp1, FILE *fp2, const time_t tTime, char ** lineptr)
{
	return loadResSnapDataLine(fp1, fp2, tTime, lineptr);
}

// Load the data 
BOOL loadCpuInfo(CpuInfoRecordType *arrCpuSnapPtr, int *nArrSizePtr, const long tInterval, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	time_t tBegTime = 0, tEndTime = 0;
	time_t tLineTime = 0;
	struct tm tmTmp;
//	CpuInfoRecordType * arrCpuSnap = (* arrCpuSnapPtr);
	CpuInfoRecordType * arrCpuSnap = ( arrCpuSnapPtr);
	char zLines[RESMGR_SHOW_RES_MAX * CPUMGR_RCD_LINE_MAX];
	char *zLine = NULL;
	int bRslt = TRUE;
	int i = 0;
	int nArrSize = 0;

	strptime(strBegTime, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tBegTime = mktime(&tmTmp);
	strptime(strEndTime, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tEndTime = mktime(&tmTmp);
	(*nArrSizePtr) = 0;

	// open files
	fp1 = fopen(zFilePath1, "r");
	if (fp1 == NULL)
	{
		return FALSE;
	}
	fp2 = fopen(zFilePath2, "r");
	if (fp2 == NULL)
	{
		// return FALSE;
	}
	i = 0;
	tLineTime = tBegTime;

	while(bRslt && (tLineTime <= tEndTime) && (i < RESMGR_SHOW_RES_MAX))
	{
		if(!loadCpuSnapDataLine(fp1, fp2, tLineTime, &zLine))
		{
			bRslt = FALSE;
			break;
		}
		strncpy(zLines+(i*CPUMGR_RCD_LINE_MAX), zLine, CPUMGR_RCD_LINE_MAX);
		zLines[i*CPUMGR_RCD_LINE_MAX + CPUMGR_RCD_LINE_MAX - 1] = 0;
		tLineTime += tInterval;
		i++;
	}
	nArrSize = i;

	if(fp1)
	{
		fclose(fp1);
		fp1 = NULL;
	}
	if(fp2)
	{
		fclose(fp2);
		fp2 = NULL;
	}
	for(i = 0; i < nArrSize && bRslt; i++)
	{
		bRslt &= getCpuSnapData(&arrCpuSnap[i], zLines+i*CPUMGR_RCD_LINE_MAX);
	}
	if(zLine)
	{
		free(zLine);
		zLine = NULL;
	}
	(*nArrSizePtr) = nArrSize;
	return bRslt;
}

// Load the data 
BOOL loadProcInfo(CpuInfoRecordType *cpuSnapBeg, CpuInfoRecordType *cpuSnapEnd, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	time_t tBegTime = 0, tEndTime = 0;
	struct tm tmTmp;
	char *zLineBeg = NULL;
	char *zLineEnd = NULL;
	int bRslt = TRUE;
	
	strptime(strBegTime, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tBegTime = mktime(&tmTmp);
	strptime(strEndTime, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tEndTime = mktime(&tmTmp);
	// open files
	fp1 = fopen(zFilePath1, "r");
	if (fp1 == NULL)
	{
		return FALSE;
	}
	fp2 = fopen(zFilePath2, "r");
	if (fp2 == NULL)
	{
		// We allow we can't find data file 2
		// return FALSE;
	}
	if(bRslt)
	{
		if(!loadCpuSnapDataLine(fp1, fp2, tBegTime, &zLineBeg))
		{
			bRslt = FALSE;
		}
	}
	if(bRslt)
	{
		if(!loadCpuSnapDataLine(fp1, fp2, tEndTime, &zLineEnd))
		{
			bRslt = FALSE;
		}
	}

	if(fp1)
	{
		fclose(fp1);
		fp1 = NULL;
	}
	if(fp2)
	{
		fclose(fp2);
		fp2 = NULL;
	}
	if(bRslt)
	{
		bRslt &= getProcSnapData(cpuSnapBeg, zLineBeg);
		bRslt &= getProcSnapData(cpuSnapEnd, zLineEnd);
	}
	if(zLineBeg)
	{
		free(zLineBeg);
		zLineBeg = NULL;
	}
	if(zLineEnd)
	{
		free(zLineEnd);
		zLineEnd = NULL;
	}
	return bRslt;
}

BOOL computeCpuInfo(CpuInfoRcdRatioType * const ratioRslt, const CpuInfoRecordType *cpuSnapBeg, const CpuInfoRecordType *cpuSnapEnd)
{
	ratioRslt->index 			= cpuSnapEnd->index;
	ratioRslt->jeffiesDuration 	= getCpuUsageSum(cpuSnapEnd, cpuSnapBeg);
	ratioRslt->jeffies_cpu_user = cpuSnapEnd->cpu_user - cpuSnapBeg->cpu_user;
	ratioRslt->jeffies_cpu_nice = cpuSnapEnd->cpu_nice - cpuSnapBeg->cpu_nice;
	ratioRslt->jeffies_cpu_system= cpuSnapEnd->cpu_system - cpuSnapBeg->cpu_system;
	ratioRslt->jeffies_cpu_idle = cpuSnapEnd->cpu_idle - cpuSnapBeg->cpu_idle;
	ratioRslt->jeffies_cpu_iowait= cpuSnapEnd->cpu_iowait - cpuSnapBeg->cpu_iowait;
	ratioRslt->jeffies_cpu_irq 	= cpuSnapEnd->cpu_irq - cpuSnapBeg->cpu_irq;
	ratioRslt->jeffies_cpu_sirq = cpuSnapEnd->cpu_sirq - cpuSnapBeg->cpu_sirq;
	ratioRslt->cpu_user 		= (float)ratioRslt->jeffies_cpu_user/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_nice 		= (float)ratioRslt->jeffies_cpu_nice/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_system 		= (float)ratioRslt->jeffies_cpu_system/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_idle 		= (float)ratioRslt->jeffies_cpu_idle/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_iowait 		= (float)ratioRslt->jeffies_cpu_iowait/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_irq 			= (float)ratioRslt->jeffies_cpu_irq/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_sirq 		= (float)ratioRslt->jeffies_cpu_sirq/ratioRslt->jeffiesDuration;

#ifdef _DEBUG_RES_MGR_TRACE_ON_
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_user, ratioRslt->jeffies_cpu_user, ratioRslt->jeffiesDuration);
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_nice, ratioRslt->jeffies_cpu_nice, ratioRslt->jeffiesDuration);
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_system, ratioRslt->jeffies_cpu_system, ratioRslt->jeffiesDuration);
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_idle, ratioRslt->jeffies_cpu_idle, ratioRslt->jeffiesDuration);
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_iowait, ratioRslt->jeffies_cpu_iowait, ratioRslt->jeffiesDuration);
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_irq, ratioRslt->jeffies_cpu_irq, ratioRslt->jeffiesDuration);
	aos_rhc_mgr_illegal_num_trace(ratioRslt->cpu_sirq, ratioRslt->jeffies_cpu_sirq, ratioRslt->jeffiesDuration);
#endif

	return TRUE;
}

// Compute the result
BOOL computeCpuInfo(CpuInfoRcdRatioType * const arrRatioRslt, const CpuInfoRecordType *arrCpuSnap, const int nArrSize)
{
	int i = 0;
	BOOL bRslt = TRUE;
	for(i = 0; i < nArrSize - 1; i++)
	{
		bRslt &= computeCpuInfo(&arrRatioRslt[i], &arrCpuSnap[i], &arrCpuSnap[i+1]);
	}
	return bRslt;
}

// Show the result
BOOL showCpuInfo(char * const local, const CpuInfoRcdRatioType *arrRatioRslt, const int nArrSize, const int nBufSize)
{
	//  Show the cpu usage rate of every event
	int i = 0; 
	struct tm *pTm;
	char zDateTime[20];

	sprintf(local+strlen(local),
		"\n   host local time| total |  user |  nice | system |  idle | iowait |  irq |  sirq\n");
	for(i = 0; i < nArrSize; i++)
	{
		pTm = localtime(&arrRatioRslt[i].index);
		strftime(zDateTime, sizeof(zDateTime), RESMGR_TIME_FMT,pTm);
		sprintf(local+strlen(local),"%18s|",zDateTime);
		sprintf(local+strlen(local),"%6lu |",arrRatioRslt[i].jeffiesDuration);
		sprintf(local+strlen(local),"%6lu |", arrRatioRslt[i].jeffies_cpu_user );
		sprintf(local+strlen(local),"%6lu |", arrRatioRslt[i].jeffies_cpu_nice );
		sprintf(local+strlen(local),"%7lu |", arrRatioRslt[i].jeffies_cpu_system );
		sprintf(local+strlen(local),"%6lu |", arrRatioRslt[i].jeffies_cpu_idle );
		sprintf(local+strlen(local),"%7lu |", arrRatioRslt[i].jeffies_cpu_iowait );
		sprintf(local+strlen(local),"%5lu |", arrRatioRslt[i].jeffies_cpu_irq );
		sprintf(local+strlen(local),"%6lu\n", arrRatioRslt[i].jeffies_cpu_sirq );
		sprintf(local+strlen(local),"\t\t  |");
		sprintf(local+strlen(local),"  100%% |");
		sprintf(local+strlen(local),"%5.1f%% |", 100*(float)arrRatioRslt[i].cpu_user);
		sprintf(local+strlen(local),"%5.1f%% |", 100*(float)arrRatioRslt[i].cpu_nice);
		sprintf(local+strlen(local),"%6.1f%% |", 100*(float)arrRatioRslt[i].cpu_system);
		sprintf(local+strlen(local),"%5.1f%% |", 100*(float)arrRatioRslt[i].cpu_idle);
		sprintf(local+strlen(local),"%6.1f%% |", 100*(float)arrRatioRslt[i].cpu_iowait);
		sprintf(local+strlen(local),"%4.1f%% |", 100*(float)arrRatioRslt[i].cpu_irq);
		sprintf(local+strlen(local),"%5.1f%%\n", 100*(float)arrRatioRslt[i].cpu_sirq);

//		if(strlen(local) > nBufSize - MAX_CMDLINE*2)
//		{
//			sprintf(local+strlen(local), RESMGR_BUF_EXPLOD_MSG_FMT, "cpu mgr show cpu");
//			break;
//		}
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// Compute the result
BOOL computeProcInfo(ProcCpuInfoRatioType *ratioRslt, const CpuInfoRecordType *cpuSnapBeg, const CpuInfoRecordType *cpuSnapEnd)
{
	int i = 0;
	int j = 0;

	ratioRslt->jeffiesDuration 	= getCpuUsageSum(cpuSnapEnd, cpuSnapBeg);
	ratioRslt->jeffies_cpu_user 	= cpuSnapEnd->cpu_user - cpuSnapBeg->cpu_user;
	ratioRslt->jeffies_cpu_nice 	= cpuSnapEnd->cpu_nice - cpuSnapBeg->cpu_nice;
	ratioRslt->jeffies_cpu_system= cpuSnapEnd->cpu_system - cpuSnapBeg->cpu_system;
	ratioRslt->jeffies_cpu_idle 	= cpuSnapEnd->cpu_idle - cpuSnapBeg->cpu_idle;
	ratioRslt->jeffies_cpu_iowait= cpuSnapEnd->cpu_iowait - cpuSnapBeg->cpu_iowait;
	ratioRslt->jeffies_cpu_irq 	= cpuSnapEnd->cpu_irq - cpuSnapBeg->cpu_irq;
	ratioRslt->jeffies_cpu_sirq 	= cpuSnapEnd->cpu_sirq - cpuSnapBeg->cpu_sirq;
	ratioRslt->cpu_user 			= (float)ratioRslt->jeffies_cpu_user/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_nice 			= (float)ratioRslt->jeffies_cpu_nice/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_system 		= (float)ratioRslt->jeffies_cpu_system/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_idle 			= (float)ratioRslt->jeffies_cpu_idle/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_iowait 		= (float)ratioRslt->jeffies_cpu_iowait/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_irq 			= (float)ratioRslt->jeffies_cpu_irq/ratioRslt->jeffiesDuration;
	ratioRslt->cpu_sirq 			= (float)ratioRslt->jeffies_cpu_sirq/ratioRslt->jeffiesDuration;

	ratioRslt->pnum = 0;
	for(i = 0; i < cpuSnapBeg->pnum ; i++)
	{
		for(j = 0; j < cpuSnapEnd->pnum; j++)
		{
			if(cpuSnapBeg->p[i].pid == cpuSnapEnd->p[j].pid)
			{
				ratioRslt->p[ratioRslt->pnum].pid 			= cpuSnapEnd->p[j].pid;
				strncpy(ratioRslt->p[ratioRslt->pnum].name, cpuSnapEnd->p[j].name, 16);
				ratioRslt->p[ratioRslt->pnum].utime 		= cpuSnapEnd->p[j].utime - cpuSnapBeg->p[i].utime;
				ratioRslt->p[ratioRslt->pnum].stime 		= cpuSnapEnd->p[j].stime - cpuSnapBeg->p[i].stime;
				ratioRslt->p[ratioRslt->pnum].using_ratio 	= 100*(float)
					(cpuSnapEnd->p[j].utime - cpuSnapBeg->p[i].utime + cpuSnapEnd->p[j].stime - cpuSnapBeg->p[i].stime) / ratioRslt->jeffiesDuration;
				ratioRslt->pnum++;

				break;
			}
		}
	}

	// sort the statistics result by using_ratio
	{
		int nSize = ratioRslt->pnum;
		int i = 0, j = 0;
		struct ProcessEntryRatio proc_entry_tmp;
		for(i = 0; i < nSize; i++)
		{
			for(j = i + 1; j < nSize; j++)
			{
				if(ratioRslt->p[i].using_ratio < ratioRslt->p[j].using_ratio)
				{
					// doing swap
					proc_entry_tmp = ratioRslt->p[i];
					ratioRslt->p[i] = ratioRslt->p[j];
					ratioRslt->p[j] = proc_entry_tmp;
				}
			}
		}
	}
	return TRUE;
}

// Show the result
BOOL showProcInfo(const ProcCpuInfoRatioType *ratioRslt, char * const local, const int nTopProcNum, const int nBufSize)
{
	int j = 0;

	//  Show the process usage rate of every event
	// print each process information
	sprintf(local+strlen(local),"\nprocess ID|   process NAME   | User Time | System Time | CPU using Rate\n");
	for(j = 0; (j < ratioRslt->pnum && j < nTopProcNum); j++)
	{
		sprintf(local+strlen(local),"%8ld  |",ratioRslt->p[j].pid);
		sprintf(local+strlen(local),"%16s  |",ratioRslt->p[j].name);
		sprintf(local+strlen(local),"%9lu  |",ratioRslt->p[j].utime);
		sprintf(local+strlen(local),"%11lu  |",ratioRslt->p[j].stime);
		sprintf(local+strlen(local),"  %5.1f%%\n",ratioRslt->p[j].using_ratio);
//		if(strlen(local) > nBufSize - MAX_CMDLINE*2)
//		{
//			sprintf(local+strlen(local), RESMGR_BUF_EXPLOD_MSG_FMT, "cpu mgr show proc");
//			break;
//		}
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//=================================================================
// Simple for API begin
//=================================================================
// Show the result
BOOL showCpuInfoAPI(char * const local, const CpuInfoRcdRatioType *arrRatioRslt, const int nArrSize, const int nBufSize)
{
	aos_cpumgr_stat_rslt_t arr_stat_rslt[256];
	for(int i = 0; i < nArrSize; i++)
	{
		arr_stat_rslt[i].m_sec_index = arrRatioRslt[i].index;
		arr_stat_rslt[i].m_jeffiesDuration = arrRatioRslt[i].jeffiesDuration;
		arr_stat_rslt[i].m_cpu_usage = (1 - arrRatioRslt[i].cpu_idle); 
		arr_stat_rslt[i].m_cpu_user = 	arrRatioRslt[i].cpu_user	;       
		arr_stat_rslt[i].m_cpu_nice = 	arrRatioRslt[i].cpu_nice	;       
		arr_stat_rslt[i].m_cpu_system = arrRatioRslt[i].cpu_system	;     
		arr_stat_rslt[i].m_cpu_idle = 	arrRatioRslt[i].cpu_idle	;       
		arr_stat_rslt[i].m_cpu_iowait = arrRatioRslt[i].cpu_iowait	;     
		arr_stat_rslt[i].m_cpu_irq = 	arrRatioRslt[i].cpu_irq		;        
		arr_stat_rslt[i].m_cpu_sirq = 	arrRatioRslt[i].cpu_sirq	;      
		arr_stat_rslt[i].m_jeffies_cpu_user = 	arrRatioRslt[i].jeffies_cpu_user   ;
		arr_stat_rslt[i].m_jeffies_cpu_nice = 	arrRatioRslt[i].jeffies_cpu_nice   ;
		arr_stat_rslt[i].m_jeffies_cpu_system = arrRatioRslt[i].jeffies_cpu_system ;
		arr_stat_rslt[i].m_jeffies_cpu_idle = 	arrRatioRslt[i].jeffies_cpu_idle   ;
		arr_stat_rslt[i].m_jeffies_cpu_iowait = arrRatioRslt[i].jeffies_cpu_iowait ;
		arr_stat_rslt[i].m_jeffies_cpu_irq = 	arrRatioRslt[i].jeffies_cpu_irq    ;
		arr_stat_rslt[i].m_jeffies_cpu_sirq = 	arrRatioRslt[i].jeffies_cpu_sirq   ;
	}
	if(0 == aos_resmgr_xml_gen_cpu_stat_rslt(local, nBufSize, arr_stat_rslt, nArrSize))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Show the result
BOOL showProcInfoAPI(const ProcCpuInfoRatioType *ratioRslt, char * const local, const int nTopProcNum, const int nBufSize)
{
	int j = 0;

	//  Show the process usage rate of every event
	// print each process information
	// sprintf(local+strlen(local),"\nprocess ID|   process NAME   | User Time | System Time | CPU using Rate\n");
	for(j = 0; (j < ratioRslt->pnum && j < nTopProcNum); j++)
	{
		sprintf(local+strlen(local), "%8ld ",ratioRslt->p[j].pid);
		sprintf(local+strlen(local), "%16s ",ratioRslt->p[j].name);
		sprintf(local+strlen(local), "%9lu ",ratioRslt->p[j].utime);
		sprintf(local+strlen(local),"%11lu ",ratioRslt->p[j].stime);
		sprintf(local+strlen(local),"%1.4f\n",(float)(ratioRslt->p[j].using_ratio/100));

		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}
//=================================================================
// Simple for API end
//=================================================================
