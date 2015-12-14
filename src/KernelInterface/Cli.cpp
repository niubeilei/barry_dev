////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Cli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/Cli.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Debug/Rslt.h"
#include "aosUtil/List.h"  
#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
// #include "../rhcInclude/sqlite3.h"
#include <stdio.h>
#include <stdlib.h>

#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SecuredShell/SecuredShellMgr.h"
#include "SecuredShell/ShellCommandMgr.h"
#include "SingletonClass/SingletonMgr.h"

#include <string.h>

//kevin 07/28/2006 for uname
#include <sys/utsname.h>
#include "KernelInterface_exe/version.h"

// secure
#include "CommandMan/CommandMan.h"
#include "keyman/keyman.h"
#include "../rhcInclude/openssl/md5.h"

#define MAX_INTERFACE 16

//struct aos_list_head sgFwFilterList;
//struct aos_list_head sgFwNatList;

//struct dnsProxyConfigEntry sgDnsproxy;
struct pptpConfigEntry sgPptp;
//struct wanConfigEntry sgWan;
struct pptpUserEntry pptpUser;
struct aos_list_head sgPptpUserList;
struct pppoeConfigEntry sgPppoe;
//struct dhcpConfigEntry sgDhcp;
//struct dhcpIpBindEntry dhcpIpBind;
//struct aos_list_head sgDhcpIpBindList;
//struct aos_list_head sgMacList;
struct aos_list_head sgBridgeConfigList;
//struct vlanSwitchEntry sgVlanSwitch;
//struct aos_list_head sgVlanConfigList;
// 
// Chen Ding, 02/08/2006, Changed to new location
//
static OmnString sgDefaultConfigName = "/usr/local/AOS/Data/current";
//static OmnString sgDefaultConfigName = "/usr/local/AOS/Data/Config/current";

// kevin, 07/18/2006 
//static OmnString sgDefaultConfigName = "/usr/local/AOS/Config/current";
//static OmnString sgDefaultFirewallConfigName = "/usr/local/AOS/Data/Config/firewall";

// kevin, 08/10/2006, for HAC 
// static OmnString sgDefaultConfigName = "/usr/local/keyou/Config/current";

static char *sgModuleNames[] = 
{
	"system",
	"app proxy",
	"forward table",
	"deny page",
	"cert manager",
	"cert chain",
//	"usbkey",
	"ocsp",
	"amm",
	"ssl",
//	"util",
	"firewall",
	"dns proxy",
	"dhcp server",
	"pppoe",
	"pptp",
	"qos",
	""
};


// kevin, 08/10/2006, for HAC
//static char *sgModuleNames[] = 
//{
//	"app proxy",
//	""
//};

//kevin, 07/10/2006
struct ifconfig
{
	OmnString interfaceAlias;
	OmnString interface;
	OmnString ip;
	OmnString mask;
} sgIfConfig[MAX_INTERFACE];

struct systemConfigListEntry
{
	OmnString type;
	OmnString interface;
	OmnString ip;
	OmnString mask;
	OmnString dns;
	OmnString dip;
	OmnString dmask;
	OmnString gateway;
	struct aos_list_head datalist;
}; 

struct aos_list_head sgSystemConfigList;
bool	sgListInited = false;
			
//firewall config functions

        int aos_Generate_MD5(const char * , char *);


AosCli::AosCli()
{
	if(!sgListInited)
	{
		AOS_INIT_LIST_HEAD(&sgSystemConfigList);
		AOS_INIT_LIST_HEAD(&sgFwFilterList);
		AOS_INIT_LIST_HEAD(&sgFwNatList);
		// Kevin Yang, 11/17/2006
		//init the other struct: such as the dnsproxy, pppoe, pptp, dhcp etc
		sgDnsproxy.ip = 0;
		sgDnsproxy.lIp = 0;
		sgDnsproxy.dev = "incard";
		sgDnsproxy.global = "no";
		sgDnsproxy.name = 0;
		sgDnsproxy.status = "stop";
		sgPppoe.username= 0;
		sgPppoe.password= 0;
		sgPppoe.dns1= 0;
		sgPppoe.dns2= 0;
		sgPppoe.status= "stop";

		sgDhcp.dft_leaseTime = "600";
		sgDhcp.ip1 = 0;
		sgDhcp.ip2 = 0;
		sgDhcp.dns = 0;
		sgDhcp.router = 0;
		sgDhcp.status = "stop";

		sgPptp.ipLocal= 0;
		sgPptp.ip1= 0;
		sgPptp.ip2= 0;
		sgPptp.status= "stop";
		
		AOS_INIT_LIST_HEAD(&sgMacList);
		AOS_INIT_LIST_HEAD(&sgPptpUserList);
		AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
		sgListInited = true;
		
		AOS_INIT_LIST_HEAD(&sgBridgeConfigList);
	}
}

