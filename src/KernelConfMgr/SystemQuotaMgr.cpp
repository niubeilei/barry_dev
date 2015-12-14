////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemQuotaMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Global.h"

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
//#include "XmlParser/XmlItem.h"

#include "rhcUtil/aosResMgrToolkit.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

static int modify_fstab_file(int status, char *partition)
{
	char mount_partition[64], filesystem[64], mount_point[64], readbuf[256];
	FILE *fp, *tfp;
	int temp[3];

	fp = fopen("/etc/fstab", "r");
	tfp = fopen("/tmp/fstab_tmp", "w+");

	if (fp == NULL || tfp == NULL)
		return -1;
	
	memset(readbuf, 0, sizeof(readbuf));
	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		memset(mount_partition, 0, sizeof(mount_partition));
		memset(mount_point, 0, sizeof(mount_point));
		memset(filesystem, 0, sizeof(filesystem));
		sscanf(readbuf, "%s %s %s %*s %d %d", mount_partition, mount_point, filesystem, &temp[0], &temp[1]); 

		if (strcmp(partition, mount_point))
		{
			if (fputs(readbuf, tfp) < 0)
				return -1;
			continue;
		}

		memset(readbuf, 0, sizeof(readbuf));
		if (status)
		{
			sprintf(readbuf, "%s\t%s\t%s\tdefaults,grpquota,usrquota\t\t%d %d\n",\
					mount_partition, mount_point, filesystem,\
					temp[0], temp[1]);
		}
		else
		{
			sprintf(readbuf, "%s\t%s\t%s\tdefaults\t\t%d %d\n",\
					mount_partition, mount_point, filesystem,\
					temp[0], temp[1]);
		}

		if (fputs(readbuf, tfp) < 0)
			return -1;
		memset(readbuf, 0, sizeof(readbuf));
	}
	fclose(fp);
	fclose(tfp);
	return 0;
}

static int remove_partition_limit(char *partition)
{
	char *path = "/usr/local/rhc/config/quota.conf";
	char *tmpPath = "/tmp/.quota_tmp"; 
	char readbuf[256], cmdline[128], read_part[128];
	FILE *fp, *tfp;
	memset(readbuf, 0, sizeof(readbuf));
	memset(cmdline, 0, sizeof(cmdline));
	memset(read_part, 0, sizeof(read_part));

	fp = fopen(path, "a+"); 
	if (NULL == fp)
		return -1;

	tfp = fopen("/tmp/.quota_tmp", "w+");
	if (NULL == tfp)
	{
		fclose(fp);
		return -1;
	}

	while (fgets(readbuf, sizeof(readbuf)-1, fp))
	{
		sscanf(readbuf, "%*d %s %*s %*s", read_part);

		if (strcmp(read_part, partition))
		{
			fputs(readbuf, tfp);
		}

		memset(read_part, 0, sizeof(read_part));
		memset(readbuf, 0, sizeof(readbuf));
	}

	fclose(fp);
	fclose(tfp);
	sprintf(cmdline, "mv %s %s", tmpPath, path);
	system(cmdline);

	return 0;
}

static int quota_status_on(char* partition)
{	
	OmnString systemCmd;
	OmnString rslt;

	if (modify_fstab_file(1, partition) < 0)
		return -1;

	systemCmd = "";
	systemCmd << "mv -f /tmp/fstab_tmp /etc/fstab";	
	OmnCliSysCmd::doShell(systemCmd, rslt);

	systemCmd = "";
	systemCmd << "mount -o remount " << partition;	
	OmnCliSysCmd::doShell(systemCmd, rslt);

	systemCmd = "";
	systemCmd << "quotacheck -cug -m " << partition;	
	OmnCliSysCmd::doShell(systemCmd, rslt);
	
	systemCmd = "";
	systemCmd << "quotaon -ug " << partition;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}

