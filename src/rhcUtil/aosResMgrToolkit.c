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

#include <time.h>

// ===================================================
// Log code copied from aosUtil/Tracer.cpp Start
// ===================================================
// For log
#include <stdarg.h>

// Write log
enum
{
	eAosLogNumLocalBuf = 8,
	eAosLogLocalBufSize = 1000
};

// 
// The caller should never modify the contents of the buffer
// returned by this function.
//
char *aos_res_mgr_log_get_msg(char *fmt, ...)
{
	static int  slBufIndex = 0;
	static char slBuffer[eAosLogNumLocalBuf][eAosLogLocalBufSize];
	unsigned int index = (slBufIndex++) & 0x07;

	va_list args;
	int n;

	va_start(args, fmt);
	n = vsprintf(slBuffer[index], fmt, args);
	va_end(args);

	return slBuffer[index];
}
// ===================================================
// Log code copied from aosUtil/Tracer.cpp End
// ===================================================
// get word
const char * get_Word(char * zDes, const char * strSrc)
{
	int nSize = 0, i = 0, nBegPos = -1, nEndPos = -1;

	if(NULL == strSrc || NULL == zDes)
	{
		return NULL;
	}

	zDes[0] = 0;

	nSize = strlen(strSrc);

	if(0 == nSize)
	{
		return NULL;
	}
	for(i = 0; i < nSize && nBegPos < 0; i++)
	{
		switch (strSrc[i])
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\0':
				break;
			default:
				nBegPos = i;
		}
	}
	if(nBegPos < 0)
	{
		return NULL;
	}

	for(i = nBegPos; i < nSize && nEndPos < 0; i++)
	{
		switch (strSrc[i])
		{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\0':
				nEndPos = i;
				break;
			default:;
		}
	}
	if(nEndPos < 0)
	{
		nEndPos = nSize;
	}
	strncpy(zDes, strSrc + nBegPos, nEndPos - nBegPos);
	zDes[nEndPos - nBegPos] = '\0';

	return (strSrc + nEndPos);
}

//////////////////////////////////////////////////////
// Math tool functions begin
// from : http://www.cppblog.com/Winux32/archive/2006/12/04/15954.html
//////////////////////////////////////////////////////
// get greatest common degree (最大公约数)
int GetGCDRec(int n, int m)
{
    if (m < n)
    {
        m ^= n;
        n ^= m;
        m ^= n;
    }

    if (n == 0)
    {
        return m;
    }
    else
    {
        return GetGCDRec(n, m % n);
    }
}

// get greatest common degree (最大公约数)(最小公倍数)
int GetGCD(int *arr, int len)
{
	int iMax = arr[0], iCurr, iRemainder;
	
	for(int i = 1; i < len; i++)
	{
		iCurr = arr[i];

		if(iMax < iCurr)
		{
			iMax ^= iCurr;
			iCurr ^= iMax;
			iMax ^= iCurr;
		}

		iRemainder = iMax % iCurr;

		while (iRemainder)
		{
			iMax = iCurr;
			iCurr = iRemainder;
			iRemainder = iMax % iCurr;
		}

	    iMax = iCurr;
	}
    return iMax;
}

// get lowest common multiplier (最小公倍数) 最小公倍数就是乘积除以最大公约数
int GetLCM(int *arr, int len)
{
	int multiple = 1;

	for (int i = 0; i < len; i++)
	{
	    multiple *= arr[i];
	}

	return multiple / GetGCD(arr, len);
}
//////////////////////////////////////////////////////
// Math tool functions end
//////////////////////////////////////////////////////

void aos_init_real_timer(void (*fn_exec)(int), unsigned int nSecInterval)
{
	// init action
	struct sigaction act; 
	act.sa_handler = (fn_exec); 
	act.sa_flags = 0; 
	sigemptyset(&act.sa_mask); 
	sigaction(SIGALRM,&act,NULL); 
	
	// init time
	struct itimerval value; 
	value.it_value.tv_sec = nSecInterval;
	value.it_value.tv_usec = 0; 
	value.it_interval = value.it_value; 
	setitimer(ITIMER_REAL,&value,NULL); 
}

void aos_do_nothing(int)
{
	return;
}

