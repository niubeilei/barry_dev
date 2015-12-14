///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemMisc.cpp
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
#include <string.h>

#include "Global.h"

int SystemShowInfo(char *data, unsigned int *optlen, 
				  struct aosUserLandApiParms *parms,
				  char *errmsg, const int errlen)
{
	OmnString cmd;
	OmnString cmd1;
	OmnString cmd2;
	OmnString contents;
	OmnString rslt;
	OmnString rslt1;
	OmnString rslt2;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[10240];

	cmd = "/bin/uname -a";
	cmd1 = "/sbin/ifconfig -a";
	cmd2 = "/sbin/route -n";

	OmnCliSysCmd::doShell(cmd, rslt);
	OmnCliSysCmd::doShell(cmd1, rslt1);
	OmnCliSysCmd::doShell(cmd2, rslt2);
	contents << "System information\n" 
	         << "-------------------------\n"
			 << "       Uname:\n"
			 <<  rslt << "\n"
			 << "       Network:\n"
	         << rslt1 << "\n"
			 << "       Route table:\n"
	         << rslt2 << "\n";

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen,
						local, strlen(local));
	*optlen = index;

	return 0;
}


int SystemSetDomain(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms,
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString domainname;
	int ip;
	char ipStr[32];
	OmnString rslt;
	bool isFileFinished;
	OmnString line1;
	OmnString newLine;
	struct in_addr tmpaddr;

	domainname = parms->mStrings[0];
	ip = parms->mIntegers[0];

	OmnFilePtr hostsFile = OmnNew OmnFile("../rhcConf/hosts.template",OmnFile::eReadOnly);
	
	OmnFilePtr tmpHostsFile = OmnNew OmnFile("../rhcConf/hosts_init",OmnFile::eCreate);

	if (!hostsFile)
	{
		strncpy(errmsg,"Failed to open configure file: hosts.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpHostsFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "hostname_init";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	isFileFinished = false;
	line1 = hostsFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;

		curPos = line1.getWord(curPos,word1);
		OmnString prefix1(word1.getBuffer(),10);
		if(word1 == "127.0.0.1")
		{
			tmpHostsFile->append(line1);
		}
		else if(word1 == "#host_list")
		{
			if(ip != 0)
			{
				tmpaddr.s_addr= ip;
				strcpy(ipStr, inet_ntoa(tmpaddr));
			}
			else
			{
				strcpy(ipStr, "");
			}
			newLine = "\n";
			newLine << ipStr << " " << domainname;
			tmpHostsFile->append(newLine);
		}
		line1 = hostsFile->getLine(isFileFinished);
	}

	tmpHostsFile->closeFile();
	hostsFile->closeFile();
    OmnString renameCmd = "/bin/mv ../rhcConf/hosts_init /etc/hosts";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	*optlen = index;
	return 0;
}

int SystemSetHostname(char *data, unsigned int *optlen,
					  struct aosUserLandApiParms *parms,
					  char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString hostname;
	OmnString rslt;
	char name[16];
	bool isFileFinished;
	OmnString line1;

	hostname = parms->mStrings[0];
	strncpy(name, hostname.data(),hostname.length());
	
	//cout << "hostname is " << hostname <<endl;
	for(int i=0; i<15; i++)
	{
		int z = int(name[i]);
		//cout << z << endl;
		if(((z >=65)&&(z<= 90))||((z>= 97)&&(z<= 122))||((z >=48) && (z <= '9'))||(z == 45)||(z == 46)||(z == 95)||(z== 150)||(z==0) )
		{
		}
		else
		{
			rslt = "You input a wrong character!";
			strncpy(errmsg,rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_InvalidHostname;

		}
	}

	OmnFilePtr hostnameFile = OmnNew OmnFile("../rhcConf/hostname.template",OmnFile::eReadOnly);
	OmnFilePtr tmpHostnameFile = OmnNew OmnFile("../rhcConf/hostname_init",OmnFile::eCreate);

	if (!hostnameFile)
	{
		strncpy(errmsg,"Failed to open configure file: hostname.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpHostnameFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "hostname_init";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	isFileFinished = false;
	line1 = hostnameFile->getLine(isFileFinished);

	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;

		curPos = line1.getWord(curPos, word1);
		OmnString prefix1(word1.getBuffer(),10);
		if(hostname != "")
		{
			OmnString  newLine= "HOSTNAME=\"";
			newLine << hostname << "\"\n";
			tmpHostnameFile->append(newLine);
		}
		else
		{
			tmpHostnameFile->append(line1);
		}
		line1 = hostnameFile->getLine(isFileFinished);
	}

	tmpHostnameFile->closeFile();
	hostnameFile->closeFile();
	
	OmnString renameCmd;
	renameCmd << "/bin/mv ../rhcConf/hostname_init /etc/conf.d/hostname";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	*optlen = index;
	return 0;
}

int SystemMisc_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("system info", SystemShowInfo);
	ret = CliUtil_regCliCmd("system set domainname", SystemSetDomain);
	ret = CliUtil_regCliCmd("system set hostname", SystemSetHostname);
	
	return ret;
}