AosCli::AosCli(const OmnString &fn)
:
mFileName(fn)
{
	if(!sgListInited)
	{
		AOS_INIT_LIST_HEAD(&sgSystemConfigList);
		AOS_INIT_LIST_HEAD(&sgFwFilterList);
		AOS_INIT_LIST_HEAD(&sgFwNatList);
		// Kevin Yang, 11/17/2006
		//init the other struct: such as the dnsproxy, pppoe, pptp, dhcp etc
		sgDnsproxy.ip = 0;
		sgDnsproxy.lIp = 0;
		sgDnsproxy.dev = "incard";
		sgDnsproxy.global = "no";
		sgDnsproxy.name = 0;
		sgDnsproxy.status = "stop";
		sgPppoe.username= 0;
		sgPppoe.password= 0;
		sgPppoe.dns1= 0;
		sgPppoe.dns2= 0;
		sgPppoe.status= "stop";

		sgDhcp.dft_leaseTime = "600";
		sgDhcp.ip1 = 0;
		sgDhcp.ip2 = 0;
		sgDhcp.dns = 0;
		sgDhcp.router = 0;
		sgDhcp.status = "stop";

		sgPptp.ipLocal= 0;
		sgPptp.ip1= 0;
		sgPptp.ip2= 0;
		sgPptp.status= "stop";
		
		AOS_INIT_LIST_HEAD(&sgMacList);
		AOS_INIT_LIST_HEAD(&sgPptpUserList);

		AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
		sgListInited = true;
		
		AOS_INIT_LIST_HEAD(&sgBridgeConfigList);
	}

	if (fn.length() == 0)
	{
		mFileName = sgDefaultConfigName;
	}
}


int 
AosCli::saveConfig(OmnString &rslt)
{
	if (!mFile)
	{
		OmnString fn = mFileName;
		fn << ".active";
		mFile = OmnNew OmnFile(fn, OmnFile::eCreate);
		if (!mFile || !mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return aos_alarm(eAosMD_Platform, eAosAlarm_FailedToOpenFile, 
				"%s", rslt.data());
		}
	}

	//
	// Configurations are saved in blocks, one for each module.
	// Module names are defined in sgModuleNames[].
	//
	int ret;
	int index = 0;
	OmnString err;
	while (strlen(sgModuleNames[index]) != 0)
	{
		if (saveConfig(sgModuleNames[index], rslt))
		{
			// 
			// Failed to save the configuration.
			//
			err << rslt << "\n";
			ret = -1;
			// mFile->closeFile();
			// mFile = 0;
			// return ret;
		}

		index++;
	}

	// 
	// Back up the current config
	//
	OmnFile file(mFileName, OmnFile::eReadOnly);
	OmnString cmd;
	if (file.isGood())
	{
		OmnString newFn = mFileName;
		newFn << ".bak";
		cmd << "cp " << mFileName << " " << newFn;
OmnTrace << "To run command: " << cmd << endl;
		system(cmd);
	}

	mFile->closeFile();
	mFile = 0;

	cmd = "mv -f ";

	cmd << mFileName << ".active " << mFileName;
	system(cmd);
OmnTrace << "To run command: " << cmd << endl;

	return 0;
}


int 
AosCli::saveConfig(const char *moduleName, OmnString &rslt)
{
	// 
	// Config files are organized in blocks:
	//
	// ------AosCliBlock: <blockname>------
	// <AosCliCmd> ... </AosCliCmd> 
	// <AosCliCmd> ... </AosCliCmd> 
	// ...
	// ------EndAosCliBlock------
	//	
	// ...
	// 
	// This function saves one block identified by 'moduleName'.
	//

	OmnString cmd = moduleName;
	cmd << " save config";
	int ret = 0;

	OmnString contents;

	//kevin, 07/10/2006
	
	if (cmd == "system save config")
	{
		saveSystemConfig(rslt);
		contents = rslt;
	}
	else if (cmd == "firewall save config")
	{
		saveFwConfig(rslt);
		contents = rslt;
	}
	else
	{
		if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
		{
			rslt = "Failed to save config for module: ";
			rslt << moduleName;
			ret = -eAosAlarm_FailedToSaveConfig;
			contents = "";
		}
		else
		{
			contents = rslt;
		}
	}

	char local[200];
	sprintf(local, "------AosCliBlock: %s------\n", moduleName);
	if (!mFile->put(local, false) ||
		!mFile->put("<Commands>\n", false) ||
	    !mFile->put(contents, false) ||
		!mFile->put("</Commands>\n", false) ||
		!mFile->put("------EndAosCliBlock------\n\n", false))
	{
		rslt = "Failed to write to config: ";
		rslt << mFileName;
		ret = aos_alarm(eAosMD_Platform, eAosAlarm_FailedToWriteConfig,
			"%s", rslt.data());
	}

	return ret;
}


// 
// This function loads configurations from the configure
// file 'mFileName'. The caller should have set the file
// name already. Configurations are organized in blocks, 
// one for each module. If one module fails to load the
// configuration, it will abort the operation. 
//
// If errors occur, the error messages are returned through
// 'rslt'.
//
int 
AosCli::loadConfig(OmnString &rslt)
{
	if (!mFile)
	{
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}
	}

	// 
	// Configurations are saved in blocks, one for each module.
	// These are stored in "sgModuleNames[]". 
	//
	int ret;
	int index = 0;

	OmnCliCmd::setLoadConfig(true);
	while (sgModuleNames[index][0] != 0)
	{
		//kevin 07/10/2006
		/*
		if(strcasecmp(sgModuleNames[index],"firewall") == 0)
		{
			//cout << "enter load firewall config" << endl;
			loadFwConfig(rslt);
		}
		else if(strcasecmp(sgModuleNames[index],"system") == 0)
		{
			//cout << "enter load system config" << endl;
		}
		*/
		if ((ret = loadConfig(sgModuleNames[index], rslt))) 
		{
			// 
			// Failed to load the configure for the module. 
			//
			// mFile->closeFile();
			// mFile = 0;
			// OmnCliCmd::setLoadConfig(false);
			// return ret;
		}

		index++;
	}

	OmnCliCmd::setLoadConfig(false);
	mFile->closeFile();
	mFile = 0;

	return 0;
}


