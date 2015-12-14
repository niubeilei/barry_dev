////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: resmgr.h
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-03-21
////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_RESMGR_H
#define _INCLUDE_RESMGR_H

#include "asm/types.h"
#include "aosApi.h"

////////////////////////////////////////////////////////////////////////////
// Resource manager API Error Code
////////////////////////////////////////////////////////////////////////////
#include "aosReturnCode.h"
#include <linux/errno.h>

enum
{
	// By CHK 2007-02-28
	// Description: Linux defined error numbers 
	//				Values' definition can be checked by command "man errno"
	// Begin
	eAosRc_Linux_EINVAL = EINVAL, /* 22 */
	eAosRc_Linux_ESRCH  = ESRCH,  /* 3 */
	eAosRc_Linux_EPERM  = EPERM,  /* 1 */
	eAosRc_Linux_EACCES = EACCES, /* 13 */
	// End

	eAosRc_ResMgrUnknownError = eAosRc_ResMgrStart+1,
	eAosRc_ResMgrNotStarted, 
	
	eAosRc_ResMgrLoadConfigFail,
	eAosRc_ResMgrSaveConfigFail,
	eAosRc_ResMgrFileNotFound,
	eAosRc_ResMgrFileFailRead,
	eRosRc_ResMgrDiskNotFound,
	eRosRc_ResMgrPartitionNotFound,

	eRosRc_ResMgrIntervalOutOfRange,
	eRosRc_ResMgrDurationOutOfRange,
	eRosRc_ResMgrIllegalRecordTime,
	eRosRc_ResMgrStartTimeIllegal, 		/* The input start_time value is out of record limitation of our resource manager. */
	eRosRc_ResMgrEndTimeIllegal, 		/* The input end_time value is out of record limitation of our resource manager. */
	eRosRc_ResMgrStatisticIntervalTime, /* We have a record limitation for any module of resource manager. The ratio value of the interval and the duration is out of limitation of our resource manager. */
	eAosRc_ResMgrInputInvalid, 			/* Input is invalid */
	eAosRc_ResMgrRcdNotFound, 			/* Not found the status record in our data base */
	eAosRc_ResMgrDataOutOfBuffer, 		/* The retrieved record data is out of buffer size */
	
	eAosRc_ResMgrInvalidBusyThreshold, 
	eAosRc_ResMgrInvalidNormalThreshold, 

	eAosRc_DiskMgrTypeExist, 			/* If the file type already exists in disk manager¡¯s record, an error is reported. */
	eAosRc_DiskMgrTypeNotExist, 		/* If the file type does not exists in disk manager¡¯s record, an error is reported. */
	eAosRc_ResMgrSizeInsuffic, 			/* The caller input array size is not sufficient for the output values. */
	eAosRc_HardDiskNotFound, 			/* The specified hard disk not found. */
};

////////////////////////////////////////////////////////////////////////////
// Resource manager API 
////////////////////////////////////////////////////////////////////////////
#define AOS_RESMGR_MAX_CMD_LEN 256

// 1. 
int aos_resmgr_start();
// 2. 
int aos_resmgr_stop();
// 3. 
int aos_resmgr_clear_config();
// 4. 
int aos_resmgr_retrieve_config(char * const config, u32 bufsize); 
// 5. 
int aos_resmgr_save_config(const char * fname);
// 6. 
int aos_resmgr_load_config(const char * fname);
// 7. 
int aos_resmgr_nice(const int pid, const int nice);

////////////////////////////////////////////////////////////////////////////
// CPU manager API 
////////////////////////////////////////////////////////////////////////////
typedef struct aos_cpumgr_stat_rslt
{
		long m_sec_index;
		float m_cpu_usage; /* (percentage)m_cpu_usage = 1 - (m_jeffies_cpu_idle) / m_jeffiesDuration */
		unsigned long m_jeffiesDuration;
		float m_cpu_user;
		float m_cpu_nice;
		float m_cpu_system;
		float m_cpu_idle;
		float m_cpu_iowait;
		float m_cpu_irq;
		float m_cpu_sirq;
		unsigned long m_jeffies_cpu_user;
		unsigned long m_jeffies_cpu_nice;
		unsigned long m_jeffies_cpu_system;
		unsigned long m_jeffies_cpu_idle;
		unsigned long m_jeffies_cpu_iowait;
		unsigned long m_jeffies_cpu_irq;
		unsigned long m_jeffies_cpu_sirq; 
} aos_cpumgr_stat_rslt_t;

