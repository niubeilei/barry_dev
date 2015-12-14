////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrToolkit.h
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#ifndef aos_rhcUtil_aosDiskMgrToolkit_h
#define aos_rhcUtil_aosDiskMgrToolkit_h

#include "rhcUtil/aosResMgrDefines.h"

#include <time.h>
#include <stdio.h>

//=============================================
// defines for Disk manager
//=============================================
// only for Disk module
// #define DISKMGR_FULLPATH "./DiskMgr"
// #define DISKMGR_PROCNAME "DiskMgr"

#define DISKMGR_MAX_PARTITION_NUM 	32
#define DISKMGR_MAX_DISK_NUM 		16
#define DISKMGR_MAX_TYPE_NUM 		1024
#define DISKMGR_MAX_USER_NUM 		512

//#define DISKLOG_FULLPATH "/var/log/aos_disk_log.0"
#define DISKMGR_MAX_TYPE_LEN 16
#define DISKMGR_MAX_NAME_LEN 64
#define DISKMGR_MAX_DIR_LEN 256

struct PartitionEntry
{
	char dname[DISKMGR_MAX_NAME_LEN];
	char pname[DISKMGR_MAX_NAME_LEN];
	char mount[DISKMGR_MAX_NAME_LEN]; 
	int used;
	int total;
	int files;
};

struct DiskEntry
{
	char diskname[DISKMGR_MAX_NAME_LEN];
	int used;
	int total;
	int files;
	int parts;
};

struct UserEntry
{
	char name[DISKMGR_MAX_NAME_LEN];
	char home[DISKMGR_MAX_NAME_LEN];
	int used;
	int files;
};

struct TypeEntry
{
	char type[DISKMGR_MAX_TYPE_LEN];
	int size;
};

struct DiskInfoRecord 
{
   int index;
   int unum;
   struct UserEntry u[DISKMGR_MAX_USER_NUM];
   int tnum;
   struct TypeEntry t[DISKMGR_MAX_TYPE_NUM];
   int pnum;
   struct PartitionEntry p[DISKMGR_MAX_PARTITION_NUM];
   int dnum;
   struct DiskEntry d[DISKMGR_MAX_DISK_NUM];
};

#define DiskInfoRecordType struct DiskInfoRecord

//=================================================================
// Functions for Disk Manager show
//=================================================================
//  Load the type result 
BOOL loadTypeInfo(struct TypeEntry * arrTypeInfo, int *nArrSizePtr, time_t *rcd_time_ptr, const char *zFilePath1, const char *zFilePath2);
//  Show the type result 
BOOL showTypeInfo(char * const local, const struct TypeEntry *arrTypeInfo, const int nArrSize, long nRcdTime, const int nBufSize);

//=================================================================
// Simple for API begin
//=================================================================
//  Show the type result 
BOOL showTypeInfoAPI(char * const local, const struct TypeEntry *arrTypeInfo, const int nArrSize, long nRcdTime, const int nBufSize);
//=================================================================
// Simple for API end
//=================================================================


//=================================================================
//	new disk manager begin
//=================================================================
#include <sys/types.h>
#define AOS_DISKMGR_MAX_HDS 32
/*
 * In Linux they assigned lower than 16 numbers for each 
 * partitions. More can be checked from the Linux file
 * "Documentation/devices.txt". 
 * Linux except that the limit on partitions is 15. 
 */
#define AOS_DISKMGR_MAX_PART_IN_HDS 15 
#define AOS_DISKMGR_MAX_PATH_LEN 256
#define AOS_DISKMGR_MAX_USR_NAME_LEN 64
#define AOS_DISKMGR_MAX_FILE_EXT_LEN 16
#define AOS_DISKMGR_MAX_TYPE_NR 128
#define AOS_DISKMGR_MAX_USR_NR 	128

