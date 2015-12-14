////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCpuMgrToolkit.h
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosCpuMgrToolkit_h
#define aos_rhcUtil_aosCpuMgrToolkit_h

#include "aosResMgrDefines.h"

#include <time.h>
#include <stdio.h>

// only for Cpu module 
#define CPUMGR_FULLPATH "./CpuMgr" 
#define CPUMGR_PROCNAME "CpuMgr" 
#define CPUMGR_LOCALBUFFER_MAX 10000 
#define CPUMGR_SHOW_PROC_MAX 50 
#define CPUMGR_SHOW_PROC_DEFAULT 10 
#define CPUMGR_TIME_LEN_MAX 32 
#define CPUMGR_RCD_LINE_MAX 8192 

/* Files */
#define CPU_INFO_FILE_0 "/var/log/aos_cpu_log.0"
#define CPU_INFO_FILE_1 "/var/log/aos_cpu_log.1"

#define PROC_STAT_FILE  "/proc/stat"

#define SA_DIR 		"/var/log/CPU"
#define PID_STAT 	"/proc/%ld/stat"

//=============================================
// defines for CPU manager
//=============================================
struct ProcessEntry 
{
   unsigned long pid;
   char		 name[16];
   unsigned long utime;
   unsigned long stime;
};

struct CpuInfoRecord 
{
   long index;
   unsigned long cpu_user;
   unsigned long cpu_nice;
   unsigned long cpu_system;
   unsigned long cpu_idle;
   unsigned long cpu_iowait;
   unsigned long cpu_irq;
   unsigned long cpu_sirq;
   int pnum;
   struct ProcessEntry p[MAX_PROCESS_NUM];
};

#define CpuInfoRecordType struct CpuInfoRecord

struct CpuInfoRcdRatio 
{
	long index;
	unsigned long jeffiesDuration;
	float cpu_user;
	float cpu_nice;
	float cpu_system;
	float cpu_idle;
	float cpu_iowait;
	float cpu_irq;
	float cpu_sirq;
	unsigned long jeffies_cpu_user;
	unsigned long jeffies_cpu_nice;
	unsigned long jeffies_cpu_system;
	unsigned long jeffies_cpu_idle;
	unsigned long jeffies_cpu_iowait;
	unsigned long jeffies_cpu_irq;
	unsigned long jeffies_cpu_sirq;
};

#define CpuInfoRcdRatioType struct CpuInfoRcdRatio

struct ProcessEntryRatio 
{
   unsigned long pid;
   char		 name[16];
   unsigned long utime;
   unsigned long stime;
   float using_ratio;
};

struct ProcCpuInfoRatio 
{
	unsigned long jeffiesDuration;
	float cpu_user;
	float cpu_nice;
	float cpu_system;
	float cpu_idle;
	float cpu_iowait;
	float cpu_irq;
	float cpu_sirq;
	unsigned long jeffies_cpu_user;
	unsigned long jeffies_cpu_nice;
	unsigned long jeffies_cpu_system;
	unsigned long jeffies_cpu_idle;
	unsigned long jeffies_cpu_iowait;
	unsigned long jeffies_cpu_irq;
	unsigned long jeffies_cpu_sirq;
	int pnum;
	struct ProcessEntryRatio p[MAX_PROCESS_NUM];
};

#define ProcCpuInfoRatioType struct ProcCpuInfoRatio

//=============================================

//=================================================================
// Functions for Cpu Manager show
//=================================================================
// get the cpu snap info from the Linux system
BOOL getCpuInfo(struct CpuInfoRecord *snap);

// save the cpu snap info to the data file
BOOL saveCpuInfo(const long index, const int maxr, const int interval, const long duration, struct CpuInfoRecord *snap);

// get the total jiefies of this time period
inline unsigned long getCpuUsageSum(const CpuInfoRecordType *CpuInfoSnapCur, const CpuInfoRecordType *CpuInfoSnapPre)
{
	return (	CpuInfoSnapCur->cpu_user-\
				CpuInfoSnapPre->cpu_user+\
				CpuInfoSnapCur->cpu_nice-\
				CpuInfoSnapPre->cpu_nice+\
				CpuInfoSnapCur->cpu_system-\
				CpuInfoSnapPre->cpu_system+\
				CpuInfoSnapCur->cpu_idle-\
				CpuInfoSnapPre->cpu_idle+\
				CpuInfoSnapCur->cpu_iowait-\
				CpuInfoSnapPre->cpu_iowait+\
				CpuInfoSnapCur->cpu_irq-\
				CpuInfoSnapPre->cpu_irq+\
				CpuInfoSnapCur->cpu_sirq-\
				CpuInfoSnapPre->cpu_sirq);
};

// Load the data 
BOOL loadProcInfo(CpuInfoRecordType * const cpuSnapBeg, CpuInfoRecordType * const cpuSnapEnd, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2);

// Compute the result
BOOL computeCpuInfo(CpuInfoRcdRatioType * const ratioRslt, const CpuInfoRecordType *cpuSnapBeg, const CpuInfoRecordType *cpuSnapEnd);
BOOL computeCpuInfo(CpuInfoRcdRatioType * const arrRatioRslt, const CpuInfoRecordType *arrCpuSnap, const int nArrSize);
// Show the result
BOOL showCpuInfo(char * const local, const CpuInfoRcdRatioType *arrRatioRslt, const int nArrSize, const int nBufSize);
// Compute the result
BOOL computeProcInfo(ProcCpuInfoRatioType * const ratioRslt, const CpuInfoRecordType *cpuSnapBeg, const CpuInfoRecordType *cpuSnapEnd);
// Show the result
BOOL showProcInfo(const ProcCpuInfoRatioType *ratioRslt, char * const local, const int nTopProcNum, const int nBufSize);
// Load the data 
BOOL loadCpuInfo(CpuInfoRecordType *arrCpuSnapPtr, int *nArrSize, const long tInterval, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2);

BOOL loadCpuSnapDataLine(FILE *fp1, FILE *fp2, const time_t tTime, char ** lineptr);

BOOL getCpuSnapData(CpuInfoRecordType * const cpuSnap, const char *line);

BOOL getProcSnapData(CpuInfoRecordType * const cpuSnap, const char *line);

//=================================================================
// Simple for API begin
//=================================================================
// Show the result
BOOL showCpuInfoAPI(char * const local, const CpuInfoRcdRatioType *arrRatioRslt, const int nArrSize, const int nBufSize);
// Show the result
BOOL showProcInfoAPI(const ProcCpuInfoRatioType *ratioRslt, char * const local, const int nTopProcNum, const int nBufSize);
//=================================================================
// Simple for API end
//=================================================================

//=================================================================

#endif //  aos_rhcUtil_aosCpuMgrToolkit_h
