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
#include "rhcUtil/aosMemMgrApp.h"
#include "rhcUtil/aosDiskMgrApp.h"
// res mgr show config
// Description: 
// 		To show the resource manager¡¯s configuration.
int	resMgrShowConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	int nRslt;
	/* here real start */
	// 1. CPU manager module
	contents << "CPU manager configuration information\n" 
		<< "--------------------------------" 
		<< "\n\t recording duration   : " << g_theCpuMgrApp.m_duration
		<< "\n\t recording interval   : " << g_theCpuMgrApp.m_interval
		<< "\n\t recording item number: " << g_theCpuMgrApp.m_maxr
		<< "\n\t rise alarm threshold : " << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio  << "%"
		<< "\n\t rise alarm edge sec  : " << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime 
		<< "\n\t fall normal threshold: " << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio << "%"
		<< "\n\t fall normal edge sec : " << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime 
		<< "\n\n"; 
	// 2. memory manager module
	contents << "Memory manager configuration information\n" 
		<< "--------------------------------" 
		<< "\n\t recording duration   : " << g_theMemMgrApp.m_duration
		<< "\n\t recording interval   : " << g_theMemMgrApp.m_interval
		<< "\n\t recording item number: " << g_theMemMgrApp.m_maxr
		<< "\n\t rise alarm threshold : " << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio  << "%"
		<< "\n\t rise alarm edge sec  : " << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime 
		<< "\n\t fall normal threshold: " << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio << "%"
		<< "\n\t fall normal edge sec : " << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime 
		<< "\n\n"; 
	// 3. disk manager module
	contents << "Disk manager configuration information\n" 
		<< "--------------------------------" 
		<< "\n\t recording duration   : " << g_theDiskMgrApp.m_duration
		<< "\n\t recording interval   : " << g_theDiskMgrApp.m_interval
		<< "\n\t recording item number: " << g_theDiskMgrApp.m_maxr
		<< "\n\t rise alarm threshold : " << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio  << "%"
		<< "\n\t rise alarm edge sec  : " << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime 
		<< "\n\t fall normal threshold: " << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio << "%"
		<< "\n\t fall normal edge sec : " << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime 
		<< "\n\n"; 
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"res mgr show config");
		errmsg[errlen-1] = 0;
		return -1;
	}
	*optlen = index;
    return 0;
}

// res mgr load config
// Description 
//		Load the last saved resource manager configuration to the current running system. 
int	resMgrLoadConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	int nRslt;
	/* here real start */
	// 1. CPU manager module
	// 2. memory manager module
	// 3. disk manager module
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"res mgr load config");
		errmsg[errlen-1] = 0;
		return -1;
	}
	*optlen = index;
    return 0;
}

// res mgr save config
// Description 
// 		Save the current running configuration to the configuration file. 
int	resMgrSaveConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	int nRslt;
	/* here real start */
	// 1. CPU manager module
	contents << "<Cmd>"
			 << "cpu mgr set record time " << g_theCpuMgrApp.m_interval << " " << g_theCpuMgrApp.m_duration
			 << "</Cmd>\n";
	contents << "<Cmd>"
			 << "cpu mgr set busy threshold   " << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime 
			 << " "							  << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio
			 << "</Cmd>\n";
	contents << "<Cmd>"
			 << "cpu mgr set normal threshold " << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime 
			 << " "							  << g_theCpuMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio 
			 << "</Cmd>\n";

	// 2. memory manager module
	contents << "<Cmd>"
			 << "mem mgr set record time " << g_theCpuMgrApp.m_interval << " " << g_theMemMgrApp.m_duration
			 << "</Cmd>\n";
	contents << "<Cmd>"
			 << "mem mgr set busy threshold   " << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime 
			 << " "							  << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio
			 << "</Cmd>\n";
	contents << "<Cmd>"
			 << "mem mgr set normal threshold " << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime 
			 << " "							  << g_theMemMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio 
			 << "</Cmd>\n";

	// 3. disk manager module
	contents << "<Cmd>"
			 << "disk mgr set record time " << g_theCpuMgrApp.m_interval << " " << g_theDiskMgrApp.m_duration
			 << "</Cmd>\n";
	contents << "<Cmd>"
			 << "disk mgr set busy threshold   " << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime 
			 << " "							  << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio
			 << "</Cmd>\n";
	contents << "<Cmd>"
			 << "disk mgr set normal threshold " << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime 
			 << " "							  << g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio 
			 << "</Cmd>\n";

	// 3.1. disk manager file type show
	FILE *fp = NULL;
	char readbuf[128] = "";

	contents << "<Cmd>"
			 << "disk mgr file type clear "
			 << "</Cmd>\n";
	fp = fopen(_DISKMGR_TYPEFILE, "r");
	if(fp)
	{
		while (fgets(readbuf, sizeof(readbuf), fp) != NULL)
		{
			if('\n' == readbuf[strlen(readbuf)-1])
			{
				readbuf[strlen(readbuf)-1] = 0;
			}
			contents << "<Cmd>" 
					 << "disk mgr file type add " << readbuf
					 << "</Cmd>\n";
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
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"res mgr save config");
		errmsg[errlen-1] = 0;
		return -1;
	}
	*optlen = index;
    return 0;
}