// According the input like "1H" "1M" translate to 
// 		the seconds of it
time_t getDuration(const char * zDurationTime)
{
	int i, nTmpSize;
	char zTmp[RESMGR_TIME_LEN_MAX];
	long lRslt = 0;

	memset(zTmp, 0, sizeof(zTmp));
	if(NULL == zDurationTime || strlen(zDurationTime) >= RESMGR_TIME_LEN_MAX)
	{
		return -eAosRc_ResMgrUnknownError;
	}
	strncpy(zTmp, zDurationTime, RESMGR_TIME_LEN_MAX - 1);

	nTmpSize = (int)sizeof(zTmp);
	for(i = 0; i < nTmpSize; i++) 
	{
		if (zTmp[i] > '9' || zTmp[i] < '0')
		{
			break;
		}
	}

	zTmp[i] = toupper(zTmp[i]);
	if (strcmp(zTmp+i, "M") == 0) 
	{
		zTmp[i] = '\0';
		lRslt = atol(zTmp) * 60;
	}
	else if (strcmp(zTmp+i, "H") == 0) 
	{
		zTmp[i] = '\0';
		lRslt = atol(zTmp) * 60 * 60;
	}
	else if (strcmp(zTmp+i, "D") == 0) 
	{
		zTmp[i] = '\0';
		lRslt = atol(zTmp) * 60 * 60 * 24;
	}
	else if (strcmp(zTmp+i, "S") == 0)  	// (zTmp[i] == 'S')
	{
		zTmp[i] = '\0';
		lRslt = atol(zTmp);
	}
	else if (zTmp[i] == 0)  	// (zTmp[i] == 'S')
	{
		zTmp[i] = '\0';
		lRslt = atol(zTmp);
	}
	else
	{
		return -eAosRc_ResMgrUnknownError;
	}

	return lRslt;
}

time_t aos_getFmtTimeSec(const char * zTimePos, const char * zTimeFmt)
{
	struct tm tmTime;
	if(!strptime(zTimePos, zTimeFmt, &tmTime))
	{
		return 0;
	}
	tmTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	return mktime(&tmTime);
}

// According the input like "1H" "1M" translate to 
// 		the seconds of it
// If no input zDuration time, the return value will be a default value which is set by formula
//  retval_intervalSec = (tEndTime - tBegTime)/30
// If input illegal, return 0
time_t aos_get_interval_sec(const char * zDurationTime, const char * zTimeBeg, const char * zTimeEnd)
{
	time_t tBegTime = 0, tEndTime = 0;
	time_t tRslt = 0;
	const char *zTimeFormat = "%Y%m%d-%H:%M:%S";

	tBegTime = aos_getFmtTimeSec(zTimeBeg, zTimeFormat);
	tEndTime = aos_getFmtTimeSec(zTimeEnd, zTimeFormat);
	if(tBegTime < 1 || tEndTime < 1)
	{
		// illegal input error!!!!
		return 0;
	}

	tRslt = getDuration(zDurationTime);
	if(tRslt < 1)
	{
		tRslt = (tEndTime - tBegTime)/RESMGR_SHOW_RES_DEFAULT;
	}

	if(tRslt < (tEndTime - tBegTime)/RESMGR_SHOW_RES_MAX)
	{
		// illegal input error!!!!
		return 0;
	}
	return tRslt;
}

// encapsulate the process of create a detached thread
void aos_uland_thread_create(pthread_t * tid_ptr, void (*function)(void*), void (*parameter), 
							 pthread_mutex_t * mutex_ptr)
{
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	pthread_mutex_init(mutex_ptr,NULL);/* 用默认属性初始化一个互斥锁对象 */
	pthread_create(tid_ptr, &attr,  (void*(*)(void*)) function, parameter);
}

// encapsulate the process of create a detached thread
void aos_uland_thread_create(pthread_t * tid_ptr, void (*function)(void*), void (*parameter), 
							 pthread_mutex_t * mutex_ptr,
							 pthread_cond_t * cond_kill_ptr, pthread_cond_t * cond_stopped_ptr, 
							 BOOL * status_ptr)
{
	(* status_ptr) = TRUE;
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE ); // PTHREAD_CREATE_DETACHED
	pthread_cond_init(cond_kill_ptr,NULL);/* 用默认属性初始化一个条件对象 */
	pthread_cond_init(cond_stopped_ptr,NULL);/* 用默认属性初始化一个条件对象 */
	pthread_mutex_init(mutex_ptr,NULL);/* 用默认属性初始化一个互斥锁对象 */
	pthread_create(tid_ptr, &attr,  (void*(*)(void*)) function, parameter);
}

// encapsulate the process of destroy a detached thread
void aos_uland_thread_destroy(pthread_mutex_t * mutex_ptr)
{
	pthread_mutex_destroy(mutex_ptr);		/* 销毁一个互斥锁对象 */
}

