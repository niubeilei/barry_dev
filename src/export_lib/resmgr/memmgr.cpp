////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: memmgr.cpp
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-03-21
////////////////////////////////////////////////////////////////////////////

#include "resmgr.h"
#include "common.h"
//#include "../../rhcUtil/ReturnCode.h"

// 1. 
int aos_memmgr_start()
{
	OmnString str_rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("mem mgr set record on", str_rslt);

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 2. 
int aos_memmgr_stop()
{
	OmnString str_rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("mem mgr set record off", str_rslt);

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 3. 
int aos_memmgr_set_record_time(const __u16 interval, const long duration)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr set record time %d %ld", (int)interval, duration);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 4. 
int aos_memmgr_retrieve_mem(aos_memmgr_stat_rslt_t * const rslt, 
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
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr show mem %ld %ld %ld", 
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
					rslt[i].m_sec_index 		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_total_usage_ratio	= atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_total_memory		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_total_free		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_total_buffer		= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt[i].m_total_cached		= atol(subtoken);
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
int aos_memmgr_retrieve_process(aos_memmgr_process_stat_rslt * const rslt, 
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
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr show process %ld %ld %d", 
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
		// load the first line of system infomation
		{
			str1 = strLine;
			token = strtok_r(str1, "\n", &saveptr1);
			if (token == NULL)
			{
				return -eAosRc_ResMgrUnknownError;
			}
			switch(str1[0])
			{
				default :
					rslt->m_beg_index = start_time;
					rslt->m_end_index = end_time;
					str2 = token;
					subtoken = strtok_r(str2, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->m_peak_index	= atol(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->m_peak_usage_ratio = atof(subtoken);
					subtoken = strtok_r(NULL, " ", &saveptr2);
					if (subtoken == NULL)
						break;
					rslt->m_pnum 		= atoi(subtoken);
			}
		}

		for(i = 0, str1 = NULL; i < AOS_RESMGR_MAX_PROCESS_NUM; i++, str1 = NULL)
		{
			token = strtok_r(str1, "\n", &saveptr1);
			if (token == NULL)
			{
				break;
			}
			switch(str1[0])
			{
				default :;
					//...........
					// Not finished yet!!!!!!!!!!!!!!!!!!!!!!!!!
			}
		}
		rslt->m_pnum = i;
	}

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 6. 
int aos_memmgr_set_threshold(
			const u32 busy_threshold, 
			const u32 busy_duration, 
			const u32 normal_threshold, 
			const u32 normal_duration)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];

	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr set busy threshold %lu %lu", 
						busy_duration, busy_threshold);
	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr set normal threshold %lu %lu", 
						normal_duration, normal_threshold);
	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 7. 
int aos_memmgr_get_threshold(
			u32 * const busy_threshold, 
			u32 * const busy_duration, 
			u32 * const normal_threshold, 
			u32 * const normal_duration)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr show threshold_api");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	sscanf(str_rslt.data(), "%lu %lu %lu %lu", 
						busy_duration, busy_threshold, 
						normal_duration, normal_threshold);

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 8. 
int aos_memmgr_retrieve_system_usage(
				u32 * const total_memory, 
				u32 * const total_used,
				u32 * const total_swapped,
				u32 * const total_cached)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr show threshold_api");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	sscanf(str_rslt.data(), "%lu %lu %lu %lu", 
						total_memory, 
						total_used,  
						total_swapped,
						total_cached);

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 9.2 
int aos_memmgr_retrieve_app_memory_usage(int * const ret_val, const char *app_name)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];

	if(!app_name || !ret_val)
	{
		return -eAosRc_NullPointerPassed;
	}

	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr get app mem usage_api %s", app_name);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the str_rslt string 
	*ret_val = atoi(str_rslt.data());

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 9.3 
int aos_memmgr_retrieve_proc_memory_usage(int * const ret_val, const u32 pid)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];

	if(!ret_val)
	{
		return -eAosRc_NullPointerPassed;
	}

	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "mem mgr get proc mem usage_api %lu", pid);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the str_rslt string
	*ret_val = atoi(str_rslt.data());

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 9.1 
int aos_memmgr_retrieve_self_memory_usage(int * const ret_val)
{
	return aos_memmgr_retrieve_proc_memory_usage(ret_val, (u32)getpid());
}