int 
AosCli::loadConfig(const char *moduleName, OmnString &rslt)
{
	// 
	// Config files are organized in blocks:
	//
	// ------AosCliBlock: <blockname>------
	// <AosCliCmd> ... </AosCliCmd> 
	// <AosCliCmd> ... </AosCliCmd> 
	// ...
	// ------EndAosCliBlock------
	//	
	// ...
	// 
	// This function loads the block identified by 'moduleName'.
	//
	
	OmnString contents;
	OmnString start = "------AosCliBlock: ";
	start << moduleName << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << moduleName << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		// 
		// Clear the module's config first
		//
		OmnString cmd = moduleName;
		cmd << " clear config";

		//kevin 07/10/2006
		if (cmd == "firewall clear config")
		{
			clearFwConfig(rslt);
			//cout << "The old firewall config be cleared!" << endl;
		}
		else if (cmd == "system clear config")
		{
			//clearSystemConfig(rslt);
			//cout << "The old system config be cleared!" << endl;
		}
		else if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
		{
			cout << "Failed to clear the module's config: " << cmd 
				<< ". Error: " << rslt << endl;
		//	return -eAosAlarm_FailedToLoadConfig;
		}
		OmnXmlItem config(contents);
		config.reset();
		while (config.hasMore())
		{
			OmnXmlItemPtr item = config.next();
			try
			{
				cmd = item->getStr();
				cout << "Found command: " << cmd << endl;
				//kevin 07/10/2006
				
				if(strcasecmp(moduleName,"firewall") == 0)
				{
					fwRunCommand(cmd, rslt);
				}
				else if(strcasecmp(moduleName,"system") == 0)
				{
					systemRunCommand(cmd, rslt);
				}
				else if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
				{
					cout << "Failed to run command: " << cmd 
						<< ". Error: " << rslt << endl;
				}
			}

			catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				return -eAosAlarm_FailedToLoadConfig;
			}
		}
	}

	catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " 
			<< moduleName << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	return 0;
}


int 
AosCli::clearConfig(OmnString &rslt)
{
	//
	// Configurations are saved in blocks, one for each module.
	// Module names are defined in sgModuleNames[].
	//
	int ret;
	int index = 0;
	while (strlen(sgModuleNames[index]) != 0)
	{
		if ((ret = clearConfig(sgModuleNames[index], rslt))) 
		{
			// 
			// Failed to clear the configuration.
			//
			return ret;
		}

		index++;
	}
	return 0;
}


int 
AosCli::clearConfig(const char *moduleName, OmnString &rslt)
{
	// this function clear all the module's configuration

	OmnString cmd = moduleName;
	cmd << " clear config";

	if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
	{
		if (rslt.length())
			rslt << "\nFailed to clear config:";
		else
			rslt = "Failed to clear config:";
		rslt << moduleName;
		return -eAosAlarm_FailedToSaveConfig;
		}
	//cout << "clear " << moduleName << " configuration" << endl;
	return 0;
}

	
// kevin 07/12/2006
// system modules config
int 
AosCli::systemRunCommand(OmnString &cmd, OmnString &rslt)
{
	OmnString ip;
	OmnString mask;
	OmnString dip;
	OmnString dmask;
	OmnString dns;
	OmnString interface;
	OmnString gateway;	

	OmnString contents, systemPrefix, subPrefix;
	//bool err;

	int curPos = 0;
	curPos = cmd.getWord(curPos,systemPrefix);
	if (systemPrefix == "system")
	{		
		curPos = cmd.getWord(curPos,subPrefix);
		if(subPrefix == "ip")
		{
			curPos = cmd.getWord(curPos,subPrefix);
			if(subPrefix == "address")
			{
				curPos = cmd.getWord(curPos,interface);
				curPos = cmd.getWord(curPos,ip);
				curPos = cmd.getWord(curPos,mask);
				systemIpAddress(interface, ip, mask, rslt);
			}
			else if(subPrefix == "dns")
			{
				curPos = cmd.getWord(curPos,dns);
				systemDns(dns, rslt);
			}
			else
			{
				cout << "Error:The system ip SubPrefix is wrong!" << endl;
			}
		}
		else if (subPrefix == "route")
		{
			curPos = cmd.getWord(curPos,dip);
			curPos = cmd.getWord(curPos,dmask);
			curPos = cmd.getWord(curPos,gateway);
			systemRouteCommon(dip, dmask, gateway, rslt);
		}
		else if (subPrefix == "default")
		{
			curPos = cmd.getWord(curPos,subPrefix);
			if (subPrefix == "route")
			{
				curPos = cmd.getWord(curPos,gateway);
				systemRouteDefault(gateway, rslt);
			}
			else 
			{
				cout << "Error:The second SubPrefix is wrong!" << endl;
			}
		}
		else
		{	
			cout << "Error:The first SubPrefix is wrong!" << endl;
		}
	}
	else
	{
		cout << "Error:This is a bad system command!" << endl;
	}
	return 0;
}

	
int 
AosCli::saveSystemConfig(OmnString &rslt)
{
	OmnString contents;
	struct systemConfigListEntry * ptr;
	
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if (ptr->type == "ipaddress")
		{
			contents << "<Cmd>system ip address " << ptr->interface << " ";
			contents << ptr->ip << " " << ptr->mask << "</Cmd>\n";
		}
		else if (ptr->type == "dns")
		{
			contents << "<Cmd>system ip dns " << ptr->ip << "</Cmd>\n";
		}
		else if (ptr->type == "routecommon")
		{
			contents << "<Cmd>system route ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->gateway << "</Cmd>\n";
		}
		else if (ptr->type == "routedefault")
		{
			contents << "<Cmd>system default route " << ptr->gateway << "</Cmd>\n";
		}
	}
	
	rslt = contents;
	
	return 0;
}


