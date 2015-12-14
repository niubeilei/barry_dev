////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemdnsmasqCli.cpp
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

// Kevin, 03/08/2007
// Global Para
struct dnsmasqBindEntry dnsmasqBind;
struct aos_list_head sgDnsmasqBindList;
struct dnsmasqHostsEntry dnsmasqHosts;
struct aos_list_head sgDnsmasqHostsList;
struct dnsmasqConfigEntry sgDnsmasq;

int dnsmasqInit()
{
	OmnString ip1 = "192.168.0.2";
	OmnString ip2 = "192.168.0.254";
	char * netmask = "255.255.255.0";
	sgDnsmasq.hostname = "rhc";
	sgDnsmasq.exceptDev = "outcard";
	sgDnsmasq.leaseTime = "24h";
	sgDnsmasq.ip1 = inet_addr(ip1.data());
	sgDnsmasq.ip2 = inet_addr(ip2.data());
	sgDnsmasq.netmask = inet_addr(netmask);
	sgDnsmasq.dns1 = 0;
	sgDnsmasq.dns2 = 0;
	sgDnsmasq.status = "stop";

	AOS_INIT_LIST_HEAD(&sgDnsmasqHostsList);
	AOS_INIT_LIST_HEAD(&sgDnsmasqBindList);
	return 0;
}