static int check_size(char* str_size, char* size)
{
	char buf_size[64];
	int index, i;

	memset(buf_size, 0, sizeof(buf_size));
	strncpy(buf_size, str_size, sizeof(buf_size));
	index = strlen(buf_size);

	if (buf_size[index-1] != 'M' && buf_size[index-1] != 'm' 
		&& buf_size[index-1] != 'K' && buf_size[index-1] != 'k')
		return -1;

	for (i=0; i<index-1; i++)
	{
		if (buf_size[i] > '9' || buf_size[i] < '0')
			return -1;
	}

	if (buf_size[index-1] == 'M' || buf_size[index-1] == 'm')
	{
		buf_size[index-1] = '\0';
		if (atoi(buf_size) > 1024 * 40)
			return -1;
		sprintf(size, "%d", atoi(buf_size) * 1024);
	} 
	else 
	{
		buf_size[index-1] = '\0';
		if (atoi(buf_size) > 1024 * 1024 * 40)
			return -1;
		sprintf(size, "%d", atoi(buf_size));
	}

	return 0;
}


static int quota_status_off(char* partition)
{
	OmnString systemCmd;
	OmnString rslt;
	char path_name[64];
	memset(path_name, 0, sizeof(path_name));

	if (modify_fstab_file(0, partition) < 0)
		return -eAosRc_FailedModifyFstab;

	if (strcmp(partition, "/"))
		strncpy(path_name, partition, sizeof(path_name));

	systemCmd = "";
	systemCmd << "mv -f /tmp/fstab_tmp /etc/fstab";	
	OmnCliSysCmd::doShell(systemCmd, rslt);

	systemCmd = "";
	systemCmd << "quotaoff -ug " << partition;	
	OmnCliSysCmd::doShell(systemCmd, rslt);

	systemCmd = "";
	systemCmd << "mount -o remount " << partition;	
	OmnCliSysCmd::doShell(systemCmd, rslt);

	systemCmd = "";
	systemCmd << "rm -rf " << path_name << "/quota.user " << path_name << "/aquota.user";
	OmnCliSysCmd::doShell(systemCmd, rslt);

	systemCmd = "";
	systemCmd << "rm -rf " << path_name << "/quota.group " << path_name << "/aquota.group";
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}

int check_partition_status(char *status, char *partition, OmnString &contents)
{
	FILE *fp;
	int isFound;
	struct stat st_buf;
	char readbuf[256], mount_partition[64], mount_point[64], mount_type[64];
	memset(readbuf, 0, sizeof(readbuf));
	isFound = false;

	fp = fopen("/etc/fstab", "r");	
	while (NULL != fgets(readbuf, sizeof(readbuf), fp))
	{
		if (strstr(readbuf, "#"))
		{
			memset(readbuf, 0, sizeof(readbuf));
			continue;
		}
		memset(mount_partition, 0, sizeof(mount_partition));
		memset(mount_point, 0, sizeof(mount_point));
		memset(mount_type, 0, sizeof(mount_type));
		sscanf(readbuf, "%s %s %*s %s %*d %*d", mount_partition, mount_point, mount_type);

		if (strcmp(mount_point, partition))
		{
			memset(readbuf, 0, sizeof(readbuf));
			continue;
		}

		isFound = true;
		if (!strcmp(status, "on"))
		{
			if (!strcmp(mount_type, "defaults,grpquota,usrquota"))
			{
				contents << "the status is already on\n";
				return -eAosRc_StatusAlready;
			}
		}
		else if (!strcmp(status, "off"))
		{
			if (strcmp(mount_type, "defaults,grpquota,usrquota"))
			{
				contents << "the status is already off\n";
				return -eAosRc_StatusAlready;
			}
		}
		else
		{
			contents << "The Invald status\n"; 
			return -eAosRc_InvaldStatus;
		}
		
		stat(partition, &st_buf);
		if (!S_ISDIR(st_buf.st_mode))
		{
			contents << partition << "is not a really directory\n";
			return -eAosRc_InvalidPartition;
		}

		stat(mount_partition, &st_buf);
		if (!S_ISBLK(st_buf.st_mode))
		{
			contents << "The " << partition << " is not a realy mount point\n";
			return -eAosRc_InvalidPartition;
		}
			
		memset(readbuf, 0, sizeof(readbuf));
	}

	if (!isFound)
	{
		contents << "The " << partition << " is not a realy mount point\n";
		return -eAosRc_InvalidPartition;
	}
	fclose(fp);
	return 0;
}

