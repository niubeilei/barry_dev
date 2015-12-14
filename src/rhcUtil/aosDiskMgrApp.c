////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 2007-02-15 Created by CHK
////////////////////////////////////////////////////////////////////////////

// Our RHC System
#include "rhcUtil/aosDiskMgrApp.h"
#include "rhcUtil/aosResMgrToolkit.h"
#include "aosUtil/Alarm.h"

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/vfs.h>    /* or <sys/statfs.h> */
#include <mntent.h>

// Linux programmer time
#include <sys/time.h>

void reset_DiskMgr_Alarm_Val(struct DiskMgr_Alarm_Val * const ptr)
{
	ptr->m_nFreeRatio = 0;	//  = 0 /* the space ratio of free */
	ptr->m_nAlarmFlag = 0;	//  = 0 /*(m_nAlarmFlag*ninterval) time period to send alarm*/ 
	ptr->m_bIsOverload = FALSE;
}

void reset_config_DiskMgr_App(struct DiskMgr_App * const ptrApp)
{
	ptrApp->m_duration = RESMGR_SAMPL_DURATION_DEFAULT;
	ptrApp->m_interval = DISKMGR_SAMPL_INTERVAL_DEFAULT;
	ptrApp->m_maxr 	   = ptrApp->m_duration / ptrApp->m_interval;
	// reset the resource manager normal configuration values
	reset_ResMgr_Config(&(ptrApp->m_resmgr_app));
}

void reset_DiskMgr_App(struct DiskMgr_App * const ptrApp)
{
	ptrApp->m_index    = 0;
	ptrApp->m_is_first_sample = TRUE;
	reset_DiskMgr_Alarm_Val(&(ptrApp->m_DiskMgrAlarm));

	ptrApp->m_duration = RESMGR_SAMPL_DURATION_DEFAULT;
	ptrApp->m_interval = DISKMGR_SAMPL_INTERVAL_DEFAULT;
	ptrApp->m_maxr 	   = ptrApp->m_duration / ptrApp->m_interval;
	reset_ResMgr_App(&(ptrApp->m_resmgr_app));
}

// Note: declare global variable here!!!
struct DiskMgr_App g_theDiskMgrApp;

// Encapsulate the operation of g_theDiskMgrApp
void get_DiskMgr_ResMgr_Threshold_App(ResMgr_Threshold_t * const ptr)
{
	if(ptr)
	{
		(*ptr) = g_theDiskMgrApp.m_resmgr_app.m_threshold_val;
	}
}

// Encapsulate the operation of g_theDiskMgrApp
void set_DiskMgr_ResMgr_Threshold_App(const ResMgr_Threshold_t * ptr)
{
	if(ptr)
	{
		if(g_theDiskMgrApp.m_resmgr_app.m_switch_on)
		{
			pthread_mutex_lock (&g_theDiskMgrApp.m_resmgr_app.m_mutex);
		}
		aos_resmgr_copy_threshold(&g_theDiskMgrApp.m_resmgr_app.m_threshold_val, ptr);
		if(g_theDiskMgrApp.m_resmgr_app.m_switch_on)
		{
			pthread_mutex_unlock (&g_theDiskMgrApp.m_resmgr_app.m_mutex);
		}
	}
}

void aos_diskmgr_folder_snap(struct DiskInfoRecord *snap, char *fullpath, const BOOL bIsRecursive)
{
	struct dirent *dirp;
	struct stat statbuf;
	char *type = NULL;
	DIR *dir = NULL;
//	char *ptr = NULL;
	int i;
	
	if (lstat(fullpath, &statbuf) < 0)
	{
#ifdef _DEBUG_
		aos_alarm( eAosMD_AosUtil, eAosAlarm_General,
				"lsstat %s error\n", fullpath);
#endif
		return;
	}

	if ((S_ISLNK(statbuf.st_mode)) != 0)
	{
		return;
	}
	else if (! S_ISDIR(statbuf.st_mode) )
	{
		type = 0;
		for (i = strlen(fullpath) - 1; i >= 0; i--)
		{
			if (fullpath[i] == '.') 
			{
				type = fullpath + i + 1;
				break;
			}
			if (fullpath[i] == '/')
				break;
		}
		if (type == NULL)
			type = "unknown";

		for (i = 0; i < snap->tnum; i++)
		{
			if (strcmp(snap->t[i].type, type) == 0)
			{
				snap->t[i].size += statbuf.st_size;
				break;
			}
		}
		return;
	}
	else
	{
		// is dir

		/*
	ptr = fullpath + strlen(fullpath);
	if (*ptr != '/')
	{
		*ptr ++ = '/';
		*ptr = 0;
	}	
		*/

	if (NULL == (dir = opendir(fullpath)))
		return;

	while (NULL != (dirp = readdir(dir))) 
	{
		if (strcmp(dirp->d_name,".") != 0
			&& strcmp(dirp->d_name,"..") != 0)
		{
			char path[1024];
			snprintf(path,1024,"%s/%s",fullpath,dirp->d_name);
			aos_diskmgr_folder_snap(snap, path, bIsRecursive);
		}
	}
	closedir(dir);
	}
	return;
}