typedef struct aos_diskmgr_part_rslt
{
	char m_part_dev[AOS_DISKMGR_MAX_PATH_LEN]; /* ID of partition */
	char m_mount[AOS_DISKMGR_MAX_PATH_LEN]; 
	float m_usage_ratio;
	long long m_total;		/* total data blocks in file system */
	long long m_used;		/* used blocks in file system */
	long long m_avail; 		/* free blocks avail to non-superuser */
	long m_files_nr;
} aos_diskmgr_part_rslt_t;

typedef struct aos_diskmgr_hd_rslt
{
	char m_hd_dev[AOS_DISKMGR_MAX_PATH_LEN]; /* ID of hard disk */
	float m_usage_ratio; 					 /* lower than 1 */
	long long m_total;	
	long long m_used;
	long long m_avail;
	long m_files_nr;
	int m_part_nr;
	aos_diskmgr_part_rslt_t m_part[AOS_DISKMGR_MAX_PART_IN_HDS];
} aos_diskmgr_hd_rslt_t;

// In reality. 
// User don't care about which partition he is in. 
// User don't care about how many percents in hard disk or in partition he used.
// User don't know what is partition, except he/she is a root user.
// So we don't support user-partition usage of the system.
// So we don't support user-harddisk usage of the system.
// We just collect the user usage of the whole logical file system.
// Actually, I think user management is on VFS layer which is on Logical layer, 
// 		while partition and harddisk management is on hardware layer, 
// 		which is under user management layer. 
// So we don't make up relationship between user and partition.


//typedef struct aos_diskmgr_usr_part_rslt
//{
//	uid_t m_uid;
//	char m_part_dev[AOS_DISKMGR_MAX_PATH_LEN]; 	/* ID of partition */
//	float m_usage_ratio; 						/* lower than 1 */
//	long m_usr_total;
//	long m_usr_used;
//	long m_usr_files_nr;
//} aos_diskmgr_usr_part_rslt_t;

//typedef struct aos_diskmgr_usr_hd_rslt
//{
//	uid_t m_uid;
//	char m_hd_dev[AOS_DISKMGR_MAX_PATH_LEN]; 	/* ID of hard disk */
//	float m_usage_ratio;
//	long m_usr_total;
//	long m_usr_used;
//	long m_usr_num_files; 
//	long m_num_partitions;
//	aos_diskmgr_usr_part_rslt_t 
//		 m_usr_part[AOS_DISKMGR_MAX_HDS * AOS_DISKMGR_MAX_PART_IN_HDS];
//} aos_diskmgr_usr_hd_rslt_t;
//
//typedef struct aos_diskmgr_usr_rslt
//{
//	uid_t m_uid;
//	char m_uname[AOS_DISKMGR_MAX_USR_NAME_LEN]; 
//	long m_usr_total;
//	long m_usr_used;
//	long m_usr_num_files; 
//	long m_num_partitions;
//	aos_diskmgr_usr_hd_rslt_t 
//		 m_usr_hd[AOS_DISKMGR_MAX_HDS * AOS_DISKMGR_MAX_PART_IN_HDS];
//} aos_diskmgr_usr_rslt_t;

typedef struct aos_diskmgr_usr_rslt
{
	uid_t m_uid;		/* user ID */
	gid_t m_gid;        /* group ID */
	char m_uname[AOS_DISKMGR_MAX_USR_NAME_LEN]; 
	char m_home_path[AOS_DISKMGR_MAX_PATH_LEN];
	float m_usage;
	long long m_usr_used;           
	long m_usr_files_nr;           
} aos_diskmgr_usr_rslt_t;


typedef struct aos_diskmgr_type_rslt
{                   
		char m_type[AOS_DISKMGR_MAX_FILE_EXT_LEN];
		long long m_type_used;
		long m_type_files_nr;
} aos_diskmgr_type_rslt_t;


typedef struct aos_diskmgr_stat_rslt
{
	long m_sec_index;
	float m_total_usage_ratio;
	long long m_total;
	long long m_total_used;
	long long m_total_avail;
	int m_partitions_nr;
	int m_hds_nr;
	aos_diskmgr_hd_rslt_t m_hds_rslt[AOS_DISKMGR_MAX_HDS];
	int m_usr_nr;
	aos_diskmgr_usr_rslt_t m_usr_rslt[AOS_DISKMGR_MAX_USR_NR];
	int m_type_nr;
	aos_diskmgr_type_rslt_t m_type_rslt[AOS_DISKMGR_MAX_TYPE_NR];
} aos_diskmgr_stat_rslt_t;


