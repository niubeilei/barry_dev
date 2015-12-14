////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemDmzCli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "KernelInterface/CliSysCmd.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/List.h"  
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include "rhcUtil/aosResMgrToolkit.h"
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include "Global.h"
#include <unistd.h>
#include <stdio.h>
// Chenjiaxin, 04/17/2007
// Global Para

int timeSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString stopdmzCmd;
	OmnString rslt;
	int h;
	int m;
	int s;
	h = parms->mIntegers[0];
	m = parms->mIntegers[1];
	s = parms->mIntegers[2];
	if(h>23||h<0||m>59||m<0||s>59||s<0)
	{
		sprintf(errmsg,"input error, the h/m/s shouled all between 1 and 59\n");
		return -eAosRc_CliCmdParmError;
	}
	struct   tm   tm;
	time_t   t,t2;   
	struct   tm   *tmp;   
	struct   timeval   tv; 
	FILE *fp=NULL;
	char log[256];
	pid_t pid;
	pid=getpid();
	//get the current time
	t   =   time(NULL);   
	tmp   =   localtime(&t);
	//log the time change settings	
	sprintf(log,"%d-%d-%d %d:%d:%d %d change_system_time %d:%d:%d\n",tmp->tm_year+1900,tmp->tm_mon+1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min,tmp->tm_sec,pid,h,m,s);
	fp=fopen("/var/log/timechange.log","a");
	if(!fp)
	{
		sprintf(errmsg,"open log file failed!\n");
		return -eAosRc_FileFailWrite;
	}
	fputs( log,fp);
	fclose(fp);
	//apply the time change set	
	memset(&tm,   0,   sizeof(tm));  
	tm.tm_sec   =s;                     
	tm.tm_min   =m;                   
	tm.tm_hour  =h;       
	tm.tm_mday  =tmp->tm_mday;                 
	tm.tm_mon   =tmp->tm_mon;                 
	tm.tm_year  =tmp->tm_year;                   
	t2   =   mktime(&tm);  
	tv.tv_sec   =   t2;       
	tv.tv_usec  =   0;       
	if(0!=settimeofday(&tv,(struct timezone *)0))
	{
		sprintf(errmsg,"set system time failed\n");
		return -eAosRc_SystemTimeSetFaild;
	}
	*optlen = index;
	return 0;
}

int dateSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString stopdmzCmd;
	OmnString rslt;
	int y;
	int m;
	int d;
	y = parms->mIntegers[0];
	m = parms->mIntegers[1];
	d = parms->mIntegers[2];
	if(y<1900||m>11||m<0||d>31||d<1)
	{
		sprintf(errmsg,"input error, check your inputs\n");
		return -eAosRc_CliCmdParmError;
	}
	struct   tm   tm;
	time_t   t,t2;   
	struct   tm   *tmp;   
	struct   timeval   tv; 
	FILE *fp=NULL;
	char log[256];
	pid_t pid;
	pid=getpid();
	//get the current time
	t   =   time(NULL);   
	tmp   =   localtime(&t);
	//log the time change settings	
	sprintf(log,"%d-%d-%d %d:%d:%d %d change_system_date %d-%d-%d\n",tmp->tm_year+1900,tmp->tm_mon+1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min,tmp->tm_sec,pid,y,m,d);
	fp=fopen("/var/log/timechange.log","a");
	if(!fp)
	{
		sprintf(errmsg,"open log file failed!\n");
		return -eAosRc_FileFailWrite;
	}
	fputs( log,fp);
	fclose(fp);
	//apply the time change set	
	memset(&tm,   0,   sizeof(tm));  
	tm.tm_sec   =tmp->tm_sec;                     
	tm.tm_min   =tmp->tm_min;                   
	tm.tm_hour  =tmp->tm_hour;       
	tm.tm_mday  =d;                 
	tm.tm_mon   =m;                 
	tm.tm_year  =y-1900;                   
	t2   =   mktime(&tm);  
	tv.tv_sec   =   t2;       
	tv.tv_usec  =   0;       
	if(0!=settimeofday(&tv,(struct timezone *)0))
	{
		sprintf(errmsg,"set system time failed\n");
		return -eAosRc_SystemDateSetFaild;
	}
	*optlen = index;
	return 0;
}

int timeFastSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	time_t t,t2;
	struct   tm   *tmp;
	int s;
	s=parms->mIntegers[0];
	if(s<0)
	{
		sprintf(errmsg,"input parameter error, check your inputs\n");
		return -eAosRc_CliCmdParmError;
	}

	FILE *fp=NULL;
	char log[256];	
	pid_t pid;
	pid=getpid();	
	t  =time(NULL);   
	tmp =localtime(&t);
	sprintf(log,"%d-%d-%d %d:%d:%d %d  set_time_faster %d seconds \n",tmp->tm_year+1900,tmp->tm_mon+1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min,tmp->tm_sec,pid,s);
	fp=fopen("/var/log/timechange.log","a");
	if(!fp)
	{
		sprintf(errmsg,"open log file failed!\n");
		return -eAosRc_FileFailWrite;
	}
	fputs( log,fp);
	fclose(fp);
	t2 =mktime(tmp);
	t2+=s;
	if(0!=stime(&t2))
	{
		sprintf(errmsg,"fast set system time failed\n");
		return -eAosRc_SystemTimeFastSetFaild;
	}   
	*optlen = index;
	return 0;
}


int timeSlowSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	time_t t,t2;
	struct   tm   *tmp;
	int s;
	s=parms->mIntegers[0];
	FILE *fp=NULL;
	char log[256];	
	pid_t pid;
	pid=getpid();	
	t  =time(NULL);   
	tmp =localtime(&t);
	sprintf(log,"%d-%d-%d %d:%d:%d %d  set_time_slow %d seconds \n",tmp->tm_year+1900,tmp->tm_mon+1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min,tmp->tm_sec,pid,s);
	fp=fopen("/var/log/timechange.log","a");
	if(!fp)
	{
		sprintf(errmsg,"open log file failed! maybe it not exist\n");
		return -eAosRc_FileFailWrite;
	}
	fputs( log,fp);
	fclose(fp);
	t2 =mktime(tmp);
	t2-=s;
	if(0!=stime(&t2))
	{
		sprintf(errmsg,"Slow set system time failed\n");
		return -eAosRc_SystemTimeSlowSetFaild;
	}   
	*optlen = index;
	return 0;
}

int timeZoneSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	char timeZone[128], commandLine[256];
	OmnString rslt;
	struct stat stBuf;
	int ret;

	memset(timeZone, 0, sizeof(timeZone));
	memset(commandLine, 0, sizeof(commandLine));
	rslt = "";
	snprintf(timeZone, sizeof(timeZone), "/usr/share/zoneinfo/%s", parms->mStrings[0]);
	ret = lstat(timeZone, &stBuf);

	if (ret < 0 || !S_ISREG(stBuf.st_mode) || strstr(timeZone, ".tab"))
	{
		sprintf(errmsg, "Usage: Invalid timezone %s", parms->mStrings[0]);
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemTimeInvalidZoneParm;
	}

	snprintf(commandLine, sizeof(commandLine), 
			"ln -sf %s /etc/localtime", timeZone);
	OmnCliSysCmd::doShell(commandLine, rslt);
	if (rslt != "")
	{
		sprintf(errmsg, "Usage: Create link file some error occur.\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemTimeLinkFileErr;
	}

	OmnCliSysCmd::doShell("hwclock --systohc", rslt);
	if (rslt != "")
	{
		sprintf(errmsg, "Usage: hwclock --systohc error.\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemTimeCommandNoExist;
	}

	*optlen = 0;
	return 0;
}

int timeLogShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString Contents;
	char readbuf[256];
	char rslt[1024*1024];
	char* buf = CliUtil_getBuff(data);
	FILE *fp=NULL;
	fp=fopen("/var/log/timechange.log","r");
	if(!fp)
	{
		sprintf(errmsg, "open log file failed! \n");
		return -eAosRc_FileFailRead;
	}
	memset(readbuf, 0, sizeof(readbuf));
	while(fgets(readbuf, sizeof(readbuf), fp))
	{
		Contents<<readbuf;
		memset(readbuf, 0, sizeof(readbuf));
	}
	fclose(fp);
	strcpy(rslt,Contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, rslt,strlen(rslt));   
	*optlen = index;
	return 0;
}


int SystemTime_regCliCmd(void)
{	
	int ret;
	ret = CliUtil_regCliCmd("system time set", timeSet);
	ret |= CliUtil_regCliCmd("system time log show", timeLogShow);
	ret |= CliUtil_regCliCmd("system timezone set", timeZoneSet);
	ret |= CliUtil_regCliCmd("system time fast set", timeFastSet);
	ret |= CliUtil_regCliCmd("system time slow set", timeSlowSet);
	ret |= CliUtil_regCliCmd("system date set", dateSet);
	return ret;
}


