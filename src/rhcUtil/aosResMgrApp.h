////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrApp.h
// Description:
//   
//
// Modification History:
// 2007-02-25 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosResMgrApp_h
#define aos_rhcUtil_aosResMgrApp_h

#include "aosResMgrDefines.h"

// for signal alarm
// extern struct sigaction g_resmgr_sigactions;

void reset_reg_alarm_sig(void * resmgr_sigactions);

// for thread
#include <pthread.h>
// For signal processing
#include <signal.h>

struct res_thread_struct
{
	char argv0[16];
	char argv1[16];
	char argv2[16];
};

#define res_thread_struct_t struct res_thread_struct

// threshold value NOT IN USE YET!!!!
typedef struct aos_resmgr_threshold 
{
	int	m_critical_ratio;
	int m_edge_sec;
} aos_resmgr_threshold_t;

// threshold data struct
typedef struct ResMgr_Threshold
{
	// convert condition: from normal state to alarm state
	int m_nAlarmThresholdTime;	/* threshold time period     */
	int m_nAlarmThresholdRatio;	/* threshold alarm threshold 100 percentage number */

	// convert condition: from alarm state to normal state
	int m_nNormalThresholdTime;	/* threshold time period     */
	int m_nNormalThresholdRatio;/* threshold alarm threshold 100 percentage number */
}ResMgr_Threshold_t;

void reset_ResMgr_Threshold(struct ResMgr_Threshold * const);

typedef struct ResMgr_App
{
	// configuration value
	ResMgr_Threshold_t m_threshold_val;	/* the threshold values */ 

	// running time status value
	pthread_mutex_t m_mutex;					/* global mutex lock! */ 

	// signal handler for the thread
	struct sigaction m_resmgr_sigactions;	/* signal handler */

	BOOL 	m_switch_on ;	// = TRUE;
	BOOL 	m_status_on ;	// = TRUE;
	pthread_cond_t m_thread_stopped;
	pthread_cond_t m_thread_kill;
	pthread_t m_thread;
	res_thread_struct_t m_thread_input;
}ResMgr_App_t;

void reset_ResMgr_App(struct ResMgr_App * const);
void reset_ResMgr_Config(struct ResMgr_App * const);

// set alarm threshold value
// note: 
//		busy_time_edge   == NULL 	means keep the old value
//		normal_time_edge == NULL 	means keep the old value
//		busy_threshold   == -1 		means keep the old value
//		normal_threshold == -1 		means keep the old value
// return value:
// 		< 0 means failure
// 		0 means success
int 
aos_resmgr_set_threshold(void * const , 
						 const char* busy_time_edge, const int busy_threshold, 
						 const char* normal_time_edge, const int normal_threshold);

// get alarm threshold value
// note: 
//		busy_time_edge   == NULL 	means don't load the value
//		normal_time_edge == NULL 	means don't load the value
//		busy_threshold   == -1 		means don't load the value
//		normal_threshold == -1 		means don't load the value
// return value:
// 		< 0 means failure
// 		0 means success
int 
aos_resmgr_get_threshold(int *busy_time_edge, int *busy_threshold, 
						 int *normal_time_edge, int *normal_threshold, 
						 const void * input_ptr);

// copy threshold value
// note: 
//		busy_time_edge   == 0 		means keep the old value
//		normal_time_edge == 0 		means keep the old value
//		busy_threshold   == 0 		means keep the old value
//		normal_threshold == 0 		means keep the old value
void 
aos_resmgr_copy_threshold(ResMgr_Threshold_t * const des_ptr, const ResMgr_Threshold_t * src_ptr);

//  API: Init API function
int ros_res_mgr_init();

//	API: system nice set <pid> <nice-value>
int aos_nice(int nPid, int nNiceVal);

#endif //  aos_rhcUtil_aosResMgrApp_h
