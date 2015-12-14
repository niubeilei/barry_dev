////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrToolkit.h
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosMemMgrToolkit_h
#define aos_rhcUtil_aosMemMgrToolkit_h

#include "aosResMgrDefines.h"

#include <time.h>
#include <stdio.h>

//=============================================
// defines for Memory manager
//=============================================
// only for Mem module
#define MEMMGR_FULLPATH "./MemMgr"
#define MEMMGR_PROCNAME "MemMgr"
#define MEMMGR_RCD_LINE_MAX 16384

//#define CPULOG_FULLPATH "/var/log/aos_mem_log.0"
#define MEMMGR_LOCALBUFFER_MAX 10000
#define MEMMGR_MAX_LINE_LEN 256
#define MEMMGR_KB_SIZE 1024
#define MEMMGR_SYS_PAGE_SIZE (getpagesize())

// Process memory status load from file /proc/%ld/statm 
struct ProcessMemEntry 
{
	unsigned long pid;		/*pid:  pid of the process     */
	char 		name[16];

	// following data have the same column order in /proc/%ld/statm
	int proc_mem_size;		/*size: total program size     */
	int proc_mem_resident;	/*resident: resident set size  */
	int proc_mem_share;		/*share: shared pages          */
	int proc_mem_text;		/*text: text (code)            */
	int proc_mem_lib;		/*lib: library                 */
	int proc_mem_data;		/*data: data/stack             */
};

void reset_ProcessMemEntry(struct ProcessMemEntry * const proc_mem_entry_ptr);

struct MemInfoRecord 
{
	long index;
	char total[MEMMGR_MAX_LINE_LEN];
	char free[MEMMGR_MAX_LINE_LEN];
	char buffer[MEMMGR_MAX_LINE_LEN];
	char cached[MEMMGR_MAX_LINE_LEN];
	char swapcached[MEMMGR_MAX_LINE_LEN];
	char active[MEMMGR_MAX_LINE_LEN];
	char inactive[MEMMGR_MAX_LINE_LEN];
	char swaptotal[MEMMGR_MAX_LINE_LEN];
	char swapfree[MEMMGR_MAX_LINE_LEN];
	int pnum;
	struct ProcessMemEntry p[MAX_PROCESS_NUM];
};
#define MemInfoRecordType struct MemInfoRecord

struct ProcessMemEntryRatio 
{
	unsigned long pid;			/*pid:  pid of the process     */
	char 		name[16];
	time_t time_peak;
//	time_t time_valley;
	float using_ratio_peak;		/*data ratio: percentage of the result */
//	float using_ratio_valley;	/*data ratio: percentage of the result */
	int 	nIntensiveCounter;	/*intensive value:  */
};
void reset_ProcessMemEntryRatio(struct ProcessMemEntryRatio * const proc_mem_entry_ptr);

struct ProcMemInfoRatio
{
	long index_peak;
//	long index_valley;
	float using_ratio_peak;		/*data ratio: percentage of the result */
//	float using_ratio_valley;	/*data ratio: percentage of the result */
	int pnum;
	struct ProcessMemEntryRatio p[MAX_RSLT_PROCESS_NUM];
};
#define ProcMemInfoRatioType struct ProcMemInfoRatio

void reset_ProcMemInfoRatioType(ProcMemInfoRatioType * const proc_mem_ratio_ptr);
void append_ProcMemInfoRatioType(ProcMemInfoRatioType * const proc_mem_ratio_ptr, 
								const ProcMemInfoRatioType * proc_mem_ratio_ptr);
void sort_ProcMemInfoRatioType(ProcMemInfoRatioType * const rslt_proc_mem_ratio_ptr);

//=============================================

//=================================================================
// Functions for Memory Manager show
//=================================================================
// Compute the result
// int computeMemInfo(ProcCpuInfoRatioType *ratioRslt, const MemInfoRecordType *cpuSnapBeg, const MemInfoRecordType *cpuSnapEnd);
// Show the result
BOOL showMemInfo(char * const local, const MemInfoRecordType *arrRatioRslt, const int nArrSize, const int nBufSize);
// int computeProcInfo(ProcCpuInfoRatioType *ratioRslt, const MemInfoRecordType *cpuSnapBeg, const MemInfoRecordType *cpuSnapEnd);
// Show the Process memory status result
BOOL showProcMemInfo(char * const local, const ProcMemInfoRatioType *ratioRslt, const char* strBegTime, const char *strEndTime, const int nInputTopProcNum, const int nBufSize);
// Load the data 
BOOL loadMemInfo(MemInfoRecordType * arrMemSnapPtr, int *nArrSizePtr, const long tInterval, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2);
BOOL loadProcMemInfo(MemInfoRecordType * arrMemSnapPtr, int *nArrSizePtr, const long tInterval, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2);

BOOL getMemSnapData(MemInfoRecordType * const memSnap, const char *line); //, const char *zFormat = "%lu %s %s %s %s %s %s %s");
// Compute the result
BOOL computeProcMemInfo(ProcMemInfoRatioType * const ratioRsltPtr, const MemInfoRecordType *arrMemSnap, const int nArrSize, const int nTopProcNum);

//=================================================================
// Simple for API begin
//=================================================================
// Show the result
BOOL showMemInfoAPI(char * const local, const MemInfoRecordType *arrRatioRslt, const int nArrSize, const int nBufSize);
// Show the Process memory status result
BOOL showProcMemInfoAPI(char * const local, const ProcMemInfoRatioType *ratioRslt, const char* strBegTime, const char *strEndTime, const int nInputTopProcNum, const int nBufSize);
//=================================================================
// Simple for API end
//=================================================================

#endif //  aos_rhcUtil_aosMemMgrToolkit_h
