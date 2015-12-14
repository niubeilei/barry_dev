////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Global.h
// Description:
//   
//
// Modification History:
// 2007-02-14 Created by CHK
////////////////////////////////////////////////////////////////////////////
#ifndef aos_rhcUtil_aosMemMgrApp_h
#define aos_rhcUtil_aosMemMgrApp_h

#include "../rhcUtil/aosResMgrDefines.h"
#include "../rhcUtil/aosResMgrApp.h"
#include "../rhcUtil/aosMemMgrToolkit.h"

// for thread
#include <pthread.h>

#define MEM_INFO_FILE_0 "/var/log/aos_mem_log.0"
#define MEM_INFO_FILE_1 "/var/log/aos_mem_log.1"

#define MEM_INFO_FILE_0_TMP "/var/log/aos_mem_log.0.tmp"
#define MEM_INFO_FILE_1_TMP "/var/log/aos_mem_log.1.tmp"

#define MEMMGR_PROC_MEM_FILE "/proc/meminfo"
#define MEMMGR_PID_STATM 	 "/proc/%ld/statm"

#define MEMMGR_PREV_STAT_MAX 1

struct MemMgr_Alarm_Val
{
	int m_nFreeRatio;	//  = 0 /* the space ratio of free */
	int m_nAlarmFlag;	//  = 0 /*(m_nAlarmFlag*ninterval) time period to send alarm*/ 
	BOOL m_bIsOverload; //  = FALSE

	// convert condition: from normal state to alarm state
	int m_nAlarmThresholdTime;	/* threshold time period     */
	int m_nAlarmThresholdRatio;	/* threshold alarm threshold 100 percentage number */

	// convert condition: from alarm state to normal state
	int m_nNormalThresholdTime;	/* threshold time period     */
	int m_nNormalThresholdRatio;/* threshold alarm threshold 100 percentage number */
};
void reset_MemMgr_Alarm_Val(struct MemMgr_Alarm_Val * const ptr);

struct MemMgr_App
{
	MemInfoRecordType m_MemInfoSnap;
	MemInfoRecordType m_MemInfoSnapPrev[MEMMGR_PREV_STAT_MAX];
	long 	m_duration;
	long 	m_index;
	int 	m_interval;
	int 	m_maxr;
//	BOOL 	m_switch_on ;	// = TRUE;
	struct 	MemMgr_Alarm_Val m_MemMgrAlarm;
//	pthread_cond_t m_thread_stopped;
//	pthread_t m_thread;
//	res_thread_struct_t m_thread_input;

	BOOL m_is_first_sample; 	// = TRUE;
	struct ResMgr_App m_resmgr_app;
//	MemMgr_App() : m_duration(0), m_index(0), m_interval(1), m_maxr(2) {};
};
void reset_config_MemMgr_App(struct MemMgr_App * const ptrApp);
void reset_MemMgr_App(struct MemMgr_App * const ptr);

// Declare global variables
extern struct MemMgr_App g_theMemMgrApp;

// Encapsulate the operation of g_theCpuMgrApp
void get_MemMgr_ResMgr_Threshold_App(ResMgr_Threshold_t * const ptr);
// Encapsulate the operation of g_theCpuMgrApp
void set_MemMgr_ResMgr_Threshold_App(const ResMgr_Threshold_t * ptr);
///////////////////////////////////////////////////////////////////////////////////
// Tool functions for Create or Kill the thread
///////////////////////////////////////////////////////////////////////////////////
// encapsulate the process of create a detached thread
BOOL memMgrSwitchOn();
// encapsulate the process of kill a detached thread
BOOL memMgrSwitchOff();

///////////////////////////////////////////////////////////////////////////////////
// Tool functions for MemMgr main.cpp 
///////////////////////////////////////////////////////////////////////////////////
// print help infomation and exit the program
void printMemMgrHelp(const char *program_name);
// set sampling record interval or duration
int aos_memmgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen);

// running this function after each g_theMemMgrApp.m_interval
void aos_mem_mgr_repeat(int nSignal);

void memMgrThreadFunc(void * ); //(res_thread_struct_t * mem_thread_input)

#endif // aos_rhcUtil_aosMemMgrApp_h