int	diskMgrQuotaStatus(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int ret;
	char *status, *partition;
	OmnString rslt;

	/* here real start */
	status = parms->mStrings[0];
	partition = parms->mStrings[1];
	rslt = "";

	ret = check_partition_status(status, partition, rslt);
	if (ret < 0)
	{
		sprintf(errmsg, "Usage:%s", rslt.data());
		errmsg[errlen-1] = 0;
		return ret;
	}
	
	if (!strcmp(status, "on"))
	{
		ret = quota_status_on(partition);
	}
	else
	{
		ret = quota_status_off(partition);
		remove_partition_limit(partition);
	}

	if (ret < 0)
	{
		sprintf(errmsg, "Usage:Failed to set status %s", status);
		errmsg[errlen-1] = 0;
		return -eAosRc_FailedModifyFstab;
	}

	/* here real end */
	*optlen = index;
    return 0;
}

static int quota_status_show(OmnString &contents)
{
	FILE *fp;
	char readbuf[256], mount_partition[64], mount_point[64], rslt[256];

	memset(readbuf, 0, sizeof(readbuf));
	memset(mount_partition, 0, sizeof(mount_partition));
	memset(mount_point, 0, sizeof(mount_point));
	memset(rslt, 0, sizeof(rslt));

	fp = fopen("/etc/fstab", "r");
	if (NULL == fp)
		return -1;
		
	sprintf(rslt, "<file system>\t\t<mount point>\n");
	contents << rslt;

	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		if (strstr(readbuf, "defaults,grpquota,usrquota"))
		{
			sscanf(readbuf, "%s %s %*s %*s %*d %*d", mount_partition, mount_point); 
			sprintf(rslt, "%10s\t\t%10s\n", mount_partition, mount_point);
			contents << rslt;
		}
		memset(readbuf, 0, sizeof(readbuf));
	}

	fclose(fp);
	return 0;
}

int	diskMgrQuotaStatusShow(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int ret;
	OmnString contents;
	char* buf = CliUtil_getBuff(data);

	/* here real start */
	ret = quota_status_show(contents);
	if (ret < 0)
	{
		sprintf(errmsg, "Failed open config file\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_FailedOpenConfigFile;
	}
		
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;

    return 0;
}

int	diskMgrQuotaList(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents;
	/* here real start */

	OmnString systemCmd;
	systemCmd << "repquota -avug";
	OmnCliSysCmd::doShell(systemCmd, contents);
	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;

    return 0;
}

static int status_is_on(char *partition)
{
	FILE *fp;
	bool isOn;
	char readbuf[256], mount_point[64], read_part[64];
	memset(readbuf, 0, sizeof(readbuf));
	memset(mount_point, 0, sizeof(mount_point));
	memset(read_part, 0, sizeof(read_part));
	isOn = false;
	
	fp = fopen("/etc/fstab", "r");
	if (NULL == fp)
		return -1;

	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		sscanf(readbuf, "%*s %s %*s %s %*d %*d", mount_point, read_part);
		if (!strcmp(mount_point, partition) && strstr(read_part, "grpquota,usrquota"))
			isOn = true;
		memset(readbuf, 0, sizeof(readbuf));
		memset(mount_point, 0, sizeof(mount_point));
		memset(read_part, 0, sizeof(read_part));
	}
	fclose(fp);
	
	if (!isOn)
		return -1;
	return 1;
}

