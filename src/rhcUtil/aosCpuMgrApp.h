////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCpuMgrApp.h
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosCpuMgrApp_h
#define aos_rhcUtil_aosCpuMgrApp_h

#include "../rhcUtil/aosResMgrDefines.h"
#include "../rhcUtil/aosResMgrApp.h"
#include "../rhcUtil/aosCpuMgrToolkit.h"

// for thread
#include <pthread.h>

#define LOG_FILE "/var/log/cpu_alarm.log"

#define MAX_PROCESS_INFO	10240

#define UTSNAME_LEN 64

#define CPUMGR_PREV_STAT_MAX 1 

//#define NR_IRQS 	256
//#define MAX_PID_NR 	256
//#define NON_FATAL 	0
//#define FATAL 		1

struct CpuMgr_Alarm_Val
{
	int m_nIdleRatio;	/* current state of the device    *///  = 0
	int m_nAlarmFlag;	/* current state last time period */// (m_nAlarmFlag*ninterval) time period to send alarm = 0
	BOOL m_bIsOverload; //  = FALSE
};

void reset_CpuMgr_Alarm_Val(struct CpuMgr_Alarm_Val * const ptr);

struct CpuMgr_App
{
	// configuration value
	long 	m_duration; 	// = 0;
	int 	m_interval; 	// = 1;
	int 	m_maxr; 		// = 2;
	ResMgr_App_t m_resmgr_app;

	// run time status value
	long 	m_index; 		// = 0;
	CpuInfoRecordType m_CpuInfoSnap;
	CpuInfoRecordType m_CpuInfoSnapPrev[CPUMGR_PREV_STAT_MAX];
	struct 	CpuMgr_Alarm_Val m_CpuMgrAlarm;
//	BOOL 	m_switch_on ;	// = TRUE;
//	pthread_cond_t m_thread_stopped;
//	pthread_t m_thread;
//	res_thread_struct_t m_thread_input;
//	pthread_mutex_t m_mutex;	// global mutex lock!

	BOOL 	m_is_first_sample; 	// = TRUE;
//	// the threshold values
//	struct ResMgr_Threshold m_threshold_val;
//	CpuMgr_App() : m_duration(0), m_index(0), m_interval(1), m_maxr(2) {};
};
void reset_config_CpuMgr_App(struct CpuMgr_App * const ptrApp);
void reset_CpuMgr_App(struct CpuMgr_App * const ptrApp);

// Declare global variables
extern struct CpuMgr_App g_theCpuMgrApp;

// Encapsulate the operation of g_theCpuMgrApp
void get_CpuMgr_ResMgr_Threshold_App(ResMgr_Threshold_t * const ptr);
// Encapsulate the operation of g_theCpuMgrApp
void set_CpuMgr_ResMgr_Threshold_App(const ResMgr_Threshold_t * ptr);

///////////////////////////////////////////////////////////////////////////////////
// Tool functions for Create or Kill the thread
///////////////////////////////////////////////////////////////////////////////////
// encapsulate the process of create a detached thread
BOOL cpuMgrSwitchOn();
// encapsulate the process of kill a detached thread
BOOL cpuMgrSwitchOff();

///////////////////////////////////////////////////////////////////////////////////
// Tool functions for CpuMgr main.cpp 
///////////////////////////////////////////////////////////////////////////////////
// print help infomation and exit the program
void printCpuMgrHelp(const char *program_name);
// set record by input string arrays
int aos_cpumgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen);

// running this function after each interval
void aos_cpu_mgr_repeat(int nSignal);

void cpuMgrThreadFunc(void * ); //(res_thread_struct_t * cpu_thread_input)

#endif // aos_rhcUtil_aosCpuMgrApp_h