//  reset the previous data structure
void aos_diskmgr_reset_stat_rslt(aos_diskmgr_stat_rslt_t * const sys_info);


//  Load the type result 
BOOL aos_diskmgr_load_type_info(aos_diskmgr_type_rslt_t * arrTypeInfo, 
								int *nArrSizePtr, time_t *rcd_time_ptr, 
								const char *zFilePath1, const char *zFilePath2);

//  Show the type result 
BOOL aos_diskmgr_show_type_info(char * const local, const int nBufSize, 
								const aos_diskmgr_type_rslt_t *arrTypeInfo, 
								const int nArrSize, long nRcdTime);

//  Show the type result 
BOOL aos_diskmgr_show_type_info_api(char * const local, const int nBufSize, 
									const aos_diskmgr_type_rslt_t *arrTypeInfo, 
									const int nArrSize, long nRcdTime);

// Get the system current status information
BOOL aos_diskmgr_get_status_info(aos_diskmgr_stat_rslt_t * const sys_info);

// Get the system hard disk status result
// 		Get the system partition status result
// Initialize the system harddisk first
BOOL aos_diskmgr_init_hd_info(aos_diskmgr_hd_rslt_t * const hd_info, 
							  int * arr_size);
BOOL aos_diskmgr_get_hd_info(aos_diskmgr_hd_rslt_t * const hd_info, 
							 const char * hd_dev_path);
BOOL aos_diskmgr_get_part_info(aos_diskmgr_part_rslt_t * const part_info, 
							   const char * part_dev_path, 
							   const char * part_mount_path);

// Get the system file type status result
// BOOL aos_diskmgr_get_type_info(aos_diskmgr_type_rslt_t * const type_info);
BOOL aos_diskmgr_init_type_info(aos_diskmgr_type_rslt_t * const type_info, 
								int * arr_size);

// Get the system usr space status result
BOOL aos_diskmgr_init_usr_info( aos_diskmgr_usr_rslt_t * const usr_info, 
								int * arr_size);

// Get the system status of 
// 		file type
// 		user information
BOOL aos_diskmgr_get_vfs_status(aos_diskmgr_type_rslt_t * const type_info, 
								const int type_arr_size, 
								aos_diskmgr_usr_rslt_t * const usr_info, 
								const int usr_arr_size,
								const char * curr_path, 
								const BOOL b_recursive);

//=================================================================
//	new disk manager end
//=================================================================

//=================================================================
// Tool functions for showing Partition of the harddisk
//=================================================================

typedef struct aos_format_harddisk_input
{
		int m_size; /* unit is mega-bytes */
		char m_part_format[16];
} aos_format_harddisk_input_t;

// API 8. 
int aos_diskmgr_get_num_hd(int * hd_num);
// API 9. 
int aos_diskmgr_get_num_partition(int * part_num, const char * dev_path);
// API 10. 
int aos_diskmgr_get_partition_info(
				aos_diskmgr_part_rslt_t * const part_info, 
				const char * dev_path);
// API 11. 
int aos_diskmgr_get_harddisk_info(aos_diskmgr_hd_rslt_t * const d, 
								   const char * dev_path);
// API 12. 
int aos_diskmgr_get_all_hard_disks(aos_diskmgr_hd_rslt_t * const arr_hd, 
								   int *num_hd);

// API 13. 
int aos_diskmgr_format_hard_disk(aos_diskmgr_hd_rslt_t * const harddisk_info, 
								 const char* hd_dev, 
								 const aos_format_harddisk_input_t* set_info);

//=================================================================
// Tool functions for showing Partition of the harddisk
//=================================================================

#endif //  aos_rhcUtil_aosDiskMgrToolkit_h
