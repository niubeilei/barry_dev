////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelStat.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifdef __KERNEL__

#include "KernelUtil/KernelStat.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "aosUtil/Timer.h"

#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/swap.h>

#define AOS_INFO_NUM 10			//the lenth of the system state info table
#define AOS_LOG_INFO_NUM 2000		//the lenth of the log system state info table

static int sgAosSystemStatStatus = 0;
static int sgAosSystemStatFrequency = 100;
static int sgAosSystemStatLogFreq = 10;
static int sgAosSystemStatLogFreq_tmp = 10;
static int sgduration;


struct aosSystemStat log_system_stat_info_table[AOS_LOG_INFO_NUM],current_system_stat_info={0,0,0,0};


struct system_stat_info
{
	unsigned long cpu_idle;
	unsigned long cpu_system;
	unsigned long cpu_user;
	unsigned long cpu_irq;
	unsigned long cpu_softirq;
	unsigned long cpu_iowait;
	unsigned long cpu_nice;	
	unsigned long cpu_total;
	unsigned long mem_idle;
} system_stat_info_table[AOS_INFO_NUM];


struct cpu_info
{
	OmnUint64 sum;
	OmnUint64 user;
	OmnUint64 nice;
	OmnUint64 system;
	OmnUint64 idle;
	OmnUint64 iowait;
	OmnUint64 irq;
	OmnUint64 softirq;
}presysinfo={0,0,0,0,0,0,0,0}, currentsysinfo={0,0,0,0,0,0,0,0};


struct sysinfo __user *info;

static AosTimer_t *freq_timer;
static AosTimer_t *duration_timer;

static int stateinfotableindex=0;
static int logstateinfotableindex=0;

//============================================
//This func get CPU stat (now only support single CPU)
//============================================
struct aosSystemStat * AosSystemStat_get()
{
	return &current_system_stat_info;
}


asmlinkage long getCPUStat(void)
{


	//
	//show the CPU info
	//
	
	presysinfo.user		=currentsysinfo.user;
	presysinfo.nice		=currentsysinfo.nice;
	presysinfo.system	=currentsysinfo.system;
	presysinfo.idle		=currentsysinfo.idle;
	presysinfo.iowait		=currentsysinfo.iowait;
	presysinfo.irq 		=currentsysinfo.irq;
	presysinfo.softirq		=currentsysinfo.softirq;
	presysinfo.sum		=currentsysinfo.sum;

	//single CPU #0
	
	currentsysinfo.user 		= kstat_cpu(0).cpustat.user;
	currentsysinfo.nice 		= kstat_cpu(0).cpustat.nice;
	currentsysinfo.system 	= kstat_cpu(0).cpustat.system;
	currentsysinfo.idle 		= kstat_cpu(0).cpustat.idle;
	currentsysinfo.iowait 		= kstat_cpu(0).cpustat.iowait;
	currentsysinfo.irq 		= kstat_cpu(0).cpustat.irq;
	currentsysinfo.softirq 		= kstat_cpu(0).cpustat.softirq;
	
	currentsysinfo.sum = currentsysinfo.user + currentsysinfo.nice    + currentsysinfo.system +\
						currentsysinfo.idle  + 	currentsysinfo.iowait  + currentsysinfo.irq       +\
						currentsysinfo.softirq ;
	
	if (stateinfotableindex==AOS_INFO_NUM)	stateinfotableindex=0;

	system_stat_info_table[stateinfotableindex].cpu_idle = 		\
			currentsysinfo.idle - presysinfo.idle;
	system_stat_info_table[stateinfotableindex].cpu_system = 	\
			currentsysinfo.system - presysinfo.system;
	system_stat_info_table[stateinfotableindex].cpu_user =		\
			currentsysinfo.user - presysinfo.user;
	system_stat_info_table[stateinfotableindex].cpu_nice =		\
			currentsysinfo.nice - presysinfo.nice;
	system_stat_info_table[stateinfotableindex].cpu_iowait=		\
			currentsysinfo.iowait - presysinfo.iowait;
	system_stat_info_table[stateinfotableindex].cpu_irq=		\
			currentsysinfo.irq - presysinfo.irq;
	system_stat_info_table[stateinfotableindex].cpu_softirq=	\
			currentsysinfo.softirq - presysinfo.softirq;
	system_stat_info_table[stateinfotableindex].cpu_total = 	\
			currentsysinfo.sum - presysinfo.sum;

	stateinfotableindex++;

	return 0;
}


static void duration_timer_fire(unsigned long param)
{
	//printk("<0>Enter the duration(finish) timer fire func\n");
	AosTimer_delTimer(duration_timer);
	AosTimer_delTimer(freq_timer);
}