int 
AosCli::loadSystemConfig(OmnString &rslt)
{
	
	return 0;
}
	

int 
AosCli::clearSystemConfig(OmnString &rslt)
{
	
	return 0;
}

	
int 
AosCli::showSystemConfig(OmnString &rslt)
{
	//
	// Show the system current configurations 
	//
	OmnString contents;
	struct systemConfigListEntry * ptr;
	
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if (ptr->type == "ipaddress")
		{
			contents << "<Cmd>system ip address " << ptr->interface << " ";
			contents << ptr->ip << " " << ptr->mask << "</Cmd>\n";
		}
		else if (ptr->type == "dns")
		{
			contents << "<Cmd>system ip dns " << ptr->dns << "</Cmd>\n";
		}
		else if (ptr->type == "routecommon")
		{
			contents << "<Cmd>system route ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->gateway << "</Cmd>\n";
		}
		else if (ptr->type == "arpproxy")
		{
			contents << "<Cmd>system arp proxy add " << ptr->interface << "</Cmd>\n";
		}
		else if (ptr->type == "routedefault")
		{
			contents << "<Cmd>system default route " << ptr->gateway << "</Cmd>\n";
		}
	}
	
	cout << "------config : system------\n" << contents;
	
	return 0;
}


int 
AosCli::systemUname(OmnString &rslt)
{
	OmnString id;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;

	curPos = mFileName.getWord(curPos,id);

	//check the valid
	if((id != "sysname") && (id != "nodename") && (id != "release") && \
	 (id != "machine") && (id != "domainname") && (id != "version") && \
	 (id != "all"))
	{
		cout << "Error: The Uname ID is invalid!" << endl;
		return -1;
	}
	
	systemUname(id, rslt);

	return 0;
}
	

int 
AosCli::systemUname(OmnString &id, OmnString &rslt)
{
	struct utsname name;

	OmnString systemCmd;
	OmnString sysname;
	bool result;

	if (uname(&name) == -1)
	{
		cout << "Error: Cannot get uname!\n";
	}

	if(0==access("/etc/gentoo-release",R_OK))
	{
		OmnFile fp = OmnFile("/etc/gentoo-release",OmnFile::eReadOnly);
		sysname = fp.getLine(result);
		fp.closeFile();
	}
	else if(0==access("/etc/debian_version",R_OK))
	{
		OmnFile fp = OmnFile("/etc/debian_version",OmnFile::eReadOnly);
		sysname << "Debian " << fp.getLine(result);
		fp.closeFile();
	}
	else if(0==access("/etc/redhat-release",R_OK))
	{
		OmnFile fp = OmnFile("/etc/redhat-release",OmnFile::eReadOnly);
		sysname = fp.getLine(result);
		fp.closeFile();
	}
	else 
	{
		sysname="Other Linux";
	}

	if (id == "all")
	{
		cout << "The Ver & Build No :" << RELEASE << endl;
		cout << "The OS Name        :" << sysname << endl;
		cout << "The Node Name      :" << name.nodename << endl;
		cout << "The Machine Name   :" << name.machine << endl;
		cout << "The Kernel Release :" << name.release << endl;
		cout << "The Kernel Ver Info:" << name.version << endl;
		cout << "The Domain Name    :" << name.domainname << endl;
	}
	else if (id == "sysname") 
	{
		cout << "The OS Name is :" << sysname << endl;
	}
	else if (id == "nodename") 
	{
		cout << "The Node Name is :" << name.nodename << endl;
	}
	else if (id == "release") 
	{
		cout << "The Kernel Release is :" << name.release << endl;
	}
	else if (id == "machine") 
	{
		cout << "The Machine Name is :" << name.machine << endl;
	}
	else if (id == "domainname") 
	{
		cout << "The Domain Name is :" << name.domainname << endl;
	}
	else if (id == "version") 
	{
		cout << "The Kernel Version Info is :" << name.version << endl;
	}
	else
	{
		cout << "Error: The Uname ID is invalid!" << endl;
		return -1;
	}

	return 0;
}


int 
AosCli::systemArpProxyAdd(OmnString &rslt)
{	
	OmnString interface;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,interface);

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	
	systemArpProxyAdd(interface, rslt);

	return 0;
}
	