int dnsmasqStart(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	char* buf = CliUtil_getBuff(data);
	unsigned int index = 0;
	char local[2048];
	OmnString stopDnsmasqCmd;
	OmnString rslt;
	struct in_addr tmpaddr;

	stopDnsmasqCmd << "/usr/bin/killall -9 dnsmasq >/dev/null 2>&1";
	OmnCliSysCmd::doShell(stopDnsmasqCmd, rslt);

	strcpy(local,"");
	bool changedFlag1 = false;
	bool changedFlag2 = false;
	bool changedFlag3 = false;
	bool isFileFinished = false;

	if ((sgDnsmasq.ip1 == 0) || (sgDnsmasq.ip2 == 0) || (sgDnsmasq.netmask == 0) || (sgDnsmasq.dns1 == 0) || (sgDnsmasq.leaseTime == "") || (sgDnsmasq.hostname == ""))
	{
		strncpy(errmsg, "The dnsmasq's config has not been setted well(at least hostname, ip_range and dns)!\nSorry, can not to run!\n",errlen-1);
		errmsg[errlen-1] = 0;
		sprintf (errmsg, "The dnsmasq's config has not been setted well\n");
		return -eAosRc_ConfigureError;
	}

	OmnFilePtr dnsmasqFile = OmnNew OmnFile("../rhcConf/dnsmasq.conf.template",OmnFile::eReadOnly);
	OmnFilePtr hostsFile = OmnNew OmnFile("../rhcConf/hosts.template",OmnFile::eReadOnly);
	OmnFilePtr resolvFile = OmnNew OmnFile("../rhcConf/resolv.conf.template",OmnFile::eReadOnly);
	// for Gentoo
	OmnFilePtr hostnameFile = OmnNew OmnFile("../rhcConf/hostname.template",OmnFile::eReadOnly);

	OmnFilePtr tmpFile = OmnNew OmnFile("../rhcConf/dnsmasq_init.conf",OmnFile::eCreate);
	OmnFilePtr tmpHostsFile = OmnNew OmnFile("../rhcConf/hosts_init",OmnFile::eCreate);
	OmnFilePtr tmpResolvFile = OmnNew OmnFile("../rhcConf/resolv_init.conf",OmnFile::eCreate);
	OmnFilePtr tmpHostnameFile = OmnNew OmnFile("../rhcConf/hostname_init",OmnFile::eCreate);

	if (!dnsmasqFile)
	{
		strncpy(errmsg,"Failed to open configure file: dnsmasq.conf.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!dnsmasqFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "dnsmasq.conf.template";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!hostsFile)
	{
		strncpy(errmsg,"Failed to open configure file: hosts.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!resolvFile)
	{
		strncpy(errmsg,"Failed to open configure file: resolv.conf.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!hostnameFile)
	{
		strncpy(errmsg,"Failed to open configure file: hostname.template\n",errlen-1);
		errmsg[errlen-1] = 0;
		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "dnsmasq_init.conf";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpHostsFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "hosts_init";
		strncpy(errmsg,rslt,errlen-1);
		errmsg[errlen-1] = 0;

		return eAosAlarm_FailedToOpenFile;
	}

	if (!tmpResolvFile->isGood())
	{
		rslt = "Failed to open configure file: ";
		rslt << "resolv_init.conf";
		strncpy(errmsg,rslt,errlen-1);
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

	//write config file

	struct dnsmasqHostsEntry * hptr;
	struct dnsmasqBindEntry * bptr;

	char ip1Str[MAX_IP_ADDR_LEN];
	char ip2Str[MAX_IP_ADDR_LEN];
	char netmaskStr[MAX_IP_ADDR_LEN];
	char ipStr[MAX_IP_ADDR_LEN];
	char dnsStr[MAX_IP_ADDR_LEN];

	OmnString line1 = dnsmasqFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;

		curPos = line1.getWord(curPos,word1);
		OmnString prefix1(word1.getBuffer(),10);
		if(changedFlag1 == false && (word1 == "except-interface="))
		{
			OmnString  newLine= "except-interface=";
			if (sgDnsmasq.exceptDev == "outcard")
				newLine << "eth0" << "\n";
			else
				newLine << "br0" << "\n";
			tmpFile->append(newLine);
			changedFlag1 = true;
		}
		else if(changedFlag2 == false && (word1 == "dhcp-range="))
		{
			OmnString  newLine= "dhcp-range=";

			tmpaddr.s_addr= sgDnsmasq.ip1;
			strcpy(ip1Str, inet_ntoa(tmpaddr));

			tmpaddr.s_addr= sgDnsmasq.ip2;
			strcpy(ip2Str, inet_ntoa(tmpaddr));

			tmpaddr.s_addr= sgDnsmasq.netmask;
			strcpy(netmaskStr, inet_ntoa(tmpaddr));

			newLine << ip1Str << "," << ip2Str << "," 
				<< netmaskStr << "," << sgDnsmasq.leaseTime << "\n";
			tmpFile->append(newLine);
			changedFlag1 = true;
		}
		else if(changedFlag3 == false && (word1 == "dhcp-host="))
		{
			aos_list_for_each_entry(bptr, &sgDnsmasqBindList, datalist)
			{
				OmnString  newLine = "dhcp-host=";
				tmpaddr.s_addr= bptr->ip;
				strcpy(ipStr, inet_ntoa(tmpaddr));
				newLine << bptr->mac << "," << ipStr << "\n";
				tmpFile->append(newLine);
			}
			changedFlag2 = true;
		}
		line1 = dnsmasqFile->getLine(isFileFinished);
	}
	/*
	   if(changedFlag1 == false || changedFlag2 == false || changedFlag3 == false)
	   {
	// it's an error, send an alarm, then return false;
	//cout << "Not change the config!";
	return 0;
	}
	*/
	tmpFile->closeFile();
	dnsmasqFile->closeFile();

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
			/* AS4 and Debian
			   OmnString  newLine= "127.0.0.1 ";
			   curPos = line1.getWord(curPos,word1);
			   newLine << word1 << " ";
			   curPos = line1.getWord(curPos,word1);
			   newLine << word1 << " ";
			   newLine << sgDnsmasq.hostname << "\n\n";
			   */
			tmpHostsFile->append(line1);
		}
		else if(word1 == "#host_list")
		{
			OmnString newLine;
			aos_list_for_each_entry(hptr, &sgDnsmasqHostsList, datalist)
			{
				if(hptr->ip != 0)
				{
					tmpaddr.s_addr= hptr->ip;
					strcpy(ipStr, inet_ntoa(tmpaddr));
				}
				else
					strcpy(ipStr, "");

				newLine = "\n";
				newLine << ipStr << " " << hptr->domain;
				tmpHostsFile->append(newLine);
			}
		}
		line1 = hostsFile->getLine(isFileFinished);
	}

	tmpHostsFile->closeFile();
	hostsFile->closeFile();

	isFileFinished = false;
	line1 = resolvFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;

		curPos = line1.getWord(curPos,word1);
		OmnString prefix1(word1.getBuffer(),10);
		if(word1 == "#nameserver")
		{
			OmnString  newLine= "nameserver ";
			if(sgDnsmasq.dns1 != 0)
			{
				tmpaddr.s_addr= sgDnsmasq.dns1;
				strcpy(dnsStr, inet_ntoa(tmpaddr));
				newLine << dnsStr;
				tmpResolvFile->append(newLine);
			}

			newLine = "";
			if (sgDnsmasq.dns2 != 0)
			{
				tmpaddr.s_addr= sgDnsmasq.dns2;
				strcpy(dnsStr, inet_ntoa(tmpaddr));
				newLine << "\nnameserver " << dnsStr;
				tmpResolvFile->append(newLine);
			}
		}
		line1 = resolvFile->getLine(isFileFinished);
	}

	tmpResolvFile->closeFile();
	resolvFile->closeFile();

	isFileFinished = false;
	line1 = hostnameFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString word1;
		int curPos = 0;

		curPos = line1.getWord(curPos, word1);
		OmnString prefix1(word1.getBuffer(),10);
		if(sgDnsmasq.hostname != "")
		{
			OmnString  newLine= "HOSTNAME=\"";
			newLine << sgDnsmasq.hostname << "\"\n";
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

	renameCmd = "";
	renameCmd << "/bin/mv ../rhcConf/dnsmasq_init.conf /etc/dnsmasq.conf";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	renameCmd = "";
	renameCmd << "/bin/mv ../rhcConf/hosts_init /etc/hosts";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	renameCmd = "";
	renameCmd << "/bin/mv ../rhcConf/resolv_init.conf /etc/resolv.conf";
	OmnCliSysCmd::doShell(renameCmd, rslt);

	OmnString systemCmd;
	systemCmd << "/usr/sbin/dnsmasq >/dev/null 2>&1 &";
	OmnCliSysCmd::doShell(systemCmd, rslt);
	sgDnsmasq.status = "start";

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}


int dnsmasqStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;
	if (sgDnsmasq.status == "stop")
	{
		rslt << "dnsmasq has already been stoped!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_ServerAlreadyStopped;
	}
	OmnString systemCmd;
	systemCmd << "/usr/bin/killall -9 dnsmasq >/dev/null 2>&1 ";
	OmnCliSysCmd::doShell(systemCmd, rslt);

	sgDnsmasq.status = "stop";

	*optlen = index;
	return 0;
}


int dnsmasqShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];

	OmnString contents;
	OmnString hostlist;
	OmnString bindlist;
	struct dnsmasqHostsEntry * hptr;
	struct dnsmasqBindEntry * bptr;

	struct in_addr tmpaddr;
	char ip1Str[MAX_IP_ADDR_LEN];
	char ip2Str[MAX_IP_ADDR_LEN];
	char netmaskStr[MAX_IP_ADDR_LEN];
	char dns1Str[MAX_IP_ADDR_LEN];
	char dns2Str[MAX_IP_ADDR_LEN];
	char ipStr[MAX_IP_ADDR_LEN];

	strcpy(local,"");

	if(sgDnsmasq.ip1 != 0)
	{
		tmpaddr.s_addr= sgDnsmasq.ip1;
		strcpy(ip1Str, inet_ntoa(tmpaddr));
	}
	else
		strcpy(ip1Str, "");

	if(sgDnsmasq.ip2 != 0)
	{
		tmpaddr.s_addr= sgDnsmasq.ip2;
		strcpy(ip2Str, inet_ntoa(tmpaddr));
	}
	else
		strcpy(ip2Str, "");

	if(sgDnsmasq.netmask != 0)
	{
		tmpaddr.s_addr= sgDnsmasq.netmask;
		strcpy(netmaskStr, inet_ntoa(tmpaddr));
	}
	else
		strcpy(netmaskStr, "");

	if(sgDnsmasq.dns1 != 0)
	{
		tmpaddr.s_addr = sgDnsmasq.dns1;
		strcpy(dns1Str, inet_ntoa(tmpaddr));
	}
	else
		strcpy(dns1Str, "");

	if(sgDnsmasq.dns2 != 0)
	{
		tmpaddr.s_addr = sgDnsmasq.dns2;
		strcpy(dns2Str, inet_ntoa(tmpaddr));
	}
	else
		strcpy(dns2Str, "");

	contents << "dnsmasq information\n" 
		<< "--------------------------------" 
		<< "\n\t   status : " << sgDnsmasq.status 
		<< "\n\t hostname : " << sgDnsmasq.hostname 
		<< "\n\t      ip1 : " << ip1Str 
		<< "\n\t      ip2 : " << ip2Str 
		<< "\n\t  netmask : " << netmaskStr 
		<< "\n\tleaseTime : " << sgDnsmasq.leaseTime
		<< "\n\t     dns1 : " << dns1Str 
		<< "\n\t     dns2 : " << dns2Str 
		<< "\n\texceptDev : " << sgDnsmasq.exceptDev 
		<< "\n\thost list : " 
		<< "\n\n"; 

	aos_list_for_each_entry(hptr, &sgDnsmasqHostsList, datalist)
	{
		tmpaddr.s_addr= hptr->ip;
		strcpy(ipStr, inet_ntoa(tmpaddr));
		hostlist << "\t\t\t" << hptr->alias << " " << ipStr << " " << hptr->domain << "\n";
	}
	contents << "\t\t\talias IP DOMAIN\n\t\t\t--------------\n" << hostlist;

	contents << "\n\tbind list : " 
		<< "\n\n"; 

	aos_list_for_each_entry(bptr, &sgDnsmasqBindList, datalist)
	{
		tmpaddr.s_addr= bptr->ip;
		strcpy(ipStr, inet_ntoa(tmpaddr));
		bindlist << "\t\t\t" << bptr->alias << " " << bptr->mac << " " << ipStr << "\n";
	}
	contents << "\t\t\talias MAC IP\n\t\t\t--------------\n" << bindlist;

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;
}


int dnsmasqSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	OmnString contents;

	struct in_addr tmpaddr;
	char ip1Str[MAX_IP_ADDR_LEN];
	char ip2Str[MAX_IP_ADDR_LEN];
	char netmaskStr[MAX_IP_ADDR_LEN];
	char dns1Str[MAX_IP_ADDR_LEN];
	char dns2Str[MAX_IP_ADDR_LEN];
	char ipStr[MAX_IP_ADDR_LEN];

	if((sgDnsmasq.hostname != "rhc"))
	{
		contents << "<Cmd>";
		contents << "dnsmasq set name " << sgDnsmasq.hostname;
		contents << "</Cmd>\n";
	}

	if((sgDnsmasq.ip1 != 0) && (sgDnsmasq.ip2 !=0) && (sgDnsmasq.netmask !=0))
	{
		tmpaddr.s_addr= sgDnsmasq.ip1;
		strcpy(ip1Str, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= sgDnsmasq.ip2;
		strcpy(ip2Str, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= sgDnsmasq.netmask;
		strcpy(netmaskStr, inet_ntoa(tmpaddr));
		contents << "<Cmd>";
		contents << "dnsmasq set ip_range " << ip1Str << " " << ip2Str << " " << netmaskStr;
		contents << "</Cmd>\n";
	}


	if((sgDnsmasq.leaseTime != "24h"))
	{
		contents << "<Cmd>";
		contents << "dnsmasq set lease_time " << sgDnsmasq.leaseTime;
		contents << "</Cmd>\n";
	}

	if(sgDnsmasq.dns1 != 0)
	{
		if(sgDnsmasq.dns2 != 0)
		{
			tmpaddr.s_addr = sgDnsmasq.dns2;
			strcpy(dns2Str, inet_ntoa(tmpaddr));
		}
		else
			strcpy(dns2Str, "");

		tmpaddr.s_addr = sgDnsmasq.dns1;
		strcpy(dns1Str, inet_ntoa(tmpaddr));
		contents << "<Cmd>";
		contents << "dnsmasq set dns " << dns1Str << " " << dns2Str;
		contents << "</Cmd>\n";
	}

	if((sgDnsmasq.exceptDev != "outcard"))
	{
		contents << "<Cmd>";
		contents << "dnsmasq set except_dev " << sgDnsmasq.exceptDev;
		contents << "</Cmd>\n";	
	}

	struct dnsmasqHostsEntry * hptr;
	struct dnsmasqBindEntry * bptr;

	aos_list_for_each_entry(hptr, &sgDnsmasqHostsList, datalist)
	{
		if (hptr->alias != "")
		{
			tmpaddr.s_addr = hptr->ip;
			strcpy(ipStr, inet_ntoa(tmpaddr));
			contents << "<Cmd>";
			contents << "dnsmasq add host "<< hptr->alias << " " << ipStr	 << " " << hptr->domain;
			contents << "</Cmd>\n";
		}
	}

	aos_list_for_each_entry(bptr, &sgDnsmasqBindList, datalist)
	{
		if (bptr->alias != "")
		{
			tmpaddr.s_addr = bptr->ip;
			strcpy(ipStr, inet_ntoa(tmpaddr));
			contents << "<Cmd>";
			contents << "dnsmasq add bind "<< bptr->alias << " " << bptr->mac << " " << ipStr;
			contents << "</Cmd>\n";
		}
	}

	contents << "<Cmd>";
	contents << "dnsmasq "<< sgDnsmasq.status;
	contents << "</Cmd>\n";

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}

int dnsmasqClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	//char* buf = CliUtil_getBuff(data);

	sgDnsmasq.hostname = "rhc";
	sgDnsmasq.exceptDev = "outcard";
	sgDnsmasq.leaseTime = "24h";
	sgDnsmasq.ip1 = 0;
	sgDnsmasq.ip2 = 0;
	sgDnsmasq.netmask = 0;
	sgDnsmasq.dns1 = 0; 
	sgDnsmasq.dns2 = 0;
	sgDnsmasq.status = "stop";

	struct dnsmasqHostsEntry * hptr;
	struct dnsmasqHostsEntry * tmp;
	struct dnsmasqBindEntry * bptr;
	struct dnsmasqBindEntry * ttmp;
	aos_list_for_each_entry_safe(hptr, tmp, &sgDnsmasqHostsList, datalist)
	{       
		aos_list_del(&hptr->datalist);
		aos_free(hptr);
	}
	AOS_INIT_LIST_HEAD(&sgDnsmasqHostsList);
	aos_list_for_each_entry_safe(bptr, ttmp, &sgDnsmasqBindList, datalist)
	{       
		aos_list_del(&bptr->datalist);
		aos_free(bptr);
	}
	AOS_INIT_LIST_HEAD(&sgDnsmasqBindList);

	*optlen = index;
	return 0;
}


int dnsmasqSetName(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString hostname;
	OmnString rslt;
	char name[32];
	hostname = parms->mStrings[0];
	sgDnsmasq.hostname = hostname;

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
			sgDnsmasq.hostname="";
			strncpy(errmsg,rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_InvalidHostname;

		}
	}
	//cmd << "echo " << "HOSTNAME=" << "\\\"" <<"\"" << hostname << "\"" <<"\\\"" << " >" << "/etc/conf.d/hostname";
	//cout << cmd << endl;
	//OmnCliSysCmd::doShell(cmd, rslt);
	*optlen = index;
	return 0;
}