#define AOS_RESMGR_MAX_PROCESS_NUM 256
#define AOS_RESMGR_MAX_PROCESS_NAME_LEN 32

typedef struct aos_cpumgr_process_stat_entry 
{
   unsigned long pid;
   char 	name[AOS_RESMGR_MAX_PROCESS_NAME_LEN];
   unsigned long utime;
   unsigned long stime;
   float using_ratio;
} aos_cpumgr_process_stat_entry_t;

typedef struct aos_cpumgr_process_stat_rslt 
{
	unsigned long jeffiesDuration;
	float cpu_user;
	float cpu_nice;
	float cpu_system;
	float cpu_idle;
	float cpu_iowait;
	float cpu_irq;
	float cpu_sirq;
	unsigned long jeffies_cpu_user;
	unsigned long jeffies_cpu_nice;
	unsigned long jeffies_cpu_system;
	unsigned long jeffies_cpu_idle;
	unsigned long jeffies_cpu_iowait;
	unsigned long jeffies_cpu_irq;
	unsigned long jeffies_cpu_sirq;
	int pnum;
	aos_cpumgr_process_stat_entry_t p[AOS_RESMGR_MAX_PROCESS_NUM];
} aos_cpumgr_process_stat_rslt_t;

// 1. 
int aos_cpumgr_start();
// 2. 
int aos_cpumgr_stop();
// 3. 
int aos_cpumgr_set_record_time(const __u16 interval, const long duration);
// 4. 
int aos_cpumgr_retrieve_cpu(aos_cpumgr_stat_rslt_t * const rslt, 
							u32 *arr_size, const long start_time, 
							const long end_time, 
							const long interval);

// 5. 
int aos_cpumgr_retrieve_process(aos_cpumgr_process_stat_rslt_t * const rslt_ptr, 
							const long start_time, 
							const long end_time, 
							const int num);

// 6. 
int aos_cpumgr_set_threshold(
			const u32 busy_threshold, 
			const u32 busy_duration, 
			const u32 normal_threshold, 
			const u32 normal_duration);

// 7. 
int aos_cpumgr_get_threshold(
			u32 * const busy_threshold, 
			u32 * const busy_duration, 
			u32 * const normal_threshold, 
			u32 * const normal_duration);

////////////////////////////////////////////////////////////////////////////
// Memory manager API 
////////////////////////////////////////////////////////////////////////////
typedef struct aos_memmgr_stat_rslt
{
	long m_sec_index;
	float m_total_usage_ratio;
	long m_total_memory;
	long m_total_free;
	long m_total_buffer;
	long m_total_cached;
} aos_memmgr_stat_rslt_t;

typedef struct aos_memmgr_process_stat_entry 
{
	unsigned long m_pid;
	char 	m_name[AOS_RESMGR_MAX_PROCESS_NAME_LEN];
	int 	m_intensve_nr;
	long 	m_peak_index;
	float 	m_peak_usage_ratio;
} aos_memmgr_process_stat_entry_t;

typedef struct aos_memmgr_process_stat_rslt 
{
	long  m_beg_index;
	long  m_end_index;
	long  m_peak_index;
	float m_peak_usage_ratio;
	int   m_pnum;
	aos_memmgr_process_stat_entry_t m_p[AOS_RESMGR_MAX_PROCESS_NUM];
} aos_memmgr_process_stat_rslt_t;

// 1. 
int aos_memmgr_start();
// 2. 
int aos_memmgr_stop();
// 3. 
int aos_memmgr_set_record_time(const __u16 interval, const long duration);
// 4. 
int aos_memmgr_retrieve_mem(aos_memmgr_stat_rslt_t * const rslt, 
							u32 *arr_size, const long start_time, 
							const long end_time, 
							const long interval);

// 5. 
int aos_memmgr_retrieve_process(aos_memmgr_process_stat_rslt_t * const rslt_ptr, 
							const long start_time, 
							const long end_time, 
							const int num);

// 6. 
int aos_memmgr_set_threshold(
			const u32 busy_threshold, 
			const u32 busy_duration, 
			const u32 normal_threshold, 
			const u32 normal_duration);

// 7. 
int aos_memmgr_get_threshold(
			u32 * const busy_threshold, 
			u32 * const busy_duration, 
			u32 * const normal_threshold, 
			u32 * const normal_duration);