int 
AosCli::systemArpProxyAdd(OmnString &interface, OmnString &rslt)
{
	OmnString systemCmd;

	//check the current config
	struct systemConfigListEntry * ptr;
	//struct systemConfigListEntry * nptr;	
	int flag = 0;
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if ((ptr->type == "arpproxy") && (ptr->interface == interface))
		{
			ptr->interface = interface;
			flag = 1;
			break;
		}
	}	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	if (flag != 1)
	{
		struct systemConfigListEntry * data;
		if((data = (struct systemConfigListEntry *)aos_malloc(sizeof(struct systemConfigListEntry)))==NULL)
		{
			//printk("malloc error");
			return -1;
		}
		memset(data, 0, sizeof(struct systemConfigListEntry));
		data->type = "arpproxy";
		data->interface = interface;
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSystemConfigList);
	}
	if (interface=="incard")
	{
		systemCmd << "/bin/echo 1 > /proc/sys/net/ipv4/conf/eth1/proxy_arp";
	}
	else 
	{
		systemCmd << "/bin/echo 1 > /proc/sys/net/ipv4/conf/eth0/proxy_arp";
	}
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}



int 
AosCli::systemIpAddress(OmnString &rslt)
{
	OmnString interface;
	OmnString ip;
	OmnString mask;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,ip);
	curPos = mFileName.getWord(curPos,mask);

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if(OmnIpAddr(ip) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The IP is invalid!" << endl;
		return -1;
	}
	if(OmnIpAddr(mask) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The netmask is invalid!" << endl;
		return -1;
	}
	
	systemIpAddress(interface, ip, mask, rslt);

	return 0;
}
	

int 
AosCli::systemIpAddress(OmnString &interface, OmnString &ip, OmnString &mask, OmnString &rslt)
{
	OmnString systemCmd;

	//check the current config
	struct systemConfigListEntry * ptr;
	//struct systemConfigListEntry * nptr;	
	int flag = 0;
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if ((ptr->type == "ipaddress") && (ptr->interface == interface))
		{
			ptr->ip = ip;
			ptr->mask = mask;
			flag = 1;
			break;
		}
	}	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	if (flag != 1)
	{
		struct systemConfigListEntry * data;
		if((data = (struct systemConfigListEntry *)aos_malloc(sizeof(struct systemConfigListEntry)))==NULL)
		{
			//printk("malloc error");
			return -1;
		}
		memset(data, 0, sizeof(struct systemConfigListEntry));
		data->type = "ipaddress";
		data->interface = interface;
		data->ip = ip;
		data->mask = mask;
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSystemConfigList);
	}
	if (interface=="incard")
	{
		systemCmd << "/sbin/ifconfig eth1 ";
	}
	else 
	{
		systemCmd << "/sbin/ifconfig eth0 ";
	}
	systemCmd << " " << ip << " netmask " << mask;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}


int 
AosCli::systemDns(OmnString &rslt)
{
	OmnString dns;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;

	curPos = mFileName.getWord(curPos,dns);

	//check the valid
	if(OmnIpAddr(dns) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The DNS IP is invalid!" << endl;
		return -1;
	}

	systemDns(dns, rslt);

	return 0;
}
	

int 
AosCli::systemDns(OmnString &dns, OmnString &rslt)
{
	OmnString systemCmd;

	//check the current config
	struct systemConfigListEntry * ptr;
	//struct systemConfigListEntry * nptr;	
	int flag = 0;
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if (ptr->type == "dns") 
		{
			ptr->dns = dns;
			flag = 1;
			break;
		}
	}	
	if (flag != 1)
	{
		struct systemConfigListEntry * data;
		if((data = (struct systemConfigListEntry *)aos_malloc(sizeof(struct systemConfigListEntry)))==NULL)
		{
			//printk("malloc error");
			return -1;
		}
		memset(data, 0, sizeof(struct systemConfigListEntry));
		data->type = "dns";
		data->dns = dns;
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSystemConfigList);
	}
	systemCmd << "/bin/echo nameserver " << dns << " > /etc/resolv.conf";
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}


int 
AosCli::systemRouteCommon(OmnString &rslt)
{
	OmnString dip;
	OmnString dmask;
	OmnString gateway;
	int curPos = 0;
//	int len = 0;
//	int value = 0;
//	int flag = 0;

	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,gateway);

	//check the valid
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The IP is invalid!" << endl;
		return -1;
	}
	if(OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The netmask is invalid!" << endl;
		return -1;
	}
	if((gateway != "incard") && (gateway != "outcard") && \
	 (OmnIpAddr(gateway) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The Gateway is invalid!" << endl;
		return -1;
	}
	
	systemRouteCommon(dip, dmask, gateway, rslt);

	return 0;
}
	

int 
AosCli::systemRouteCommon(OmnString &dip, OmnString &dmask, OmnString &gateway, OmnString &rslt)
{
	OmnString systemCmd;

	//check the current config
	struct systemConfigListEntry * ptr;
	//struct systemConfigListEntry * nptr;	
	int flag = 0;
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if ((ptr->type == "routecommon") && (ptr->dip == dip) && (ptr->dmask == dmask)) 
		{
			ptr->gateway = gateway;
			flag = 1;
			break;
		}
	}	
	if (flag != 1)
	{
		struct systemConfigListEntry * data;
		if((data = (struct systemConfigListEntry *)aos_malloc(sizeof(struct systemConfigListEntry)))==NULL)
		{
			//printk("malloc error");
			return -1;
		}
		memset(data, 0, sizeof(struct systemConfigListEntry));
		data->type = "routecommon";
		data->dip = dip;
		data->dmask = dmask;
		data->gateway = gateway;
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSystemConfigList);
	}
	if (gateway == "incard")
	{
		systemCmd << "/sbin/route add -net " << dip << " netmask " << dmask << " dev eth1";
	}
	else if (gateway == "outcord")
	{
		systemCmd << "/sbin/route add -net " << dip << " netmask " << dmask << " dev eth0";
	}
	else 
	{
		systemCmd << "/sbin/route add -net " << dip << " netmask " << dmask << " gw " << gateway;
	}
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}
	