// encapsulate the process of destroy a detached thread
void aos_uland_thread_destroy(pthread_t * tid_ptr, pthread_mutex_t * mutex_ptr, pthread_cond_t * cond_kill_ptr, pthread_cond_t * cond_stopped_ptr, BOOL * status_ptr)
{
	int nRet;
	(*status_ptr) = FALSE;

	pthread_mutex_lock (mutex_ptr);
	pthread_cond_signal(cond_kill_ptr);
	nRet = pthread_cond_wait(cond_stopped_ptr, mutex_ptr);
	pthread_mutex_unlock (mutex_ptr);

	nRet = pthread_join((*tid_ptr), NULL);
	pthread_cond_destroy(cond_kill_ptr);	/* 销毁一个条件对象 */
	pthread_cond_destroy(cond_stopped_ptr);	/* 销毁一个条件对象 */
	pthread_mutex_destroy(mutex_ptr);		/* 销毁一个互斥锁对象 */
}

void aos_resmgr_sighand(int signo)
{
#ifdef _DEBUG_
	pthread_t             self = pthread_self();
	printf("Thread %lu in signal %d handler\n", self, signo);
#endif

	return;
}

/////////////////////////////////////////////////////////////
// Tool functions only for this area
/////////////////////////////////////////////////////////////

// Convert time from time_t to string
static char zLocalTimeStr[32];
char* getLocalTimeStr(const char *zFormat, const time_t t)
{
	struct tm *pTm;

	if(NULL == zFormat || strlen(zFormat) == 0)
	{
		return NULL;
	}

	// should have mutex section ---- begin
	pTm = localtime(&t);	// for local time
	// pTm = gmtime(&t);	// for gmt time
	strftime(zLocalTimeStr, 20, zFormat,pTm);
	// should have mutex section ---- end
	return zLocalTimeStr;
}

// Get current time
char* getLocalTimeStr(const char *zFormat)
{
	time_t t;
	t = time(NULL); // get current time 
	return getLocalTimeStr(zFormat, t);
}

BOOL getLocalTime(char *zTime, const int nStrSize, const char *zFormat)
{
	struct tm *pTm;
	time_t t;

	// zBuf = new char [20];
	if(NULL == zTime || NULL == zFormat || strlen(zFormat) == 0)
	{
		return FALSE;
	}
	t = time(NULL);

	// should have mutex section ---- begin
	pTm = localtime(&t);	// for local time
	// pTm = gmtime(&t);	// for gmt time
	strftime(zTime, nStrSize, zFormat,pTm);
	// should have mutex section ---- end
	return TRUE;
}

BOOL aos_get_pidof(char * const readbuf, const char* zProgramName)
{
	char zCmdline[MAX_CMDLINE];
	int fd[2];

	memset(zCmdline, 0, sizeof(zCmdline));
	memset(readbuf, 0, sizeof(readbuf));
	sprintf(zCmdline, "pidof %s", zProgramName);

	pid_t cpid;
	pipe(fd);
	cpid = fork();
	if (cpid == -1)
	{
		perror("fork");
		return FALSE;
//		exit(EXIT_FAILURE);
	}
	if (cpid == 0)	   /* Child write command */
	{
		close(fd[0]);
		dup2(fd[1], 1);
		system(zCmdline);
		return FALSE;
//		_exit(EXIT_SUCCESS);
	}
	else // if (pid > 0)   /* Parent reads command result from pipe */
	{
		close(fd[1]);
//		char* bufTmp = readbuf;
//		while (read(fd[0], &buf1, 1) > 0)
//		   write(STDOUT_FILENO, &buf1, 1);
//		read(fd[0], readbuf, sizeof(readbuf));
		read(fd[0], readbuf, RECORD_NUM);
		return TRUE;
//		_exit(EXIT_SUCCESS);
	}

//	read(STDOUT_FILENO, readbuf, sizeof(readbuf));
	return TRUE;
}

