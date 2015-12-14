////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
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
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

// for local function loadConfig
#include "rhcUtil/aosResMgrToolkit.h"

#include "Global.h"

#define SNMP_NAME "snmpd"
#define SNMPAGNT_CONF_FILE_FULL_PATH "/usr/local/share/snmp/snmpd.conf"

// set a global variable
snmpConfigEntry_t * sgSnmpEntryPtr;

//////////////////////////////////////////////////////////////////
// Local tool functions
// Done by CHK
// Start
//////////////////////////////////////////////////////////////////
// reset the global object sgSnmpEntryPtr
// return 1 for success
// return 0 for fails
int resetSnmpConfigEntry(snmpConfigEntry_t **localSnmpEntryPtrPtr, char *errmsg, const int errlen)
{
	snmpConfigEntry_t *localSnmpEntryPtr = (*localSnmpEntryPtrPtr);
	if(NULL == localSnmpEntryPtr)
	{
		localSnmpEntryPtr = (snmpConfigEntry_t *) malloc(sizeof(snmpConfigEntry_t));
	}
	if(localSnmpEntryPtr)
	{
		memset(localSnmpEntryPtr, 0, sizeof(snmpConfigEntry_t));
	
		localSnmpEntryPtr->zLoadTime[0] = 0;
		localSnmpEntryPtr->zContent[0] = 0;
	}
	else
	{
		sprintf(errmsg,"System Error: Can't allocate memory!\n");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return 0; // return FALSE
	}
	(*localSnmpEntryPtrPtr) = localSnmpEntryPtr;
	return 1;
}

// destroy the global object localSnmpEntryPtr
// return 1 for success
// return 0 for fails
int destroySnmpConfigEntry(snmpConfigEntry_t *localSnmpEntryPtr, char *errmsg, const int errlen)
{
	if(localSnmpEntryPtr)
	{
		free(localSnmpEntryPtr);
		localSnmpEntryPtr = NULL;
	}
	return 1;
}

// load the global object localSnmpEntryPtr from the local machine
// return 1 for success
// return 0 for fails
int loadSnmpConfig(snmpConfigEntry_t * const localSnmpEntryPtr, char *errmsg, const int errlen)
{
	// SNMPAGNT_CONF_FILE_FULL_PATH
	char * line = NULL;
	size_t len = 0;
	ssize_t read = 1;
	FILE * fp = NULL;


	if(NULL == localSnmpEntryPtr)
	{
		sprintf(errmsg,"System Error: Illegal pointer passed in!\n" );
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return 0; // return FALSE
	}

	fp = fopen(SNMPAGNT_CONF_FILE_FULL_PATH, "r");
	if(!fp)
	{
		sprintf(errmsg,"Error: Can't open file [%s] \n", SNMPAGNT_CONF_FILE_FULL_PATH );
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
		}
#endif
		errmsg[errlen-1] = 0;
		return 0; // return FALSE
	}

	localSnmpEntryPtr->zContent[0] = 0;
	while (read > 0) 
	{
		read = getline(&line, &len, fp);
		if(read < 0)
		{
			break;
		}
		if(strcmp(line, "#       sec.name  source          community\n") == 0)
		{
			strcat(localSnmpEntryPtr->zContent, line);
			read = getline(&line, &len, fp);
			if(read < 0)
			{
				break;
			}
			strcat(localSnmpEntryPtr->zContent, line);
		}
	}

	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	getLocalTime(localSnmpEntryPtr->zLoadTime, SNMPAGNT_TIME_MAX_LEN, "%Y%m%d-%T");

	if(line)
	{
		free(line);
		line = NULL;
	}
	return 1;// return TRUE
}

//////////////////////////////////////////////////////////////////
// Local tool functions
// End
//////////////////////////////////////////////////////////////////

// snmp agent start
int	snmpAgentStart(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	OmnString strCmdLine;

	// initialize and reset
	if(!resetSnmpConfigEntry(&sgSnmpEntryPtr, errmsg, errlen))
	{
		errmsg[errlen-1] = 0;
		return -1;
	}

	strCmdLine << SNMP_NAME;
	OmnCliSysCmd::doShell(strCmdLine,rslt);

	if(strlen(rslt.data()) > 0)
	{
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// snmp agent stop
int	snmpAgentStop(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	OmnString strCmdLine;

	// destroy!!!
	destroySnmpConfigEntry(sgSnmpEntryPtr, errmsg, errlen);

	strCmdLine << "pkill " << SNMP_NAME;
	OmnCliSysCmd::doShell(strCmdLine,rslt);

	if(strlen(rslt.data()) > 0)
	{
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

// snmp show config
int	snmpShowConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;

	OmnString contents;
	/* here real start */
	OmnString strCmdLine;

	// initialize and reset
	if(!resetSnmpConfigEntry(&sgSnmpEntryPtr, errmsg, errlen))
	{
		errmsg[errlen-1] = 0;
		return 0;
	}
	// load Config data
	if(!loadSnmpConfig(sgSnmpEntryPtr, errmsg, errlen))
	{
		errmsg[errlen-1] = 0;
		return -1;
	}

	contents << "SNMP agent information\n----------------------------\n" 
			<< sgSnmpEntryPtr->zContent
			<< "\n";
/*			<< "\t    status : " \
//			<< sgDhcp.status << "\n\t" << "lease time : " << sgDhcp.dft_leaseTime \
//			<< "\n\t    router : " << sgDhcp.router << "\n\t       dns : " << sgDhcp.dns \
//			<< "\n\t  ip block : " << sgDhcp.ip1 << " " << sgDhcp.ip2 \
			<< "\n\t bind list : \n" ;*/

	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), strlen(contents.data()));
	*optlen = index;
    return 0;
}

int Snmp_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("snmp agent start",snmpAgentStart);
	ret |= CliUtil_regCliCmd("snmp agent stop",snmpAgentStop);
	ret |= CliUtil_regCliCmd("snmp show config",snmpShowConfig);
	
	return ret;
}



