////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tracer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tracer.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/PlatformRc.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/string.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/kernel.h"
#include "util_c/modules.h"
#include <stdarg.h>

#include "aosUtil/TracerPriv.h"
#include "aosUtil/AosPlatform.h"

#ifdef __KERNEL__
#include <linux/module.h>
#endif

// 
// Chen Ding, 04/30/2006
// In-Memory Logs
//
#ifdef CONFIG_AOS_INMEMORY_LOG

#define AOS_LOG_ARRAY_SIZE 1000000
#define AOS_LOG_ENTRY_SIZE 100
static char sgLogEntries[AOS_LOG_ARRAY_SIZE][AOS_LOG_ENTRY_SIZE];
static u32  sgLogJiffies[AOS_LOG_ARRAY_SIZE];
static u32  sgInMemoryLogEntryIndex = 0;
static u32  sgInMemoryLogEntryCount = 0;
static int  sgInMemoryLogStatus = 0;
static int  sgInMemoryRetrievePos = -1;
static int  sgInMemoryRetrieveCount = -1;
static int  sgInMemoryLogWrap = 1;
extern int AosTracer_addInMemEntry(const char *entry);
#endif

static char sgLogModuleFilters[eAosModule_End];
static char sgInitFlag = 0;

int aos_log_init(void)
{
	int i;

	if (sgInitFlag)
	{
		return 0;
	}

	// 
	// Init Module flags
	//
	for (i=0; i<eAosModule_End; i++)
	{
		// sgLogModuleFilters[i] = eAosLogLevel_Debug;
		sgLogModuleFilters[i] = eAosLogLevel_Minimum;
	}

	sgInitFlag = 1;

	// 
	// Chen Ding, 04/30/2006
	//
#ifdef CONFIG_AOS_INMEMORY_LOG
	for (i=0; i<AOS_LOG_ARRAY_SIZE; i++)
	{
		sgLogEntries[i][0] = 0;
	}

#endif

	return 0;
};


#ifdef __KERNEL__
EXPORT_SYMBOL(aos_log_get_msg);
#endif
// 
// The caller should never modify the contents of the buffer
// returned by this function.
//
char *aos_log_get_msg(char *fmt, ...)
{
	static int  slBufIndex = 0;
	static char slBuffer[eAosLogNumLocalBuf][eAosLogLocalBufSize];
	unsigned int index = (slBufIndex++) & 0x07;

	va_list args;
	int n;

	va_start(args, fmt);
	n = vsprintf(slBuffer[index], fmt, args);
	va_end(args);

	return slBuffer[index];
}


const char *AosLog_getLevelName(int level)
{
	if (level <= eAosLogLevel_Start || level >= eAosLogLevel_End)
	{
		return "Invlid Log Level";
	}
	switch (level)
	{
	case eAosLogLevel_NoLog:
		 return "No log";

    case eAosLogLevel_Minimum:
		 return "Minimum";

    case eAosLogLevel_Production:
		 return "Production";

    case eAosLogLevel_Engineering:
		 return "Engineering";

    case eAosLogLevel_Debug:
		 return "Debug";
	
	default:
		 return "Invalid log level";
	}
}

	
#ifdef __KERNEL__
EXPORT_SYMBOL(aos_create_log_entry);
#endif
int aos_create_log_entry(const char *file, 
					const int line, 
					int level, 
					int module, 
					char *msg)
{
	char local[eAosLogLocalBufSize+100];
	int len;

	if (!sgInitFlag)
	{
		aos_log_init();
	}
	
	if (level <= eAosLogLevel_Start || level >= eAosLogLevel_End)
	{
		level = eAosLogLevel_Minimum;
	}

	if (module <= eAosModule_Start || module >= eAosModule_End)
	{
		module = eAosModule_Start;
	}


	// 
	// Check the filters
	//
	if (level > sgLogModuleFilters[module])
	{
		return 0;
	}

	len = strlen(msg);
	if (len >= eAosLogLocalBufSize)
	{
		msg[eAosLogLocalBufSize] = 0;
	}

	sprintf(local, "<%s:%d>:  %s\n", file, line, msg);

#ifdef CONFIG_AOS_INMEMORY_LOG
	if (sgInMemoryLogStatus)
	{
		AosTracer_addInMemEntry(local);
	}
	else
	{
		aos_printk(local);
	}
#else
	aos_printk("%s", local);
#endif

	return 0;
}


