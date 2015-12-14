////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCpuMgr.cpp
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"

#include "rhcUtil/aosCpuMgrApp.h"
#include "rhcUtil/aosResMgrApp.h"
#include "rhcUtil/aosCpuMgrToolkit.h"
#include "rhcUtil/aosResMgrToolkit.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

// int pthread_kill(pthread_t thread, int sig);
// pthread_exit()
// int pthread_attr_destroy(pthread_attr_t *attr);
// int pthread_attr_init(pthread_attr_t *attr);
// pthread_mutex_init()
/* Ëø¶¨»¥³âËø*/
// pthread_mutex_lock (&mutex);
/* ´ò¿ª»¥³âËø*/
// pthread_mutex_unlock(&mutex);

// switch on/off
// Here: create or kill the thread
// "cpu mgr set record [on/off]"
int	
cpuMgrSwitchOnOff(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
//	unsigned int index = 0;
//	char* buf = CliUtil_getBuff(data);
//	char zLocal[256] = "";
	/* here real start */
	if(strcmp(parms->mStrings[0], "on") == 0)
	{
		// destroy the old thread first
		if(!cpuMgrSwitchOff())
		{
			strncpy(errmsg,"Error: Switch Off Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}


		// start the thread ( include create the thread )
		if(!cpuMgrSwitchOn())
		{
			strncpy(errmsg,"Error: Switch On Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
	}
	else if(strcmp(parms->mStrings[0], "off") == 0)
	{
		// destroy the thread 
		if(!cpuMgrSwitchOff())
		{
			strncpy(errmsg,"Error: Switch Off Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
	}
	else
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}

	/* here real end */
//	CliUtil_checkAndCopy(buf, &index, *optlen, zLocal, strlen(zLocal));
//	*optlen = index;
	*optlen = 0;
    return 0;
}

// Here just make up and maintain the global struct of running the cpuMgrRecord 
// We have some default setting here!
// cpu mgr set record time <interval> <record time>
int	cpuMgrSetRecordTime(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	/* here real start */
	int nRslt = 0;
	nRslt = aos_cpumgr_set_record_time(parms->mNumStrings, parms->mStrings, errmsg, errlen);
	if(nRslt < 0)
	{
		errmsg[errlen-1] = 0;
		return nRslt;
	}

	/* here real end */
	*optlen = 0;
    return 0;
}

// I am going to kill this function and move the feature to switch off
int	cpuMgrSetRecordStop(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	// destroy the thread 
	cpuMgrSwitchOff();
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// cpu mgr show cpu
int	cpuMgrShowCpu(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);

	char local[CPUMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	char strBegTime[CPUMGR_TIME_LEN_MAX] = "";
	char strEndTime[CPUMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration, tSampleInterval;
	time_t tShowInterval;
	unsigned int index = 0;
	int bRslt = 1, nRslt = 1;
	CpuInfoRecordType arrCpuSnap[RESMGR_SHOW_RES_MAX];
	int nArrCpuSnapSize = 0;
	CpuInfoRcdRatioType arrRatioRslt[RESMGR_SHOW_RES_MAX];

	bRslt &= aos_getFmtTimeStr(strBegTime,parms->mStrings[0]);
	bRslt &= aos_getFmtTimeStr(strEndTime,parms->mStrings[1]);
	tShowInterval = aos_get_interval_sec(parms->mStrings[2], strBegTime, strEndTime);
	bRslt &= ((tShowInterval) > (0) ? (1) : (0));
	if(!bRslt)
	{
		strncpy(errmsg,"System Error: Input invalid!\nYour input may have one of the following three errors:!\n"\
						"1. Input begin time or end time format is illegal!\n"\
						"2. Input interval time format is illegal! \n"\
						"3. Input interval time seconds number interval_time < (EndTime - BegTime)/120 ! \n"\
						"4. Input interval time seconds number interval_time < sampling_interval_time ! \n"
						,errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}

	// 1. Validate Input
	tSampleDuration = (time_t)getDurationSec(CPU_INFO_FILE_0);
	tSampleInterval = (time_t)getIntervalSec(CPU_INFO_FILE_0);
	if(!isValidTimeInput(strBegTime, strEndTime, tShowInterval, tSampleDuration, tSampleInterval))
	{
		strncpy(errmsg,"System Error: Input invalid!\nYour input may have one of the following three errors:!\n"\
						"1. Input interval time seconds number interval_time < (EndTime - BegTime)/120 ! \n"\
						"2. Input interval time seconds number interval_time < sampling_interval_time ! \n"
						,errlen-1);
		errmsg[errlen-1] = 0;
		char tmp_errmsg[1024];
		snprintf(tmp_errmsg, sizeof(tmp_errmsg), 
								 "\nstrBegTime[%s], \nstrEndTime[%s], tShowInterval[%ld], "\
								 "tSampleDuration[%ld], tSampleInterval[%ld]\n", 
								  strBegTime, strEndTime, tShowInterval, 
								  tSampleDuration, tSampleInterval);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , tmp_errmsg);
		}
#endif
		return -eAosRc_ResMgrInputInvalid;
	}
	// 2. Load the data (CpuInfoRecordType*)
	if(!loadCpuInfo(arrCpuSnap, &nArrCpuSnapSize, tShowInterval, strBegTime, strEndTime,CPU_INFO_FILE_0, CPU_INFO_FILE_1))
	{
		sprintf(errmsg, "System Error: Can't find cpu infomation! \n");
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrRcdNotFound;
	}
	// 3. Compute the result
	if(!computeCpuInfo(arrRatioRslt, arrCpuSnap, nArrCpuSnapSize))
	{
		sprintf(errmsg, "System Error: Can't compute out cpu info! \n");
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrUnknownError;
	}

	// 4. Show the result 
	if(!showCpuInfo(local, arrRatioRslt, nArrCpuSnapSize - 1, (*optlen-64)))
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "cpu mgr show cpu");
		return -eAosRc_ResMgrDataOutOfBuffer;
	}

	/* here real end */
	// take care of the string length. 
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"cpu mgr show cpu");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// cpu mgr show process
int	cpuMgrShowProcess(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	char local[CPUMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	int nTopProcNum = 0;
	char strBegTime[CPUMGR_TIME_LEN_MAX] = "", strEndTime[CPUMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration;
	unsigned int index = 0;
	int bRslt = 1;
	CpuInfoRecordType cpuSnapBeg, cpuSnapEnd;
	ProcCpuInfoRatioType 	ratioRslt;

	bRslt &= aos_getFmtTimeStr(strBegTime,parms->mStrings[0]);
	bRslt &= aos_getFmtTimeStr(strEndTime,parms->mStrings[1]);
	if(!bRslt)
	{
		strncpy(errmsg,"System Error: Input invalid!\n\0",errlen-1);
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrInputInvalid;
	}
	nTopProcNum = parms->mIntegers[0];
	if(nTopProcNum < 1)
	{
		nTopProcNum = CPUMGR_SHOW_PROC_DEFAULT;
	}

	// 1. Validate Input
	tSampleDuration = getDurationSec(CPU_INFO_FILE_0);
	if(!isValidTimeZone(strBegTime, strEndTime, tSampleDuration))
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrInputInvalid;
	}
	// 2. Load the data 
	if(!loadProcInfo(&cpuSnapBeg, &cpuSnapEnd, strBegTime, strEndTime,CPU_INFO_FILE_0, CPU_INFO_FILE_1))
	{
		sprintf(errmsg, "System Error: Can't find cpu info! \n");
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrRcdNotFound;
	}
	// 3. Compute the result
	if(!computeProcInfo(&ratioRslt, &cpuSnapBeg, &cpuSnapEnd))
	{
		sprintf(errmsg, "System Error: Can't compute out cpu info! \n");
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrUnknownError;
	}

	// 4. Show the result
	if(!showProcInfo(&ratioRslt, local, nTopProcNum, (*optlen-64)))
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "cpu mgr show process");
		return -eAosRc_ResMgrDataOutOfBuffer;
	}

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}

// cpu mgr show threshold
int	cpuMgrShowThreshold(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;
	/* here real start */
	// show threshold 
	int nRslt;
	int nBusyEdgeTime, nBusyThreshold;
	int nNormEdgeTime, nNormThreshold;
	ResMgr_Threshold_t obj_threshold;
	get_CpuMgr_ResMgr_Threshold_App(&obj_threshold);
	nRslt = aos_resmgr_get_threshold(&nBusyEdgeTime, &nBusyThreshold, &nNormEdgeTime, &nNormThreshold, &obj_threshold);
	if(nRslt < 0)
	{
		sprintf(errmsg, "Error[%d]\nOperation Error: input invalid! \n", nRslt);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return nRslt;
	}
	contents << "The busy time period: " << nBusyEdgeTime << "\n"
			 << "The busy threshold  : " << nBusyThreshold << "\n"
			 << "The normal time period: " << nNormEdgeTime << "\n"
			 << "The normal threshold  : " << nNormThreshold << "\n";
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"cpu mgr show threashold");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// cpu mgr set busy threshold
int	cpuMgrSetBusyThreshold(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;
	/* here real start */
	// set the threshold 
	int nRslt;
	if(parms->mIntegers[0] < 1 || parms->mIntegers[0] > 100)
	{
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", (-eAosRc_InvalidParm));
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInvalidBusyThreshold;
	}
	ResMgr_Threshold_t obj_threshold;
	nRslt = aos_resmgr_set_threshold(&obj_threshold, parms->mStrings[0], parms->mIntegers[0], NULL, -1);
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", nRslt);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return nRslt;
	}
	set_CpuMgr_ResMgr_Threshold_App(&obj_threshold);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// cpu mgr set normal threshold
int	cpuMgrSetNormalThreshold(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	// set the threshold 
	int nRslt;
	if(parms->mIntegers[0] < 1 || parms->mIntegers[0] > 100)
	{
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", (-eAosRc_InvalidParm));
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInvalidNormalThreshold;
	}
	ResMgr_Threshold_t obj_threshold;
	nRslt = aos_resmgr_set_threshold(&obj_threshold, NULL, -1, parms->mStrings[0], parms->mIntegers[0]);
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", nRslt);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return nRslt;
	}
	set_CpuMgr_ResMgr_Threshold_App(&obj_threshold);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// API: cpu mgr
int	cpuMgrShowCpuAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);

	char local[CPUMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	char strBegTime[CPUMGR_TIME_LEN_MAX] = "";
	char strEndTime[CPUMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration, tSampleInterval;
	time_t tShowInterval;
	unsigned int index = 0;
	int bRslt = 1, nRslt = 1;
	CpuInfoRecordType arrCpuSnap[RESMGR_SHOW_RES_MAX];
	int nArrCpuSnapSize = 0;
	CpuInfoRcdRatioType arrRatioRslt[RESMGR_SHOW_RES_MAX];

	bRslt &= aos_getFmtTimeStr(strBegTime,parms->mStrings[0]);
	bRslt &= aos_getFmtTimeStr(strEndTime,parms->mStrings[1]);
	tShowInterval = aos_get_interval_sec(parms->mStrings[2], strBegTime, strEndTime);
	bRslt &= ((tShowInterval) > (0) ? (1) : (0));
	if(!bRslt)
	{
		strncpy(errmsg,"System Error: Input invalid!\nYour input may have one of the following three errors:!\n"\
						"1. Input begin time or end time format is illegal!\n"\
						"2. Input interval time format is illegal! \n"\
						"3. Input interval time seconds number interval_time < (EndTime - BegTime)/120 ! \n"
						"4. Input interval time seconds number interval_time < sampling_interval_time ! \n"
						,errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}

	// 1. Validate Input
	tSampleDuration = (time_t)getDurationSec(CPU_INFO_FILE_0);
	tSampleInterval = (time_t)getIntervalSec(CPU_INFO_FILE_0);
	if(!isValidTimeInput(strBegTime, strEndTime, tShowInterval, tSampleDuration, tSampleInterval))
	{
		strncpy(errmsg,"System Error: Input invalid!\nYour input may have one of the following three errors:!\n"\
						"1. Input interval time seconds number interval_time < (EndTime - BegTime)/120 ! \n"\
						"2. Input interval time seconds number interval_time < sampling_interval_time ! \n"
						,errlen-1);
		errmsg[errlen-1] = 0;
		char tmp_errmsg[1024];
		snprintf(tmp_errmsg, sizeof(tmp_errmsg), 
								 "\nstrBegTime[%s], \nstrEndTime[%s], tShowInterval[%ld], "\
								 "tSampleDuration[%ld], tSampleInterval[%ld]\n", 
								  strBegTime, strEndTime, tShowInterval, 
								  tSampleDuration, tSampleInterval);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , tmp_errmsg);
		}
#endif
		return -eAosRc_ResMgrInputInvalid;
	}
	// 2. Load the data (CpuInfoRecordType*)
	if(!loadCpuInfo(arrCpuSnap, &nArrCpuSnapSize, tShowInterval, strBegTime, strEndTime,CPU_INFO_FILE_0, CPU_INFO_FILE_1))
	{
		snprintf(errmsg, errlen-1, "System Error: Can't find cpu infomation! \n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	// 3. Compute the result
	if(!computeCpuInfo(arrRatioRslt, arrCpuSnap, nArrCpuSnapSize))
	{
		snprintf(errmsg, errlen-1, "System Error: Can't compute out cpu info! \n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 4. Show the result 
	if(!showCpuInfoAPI(local, arrRatioRslt, nArrCpuSnapSize - 1, (*optlen-64)))
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "cpu mgr show cpu");
		return -eAosRc_ResMgrUnknownError;
	}

	/* here real end */
	// take care of the string length. 
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"cpu mgr show cpu");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// cpu mgr show process_api <time_start> <time_end> <top_process_num>
int	cpuMgrShowProcessAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	char local[CPUMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	int nTopProcNum = 0;
	char strBegTime[CPUMGR_TIME_LEN_MAX] = "", strEndTime[CPUMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration;
	unsigned int index = 0;
	int bRslt = 1;
	CpuInfoRecordType cpuSnapBeg, cpuSnapEnd;
	ProcCpuInfoRatioType 	ratioRslt;

	bRslt &= aos_getFmtTimeStr(strBegTime,parms->mStrings[0]);
	bRslt &= aos_getFmtTimeStr(strEndTime,parms->mStrings[1]);
	if(!bRslt)
	{
		strncpy(errmsg,"System Error: Input invalid!\n\0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	nTopProcNum = parms->mIntegers[0];
	if(nTopProcNum < 1)
	{
		nTopProcNum = CPUMGR_SHOW_PROC_DEFAULT;
	}

	// 1. Validate Input
	tSampleDuration = getDurationSec(CPU_INFO_FILE_0);
	if(!isValidTimeZone(strBegTime, strEndTime, tSampleDuration))
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	// 2. Load the data 
	if(!loadProcInfo(&cpuSnapBeg, &cpuSnapEnd, strBegTime, strEndTime,CPU_INFO_FILE_0, CPU_INFO_FILE_1))
	{
		snprintf(errmsg, errlen-1, "System Error: Can't find cpu info! \n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	// 3. Compute the result
	if(!computeProcInfo(&ratioRslt, &cpuSnapBeg, &cpuSnapEnd))
	{
		snprintf(errmsg, errlen-1, "System Error: Can't compute out cpu info! \n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 4. Show the result
	if(!showProcInfoAPI(&ratioRslt, local, nTopProcNum, (*optlen-64)))
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "cpu mgr show process");
		return -eAosRc_ResMgrUnknownError;
	}

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}

// cpu mgr show threshold
int	cpuMgrShowThresholdAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;
	/* here real start */
	// show threshold 
	int nRslt;
	int nBusyEdgeTime, nBusyThreshold;
	int nNormEdgeTime, nNormThreshold;
	ResMgr_Threshold_t obj_threshold;
	get_CpuMgr_ResMgr_Threshold_App(&obj_threshold);
	nRslt = aos_resmgr_get_threshold(&nBusyEdgeTime, &nBusyThreshold, &nNormEdgeTime, &nNormThreshold, &obj_threshold);
	if(nRslt)
	{
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", nRslt);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	contents << nBusyEdgeTime << " "
			 << nBusyThreshold << " "
			 << nNormEdgeTime << " "
			 << nNormThreshold << "\n";
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"cpu mgr show threashold");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

int CpuMgr_regCliCmd(void)
{
	int ret = 0;

	reset_CpuMgr_App(&g_theCpuMgrApp);
	ret |= CliUtil_regCliCmd("cpu mgr set record time",cpuMgrSetRecordTime);
	ret |= CliUtil_regCliCmd("cpu mgr set record",cpuMgrSwitchOnOff);
	ret |= CliUtil_regCliCmd("cpu mgr set record stop",cpuMgrSetRecordStop);
	ret |= CliUtil_regCliCmd("cpu mgr show cpu",cpuMgrShowCpu);
	ret |= CliUtil_regCliCmd("cpu mgr show process",cpuMgrShowProcess);
	
	ret |= CliUtil_regCliCmd("cpu mgr show threshold",  cpuMgrShowThreshold);
	ret |= CliUtil_regCliCmd("cpu mgr set busy threshold",  cpuMgrSetBusyThreshold);
	ret |= CliUtil_regCliCmd("cpu mgr set normal threshold",cpuMgrSetNormalThreshold);

	// Simple for API begin
	ret |= CliUtil_regCliCmd("cpu mgr show cpu_api",cpuMgrShowCpuAPI);
	ret |= CliUtil_regCliCmd("cpu mgr show process_api",cpuMgrShowProcessAPI);
	ret |= CliUtil_regCliCmd("cpu mgr show threshold_api",  cpuMgrShowThresholdAPI);
	// Simple for API end

	return ret;
}
