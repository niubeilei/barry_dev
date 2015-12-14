////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelStat.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_KernelUtil_SystemStat_h
#define Aos_KernelUtil_SystemStat_h

struct aosKernelApiParms;

struct aosSystemStat
{
	int cpu_idle_rate;
	int cpu_system_rate;
	int cpu_user_rate;
	int cpu_irq_rate;
	int cpu_softirq_rate;
	int cpu_iowait_rate;
	int cpu_nice_rate;	
	unsigned long mem_idle;
};

extern struct aosSystemStat * AosSystemStat_get(void);

extern int AosSystemStat_startCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosSystemStat_stopCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosSystemStat_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosSystemStat_retrieveCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosSystemStat_freqCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosSystemStat_logfreqCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

extern int AosSystemStat_init(void);

#endif