// 2007-04-27
// if running well in this week 
// we may delete this function!
BOOL getDiskInfo(struct DiskInfoRecord *snap)
{
	struct mntent *mnt;
	struct statfs fs;
	struct stat statbuf;
	char diskname[16]; 
	char readbuf[256];
	char * zToken = NULL;
	FILE *fp = NULL;
	int index = 0;
	int i, nTmpSize;

	// Get the User Entry
	readbuf[0] = 0; 
	if ((fp = fopen("/etc/passwd", "r")) == NULL) 
	{
#ifdef _DEBUG_
		aos_alarm(eAosMD_AosUtil, eAosAlarm_General, 
			"open the passwd file error");
#endif
		return FALSE; 
	}

	index = 0;
	nTmpSize = sizeof(readbuf);
	while (fgets(readbuf, nTmpSize, fp) != NULL) 
	{
		zToken = strtok(NULL, ":");
		if(NULL == zToken)
		{
			break;
		}
		strcpy(snap->u[index].name, zToken);
		for (i = 0; i < 4; i++)
		{
			strtok(NULL, ":");
		}
		zToken = strtok(NULL, ":");
		if(NULL == zToken)
		{
			break;
		}
		strcpy(snap->u[index].home, zToken); 

		if (lstat(snap->u[index].home, &statbuf) < 0)
		{
			continue;
		}
		else if ((S_ISDIR(statbuf.st_mode)) == 0)
		{
			continue;
		}
		statfs(snap->u[index].home, &fs);

		snap->u[index].used = fs.f_blocks;
		snap->u[index].files = fs.f_files;

		index++;
	}

	fclose(fp);
	snap->unum = index;

	// Get the Partition Entry
	index = 0;
	fp = setmntent("/etc/mtab", "r");

	while ((mnt = getmntent(fp)) != NULL) 
	{
		if (NULL == strstr(mnt->mnt_fsname, "/dev/"))
			continue;
		if (strstr(mnt->mnt_dir, "/dev/") != NULL)
			continue;
		statfs(mnt->mnt_dir, &fs);
		strcpy(diskname, mnt->mnt_fsname);
		nTmpSize = sizeof(diskname);
		for (i = 0; i < nTmpSize; i++) 
		{
			if (diskname[i] >= '0' && diskname[i] <= '9') 
			{
				diskname[i] = '\0';
				break;
			}
		}
		strcpy(snap->p[index].dname, diskname);
		strcpy(snap->p[index].pname, mnt->mnt_fsname);
		strcpy(snap->p[index].mount, mnt->mnt_dir);
		snap->p[index].used = fs.f_blocks - fs.f_bfree;
		snap->p[index].total = fs.f_blocks;
		snap->p[index].files = fs.f_files;
		index ++;
	}

	fclose(fp);
	snap->pnum = index;

	// Get the Disk Entry
	index = 0;
	snap->dnum = 0;

	for (i = 0; i < snap->pnum; i++)
	{
		for (index = 0; index < snap->dnum; index ++)
		{
			if(strcmp(snap->p[i].dname, snap->d[index].diskname) == 0)
				break;
		}
		if(DISKMGR_MAX_DISK_NUM == index)
		{
			// error!!!
			// break;
			return FALSE;
		}
		if(index == snap->dnum)
		{
			strcpy(snap->d[index].diskname, snap->p[i].dname);
			snap->d[index].used = snap->p[i].used;
			snap->d[index].total = snap->p[i].total;
			snap->d[index].files = snap->p[i].files;
			snap->d[index].parts = 1;
			snap->dnum ++;
		}
		else
		{
			snap->d[index].used += snap->p[i].used;
			snap->d[index].total += snap->p[i].total; 
			snap->d[index].files += snap->p[i].files;
			snap->d[index].parts ++;
		}
	}

	//Get the Type Entry
	fp = fopen(_DISKMGR_TYPEFILE, "r");
	if (fp)
	{
		index = 0;
		readbuf[0] = 0;
		nTmpSize = sizeof(readbuf);
		// getline(&line, &len, fp)
		while ((fgets(readbuf, nTmpSize, fp)) != NULL) 
		{
			for (i = 0; i < nTmpSize; i++) 
			{
				switch(readbuf[i])
				{
					case '\n':
					case '\0':
						readbuf[i] = '\0';
						break;
					default:;
				}
				if ('\0' == readbuf[i])
				{
					break;
				}
			}
			if(0 == readbuf[0])
			{
				break;
			}
			strncpy(snap->t[index].type, readbuf, DISKMGR_MAX_TYPE_LEN);
			snap->t[index].size = 0;
			// memset(readbuf, 0, sizeof(readbuf));
			readbuf[0] = 0;
			index ++;
		}
		snap->tnum = index;
	}
	else
	{
#ifdef _DEBUG_
		aos_alarm(eAosMD_AosUtil, eAosAlarm_General, 
			"no any type, please add the file types\n");
#endif
//		return FALSE;
	}

	if(fp)
	{
		fclose(fp);
	}
	char fullpath[DISKMGR_MAX_DIR_LEN];
	strcpy(fullpath, "/");

	aos_diskmgr_folder_snap(snap, fullpath, TRUE);
	return TRUE;
}