/*
int AosTracer_setFilterCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // log set filter <module-name> <level> 
    //
    char *mname = parms->mStrings[0];
    char *levelStr = parms->mStrings[1];
	int level;
	int modId;

	if (!sgInitFlag)
	{
		aos_log_init();
	}

    *length = 0;

	if (!mname || !levelStr)
	{
		strcpy(errmsg, "Command error");
		return -eAosRc_NullPointer;
	}

	if (strcmp(levelStr, "nolog") == 0)
	{
		level = eAosLogLevel_NoLog;
	}
	else if (strcmp(levelStr, "minimum") == 0)
	{
		level = eAosLogLevel_Minimum;
	}
	else if (strcmp(levelStr, "production") == 0)
	{
		level = eAosLogLevel_Production;
	}
	else if (strcmp(levelStr, "engineering") == 0)
	{
		level = eAosLogLevel_Engineering;
	}
	else if (strcmp(levelStr, "debug") == 0)
	{
		level = eAosLogLevel_Debug;
	}
	else
	{
		strcpy(errmsg, "Log level incorrect");
		return -eAosRc_UnrecognizedLogLevel;
	}

	if (strcmp(mname, "all") == 0)
	{
		int i;
		for (i=eAosModule_Start+1; i<eAosModule_End; i++)
		{
			sgLogModuleFilters[i] = level;
		}

		return 0;
	}

	modId = AosModule_getId(mname);
	if (modId <= eAosModule_Start)
	{
		strcpy(errmsg, "Unrecognized module name");
		return -eAosRc_UnrecognizedModuleName;
	}

	sgLogModuleFilters[modId] = level;
	return 0;
}
*/


void aos_trace_hex_f(
			const char *file,
			int line,
			const char *title, 
			const char *data, 
			const unsigned int data_len)
{
    unsigned int i, count;
	
	// 
	// Check the filters
	//
	if ( eAosLogLevel_Debug > sgLogModuleFilters[eAosMD_Tracer])
	{
		return ;
	}

    aos_printk("<%s:%d> Title: %s, length=[%d]\n", file, line, title, data_len);
    for(i=0, count=0;i<data_len;i++)
	{
        aos_printk("%02x ", ((unsigned int)data[i]) & 0xff);
        count++;

        if(count%16==0)
		{
            aos_printk("\n");
            count=0;
        }
    }
    aos_printk("\n");

	
}


int AosTracer_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // command: log show
    //
	int i;
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char local[200];

    *length = 0;
	sprintf(local, "--------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	for (i=eAosModule_Start+1; i<eAosModule_End; i++)
	{
		sprintf(local, "%-10s  %s\n", aos_get_module_name(i), 
			aos_get_module_name(sgLogModuleFilters[i]));
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

#ifdef CONFIG_AOS_INMEMORY_LOG
	// In-Memory Log Array Size: 
	sprintf(local, "In-Memory Log Array Size:  %u\n", AOS_LOG_ARRAY_SIZE);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	// In-Memory Log Entry Size:
	sprintf(local, "In-Memory Log Entry Size:  %u\n", AOS_LOG_ENTRY_SIZE);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	// In-Memory Log Entry Count:
	sprintf(local, "In-Memory Log Entry Count: %u\n", sgInMemoryLogEntryCount);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	// In-Memory Index: 
	sprintf(local, "In-Memory Index:           %u\n", sgInMemoryLogEntryIndex);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "In-Memory Log Status:      on\n");

	// In-Memory Log Status: on|off
	if (sgInMemoryLogStatus)
	{
		sprintf(local, "In-Memory Log Status:      on\n");
	}
	else
	{
		sprintf(local, "In-Memory Log status:      off\n");
	}
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	// In-Memory Wrap: on|off
	if (sgInMemoryLogWrap)
	{
		sprintf(local, "In-Memory Log Wrap:        on\n");
	}
	else
	{
		sprintf(local, "In-Memory Log Wrap:        off\n");
	}
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

#endif

	*length = rsltIndex;
	return 0;
}


// 
// Chen Ding, 04/30/2006
// In-Memory Logs
//
#ifdef CONFIG_AOS_INMEMORY_LOG
int AosTracer_addInMemEntry(const char *entry)
{
	u32 len = strlen(entry);

	if (sgInMemoryLogEntryIndex >= AOS_LOG_ARRAY_SIZE)
	{
		if (sgInMemoryLogWrap)
		{
			// 
			// This is an error
			//
			aos_alarm(eAosMD_AosUtil, eAosAlarm_ProgErr, "Index out of bound: %u\n", 
					sgInMemoryLogEntryIndex);
			sgInMemoryLogEntryIndex = 0;
		}
		else
		{
			// 
			// Wrapping is off, and the log is full. Do nothing.
			//
			return 0;
		}
	}

	if (len >= AOS_LOG_ENTRY_SIZE) len = AOS_LOG_ENTRY_SIZE-1;

	strncpy(sgLogEntries[sgInMemoryLogEntryIndex], entry, len);
	sgLogEntries[sgInMemoryLogEntryIndex][len] = 0;
	sgLogJiffies[sgInMemoryLogEntryIndex++] = jiffies;
	sgInMemoryLogEntryCount++;

	if (sgInMemoryLogWrap && sgInMemoryLogEntryIndex >= AOS_LOG_ARRAY_SIZE) sgInMemoryLogEntryIndex = 0;

	return 0;
}