static int get_gid_of_directory( const OmnString &dir )
{
	int ret;
	struct stat st;
	ret = stat(dir.data(), &st);
	if(ret<0)
		return ret;

	return st.st_gid;
}

static int gid_is_used( const char * dir_name, unsigned int gid )
{
	struct stat st;
	int ret;	
	int used;

	ret = stat(dir_name,&st);
	if(ret<0)
	{
		used = false;
	}
	else if(gid==st.st_gid)
	{
		used = true;
	}
	else if( (st.st_mode&S_IFDIR) && !(st.st_mode&S_IFLNK) )
	{
		// is a directory
		struct dirent * ptr;
		DIR * dir=opendir(dir_name);
		used = false;
		if(dir)
		{
			while((ptr=readdir(dir)))
			{
//				char tmp[1024];
				if (0==strcmp(ptr->d_name,".") || 0==strcmp(ptr->d_name,".."))
					continue;
				chdir(dir_name);
				if (gid_is_used(ptr->d_name,gid))
				{
					used = true;
					break;
				}
			//	chdir(
	
			}
		}
		closedir(dir);
	}
	else
	{
		// is not a directory
		used = false;
	}
//	std::cout<<dir_name<<" "<<gid<<" "<<used<<std::endl;
	return used;
}

static int get_unused_gid( const OmnString & mp )
{
	int gid=60000;

	while(gid<=65000 && gid_is_used(mp.data(),gid))
	{
		gid++;
	}
	if(gid<=65000)
		return gid;
	else
		return -1;
}

static int save_limit(int type, char* partition, char* user, char* size)
{
	char *path = "/usr/local/rhc/config/quota.conf";
	char *tmpPath = "/tmp/.quota_tmp"; 
	char readbuf[128], cmdline[64];
	char read_part[64], read_user[64];
	int read_type;
	FILE *fp, *tfp;
	bool isPut = false;
	memset(readbuf, 0, sizeof(readbuf));
	memset(cmdline, 0, sizeof(cmdline));
	memset(read_part, 0, sizeof(read_part));
	memset(read_user, 0, sizeof(read_user));

	fp = fopen(path, "a+"); 
	if (NULL == fp)
		return -1;

	tfp = fopen("/tmp/.quota_tmp", "w+");
	if (NULL == tfp)
	{
		fclose(fp);
		return -1;
	}

	sprintf(cmdline, "%d %s %s %s\n", type, partition, user, size);

	while (fgets(readbuf, sizeof(readbuf)-1, fp))
	{
		sscanf(readbuf, "%d %s %s %*s", &read_type, read_part, read_user);

		if ((read_type == type) && (!strcmp(read_part, partition)) && (!strcmp(read_user, user)))
		{
			fputs(cmdline, tfp);
			isPut = true;
		}
		else 
		{
			fputs(readbuf, tfp);
		}

		memset(read_part, 0, sizeof(read_part));
		memset(read_user, 0, sizeof(read_user));
		memset(readbuf, 0, sizeof(readbuf));
	}

	if (!isPut)
		fputs(cmdline, tfp);

	fclose(fp);
	fclose(tfp);

	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "mv %s %s", tmpPath, path);
	system(cmdline);

	return 0;
}

static int remove_limit(int type, char *partition, char *user)
{
	char *path = "/usr/local/rhc/config/quota.conf";
	char *tmpPath = "/tmp/.quota_tmp"; 
	char readbuf[128], cmdline[64];
	char read_part[64], read_user[64];
	int read_type;
	FILE *fp, *tfp;
	bool isPut = false;
	memset(readbuf, 0, sizeof(readbuf));
	memset(cmdline, 0, sizeof(cmdline));
	memset(read_part, 0, sizeof(read_part));
	memset(read_user, 0, sizeof(read_user));

	fp = fopen(path, "a+"); 
	if (NULL == fp)
		return -1;

	tfp = fopen("/tmp/.quota_tmp", "w+");
	if (NULL == tfp)
	{
		fclose(fp);
		return -1;
	}

	while (fgets(readbuf, sizeof(readbuf)-1, fp))
	{
		sscanf(readbuf, "%d %s %s %*s", &read_type, read_part, read_user);

		if ((read_type == type) && (!strcmp(read_part, partition)) && (!strcmp(read_user, user)))
		{
			isPut = true;
		}
		else 
		{
			fputs(readbuf, tfp);
		}

		memset(read_part, 0, sizeof(read_part));
		memset(read_user, 0, sizeof(read_user));
		memset(readbuf, 0, sizeof(readbuf));
	}

	if (!isPut)
		return -1;

	fclose(fp);
	fclose(tfp);
	memset(cmdline, 0, sizeof(cmdline));
	sprintf(cmdline, "mv %s %s", tmpPath, path);
	system(cmdline);

	return 0;
}