static void freq_timer_fire(unsigned long param)
{
	struct sysinfo val;
	unsigned long mem_total, sav_total;
	unsigned int mem_unit, bitcount;
	//unsigned long seq;
	int tmpi,tmpidlesum=0,tmpusersum=0,tmpsystemsum=0,tmpsum=0;
	int tmpirqsum=0,tmpsoftirqsum=0,tmpiowaitsum=0,tmpnicesum=0;
	
	if (sgAosSystemStatLogFreq_tmp==0)
	{
		
		if(logstateinfotableindex==AOS_LOG_INFO_NUM)  logstateinfotableindex=0;
	 	for(tmpi=0;tmpi<AOS_INFO_NUM;tmpi++)
		{
			tmpidlesum		+=system_stat_info_table[tmpi].cpu_idle;
			tmpusersum		+=system_stat_info_table[tmpi].cpu_user;
			tmpsystemsum	+=system_stat_info_table[tmpi].cpu_system;
			tmpirqsum		+=system_stat_info_table[tmpi].cpu_irq;
			tmpsoftirqsum		+=system_stat_info_table[tmpi].cpu_softirq;
			tmpnicesum		+=system_stat_info_table[tmpi].cpu_nice;
			tmpiowaitsum		+=system_stat_info_table[tmpi].cpu_iowait;
			tmpsum			+=system_stat_info_table[tmpi].cpu_total;
		}	
		current_system_stat_info.cpu_idle_rate=tmpidlesum *100 /tmpsum;		
		current_system_stat_info.cpu_user_rate=tmpusersum *100 /tmpsum;
		current_system_stat_info.cpu_system_rate=tmpsystemsum *100 /tmpsum;
		current_system_stat_info.cpu_irq_rate=tmpirqsum *100 /tmpsum;
		current_system_stat_info.cpu_softirq_rate=tmpsoftirqsum *100 /tmpsum;
		current_system_stat_info.cpu_iowait_rate=tmpiowaitsum *100 /tmpsum;
		current_system_stat_info.cpu_nice_rate=tmpnicesum *100 /tmpsum;
		
		log_system_stat_info_table[logstateinfotableindex].cpu_idle_rate= 		\
			current_system_stat_info.cpu_idle_rate;
		log_system_stat_info_table[logstateinfotableindex].cpu_user_rate= 		\
			current_system_stat_info.cpu_user_rate;
		log_system_stat_info_table[logstateinfotableindex].cpu_system_rate= 	\
			current_system_stat_info.cpu_system_rate;
		log_system_stat_info_table[logstateinfotableindex].cpu_irq_rate= 	\
			current_system_stat_info.cpu_irq_rate;
		log_system_stat_info_table[logstateinfotableindex].cpu_softirq_rate= 	\
			current_system_stat_info.cpu_softirq_rate;
		log_system_stat_info_table[logstateinfotableindex].cpu_iowait_rate= 	\
			current_system_stat_info.cpu_iowait_rate;
		log_system_stat_info_table[logstateinfotableindex].cpu_nice_rate= 	\
			current_system_stat_info.cpu_nice_rate;
		
		si_meminfo(&val);
		si_swapinfo(&val);

		mem_total = val.totalram + val.totalswap;
		if (mem_total < val.totalram || mem_total < val.totalswap)	goto out;
		bitcount = 0;
		mem_unit = val.mem_unit;
		while (mem_unit > 1) 
		{
			bitcount++;
			mem_unit >>= 1;
			sav_total = mem_total;
			mem_total <<= 1;
			if (mem_total < sav_total)
			goto out;
		}

		val.mem_unit = 1;
		val.totalram <<= bitcount;
		val.freeram <<= bitcount;
		val.sharedram <<= bitcount;
		val.bufferram <<= bitcount;
		val.totalswap <<= bitcount;
		val.freeswap <<= bitcount;
		val.totalhigh <<= bitcount;
		val.freehigh <<= bitcount;

		out:
		
		current_system_stat_info.mem_idle=val.freeram;
		log_system_stat_info_table[logstateinfotableindex].mem_idle= \
			current_system_stat_info.mem_idle;

		logstateinfotableindex++;	
		sgAosSystemStatLogFreq_tmp=sgAosSystemStatLogFreq;
		
	}	
	
	getCPUStat();
	AosTimer_modTimer(freq_timer,sgAosSystemStatFrequency);
	
	sgAosSystemStatLogFreq_tmp--;
	
}


