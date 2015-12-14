////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Global.h
// Description:
//   
//
// Modification History:
// 2007-02-15 Created by CHK
////////////////////////////////////////////////////////////////////////////
#ifndef aos_DiskMgr_Global_h
#define aos_DiskMgr_Global_h

#include "rhcUtil/aosDiskMgrToolkit.h"
#include "rhcUtil/aosResMgrApp.h"
#include "rhcUtil/aosResMgrDefines.h"

// for thread
#include <pthread.h>

#define DISKMGR_MAX_USER_INFO_LEN 	4096
#define DISKMGR_MAX_TYPE_INFO_LEN 	10240
#define DISKMGR_MAX_PARTITION_INFO_LEN 	1024
#define DISKMGR_MAX_DISK_INFO_LEN 	1024

/* Files */
#define DISK_INFO_FILE_0 "/var/log/aos_disk_log.0"
#define DISK_INFO_FILE_1 "/var/log/aos_disk_log.1"

#define _DISKMGR_TYPEFILE "/usr/local/.type"
#define _DISKMGR_TYPEFILE_TMP "/usr/local/.type.tmp"

#define DISKMGR_SAMPL_INTERVAL_DEFAULT 1*60*60 	 // "1H"
#define DISKMGR_PREV_STAT_MAX 1 

struct DiskMgr_Alarm_Val
{
	int m_nFreeRatio;	//  = 0
	int m_nAlarmFlag;	// (m_nAlarmFlag*ninterval) time period to send alarm = 0
	BOOL m_bIsOverload; //  = FALSE
};
void reset_DiskMgr_Alarm_Val(struct DiskMgr_Alarm_Val * const ptr);

struct DiskMgr_App
{
	aos_diskmgr_stat_rslt_t m_DiskInfoSnap;
	aos_diskmgr_stat_rslt_t m_DiskInfoSnapPrev[DISKMGR_PREV_STAT_MAX];
	long m_duration; 	// = 0;
	long m_index; 		// = 0;
	int m_interval; 	// = 1;
	int m_maxr; 		// = 2;
	struct DiskMgr_Alarm_Val m_DiskMgrAlarm;

	BOOL m_is_first_sample; 	// = TRUE;
	struct ResMgr_App m_resmgr_app;
};
void reset_config_DiskMgr_App(struct DiskMgr_App * const ptrApp);
void reset_DiskMgr_App(struct DiskMgr_App * const ptr);

// Declare global variables
extern struct DiskMgr_App g_theDiskMgrApp;

// Encapsulate the operation of g_theCpuMgrApp
void get_DiskMgr_ResMgr_Threshold_App(ResMgr_Threshold_t * const ptr);
// Encapsulate the operation of g_theCpuMgrApp
void set_DiskMgr_ResMgr_Threshold_App(const ResMgr_Threshold_t * ptr);

///////////////////////////////////
// By CHK
// 2007-01-31
// hard disk manager
///////////////////////////////////
// Disk Mgr APIs
#define DISKMGR_PARTITION_FILE 	"/proc/partitions"
#define DISKMGR_MOUNTS_FILE 	"/etc/mtab"
#define DISKMGR_PARTITION_MAX_NUM 	32
#define DISKMGR_HDISK_MAX_NUM 		32

typedef struct ros_harddisk_info
{
	int hd_id;					/* u32 */
	int num_partitions;  		/* u32 */
	unsigned long space_total;
	unsigned long space_used;
}ros_harddisk_info_t;

typedef struct ros_partition_info 
{
	int partition_id;  			/* u32 */
	int hd_id;			  		/* u32 */
	int is_mounted;		  		/* u32 */
	char mount_path[256];  		/* mount from path */
	char mount_dir[256];   		/* mount to dir */
	unsigned long space_total;	/* total data blocks in file system */
	unsigned long space_used;	/* other from free blocks in fs */
	unsigned long space_avail;	/* free blocks avail to non-superuser */
}ros_partition_info_t;

typedef struct ros_sys_harddisk_info_app
{
	time_t tLoadTime;
	int hd_num;
	ros_harddisk_info_t hd_info[DISKMGR_HDISK_MAX_NUM];
	int part_num;
	ros_partition_info_t part_info[DISKMGR_PARTITION_MAX_NUM];
}ros_sys_harddisk_info_app_t;

void reset_ros_sys_harddisk_info_app_t(ros_sys_harddisk_info_app_t * const ptr);

///////////////////////////////////////////////////////////////////////////////////
// Tool functions for showing Partition of the harddisk
///////////////////////////////////////////////////////////////////////////////////
extern ros_sys_harddisk_info_app_t g_theAPIDiskMgrApp;
//  API: Init API function
int ros_disk_mgr_init();
BOOL ros_load_sys_hd_info(ros_sys_harddisk_info_app_t * const theApp);
int ros_get_harddisk_info(int hd, ros_harddisk_info_t * const diskinfo);
int ros_get_partition_info(int hd, int partition, struct ros_partition_info * const p );
char* showPartitionInfo(char * const display_container, const struct ros_partition_info * part_info_ptr, int nArrNum);
char* showHarddiskInfo(char * const display_container, const ros_harddisk_info_t * hd_info_ptr, const int nArrNum);

// API: ros_get_all_hard_disks
// Get all hard disks 
// return value: the number of retrieved harddisks
int ros_get_all_hard_disks(ros_harddisk_info_t * const d, int *num_hd);
// API: ros_get_all_partitions
// Get all hard disks 
// return value: the number of retrieved partitions
int ros_get_all_partitions(struct ros_partition_info * const p, int *num_part);

///////////////////////////////////////////////////////////////////////////////////
// Tool functions for Create or Kill the thread
///////////////////////////////////////////////////////////////////////////////////
// encapsulate the process of create a detached thread
BOOL diskMgrSwitchOn();
// encapsulate the process of kill a detached thread
BOOL diskMgrSwitchOff();

///////////////////////////////////////////////////////////////////////////////////
// Tool functions for DiskMgr main.cpp and thread
///////////////////////////////////////////////////////////////////////////////////
// print help infomation and exit the program
void printDiskMgrHelp(const char *program_name);

// set sampling record interval or duration
int aos_diskmgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen);

// running this function after each g_theDiskMgrApp.m_interval
void aos_disk_mgr_repeat(int nSignal);

// thread function
void diskMgrThreadFunc(void * ); //(res_thread_struct_t * disk_thread_input)

#endif // aos_DiskMgr_Global_h