static int save_quota_status(OmnString &contents)
{
	char mount_point[64], readbuf[256];
	FILE *fp;

	memset(mount_point, 0, sizeof(mount_point));
	memset(readbuf, 0, sizeof(readbuf));

	fp = fopen("/etc/fstab", "r");
	if (fp == NULL)
	{
		cout << "open the file error\n" << endl;
		return -1;
	}
		
	while (fgets(readbuf, sizeof(readbuf), fp))
	{

		if (!strstr(readbuf, "defaults,grpquota,usrquota"))
			continue;

		sscanf(readbuf, "%*s %s %*s %*s %*d %*d", mount_point); 

		contents << "<Cmd>";
		contents << "disk mgr quota status " << "on " <<  mount_point;
		contents << "</Cmd>\n";
	}
	fclose(fp);

	return 0;
}

static int save_quota_limit(OmnString &contents)
{
	int type;
	FILE *fp;
	char readbuf[256], partition[64], user[64], size[64];

	memset(readbuf, 0, sizeof(readbuf));
	memset(partition, 0, sizeof(partition));
	memset(user, 0, sizeof(user));
	memset(size, 0, sizeof(size));

	fp = fopen("/usr/local/rhc/config/quota.conf", "r");
	if (fp == NULL)
		return -1;

	memset(readbuf, 0, sizeof(readbuf));

	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		sscanf(readbuf, "%d %s %s %s", &type, partition, user, size);
		if (type)
		{
			contents << "<Cmd>";
			contents << "disk mgr quota add dir " << partition << " " << user << " " << size; 
			contents << "</Cmd>\n";
		}
		else
		{
			contents << "<Cmd>";
			contents << "disk mgr quota add user " << partition << " " << user << " " << size; 
			contents << "</Cmd>\n";
		}
	}

	fclose(fp);
	return 0;
}

static int check_directory(char *mount_point, char *directory)
{
	struct stat fs;
	int p_len, d_len;
	p_len = strlen(mount_point);
	d_len = strlen(directory);

	if (strncmp(mount_point, directory, p_len)) 
		return -1;
	
	if (p_len == d_len)
		return -1;
	
	if (p_len == 1 && directory[d_len-1] == '/')
		return -1;

	if (p_len != 1 && (directory[p_len] != '/' || directory[d_len-1] == '/'))
		return -1;

	if (strstr(directory, "/../"))
		return -1;

	if ((d_len-p_len == 2 ) && (directory[p_len+1] == '.'))
		return -1;

	if ((d_len-p_len == 3 ) && ((directory[p_len+1] == '.') && (directory[p_len+2] == '.')))
		return -1;

	if (stat(directory, &fs) < 0)
		return -1;

	if (!S_ISDIR(fs.st_mode))
		return -1;

	return 0;
}

