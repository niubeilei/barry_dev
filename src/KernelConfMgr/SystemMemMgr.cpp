////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemMemMgr.cpp
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
#include "XmlParser/XmlItem.h"

#include "rhcUtil/aosMemMgrApp.h"
#include "rhcUtil/aosResMgrApp.h"
#include "rhcUtil/aosMemMgrToolkit.h"
#include "rhcUtil/aosResMgrToolkit.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

// switch on/off
// Here: create or kill the thread
// "mem mgr set record [on/off]"
int	
memMgrSwitchOnOff(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char zLocal[256] = "";
	/* here real start */
	if(strcmp(parms->mStrings[0], "on") == 0)
	{
		// destroy the old thread first
		if(!memMgrSwitchOff())
		{
			strncpy(errmsg,"Error: Switch Off Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
		// switch on process ( include create the thread )
		// at least set: 
		//			g_theCpuMgrApp.m_interval and g_theCpuMgrApp.m_duration these two values
		if(!memMgrSwitchOn())
		{
			strncpy(errmsg,"Error: Switch On Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
	}
	else if(strcmp(parms->mStrings[0], "off") == 0)
	{
		// destroy the old thread first
		if(!memMgrSwitchOff())
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
	CliUtil_checkAndCopy(buf, &index, *optlen, zLocal, strlen(zLocal));
	*optlen = index;
//	*optlen = 0;
    return 0;
}

// mem mgr set record time <time_start> <time_end>
int	
memMgrSetRecordTime(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char zLocal[256] = "";

	/* here real start */
	int nRslt = 0;
	nRslt = aos_memmgr_set_record_time(parms->mNumStrings, parms->mStrings, errmsg, errlen);
	if(nRslt < 0)
	{
		errmsg[errlen-1] = 0;
		return nRslt;
	}
	// switch on process ( include create the thread )
//	memMgrSwitchOn();

	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, zLocal, strlen(zLocal));
	*optlen = index;
    return 0;
}

// mem mgr set record stop
int	memMgrSetRecordStop(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	// destroy the thread 
	memMgrSwitchOff();
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// mem mgr show mem <time_start> <time_end>
int	memMgrShowMem(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);

	char local[MEMMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	char strBegTime[RESMGR_TIME_LEN_MAX] = "";
	char strEndTime[RESMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration, tSampleInterval;
	time_t tShowInterval;
	unsigned int index = 0;
	int bRslt = 1;
	MemInfoRecordType arrMemSnap[RESMGR_SHOW_RES_MAX];
	int nArrMemSnapSize = 0;
//	MemInfoRcdRatioType arrRatioRslt[RESMGR_SHOW_RES_MAX];

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
	tSampleDuration = (time_t)getDurationSec(MEM_INFO_FILE_0);
	tSampleInterval = (time_t)getIntervalSec(MEM_INFO_FILE_0);
	if(!isValidTimeInput(strBegTime, strEndTime, tShowInterval, tSampleDuration, tSampleInterval))
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}

	// 2. Load the data
	if(!loadMemInfo(arrMemSnap, &nArrMemSnapSize, tShowInterval, strBegTime, strEndTime, MEM_INFO_FILE_0, MEM_INFO_FILE_1))
	{
		sprintf(errmsg, "System Error: Can't find memory info! \n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrRcdNotFound;
	}

//#ifdef _DEBUG_
//	printf("\n LoadMemInfo: %s\n", MEM_INFO_FILE_0);
//#endif 
//	// 3. Compute the result
//	if(!computeMemInfo(ratioRslt, memSnapBeg, memSnapEnd))
//	{
//		strncpy(errmsg,"System Error: Can't compute out memory info! \nFile:SystemCpuMgr.cpp\nLine:275\n\0",errlen-1);
//		errmsg[errlen-1] = 0;
//		return -eAosRc_ResMgrUnknownError;
//	}

	// 4. Show the result
	if(!showMemInfo(local, arrMemSnap, nArrMemSnapSize - 1, (*optlen-64)))
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show mem");
		return -eAosRc_ResMgrDataOutOfBuffer;
	}

	/* here real end */
	if (CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local)) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show mem");
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// mem mgr show process <time_start> <time_end> <top_process_num>
int	memMgrShowProcess(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	char local[RESMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	int nTopProcNum = 0;
	char strBegTime[RESMGR_TIME_LEN_MAX] = "", strEndTime[RESMGR_TIME_LEN_MAX] = "";
	char strPreTime[RESMGR_TIME_LEN_MAX] = "", strCurTime[RESMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration;
	time_t timeInterval;
	unsigned int index = 0;
	int bRslt = 1;
	MemInfoRecordType arrMemSnap[RESMGR_SHOW_RES_MAX];
	int nArrSize = 0;
	ProcMemInfoRatioType 	ratioRslt, ratioRsltTmp;

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
		nTopProcNum = RESMGR_SHOW_PROC_DEFAULT;
	}

	const int nInputTopProcNum = nTopProcNum;
	nTopProcNum = RESMGR_MINIMUM(nTopProcNum, MAX_PROCESS_NUM);

	// 1. Validate Input
	tSampleDuration = getDurationSec(MEM_INFO_FILE_0);

	if(!isValidTimeZone(strBegTime, strEndTime, tSampleDuration))
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	// 2.0 Copy the memory record file to a new place
	OmnString strCmdLine;
	strCmdLine << "cp -f " << MEM_INFO_FILE_0 << " " << MEM_INFO_FILE_0_TMP << "; " 
				<< "cp -f " << MEM_INFO_FILE_1 << " " << MEM_INFO_FILE_1_TMP << "; " ;
	OmnCliSysCmd::doShell(strCmdLine,rslt);

	timeInterval = (time_t)getIntervalSec(MEM_INFO_FILE_0_TMP);
	if(timeInterval < 1)
	{
		strncpy(errmsg,"System Error: memory record file is modified!! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	aos_get_next_interval_time_str(strPreTime, strBegTime, timeInterval);
	if(!aos_get_next_computable_time_str(strCurTime, strBegTime, timeInterval))
	{
		sprintf(errmsg, "System Error: Can't compute time! \n");
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrUnknownError;
	}
	if(strcmp(strCurTime, strEndTime) > 0)
	{
		strcpy(strCurTime, strEndTime);
	}

	reset_ProcMemInfoRatioType(&ratioRslt);
	while(strcmp(strPreTime, strEndTime) < 0)
	{
		// 2. Load the data 
		if(!loadProcMemInfo(arrMemSnap, &nArrSize, timeInterval, strPreTime, strCurTime, MEM_INFO_FILE_0_TMP, MEM_INFO_FILE_1_TMP))
		{
			sprintf(errmsg, "System Error: Can't find mem info! \n");
			errmsg[errlen-1] = 0;
	#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
			}
	#endif
			return -eAosRc_ResMgrUnknownError;
		}

		// 3. Compute the result
		if(!computeProcMemInfo(&ratioRsltTmp, arrMemSnap, nArrSize, nTopProcNum))
		{
			snprintf(errmsg, errlen-1, "System Error: Can't compute out memory info! \n");
			errmsg[errlen-1] = 0;
	#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
			}
	#endif
			return -eAosRc_ResMgrUnknownError;
		}

		// generate the total result
		append_ProcMemInfoRatioType(&ratioRslt, &ratioRsltTmp); 
		/*
		 * why I sort it here? because we may always throw away some data in this while loop.
		 */
		sort_ProcMemInfoRatioType(&ratioRslt);
		strcpy(strPreTime, strCurTime);

		if(!aos_get_next_computable_time_str(strCurTime, strPreTime, timeInterval))
		{
			snprintf(errmsg, errlen-1, "System Error: Can't compute time! \n");
			errmsg[errlen-1] = 0;
	#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
			}
	#endif
			return -eAosRc_ResMgrUnknownError;
		}
		if(strcmp(strCurTime, strEndTime) > 0)
		{
			strcpy(strCurTime, strEndTime);
		}
	}
	// sort_ProcMemInfoRatioType(&ratioRslt);

	// 4. Show the result
	if(!showProcMemInfo(local, &ratioRslt, strBegTime, strEndTime, nInputTopProcNum, (*optlen-64)))
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show process");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 6. Remove the temperory memory record file
	strCmdLine = "";
	strCmdLine << "rm -f " << MEM_INFO_FILE_0_TMP << "; " 
				<< "rm -f " << MEM_INFO_FILE_1_TMP << "; " ;
	OmnCliSysCmd::doShell(strCmdLine,rslt);
	/* here real end */

	if (CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local)) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show process");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
	return 0;
}

// mem mgr show threshold
int	memMgrShowThreshold(char *data, unsigned int *optlen, 
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
	get_MemMgr_ResMgr_Threshold_App(&obj_threshold);
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
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"mem mgr show mem");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// mem mgr set busy threshold
int	memMgrSetBusyThreshold(char *data, unsigned int *optlen, 
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
	set_MemMgr_ResMgr_Threshold_App(&obj_threshold);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// mem mgr set normal threshold
int	memMgrSetNormalThreshold(char *data, unsigned int *optlen, 
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
	ResMgr_Threshold_t obj_threshold_app;
	nRslt = aos_resmgr_set_threshold(&obj_threshold_app, NULL, -1, parms->mStrings[0], parms->mIntegers[0]);
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
	set_MemMgr_ResMgr_Threshold_App(&obj_threshold_app);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

//=================================================================
// Simple for API begin
//=================================================================
// mem mgr show mem_api <time_start> <time_end>
int	memMgrShowMemAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);

	char local[MEMMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	char strBegTime[RESMGR_TIME_LEN_MAX] = "";
	char strEndTime[RESMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration, tSampleInterval;
	time_t tShowInterval;
	unsigned int index = 0;
	int bRslt = 1;
	MemInfoRecordType arrMemSnap[RESMGR_SHOW_RES_MAX];
	int nArrMemSnapSize = 0;

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
		return -eAosRc_ResMgrUnknownError;
	}

	// 1. Validate Input
	tSampleDuration = (time_t)getDurationSec(MEM_INFO_FILE_0);
	tSampleInterval = (time_t)getIntervalSec(MEM_INFO_FILE_0);
	if(!isValidTimeInput(strBegTime, strEndTime, tShowInterval, tSampleDuration, tSampleInterval))
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 2. Load the data
	if(!loadMemInfo(arrMemSnap, &nArrMemSnapSize, tShowInterval, strBegTime, strEndTime, MEM_INFO_FILE_0, MEM_INFO_FILE_1))
	{
		sprintf(errmsg, "System Error: Can't find memory info! \n");
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
	if(!showMemInfoAPI(local, arrMemSnap, nArrMemSnapSize - 1, (*optlen-64)))
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show mem");
		return -eAosRc_ResMgrUnknownError;
	}

	/* here real end */
	if (CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local)) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show mem");
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// mem mgr show process_api <time_start> <time_end> <top_process_num>
int	memMgrShowProcessAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	char local[RESMGR_LOCALBUFFER_MAX] = "";
	/* here real start */
	int nTopProcNum = 0;
	char strBegTime[RESMGR_TIME_LEN_MAX] = "", strEndTime[RESMGR_TIME_LEN_MAX] = "";
	char strPreTime[RESMGR_TIME_LEN_MAX] = "", strCurTime[RESMGR_TIME_LEN_MAX] = "";
	time_t tSampleDuration;
	time_t timeInterval;
	unsigned int index = 0;
	int bRslt = 1;
	MemInfoRecordType arrMemSnap[RESMGR_SHOW_RES_MAX];
	int nArrSize = 0;
	ProcMemInfoRatioType 	ratioRslt, ratioRsltTmp;

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
		nTopProcNum = RESMGR_SHOW_PROC_DEFAULT;
	}

	const int nInputTopProcNum = nTopProcNum;
	nTopProcNum = RESMGR_MINIMUM(nTopProcNum, MAX_PROCESS_NUM);

	// 1. Validate Input
	tSampleDuration = getDurationSec(MEM_INFO_FILE_0);

	if(!isValidTimeZone(strBegTime, strEndTime, tSampleDuration))
	{
		strncpy(errmsg,"Error: Invalid Input! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	// 2.0 Copy the memory record file to a new place
	OmnString strCmdLine;
	strCmdLine << "cp -f " << MEM_INFO_FILE_0 << " " << MEM_INFO_FILE_0_TMP << "; " 
				<< "cp -f " << MEM_INFO_FILE_1 << " " << MEM_INFO_FILE_1_TMP << "; " ;
	OmnCliSysCmd::doShell(strCmdLine,rslt);

	timeInterval = (time_t)getIntervalSec(MEM_INFO_FILE_0_TMP);
	if(timeInterval < 1)
	{
		strncpy(errmsg,"System Error: memory record file is modified!! \0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	aos_get_next_interval_time_str(strPreTime, strBegTime, timeInterval);
	if(!aos_get_next_computable_time_str(strCurTime, strBegTime, timeInterval))
	{
		sprintf(errmsg, "System Error: Can't compute time! \n");
		errmsg[errlen-1] = 0;
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		return -eAosRc_ResMgrUnknownError;
	}
	if(strcmp(strCurTime, strEndTime) > 0)
	{
		strcpy(strCurTime, strEndTime);
	}

	reset_ProcMemInfoRatioType(&ratioRslt);
	while(strcmp(strPreTime, strEndTime) < 0)
	{
		// 2. Load the data 
		if(!loadProcMemInfo(arrMemSnap, &nArrSize, timeInterval, strPreTime, strCurTime, MEM_INFO_FILE_0_TMP, MEM_INFO_FILE_1_TMP))
		{
			sprintf(errmsg, "System Error: Can't find mem info! \n");
			errmsg[errlen-1] = 0;
	#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
			}
	#endif
			return -eAosRc_ResMgrUnknownError;
		}

		// 3. Compute the result
		if(!computeProcMemInfo(&ratioRsltTmp, arrMemSnap, nArrSize, nTopProcNum))
		{
			snprintf(errmsg, errlen-1, "System Error: Can't compute out memory info! \n");
			errmsg[errlen-1] = 0;
	#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
			}
	#endif
			return -eAosRc_ResMgrUnknownError;
		}

		// generate the total result
		append_ProcMemInfoRatioType(&ratioRslt, &ratioRsltTmp); 
		/*
		 * why I sort it here? because we may always throw away some data in this while loop.
		 */
		sort_ProcMemInfoRatioType(&ratioRslt);
		strcpy(strPreTime, strCurTime);

		if(!aos_get_next_computable_time_str(strCurTime, strPreTime, timeInterval))
		{
			snprintf(errmsg, errlen-1, "System Error: Can't compute time! \n");
			errmsg[errlen-1] = 0;
	#ifdef _DEBUG_RES_MGR_TRACE_ON_
			if(_DEBUG_RES_MGR_TRACE_ON_)
			{
				aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
			}
	#endif
			return -eAosRc_ResMgrUnknownError;
		}
		if(strcmp(strCurTime, strEndTime) > 0)
		{
			strcpy(strCurTime, strEndTime);
		}
	}
	// sort_ProcMemInfoRatioType(&ratioRslt);

	// 4. Show the result
	if(!showProcMemInfoAPI(local, &ratioRslt, strBegTime, strEndTime, nInputTopProcNum, (*optlen-64)))
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show process");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 6. Remove the temperory memory record file
	strCmdLine = "";
	strCmdLine << "rm -f " << MEM_INFO_FILE_0_TMP << "; " 
				<< "rm -f " << MEM_INFO_FILE_1_TMP << "; " ;
	OmnCliSysCmd::doShell(strCmdLine,rslt);
	/* here real end */

	if (CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local)) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "mem mgr show process");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
	return 0;
}

// mem mgr show threshold_api
int	memMgrShowThresholdAPI(char *data, unsigned int *optlen, 
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
	get_MemMgr_ResMgr_Threshold_App(&obj_threshold);
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
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"mem mgr show mem");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// mem mgr get proc mem usage_api
int	memMgrGetProcMemUsageAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;
	int nRslt;
	/* here real start */
	//!!!!!!!!!!!!!!!!!!!!!!
	// NOT FINISHED !!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!
	contents << 123456 ;

	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"mem mgr get proc mem usage_api");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// mem mgr get app mem usage_api
int	memMgrGetAppMemUsageAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;
	int nRslt;
	/* here real start */
	//!!!!!!!!!!!!!!!!!!!!!!
	// NOT FINISHED !!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!
	contents << 123456 ;
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"mem mgr get app mem usage_api");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// mem mgr get mem usage_api
int	memMgrGetMemUsageAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;
	int nRslt;
	/* here real start */
	//!!!!!!!!!!!!!!!!!!!!!!
	// NOT FINISHED !!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!
	contents << 123456 ;
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"mem mgr get mem usage_api");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}
//=================================================================
// Simple for API end
//=================================================================