int 
AosCli::systemRouteDefault(OmnString &rslt)
{
	OmnString gateway;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;

	curPos = mFileName.getWord(curPos,gateway);

	//check the valid
	if(OmnIpAddr(gateway) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The Gateway is invalid!" << endl;
		return -1;
	}

	systemRouteDefault(gateway, rslt);

	return 0;
}
	

int 
AosCli::systemRouteDefault(OmnString &gateway, OmnString &rslt)
{
	OmnString systemCmd;

	//check the current config
	struct systemConfigListEntry * ptr;
	//struct systemConfigListEntry * nptr;	
	int flag = 0;
	aos_list_for_each_entry(ptr, &sgSystemConfigList, datalist)
	{
		if (ptr->type == "routedefault") 
		{
			ptr->gateway = gateway;
			flag = 1;
			break;
		}
	}	
	if (flag != 1)
	{
		struct systemConfigListEntry * data;
		if((data = (struct systemConfigListEntry *)aos_malloc(sizeof(struct systemConfigListEntry)))==NULL)
		{
			//printk("malloc error");
			return -1;
		}
		memset(data, 0, sizeof(struct systemConfigListEntry));
		data->type = "routedefault";
		data->gateway = gateway;
		aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgSystemConfigList);
	}
	systemCmd << "/sbin/route add default gw " << gateway;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}
	

//xia	
int 
AosCli::systemUpdate(OmnString &rslt)
{
	OmnString url;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,url);

	systemUpdate(url, rslt);

	return 0;
}


int 
AosCli::systemUpdate(OmnString &url, OmnString &rslt)
{
	OmnString systemCmd;
	
	systemCmd << "/usr/local/AOS/Bin/system_update.pl " << url;
	OmnCliSysCmd::doShell(systemCmd, rslt);
	
	return 0;
}


int 
AosCli::systemArpProxyDel(OmnString &rslt)
{
	OmnString interface;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;

	curPos = mFileName.getWord(curPos,interface);

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	
	OmnString systemCmd;
				
	struct systemConfigListEntry * ptr;
    struct systemConfigListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgSystemConfigList, datalist)
    {       
    	if(	(ptr->type == "arpproxy") && (ptr->interface == interface)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
    if (existflag == 0) 
    {   
        cout << "Error: The rule not exist !" << endl;
		return -1;
    }
    else if (interface=="incard")
	{
		systemCmd << "/bin/echo 0 > /proc/sys/net/ipv4/conf/eth1/proxy_arp";
	}
	else 
	{
		systemCmd << "/bin/echo 0 > /proc/sys/net/ipv4/conf/eth0/proxy_arp";
	}
    OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}
	

int 
AosCli::systemDelRouteCommon(OmnString &rslt)
{
	OmnString dip;
	OmnString dmask;
	OmnString gateway;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;

	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,gateway);

	//check the valid
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The IP is invalid!" << endl;
		return -1;
	}
	if(OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The netmask is invalid!" << endl;
		return -1;
	}
	if((gateway != "incard") && (gateway != "outcard") && \
	 (OmnIpAddr(gateway) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The Gateway is invalid!" << endl;
		return -1;
	}
	
	OmnString systemCmd;
				
	struct systemConfigListEntry * ptr;
    struct systemConfigListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgSystemConfigList, datalist)
    {       
    	if(	(ptr->type == "routecommon") && (ptr->dip == dip) \
			&& (ptr->dmask == dmask) && (ptr->gateway == gateway)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
    if (existflag == 0) 
    {   
        cout << "Error: The rule not exist !" << endl;
		return -1;
    }
    else if (gateway == "incard")
	{
		systemCmd << "/sbin/route delete -net " << dip << " netmask " << dmask << " dev eth1";
	}
	else if (gateway == "outcord")
	{
		systemCmd << "/sbin/route delete -net " << dip << " netmask " << dmask << " dev eth0";
	}
	else 
	{
		systemCmd << "/sbin/route delete -net " << dip << " netmask " << dmask << " gw " << gateway;
	}
    OmnCliSysCmd::doShell(systemCmd, rslt);

	return 0;
}
	

int 
AosCli::systemDelRouteDefault(OmnString &rslt)
{
	OmnString gateway;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,gateway);

	//check the valid
	if(OmnIpAddr(gateway) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The Gateway is invalid!" << endl;
		return -1;
	}
	
	OmnString systemCmd;
				
	struct systemConfigListEntry * ptr;
    struct systemConfigListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgSystemConfigList, datalist)
    {       
    	if(	(ptr->type == "routedefault") && (ptr->gateway == gateway)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
    if (existflag == 0) 
    {   
        cout << "Error: The rule not exist !" << endl;
		return -1;
    }
    else
    {
		systemCmd << "/sbin/route delete default gw " << gateway;
        OmnCliSysCmd::doShell(systemCmd, rslt);
    }	

	return 0;
}
	
//end kevin

//dxr,11/27/2006
int 
AosCli::systemRouteShow(OmnString &rslt)
{
	OmnString systemCmd;
	systemCmd << "/sbin/route -n";
    OmnCliSysCmd::doShell(systemCmd, rslt);
	return 0;
}

int 
AosCli::systemDevRoute(OmnString &rslt)
{
	OmnString dip;
	OmnString dmask;
	OmnString dev;
	int curPos = 0;
	
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dev);

	if(dev == "incard")
	{
		dev = "eth1";
	}

	else if(dev == "outcard")
	{
		dev = "eth0";
	}
	
	else
	{
		dev ="eth0";
	}
	
	OmnString systemCmd;
	systemCmd << "/sbin/route add -net " << dip <<" netmask " << dmask <<" dev " << dev;
    OmnCliSysCmd::doShell(systemCmd, rslt);
	return 0;
}

