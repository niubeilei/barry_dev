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
// 2007-02-14 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosResMgrToolkit_h
#define aos_rhcUtil_aosResMgrToolkit_h

#include "aosResMgrDefines.h"

//#define _XOPEN_SOURCE /* glibc2 needs this */
#include <time.h>
#include <stdio.h>
#include <pthread.h>

// special debugging define
#define _DEBUG_RES_MGR_TRACE_ON_ 1

// inner using only for resouce manager!
void aos_init_real_timer(void (*fn_exec)(int), unsigned int nSecInterval);
void aos_do_nothing(int);

// get the pid of the input program's name
BOOL aos_get_pidof(char * const readbuf, const char* zProgramName);
// kill the same name's processes by CLI
BOOL killSameProc(const char *zProgramName);
BOOL aos_kill_same_proc(const char *zProgramName);

// Translate input time string to formatted time string
// from "now", "-1H", "2M" to "yyyymmdd-hh:MM:SS"
BOOL aos_getFmtTimeStr(char *const zDes, const char *zSrc);
// Get one more minutes time string from formatted time string to formatted time string
// from "yyyymmdd-hh:MM:SS" to "yyyymmdd-hh:(MM):(SS++MAX_PROCESS_NUM*tIntervalTime)"
BOOL aos_get_next_computable_time_str(char *const zDes, const char *zSrc, const time_t tIntervalTime);
// Get one more minutes time string from formatted time string to formatted time string
// from "yyyymmdd-hh:MM:SS" to "yyyymmdd-hh:(MM):(SS+tIntervalTime)"
BOOL aos_get_next_interval_time_str(char *const zDes, const char *zSrc, const time_t tIntervalTime);

// Get the duration seconds number the log file in system
// Only work in this specific resource system
time_t getDurationSec(const char *zFilePath0);
int getIntervalSec(const char *zFilePath0);

// encapsulate the process of create a detached thread
void aos_uland_thread_create(pthread_t * tid_ptr, void (*function)(void*), void (*parameter), 
							 pthread_mutex_t * mutex_ptr);
void aos_uland_thread_create(pthread_t * tid_ptr, void (*function)(void*), void (*parameter), 
							 pthread_mutex_t * mutex_ptr, 
							 pthread_cond_t * cond_kill_ptr, pthread_cond_t * cond_stopped_ptr,
							 BOOL * status_ptr);

// encapsulate the process of destroy a detached thread
void aos_uland_thread_destroy(pthread_mutex_t * mutex_ptr);
void aos_uland_thread_destroy(pthread_t * thread_id, pthread_mutex_t * mutex_ptr, 
							  pthread_cond_t * cond_kill_ptr, 
							  pthread_cond_t * cond_stopped_ptr, BOOL * status_ptr);

void aos_resmgr_sighand(int signo);

//=================================================================
// Functions String operation
//=================================================================
// get word
const char * get_Word(char * zDes, const char * strSrc);
/*
 * How to use the upper function?
#ifdef _TEST_GETWORD_
	char strSrc[1024]= "we like 	this	game  !!!";
	char zDes[64] = "";

	const char *zSavePtr = strSrc;

	while(zSavePtr)
	{
		zSavePtr = get_Word(zDes, zSavePtr);
		if(!zSavePtr)
		{
			break;
		}
		printf("%s\n", zSavePtr);
		printf("word:[%s]\n", zDes);
	}
	return 0;
#endif // _TEST_GETWORD_
*/

// erase ' ' in the input src
BOOL eraseSpace(char * const src);

// inner using only for resouce manager!
char * getLocalTimeStr(const char *zFormat, const time_t t);
char * getLocalTimeStr(const char *zFormat);
BOOL getLocalTime(char *zTime, const int nStrSize, const char *zFormat);
time_t getDuration(const char * zDurationTime);
time_t aos_get_interval_sec(const char * zDurationTime, const char * zTimeBeg, const char * zTimeEnd);

// 
// The caller should never modify the contents of the buffer
// returned by this function.
//
char *aos_res_mgr_log_get_msg(char *fmt, ...);
// Write the log into our log file
void aos_rhc_mgr_create_log_entry(const char *zFileName, const int nLine, const char *zErr);

#ifndef aos_rhc_mgr_trace
	#define aos_rhc_mgr_trace(format, x...) \
		aos_rhc_mgr_create_log_entry(__FILE__, __LINE__, \
				aos_res_mgr_log_get_msg(format, ##x) )
#endif

#ifndef aos_rhc_mgr_illegal_num_trace
	#define aos_rhc_mgr_illegal_num_trace(x1, x2, x3) \
		if(x1 > 1)\
		{\
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__, \
					aos_res_mgr_log_get_msg("%3.3f = %ld/%ld", x1, x2, x3) );\
		}
#endif

// Tool functions only for this area
// Validate the time zone
int isValidTimeZone(const char *strBegTime, const char *strEndTime, const long lDurationSec); // const unsigned long lDurationSec = (time(NULL))
// Validate the time zone
int isValidTimeInput(const char *strBegTime, const char *strEndTime, const long lShowInterval, const long lSampleDurationSec, const long lSampleIntervalSec);

//=================================================================
// Functions for Resource Manager
//=================================================================
// back up the last resource file
// check whether file1 or file2 is the last file. rename it the file2
BOOL backupLastResFile(const char *zFilePath1, const char *zFilePath2);

// load the data line 
// if not find return false
int loadResSnapDataLine(FILE *fp1, FILE *fp2, const time_t tTime, char ** lineptr);

// back up the last resource file
// check whether file1 or file2 is the last file. rename it to the file2
BOOL backupLastResFile(const char *zFilePath1, const char *zFilePath2);

// load the data line of most recent to tTime
// if not find return most recent data line out
int loadResSnapDataLineLast(FILE *fp1, FILE *fp2, const time_t tTime, char ** lineptr);

// validate the input record time 
int aos_resmgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen, int * nMaxrPtr, long * lDurationPtr, int * nIntervalPtr);
//=================================================================

#endif //  aos_rhcUtil_aosResMgrToolkit_h
