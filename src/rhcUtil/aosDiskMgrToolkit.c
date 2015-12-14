////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrToolkit.c
// Description:
//   
//
// Modification History:
// 2007-02-13 Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "aosDiskMgrToolkit.h"
#include "aosResMgrToolkit.h"

#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/utsname.h>
// qsort
#include <stdlib.h>

// Linux programmer
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//===========================================================================================================
// Tool functions for Disk Manager 
//===========================================================================================================
// load Type Info from the data line
BOOL getTypeEntryData(struct TypeEntry * arrTypeInfo, int *nSizePtr, time_t *rcd_time_ptr, const char *line)
{
	int nRslt = 0;
	char *str1, *token = NULL;
	char *saveptr = NULL;
	char strLine[RESMGR_RCD_LINE_MAX];
	int i = 0, nSize = 0;
	time_t tRcdTime = 0;

	// Skip the Partition Record
	if(!line)
	{
		return FALSE;
	}
	strncpy(strLine, line, RESMGR_RCD_LINE_MAX);
	strLine[RESMGR_RCD_LINE_MAX - 1] = 0;
	nRslt = sscanf(strLine, "%lu %d %*s", &tRcdTime, &nSize);
	if(nRslt <= 0)
	{
		return 0;
	}
	nSize = nSize*6 + 3;
	for(i = 0, str1 = strLine; i < nSize; i++, str1 = NULL)
	{
		token = strtok_r(str1, " \n", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return 0;
		}
	}

	// Skip the Disk Record
	nRslt = sscanf(token, "%d", &nSize);
	nSize = nSize*5 + 1;
	if(nRslt <= 0)
	{
		return 0;
	}
	for(i = 0; i < nSize; i++)
	{
		token = strtok_r(NULL, " \n", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return 0;
		}
	}

 	// Skip the User Record
	nRslt = sscanf(token, "%d %*s", &nSize);
	nSize = nSize*4 + 1;
	if(nRslt <= 0)
	{
		return 0;
	}
	for(i = 0; i < nSize; i++)
	{
		token = strtok_r(NULL, " \n", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return 0;
		}
	}

	// Load the Type Record
	nRslt = sscanf(token, "%d %*s", &nSize);
	if(nRslt <= 0)
	{
		return 0;
	}
	for(i = 0; i < nSize; i++)
	{
		token = strtok_r(NULL, " \n", &saveptr);
		if (NULL == token)
		{
			break;
		}
		strncpy(arrTypeInfo[i].type, token, DISKMGR_MAX_TYPE_LEN);
		if(0 == strlen(arrTypeInfo[i].type))
		{
			nSize --;
			break;
		}
		arrTypeInfo[i].type[15] = 0;
		token = strtok_r(NULL, " \n", &saveptr);
		if (NULL == token)
		{
			nSize --;
			break;
		}
		arrTypeInfo[i].size = atoi(token);
	}
	*nSizePtr = nSize;
	*rcd_time_ptr = tRcdTime;

	if(nRslt <= 0 || (EOF == nRslt))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//  Load the type result 
BOOL loadTypeInfo(struct TypeEntry * arrTypeInfo, int *nArrSizePtr, time_t *rcd_time_ptr, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	char *zLine = NULL;
	char zLinePre[RESMGR_RCD_LINE_MAX];
	int bRslt = TRUE;
	int i = 0;
	(*nArrSizePtr) = 0;
	// open files
	fp1 = fopen(zFilePath1, "r");
	if (fp1 == NULL)
	{
		// return FALSE;
	}
	fp2 = fopen(zFilePath2, "r");
	if (fp2 == NULL)
	{
		// return FALSE;
	}
	if(!fp1 && !fp2)
	{
		return FALSE;
	}
	i = 0;
	const time_t tLineTime = time(NULL);

	loadResSnapDataLineLast(fp1, fp2, tLineTime, &zLine);
	strcpy(zLinePre, zLine);
	if(fp1)
	{
		fclose(fp1);
		fp1 = NULL;
	}
	if(fp2)
	{
		fclose(fp2);
		fp2 = NULL;
	}
//	for(i = 0; i < nArrSize && bRslt; i++)
//	{
		bRslt &= getTypeEntryData(arrTypeInfo, nArrSizePtr, rcd_time_ptr, zLinePre);
//	}
	if(zLine)
	{
		free(zLine);
		zLine = NULL;
	}
	return bRslt;
}

//  Show the type result 
BOOL showTypeInfo(char * const local, const struct TypeEntry *arrTypeInfo, const int nArrSize, long nRcdTime, const int nBufSize)
{
	//  Show the disk usage rate of every event
	int i = 0; 
	struct tm *pTm;
	char zDateTime[20];

	pTm = localtime(&nRcdTime);
	strftime(zDateTime, sizeof(zDateTime), RESMGR_TIME_FMT,pTm);
	sprintf(local+strlen(local),
		"\nRecord date time: %s \n", zDateTime);
	sprintf(local+strlen(local),
		"\n\t file type | disk size\n");
	for(i = 0; i < nArrSize; i++)
	{
		sprintf(local+strlen(local),"  %16s |", arrTypeInfo[i].type);
		sprintf(local+strlen(local),"%16d\n", arrTypeInfo[i].size);
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//  Show the type result 
BOOL showTypeInfoAPI(char * const local, const struct TypeEntry *arrTypeInfo, const int nArrSize, long nRcdTime, const int nBufSize)
{
	//  Show the disk usage rate of every event
	int i = 0; 
	for(i = 0; i < nArrSize; i++)
	{
		sprintf(local+strlen(local),"  %16s  ", arrTypeInfo[i].type);
		sprintf(local+strlen(local),"%16d\n", arrTypeInfo[i].size);
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}


//=================================================================
//	new disk manager begin
//=================================================================
// load Type Info from the data line
BOOL aos_diskmgr_load_type_entry_data(aos_diskmgr_type_rslt_t * arrTypeInfo, int *nSizePtr, time_t *rcd_time_ptr, const char *line)
{
	int nRslt = 0;
	char *str1, *token = NULL;
	char *saveptr = NULL;
	char strLine[RESMGR_RCD_LINE_MAX];
	int i = 0, nSize = 0;
	time_t tRcdTime = 0;

	// Skip the Partition Record
	if(!line)
	{
		return FALSE;
	}
	strcpy(strLine, line);
	nRslt = sscanf(strLine, "%lu %d %*s", &tRcdTime, &nSize);
	if(nRslt <= 0)
	{
		return FALSE;
	}
	nSize = nSize*6 + 3;
	for(i = 0, str1 = strLine; i < nSize; i++, str1 = NULL)
	{
		token = strtok_r(str1, " \n", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return FALSE;
		}
	}

	// Skip the Disk Record
	nRslt = sscanf(token, "%d", &nSize);
	nSize = nSize*5 + 1;
	if(nRslt <= 0)
	{
		return FALSE;
	}
	for(i = 0; i < nSize; i++)
	{
		token = strtok_r(NULL, " \n", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return FALSE;
		}
	}

 	// Skip the User Record
	nRslt = sscanf(token, "%d %*s", &nSize);
	nSize = nSize*4 + 1;
	if(nRslt <= 0)
	{
		return 0;
	}
	for(i = 0; i < nSize; i++)
	{
		token = strtok_r(NULL, " \n", &saveptr);
		if (token == NULL)
		{
			// error!!!!!
			return FALSE;
		}
	}

	// Load the Type Record
	nRslt = sscanf(token, "%d %*s", &nSize);
	if(nRslt <= 0)
	{
		return 0;
	}
	for(i = 0; i < nSize; i++)
	{
		token = strtok_r(NULL, " \n", &saveptr);
		if (NULL == token)
		{
			break;
		}
		strncpy(arrTypeInfo[i].m_type, token, DISKMGR_MAX_TYPE_LEN);
		if(0 == strlen(arrTypeInfo[i].m_type))
		{
			nSize --;
			break;
		}
		arrTypeInfo[i].m_type[AOS_DISKMGR_MAX_FILE_EXT_LEN-1] = 0;
		token = strtok_r(NULL, " \n", &saveptr);
		if (NULL == token)
		{
			nSize --;
			break;
		}
		arrTypeInfo[i].m_type_used = atoi(token);
	}
	*nSizePtr = nSize;
	*rcd_time_ptr = tRcdTime;

	if(nRslt <= 0 || (EOF == nRslt))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

//  Load the type result 
BOOL aos_diskmgr_load_type_info(aos_diskmgr_type_rslt_t * arrTypeInfo, int *nArrSizePtr, time_t *rcd_time_ptr, const char *zFilePath1, const char *zFilePath2)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	char *zLine = NULL;
	char zLinePre[RESMGR_RCD_LINE_MAX];
	BOOL bRslt = TRUE;
	int i = 0;
	(*nArrSizePtr) = 0;
	// open files
	fp1 = fopen(zFilePath1, "r");
	fp2 = fopen(zFilePath2, "r");
	if(!fp1 && !fp2)
	{
		return FALSE;
	}
	i = 0;
	const time_t tLineTime = time(NULL);

	loadResSnapDataLineLast(fp1, fp2, tLineTime, &zLine);
	strcpy(zLinePre, zLine);
	if(fp1)
	{
		fclose(fp1);
		fp1 = NULL;
	}
	if(fp2)
	{
		fclose(fp2);
		fp2 = NULL;
	}
	bRslt &= aos_diskmgr_load_type_entry_data(arrTypeInfo, nArrSizePtr, rcd_time_ptr, zLinePre);
	if(zLine)
	{
		free(zLine);
		zLine = NULL;
	}
	return bRslt;
}


//  Show the type result 
BOOL aos_diskmgr_show_type_info(char * const local, const int nBufSize, const aos_diskmgr_type_rslt_t *arrTypeInfo, const int nArrSize, long nRcdTime)
{
	//  Show the disk usage rate of every event
	int i = 0; 
	struct tm *pTm;
	char zDateTime[20];

	pTm = localtime(&nRcdTime);
	strftime(zDateTime, sizeof(zDateTime), RESMGR_TIME_FMT,pTm);
	sprintf(local+strlen(local),
		"\nRecord date time: %s \n", zDateTime);
	sprintf(local+strlen(local),
		"\n\t file type | disk size\n");
	for(i = 0; i < nArrSize; i++)
	{
		sprintf(local+strlen(local),"  %16s |", arrTypeInfo[i].m_type);
		sprintf(local+strlen(local),"%16lld\n", arrTypeInfo[i].m_type_used);
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//  Show the type result 
BOOL aos_diskmgr_show_type_info_api(char * const local, const int nBufSize, const aos_diskmgr_type_rslt_t *arrTypeInfo, const int nArrSize, long nRcdTime)
{
	//  Show the disk usage rate of every event
	int i = 0; 
	for(i = 0; i < nArrSize; i++)
	{
		sprintf(local+strlen(local),"  %16s  ", arrTypeInfo[i].m_type);
		sprintf(local+strlen(local),"%16lld\n", arrTypeInfo[i].m_type_used);
		if(strlen(local) > (size_t)nBufSize)
		{
			return FALSE;
		}
	}
	return TRUE;
}

#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <mntent.h>
#include <pwd.h>

// The following file "/etc/passwd" is called user database file in system. 
// We can call system call "getpwent" to get the user information from this file. 
// Don't need to open this file directly.
#define _DISKMGR_SYS_USR_DB_FILE 		"/etc/passwd"
#define _DISKMGR_SYS_MOUNT_INFO_FILE 	"/etc/mtab"
#define _DISKMGR_SYS_PROC_PARTITIONS	"/proc/partitions"

/* Files */
#define _DISK_INFO_FILE_0 "/var/log/aos_disk_log.0"
#define _DISK_INFO_FILE_1 "/var/log/aos_disk_log.1"

#define _DISKMGR_TYPEFILE 		"/usr/local/.type"
#define _DISKMGR_TYPEFILE_TMP 	"/usr/local/.type.tmp"


// 		Reset the system partition status result
void aos_diskmgr_reset_part_info(aos_diskmgr_part_rslt_t * const part_info)
{
	part_info->m_part_dev[0]	= 0; /* ID of partition */
	part_info->m_mount[0] 		= 0; 
	part_info->m_usage_ratio 	= 0;
	part_info->m_total		 	= 0;	
	part_info->m_used		 	= 0;	
	part_info->m_avail			= 0;
	part_info->m_files_nr	 	= 0;
}

// 		Get the system partition status result
BOOL aos_diskmgr_get_part_info(aos_diskmgr_part_rslt_t * const part_info, 
							   const char * part_dev_path, 
							   const char * part_mount_path)
{
	if(!part_info || !part_mount_path)
	{
		return FALSE;
	}

	struct statfs fs;
	statfs(part_mount_path, &fs);
	strncpy(part_info->m_part_dev, part_dev_path, AOS_DISKMGR_MAX_PATH_LEN);
	strncpy(part_info->m_mount, part_mount_path,  AOS_DISKMGR_MAX_PATH_LEN);
	part_info->m_total 		=  fs.f_blocks * fs.f_bsize;
	part_info->m_used 		= (fs.f_blocks - fs.f_bfree) * fs.f_bsize;
	part_info->m_avail 		=  fs.f_bavail * fs.f_bsize;
	part_info->m_files_nr	=  fs.f_files - fs.f_ffree;
	part_info->m_usage_ratio= part_info->m_used / part_info->m_total;

	return TRUE;
}


/*
 * <linux/hdreg.h> defines HDIO_GETGEO and
 * struct hd_geometry {
 *      unsigned char heads;
 *      unsigned char sectors;
 *      unsigned short cylinders;
 *      unsigned long start;
 * };
 *
 * For large disks g.cylinders is truncated, so we use BLKGETSIZE.
 */
// Verify whether the device is a harddisk
BOOL is_probably_full_disk(char *dev_path) {
	struct hd_geometry geometry;
	int fd, i = 0;

	fd = open(dev_path, O_RDONLY);
	if (fd >= 0) 
	{
		i = ioctl(fd, HDIO_GETGEO, &geometry);
		close(fd);
	}
	return (fd >= 0 && i == 0 && geometry.start == 0);
}


// 		Reset the system hard disk status result
void aos_diskmgr_reset_hd_info(aos_diskmgr_hd_rslt_t * const hd_info)
{
	hd_info->m_hd_dev[0] 	= 0;	/* ID of hard disk */
	hd_info->m_usage_ratio	= 0;	/* lower than 1 */
	hd_info->m_total		= 0;	
	hd_info->m_used			= 0;
	hd_info->m_avail		= 0;
	hd_info->m_files_nr		= 0;
	hd_info->m_part_nr		= 0;

	// Reset all the partition information first
	for(int i = 0; i < AOS_DISKMGR_MAX_PART_IN_HDS; i++)
	{
		aos_diskmgr_reset_part_info(&hd_info->m_part[i]);
	}
}


// Initialize the system harddisk first
BOOL aos_diskmgr_init_hd_info(aos_diskmgr_hd_rslt_t * const hd_info, 
							  int * arr_size)
{
	int index;
	char line[128], ptname[128], devname[128];
	int ma, mi, sz;
	FILE * fp;

	// 1. Reset the system hard disk status result
	for(int i = 0; i < (*arr_size); i++)
	{
		aos_diskmgr_reset_hd_info(&hd_info[i]);
	}

	// 2. Initialize the hard disk information
	fp = fopen(_DISKMGR_SYS_PROC_PARTITIONS, "r");
	if (fp)
	{
		index = 0;
		while (fgets(line, sizeof(line), fp) != NULL) 
		{
			if(index >= (*arr_size))
			{
				return FALSE;
			}
			if (sscanf (line, " %d %d %d %[^\n ]",
				    &ma, &mi, &sz, ptname) != 4)
			{
				continue;
			}
			snprintf(devname, sizeof(devname), "/dev/%s", ptname);
			if (!is_probably_full_disk(devname))
			{
				continue;
			}
			strncpy(hd_info[index].m_hd_dev, devname, AOS_DISKMGR_MAX_PATH_LEN);
			index++;
		}
		fclose(fp);
	}
	else
	{
		return FALSE;
	}

	(*arr_size) = index;
	return TRUE;
}


// Get the system hard disk status result
BOOL aos_diskmgr_get_hd_info(aos_diskmgr_hd_rslt_t * const hd_info) 
{
	if(!hd_info)
	{
		return FALSE;
	}

	FILE *fp = NULL;
	int index, max_index = 0;
	int nPos;
	struct mntent * mnt;

	// 1. Get the Partition Entry
	fp = setmntent(_DISKMGR_SYS_MOUNT_INFO_FILE, "r");
	while ((mnt = getmntent(fp)) != NULL) 
	{
		if (NULL == strstr(mnt->mnt_fsname, hd_info->m_hd_dev))
			continue;
		if (strstr(mnt->mnt_dir, "/dev/") != NULL)
			continue;
		nPos = strlen(hd_info->m_hd_dev);
		index = atoi(&mnt->mnt_fsname[nPos]) - 1;
		aos_diskmgr_get_part_info(&hd_info->m_part[index], mnt->mnt_fsname, mnt->mnt_dir);
		max_index = RESMGR_MAXIMUM(index, max_index);
	}
	fclose(fp);

	// 2. Generalize the statistics result of the hard disk
	hd_info->m_part_nr = max_index + 1;
	for(int i = 0; i < hd_info->m_part_nr; i++)
	{
		hd_info->m_total 	+= hd_info->m_part[i].m_total;
		hd_info->m_used  	+= hd_info->m_part[i].m_used; 
		hd_info->m_avail 	+= hd_info->m_part[i].m_avail;
		hd_info->m_files_nr += hd_info->m_part[i].m_files_nr;
	}
	hd_info->m_usage_ratio = float(hd_info->m_used / hd_info->m_total);

	return TRUE;
}

// Get the system hard disk status result
BOOL aos_diskmgr_get_hd_info(aos_diskmgr_hd_rslt_t * const hd_info, 
							 const char * hd_dev_path) 
{
	char hd_dev[AOS_DISKMGR_MAX_PATH_LEN]; /* ID of hard disk */
	strncpy(hd_dev, 	hd_dev_path, 	AOS_DISKMGR_MAX_PATH_LEN);
	strncpy(hd_info->m_hd_dev, hd_dev, 	AOS_DISKMGR_MAX_PATH_LEN);

	return aos_diskmgr_get_hd_info(hd_info);
}


// Get the system file type status result
BOOL aos_diskmgr_init_type_info(aos_diskmgr_type_rslt_t * const type_info, 
								int *arr_size)
{
	FILE *fp = NULL;
	char readbuf[AOS_DISKMGR_MAX_FILE_EXT_LEN] = "";
	const int input_size = (*arr_size);
	int i = 0;

	fp = fopen(_DISKMGR_TYPEFILE, "r");
	if(fp)
	{
		for(i = 0; i < input_size; i++)
		{
			if(fgets(readbuf, sizeof(readbuf), fp) != NULL)
			{
				readbuf[AOS_DISKMGR_MAX_FILE_EXT_LEN - 1] = 0;
				if('\n' == readbuf[strlen(readbuf) - 1])
				{
					readbuf[strlen(readbuf) - 1] = 0;
				}
				strncpy(type_info[i].m_type, 
						readbuf, 
						AOS_DISKMGR_MAX_FILE_EXT_LEN);
				type_info[i].m_type_files_nr = 0;
			}
			else
			{
				break;
			}
		}
		while(fgets(readbuf, sizeof(readbuf), fp) != NULL)
		{
			i++;
		}
	}
	(*arr_size) = i;
	if(fp)
	{
		fclose(fp);
	}

	return ((i <= input_size)? TRUE:FALSE);
}


// sort user's array by user's uid
static int
cmp_usr_info(const void *p1, const void *p2)
{
	/* The actual arguments to this function are "pointers to
	pointers to char", but strcmp() arguments are "pointers
	to char", hence the following cast plus dereference */
	if(((aos_diskmgr_usr_rslt_t*)p1)->m_uid < ((aos_diskmgr_usr_rslt_t*)p2)->m_uid)
		return -1;
	else if(((aos_diskmgr_usr_rslt_t*)p1)->m_uid == ((aos_diskmgr_usr_rslt_t*)p2)->m_uid)
		return 0;
	else
		return 1;
}


// look up the uid item index
static int 
lookup_usr_info(const aos_diskmgr_usr_rslt_t *usr_info, 
				const int arr_size, const uid_t usr_uid, 
				const int nFirstPos, const int nLastPos)
{
	int i;
	if(nFirstPos >= nLastPos)
	{
		if(usr_info[nFirstPos].m_uid == usr_uid)
		{
			return nFirstPos;
		}
		else
		{
			return -1;
		}
	}

	i = (nFirstPos + nLastPos) / 2;
	if(usr_info[i].m_uid == usr_uid)
	{
		return i;
	}
	else if(usr_info[i].m_uid < usr_uid)
	{
		return lookup_usr_info(usr_info, arr_size, usr_uid, i+1, nLastPos);
	}
	else	//  if(usr_info[i].m_uid > usr_uid)
	{
		return lookup_usr_info(usr_info, arr_size, usr_uid, nFirstPos, i-1);
	}
}


//#include <sys/types.h>
//#include <pwd.h>
//// These apis are loading information from file "/etc/passwd"
//struct passwd *getpwent(void);
//void setpwent(void);
//void endpwent(void);
//
// Get the system usr space status result
// 		Get the system usr partition status result
BOOL aos_diskmgr_init_usr_info(aos_diskmgr_usr_rslt_t * const usr_info, int * arr_size)
{
	struct passwd * usr_info_ptr;
	const int input_size = (*arr_size);
	int i;

	setpwent();
	for(i = 0; i < input_size; i++)
	{
		usr_info_ptr = getpwent();
		if(usr_info_ptr)
		{
			usr_info[i].m_uid 		= usr_info_ptr->pw_uid;
			usr_info[i].m_gid 		= usr_info_ptr->pw_gid;
			usr_info[i].m_usr_used	= 0;
			usr_info[i].m_usr_files_nr= 0;
			strncpy(usr_info[i].m_uname, 
					usr_info_ptr->pw_name, 
					AOS_DISKMGR_MAX_USR_NAME_LEN);
			strncpy(usr_info[i].m_home_path, 
					usr_info_ptr->pw_dir, 
					AOS_DISKMGR_MAX_PATH_LEN);
		}
		else
		{
			break;
		}
	}
	while(NULL != (usr_info_ptr = getpwent()))
	{
		i++;
	}
	(*arr_size) = i;
	endpwent();
	
	// sort the user's array by uid
	qsort(usr_info, RESMGR_MINIMUM(i, input_size), 
			sizeof(aos_diskmgr_usr_rslt_t), cmp_usr_info);

	return ((i <= input_size)? TRUE:FALSE);
}


// Get the system status of 
// 		file type
// 		user information
BOOL aos_diskmgr_get_vfs_status(aos_diskmgr_type_rslt_t * const type_info, 
								const int type_arr_size, 
								aos_diskmgr_usr_rslt_t * const usr_info, 
								const int usr_arr_size,
								const char * curr_path, 
								const BOOL b_recursive)
{
	struct dirent *dirp;
	struct stat statbuf;
	char type[RESMGR_EXT_LEN] = "";
	DIR *dir = NULL;
	int i;
	off_t tmp_size;

	if(NULL == type_info || NULL == usr_info || NULL == curr_path)
	{
#ifdef _DEBUG_ALARM_PRINT
		printf("Input error ptrs: type_info[%p] usr_info[%p] curr_path[%p] TIME(%ld) FILE(%s)LINE(%d)\n", 
				type_info, usr_info, curr_path, time(NULL), __FILE__, __LINE__ );
#endif
#ifdef _DEBUG_
		aos_alarm( eAosMD_AosUtil, eAosAlarm_General,
				"Input error [%d]\n", errno);
#endif
		return FALSE;
	}

	// Validate whether curr_path is regular file or directory
	if (lstat(curr_path, &statbuf) < 0)
	{
#ifdef _DEBUG_
		aos_alarm( eAosMD_AosUtil, eAosAlarm_General,
				"lsstat error [%d]\n", errno);
#endif
		return FALSE;
	}

	if(S_ISLNK(statbuf.st_mode))
	{
		return TRUE;
	}
	else if(S_ISREG(statbuf.st_mode))
	{
		for (i = strlen(curr_path) - 1; i >= 0; i--)
		{
			if (curr_path[i] == '.') 
			{
				strncpy(type , curr_path + i + 1, RESMGR_EXT_LEN);
				type[RESMGR_EXT_LEN - 1] = 0;
				break;
			}
			if (curr_path[i] == '/')
				break;
		}

		tmp_size = statbuf.st_size / statbuf.st_nlink; // size / num_hard_link
		if(0 != type[0])
		{
			for (i = 0; i < type_arr_size; i++)
			{
				if (strcmp(type_info[i].m_type, type) == 0)
				{
					type_info[i].m_type_used += tmp_size;
					type_info[i].m_type_files_nr ++;
					break;
				}
			}
		}

//		for (i = 0; i < usr_arr_size; i++)
//		{
//			if (usr_info[i].m_uid == statbuf.st_uid)
//			{
//				usr_info[i].m_usr_used += tmp_size;
//				usr_info[i].m_usr_files_nr ++;
//				break;
//			}
//		}
		i = lookup_usr_info(usr_info, usr_arr_size, statbuf.st_uid, 0, usr_arr_size);
		if(i >= 0)
		{
			usr_info[i].m_usr_used += tmp_size;
			usr_info[i].m_usr_files_nr ++;
		}
	}
	else if(S_ISDIR(statbuf.st_mode) && b_recursive)
	{
		dir = opendir(curr_path);
		if (dir)
		{
			while(NULL != (dirp = readdir(dir)))
			{
				if (strcmp(dirp->d_name,".") != 0 
				 && strcmp(dirp->d_name,"..") != 0)
				{
					char path[256];
					strcpy(path, curr_path);
					if(path[strlen(curr_path) - 1] == '/')
					{
						snprintf(path,256,"%s%s",curr_path, dirp->d_name);
					}
					else
					{
						snprintf(path,256,"%s/%s",curr_path, dirp->d_name);
					}
//#ifdef _DEBUG_ALARM_PRINT
//				printf("curr_path[%s] path[%s] time(%ld) FILE(%s)LINE(%d)\n", curr_path, path, time(NULL), __FILE__, __LINE__ );
//#endif
					aos_diskmgr_get_vfs_status( type_info, type_arr_size, 
												usr_info, usr_arr_size, 
												path,
												b_recursive);
//#ifdef _DEBUG_ALARM_PRINT
//				printf("curr_path[%s] path[%s] time(%ld) FILE(%s)LINE(%d)\n", curr_path, path, time(NULL), __FILE__, __LINE__ );
//#endif
				}
			}
			closedir(dir);
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}


void aos_diskmgr_reset_stat_rslt(aos_diskmgr_stat_rslt_t * const sys_info)
{
	sys_info->m_sec_index			= 0;
	sys_info->m_total_usage_ratio	= 0;
	sys_info->m_total	 			= 0;
	sys_info->m_total_used 			= 0;
	sys_info->m_total_avail			= 0;
	sys_info->m_partitions_nr		= 0;
	sys_info->m_hds_nr				= 0;
	sys_info->m_usr_nr				= 0;
	sys_info->m_type_nr				= 0;
}

// Get the system current status information
BOOL aos_diskmgr_get_status_info(aos_diskmgr_stat_rslt_t * const sys_info)
{
	int i;

	aos_diskmgr_reset_stat_rslt(sys_info);
	sys_info->m_sec_index = time(NULL);

	// Get the system hard disk status result
	sys_info->m_hds_nr = AOS_DISKMGR_MAX_HDS;
	if(!aos_diskmgr_init_hd_info(sys_info->m_hds_rslt, &sys_info->m_hds_nr))
	{
		return FALSE;
	}

	for(i = 0; i < sys_info->m_hds_nr; i++)
	{
		aos_diskmgr_get_hd_info(&sys_info->m_hds_rslt[i]);
		sys_info->m_total	 			+= sys_info->m_hds_rslt[i].m_total;
		sys_info->m_total_used 			+= sys_info->m_hds_rslt[i].m_used;
		sys_info->m_total_avail			+= sys_info->m_hds_rslt[i].m_used;
		sys_info->m_partitions_nr		+= sys_info->m_hds_rslt[i].m_part_nr;
	}

	sys_info->m_total_usage_ratio = float(sys_info->m_total_used / sys_info->m_total);

	// Get the system file type status result
	sys_info->m_type_nr = AOS_DISKMGR_MAX_TYPE_NR;
	aos_diskmgr_init_type_info(  sys_info->m_type_rslt, 
								&sys_info->m_type_nr);

	// Get the system usr space status result
	sys_info->m_usr_nr = AOS_DISKMGR_MAX_USR_NR;
	aos_diskmgr_init_usr_info(   sys_info->m_usr_rslt, 
								&sys_info->m_usr_nr);

#ifdef _DEBUG_ALARM_PRINT
				printf("TIME(%ld) FILE(%s)LINE(%d)\n", time(NULL), __FILE__, __LINE__ );
#endif
	// Get the system status of 
	// 		file type
	// 		user information
	aos_diskmgr_get_vfs_status( sys_info->m_type_rslt, 
								sys_info->m_type_nr, 
								sys_info->m_usr_rslt, 
								sys_info->m_usr_nr,
								"/", 
								TRUE);

#ifdef _DEBUG_ALARM_PRINT
				printf("TIME(%ld) FILE(%s)LINE(%d)\n", time(NULL), __FILE__, __LINE__ );
#endif
	return TRUE;
}

//=================================================================
//	new disk manager end
//=================================================================


//=================================================================
// Tool functions for showing Partition of the harddisk
//=================================================================

// API 8. 
int aos_diskmgr_get_num_hd(int * hd_num)
{
	return 0;
}

// API 9. 
int aos_diskmgr_get_num_partition(int * part_num, const char * dev_path)
{
	return 0;
}

// API 10. 
int aos_diskmgr_get_partition_info(
				aos_diskmgr_part_rslt_t * const part_info, 
				const char * dev_path)
{
	return 0;
}

// API 11. 
int aos_diskmgr_get_harddisk_info(aos_diskmgr_hd_rslt_t * const d, 
								   const char * dev_path)
{
	return 0;
}

// API 12. 
int aos_diskmgr_get_all_hard_disks(aos_diskmgr_hd_rslt_t * const arr_hd, 
								   int *num_hd)
{
	return 0;
}


// API 13. 
int aos_diskmgr_format_hard_disk(aos_diskmgr_hd_rslt_t * const harddisk_info, 
								 const char* hd_dev, 
								 const aos_format_harddisk_input_t* set_info)
{
	return 0;
}
//=================================================================
// Tool functions for showing Partition of the harddisk
//=================================================================
