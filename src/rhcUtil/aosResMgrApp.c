////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrApp.c
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "rhcUtil/aosResMgrApp.h"
//#include "aosDiskMgrApp.h"
#include "rhcUtil/aosResMgrToolkit.h"

// For signal processing
#include <string.h>

// struct sigaction g_resmgr_sigactions;

void reset_sig(int signo)
{
	int rc = sigaction(signo, NULL,NULL);
	if(rc)
	{
		return;
	}
}

void reset_reg_alarm_sig(void * resmgr_sigactions)
{	
	memset((struct sigaction *)resmgr_sigactions, 0, sizeof(struct sigaction));
	sigemptyset(&((struct sigaction *)resmgr_sigactions)->sa_mask);
	((struct sigaction *)resmgr_sigactions)->sa_flags = 0;
	((struct sigaction *)resmgr_sigactions)->sa_handler = aos_resmgr_sighand;

	int rc = sigaction(SIGALRM, (struct sigaction *)resmgr_sigactions,NULL);
	if(rc)
	{
		return;
	}
}

// ===================================================
// Self defined value reset
// ===================================================
void reset_ResMgr_Threshold(struct ResMgr_Threshold * const ptr)
{
	// convert condition: from normal state to alarm state
	ptr->m_nAlarmThresholdTime 	= RESMGR_DEFAULT_ALARM__EDGE_PERIOD_LIMIT;	/* threshold time period     */
	ptr->m_nAlarmThresholdRatio = RESMGR_DEFAULT_ALARM__USAGE_RATIO_LIMIT;	/* threshold alarm threshold 100 percentage number */

	// convert condition: from alarm state to normal state
	ptr->m_nNormalThresholdTime = RESMGR_DEFAULT_NORMAL_EDGE_PERIOD_LIMIT;	/* threshold time period     */
	ptr->m_nNormalThresholdRatio = RESMGR_DEFAULT_NORMAL_USAGE_RATIO_LIMIT;	/* threshold alarm threshold 100 percentage number */
}

void reset_ResMgr_App(struct ResMgr_App * const ptr)
{
	if(ptr)
	{
		reset_ResMgr_Threshold(&(ptr->m_threshold_val));
		ptr->m_switch_on= FALSE;	// = TRUE;
		ptr->m_status_on= FALSE;	// = TRUE;
		memset(&ptr->m_resmgr_sigactions, 0 , sizeof(ptr->m_resmgr_sigactions));
	}
}

void reset_ResMgr_Config(struct ResMgr_App * const ptr)
{
	if(ptr)
	{
		reset_ResMgr_Threshold(&(ptr->m_threshold_val));
	}
}

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
aos_resmgr_set_threshold(void * const input_ptr, 
						 const char* busy_time_edge, const int busy_threshold, 
						 const char* normal_time_edge, const int normal_threshold)
{
	int nTmp = 0;
	int nRetVal = 0;// -eAosRc_Success;
	ResMgr_Threshold_t * const threshold_ptr = (ResMgr_Threshold_t *)input_ptr;

	/* start of the modify task */
	if(threshold_ptr)
	{
		// m_nAlarmThresholdTime
		if(busy_time_edge)
		{
			nTmp = getDuration(busy_time_edge);
			if(nTmp > 0)
			{
				threshold_ptr->m_nAlarmThresholdTime = nTmp;
			}
			else
			{
				nRetVal = -eAosRc_ResMgrInputInvalid;
			}
		}
		else
		{
			threshold_ptr->m_nAlarmThresholdTime = 0;
		}
		
		// m_nAlarmThresholdRatio
		if(busy_threshold > 0)
		{
			threshold_ptr->m_nAlarmThresholdRatio = busy_threshold;
		}
		else
		{
			threshold_ptr->m_nAlarmThresholdRatio = 0;
		}
		
		// m_nNormalThresholdTime
		if(normal_time_edge)
		{
			nTmp = getDuration(normal_time_edge);
			if(nTmp > 0)
			{
				threshold_ptr->m_nNormalThresholdTime = nTmp;
			}
			else
			{
				nRetVal = -eAosRc_ResMgrInputInvalid;
			}
		}
		else
		{
			threshold_ptr->m_nNormalThresholdTime = 0;
		}
		
		// m_nNormalThresholdRatio
		if(normal_threshold > 0)
		{
			threshold_ptr->m_nNormalThresholdRatio = normal_threshold;
		}
		else
		{
			threshold_ptr->m_nNormalThresholdRatio = 0;
		}
	}
	else
	{
		return -eAosRc_NullPointer;
	}
	/* end of the modify task */

	return nRetVal;
}

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
						 const void * input_ptr)
{
	int nRetVal = 0;// -eAosRc_Success;
	ResMgr_Threshold_t * const threshold_ptr = (ResMgr_Threshold_t *)input_ptr;

	if(threshold_ptr)
	{
		if(busy_time_edge)
		{
			*busy_time_edge = threshold_ptr->m_nAlarmThresholdTime;
		}
		if(busy_threshold)
		{
			*busy_threshold = threshold_ptr->m_nAlarmThresholdRatio;
		}
		if(normal_time_edge)
		{
			*normal_time_edge = threshold_ptr->m_nNormalThresholdTime;
		}
		if(normal_threshold)
		{
			*normal_threshold = threshold_ptr->m_nNormalThresholdRatio;
		}
	}
	else
	{
		return -eAosRc_NullPointer;
	}

	return nRetVal;
}