int dnsmasqSetDns(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	sgDnsmasq.dns1 = parms->mIntegers[0];
	sgDnsmasq.dns2 = parms->mIntegers[1];

	*optlen = index;
	return 0;
}


int dnsmasqAddHosts(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{

	unsigned int index = 0;
	OmnString rslt;
	char local[256];

	/*OmnString tmpAlias = parms->mStrings[0];
	if ( tmpAlias.length() > 16)
	{
		return -eAosRc_AliasTooLong;
	}*/

	struct dnsmasqHostsEntry * ptr;
	aos_list_for_each_entry(ptr, &sgDnsmasqHostsList, datalist)
	{       
		if(ptr->alias == parms->mStrings[0]) 
		{       
			strcpy(local,rslt);
			rslt = "Error: The alias already exists !";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_LabelAlreadyUsed;
		}            
	}

	struct dnsmasqHostsEntry * bind;
	if((bind = (struct dnsmasqHostsEntry *)aos_malloc(sizeof(struct dnsmasqHostsEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(bind, 0, sizeof(struct dnsmasqHostsEntry));
	bind->alias = parms->mStrings[0];
	bind->ip = parms->mIntegers[0];
	bind->domain = parms->mStrings[1];
	aos_list_add_tail((struct aos_list_head *)&bind->datalist, &sgDnsmasqHostsList);

	*optlen = index;
	return 0;
}

int dnsmasqDelHosts(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int existflag = 0;
	OmnString rslt;

	struct dnsmasqHostsEntry * ptr;
	struct dnsmasqHostsEntry * tmp;

	aos_list_for_each_entry_safe(ptr, tmp, &sgDnsmasqHostsList, datalist)
	{       
		if( ptr->alias == parms->mStrings[0] ) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}

	if ( existflag != 1 )
	{
		rslt =  "Error: The alias does not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_LabelNotFound;
	}

	*optlen = index;
	return 0;
}


int dnsmasqAddBind(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{

	unsigned int index = 0;
	OmnString rslt;
	//char local[256];

	/*OmnString tmpAlias = parms->mStrings[0];
	if ( tmpAlias.length() > 16)
	{
		return -eAosRc_AliasTooLong;
	}*/

	struct dnsmasqBindEntry * ptr;
	aos_list_for_each_entry(ptr, &sgDnsmasqBindList, datalist)
	{       
		if(ptr->alias == parms->mStrings[0]) 
		{       
			//strcpy(local,rslt);
			rslt = "Error: The alias already exists !";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_LabelAlreadyUsed;
		}            
	}

	struct dnsmasqBindEntry * bind;
	if((bind = (struct dnsmasqBindEntry *)aos_malloc(sizeof(struct dnsmasqBindEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(bind, 0, sizeof(struct dnsmasqBindEntry));
	bind->alias = parms->mStrings[0];
	bind->mac = parms->mStrings[1];
	bind->ip = parms->mIntegers[0];
	aos_list_add_tail((struct aos_list_head *)&bind->datalist, &sgDnsmasqBindList);

	*optlen = index;
	return 0;
}

int dnsmasqDelBind(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int existflag = 0;
	OmnString rslt;

	struct dnsmasqBindEntry * ptr;
	struct dnsmasqBindEntry * tmp;

	aos_list_for_each_entry_safe(ptr, tmp, &sgDnsmasqBindList, datalist)
	{       
		if( ptr->alias == parms->mStrings[0] ) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}

	if ( existflag != 1 )
	{
		rslt =  "Error: The alias does not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_LabelNotFound;
	}

	*optlen = index;
	return 0;
}


int dnsmasqSetIpRange(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;

	sgDnsmasq.ip1 = parms->mIntegers[0];
	sgDnsmasq.ip2 = parms->mIntegers[1];
	sgDnsmasq.netmask = parms->mIntegers[2];

	*optlen = index;
	return 0;
}


int dnsmasqSetLeaseTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	
	unsigned int index = 0;
	OmnString leaseTime = parms->mStrings[0];
	OmnString rslt;
	int i = leaseTime.length();
	char time[3];
	strcpy(time,leaseTime.data());
	if (( i>= 4)|| (i < 2))
	{
		rslt =  "Error: 1 The leastTime should be between 1h(m) to 59h(m)!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidLeaseTime;
	}

	if ((time[i-1] == 'm') ||(time[i-1] == 'M') ||(time[i-1] == 'h') || (time[i-1] == 'H') )
	{
	}
	else
	{
		rslt =  "Error: Units should be h,H,m,M!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidLeaseTime;

	}
	if (i == 2)
	{
		if (( time[0] <= '0' )|| time[0] > '9')
		{
			rslt =  "Error: The leastTime should be between 1h(H m M) to 59h(H m M)!";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_InvalidLeaseTime;

		}
	}
	if (i == 3)
	{
		if ((time[0] <= '0' )|| ((time[0]) > '5'))
		{
			rslt =  "Error: The leastTime should be between 1h(m) to 59h(m)!";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_InvalidLeaseTime;

		}
		if ((time[1] < '0' )||(time[1] > '9'))
		{
			rslt =  "Error: 1 The leastTime should be between 1h(m) to 59h(m)!";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_InvalidLeaseTime;

		}
	}

		sgDnsmasq.leaseTime = leaseTime;

	*optlen = index;
	return 0;
}


int dnsmasqSetExceptDev(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;

	sgDnsmasq.exceptDev = parms->mStrings[0];

	*optlen = index;
	return 0;
}

int dnsmasqGetBind(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;	
}

int dnsmasqGetBindByMac(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;
}

int dnsmasqGetLeaseTime(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;

}


int dnsmasqGetBindByLabel(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;
}

int dnsmasqGetBindByIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;
}

int dnsmasqGetDns(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;
}

int dnsmasqgetIpRange(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	return 0;
}


extern int dnsmasqShowLog(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[10000];
	OmnString cmd;
	
	cmd << "/bin/cat /var/log/messages* |grep -m 1000 DHCPOFFER\\("; 
	OmnCliSysCmd::doShell(cmd, rslt);
	cout << rslt << endl;
	strncpy(local, rslt.data(), 10000);

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;

}

int Dnsmasq_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("dnsmasq show config", dnsmasqShowConfig);
	ret |= CliUtil_regCliCmd("dnsmasq set ip_range", dnsmasqSetIpRange);
	ret |= CliUtil_regCliCmd("dnsmasq set lease_time", dnsmasqSetLeaseTime);
	ret |= CliUtil_regCliCmd("dnsmasq set except_dev", dnsmasqSetExceptDev);
	ret |= CliUtil_regCliCmd("dnsmasq set name", dnsmasqSetName);
	ret |= CliUtil_regCliCmd("dnsmasq set dns", dnsmasqSetDns);
	ret |= CliUtil_regCliCmd("dnsmasq add host", dnsmasqAddHosts);
	ret |= CliUtil_regCliCmd("dnsmasq del host", dnsmasqDelHosts);
	ret |= CliUtil_regCliCmd("dnsmasq add bind", dnsmasqAddBind);
	ret |= CliUtil_regCliCmd("dnsmasq del bind", dnsmasqDelBind);
	ret |= CliUtil_regCliCmd("dnsmasq stop", dnsmasqStop);
	ret |= CliUtil_regCliCmd("dnsmasq show log", dnsmasqShowLog);
	ret |= CliUtil_regCliCmd("dnsmasq start", dnsmasqStart);
	ret |= CliUtil_regCliCmd("dnsmasq clear config", dnsmasqClearConfig);
	ret |= CliUtil_regCliCmd("dnsmasq save config", dnsmasqSaveConfig);

	return ret;
}

