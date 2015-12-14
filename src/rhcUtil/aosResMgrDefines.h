////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrDefines.h
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosResMgrDefines_h
#define aos_rhcUtil_aosResMgrDefines_h

#include "ReturnCode.h"
#include "TinyXml/TinyXml.h"

#define RECORD_NUM 1024
#define MAX_CMDLINE 256
#define MAX_FNAME 256

#ifndef BOOL
	#define BOOL int
	#define TRUE 1
	#define FALSE 0
#endif

#define RESMGR_SYS_PROC "/proc"

#define MAX_PROCESS_INFO	10240
#define MAX_PROCESS_NUM		128
#define MAX_RSLT_PROCESS_NUM MAX_PROCESS_NUM*2

#define MAX_MEM_INFO 10240
#define MAX_CMDLINE 256

#define MAX_TIME_RATIO 1000000
#define MIN_TIME_RATIO 10

#define USAGE_INFO_FMT "%s\nTry \"%s --help\" for more information.\n"

#define RESMGR_EXT_LEN 16

#define RESMGR_LOG_ENTRY_MAX 1024 
#define RESMGR_TIME_LEN_MAX  32 
#define RESMGR_RCD_LINE_MAX 8192 
#define RESMGR_LOCALBUFFER_MAX 10240 
#define RESMGR_SHOW_RES_MAX  128 
#define RESMGR_SHOW_RES_DEFAULT  1 
#define RESMGR_EXPIRED_TIME_DEFAULT  10  /* The max expired wait time period */
#define RESMGR_SAMPL_INTERVAL_DEFAULT  5 
#define RESMGR_SAMPL_DURATION_DEFAULT 1*24*60*60 // "1D"
#define RESMGR_SHOW_PROC_DEFAULT 10 
#define RESMGR_LOG_FILE_FMT  "/var/log/aos_res_mgr_debug_%s.log"

#define RESMGR_TIME_FMT "%Y%m%d-%T"
#define RESMGR_1ST_LINE_FMT "%s %d %ld %*d"
#define RESMGR_BUF_EXPLOD_MSG_FMT "Your command [%s] return "\
								"string out of buffer size. Please contact your "\
								"system administrator to solve the problem!\n"

// Alarm default values
#define RESMGR_DEFAULT_ALARM__USAGE_RATIO_LIMIT 90 /* unit: usage percentage */
#define RESMGR_DEFAULT_ALARM__EDGE_PERIOD_LIMIT 10 /* unit: current sampling time interval */
#define RESMGR_DEFAULT_NORMAL_USAGE_RATIO_LIMIT 90 /* unit: usage percentage */
#define RESMGR_DEFAULT_NORMAL_EDGE_PERIOD_LIMIT 10 /* unit: current sampling time interval */

#define RESMGR_MINIMUM(a,b)	((a) < (b) ? (a) : (b))
#define RESMGR_MAXIMUM(a,b)	((a) > (b) ? (a) : (b))

#define _DEBUG_ALARM_PRINT 1

#endif //  aos_rhcUtil_aosResMgrDefines_h
