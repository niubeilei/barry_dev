////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrToolkit.c
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "aosMemMgrToolkit.h"
#include "aosResMgrToolkit.h"

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
// Tool functions for Memory Manager 
//===========================================================================================================
BOOL getMemProcSnapData(MemInfoRecordType * const memSnap, const char *line)
{
	int nRslt = 0;
	char *str1, *token = NULL;
	char *saveptr = NULL;
	char strLine[MEMMGR_LOCALBUFFER_MAX];
	int i = 0;
	const char strDelim[3] = " \n";

	nRslt = sscanf(line, "%lu %s %s %s %s %s %s %s %s %s %d %*s", &memSnap->index, \
					memSnap->total, memSnap->free, memSnap->buffer, \
					memSnap->cached, memSnap->swapcached, \
					memSnap->active, memSnap->inactive, \
			   		memSnap->swaptotal, memSnap->swapfree, \
			   		&memSnap->pnum);
	if(nRslt <= 0)
	{
		return 0;
	}
	strcpy(strLine, line);
	for(i = 0, str1 = strLine; i < 11; i++, str1 = NULL)
	{
		token = strtok_r(str1, " ", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return 0;
		}
	}
	// value format = "%lu %d %d %d %d %d %d"
	for(i = 0; i < memSnap->pnum && i < MAX_PROCESS_NUM; i++)
	{
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].pid = atol(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].proc_mem_size = atoi(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].proc_mem_resident = atoi(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].proc_mem_share = atoi(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].proc_mem_text = atoi(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].proc_mem_lib = atoi(token);
		
		token = strtok_r(NULL, strDelim, &saveptr);
		if(NULL == token)
			break;
		memSnap->p[i].proc_mem_data = atoi(token);
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


BOOL getMemSnapData(MemInfoRecordType * const memSnap, const char *line)
{
	const char *zFormat = "%lu %s %s %s %s %s %s %s";
	int nRslt = 0;
	nRslt = sscanf(line, zFormat, 
					&(memSnap->index),
					&(memSnap->total), &(memSnap->free), &(memSnap->buffer), 
					&(memSnap->active), &(memSnap->inactive), 
					&(memSnap->swaptotal), &(memSnap->swapfree));
	if(EOF == nRslt)	// assert(-1 == EOF)
	{
		return FALSE;
	}
	else if(nRslt <= 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

// Load the data 
BOOL loadMemInfo(MemInfoRecordType * arrMemSnapPtr, int *nArrSizePtr, const long tInterval, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	time_t tBegTime = 0, tEndTime = 0;
	time_t tLineTime = 0;
	struct tm tmTmp;
	MemInfoRecordType * arrMemSnap = ( arrMemSnapPtr);
	char zLines[RESMGR_SHOW_RES_MAX * MEMMGR_RCD_LINE_MAX];
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

	while(bRslt && (tLineTime < tEndTime) && (i < RESMGR_SHOW_RES_MAX))
	{
		if(!loadResSnapDataLine(fp1, fp2, tLineTime, &zLine))
		{
			bRslt = FALSE;
			break;
		}
#ifdef _DEBUG_
	printf("Line:\n%s \n(FILE:%s)(LINE:%d)\n", zLine, __FILE__, __LINE__); 
	printf("LineLen:\n[%d] \n(FILE:%s)(LINE:%d)\n", strlen(zLine), __FILE__, __LINE__); 
#endif
		strncpy(zLines+(i*MEMMGR_RCD_LINE_MAX), zLine, MEMMGR_RCD_LINE_MAX);
		zLines[i*MEMMGR_RCD_LINE_MAX + MEMMGR_RCD_LINE_MAX - 1] = 0;
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
		bRslt &= getMemSnapData(&arrMemSnap[i], zLines+i*MEMMGR_RCD_LINE_MAX);
	}
	if(zLine)
	{
		free(zLine);
		zLine = NULL;
	}
	(*nArrSizePtr) = nArrSize;
	return bRslt;
}

// Load the Process memory status data 
BOOL loadProcMemInfo(MemInfoRecordType * arrMemSnapPtr, int *nArrSizePtr, const long tInterval, const char *strBegTime, const char *strEndTime, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	time_t tBegTime = 0, tEndTime = 0;
	time_t tLineTime = 0;
	struct tm tmTmp;
	MemInfoRecordType * arrMemSnap = ( arrMemSnapPtr);
	char zLines[RESMGR_SHOW_RES_MAX * MEMMGR_RCD_LINE_MAX];
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

	while(bRslt && (tLineTime < tEndTime) && (i < RESMGR_SHOW_RES_MAX))
	{
		if(!loadResSnapDataLine(fp1, fp2, tLineTime, &zLine))
		{
			bRslt = FALSE;
			break;
		}
		strncpy(zLines+(i*MEMMGR_RCD_LINE_MAX), zLine, MEMMGR_RCD_LINE_MAX);
		zLines[i*MEMMGR_RCD_LINE_MAX + MEMMGR_RCD_LINE_MAX - 1] = 0;
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
		bRslt = getMemProcSnapData(&arrMemSnap[i], zLines+i*MEMMGR_RCD_LINE_MAX);
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
BOOL loadMemInfo(MemInfoRecordType *cpuSnapBeg, const char *strBegTime, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	time_t tBegTime = 0;
	struct tm tmTmp;
	char *zLineBeg = NULL;
	char *zLineEnd = NULL;
	int bRslt = TRUE;
	
	strptime(strBegTime, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tBegTime = mktime(&tmTmp);
	// open files
	fp1 = fopen(zFilePath1, "r");
	if (fp1 == NULL)
	{
		return FALSE;
	}
	fp2 = fopen(zFilePath2, "r");
	if (fp2 == NULL)
	{
		// We allow we can't find 2nd data file 
		// return FALSE;
	}
	if(bRslt)
	{
		if(!loadResSnapDataLine(fp1, fp2, tBegTime, &zLineBeg))
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
		bRslt &= getMemSnapData(cpuSnapBeg, zLineBeg);
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

// Show the result
BOOL showMemInfo(char * const local, const MemInfoRecordType *arrRatioRslt, const int nArrSize, const int nBufSize)
{
	//  Show the memory usage rate of every event
	int i = 0; 
	struct tm *pTm;
	char zDateTime[20];

	sprintf(local+strlen(local),
		"\n   host local time|idle ratio|   total  |   free   |  buffer  | active   | inactive | swaptotal| swapfree \n");
	for(i = 0; i < nArrSize; i++)
	{
		pTm = localtime(&arrRatioRslt[i].index);
		strftime(zDateTime, sizeof(zDateTime), RESMGR_TIME_FMT,pTm);
		sprintf(local+strlen(local),"%18s|",zDateTime);
		sprintf(local+strlen(local),"  %3.1f%%  |",100*(float)atol(arrRatioRslt[i].free)/atol(arrRatioRslt[i].total));
		sprintf(local+strlen(local),"%10s|", arrRatioRslt[i].total);
		sprintf(local+strlen(local),"%10s|", arrRatioRslt[i].free);
		sprintf(local+strlen(local),"%10s|", arrRatioRslt[i].buffer);
		sprintf(local+strlen(local),"%10s|", arrRatioRslt[i].active);
		sprintf(local+strlen(local),"%10s|", arrRatioRslt[i].inactive);
		sprintf(local+strlen(local),"%10s|", arrRatioRslt[i].swaptotal);
		sprintf(local+strlen(local),"%10s\n", arrRatioRslt[i].swapfree);
//		if(strlen(local) > nBufSize - MAX_CMDLINE*2)
//		{
//			sprintf(local+strlen(local), RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show mem");
//			break;
//		}
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// Compute each process entry result
BOOL computeProcMemEntry(struct ProcessMemEntryRatio * const objProcMemRslt, const struct ProcessMemEntry *objProcMemEntry, const time_t tIndex, const int nSysMemSize)
{
	const int nPageSize = MEMMGR_SYS_PAGE_SIZE;
	float tmp_ratio = 0;

	// initialize result value first
	reset_ProcessMemEntryRatio(objProcMemRslt);

	objProcMemRslt->pid = objProcMemEntry->pid;
	strcpy(objProcMemRslt->name, objProcMemEntry->name);
	/*data ratio: percentage of the result */
	/* The process used memory should be 
			memSnap->proc_mem_size * MEMMGR_SYS_PAGE_SIZE
	   The process used physical memory should be 
			memSnap->proc_mem_resident * MEMMGR_SYS_PAGE_SIZE
			===========design's formula is following=====
			(memSnap->proc_mem_size + memSnap->proc_mem_resident 
			+ memSnap->proc_mem_share + memSnap->proc_mem_text 
			+ memSnap->proc_mem_lib + memSnap->proc_mem_data)*MEMMGR_SYS_PAGE_SIZE	// 
			===========my implementation formula is following=====
			(memSnap->proc_mem_resident)*MEMMGR_SYS_PAGE_SIZE	// 
	 */
	tmp_ratio = 
			100*
				((float)(objProcMemEntry->proc_mem_resident * nPageSize)
				/nSysMemSize);

	// objProcMemRslt->using_ratio_peak = RESMGR_MAXIMUM(objProcMemRslt->using_ratio_peak, tmp_ratio);
	if(tmp_ratio > objProcMemRslt->using_ratio_peak || 0 == objProcMemRslt->using_ratio_peak)
	{
		objProcMemRslt->using_ratio_peak = tmp_ratio;
		objProcMemRslt->time_peak = tIndex;
#ifdef _DEBUG_
	printf("\n\nPeak Value:\ntmp_ratio[%f] using_ratio_peak[%f] index_peak(%ld) objProcMemEntry->pid(%d) FILE(%s)LINE(%d)\n", tmp_ratio, objProcMemRslt->using_ratio_peak,tIndex, objProcMemEntry->pid,  __FILE__, __LINE__ );
	printf("objProcMemEntry->proc_mem_size(%ld) objProcMemEntry->proc_mem_resident(%ld)"
					" objProcMemEntry->proc_mem_share(%ld) objProcMemEntry->proc_mem_text(%ld)"
					" objProcMemEntry->proc_mem_lib(%ld) objProcMemEntry->proc_mem_data(%ld)\n FILE(%s)LINE(%d)\n"
					, objProcMemEntry->proc_mem_size, objProcMemEntry->proc_mem_resident
					, objProcMemEntry->proc_mem_share, objProcMemEntry->proc_mem_text
					, objProcMemEntry->proc_mem_lib, objProcMemEntry->proc_mem_data
					, __FILE__, __LINE__ );
	printf("nTotalProcPage(%ld) nSysMemSize(%d)\n FILE(%s)LINE(%d)\n"
				, (objProcMemEntry->proc_mem_size + objProcMemEntry->proc_mem_resident 
					+ objProcMemEntry->proc_mem_share + objProcMemEntry->proc_mem_text 
					+ objProcMemEntry->proc_mem_lib + objProcMemEntry->proc_mem_data)
				,nSysMemSize				//,nPageSize
				, __FILE__, __LINE__ );
#endif
	}
	return TRUE;
}

// Compute each record memory result
BOOL computeProcMemInfoEntry(ProcMemInfoRatioType * const ratioRslt, const MemInfoRecordType *objMemSnap, const int TopProcNum)
{
	int i = 0, j = 0;
	const int nSysMemSize = atoi(objMemSnap->total)*MEMMGR_KB_SIZE;
	int nSize = 0;
	
	// initialize the result value
	reset_ProcMemInfoRatioType(ratioRslt);

	/* data ratio: percentage of the result */
	/* The total used memory should be MemTotal - ( MemFree + Buffers + Cached ) */
	ratioRslt->using_ratio_peak = 100*(float)
							(atoi(objMemSnap->total)
								 - atoi(objMemSnap->free) 
								 - atoi(objMemSnap->buffer) 
								 - atoi(objMemSnap->cached))
							/(atoi(objMemSnap->total));
	ratioRslt->index_peak = objMemSnap->index;

	// assert(ratioRslt->pnum != objMemSnap->pnum);
	for(i = 0; i < objMemSnap->pnum; i++)
	{
		for(j = 0; j < ratioRslt->pnum; j++)
		{
			if(ratioRslt->p[j].pid == objMemSnap->p[i].pid)
			{
				computeProcMemEntry(&ratioRslt->p[j], &objMemSnap->p[i], objMemSnap->index, nSysMemSize);
				break;
			}
		}
		if(j == ratioRslt->pnum)
		{
			if(ratioRslt->pnum < MAX_RSLT_PROCESS_NUM)
			{
				computeProcMemEntry(&ratioRslt->p[j], &objMemSnap->p[i], objMemSnap->index, nSysMemSize);
				ratioRslt->pnum++;
			}
			else // makes new process can be inside the array!!!! must lost some data !!
			{
				j = MAX_PROCESS_NUM;
				computeProcMemEntry(&ratioRslt->p[j], &objMemSnap->p[i], objMemSnap->index, nSysMemSize);
				ratioRslt->pnum = j + 1;
			}
		}
	}

	// sort the statistics result by using_ratio_peak
	{
		const int nSize = ratioRslt->pnum;
		int i = 0, j = 0;
		struct ProcessMemEntryRatio proc_entry_tmp;
		for(i = 0; i < nSize; i++)
		{
			for(j = i + 1; j < nSize; j++)
			{
				if(ratioRslt->p[i].using_ratio_peak < ratioRslt->p[j].using_ratio_peak)
				{
					// doing swap
					proc_entry_tmp = ratioRslt->p[i];
					ratioRslt->p[i] = ratioRslt->p[j];
					ratioRslt->p[j] = proc_entry_tmp;
				}
			}
		}
	}
	nSize = RESMGR_MINIMUM(TopProcNum, MAX_RSLT_PROCESS_NUM);
	for(i = 0; i < nSize; i++)
	{
		ratioRslt->p[i].nIntensiveCounter++;
	}
	return TRUE;
}

// Compute the total result in current array
BOOL computeProcMemInfo(ProcMemInfoRatioType * const ratioRsltPtr, const MemInfoRecordType *arrMemSnap, const int nArrSize, const int nTopProcNum)
{
	int i = 0;
	ProcMemInfoRatioType ratioRsltTmp;

	// initialize result value first
	reset_ProcMemInfoRatioType(ratioRsltPtr);
	for(i = 0; i < nArrSize; i++)
	{
		computeProcMemInfoEntry(&ratioRsltTmp, &arrMemSnap[i], nTopProcNum);
		append_ProcMemInfoRatioType(ratioRsltPtr, &ratioRsltTmp); 
	}

	return TRUE;
}

// Show the Process memory status result
BOOL showProcMemInfo(char * const local, const ProcMemInfoRatioType *ratioRslt, const char* strBegTime, const char *strEndTime, const int nInputTopProcNum, const int nBufSize)
{
	int j = 0;
	char zTime[20];
	const int nTopProcNum = RESMGR_MINIMUM(nInputTopProcNum, MAX_PROCESS_NUM);

	//  Show the process usage rate of every event
	// print top N process ratio information
	sprintf(local+strlen(local),"\nDescription: \nShow top %d intensive processes in system memory.\n", 
								nInputTopProcNum);
	{
		struct tm *pTm;
		time_t tTime = ratioRslt->index_peak;
		pTm = localtime(&tTime);	// for local time
		strftime(zTime, sizeof(zTime), "%Y%m%d-%H:%M:%S",pTm);
	}
	sprintf(local+strlen(local),"Begin Time: %s\nEnd Time  : %s\nPeak usage time : %s\n", strBegTime, strEndTime, zTime);
	sprintf(local+strlen(local),"Peak usage ratio: %5.1f%%\n", ratioRslt->using_ratio_peak);
	sprintf(local+strlen(local),"\nprocess ID| intensive times |         peak Time  | peak usage ratio\n");
	for(j = 0; (j < ratioRslt->pnum && j < nTopProcNum); j++)
	{
		sprintf(local+strlen(local),"%8lu  |",ratioRslt->p[j].pid);
		sprintf(local+strlen(local),"%15d  |",ratioRslt->p[j].nIntensiveCounter);
		sprintf(local+strlen(local),"%18s  |",getLocalTimeStr(RESMGR_TIME_FMT, ratioRslt->p[j].time_peak));
		sprintf(local+strlen(local),"  %3.3f%%\n",ratioRslt->p[j].using_ratio_peak);
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// reset ProcessMemEntry
void reset_ProcessMemEntry(struct ProcessMemEntry * const proc_mem_entry_ptr)
{
	proc_mem_entry_ptr->pid = 0;					/*pid:  pid of the process     */
	proc_mem_entry_ptr->name[0] = 0;
	proc_mem_entry_ptr->proc_mem_size 		= 0;	/*size: total program size     */
	proc_mem_entry_ptr->proc_mem_resident 	= 0;	/*resident: resident set size  */
	proc_mem_entry_ptr->proc_mem_share 		= 0;	/*share: shared pages          */
	proc_mem_entry_ptr->proc_mem_text 		= 0;	/*text: text (code)            */
	proc_mem_entry_ptr->proc_mem_lib 		= 0;	/*lib: library                 */
	proc_mem_entry_ptr->proc_mem_data 		= 0;	/*data: data/stack             */
}

// reset ProcessMemEntryRatio
void reset_ProcessMemEntryRatio(struct ProcessMemEntryRatio * const proc_mem_entry_ptr)
{
	proc_mem_entry_ptr->pid = 0;					/*pid:  pid of the process     */
	proc_mem_entry_ptr->name[0] = 0;
	proc_mem_entry_ptr->time_peak = 0;
	proc_mem_entry_ptr->using_ratio_peak = 0;		/*data ratio: percentage of the result */
	proc_mem_entry_ptr->nIntensiveCounter = 0;		/*intensive value:  */
}

void reset_ProcMemInfoRatioType(ProcMemInfoRatioType * const proc_mem_ratio_ptr)
{
	proc_mem_ratio_ptr->index_peak = 0;
	proc_mem_ratio_ptr->using_ratio_peak = 0;		/*data ratio: percentage of the result */
	proc_mem_ratio_ptr->pnum = 0;
}

void append_ProcMemInfoRatioType(ProcMemInfoRatioType * const rslt_proc_mem_ratio_ptr, 
								const ProcMemInfoRatioType * src_proc_mem_ratio_ptr)
{
	int i = 0, j = 0;

	/*data ratio: percentage of the result */
	// RESMGR_MAXIMUM(rslt_proc_mem_ratio_ptr->using_ratio_peak, src_proc_mem_ratio_ptr->using_ratio_peak);
	if(src_proc_mem_ratio_ptr->using_ratio_peak > rslt_proc_mem_ratio_ptr->using_ratio_peak)
	{
		rslt_proc_mem_ratio_ptr->using_ratio_peak = src_proc_mem_ratio_ptr->using_ratio_peak;
		rslt_proc_mem_ratio_ptr->index_peak = src_proc_mem_ratio_ptr->index_peak;
#ifdef _DEBUG_
	printf( "\n\n\nPeak Value:\nindex_peak(%ld) using_ratio_peak(%5.1f)"
			"\nFILE(%s)LINE(%d)\n"
							, rslt_proc_mem_ratio_ptr->index_peak, rslt_proc_mem_ratio_ptr->using_ratio_peak
							, __FILE__, __LINE__ );
#endif
	}
	for(i = 0; i < src_proc_mem_ratio_ptr->pnum; i++)
	{
		for(j = 0; j < rslt_proc_mem_ratio_ptr->pnum; j++)
		{
			if(rslt_proc_mem_ratio_ptr->p[j].pid == src_proc_mem_ratio_ptr->p[i].pid)
			{
				rslt_proc_mem_ratio_ptr->p[j].nIntensiveCounter += src_proc_mem_ratio_ptr->p[i].nIntensiveCounter;
				break;
			}
		}
		// if not find the same process in the total result, 
		// 		then append it to the end of the array
		if(j == rslt_proc_mem_ratio_ptr->pnum)
		{
			if(rslt_proc_mem_ratio_ptr->pnum < MAX_RSLT_PROCESS_NUM)
			{
				rslt_proc_mem_ratio_ptr->p[j] = src_proc_mem_ratio_ptr->p[i];
				rslt_proc_mem_ratio_ptr->pnum++;
			}
			else // makes new process can be inside the array!!!! must lost some data !!
			{
				j = MAX_PROCESS_NUM;
				rslt_proc_mem_ratio_ptr->p[j] = src_proc_mem_ratio_ptr->p[i];
				rslt_proc_mem_ratio_ptr->pnum = MAX_PROCESS_NUM + 1;
			}
		}
	}
}

void sort_ProcMemInfoRatioType(ProcMemInfoRatioType * const rslt_proc_mem_ratio_ptr)
{
	// sort the statistics result by nIntensiveCounter
	const int nSize = rslt_proc_mem_ratio_ptr->pnum;
	int i = 0, j = 0;
	struct ProcessMemEntryRatio proc_entry_tmp;

	for(i = 0; i < nSize; i++)
	{
		for(j = i + 1; j < nSize; j++)
		{
			if(rslt_proc_mem_ratio_ptr->p[i].nIntensiveCounter < rslt_proc_mem_ratio_ptr->p[j].nIntensiveCounter)
			{
				// doing swap
				proc_entry_tmp = rslt_proc_mem_ratio_ptr->p[i];
				rslt_proc_mem_ratio_ptr->p[i] = rslt_proc_mem_ratio_ptr->p[j];
				rslt_proc_mem_ratio_ptr->p[j] = proc_entry_tmp;
			}
		}
	}
}

//=================================================================
// Simple for API begin
//=================================================================
// Show the result
BOOL showMemInfoAPI(char * const local, const MemInfoRecordType *arrRatioRslt, const int nArrSize, const int nBufSize)
{
	//  Show the memory usage rate of every event
	int i = 0; 

	//host local time|idle ratio|   total  |   free   |  buffer  | cached  | active   | inactive | swaptotal| swapfree
	for(i = 0; i < nArrSize; i++)
	{
		sprintf(local+strlen(local),"%ld ",	 arrRatioRslt[i].index);
		sprintf(local+strlen(local),"%1.4f ",(float)atol(arrRatioRslt[i].free)/atol(arrRatioRslt[i].total));
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].total);
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].free);
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].buffer);
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].cached);
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].active);
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].inactive);
		sprintf(local+strlen(local),"%10s ", arrRatioRslt[i].swaptotal);
		sprintf(local+strlen(local),"%10s\n", arrRatioRslt[i].swapfree);
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// Show the Process memory status result
BOOL showProcMemInfoAPI(char * const local, const ProcMemInfoRatioType *ratioRslt, const char* strBegTime, const char *strEndTime, const int nInputTopProcNum, const int nBufSize)
{
	int j = 0;
	char zTime[20];
	const int nTopProcNum = RESMGR_MINIMUM(nInputTopProcNum, MAX_PROCESS_NUM);

	//  Show the process usage rate of every event
	// print top N process ratio information
//	sprintf(local+strlen(local),"\nDescription: \nShow top %d intensive processes in system memory.\n", 
//								nInputTopProcNum);
	{
		struct tm *pTm;
		time_t tTime = ratioRslt->index_peak;
		pTm = localtime(&tTime);	// for local time
		strftime(zTime, sizeof(zTime), "%Y%m%d-%H:%M:%S",pTm);
	}
	sprintf(local+strlen(local),"%ld ", ratioRslt->index_peak);
	sprintf(local+strlen(local),"%1.4f ", (ratioRslt->using_ratio_peak/100));
	sprintf(local+strlen(local),"%d \n", nTopProcNum);
	// process ID| intensive times |         peak Time  | peak usage ratio 
	for(j = 0; (j < ratioRslt->pnum && j < nTopProcNum); j++)
	{
		sprintf(local+strlen(local),"%lu ",ratioRslt->p[j].pid);
		sprintf(local+strlen(local),"%d ",ratioRslt->p[j].nIntensiveCounter);
		sprintf(local+strlen(local),"%ld ",ratioRslt->p[j].time_peak);
		sprintf(local+strlen(local),"%1.4f\n",(ratioRslt->p[j].using_ratio_peak/100));
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
