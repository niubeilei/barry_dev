////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cpumgr.cpp
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-03-21
////////////////////////////////////////////////////////////////////////////

#include "resmgr.h"
#include "common.h"

// 1. 
int aos_cpumgr_start()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("cpu mgr set record on", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 2. 
int aos_cpumgr_stop()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("cpu mgr set record off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 3. 
int aos_cpumgr_set_record_time(const __u16 interval, const long duration)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "cpu mgr set record time %d %ld", (int)interval, duration);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 4. 
int aos_cpumgr_retrieve_cpu(aos_cpumgr_stat_rslt_t * const rslt, 
							u32 *arr_size, 
							const long start_time, 
							const long end_time, 
							const long interval)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	if(!rslt || !arr_size)
	{
		return -eAosRc_NullPointerPassed;
	}
	const int input_arr_size = (*arr_size);
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "cpu mgr show cpu %ld %ld %ld", 
						start_time, end_time, interval);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	int ret = (*((int*)tmp));
	if(ret < 0)
	{
		return ret;
	}
	// load the data from the rslt string 
	{
		char strLine[10000];
		char *str1, *str2, *token = NULL, *subtoken;
		char *saveptr1, *saveptr2;
		int i;

		strncpy(strLine, str_rslt.data(), 10000);

		for(i = 0, str1 = strLine; i < input_arr_size; i++, str1 = NULL)
		{
			token = strtok_r(str1, "\n", &saveptr1);
			if (token == NULL)
			{
				break;
			}
			switch(str1[0])
			{
				default :
					str2 = token;
					subtoken = strtok_r(str2, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_sec_index 			= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffiesDuration 		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_user 	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_nice 	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_system 	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_idle 	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_iowait 	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_irq 		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_jeffies_cpu_sirq 	= atol(subtoken); 
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_usage 			= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_user 			= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_nice 			= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_system 			= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_idle 			= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_iowait 			= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_irq 				= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_cpu_sirq 			= atof(subtoken);
			}
		}
		(*arr_size) = i;
	}
	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 5. 
int aos_cpumgr_retrieve_process(aos_cpumgr_process_stat_rslt_t * const rslt, 
							const long start_time, 
							const long end_time, 
							const int num)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	if(!rslt)
	{
		return -eAosRc_NullPointerPassed;
	}
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "cpu mgr show process %ld %ld %d", 
						start_time, end_time, num);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	int ret = (*((int*)tmp));
	if(ret < 0)
	{
		return ret;
	}
	// load the data from the rslt string 
	{
		char strLine[10000];
		char *str1, *str2, *token = NULL, *subtoken;
		char *saveptr1, *saveptr2;
		int i;

		strncpy(strLine, str_rslt.data(), 10000);

		for(i = 0, str1 = strLine; i < AOS_RESMGR_MAX_PROCESS_NUM; i++, str1 = NULL)
		{
			token = strtok_r(str1, "\n", &saveptr1);
			if (token == NULL)
			{
				break;
			}
			switch(str1[0])
			{
				default :
					str2 = token;
					subtoken = strtok_r(str2, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->p[i].pid	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					strncpy(rslt->p[i].name, subtoken, AOS_RESMGR_MAX_PROCESS_NAME_LEN);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->p[i].utime 		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->p[i].stime 		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->p[i].using_ratio= atof(subtoken);
			}
		}
		rslt->pnum = i;
	}

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 6. 
int aos_cpumgr_set_threshold(
			const u32 busy_threshold, 
			const u32 busy_duration, 
			const u32 normal_threshold, 
			const u32 normal_duration)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];

	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "cpu mgr set busy threshold %lu %lu", 
						busy_duration, busy_threshold);
	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "cpu mgr set normal threshold %lu %lu", 
						normal_duration, normal_threshold);
	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 7. 
int aos_cpumgr_get_threshold(
			u32 * const busy_threshold, 
			u32 * const busy_duration, 
			u32 * const normal_threshold, 
			u32 * const normal_duration)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "cpu mgr show threshold_api");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);
	sscanf(rslt.data(), "%lu %lu %lu %lu", 
						busy_duration, busy_threshold, 
						normal_duration, normal_threshold);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}