// save disk status information 
BOOL saveDiskInfo(const long index, const int maxr, const int interval, const long duration, struct DiskInfoRecord *snap)
{
	char user_info[DISKMGR_MAX_USER_INFO_LEN];
	char type_info[DISKMGR_MAX_TYPE_INFO_LEN];
	char disk_info[DISKMGR_MAX_DISK_INFO_LEN];
	char partition_info[DISKMGR_MAX_PARTITION_INFO_LEN];
	int i = 0;
	char tmp[64];
	char *zLocalTime = NULL;
	FILE *fp = NULL;

	if (0 == ((index / maxr) % 2))
	{
		if (0 == (index % maxr))
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if (NULL == zLocalTime || NULL == (fp = fopen(DISK_INFO_FILE_0, "w+"))) 
			{
				return FALSE;
			}
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if(NULL == (fp = fopen(DISK_INFO_FILE_0, "a")))
				return FALSE; 
		}
	}
	else
	{
		if (0 == (index%maxr))
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if (NULL == zLocalTime || NULL == (fp = fopen(DISK_INFO_FILE_1, "w+")))
			{
				return FALSE;
			}
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if (NULL == (fp = fopen(DISK_INFO_FILE_1, "a")))
				return FALSE;
		}
	}

	// Save the Partition Record
	sprintf(partition_info, "%d", snap->pnum);
	for(i = 0; i < snap->pnum; i++)
	{
		strcat(partition_info, " ");
		sprintf(tmp, "%s %s %s %d %d %d", snap->p[i].dname, snap->p[i].pname, snap->p[i].mount, snap->p[i].used, snap->p[i].total, snap->p[i].files);
		strcat(partition_info, tmp);
	}

	// Save the Disk Record
	sprintf(disk_info, "%d", snap->dnum);
	for(i = 0; i < snap->dnum; i++)
	{
		strcat(disk_info, " ");
		sprintf(tmp, "%s %d %d %d %d", snap->d[i].diskname, snap->d[i].used, snap->d[i].total, snap->d[i].files, snap->d[i].parts);
		strcat(disk_info, tmp);
	}

	// Save the Type Record
	sprintf(type_info, "%d", snap->tnum);
	for(i = 0; i < snap->tnum; i++)
	{
		strcat(type_info, " ");
		sprintf(tmp, "%s %d", snap->t[i].type, snap->t[i].size);
		strcat(type_info, tmp);
	}
#ifdef _DEBUG_
	printf("type_info:[%s]\nsnap->tnum:[%d]\n", type_info, snap->tnum);
#endif

 	// Save the User Record
	sprintf(user_info, "%d", snap->unum);
	for(i = 0; i < snap->unum; i++)
	{
		strcat(user_info, " ");
		sprintf(tmp, "%s %s %d %d", snap->u[i].name, snap->u[i].home, snap->u[i].files, snap->u[i].used);
		strcat(user_info, tmp);
	}

   	fprintf(fp, "%ld %s %s %s %s\n", (time(NULL)), \
		partition_info, disk_info, user_info, type_info);
#ifdef _DEBUG_
	printf("%ld %s %s %s %s\n", (time(NULL)), \
		partition_info, disk_info, user_info, type_info);
#endif
	fclose(fp); 

	return TRUE;
}