BOOL killSameProc(const char *zProgramName)
{
//	struct CpuInfoRecord CpuInfoSnap;
	char * zCmdline = (char *) malloc(sizeof(char)*MAX_CMDLINE);
	char * buff = (char *)malloc(sizeof(char)*RECORD_NUM);
	char * pidBuff = NULL;
	const pid_t curPid = getpid();
//	char * process_pattern = (char *)malloc(sizeof(char)*MAX_FNAME);

//	int i = 0, nRslt = 0;

	// 1. get all of the pid for this program_name
// Solution 1
//	if(!getCpuInfo(&CpuInfoSnap))
//	{
//		return FALSE;
//	}
// Solution 2
	if(!aos_get_pidof(buff, zProgramName))
	{
		if(zCmdline)
			free(zCmdline);
		zCmdline = NULL;
		if(buff)
			free(buff);
		buff = NULL;
		return FALSE;
	}

	// 2. find the pid and kill the processes except itself
// Solution 1
//	for(i = 0; i < CpuInfoSnap.pnum; i++)
//	{
//		strcpy(process_pattern, "*");
//		strncat(process_pattern, CpuInfoSnap.p[i].name + (sizeof(char)), strlen(CpuInfoSnap.p[i].name) - 2*(sizeof(char)));
//		nRslt = fnmatch(process_pattern, proc_name, FNM_NOESCAPE);
//		if(0 == nRslt &&
//			curPid != CpuInfoSnap.p[i].pid)
//		{
//			sprintf(zCmdline, "kill -9 %ld", CpuInfoSnap.p[i].pid);
//			system(zCmdline);
//		}
//	}
//	if(process_pattern)
//		free(process_pattern);
//	process_pattern = NULL;
// Solution 2
	
	for(pidBuff = strtok(buff, " "); pidBuff != NULL; pidBuff = strtok(NULL, " "))
	{
		if(curPid != atoi(pidBuff))
		{
			memset(zCmdline, 0, sizeof(zCmdline));
			sprintf(zCmdline, "kill -9 %s", pidBuff);
			system(zCmdline);
		}
	}
	if(zCmdline)
		free(zCmdline);
	zCmdline = NULL;
	if(buff)
		free(buff);
	buff = NULL;
	
	return TRUE;
}

BOOL aos_kill_same_proc(const char *zProgramName)
{
	return killSameProc(zProgramName);
}

// Translate input time string to formatted time string
// from "now", "-1H", "2M" to "yyyymmdd-hh:MM:SS"
BOOL aos_getFmtTimeStr(char * const zDes, const char *zSrc)
{
	struct tm *pTm = NULL;
	struct tm tmTime;
	time_t tOffset = 0, tRslt = 0;
	const time_t tNowTime = time(NULL);

//	if(NULL == zDes || RESMGR_TIME_LEN_MAX > sizeof(zDes) || NULL == zSrc || strlen(zSrc) == 0)
	if(NULL == zDes || NULL == zSrc || strlen(zSrc) == 0)
	{
		return FALSE;
	}
	else if(strptime(zSrc, "%Y%m%d-%H:%M:%S", &tmTime))
	{
		strncpy(zDes, zSrc, RESMGR_TIME_LEN_MAX);
		zDes[RESMGR_TIME_LEN_MAX-1] = '\0';
		return TRUE;
	}

	if(strcasecmp(zSrc, "now") == 0)
	{
		tRslt = tNowTime - 1;
	}
	else if(zSrc[0] == '-')
	{
		tOffset = getDuration(zSrc+1);
		tRslt = tNowTime - tOffset;
	}
	else
	{
		tOffset = getDuration(zSrc);
		tRslt = tNowTime + tOffset;
	}

	pTm = localtime(&tRslt);	// for local time

	// pTm = gmtime(&tRslt);	// for gmt time
	strftime(zDes, RESMGR_TIME_LEN_MAX, "%Y%m%d-%T",pTm);

	return TRUE;
}

// Get one more minutes time string from formatted time string to formatted time string
// from "yyyymmdd-hh:MM:SS" to "yyyymmdd-hh:(MM):(SS++MAX_PROCESS_NUM*tIntervalTime)"
BOOL aos_get_next_computable_time_str(char *const zDes, const char *zSrc, const time_t tIntervalTime)
{
	struct tm tmTime;
	time_t tTime = 0;
	const char zTimeFmt[RESMGR_TIME_LEN_MAX] = "%Y%m%d-%H:%M:%S";

	if(NULL == zDes || NULL == zSrc) // || strlen(zSrc) == 0)
	{
		return FALSE;
	}
	if(NULL == strptime(zSrc, zTimeFmt, &tmTime))
	{
		return FALSE;
	}

	tmTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tTime = mktime(&tmTime);
	if(tTime < 0)
	{
		return FALSE;
	}
	tTime += MAX_PROCESS_NUM * tIntervalTime;
	tmTime = (*localtime(&tTime));
	
	strftime(zDes, RESMGR_TIME_LEN_MAX, zTimeFmt, &tmTime);
	return TRUE;
}