int AosTracer_printLogCli(char *data,
		      		   unsigned int *length,
			       	   struct aosKernelApiParms *parms,
				       char *errmsg,
					   const int errlen)
{
	// 
	// inmemory log print <start> <count>
	// 
	int i;
	char local[300];
	int start = parms->mIntegers[0];
	int count = parms->mIntegers[1];

	*length = 0;
	if (start < 0 || start >= AOS_LOG_ARRAY_SIZE)
	{
		sprintf(errmsg, "Invalid start: %d", start);
		return eAosRc_InvalidParm;
	}

	if (count <= 0 || count > AOS_LOG_ARRAY_SIZE)
	{
		sprintf(errmsg, "Invalid count: %d", start);
		return eAosRc_InvalidParm;
	}

	i = start;
	while (count)
	{
		if (sgLogEntries[i][0] == 0) break;

		sprintf(local, "%u:%s\n", sgLogJiffies[i], sgLogEntries[i]);
		aos_printk(local);

		i++;
		if (i >= AOS_LOG_ARRAY_SIZE)
		{
			i = 0;
		}

		count--;
	}

	return 0;
}


int AosTracer_setStatusCli(char *data,
		      		   unsigned int *length,
			       	   struct aosKernelApiParms *parms,
				       char *errmsg,
					   const int errlen)
{
	// 
	// inmemory log status [on|off]
	// 
	char *status = parms->mStrings[0];

	*length = 0;

	if (strcmp(status, "on") == 0)
	{
		sgInMemoryLogStatus = 1;
	}
	else if (strcmp(status, "off") == 0)
	{
		sgInMemoryLogStatus = 0;
	}
	else
	{
		sprintf(errmsg, "Invalid status: %s", status);
		return eAosRc_InvalidParm;
	}

	return 0;
}


int AosTracer_resetCli(char *data,
		      		   unsigned int *length,
			       	   struct aosKernelApiParms *parms,
				       char *errmsg,
					   const int errlen)
{
	// 
	// inmemory log reset
	// 
	int i;

	*length = 0;

	for (i=0; i<AOS_LOG_ARRAY_SIZE; i++)
	{
		sgLogEntries[i][0] = 0;
	}

	sgInMemoryLogEntryCount = 0;
	sgInMemoryLogEntryIndex = 0;
	return 0;
}


int AosTracer_setWrapCli(char *data,
		      		   unsigned int *length,
			       	   struct aosKernelApiParms *parms,
				       char *errmsg,
					   const int errlen)
{
	// 
	// inmemory log wrap [on|off]
	// 
	char *status = parms->mStrings[0];

	*length = 0;

	if (strcmp(status, "on") == 0)
	{
		sgInMemoryLogWrap = 1;
	}
	else if (strcmp(status, "off") == 0)
	{
		sgInMemoryLogWrap = 0;
	}
	else
	{
		sprintf(errmsg, "Invalid status: %s", status);
		return eAosRc_InvalidParm;
	}

	return 0;
}


int AosTracer_retrieveCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // command: inmemory log retrieve <start> <count> <bufflen>
	// This command will be executed many times until all the entries are retrieved.
	// The first time the command is executed, it sets the start position and count.
	// The subsequent command will use the next command "inmemory log retrieve cont
    //
	int i;
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
	char local[200];
	int start = parms->mIntegers[0];
	int count = parms->mIntegers[1];
	int buflen = parms->mIntegers[1];

    *length = 0;
	
	if (start < 0 || start >= AOS_LOG_ARRAY_SIZE)
	{
		sprintf(errmsg, "Invalid start: %d", start);
		return eAosRc_InvalidParm;
	}

	if (count <= 0 || count > AOS_LOG_ARRAY_SIZE)
	{
		sprintf(errmsg, "Invalid count: %d", start);
		return eAosRc_InvalidParm;
	}

	if (sgInMemoryRetrievePos < 0)
	{
		sgInMemoryRetrievePos = start;
	}

	if (sgInMemoryRetrieveCount < 0)
	{
		sgInMemoryRetrieveCount = count;
	}

	i = sgInMemoryRetrievePos;
	count = sgInMemoryRetrieveCount;
	while (count)
	{
		if (sgLogEntries[i][0] == 0) break;

		sprintf(local, "%u:%s\n", sgLogJiffies[i], sgLogEntries[i]);
		if (strlen(local) > buflen - 3)
		{
			// 
			// Run out of the buf. Return.
			//
			break;
		}

    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

		i++;
		if (i >= AOS_LOG_ARRAY_SIZE)
		{
			i = 0;
		}

		count--;
	}

	sgInMemoryRetrievePos = i;
	sgInMemoryRetrieveCount = count;

	*length = rsltIndex;
	return 0;
}

#endif

#ifdef __KERNEL__
EXPORT_SYMBOL( aos_trace_hex_f );
#endif 