int AosSystemStat_startCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system stat start <duration>
	//
	sgduration = parms->mIntegers[0];

	*length = 0;
	if (!sgAosSystemStatStatus)
	{
		sgAosSystemStatStatus = 1;	
	}	

	//init
		
	currentsysinfo.user 		= kstat_cpu(0).cpustat.user;
	currentsysinfo.nice 		= kstat_cpu(0).cpustat.nice;
	currentsysinfo.system 	= kstat_cpu(0).cpustat.system;
	currentsysinfo.idle 		= kstat_cpu(0).cpustat.idle;
	currentsysinfo.iowait 		= kstat_cpu(0).cpustat.iowait;
	currentsysinfo.irq 		= kstat_cpu(0).cpustat.irq;
	currentsysinfo.softirq 		= kstat_cpu(0).cpustat.softirq;
	currentsysinfo.sum = currentsysinfo.user + currentsysinfo.nice + currentsysinfo.system +\
						currentsysinfo.idle  + 	currentsysinfo.iowait + currentsysinfo.irq +\
						currentsysinfo.softirq ;

	//printk("<0>init sys user %lu \n",currentsysinfo.user);
	
	sgAosSystemStatLogFreq_tmp=sgAosSystemStatLogFreq;	
		
	duration_timer = AosTimer_addTimer(sgduration*1000, 0, duration_timer_fire);
	freq_timer = AosTimer_addTimer(sgAosSystemStatFrequency, 0, freq_timer_fire);

	return 0;
}


int AosSystemStat_stopCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system stat stop
	//	

	*length = 0;

	if (1==sgAosSystemStatStatus)	sgAosSystemStatStatus = 0;
	
	return 0;
}


int AosSystemStat_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system stat show
	//	

	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);	// Retrieve the beginning of the return buffer
	unsigned int rsltIndex = 0;						// Return buffer index;
	unsigned int optlen = *length;

	sprintf(local, "System Statistics Information\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "--------------------------------------\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	if (sgAosSystemStatStatus)
	{
		sprintf(local, "Module Status: on\n");
	}
	else
	{
		sprintf(local, "Module Status: off\n");
	}

	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Statistics Frequency: %d\n", sgAosSystemStatFrequency);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Statistics Log Frequency: %d\n", sgAosSystemStatLogFreq);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;

	return 0;
}


int AosSystemStat_retrieveCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system stat retrieve <filename>
	//
	//retrieve_filename = parms->mString[0];
	
	char local[2000];
	char *rsltBuff = aosKernelApi_getBuff(data);	// Retrieve the beginning of the return buffer
	unsigned int rsltIndex = 0;						// Return buffer index;
	unsigned int optlen = *length;
	int tmpindex=0;

	sprintf(local, "System Statistics Log List\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "-------------------------------------------------------------------------------\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "--LOG--------------------------CPU(%s)------------------------------MEM(k)-----\n","%");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "-index-------idle---user---system---irq---softirq---iowait---nice---free mem---\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	while(tmpindex<logstateinfotableindex)
	{
		sprintf(local, "log[%4d]: %6d %6d %6d %6d %6d %6d %6d %16lu\n",tmpindex, \
				log_system_stat_info_table[tmpindex].cpu_idle_rate,\
				log_system_stat_info_table[tmpindex].cpu_user_rate,\
				log_system_stat_info_table[tmpindex].cpu_system_rate,\
				log_system_stat_info_table[tmpindex].cpu_irq_rate,\
				log_system_stat_info_table[tmpindex].cpu_softirq_rate,\
				log_system_stat_info_table[tmpindex].cpu_iowait_rate,\
				log_system_stat_info_table[tmpindex].cpu_nice_rate,\
				log_system_stat_info_table[tmpindex].mem_idle/1000);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		tmpindex++;
	}
	
	*length = rsltIndex;

	return 0;
}


int AosSystemStat_freqCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system stat freq <frequency>
	//
	sgAosSystemStatFrequency = parms->mIntegers[0];

	*length = 0;	

	return 0;
}


int AosSystemStat_logfreqCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system stat log freq <frequency>
	//
	sgAosSystemStatLogFreq = parms->mIntegers[0];

	*length = 0;	

	return 0;
}


int AosSystemStat_init()
{

	OmnKernelApi_addCliCmd("sysstat_start", AosSystemStat_startCli);
	OmnKernelApi_addCliCmd("sysstat_stop", AosSystemStat_stopCli);
	OmnKernelApi_addCliCmd("sysstat_show", AosSystemStat_showCli);
	OmnKernelApi_addCliCmd("sysstat_retrieve", AosSystemStat_retrieveCli);
	OmnKernelApi_addCliCmd("sysstat_freq", AosSystemStat_freqCli);
	OmnKernelApi_addCliCmd("sysstat_logfreq", AosSystemStat_logfreqCli);

	return 0;
}

#endif