// Get one more minutes time string from formatted time string to formatted time string
// from "yyyymmdd-hh:MM:SS" to "yyyymmdd-hh:(MM):(SS+tIntervalTime)"
BOOL aos_get_next_interval_time_str(char *const zDes, const char *zSrc, const time_t tIntervalTime)
{
	struct tm tmTime;
	time_t tTime = 0;
	const char zTimeFmt[RESMGR_TIME_LEN_MAX] = "%Y%m%d-%H:%M:%S";

	if(NULL == zDes || NULL == zSrc) // || strlen(zSrc) == 0)
	{
		return FALSE;
	}
	if(NULL == strptime(zSrc, zTimeFmt, &tmTime))
	{
		return FALSE;
	}
	tmTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tTime = mktime(&tmTime);
	if(tTime < 0)
	{
		return FALSE;
	}
	tTime += tIntervalTime;
	tmTime = (*localtime(&tTime));
	
	strftime(zDes, RESMGR_TIME_LEN_MAX, zTimeFmt, &tmTime);
	return TRUE;
}

time_t getDurationSec(const char *zFilePath)
{
	FILE *fp = NULL;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	time_t lDuration = 0;
	int  nTimeInterval = 0;
	char zFileTime[50];

	fp = fopen(zFilePath, "r");
	if (fp == NULL)
	{
		return 0;
	}

	read = getline(&line, &len, fp);
	if(read != -1)
	{
		sscanf(line, RESMGR_1ST_LINE_FMT , zFileTime, &nTimeInterval, &lDuration);
	}

	if(line)
	{
		free(line);
		line = NULL;
	}
	if(fp)
		fclose(fp);
	return lDuration;
}

int getIntervalSec(const char *zFilePath0)
{
	FILE *fp = NULL;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	time_t lDuration = 0;
	int  nTimeInterval = 0;
	char zFileTime[50];

	fp = fopen(zFilePath0, "r");
	if (fp == NULL)
	{
		return 0;
	}

	read = getline(&line, &len, fp);
	if(read != -1)
	{
		sscanf(line, RESMGR_1ST_LINE_FMT , zFileTime, &nTimeInterval, &lDuration);
	}

	if(line)
	{
		free(line);
		line = NULL;
	}
	if(fp)
		fclose(fp);
	return nTimeInterval;
}

void aos_rhc_mgr_create_log_entry(const char *zFileName, const int nLine, const char *zErr)
{
	char zLogFileName[64] = "";
	char *zDateTime;
	zDateTime = getLocalTimeStr("%Y%m%d");
	sprintf(zLogFileName, RESMGR_LOG_FILE_FMT, zDateTime);

	FILE * fpLog = fopen(zLogFileName, "a");
	if(fpLog)
	{
		zDateTime = getLocalTimeStr("%Y%m%d-%H:%M:%S");
	   	fprintf(fpLog, "Time=[%s] FILE=[%s] LINE=[%d] %s \n", 
						zDateTime, zFileName, nLine, zErr);
	}
	if(fpLog)
	{
		fclose(fpLog);
		fpLog = NULL;
	}
}

// erase ' ' in the input src
BOOL eraseSpace(char * const src)
{
	int i = 0, j = 0;
	int nCounter = strlen(src);
	for(i = 0; i < nCounter; i++)
	{
		if(src[i] == ' ')
		{
			for(j = i; j < nCounter-1; j++)
				src[j] = src[j+1];
			src[j] = '\0';
		}
	}
	return TRUE;
}

// Validate the time zone
BOOL isValidTimeZone(const char *strBegTime, const char *strEndTime, const long lDurationSec)
{
	const char *zTimeFormat = "%Y%m%d-%H:%M:%S";
	struct tm tmBegTime;
	struct tm tmEndTime;
	time_t tBegTime, tEndTime;
	const time_t tNowTime = time(NULL) - 1; // set current time to now-1sec

	memset(&tmBegTime, 0, sizeof(tmBegTime));
	memset(&tmEndTime, 0, sizeof(tmEndTime));
	if(!strptime(strBegTime, zTimeFormat, &tmBegTime))
	{
		return FALSE;
	}

	if(!strptime(strEndTime, zTimeFormat, &tmEndTime))
	{
		return FALSE;
	}
	tmBegTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tmEndTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */

	tBegTime = mktime(&tmBegTime);
	tEndTime = mktime(&tmEndTime);

	if((tNowTime - lDurationSec <= tBegTime) 
		&& tBegTime < tEndTime 
		&& tEndTime <= tNowTime)
	{
		return TRUE;
	}
	else	// out of time zone
	{
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , "condition error. ");
		}
#endif
		return FALSE;
	}
}