// save disk status information 
BOOL aos_diskmgr_save_status_info(const long index, const int maxr, const int interval, const long duration, const aos_diskmgr_stat_rslt_t * snap)
{
	char user_info[DISKMGR_MAX_USER_INFO_LEN];
	char type_info[DISKMGR_MAX_TYPE_INFO_LEN];
	char disk_info[DISKMGR_MAX_DISK_INFO_LEN];
	char partition_info[DISKMGR_MAX_PARTITION_INFO_LEN];
	int i, j;
	char tmp[256];
	char *zLocalTime = NULL;
	FILE *fp = NULL;

#ifdef _DEBUG_ALARM_PRINT
				printf("FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
	if (0 == ((index / maxr) % 2))
	{
		if (0 == (index % maxr))
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if (NULL == zLocalTime || NULL == (fp = fopen(DISK_INFO_FILE_0, "w+"))) 
			{
				return FALSE;
			}
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if(NULL == (fp = fopen(DISK_INFO_FILE_0, "a")))
				return FALSE; 
		}
	}
	else
	{
		if (0 == (index%maxr))
		{
			zLocalTime = getLocalTimeStr(RESMGR_TIME_FMT);
			if (NULL == zLocalTime || NULL == (fp = fopen(DISK_INFO_FILE_1, "w+")))
			{
				return FALSE;
			}
			fprintf(fp, "%s %d %ld %d\n", zLocalTime, interval, duration, maxr);
		}
		else
		{
			if (NULL == (fp = fopen(DISK_INFO_FILE_1, "a")))
				return FALSE;
		}
	}

#ifdef _DEBUG_ALARM_PRINT
				printf("FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
	// Save the Partition Record
	sprintf(partition_info, "%d", snap->m_partitions_nr);
	for(i = 0; i < snap->m_hds_nr; i++)
	{
		for(j = 0; j < snap->m_hds_rslt[i].m_part_nr; j++)
		{
			strcat(partition_info, " ");
			sprintf(tmp, "%s %s %s %lld %lld %ld", 
					snap->m_hds_rslt[i].m_hd_dev, 
					snap->m_hds_rslt[i].m_part[j].m_part_dev, 
					snap->m_hds_rslt[i].m_part[j].m_mount, 
					snap->m_hds_rslt[i].m_part[j].m_used, 
					snap->m_hds_rslt[i].m_part[j].m_total, 
					snap->m_hds_rslt[i].m_part[j].m_files_nr);
			strcat(partition_info, tmp);
		}
	}

#ifdef _DEBUG_ALARM_PRINT
				printf("FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
	// Save the Disk Record
	sprintf(disk_info, "%d", snap->m_hds_nr);
	for(i = 0; i < snap->m_hds_nr; i++)
	{
		strcat(disk_info, " ");
		sprintf(tmp, "%s %lld %lld %ld %d", 
					snap->m_hds_rslt[i].m_hd_dev, 
					snap->m_hds_rslt[i].m_used, 
					snap->m_hds_rslt[i].m_total, 
					snap->m_hds_rslt[i].m_files_nr, 
					snap->m_hds_rslt[i].m_part_nr);
		strcat(disk_info, tmp);
	}

 	// Save the User Record
	sprintf(user_info, "%d", snap->m_usr_nr);
	for(i = 0; i < snap->m_usr_nr; i++)
	{
		strcat(user_info, " ");
		sprintf(tmp, "%s %s %ld %lld", 
				snap->m_usr_rslt[i].m_uname, 
				snap->m_usr_rslt[i].m_home_path, 
				snap->m_usr_rslt[i].m_usr_files_nr, 
				snap->m_usr_rslt[i].m_usr_used);
		strcat(user_info, tmp);
	}

	// Save the Type Record
	sprintf(type_info, "%d", snap->m_type_nr);
	for(i = 0; i < snap->m_type_nr; i++)
	{
		strcat(type_info, " ");
		sprintf(tmp, "%s %lld", snap->m_type_rslt[i].m_type, snap->m_type_rslt[i].m_type_used);
		strcat(type_info, tmp);
	}
#ifdef _DEBUG_
	printf("type_info:[%s]\nsnap->tnum:[%d]\n", type_info, snap->tnum);
#endif

   	fprintf(fp, "%ld %s %s %s %s\n", (time(NULL)), \
		partition_info, disk_info, user_info, type_info);
	fclose(fp); 

	return TRUE;
}


// print help infomation and exit the program
void printDiskMgrHelp(const char *program_name)
{
	printf ("Usage: \n%s [Interval] [Duration]\n"
			"Description: \n\tshow Disk statistics daemon %s\n"
			"\t[Interval], Interger number, For setting the sampling time interval.\n"
			"\t[Duration], Interger number, For setting the sampling data time length.\n"
			,program_name, program_name);
}


// VFS statistics design:
// FreeRatio = (freeSize)/(totalSize) * 100;
int getFreeRatio(const aos_diskmgr_stat_rslt_t *snapCurr)
{
	return (int)(100 * snapCurr->m_total_usage_ratio);
}


// hard disk usage checking
// assuming it's overload now
BOOL IsOverload(const int nFreeRatio, int *nAlarmFlag)
{
	BOOL bStatus = FALSE;
	if(100 - nFreeRatio >= g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdRatio)
		(*nAlarmFlag)++;
	else
		(*nAlarmFlag) = 0;
		
	bStatus = 
			(	(*nAlarmFlag) * g_theDiskMgrApp.m_interval
				>= g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nAlarmThresholdTime);

	(*nAlarmFlag) = (bStatus) ? 0 : (*nAlarmFlag);
	return bStatus;
}


// hard disk normalize checking
// assuming it's underload now
BOOL IsUnderload(const int nFreeRatio, int *nAlarmFlag)
{
	BOOL bStatus = FALSE;
	if(100 - nFreeRatio < g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdRatio)
		(*nAlarmFlag)++;
	else
		(*nAlarmFlag) = 0;
		
	bStatus = 
			!(	(*nAlarmFlag) * g_theDiskMgrApp.m_interval
				>= g_theDiskMgrApp.m_resmgr_app.m_threshold_val.m_nNormalThresholdTime);

	(*nAlarmFlag) = (bStatus) ? 0 : (*nAlarmFlag);
	return bStatus;
}


// running this function after each interval
void aos_disk_mgr_repeat(int nSignal)
{
	aos_diskmgr_get_status_info(&g_theDiskMgrApp.m_DiskInfoSnap);
#ifdef _DEBUG_ALARM_PRINT
				printf("FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
	if(!g_theDiskMgrApp.m_is_first_sample)
	{
		// doing the statistics process
		g_theDiskMgrApp.m_DiskMgrAlarm.m_nFreeRatio = 
			getFreeRatio(&g_theDiskMgrApp.m_DiskInfoSnap);
#ifdef _DEBUG_ALARM_PRINT
				printf("disk m_nFreeRatio = %d! FILE(%s)LINE(%d)\n",
						g_theDiskMgrApp.m_DiskMgrAlarm.m_nFreeRatio, __FILE__, __LINE__ );
#endif
		if(!g_theDiskMgrApp.m_DiskMgrAlarm.m_bIsOverload)
		{
			// doing the checking process
			g_theDiskMgrApp.m_DiskMgrAlarm.m_bIsOverload = 
						IsOverload(  g_theDiskMgrApp.m_DiskMgrAlarm.m_nFreeRatio, 
									&g_theDiskMgrApp.m_DiskMgrAlarm.m_nAlarmFlag);
			if(g_theDiskMgrApp.m_DiskMgrAlarm.m_bIsOverload)
			{
				// Sending up to alarm state message!
				aos_alarm( eAosMD_AosUtil, eAosAlarm_General, 
					"Hard disk alarm: hard disk is overload!");
#ifdef _DEBUG_ALARM_PRINT
				printf("Hard disk alarm: hard disk is overload! FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
				TiXmlDocument writeXmlDoc("./test.xml");
#endif
			//	write_alarm("Hard disk alarm: hard disk is overload!");
			}
		}
		else
		{
			// doing the checking process
			g_theDiskMgrApp.m_DiskMgrAlarm.m_bIsOverload = 
						IsUnderload( g_theDiskMgrApp.m_DiskMgrAlarm.m_nFreeRatio, 
										&g_theDiskMgrApp.m_DiskMgrAlarm.m_nAlarmFlag);
			if(!g_theDiskMgrApp.m_DiskMgrAlarm.m_bIsOverload)
			{
				// Sending down to normal state message!
				aos_alarm( eAosMD_AosUtil, eAosAlarm_General, 
					"Hard disk alarm: hard disk is underload!");
#ifdef _DEBUG_ALARM_PRINT
				printf("Hard disk alarm: hard disk is underload! FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
			//	write_alarm("Hard disk alarm: hard disk is underload!");
			}
		}
	}
	else
	{
		g_theDiskMgrApp.m_is_first_sample = FALSE;
	}
#ifdef _DEBUG_ALARM_PRINT
				printf("FILE(%s)LINE(%d)\n", __FILE__, __LINE__ );
#endif
	aos_diskmgr_save_status_info(g_theDiskMgrApp.m_index, 
								 g_theDiskMgrApp.m_maxr, 
								 g_theDiskMgrApp.m_interval, 
								 g_theDiskMgrApp.m_duration, 
								&g_theDiskMgrApp.m_DiskInfoSnap);

	// Doing the data copy and setting process
	++g_theDiskMgrApp.m_index;
	if(g_theDiskMgrApp.m_index >= g_theDiskMgrApp.m_maxr*2)
	{
		g_theDiskMgrApp.m_index = 0;
	}
	g_theDiskMgrApp.m_DiskInfoSnapPrev[g_theDiskMgrApp.m_index%DISKMGR_PREV_STAT_MAX] 
				=  g_theDiskMgrApp.m_DiskInfoSnap;
}

// set sampling record interval or duration
int aos_diskmgr_set_record_time(int argc, char **argv, char * zErrmsg, const int nErrorLen)
{
	return aos_resmgr_set_record_time(argc, argv, 
									zErrmsg, nErrorLen, 
									&g_theDiskMgrApp.m_maxr, &g_theDiskMgrApp.m_duration, 
									&g_theDiskMgrApp.m_interval);
}

// main --> refine to thread 
void diskMgrThreadFunc(void * disk_thread_input) //(res_thread_struct_t * disk_thread_input)
{
	char zErrmsg[256] = "";
	int rc;

	// 1. Validate the input 
	if(strcasecmp(((res_thread_struct_t *)disk_thread_input)->argv1, "--help") == 0)
	{
		printDiskMgrHelp(((res_thread_struct_t *)disk_thread_input)->argv0);
		return ;
	}

	if(0 != getuid())
	{
		strcpy(zErrmsg, "Error: Authority not enough.");
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)disk_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 2. Get the input value
	g_theDiskMgrApp.m_interval = getDuration(((res_thread_struct_t *)disk_thread_input)->argv1);
	g_theDiskMgrApp.m_duration = getDuration(((res_thread_struct_t *)disk_thread_input)->argv2);
	if(g_theDiskMgrApp.m_duration <= 0 || g_theDiskMgrApp.m_interval <= 0)
	{
		snprintf(zErrmsg, 256, "Error: Illegal input. \nargv0[%s]\nargv1[%s]\nargv2[%s]\n", 
								((res_thread_struct_t *)disk_thread_input)->argv0, 
								((res_thread_struct_t *)disk_thread_input)->argv1, 
								((res_thread_struct_t *)disk_thread_input)->argv2);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)disk_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}
	// 3. Get the key value 
	g_theDiskMgrApp.m_maxr = g_theDiskMgrApp.m_duration / g_theDiskMgrApp.m_interval;
	if(g_theDiskMgrApp.m_maxr > MAX_TIME_RATIO || g_theDiskMgrApp.m_maxr < MIN_TIME_RATIO)
	{
		// illegal input! 
		snprintf(zErrmsg, 256, "Error: Records number [%d] out of bound [%d,%d].", 
								g_theDiskMgrApp.m_maxr, MIN_TIME_RATIO, MAX_TIME_RATIO);
//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)disk_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 4.1 if the old thread is running 
	if(g_theDiskMgrApp.m_resmgr_app.m_status_on)
	{
		snprintf(zErrmsg, 256, "Error: system failure. The old manager thread is running.");
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}
	g_theDiskMgrApp.m_resmgr_app.m_status_on = TRUE;

	// 4.2 move and cover the old file to *.1 file (keep some old record)
	if(!backupLastResFile(DISK_INFO_FILE_0, DISK_INFO_FILE_1))
	{
		snprintf(zErrmsg, 256, "Error: system failure when backup the files [%s] and [%s].", DISK_INFO_FILE_0, DISK_INFO_FILE_1);

//		printf (USAGE_INFO_FMT,zErrmsg,((res_thread_struct_t *)disk_thread_input)->argv0);
#ifdef _DEBUG_RES_MGR_TRACE_ON_
		if(_DEBUG_RES_MGR_TRACE_ON_)
		{
			aos_rhc_mgr_trace("%s", zErrmsg);
		}
#endif
		return ;
	}

	// 4.3 registe the signal handler
	reset_reg_alarm_sig(&g_theDiskMgrApp.m_resmgr_app.m_resmgr_sigactions);
	
	// 4.4 set sleep time 
	struct timespec timeout;

	// 5. Always hold the recent maxr number records
	if(g_theDiskMgrApp.m_resmgr_app.m_switch_on)
	{
		aos_disk_mgr_repeat(0);
	}
	while (g_theDiskMgrApp.m_resmgr_app.m_switch_on)
	{
		pthread_mutex_lock (&g_theDiskMgrApp.m_resmgr_app.m_mutex);
		/* prepare timeout value.              */
		/* Note that we need an absolute time. */
		clock_gettime(CLOCK_REALTIME, &timeout);
		// Delaying interval 
		timeout.tv_sec += g_theDiskMgrApp.m_interval;
		rc = pthread_cond_timedwait(&g_theDiskMgrApp.m_resmgr_app.m_thread_kill, &g_theDiskMgrApp.m_resmgr_app.m_mutex, &timeout);
		pthread_mutex_unlock (&g_theDiskMgrApp.m_resmgr_app.m_mutex);
		if(g_theDiskMgrApp.m_resmgr_app.m_switch_on)
		{
			aos_disk_mgr_repeat(0);
		}
	}
	g_theDiskMgrApp.m_resmgr_app.m_status_on = FALSE;

	// 6. Leave the current thread
	{
		pthread_mutex_lock (&g_theDiskMgrApp.m_resmgr_app.m_mutex);
		pthread_cond_signal(&g_theDiskMgrApp.m_resmgr_app.m_thread_stopped);
		pthread_mutex_unlock (&g_theDiskMgrApp.m_resmgr_app.m_mutex);
	}

	return ;
}

// encapsulate the process of create a detached thread
BOOL diskMgrSwitchOn()
{
	strcpy(g_theDiskMgrApp.m_resmgr_app.m_thread_input.argv0, "DiskMgr");
	sprintf(g_theDiskMgrApp.m_resmgr_app.m_thread_input.argv1, "%d", g_theDiskMgrApp.m_interval);
	sprintf(g_theDiskMgrApp.m_resmgr_app.m_thread_input.argv2, "%ld", g_theDiskMgrApp.m_duration);
	g_theDiskMgrApp.m_resmgr_app.m_switch_on = TRUE;
	
	// create the thread 
	aos_uland_thread_create(&g_theDiskMgrApp.m_resmgr_app.m_thread, 
							diskMgrThreadFunc, 
							&g_theDiskMgrApp.m_resmgr_app.m_thread_input, 
							&g_theDiskMgrApp.m_resmgr_app.m_mutex, 
							&g_theDiskMgrApp.m_resmgr_app.m_thread_kill, 
							&g_theDiskMgrApp.m_resmgr_app.m_thread_stopped,
							&g_theDiskMgrApp.m_resmgr_app.m_switch_on);
	return TRUE;
}

// encapsulate the process of kill a detached thread
BOOL diskMgrSwitchOff()
{
	// if the old thread is not running, return 
	if(!g_theDiskMgrApp.m_resmgr_app.m_switch_on)
	{
		return TRUE;
	}

	// kill old thread
	g_theDiskMgrApp.m_resmgr_app.m_switch_on = FALSE;

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += (RESMGR_EXPIRED_TIME_DEFAULT);
	aos_uland_thread_destroy(&g_theDiskMgrApp.m_resmgr_app.m_thread, 
							 &g_theDiskMgrApp.m_resmgr_app.m_mutex, 
							 &g_theDiskMgrApp.m_resmgr_app.m_thread_kill, 
							 &g_theDiskMgrApp.m_resmgr_app.m_thread_stopped, 
							 &g_theDiskMgrApp.m_resmgr_app.m_switch_on);
	g_theDiskMgrApp.m_resmgr_app.m_status_on = FALSE;
	g_theDiskMgrApp.m_index 	= 0;
	g_theDiskMgrApp.m_is_first_sample= TRUE;

	return TRUE;
}

// /proc/partitions DISKMGR_PARTITION_FILE
// /etc/mtab 	DISKMGR_MOUNTS_FILE
int ros_load_partition(ros_harddisk_info_t * const hd_info_ptr, int *hd_num , ros_partition_info_t * const part_info_ptr, int *part_num)
{
	int bRslt = 1;
	FILE *fp = NULL;
	char * line = NULL;
	size_t len = 0;
	int index = 0;
	int i = 0, j = 0, nSize = 0;
	char mount_fname[256] = "";
	struct mntent *mnt_ptr;

	struct statfs fs;

	fp = fopen(DISKMGR_PARTITION_FILE, "r");
	if(fp)
	{
		while(getline(&line, &len, fp) > 0)
		{
			if(index > 1)
			{
				for(i = 0; i < (int)len; i++)
				{
					if(' ' == *(line+i))
					{
					}
					else if('\t' == *(line+i))
					{
					}
					else if('\n' == *(line+i))
					{
					}
					else
					{
						break;
					}
				}
				sscanf((line+i), "%d %d %ld %s" , 
						&part_info_ptr[index-2].hd_id, 
						&part_info_ptr[index-2].partition_id, 
						&part_info_ptr[index-2].space_total, 
						mount_fname);

				part_info_ptr[index-2].is_mounted = 0;
				strcpy(part_info_ptr[index-2].mount_path, "/dev/");
				strcat(part_info_ptr[index-2].mount_path, mount_fname);
				part_info_ptr[index-2].space_used = 0;
				part_info_ptr[index-2].space_avail = 0;
			}
			index++;
		}
		if(line)
		{
			free(line);
			line = NULL;
		}
		if(index > 2)
		{
			nSize = index - 2;
			if(nSize > DISKMGR_PARTITION_MAX_NUM)
			{
				bRslt = 0;
			}
		}
	}
	for(index = nSize; index < DISKMGR_PARTITION_MAX_NUM; index++)
	{
		part_info_ptr[index].hd_id = 0; 
		part_info_ptr[index].partition_id = 0; 
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if(nSize > DISKMGR_PARTITION_MAX_NUM || 0 == bRslt)
	{
		bRslt = 0;
		return bRslt;
	}
	
	// Get the Partition Entry
	fp = setmntent(DISKMGR_MOUNTS_FILE, "r");
	if(fp)
	{
		while ((mnt_ptr = getmntent(fp)) != NULL) 
		{
			if (NULL == strcasestr(mnt_ptr->mnt_fsname, "/dev/"))
			{
				continue;
			}
			for(i = 0; i < nSize; i++)
			{
				if(0 == strcasecmp(mnt_ptr->mnt_fsname, part_info_ptr[i].mount_path))
				{
					statfs(mnt_ptr->mnt_dir, &fs);
					strcpy(part_info_ptr[i].mount_dir, mnt_ptr->mnt_dir);
					part_info_ptr[i].space_total = fs.f_blocks;  				/* total data blocks in file system */
					part_info_ptr[i].space_used  = fs.f_blocks - fs.f_bfree;	/* other from free blocks in fs */
					part_info_ptr[i].space_avail = fs.f_bavail;   				/* free blocks avail to non-superuser */
					part_info_ptr[i].is_mounted  = 1;
					break;
				}
			}
		}
	}
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	// Generate the Hard disk info Entry
	// should sort part_info_ptr first!!! by hd_id , part_id
	{
		int j = 0;
		ros_partition_info_t part_info_tmp;
		for(i = 0; i < nSize; i++)
		{
			for(j = i + 1; j < nSize; j++)
			{
				if(part_info_ptr[i].hd_id > part_info_ptr[j].hd_id)
				{
					// doing swap
					part_info_tmp = part_info_ptr[i];
					part_info_ptr[i] = part_info_ptr[j];
					part_info_ptr[j] = part_info_tmp;
				}
			}
		}
	}
	index = 0;
	for(i = 0; i < nSize; i++) 		/* i for partition   --- index for hd */
	{
		for(j = 0; j < index; j++) 	/* i for partition, j for hd  --- index for hd size */
		{
			if(hd_info_ptr[j].hd_id == part_info_ptr[i].hd_id)
			{
				hd_info_ptr[j].num_partitions++;
				hd_info_ptr[j].space_total += part_info_ptr[i].space_total;	/* total data blocks in file system */
				hd_info_ptr[j].space_used  += part_info_ptr[i].space_used;	/* other from free blocks in fs */
			}
		}
		if(j == index)
		{
			if(index == DISKMGR_HDISK_MAX_NUM)
			{
				bRslt = 0;
				break;
			}
			hd_info_ptr[j].hd_id = part_info_ptr[i].hd_id;
			hd_info_ptr[j].num_partitions = 1;
			hd_info_ptr[j].space_total = part_info_ptr[i].space_total;	/* total data blocks in file system */
			hd_info_ptr[j].space_used  = part_info_ptr[i].space_used;	/* other from free blocks in fs */
			index++;
		}
	}
	*hd_num 	= index;
	*part_num 	= nSize;
	for(; index < DISKMGR_HDISK_MAX_NUM; index++)
	{
		hd_info_ptr[index].hd_id = 0; 
		hd_info_ptr[index].num_partitions = 0; 
	}
	return bRslt;
}

//  API: 
void reset_ros_sys_harddisk_info_app_t(ros_sys_harddisk_info_app_t * const ptr)
{
	ptr->tLoadTime 	= 0;
	ptr->hd_num 	= 0;
	ptr->part_num 	= 0;
}

//  API: 
BOOL ros_load_sys_hd_info(ros_sys_harddisk_info_app_t * const theApp)
{
	if((theApp->tLoadTime + 60*60) > time(NULL))
	{
		return TRUE;
	}
	return ros_load_partition(theApp->hd_info, &theApp->hd_num , theApp->part_info, &theApp->part_num);
}

//  API: 
ros_sys_harddisk_info_app_t g_theAPIDiskMgrApp;

//  API: Init API function
int ros_disk_mgr_init()
{
	reset_ros_sys_harddisk_info_app_t(&g_theAPIDiskMgrApp);
	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	
	return 0;
}

//  API: 
int ros_get_num_hd()
{
	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	return g_theAPIDiskMgrApp.hd_num;
}

//  API: 
int ros_get_num_partition(int hd)
{
	int i = 0;

	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	for(i = 0; i < g_theAPIDiskMgrApp.hd_num; i++)
	{
		if(g_theAPIDiskMgrApp.hd_info[i].hd_id == hd)
		{
			return g_theAPIDiskMgrApp.hd_info[i].num_partitions;
		}
	}

	return 0;
}

//  API: ros_get_harddisk_info
// 	return value: 0 means success
int ros_get_harddisk_info(int hd, struct ros_harddisk_info * const diskinfo)
{
	if(NULL == diskinfo)
	{
		return -eAosRc_NullPointer;
	}
	int i = 0;
	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	for(i = 0; i < g_theAPIDiskMgrApp.hd_num && i < DISKMGR_HDISK_MAX_NUM; i++)
	{
		if(g_theAPIDiskMgrApp.hd_info[i].hd_id == hd)
		{
			*diskinfo = g_theAPIDiskMgrApp.hd_info[i];
			break;
		}
	}

	if(i == g_theAPIDiskMgrApp.hd_num || i == DISKMGR_HDISK_MAX_NUM)
	{
		return -eRosRc_ResMgrDiskNotFound;
	}
	return 0; // return -eAosRc_Success;
}

//  API: ros_get_partition_info
// 	return value: 0 means success
int ros_get_partition_info(int hd, int partition, struct ros_partition_info * const p )
{
	if(NULL == p)
	{
		return -eAosRc_NullPointer;
	}
	int i = 0;
	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);

	for(i = 0; i < g_theAPIDiskMgrApp.part_num && i < DISKMGR_PARTITION_MAX_NUM; i++)
	{
		if(g_theAPIDiskMgrApp.part_info[i].hd_id == hd && g_theAPIDiskMgrApp.part_info[i].partition_id == partition)
		{
			*p = g_theAPIDiskMgrApp.part_info[i];
			break;
		}
	}

	if(i == g_theAPIDiskMgrApp.part_num || i == DISKMGR_PARTITION_MAX_NUM)
	{
		return -eRosRc_ResMgrPartitionNotFound;
	}
	return 0; // -eAosRc_Success
}

// API: ros_get_all_hard_disks
// Get all hard disks 
// return value: the number of retrieved harddisks
int ros_get_all_hard_disks(ros_harddisk_info_t * const d, int *num_hd)
{
	int hd_size = 0;
	int i = 0;

	hd_size = ros_get_num_hd();
	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	for(i = 0; i < hd_size; i++)
	{
		d[i] = g_theAPIDiskMgrApp.hd_info[i];
	}
	*num_hd = hd_size;
	return hd_size;
}

// API: ros_get_all_partitions
// Get all hard disks 
// return value: the number of retrieved partitions
int ros_get_all_partitions(struct ros_partition_info * const p, int *num_part)
{
	int i;

	ros_load_sys_hd_info(&g_theAPIDiskMgrApp);
	
	for(i = 0; i < g_theAPIDiskMgrApp.part_num && i < DISKMGR_PARTITION_MAX_NUM; i++)
	{
		p[i] = g_theAPIDiskMgrApp.part_info[i];
	}
	*num_part = g_theAPIDiskMgrApp.part_num;
	return g_theAPIDiskMgrApp.part_num;
}

char* showPartitionInfo(char * const display_container, const struct ros_partition_info * part_info_ptr, int nArrNum)
{
	display_container[0] = 0;
	int i = 0;
#ifdef _DEBUG_
	sprintf(display_container+strlen(display_container), "\nhd num  : %d\n",ros_get_num_hd());
	sprintf(display_container+strlen(display_container), "\npart num: %d\n",ros_get_num_partition(0));
#endif
	for(i = 0; i < nArrNum; i++)
	{
		sprintf(display_container+strlen(display_container), "\n=================\n");
		sprintf(display_container+strlen(display_container),
			"partition ID   : %d\n", int(part_info_ptr->partition_id));
		sprintf(display_container+strlen(display_container),
			"hard disk ID   : %d\n", int(part_info_ptr->hd_id));
		sprintf(display_container+strlen(display_container),
			"is mounted     : %s\n", (part_info_ptr->is_mounted?("TRUE"):("FALSE")));
		sprintf(display_container+strlen(display_container),
			"total space    : %ld\n", part_info_ptr->space_total);	/* total data blocks in file system */
		sprintf(display_container+strlen(display_container),
			"used space     : %ld\n", part_info_ptr->space_used);	/* other from free blocks in fs */
		sprintf(display_container+strlen(display_container),
			"available space: %ld\n", part_info_ptr->space_avail);	/* free blocks avail to non-superuser */
		sprintf(display_container+strlen(display_container),
			"mount path     : %s\n", part_info_ptr->mount_path);	/* mount from path */
		sprintf(display_container+strlen(display_container),
			"mount dir      : %s\n", part_info_ptr->mount_dir);		/* mount to dir */
	}
	return display_container;
}

char* showHarddiskInfo(char * const display_container, const struct ros_harddisk_info * hd_info_ptr, const int nArrNum)
{
	display_container[0] = 0;
#ifdef _DEBUG_
	sprintf(display_container+strlen(display_container), "\nhd num  : %d\n",ros_get_num_hd());
	sprintf(display_container+strlen(display_container), "\npart num: %d\n",ros_get_num_partition(0));
	sprintf(display_container+strlen(display_container), "\ninput para: %d\n",nArrNum);
#endif
	int i = 0;
	for(i = 0; i < nArrNum; i++)
	{
		sprintf(display_container+strlen(display_container), "\n=================\n");
		sprintf(display_container+strlen(display_container),
			"hard disk ID   : %d\n", int(hd_info_ptr[i].hd_id));
		sprintf(display_container+strlen(display_container),
			"partition num  : %d\n", int(hd_info_ptr[i].num_partitions));
		sprintf(display_container+strlen(display_container),
			"total space    : %ld\n", hd_info_ptr[i].space_total);	/* total data blocks in file system */
		sprintf(display_container+strlen(display_container),
			"used space     : %ld\n", hd_info_ptr[i].space_used);	/* other from free blocks in fs */
	}

	return display_container;
}

BOOL showHarddiskInfoAPI(char * const display_container, const struct ros_harddisk_info * hd_info_ptr, const int nArrNum)
{
	display_container[0] = 0;
#ifdef _DEBUG_
	sprintf(display_container+strlen(display_container), "\nhd num  : %d\n",ros_get_num_hd());
	sprintf(display_container+strlen(display_container), "\npart num: %d\n",ros_get_num_partition(0));
	sprintf(display_container+strlen(display_container), "\ninput para: %d\n",nArrNum);
#endif
	int i = 0;
	for(i = 0; i < nArrNum; i++)
	{
		sprintf(display_container+strlen(display_container),
			" %d\n", int(hd_info_ptr[i].hd_id));
		sprintf(display_container+strlen(display_container),
			" %d\n", int(hd_info_ptr[i].num_partitions));
		sprintf(display_container+strlen(display_container),
			" %ld\n", hd_info_ptr[i].space_total);	/* total data blocks in file system */
		sprintf(display_container+strlen(display_container),
			" %ld\n", hd_info_ptr[i].space_used);	/* other from free blocks in fs */
	}

	return TRUE;
}