// res mgr clear config
// Description 
// 		Clear the current running configuration. After this operation the current resource manager system configuration will be back to the default operation status. 
int	resMgrClearConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	int nRslt;
	/* here real start */
	// 1. CPU manager module
	reset_config_CpuMgr_App(&g_theCpuMgrApp);
	// 2. memory manager module
	reset_config_MemMgr_App(&g_theMemMgrApp);
	// 3. disk manager module
	reset_config_DiskMgr_App(&g_theDiskMgrApp);
	/* here real end */
	nRslt = CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	if(nRslt < 0)
	{
		sprintf(errmsg,RESMGR_BUF_EXPLOD_MSG_FMT,"res mgr clear config");
		errmsg[errlen-1] = 0;
		return -1;
	}
	*optlen = index;
    return 0;
}

//////////////////////////////////////////////////////////////////////
// System nice setting function testing begin
//////////////////////////////////////////////////////////////////////
#include "rhcUtil/aosResMgrApp.h"

#ifdef _DEBUG_
	// only for system nice testing
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <sys/errno.h>
#endif
// system nice set <pid> <nice-value>
int system_nice_set(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	OmnString contents;

	/* here real start */
	int nRslt = 0;
	int nPid = 1, nNiceVal = 0; 
	nPid 	= parms->mIntegers[0];
	nNiceVal= parms->mIntegers[1];
	nRslt = aos_nice(nPid, nNiceVal);
	if(nRslt < 0)
	{
		strncpy(errmsg,strerror(errno), errlen);
		errmsg[errlen-1] = 0;
		return -1;
	}
#ifdef _DEBUG_
	if(nRslt)
	{
		sprintf(errmsg,"result illegal!! \nInpuNice(%d) RealNice(%d)\n", nNiceVal, getpriority(PRIO_PROCESS, nPid));
		errmsg[errlen-1] = 0;
		return -1;
	}
	else
	{
		sprintf(errmsg,"result legal!! \nInpuNice(%d) RealNice(%d)\n", nNiceVal, getpriority(PRIO_PROCESS, nPid));
		errmsg[errlen-1] = 0;
		return -1;
	}
#endif
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}
//////////////////////////////////////////////////////////////////////
// System nice setting function testing end
//////////////////////////////////////////////////////////////////////

int ResMgr_regCliCmd(void)
{
	int ret = 0;
	
	ros_res_mgr_init();

	// reset_ResMgr_App(&g_resmgr_sigactions);

	ret |= CliUtil_regCliCmd("res mgr show config",resMgrShowConfig);
	ret |= CliUtil_regCliCmd("res mgr load config",resMgrLoadConfig);
	ret |= CliUtil_regCliCmd("res mgr save config",resMgrSaveConfig);
	ret |= CliUtil_regCliCmd("res mgr clear config",resMgrClearConfig);
	ret |= CliUtil_regCliCmd("system nice set",system_nice_set);

	return ret;
}
