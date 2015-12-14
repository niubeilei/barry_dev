////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemDnsProxyCli.cpp
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
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

#include "Global.h"

// add for get_ip_address
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#define MAXINTERFACES 32

// Global Para
struct dnsProxyConfigEntry sgDnsproxy;

//get eth0 IpAddr
char* get_ip_address(void)
{
	register int fd, intrface;
	struct ifreq buf[MAXINTERFACES];
	//struct arpreq arp;
	struct ifconf ifc;
	static char ip_addr[50];
	char* p;

	memset(ip_addr, 0, 50);
	//============Part ONE: Get incard Ip (br0)
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		{
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			intrface--;
			while (intrface >= 0)
			{
				//Get br0 IP
				if (strcasecmp(buf[intrface].ifr_name,"br0")!=0)
				{
					intrface--;
					continue;
				}

				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
				{
					//puts ("Ip address is:");
					p = inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr); 
					strncpy(ip_addr, p, 32);
					//puts("");
					//memcpy(ip_add, buf[intrface].ifr_hwaddr.sa_data, 6);
					//printf("%s\n", ip_add);	

				}
				break;
			}
		}
	}
	close (fd);
	//============End of Part ONE
	return ip_addr;
}

int dnsproxyInit()
{
	sgDnsproxy.ip = "";
	sgDnsproxy.lIp = "";
	sgDnsproxy.dev = "incard";
	sgDnsproxy.global = "no";
	sgDnsproxy.name = "";
	sgDnsproxy.status = "stop";
	return 0;
}
int dnsproxyStart(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);
	unsigned int index = 0;
	char local[2048];
	OmnString stopDnsproxyCmd;
	OmnString stopNamedCmd;
	OmnString rslt;
	stopDnsproxyCmd << "/usr/bin/killall -9 pdnsd >/dev/null 2>&1";
	OmnCliSysCmd::doShell(stopDnsproxyCmd, rslt);

	stopNamedCmd << "/etc/init.d/named stop";
	OmnCliSysCmd::doShell(stopNamedCmd, rslt);

	bool changedFlag1 = false;
	bool changedFlag2 = false;
	bool changedFlag3 = false;
	bool isFileFinished = false;

	//get incard ip_addr
	sgDnsproxy.lIp = "";	
	sgDnsproxy.lIp = get_ip_address();
	if(sgDnsproxy.lIp == "")
	{
		rslt << "Incard not exist or not set ip address!";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if ((sgDnsproxy.ip == 0) || (sgDnsproxy.ip == "") || (sgDnsproxy.lIp == 0) || (sgDnsproxy.lIp == ""))
	{
		strncpy(errmsg, "The DNSPROXY's config has not been setted well! Sorry, can not to run!\n",errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	OmnFilePtr pdnsFile = OmnNew OmnFile("../rhcConf/pdnsd.conf.template",OmnFile::eReadOnly);
	OmnFilePtr tmpFile = OmnNew OmnFile("../rhcConf/pdnsd_init.conf",OmnFile::eCreate);

	if (!pdnsFile)
	{
		strncpy(errmsg,"Failed to open configure file: pdnsd.conf.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!pdnsFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pdnsd.conf.template";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile)
	{
		rslt = "Failed to open configure file: ";
		rslt << "dnsd_init.conf";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "pdnsd_init.conf";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}


	//write config file

	OmnString line1 = pdnsFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;
		curPos = line1.getWord(curPos,word1);
		OmnString prefix1(word1.getBuffer(),10);
		if(changedFlag1 == false && (word1 == "label" || word1 == "label="))
		{
			OmnString  newLine= "\tlabel";
			newLine << "= \"" << sgDnsproxy.name << "\";\n";
			tmpFile->append(newLine);
			changedFlag1 = true;
		}
		else if(changedFlag2 == false && (word1 == "ip" || word1 == "ip="))
		{
			OmnString  newLine = "\tip";
			newLine << " = " << sgDnsproxy.ip << ";\n";
			tmpFile->append(newLine);
			changedFlag2 = true;
		}
		else if(changedFlag3 == false && word1 == "server_ip")
		{
			OmnString  newLine = "\tserver_ip";
			newLine << " = " << sgDnsproxy.lIp << ";\n";
			tmpFile->append(newLine);
			changedFlag3 = true;
		}
		/*
		   else if(changedFlag4 == false &&  prefix1 == "interface=")
		   {
		   OmnString  eth;
		   if(sgDnsproxy.dev == "outcard")
		   {
		   eth = "eth0";
		   }
		   else if(sgDnsproxy.dev == "incard")
		   {
		   eth = "br0";
		   }
		   else
		   {
		   rslt << "You input is wrong, please check!";
		   strncpy(errmsg,rslt,errlen-1);
		   errmsg[errlen-1] = 0;
		   return -1;
		   }
		   OmnString  newLine = "\tinterface=";
		   newLine << " " << eth << ";\n";
		   tmpFile->append(newLine);
		   changedFlag4 = true;
		   }*/
		else if (sgDnsproxy.global == "yes" && line1 == "/*# This section is meant for resolving from root servers.")
		{
			OmnString  newLine = "#This section is meant for resolving from root servers.\n";
			tmpFile->append(newLine);
		}
		else if (sgDnsproxy.global == "yes" && line1 == "}*/")
		{
			OmnString  newLine = "}\n";
			tmpFile->append(newLine);
		}
		else
		{
			line1 << "\n";
			tmpFile->append(line1);
		}
		line1 = pdnsFile->getLine(isFileFinished);
	}

	if(changedFlag1 == false || changedFlag2 == false || changedFlag3 == false )
	{
		// it's an error, send an alarm, then return false;
		//cout << "Not change the config!";
		return 0;
	}
	tmpFile->closeFile();
	pdnsFile->closeFile();

	OmnString renameCmd;
	renameCmd << "/bin/mv ../rhcConf/pdnsd_init.conf /usr/local/etc/pdnsd.conf";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	OmnString systemCmd;
	systemCmd << "/usr/local/sbin/pdnsd >/dev/null 2>&1 &";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	sgDnsproxy.status = "start";


	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}


	int 
dnsproxyStop(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;
	if (sgDnsproxy.status == "stop")
	{
		rslt << "Dnsproxy has already been stoped!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	OmnString systemCmd;
	systemCmd << "/usr/bin/killall -9 pdnsd >/dev/null 2>&1 ";
	OmnCliSysCmd::doShell(systemCmd, rslt);

	sgDnsproxy.status = "stop";

	*optlen = index;
	return 0;
}


// GB, 11/14/2006
int dnsproxyShow(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	sgDnsproxy.lIp = "";
	sgDnsproxy.lIp = get_ip_address();
	OmnString contents;

	contents << "DNS PROXY information\n" << "--------------------------------\n" <<"\t   status : " << sgDnsproxy.status << "\n\t   global : " << sgDnsproxy.global << "\n\t    label : " << sgDnsproxy.name << "\n\t      dns : " << sgDnsproxy.ip << "\n\t local ip : " << sgDnsproxy.lIp << "\n\t      dev : " << sgDnsproxy.dev << "\n"; 

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;


	return 0;
}

int dnsproxySaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	OmnString contents;
	
	 if (sgDnsproxy.ip != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "dns proxy set ip " << sgDnsproxy.ip;
	 	contents << "</Cmd>\n";
	 }  
	 if (sgDnsproxy.name != "")
	 {
	 	contents<< "<Cmd>";
	 	contents << "dns proxy set name " << sgDnsproxy.name;
	 	contents << "</Cmd>\n";
	 }
	 if (sgDnsproxy.global == "yes")
	 {
	 	contents<< "<Cmd>";
	 	contents << "dns proxy add global";
	 	contents << "</Cmd>\n";
	 }
	 else 
	 {
	 	contents << "<Cmd>";
	 	contents << "dns proxy del global" ;
	 	contents << "</Cmd>\n";
	 }
	 
	contents << "<Cmd>";
	contents << "dns proxy "<< sgDnsproxy.status;
	contents << "</Cmd>\n";
		 
	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}

int dnsproxySetName(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString cmd;
	OmnString hostname;
	OmnString rslt;
	char name[32];
	hostname = parms->mStrings[0];
	sgDnsproxy.name = hostname;

	strncpy(name, hostname.data(),32);
	for(int i=0; i<31; i++)
	{
		int z = int(name[i]);
		//cout << z << endl;
		if(((z >=65)&&(z<= 90))||((z>= 97)&&(z<= 122))||((z >=48) && (z <= '9'))||(z == 45)||(z == 46)||(z == 95)||(z== 150)||(z==0) )
		{
		}
		else
		{
			rslt = "You input a wrong character!";
			sgDnsproxy.name="";
			strncpy(errmsg,rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;

		}
	}
	cmd << "echo " << "HOSTNAME=" << "\\\"" <<"\"" << hostname << "\"" <<"\\\"" << " >" << "/etc/conf.d/hostname";
	//cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
	*optlen = index;
	return 0;
}


int dnsproxySetIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	sgDnsproxy.ip = parms->mStrings[0];

	*optlen = index;
	return 0;
}


int dnsproxyAddGlobal(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{

	unsigned int index = 0;
	sgDnsproxy.global = "yes";

	*optlen = index;
	return 0;
}


int dnsproxyDelGlobal(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;
	if(sgDnsproxy.global == "no")
	{
		rslt << "You not use global option!";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	else
	{
		sgDnsproxy.global = "no";
	}

	*optlen = index;
	return 0;
}


// GB   11/24/2006 
int dnsproxySetLIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;

	sgDnsproxy.lIp = parms->mStrings[0];
	sgDnsproxy.dev = parms->mStrings[1];

	if((sgDnsproxy.dev == 0) || (sgDnsproxy.dev == "") )
	{
		sgDnsproxy.dev = "incard";
	}
	else if((sgDnsproxy.dev == "outcard") ||(sgDnsproxy.dev == "incard"))
	{	
		sgDnsproxy.lIp = parms->mStrings[0];
	}
	else
	{
		sgDnsproxy.dev = "incard";
		sgDnsproxy.lIp = "";
		rslt << "You input is wrong, the parameters must be outcard or incard";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	*optlen = index;
	return 0;
}

int dnsproxyClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	dnsproxyInit();
	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}

int DnsProxy_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("dns proxy show config", dnsproxyShow);
	ret |= CliUtil_regCliCmd("dns proxy set ip", dnsproxySetIp);
	ret |= CliUtil_regCliCmd("dns proxy set name", dnsproxySetName);
	ret |= CliUtil_regCliCmd("dns proxy add global", dnsproxyAddGlobal);
	ret |= CliUtil_regCliCmd("dns proxy del global", dnsproxyDelGlobal);
	ret |= CliUtil_regCliCmd("dns proxy set lip", dnsproxySetLIp);
	ret |= CliUtil_regCliCmd("dns proxy stop", dnsproxyStop);
	ret |= CliUtil_regCliCmd("dns proxy start", dnsproxyStart);
	ret |= CliUtil_regCliCmd("dns proxy save config", dnsproxySaveConfig);
	ret |= CliUtil_regCliCmd("dns proxy clear config", dnsproxyClearConfig);

	return ret;
}