// Validate the time zone
BOOL isValidTimeInput(const char *strBegTime, const char *strEndTime, const long lShowInterval, const long lSampleDurationSec, const long lSampleIntervalSec)
{
	const char *zTimeFormat = "%Y%m%d-%H:%M:%S";
	struct tm tmBegTime;
	struct tm tmEndTime;
	time_t tBegTime, tEndTime;
	const time_t tNowTime = time(NULL) - 1; // set current time to now-1sec

	memset(&tmBegTime, 0, sizeof(tmBegTime));
	memset(&tmEndTime, 0, sizeof(tmEndTime));
	if(!strptime(strBegTime, zTimeFormat, &tmBegTime))
	{
		return FALSE;
	}

	if(!strptime(strEndTime, zTimeFormat, &tmEndTime))
	{
		return FALSE;
	}

	tmBegTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	tmEndTime.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */

	tBegTime = mktime(&tmBegTime);
	tEndTime = mktime(&tmEndTime);

	if(((unsigned long)(tNowTime - lSampleDurationSec) <= (unsigned long)tBegTime) 
		&& (unsigned long)tBegTime < (unsigned long)tEndTime 
		&& (unsigned long)tEndTime <= (unsigned long)tNowTime 
		&& (unsigned long)lShowInterval <= (unsigned long)(tEndTime - tBegTime)
		&& (unsigned long)lShowInterval >= (unsigned long)lSampleIntervalSec)
	{
		return TRUE;
	}
	else	// out of time zone
	{
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , "condition error. ");
		}
#endif
		return FALSE;
	}
}