// 8. 
int aos_memmgr_retrieve_system_usage(
		u32 *total_memory, 
		u32 *total_used,
		u32 *total_swapped,
		u32 *total_cached);

// 9.1 
int aos_memmgr_retrieve_self_memory_usage(int * const ret_val);
// 9.2 
int aos_memmgr_retrieve_app_memory_usage(int * const ret_val, const char *app_name);
// 9.3 
int aos_memmgr_retrieve_proc_memory_usage(int * const ret_val, const u32 pid);

////////////////////////////////////////////////////////////////////////////
// Disk manager API 
////////////////////////////////////////////////////////////////////////////
#define AOS_DISKMGR_MAX_HDS 32
#define AOS_DISKMGR_MAX_PARTNUM_IN_HD 16
#define AOS_RESMGR_MAX_DEV_PATH_LEN 128
#define AOS_HD_VENDOR_NAME_LEN 256

typedef struct aos_diskmgr_partition_info
{
		char m_dev_path[AOS_RESMGR_MAX_DEV_PATH_LEN]; /* ID of device */
		float m_usage_ratio;
		long m_space;
		long m_used;
		unsigned int m_starting_cyls;
		unsigned int m_nr_of_cyls;
		unsigned char m_type_id; /* Each partition has a type, its Id */ 
		long m_num_files;
} aos_diskmgr_partition_info_t;

typedef struct aos_diskmgr_harddisk_info
{
		char m_dev_path[AOS_RESMGR_MAX_DEV_PATH_LEN]; /* ID of device */
		char m_vendor[AOS_HD_VENDOR_NAME_LEN]; /* here we just get the information on hard-disk¡¯s chip */
		long m_space;
		long m_used;
		long m_num_files;
		long m_pnum;
		aos_diskmgr_partition_info_t m_p [AOS_DISKMGR_MAX_PARTNUM_IN_HD];
} aos_diskmgr_harddisk_info_t;

typedef struct aos_format_harddisk_set_info
{
		int m_size; /* unit is mega-bytes */
		char m_part_format[16];
} aos_format_harddisk_input_t;

typedef struct aos_diskmgr_file_stat 
{
		char m_file_type[16];
		unsigned long m_file_type_size;
} aos_diskmgr_file_stat_t;

typedef struct aos_diskmgr_filetype 
{
		char m_file_type[16];
} aos_diskmgr_filetype_t;

// 1. 
int aos_diskmgr_start();
// 2. 
int aos_diskmgr_stop();
// 3. 
int aos_diskmgr_set_record_time(const __u16 interval, const long duration);
// 4. 
int aos_diskmgr_set_threshold(
			const u32 busy_threshold, 
			const u32 busy_duration, 
			const u32 normal_threshold, 
			const u32 normal_duration);

// 5. 
int aos_diskmgr_get_threshold(
			u32 * const busy_threshold, 
			u32 * const busy_duration, 
			u32 * const normal_threshold, 
			u32 * const normal_duration);

// 6. 
int aos_diskmgr_retrieve_statistics(aos_diskmgr_file_stat_t * rslt,
									u32 * const arr_size);

// 7. 
int aos_diskmgr_filetype_retrieve(aos_diskmgr_filetype_t * const rslt, 
								  int * const arr_size);

// 8. 
int aos_diskmgr_get_num_hd(int * hd_num);
// 9. 
int aos_diskmgr_get_num_partition(int * part_num, const char * dev_path);
// 10. 
int aos_diskmgr_get_partition_info(
				aos_diskmgr_partition_info_t * const part_info, 
				const char * dev_path);
// 11. 
int aos_diskmgr_get_harddisk_info(aos_diskmgr_harddisk_info_t * const d, 
								   const char * dev_path);
// 12. 
int aos_diskmgr_get_all_hard_disks(aos_diskmgr_harddisk_info_t * const d, 
								   int *num_hd);
// 13. 
int aos_diskmgr_format_hard_disk(aos_diskmgr_harddisk_info_t * const harddisk_info, 
								 const char* hd_dev, 
								 const struct aos_harddisk_set_info* set_info);

// 14. 
int aos_diskmgr_mount_folder(const char *dest_path, 
								const char *dev_path);
// 15. 
int aos_diskmgr_unmount_folder(char *path);
// 16. 
int aos_diskmgr_filetype_add(const char * file_type);
// 17. 
int aos_diskmgr_filetype_remove(const char * file_type);
// 18. 
int aos_diskmgr_filetype_clear();

#endif // _INCLUDE_RESMGR_H
