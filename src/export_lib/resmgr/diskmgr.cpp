////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: diskmgr.cpp
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
int aos_diskmgr_start()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("disk mgr set record on", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 2. 
int aos_diskmgr_stop()
{
	OmnString rslt = "";
	char* tmp;

	OmnCliProc::getSelf()->runCliAsClient("disk mgr set record off", rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 3. 
int aos_diskmgr_set_record_time(const __u16 interval, const long duration)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr set record time %d %ld", (int)interval, duration);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 4. 
int aos_diskmgr_set_threshold(
			const u32 busy_threshold, 
			const u32 busy_duration, 
			const u32 normal_threshold, 
			const u32 normal_duration)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];

	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr set busy threshold %lu %lu", 
						busy_duration, busy_threshold);
	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr set normal threshold %lu %lu", 
						normal_duration, normal_threshold);
	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 5. 
int aos_diskmgr_get_threshold(
			u32 * const busy_threshold, 
			u32 * const busy_duration, 
			u32 * const normal_threshold, 
			u32 * const normal_duration)
{
	OmnString rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr show threshold_api");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, rslt);
	sscanf(rslt.data(), "%lu %lu %lu %lu", 
						busy_duration, busy_threshold, 
						normal_duration, normal_threshold);

	// get the return code
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 6. 
int aos_diskmgr_retrieve_statistics(aos_diskmgr_file_stat_t * rslt,
									u32 * const arr_size)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr show statistics_api");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the str_rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

///////////////////////////////////////////////////////////////////////////

// 7. 
int aos_diskmgr_filetype_retrieve(aos_diskmgr_filetype_t * const rslt, 
								  int * const arr_size)
{
	int ret = 0;

	//!!!!!!!!!!!!!!!!!!!!!!
	// retrieve file type information  NOT FINISHED !!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!

	return ret;
}

// 12. 
int aos_diskmgr_get_all_hard_disks(aos_diskmgr_harddisk_info_t * const d, 
								   int *num_hd)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr getall hard disk info_api");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the str_rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 8. 
int aos_diskmgr_get_num_hd(int * hd_num)
{
	int ret;
	int disk_num = 64;
	aos_diskmgr_harddisk_info_t harddisk_info[64];
	ret = aos_diskmgr_get_all_hard_disks(harddisk_info, &disk_num);
	// set value to hd_num
	(*hd_num) = disk_num;

	return ret;
}

// 9. 
int aos_diskmgr_get_num_partition(int * part_num, const char * dev_path)
{
	int ret, i;
	int disk_num = 64;
	aos_diskmgr_harddisk_info_t harddisk_info[64];
	ret = aos_diskmgr_get_all_hard_disks(harddisk_info, &disk_num);
	if(ret)
	{
		return ret;
	}

	// set value to part_num
	(*part_num) = 0;
	for(i = 0; i < disk_num; i++)
	{
		if(strcmp(dev_path, harddisk_info[i].m_dev_path) == 0)
		{
			(*part_num) = harddisk_info[i].m_pnum;
		}
	}
	if(i == disk_num)
	{
		return -eAosRc_HardDiskNotFound;
	}

	return ret;
}

// 10. 
int aos_diskmgr_get_partition_info(
				aos_diskmgr_partition_info_t * const part_info, 
				const char * dev_path)
{
	int ret, i, j;
	int bFound;
	int disk_num = 64;
	aos_diskmgr_harddisk_info_t harddisk_info[64];
	ret = aos_diskmgr_get_all_hard_disks(harddisk_info, &disk_num);
	if(ret < 0)
	{
		return ret;
	}

	// set value to part_num
	bFound = 0;
	for(i = 0; i < disk_num; i++)
	{
		for(j = 0; j < harddisk_info[i].m_pnum; j++)
		{
			if(strcmp(dev_path, harddisk_info[i].m_p[j].m_dev_path) == 0)
			{
				(*part_info) = harddisk_info[i].m_p[j];
				bFound = 1;
				break;
			}
		}
		if(bFound)
		{
			break;
		}
	}
	if(i == disk_num)
	{
		return -eAosRc_HardDiskNotFound;
	}

	return ret;
}

// 11. 
int aos_diskmgr_get_harddisk_info(aos_diskmgr_harddisk_info_t * const d, 
								   const char * dev_path)
{
	int ret, i;
	int bFound;
	int disk_num = 64;
	aos_diskmgr_harddisk_info_t harddisk_info[64];
	ret = aos_diskmgr_get_all_hard_disks(harddisk_info, &disk_num);
	if(ret < 0)
	{
		return ret;
	}

	// set value to part_num

	bFound = 0;
	for(i = 0; i < disk_num; i++)
	{
		if(strcmp(dev_path, harddisk_info[i].m_dev_path) == 0)
		{
			(*d) = harddisk_info[i];
			bFound = 1;
			break;
		}
	}
	if(!bFound)
	{
		return -eAosRc_HardDiskNotFound;
	}

	return ret;
}

// 13. 
int aos_diskmgr_format_hard_disk(aos_diskmgr_harddisk_info_t * const harddisk_info, 
								 const char* hd_dev, 
								 const struct aos_harddisk_set_info* set_info)
{
	int ret;

	//!!!!!!!!!!!!!!!!!!!!!!
	// format disk with information of set_info NOT FINISHED
	//!!!!!!!!!!!!!!!!!!!!!!

	ret = aos_diskmgr_get_harddisk_info(harddisk_info, hd_dev);
	if(ret < 0)
	{
		return ret;
	}
	return ret;
}

// 14. 
int aos_diskmgr_mount_folder(const char *dest_path, 
								const char *dev_path)
{
	OmnString str_rslt = "";
	char* tmp;

	if(dest_path == NULL || dev_path == NULL)
	{
		return -eAosRc_NullPointerPassed;
	}
	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr mount folder %s %s", dev_path, dest_path);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 15. 
int aos_diskmgr_unmount_folder(char *path)
{
	OmnString str_rslt = "";
	char* tmp;

	if(path == NULL)
	{
		return -eAosRc_NullPointerPassed;
	}
	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr unmount folder %s", path);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 16. 
int aos_diskmgr_filetype_add(const char * file_type)
{
	OmnString str_rslt = "";
	char* tmp;

	if(file_type == NULL)
	{
		return -eAosRc_NullPointerPassed;
	}
	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr file type add %s", file_type);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 17. 
int aos_diskmgr_filetype_remove(const char * file_type)
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr file type remove %s", file_type);

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}

// 18. 
int aos_diskmgr_filetype_clear()
{
	OmnString str_rslt = "";
	char* tmp;

	char zCmd[AOS_RESMGR_MAX_CMD_LEN];
	snprintf(zCmd, AOS_RESMGR_MAX_CMD_LEN, "disk mgr file type clear");

	OmnCliProc::getSelf()->runCliAsClient(zCmd, str_rslt);
	// load the data from the rslt string 

	// get the return code
	tmp = str_rslt.getBuffer();
	tmp += str_rslt.find(0, false) + 1;

	return *((int*)tmp);
}
///////////////////////////////////////////////////////////////////////////////