//===========================================================================================================
// Tool functions for Resource Manager 
//===========================================================================================================
// load the data line 
// if not find return false
int loadResSnapDataLine(FILE *fp1, FILE *fp2, const time_t tTime, char ** lineptr)
{
	char zFileTime1[50] = "", zFileTime2[50] = "";
	time_t tFileTime1 = 0, tFileTime2 = 0, tFileTimeStd = 0;
	time_t tLineTime = 0, tLnCmpTime = 0;
	
	time_t lDurationStd = 0;
	int nIntervalStd = 0;
	char * line = NULL;
	char * zTmp = NULL;
	size_t len = 0;
	ssize_t read;
	struct tm tmTmp;

	// Initialize data
	if(* lineptr)
	{
		free(* lineptr);
		*lineptr = NULL;
	}

	// set to the top of the files
	if(fp1)
	{
		rewind(fp1); // same as fseek(fp1, 0L, SEEK_SET);
	}
	if(fp2)
	{
		rewind(fp2); // same as fseek(fp2, 0L, SEEK_SET);
	}

	// get file data
	if(fp1)
	{
		if((read = getline(&line, &len, fp1)) != -1)
		{
			// sscanf(line, "%s %d %ld %*ld" , zFileTime1, &nIntervalStd, &lDurationStd);
			sscanf(line, "%s %d %ld" , zFileTime1, &nIntervalStd, &lDurationStd);
		}
	}
	if(fp2)
	{
		if((read = getline(&line, &len, fp2)) != -1)
		{
			// sscanf(line, "%s %*d %*ld %*ld" , zFileTime2);
			sscanf(line, "%s %*d" , zFileTime2);
		}
	}

	zTmp = strptime(zFileTime1, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	if(NULL != zTmp) //  && strlen(zTmp) != 0
	{
		tFileTime1 = mktime(&tmTmp);
	}
	zTmp = strptime(zFileTime2, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	if(NULL != zTmp)
	{
		tFileTime2 = mktime(&tmTmp);
	}

	tFileTimeStd = (tFileTime1 > tFileTime2)?tFileTime1:tFileTime2;

	// Find the CPU Info
	if(tTime >= tFileTimeStd)
	{
		FILE * fpTmp = NULL;
		if(tFileTimeStd == tFileTime1)
		{
			fpTmp = fp1;
		}
		else
		{
			fpTmp = fp2;
		}
		tLnCmpTime = tTime - nIntervalStd;

		while (tLineTime <= tLnCmpTime) 
		{
			read = getline(&line, &len, fpTmp);
			if(-1 == read)
			{
				break;
			}
			sscanf(line, "%lu %*s", &tLineTime);
		}
		if(read < 0)	// Failed to find the data
		{
			if(line)
			{
				free(line);
				line = NULL;
			}
			return 0;
		}
		else			// Find the data successfully!
		{
/*
//		   	sscanf(line, "%lu %lu %lu %lu %lu %lu %lu %lu %*s\n", &cpuSnap->index, \
//				&cpuSnap->cpu_user, &cpuSnap->cpu_nice, &cpuSnap->cpu_system, &cpuSnap->cpu_idle, \
//		   		&cpuSnap->cpu_iowait, &cpuSnap->cpu_irq, &cpuSnap->cpu_sirq);
 */
		}
	}
	else
	{
		FILE * fpTmp = NULL;
		if(tFileTimeStd == tFileTime1)
		{
			fpTmp = fp2;
		}
		else
		{
			fpTmp = fp1;
		}
		tLnCmpTime = tTime - nIntervalStd;
		while (tLineTime <= tLnCmpTime) 
		{
			read = getline(&line, &len, fpTmp);
			if(-1 == read)
			{
				break;
			}
			sscanf(line, "%lu %*s", &tLineTime);
		}
		if(read < 0)	// Failed to find the data
		{
			if(line)
			{
				free(line);
				line = NULL;
			}
			return 0;
		}
		else			// Find the data successfully!
		{
/*
//		   	sscanf(line, "%lu %lu %lu %lu %lu %lu %lu %lu %*s\n", &cpuSnap->index, \
//				&cpuSnap->cpu_user, &cpuSnap->cpu_nice, &cpuSnap->cpu_system, &cpuSnap->cpu_idle, \
//		   		&cpuSnap->cpu_iowait, &cpuSnap->cpu_irq, &cpuSnap->cpu_sirq);
 */
		}
	}

//	if(line)
//	{
//		free(line);
//		line = NULL;
//	}
	*lineptr = line; // transfered to outer place
	return 1;
}


// back up the last resource file
// check whether file1 or file2 is the last file. rename it to the file2
BOOL backupLastResFile(const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL;
	FILE *fp2 = NULL;
	char zFileTime1[50] = "", zFileTime2[50] = "";
	time_t tFileTime1 = 0, tFileTime2 = 0, tFileTimeStd = 0;

	time_t lDurationStd = 0;
	int nIntervalStd = 0;
	char * line = NULL;
	char * zTmp = NULL;
	size_t len = 0;
	ssize_t read;
	struct tm tmTmp;

	// open files
	fp1 = fopen(zFilePath1, "r");
	if (fp1 == NULL)
	{
		// No record yet. Just leave.
		return TRUE;
	}
	fp2 = fopen(zFilePath2, "r");
	if (fp2 == NULL)
	{
		// return FALSE;
	}

	// set to the top of the files
	if(fp1)
	{
		rewind(fp1); // same as fseek(fp1, 0L, SEEK_SET);
	}
	if(fp2)
	{
		rewind(fp2); // same as fseek(fp2, 0L, SEEK_SET);
	}

	// get file data
	if(fp1)
	{
		if((read = getline(&line, &len, fp1)) != -1)
		{
			// sscanf(line, "%s %d %ld %*ld" , &zFileTime1, &nIntervalStd, &lDurationStd);
			sscanf(line, "%s %d %ld" , zFileTime1, &nIntervalStd, &lDurationStd);
		}
	}
	if(fp2)
	{
		if((read = getline(&line, &len, fp2)) != -1)
		{
			// sscanf(line, "%s %*d %*ld %*ld" , &zFileTime2);
			sscanf(line, "%s %*d" , zFileTime2);
		}
	}

	zTmp = strptime(zFileTime1, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	if(NULL != zTmp) //  && strlen(zTmp) != 0
	{
		tFileTime1 = mktime(&tmTmp);
	}
	zTmp = strptime(zFileTime2, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	if(NULL != zTmp)
	{
		tFileTime2 = mktime(&tmTmp);
	}

	tFileTimeStd = (tFileTime1 > tFileTime2)?tFileTime1:tFileTime2;
	
	if(line)
	{
		free(line);
		line = NULL;
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
	if(tFileTimeStd == tFileTime1)
	{
		// int rename(const char *oldpath, const char *newpath)
		if(0 > rename(zFilePath1, zFilePath2))
		{
#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_trace("error on rename from [%s] to [%s]", zFilePath1, zFilePath2);
			}
#endif
			return FALSE;
		}
	}

	return TRUE;
}

// load the data line of most recent to tTime
// if not find return most recent data line out
int loadResSnapDataLineLast(FILE *fp1, FILE *fp2, const time_t tTime, char ** lineptr)
{
	char zFileTime1[50] = "", zFileTime2[50] = "";
	time_t tFileTime1 = 0, tFileTime2 = 0, tFileTimeStd = 0;
	time_t tLineTime = 0, tLnCmpTime = 0;
	
	time_t lDurationStd = 0;
	int nIntervalStd = 0;
	char * line = NULL;
	char * zTmp = NULL;
	size_t len = 0;
	ssize_t read;
	struct tm tmTmp;
	// Initialize data
	if(* lineptr)
	{
		free(* lineptr);
		*lineptr = NULL;
	}

	// set to the top of the files
	if(fp1)
		rewind(fp1); // same as fseek(fp1, 0L, SEEK_SET);
	if(fp2)
		rewind(fp2); // same as fseek(fp2, 0L, SEEK_SET);

	// get file data
	if(fp1)
	{
		if((read = getline(&line, &len, fp1)) != -1)
		{
			// sscanf(line, "%s %d %ld %*ld" , &zFileTime1, &nIntervalStd, &lDurationStd);
			sscanf(line, "%s %d %ld" , zFileTime1, &nIntervalStd, &lDurationStd);
		}
	}
	if(fp2)
	{
		if((read = getline(&line, &len, fp2)) != -1)
		{
			sscanf(line, "%s %*d" , zFileTime2);
		}
	}

	zTmp = strptime(zFileTime1, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	if(NULL != zTmp) //  && strlen(zTmp) != 0
	{
		tFileTime1 = mktime(&tmTmp);
	}
	zTmp = strptime(zFileTime2, "%Y%m%d-%H:%M:%S", &tmTmp);
	tmTmp.tm_isdst = ((daylight == 0)?(0):(1)); /* include <time.h> check by "man tzname" */
	if(NULL != zTmp)
	{
		tFileTime2 = mktime(&tmTmp);
	}
	zTmp = (char*)malloc(RESMGR_RCD_LINE_MAX);

	tFileTimeStd = (tFileTime1 > tFileTime2)?tFileTime1:tFileTime2;
	// Find the CPU Info
	if(tTime >= tFileTimeStd)
	{
		FILE * fpTmp = NULL;
		if(tFileTimeStd == tFileTime1)
		{
			fpTmp = fp1;
		}
		else
		{
			fpTmp = fp2;
		}
		tLnCmpTime = tTime - nIntervalStd;
		while (tLineTime <= tLnCmpTime) 
		{
			read = getline(&line, &len, fpTmp);
			if(-1 == read)
			{
				break;
			}
			strcpy(zTmp, line);
		}
	}
	else
	{
		FILE * fpTmp = NULL;
		if(tFileTimeStd == tFileTime1)
		{
			fpTmp = fp2;
		}
		else
		{
			fpTmp = fp1;
		}
		tLnCmpTime = tTime - nIntervalStd;
		while (tLineTime <= tLnCmpTime) 
		{
			read = getline(&line, &len, fpTmp);
			if(-1 == read)
			{
				break;
			}
			strcpy(zTmp, line);
		}
	}
	if(line)
	{
		free(line);
		line = NULL;
	}
	*lineptr = zTmp; // transfered to outer place
	return 1;
}


// set sampling record interval or duration
int aos_resmgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen, int * nMaxrPtr, long * lDurationPtr, int * nIntervalPtr)
{
	char *zTmp = NULL;
	long 	lDuration; 	// = 0;
	int 	nInterval; 	// = 1;
	int 	nMaxr; 		// = 2;

	// 1. Validate the input
	if(0 != getuid())
	{
		zTmp = "Error: Authority not enough.";
		snprintf (zErrmsg, nErrorLen, USAGE_INFO_FMT,zTmp,"ResMgr");
		zErrmsg[nErrorLen-1] = 0;
		return -eAosRc_PermissionDenied;
	}
	else if(2 != argc ||
			nMaxrPtr	 == NULL ||
		    lDurationPtr == NULL ||
		    nIntervalPtr == NULL)
	{
		zTmp = "Error: Illegal input.";
		snprintf (zErrmsg, nErrorLen, USAGE_INFO_FMT,zTmp,"ResMgr");
		zErrmsg[nErrorLen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}
            
	// 2. Get the input value
	nInterval = getDuration(argv[0]);
	lDuration = getDuration(argv[1]);
	if(lDuration <= 0 || nInterval <= 0)
	{
		zTmp = "Error: Illegal input.";
		snprintf (zErrmsg, nErrorLen, USAGE_INFO_FMT,zTmp,"ResMgr");
		zErrmsg[nErrorLen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}
	// 3. Get the key value 
	nMaxr = lDuration / nInterval;
	if(nMaxr > MAX_TIME_RATIO || nMaxr < MIN_TIME_RATIO)
	{
		// illegal input! 
		char zTmpErr[256] = "";
		snprintf (zTmpErr, 256, "Error: Records number [%d] out of bound [%d,%d].",nMaxr, MIN_TIME_RATIO, MAX_TIME_RATIO);
		zTmpErr[256-1] = 0;
		snprintf (zErrmsg, nErrorLen, USAGE_INFO_FMT,zTmpErr,"res mgr");
		zErrmsg[nErrorLen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}
	(*nMaxrPtr) 	= nMaxr;
	(*lDurationPtr) = lDuration;
	(*nIntervalPtr) = nInterval;
	return 0;
}