// copy threshold value
// note: 
//		busy_time_edge   == 0 		means keep the old value
//		normal_time_edge == 0 		means keep the old value
//		busy_threshold   == 0 		means keep the old value
//		normal_threshold == 0 		means keep the old value
void aos_resmgr_copy_threshold(ResMgr_Threshold_t * const des_ptr, const ResMgr_Threshold_t * src_ptr)
{
	if(src_ptr && des_ptr)
	{
		// m_nAlarmThresholdTime
		if(src_ptr->m_nAlarmThresholdTime > 0)
		{
			des_ptr->m_nAlarmThresholdTime
				 = src_ptr->m_nAlarmThresholdTime;
		}
	
		// m_nAlarmThresholdRatio
		if(src_ptr->m_nAlarmThresholdRatio > 0)
		{
			des_ptr->m_nAlarmThresholdRatio
				 = src_ptr->m_nAlarmThresholdRatio;
		}
	
		// m_nNormalThresholdTime
		if(src_ptr->m_nNormalThresholdTime > 0)
		{
			des_ptr->m_nNormalThresholdTime
				 = src_ptr->m_nNormalThresholdTime;
		}
	
		// m_nNormalThresholdRatio
		if(src_ptr->m_nNormalThresholdRatio > 0)
		{
			des_ptr->m_nNormalThresholdRatio
				 = src_ptr->m_nNormalThresholdRatio;
		}
	}
}

//  API: Init API function
int ros_res_mgr_init()
{
	sighandler_t ret_val;
	ret_val = signal(SIGALRM, SIG_IGN);
	if(SIG_ERR == ret_val)
	{
		// error
		// return;
	}

	int rslt = 0;
//	rslt &= ros_disk_mgr_init();
	return rslt;
}

// only for system nice 
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/errno.h>
#include <string.h>

//	API: system nice set <pid> <nice-value>
int aos_nice(int nPid, int nInc)
{
	const int errlen = 256;
	char errmsg[256] = "";
	int nRslt, nOld, nNiceVal;

	errno = 0;
	nOld = getpriority(PRIO_PROCESS, nPid);
	if(errno)
	{
		strcpy(errmsg,strerror(errno));
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -errno;
	}

	/*
	 * System MACRO
	 * `PRIO_MIN'
	 *   The lowest valid nice value.
	 * `PRIO_MAX'
	 *   The highest valid nice value.
	 * More information can be found by linux command "info nice"
	 */
	nNiceVal = nOld - nInc;
	nNiceVal = RESMGR_MINIMUM(nNiceVal, PRIO_MAX);
	nNiceVal = RESMGR_MAXIMUM(nNiceVal, PRIO_MIN);

	nRslt = setpriority(PRIO_PROCESS, nPid, nNiceVal);
	if(nRslt < 0)
	{
		strcpy(errmsg,strerror(errno));
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -errno;
	}
    return 0;
}
