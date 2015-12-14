////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemDiskMgr.cpp
// Description:
//   
//
// Modification History:
// 2007-02-14 Created by CHK
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

#include "rhcUtil/aosDiskMgrApp.h"
#include "rhcUtil/aosResMgrApp.h"
#include "rhcUtil/aosDiskMgrToolkit.h"
#include "rhcUtil/aosResMgrToolkit.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

// switch on/off
// Here: create or kill the thread
// "disk mgr set record [on/off]"
int	
diskMgrSwitchOnOff(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
//	char zRslt[256] = "";
	char zLocal[256] = "";
	/* here real start */
	if(strcmp(parms->mStrings[0], "on") == 0)
	{
		// destroy the old thread first
		if(!diskMgrSwitchOff())
		{
			strncpy(errmsg,"Error: Switch Off Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}

		// switch on process ( include create the thread )
		if(!diskMgrSwitchOn())
		{
			strncpy(errmsg,"Error: Switch On Error! ",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
	}
	else if(strcmp(parms->mStrings[0], "off") == 0)
	{
		// destroy the thread 
		if(!diskMgrSwitchOff())
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
		return -eAosRc_ResMgrUnknownError;
	}

	/* here real end */
	if (CliUtil_checkAndCopy(buf, &index, *optlen, zLocal, strlen(zLocal)) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr set record");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr set record time <interval> <record time>
int	
diskMgrSetRecordTime(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char zLocal[256] = "";

	/* here real start */
	int nRslt = 0;
	nRslt = aos_diskmgr_set_record_time(parms->mNumStrings, parms->mStrings, errmsg, errlen);
	if(nRslt < 0)
	{
		errmsg[errlen-1] = 0;
		return nRslt;
	}
	// switch on 
//	diskMgrSwitchOn();

	/* here real end */
	if (CliUtil_checkAndCopy(buf, &index, *optlen, zLocal, strlen(zLocal)) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr set record time");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr set record stop
int	
diskMgrSetRecordStop(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	// destroy the thread 
	diskMgrSwitchOff();
	/* here real end */
	if (CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data())) < 0)
	{
		sprintf(errmsg, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr set record stop");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr show threshold
int	diskMgrShowThreshold(char *data, unsigned int *optlen, 
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
	get_DiskMgr_ResMgr_Threshold_App(&obj_threshold);
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
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"disk mgr show disk");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr set busy threshold
int	diskMgrSetBusyThreshold(char *data, unsigned int *optlen, 
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
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", (-eAosRc_ResMgrInputInvalid));
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
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
	set_DiskMgr_ResMgr_Threshold_App(&obj_threshold);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// disk mgr set normal threshold
int	diskMgrSetNormalThreshold(char *data, unsigned int *optlen, 
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
		snprintf(errmsg, errlen-1, "Error[%d]\nOperation Error: input invalid! \n", (-eAosRc_ResMgrInputInvalid));
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrInputInvalid;
	}
	ResMgr_Threshold_t obj_threshold;
	nRslt = aos_resmgr_set_threshold(&obj_threshold, NULL, -1, parms->mStrings[0], parms->mIntegers[0]);
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
	set_DiskMgr_ResMgr_Threshold_App(&obj_threshold);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// disk mgr show statistics
int	diskMgrShowStatistics(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[RESMGR_LOCALBUFFER_MAX] = "";
	int nRslt = 0;
	char zRslt[MAX_CMDLINE*2] = ""; 
	/* here real start */
	int nArrSize = 0;
	struct TypeEntry arrTypeInfo[DISKMGR_MAX_TYPE_NUM];
	time_t rcd_time = 0;
	// 1. Load the data (CpuInfoRecordType*)
	if(!loadTypeInfo(arrTypeInfo, &nArrSize, &rcd_time, DISK_INFO_FILE_0, DISK_INFO_FILE_1))
	{
		snprintf(errmsg, errlen-1, "Error: Can't find disk information. Please start disk manager first.\n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 2. Show the result 
	showTypeInfo(local, arrTypeInfo, nArrSize, rcd_time, (*optlen-64));
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		sprintf(zRslt, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr show statistics");
		strncpy(errmsg,zRslt,errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr file type add <type>
int	diskMgrFileTypeAdd(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents, rslt;
	int nRslt = 0;
	/* here real start */
	int bFindType = 0;
	FILE *fp = NULL;
	char readbuf[16];
	char type[16];
	char * zLine = NULL; 
	size_t nLn = 0;

	sprintf(type, "%s\n", parms->mStrings[0]);

	if ((fp = fopen(_DISKMGR_TYPEFILE, "a+")) == NULL)
	{
		strncpy(errmsg,"Error: Can't open file!\n\0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrRcdNotFound;
	}

	readbuf[0] = 0;
	while (getline(&zLine, &nLn, fp) >= 0)
	{
		if (strcmp(zLine, type) == 0)
		{
			bFindType = 1;
			break;
		}
	}
	if(bFindType)
	{
		strncpy(errmsg,"Error: This file type already exists.\n",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DiskMgrTypeExist;
	}
	else // (!bFindType)
	{
		fseek(fp, 0L, SEEK_END);
		if (fputs(type, fp) < 0)
		{
			strncpy(errmsg,"Error: Can't add file type!\n\0",errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
	}
	if(zLine)
	{
		free(zLine);
		zLine = NULL;
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr file type add");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr file type remove <type>
int	diskMgrFileTypeRemove(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents, rslt;
	int nRslt = 0;
	/* here real start */

	// find and remove the string from the file
//	{
//		char zCmdLine[MAX_CMDLINE] = "";
//		strcat(zCmdLine, "sed -i '/^");
//		strcat(zCmdLine, parms->mStrings[0]);
//		strcat(zCmdLine, "/d' ");
//		strcat(zCmdLine, _DISKMGR_TYPEFILE);
//		OmnCliSysCmd::doShell(OmnString(zCmdLine),rslt);
//	}
	{
		FILE *fp = NULL;
		FILE *cpyfp = NULL;
		char *zLine = NULL;
		size_t nLn = 0;
		char type[16];

		if(NULL == parms->mStrings[0])
		{
			strcpy(type, "\n");
		}
		else
		{
			sprintf(type, "%s\n", parms->mStrings[0]);
		}

		if ((fp = fopen(_DISKMGR_TYPEFILE, "r+")) == NULL)
		{
			snprintf(errmsg, errlen-1,"Error: Can't open file! [%s]\n", _DISKMGR_TYPEFILE);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrRcdNotFound;
		}

		if ((cpyfp = fopen(_DISKMGR_TYPEFILE_TMP, "w+")) == NULL)
		{
			snprintf(errmsg, errlen-1,"Error: Can't open file! [%s]\n", _DISKMGR_TYPEFILE_TMP);
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}

		while (getline(&zLine, &nLn, fp) >= 0)
		{
			if (strcmp(zLine, type) != 0)
			{
				fprintf(cpyfp, "%s", zLine);
			}
		}
		if(zLine)
		{
			free(zLine);
			zLine = NULL;
		}
		if(cpyfp)
		{
			fclose(cpyfp);
			cpyfp = NULL;
		}
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}
		remove(_DISKMGR_TYPEFILE);
		// int rename(const char *oldpath, const char *newpath);
		rename(_DISKMGR_TYPEFILE_TMP, _DISKMGR_TYPEFILE);
	}

//	if(strlen(rslt.data()) > 0)
//	{
//		strncpy(errmsg,rslt.data(),errlen-1);
//		errmsg[errlen-1] = 0;
//		return -eAosRc_ResMgrUnknownError;
//	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr file type remove");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr file type show
int	diskMgrFileTypeShow(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents, rslt;
	int nRslt = 0;
	/* here real start */
	FILE *fp = NULL;
	char readbuf[128] = "";

	fp = fopen(_DISKMGR_TYPEFILE, "r");
	if(fp)
	{
		while (fgets(readbuf, sizeof(readbuf), fp) != NULL)
		{
			contents << readbuf;
		}
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}

	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr file type show");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr file type clear
int	diskMgrFileTypeClear(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents, rslt;
	int nRslt = 0;
	/* here real start */
	FILE *fp = NULL;
	if ((fp = fopen(_DISKMGR_TYPEFILE, "w+")) == NULL)
	{
		strncpy(errmsg,"Error: Can't open file!\n\0",errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr file type clear");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// Tool CLIs for Partition the hard disk
///////////////////////////////////////////////////////////////////////////////////
// disk mgr get hdinfo
int	diskMgrGetHDInfo(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[1024*2];
	int nRslt = 0;
	/* here real start */
	struct ros_harddisk_info hd_info;

	if(parms->mNumIntegers < 1)
	{
		ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
		// show harddisk information 
		showHarddiskInfo(local, g_theAPIDiskMgrApp.hd_info, g_theAPIDiskMgrApp.hd_num);
	}
	else
	{
		ros_get_harddisk_info(parms->mIntegers[0], &hd_info);
		// show harddisk information
		showHarddiskInfo(local, &hd_info, 1);
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr get hdinfo");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0; // -eAosRc_Success
}

// disk mgr get partinfo
int	diskMgrGetPartInfo(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[1024*2];
	int nRslt = 0;
	/* here real start */
	struct ros_partition_info part_info;

	// hard disk id	= parms->mIntegers[0];
	// part id 		= parms->mIntegers[1];
	if(parms->mNumIntegers < 1)
	{
		ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
		// show partition information
		showPartitionInfo(local, g_theAPIDiskMgrApp.part_info, g_theAPIDiskMgrApp.part_num);
	}
	if(1 == parms->mNumIntegers)
	{
		ros_get_partition_info(parms->mIntegers[0], parms->mIntegers[1], &part_info);
		// show partition information
		showPartitionInfo(local, &part_info, 1);
	}
	if(2 == parms->mNumIntegers)
	{
		ros_get_partition_info(parms->mIntegers[0], parms->mIntegers[1], &part_info);
		// show partition information
		showPartitionInfo(local, &part_info, 1);
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr get partinfo");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0; // -eAosRc_Success
}

// disk mgr getall folder <path>
int	diskMgrGetFolderInfo(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents, rslt;
	int nRslt = 0;
	/* here real start */

	// find and remove the string from the file
	char zCmdLine[MAX_CMDLINE] = "";
	char zFolderDir[MAX_CMDLINE] = "";
	if(parms->mNumStrings < 1)
	{
		strcpy(zFolderDir, "/");
	}
	else
	{
		const int nStrLen = strlen(parms->mStrings[0]);
		if(nStrLen >= MAX_CMDLINE-1)
		{
			snprintf(errmsg, errlen-1, "Error: Input directory is too long.");
			errmsg[errlen-1] = 0;
			return -eAosRc_ResMgrUnknownError;
		}
		strncpy(zFolderDir, parms->mStrings[0], MAX_CMDLINE);
		if('/' != zFolderDir[nStrLen-1])
		{
			zFolderDir[nStrLen] = '/';
			zFolderDir[nStrLen+1] = 0;
		}
	}
	strcat(zCmdLine, "ls ");
	strcat(zCmdLine, zFolderDir);
	strcat(zCmdLine, " 2>&1");
	OmnCliSysCmd::doShell(OmnString(zCmdLine),rslt);
	if(strlen(rslt.data()) > 0)
	{
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr getall folder");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr format disk
// Description: 
// 			Format the a non-primary hard disk. 
// 		!!!!!!!!!!!!!!!Not Finished yet!!!!!!!!!!!!!!!!!!!
int	diskMgrFormatDisk(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[1024];
	int nRslt = 0;
	/* here real start */
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr format disk");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0; // -eAosRc_Success
}

//	disk mgr mount folder <dev_path> <folder_path>
int	diskMgrMountFolder(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[1024];
	int nRslt = 0;
	/* here real start */
	OmnString systemCmd, contents;
	systemCmd = "";
	systemCmd << "mount " 
			  << parms->mStrings[0] << " " 
			  << parms->mStrings[1] << " 2>&1"; 
	OmnCliSysCmd::doShell(systemCmd, contents);
	strncpy(local, contents.data(), 1024);
	local[1023] = 0;
	if(strlen(local) > 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr mount folder");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr mount folder");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0; // -eAosRc_Success
}

//	disk mgr unmount folder <folder_path>
int	diskMgrUnmountFolder(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[1024];
	int nRslt = 0;
	/* here real start */
	OmnString systemCmd, contents;
	systemCmd = "";
	systemCmd << "umount " 
			  << parms->mStrings[0] << " 2>&1"; 
	OmnCliSysCmd::doShell(systemCmd, contents);
	strncpy(local, contents.data(), 1024);
	local[1023] = 0;
	if(strlen(local) > 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr unmount folder");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr unmount folder");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0; // -eAosRc_Success
}

//=================================================================
// Simple for API begin
//=================================================================
// disk mgr show threshold_api
int	diskMgrShowThresholdAPI(char *data, unsigned int *optlen, 
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
	get_DiskMgr_ResMgr_Threshold_App(&obj_threshold);
	nRslt = aos_resmgr_get_threshold(&nBusyEdgeTime, &nBusyThreshold, &nNormEdgeTime, &nNormThreshold, &obj_threshold);
	if(nRslt)
	{
		sprintf(errmsg, "Error[%d]\nOperation Error: input invalid! \n", nRslt);
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
			 << nBusyThreshold<< " "
			 << nNormEdgeTime << " "
			 << nNormThreshold << "\n";
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"disk mgr show disk");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr show statistics_api
int	diskMgrShowStatisticsAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[RESMGR_LOCALBUFFER_MAX] = "";
	int nRslt = 0;
	char zRslt[MAX_CMDLINE*2] = ""; 
	/* here real start */
	int nArrSize = 0;
	struct TypeEntry arrTypeInfo[DISKMGR_MAX_TYPE_NUM];
	time_t rcd_time = 0;
	// 1. Load the data (CpuInfoRecordType*)
	if(!loadTypeInfo(arrTypeInfo, &nArrSize, &rcd_time, DISK_INFO_FILE_0, DISK_INFO_FILE_1))
	{
		snprintf(errmsg, errlen-1, "Error: Can't find disk information. Please start disk manager first.\n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}

	// 2. Show the result 
	showTypeInfo(local, arrTypeInfo, nArrSize, rcd_time, (*optlen-64));
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		sprintf(zRslt, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr show statistics");
		strncpy(errmsg,zRslt,errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0;
}

// disk mgr getall hard disk info_api
int	diskMgrGetAllHardDiskInfoAPI(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	char local[1024*2];
	int nRslt = 0;
	/* here real start */
//	struct ros_harddisk_info hd_info;

	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	// show harddisk information 
	showHarddiskInfo(local, g_theAPIDiskMgrApp.hd_info, g_theAPIDiskMgrApp.hd_num);

	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	if(nRslt < 0)
	{
		snprintf(errmsg, errlen-1, RESMGR_BUF_EXPLOD_MSG_FMT, "disk mgr get hdinfo");
		errmsg[errlen-1] = 0;
		return -eAosRc_ResMgrUnknownError;
	}
	*optlen = index;
    return 0; // -eAosRc_Success
}

//=================================================================
// Simple for API end
//=================================================================

int DiskMgr_regCliCmd(void)
{
	int ret;

	reset_DiskMgr_App(&g_theDiskMgrApp);
	reset_ros_sys_harddisk_info_app_t(&g_theAPIDiskMgrApp);
	
	ret = 0;
	ret |= CliUtil_regCliCmd("disk mgr set record",diskMgrSwitchOnOff);
	ret |= CliUtil_regCliCmd("disk mgr set record time",diskMgrSetRecordTime);
	ret |= CliUtil_regCliCmd("disk mgr set record stop",diskMgrSetRecordStop);
	ret |= CliUtil_regCliCmd("disk mgr show statistics",diskMgrShowStatistics);	
	ret |= CliUtil_regCliCmd("disk mgr file type add",diskMgrFileTypeAdd);
	ret |= CliUtil_regCliCmd("disk mgr file type remove",diskMgrFileTypeRemove);
	ret |= CliUtil_regCliCmd("disk mgr file type show",diskMgrFileTypeShow);
	ret |= CliUtil_regCliCmd("disk mgr file type clear",diskMgrFileTypeClear);

	ret |= CliUtil_regCliCmd("disk mgr show threshold",  diskMgrShowThreshold);
	ret |= CliUtil_regCliCmd("disk mgr set busy threshold",  diskMgrSetBusyThreshold);
	ret |= CliUtil_regCliCmd("disk mgr set normal threshold",diskMgrSetNormalThreshold);

	ret |= CliUtil_regCliCmd("disk mgr get hdinfo",diskMgrGetHDInfo);
	ret |= CliUtil_regCliCmd("disk mgr get partinfo",diskMgrGetPartInfo);
	ret |= CliUtil_regCliCmd("disk mgr getall partition",diskMgrGetPartInfo);
	ret |= CliUtil_regCliCmd("disk mgr getall folder",diskMgrGetFolderInfo);
	ret |= CliUtil_regCliCmd("disk mgr format disk",diskMgrFormatDisk);

	ret |= CliUtil_regCliCmd("disk mgr mount folder",  diskMgrMountFolder);
	ret |= CliUtil_regCliCmd("disk mgr unmount folder",  diskMgrUnmountFolder);

	// Simple for API begin
	ret |= CliUtil_regCliCmd("disk mgr show statistics_api",diskMgrShowStatisticsAPI);
	ret |= CliUtil_regCliCmd("disk mgr show threshold_api",  diskMgrShowThresholdAPI);
	ret |= CliUtil_regCliCmd("disk mgr getall hard disk info_api",diskMgrGetAllHardDiskInfoAPI);
	// Simple for API end

	return ret;
}