int 
AosCli::systemDelDevRoute(OmnString &rslt)
{
	OmnString dip;
	OmnString dmask;
	OmnString dev;
	int curPos = 0;
	
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dev);

	if(dev == "incard")
	{
		dev = "eth1";
	}

	else if(dev == "outcard")
	{
		dev = "eth0";
	}
	
	else
	{
		dev ="eth0";
	}
	
	OmnString systemCmd;
	systemCmd << "/sbin/route del -net " << dip <<" netmask " << dmask <<" dev " << dev;
    OmnCliSysCmd::doShell(systemCmd, rslt);
	return 0;
}


// dxr, 12/06/2006
int 
AosCli::secureAuthcmdPolicy(OmnString &rslt)
{
	// 
	// Chen Ding, 02/15/2007
	//
	static OmnStoreMgr *slStoreMgr = OmnStoreMgr::getSelf();
 	OmnDataStorePtr store = slStoreMgr->getStore(OmnStoreId::eGeneral);
	if (!store)
	{
		OmnAlarm << "Database is not available" << enderr;
		return -1;
	}

	// sqlite3 *db=NULL;
	// char *zErrMsg = 0;

	// int rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	// if( rc )
	// {
	// 	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	// 	sqlite3_close(db);
	// 	return -1;
	// }
	OmnString para;
	int curPos = 0;
	curPos = mFileName.getWord(curPos,para);

	char *sql;
	
	if(para == "accept")
	{
	    sql ="delete from policy;";
		// sqlite3_exec(db,sql,0,0,&zErrMsg);
		OmnRslt r = store->runSQL(sql);
		if (!r)
		{
			OmnAlarm << "Failed to run SQL: " << sql << enderr;
			return -1;
		}

		sql ="insert into \"policy\" values (\"accept\");";
		// sqlite3_exec(db,sql,0,0,&zErrMsg);
		r = store->runSQL(sql);
		if (!r)
		{
			OmnAlarm << "Failed to run SQL: " << sql << enderr;
			return -1;
		}
	}
	else if(para == "deny")
	{
	    sql ="delete from policy;";
		// sqlite3_exec(db,sql,0,0,&zErrMsg);
		OmnRslt r = store->runSQL(sql);
		if (!r)
		{
			OmnAlarm << "Failed to run SQL: " << sql << enderr;
			return -1;
		}

		sql ="insert into \"policy\" values (\"deny\");";
		// sqlite3_exec(db,sql,0,0,&zErrMsg);
		r = store->runSQL(sql);
		if (!r)
		{
			OmnAlarm << "Failed to run SQL: " << sql << enderr;
			return -1;
		}
	}
	else
	{
		cout << "The param is wrong" <<endl;
		return -1;
	}
	
	// sqlite3_close(db);
	return 0;
}

int 
AosCli::secureAuthcmdCommandAdd(OmnString &rslt)
{
	static OmnStoreMgr *slStoreMgr = OmnStoreMgr::getSelf();
 	OmnDataStorePtr store = slStoreMgr->getStore(OmnStoreId::eGeneral);
	if (!store)
	{
		OmnAlarm << "Database is not available" << enderr;
		return -1;
	}

	OmnString fileName;
	int curPos = 0;
	curPos = mFileName.getWord(curPos, fileName);
	
	// sqlite3 *db=NULL;
	// char *zErrMsg = 0;
	char sql[1024];
	char md5[33];

	// int rc;
    int ret = aos_Generate_MD5(fileName.data(), md5);

	// rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	// if( rc )
	// {
	// 	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	// 	sqlite3_close(db);
	// 	return -1;
	// }
		
	if(ret == -1)
	{
		cout << "No such command!" << endl;
		return -1;
	}
	
	sprintf(sql, "insert into cmdtable values (\"%s\",\"%s\");",fileName.data(), md5);
	OmnRslt r = store->runSQL(sql);
	if (!r)
	{
		OmnAlarm << "Failed to run the sql: " << sql 
			<< r.toString() << enderr;
		return -1;
	}

	// sqlite3_exec(db, sql,0,0,&zErrMsg);
	// sqlite3_close(db);
	return 0;
}

int 
AosCli::secureAuthcmdCommandDel(OmnString &rslt)
{	
/* Chen Ding, 02/15/2007
	OmnString fileName;
	int curPos = 0;
	curPos = mFileName.getWord(curPos, fileName);
	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char sql[1024];

	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	sprintf(sql, "delete from cmdtable where filename =\"%s\";", fileName.data());
	sqlite3_exec(db, sql,0,0,&zErrMsg);

	sqlite3_close(db);
*/
	return 0;
}