int	diskMgrQuotaAddDir(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;

	OmnString contents;
	OmnString systemCmd;
	char size[64];
	memset(size, 0, sizeof(size));

	if (status_is_on(parms->mStrings[0]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s partition status is not on\n", parms->mStrings[0]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_StatusNoOn;
	}

	if (check_directory(parms->mStrings[0], parms->mStrings[1]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s is a invald directory\n", parms->mStrings[1]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_InvalidDir;
	}

	if (check_size(parms->mStrings[2], size) < 0)
	{
		snprintf(errmsg, errlen-1, 
				"Usage: Size:%s, Invald input, must be end of [M|m|K|k], can't be much than 40G", 
				parms->mStrings[2]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_SizeError;
	}

	int gid=get_unused_gid(parms->mStrings[0]);
	
	systemCmd = "";
	systemCmd << "chgrp -R "<<gid<<" "<< parms->mStrings[1];
	OmnCliSysCmd::doShell(systemCmd, contents);
	systemCmd = "";
	systemCmd << "chmod g+s "<< parms->mStrings[1];
	OmnCliSysCmd::doShell(systemCmd, contents); 

	systemCmd = "";
	systemCmd << "setquota -g " << gid << " " << 
		size << " "<< size << " 10000" 
		<< " 10000 " << parms->mStrings[0] << " 2>&1"; 
	OmnCliSysCmd::doShell(systemCmd, contents);
	/*save limit*/
	save_limit(1, parms->mStrings[0], parms->mStrings[1], parms->mStrings[2]);
	*optlen = index;

	return 0;
}


int diskMgrQuotaDeleteDir(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString contents;
	OmnString systemCmd;

	if (status_is_on(parms->mStrings[0]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s partition status is not on\n", parms->mStrings[0]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_StatusNoOn;
	}

	if (check_directory(parms->mStrings[0], parms->mStrings[1]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s is a invald directory\n", parms->mStrings[1]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_InvalidDir;
	}
	if (remove_limit(1, parms->mStrings[0], parms->mStrings[1]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s is not be limit by quota\n", parms->mStrings[1]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_DirNoBeLimit;
	}

	/* here real start */
	int gid=get_gid_of_directory( parms->mStrings[1] );

	systemCmd = "";
	systemCmd << "chgrp -R 0 "<< parms->mStrings[1];
	OmnCliSysCmd::doShell(systemCmd, contents); 
	systemCmd = "";
	systemCmd << "chmod g-s "<< parms->mStrings[1];
	OmnCliSysCmd::doShell(systemCmd, contents); 

	systemCmd = "";
	systemCmd << "setquota -g " << gid << " 0" <<
		" 0" << " 0" << " 0 " << parms->mStrings[0]; 
	OmnCliSysCmd::doShell(systemCmd, contents); 
	/* here real end */
	*optlen = index;
    return 0;
}


int	diskMgrQuotaAddUser(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char size[64];
	OmnString contents;
	OmnString systemCmd;

	memset(size, 0, sizeof(size));
	if (status_is_on(parms->mStrings[0]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s partition status is not on\n", parms->mStrings[0]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_StatusNoOn;
	}

	if (check_size(parms->mStrings[2], size) < 0)
	{
		snprintf(errmsg, errlen-1, 
				"Usage: Size:%s, Invald input, must be end of [M|m|K|k], can't be much than 40G", 
				parms->mStrings[2]); 
		errmsg[errlen - 1] = '\0';
		return -eAosRc_SizeError;
	}

	if (!getpwnam(parms->mStrings[1]))
	{
		snprintf(errmsg, errlen-1, "Usage:%s is not a exist user\n", parms->mStrings[1]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_UserNoExist;
	}
	
	systemCmd << "setquota -u " << parms->mStrings[1] << " " << 
		size << " "<< size << " 10000" 
		<< " 10000 " << parms->mStrings[0] << " 2>&1"; 
	OmnCliSysCmd::doShell(systemCmd, contents);
	/*save config*/
	save_limit(0, parms->mStrings[0], parms->mStrings[1], parms->mStrings[2]);

	*optlen = index;
	return 0;
}

int diskMgrQuotaDeleteUser(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString contents;
	OmnString systemCmd;
	/* here real start */

	if (status_is_on(parms->mStrings[0]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s partition status is not on\n", parms->mStrings[0]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_StatusNoOn;
	}

	if (!getpwnam(parms->mStrings[1]))
	{
		snprintf(errmsg, errlen-1, "Usage:%s is not a exist user\n", parms->mStrings[1]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_UserNoExist;
	}

	if (remove_limit(0, parms->mStrings[0], parms->mStrings[1]) < 0)
	{
		snprintf(errmsg, errlen-1, "Usage:%s is not be limit by quota\n", parms->mStrings[1]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_UserNoBeLimit;
	}

	systemCmd << "setquota -u " << parms->mStrings[1] << " 0" <<
		" 0" << " 0" << " 0 " << parms->mStrings[0] << " 2>&1"; 
	OmnCliSysCmd::doShell(systemCmd, contents); 
	/* here real end */

	*optlen = index;
    return 0;
}

static int quota_show_limit(int show_type, char *name, OmnString &contents)
{
	FILE *fp;
	int type;
	char readbuf[256], partition[64], user[64], size[64], local[256]; 
	memset(readbuf, 0, sizeof(readbuf));
	memset(partition, 0, sizeof(partition));
	memset(user, 0, sizeof(user));
	memset(size, 0, sizeof(size));

	fp = fopen("/usr/local/rhc/config/quota.conf", "r");
	if (fp == NULL)
		return -1;

	if (show_type)
	{
		contents << "Disk quotas for dir: " << name << "\n" 
			<< "Filesystem\t\t DirName\t\tSize\n";
	}
	else
	{
		contents << "Disk quotas for user: " << name << "\n" 
			<< "Filesystem\t\tUserName\t\tSize\n";
	}

	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		sscanf(readbuf, "%d %s %s %s", &type, partition, user, size);
		if (show_type == type && !strcmp(user, name))
		{
			memset(local, 0, sizeof(local));
			sprintf(local, "%8s\t\t%8s\t\t%4s\n", partition, user, size);
			contents << local;
		}
		memset(readbuf, 0, sizeof(readbuf));
	}
	fclose(fp);

	return 0;
}

int diskMgrQuotaShowUser(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString contents;

	if (!getpwnam(parms->mStrings[0]))
	{
		snprintf(errmsg, errlen-1, "Usage:%s is not a exist user\n", parms->mStrings[0]);
		errmsg[errlen - 1] = '\0';
		return -eAosRc_UserNoExist;
	}
	/* here real start */
	quota_show_limit(0, parms->mStrings[0], contents);

	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;
    return 0;
}

int diskMgrQuotaShowDir(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString contents;
	OmnString systemCmd;
	char* buf = CliUtil_getBuff(data);

	/* here real start */
	quota_show_limit(1, parms->mStrings[0], contents);

	/* here real end */
	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;
    return 0;
}

static int get_on_partition(char partition[][64])
{
	FILE *fp;
	char readbuf[256], mount_point[64];
	int i = 0;

	memset(readbuf, 0, sizeof(readbuf));
	memset(mount_point, 0, sizeof(mount_point));
	fp = fopen("/etc/fstab", "r");
	if (fp == NULL)
		return -1;

	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		if (strstr(readbuf, "defaults,grpquota,usrquota"))
		{
			sscanf(readbuf, "%*s %s %*s %*s %*d %*d", mount_point); 	
			strcpy(partition[i], mount_point);
			i++;
			memset(mount_point, 0, sizeof(mount_point));
		}
		memset(readbuf, 0, sizeof(readbuf));
	}

	fclose(fp);
	return 0;
}

int diskMgrQuotaClearConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char partition[16][64];
	OmnString contents;
	int i;

	for (i=0; i<16; i++)
		memset(partition[i], 0, sizeof(partition[i]));
	contents = "";
	
	if (get_on_partition(partition) < 0)
	{
		sprintf(errmsg, "Usage: Clear config error, For open fstat file failed \n");
		errmsg[errlen - 1] = '\0';
		return -eAosRc_FailedOpenFile;
	}

	i = 0;
	while (strcmp(partition[i], ""))
	{
		if (quota_status_off(partition[i]) < 0)
		{
			sprintf(errmsg, "Usage:Failed to clear status config");
			errmsg[errlen-1] = 0;
			return -eAosRc_FailedClearStatus;
		}
		i++;
	}
	unlink("/usr/local/rhc/config/quota.conf");

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;
	return 0; 
}

int diskMgrQuotaSaveConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);

	OmnString contents;
	contents = "";
	
	/*save partition status*/
	save_quota_status(contents);
	/*save user*/
	save_quota_limit(contents);

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;
	
	return 0;
}

static int get_limit_user(int show_type, char username[][64])
{
	FILE *fp;
	int type, i, j;
	char readbuf[256], user[64]; 
	memset(readbuf, 0, sizeof(readbuf));
	memset(user, 0, sizeof(user));

	fp = fopen("/usr/local/rhc/config/quota.conf", "r");
	if (fp == NULL)
		return -1;

	i = j =0;
	while (fgets(readbuf, sizeof(readbuf), fp))
	{
		sscanf(readbuf, "%d %*s %s %*s", &type, user);
		if (show_type == type)
		{
			for (i=0; i<j; i++)
			{
				if (!strcmp(username[i], user))
					break;
			}
			if (i == j)
			{
				strcpy(username[j], user);
				j++;
			}
		}
		memset(readbuf, 0, sizeof(readbuf));
	}

	fclose(fp);
	return 0;
}

int diskMgrQuotaShowConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	int ret, i;

	OmnString contents;
	contents = "";
	char username[16][64];
	for (i=0; i<16; i++)
	{
		memset(username[i], 0, sizeof(username[i]));
	}

	/*show quota status*/
	ret = quota_status_show(contents);
	if (ret < 0)
	{
		sprintf(errmsg, "Failed open config file\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_FailedOpenFile;
	}

	contents << "\n";
	/*show user limit*/
	get_limit_user(0, username);
	i = 0;
	while (strcmp(username[i], ""))
	{
		quota_show_limit(0, username[i], contents);
		i++;
	}

	for (i=0; i<16; i++)
	{
		memset(username[i], 0, sizeof(username[i]));
	}
	/*show directory limit*/
	get_limit_user(1, username);
	i = 0;
	while (strcmp(username[i], ""))
	{
		quota_show_limit(1, username[i], contents);
		i++;
	}

	CliUtil_checkAndCopy(buf, &index, *optlen, contents.data(), contents.length());
	*optlen = index;
	
	return 0;
}

int QuotaMgr_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("disk mgr quota status",diskMgrQuotaStatus);
	ret |= CliUtil_regCliCmd("disk mgr quota status show",diskMgrQuotaStatusShow);
	ret |= CliUtil_regCliCmd("disk mgr quota list",diskMgrQuotaList);
	ret |= CliUtil_regCliCmd("disk mgr quota add user",diskMgrQuotaAddUser);
	ret |= CliUtil_regCliCmd("disk mgr quota delete user",diskMgrQuotaDeleteUser);
	ret |= CliUtil_regCliCmd("disk mgr quota add dir",diskMgrQuotaAddDir);
	ret |= CliUtil_regCliCmd("disk mgr quota delete dir",diskMgrQuotaDeleteDir);
	ret |= CliUtil_regCliCmd("disk mgr quota show user",diskMgrQuotaShowUser);
	ret |= CliUtil_regCliCmd("disk mgr quota show dir",diskMgrQuotaShowDir);
	ret |= CliUtil_regCliCmd("disk mgr quota show config",diskMgrQuotaShowConfig);
	ret |= CliUtil_regCliCmd("disk mgr quota clear config",diskMgrQuotaClearConfig);
	ret |= CliUtil_regCliCmd("disk mgr quota save config",diskMgrQuotaSaveConfig);
	
	return ret;
}