int MemMgr_regCliCmd(void)
{
	int ret;

	reset_MemMgr_App(&g_theMemMgrApp);
	ret = CliUtil_regCliCmd("mem mgr set record",memMgrSwitchOnOff);
	ret |= CliUtil_regCliCmd("mem mgr set record time",memMgrSetRecordTime);
	ret |= CliUtil_regCliCmd("mem mgr set record stop",memMgrSetRecordStop);
	ret |= CliUtil_regCliCmd("mem mgr show mem",memMgrShowMem);
	ret |= CliUtil_regCliCmd("mem mgr show process",memMgrShowProcess);

	ret |= CliUtil_regCliCmd("mem mgr show threshold",  memMgrShowThreshold);
	ret |= CliUtil_regCliCmd("mem mgr set busy threshold",  memMgrSetBusyThreshold);
	ret |= CliUtil_regCliCmd("mem mgr set normal threshold",memMgrSetNormalThreshold);

	// Simple for API begin
	ret |= CliUtil_regCliCmd("mem mgr show cpu_api",memMgrShowMemAPI);
	ret |= CliUtil_regCliCmd("mem mgr show process_api",memMgrShowProcessAPI);
	ret |= CliUtil_regCliCmd("mem mgr show threshold_api",  memMgrShowThresholdAPI);
	ret |= CliUtil_regCliCmd("mem mgr get proc mem usage_api",  memMgrGetProcMemUsageAPI);
	ret |= CliUtil_regCliCmd("mem mgr get app mem usage_api",  memMgrGetAppMemUsageAPI);
	ret |= CliUtil_regCliCmd("mem mgr get mem usage_api",  memMgrGetMemUsageAPI);
	// Simple for API end

	return ret;
}