int 
AosCli::secureAuthcmdCommandShow(OmnString &rslt)
{
/* Chen Ding, 02/15/2007
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char *sql;
	int nrow = 0, ncolumn = 0;
	char **azResult; 
	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
		//sprintf(sql, "select * from cmdtable;");
		//sql << fileName << "\");";
		//sqlite3_exec(db, sql, 0, 0,&zErrMsg);
		//printf("The Command List\n-------------------\n%s\n", rst);
	sql = "select * from cmdtable;";
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg );
	
	int i = 0 ;
	printf( "The Command List\n------------------------\n" );
	for( i=0 ; i<( nrow + 1 ) * ncolumn ; i=i+2 )
	{	
		printf( "%s\t", azResult[i] );
		printf( "%s\n", azResult[i+1] );
	}	
	
	sql = "select \"policy\" from policy; ";
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg );
	sqlite3_get_table(db,sql,&azResult,&nrow,&ncolumn,&zErrMsg );
	printf( "\n" );
	printf( "The Policy\n------------------------\n" );
	printf( "%s",azResult[1] );

	sqlite3_free_table( azResult );
	sqlite3_close(db);
*/
	return 0;
}

int 
AosCli::secureAuthcmdCommandReset(OmnString &rslt)
{
/*
	sqlite3 *db=NULL;
	char *zErrMsg = 0;

	int rc;

	rc = sqlite3_open("../aos_secure/securecmd.db", &db);
	if( rc )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	char *sql;
	
	sql ="delete from cmdtable;";
	sqlite3_exec(db,sql,0,0,&zErrMsg);
	
	sqlite3_close(db);
*/
	return 0;
}

//jzz , 11/20/2006
//
//knocking on SecuredShellMgr
const static OmnString sgSecuredShellConfigFileName = "config.txt";

static  OmnString   sgUsername = "";

bool
AosCli::startSecuredShell(OmnString &rslt)
{
	AosSecuredShellMgr shellMgr;

	//
	// secured shell start <username> <shell_id> <password> <duration>
	//
	int curPos = 0;

	OmnString username,shellId, password, temp;

	int duration;


//	username = sgUsername;
	curPos = mFileName.getWord(curPos, shellId);
	curPos = mFileName.getWord(curPos, password);
	curPos = mFileName.getWord(curPos, username);
	curPos = mFileName.getWord(curPos, temp);

	if(username=="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	if(temp!="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	bool ret = shellMgr.startSecuredShell(username,shellId,password,duration,rslt);
	if(ret == false)
		return false;
	return true;
}

bool
AosCli::stopSecuredShell(OmnString &rslt)
{
	AosSecuredShellMgr shellMgr;

	//
	// secured shell start <username> <shell_id> <password> <duration>
	//
	int curPos = 0;

	OmnString username,shellId, password,temp;


	//username = sgUsername;
	curPos = mFileName.getWord(curPos, shellId);
	curPos = mFileName.getWord(curPos, password);
	//curPos = mFileName.getWord(curPos, duration);
	curPos = mFileName.getWord(curPos, username);
	curPos = mFileName.getWord(curPos, temp);

	if(username=="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	if(temp!="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}

	bool ret = shellMgr.stopSecuredShell(username,shellId,password,rslt);
	return ret;
}

bool
AosCli::username(OmnString &rslt)
{
	int  curPos = 0;
	curPos = mFileName.getWord(curPos, sgUsername);
	return 0;
}

bool
AosCli::createSecuredShell(OmnString &rslt)
{
	AosSecuredShellMgr shellMgr;

	//
	// secured shell start <username> <shell_id> <password> <duration>
	//
	int curPos = 0;

	OmnString username,temp;
	curPos = mFileName.getWord(curPos, username);
	curPos = mFileName.getWord(curPos, temp);

	if(username=="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	if(temp!="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	bool ret = shellMgr.createSecuredShell(rslt);
	if(ret == false)
		return false;
	return true;
}


bool
AosCli::removeShellCommands(OmnString &rslt)
{
	bool ret;
	AosSecuredShellMgr shellMgr;

	//
	// secured shell start <username> <shell_id> <password> <duration>
	//
	int curPos = 0;

	OmnString shellId,username,temp;

	curPos = mFileName.getWord(curPos, shellId);
	curPos = mFileName.getWord(curPos, username);
	curPos = mFileName.getWord(curPos, temp);

	if(username=="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	if(temp!="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	ret = shellMgr.removeSecuredShell(shellId,rslt);
	return ret;
}

bool
AosCli::restoreShellCommands(OmnString &rslt)
{
	bool ret; 
	AosSecuredShellMgr shellMgr;

	//
	// secured shell start <username> <shell_id> <password> <duration>
	//
	int curPos = 0;

	OmnString shellId,username,temp;

	curPos = mFileName.getWord(curPos, shellId);
	curPos = mFileName.getWord(curPos, username);
	curPos = mFileName.getWord(curPos, temp);

	if(username=="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	if(temp!="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	ret = shellMgr.restoreSecuredShell(shellId,rslt);
	return ret;
}

bool
AosCli::clearActiveShells(OmnString &rslt)
{
	bool ret; 
	AosSecuredShellMgr shellMgr;
	int curPos = 0;
	OmnString username,temp;
	curPos = mFileName.getWord(curPos, username);
	curPos = mFileName.getWord(curPos, temp);
	if(username=="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	if(temp!="")
	{	
		rslt << "Command format is wrong!";
		return false;
	}
	ret = shellMgr.stopAllShell(rslt);
	return ret;
}

#endif

