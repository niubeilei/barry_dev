////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemConfCli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "KernelInterface/CliSysCmd.h"
#include "KernelInterface_exe/version.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/List.h"  
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

#include "Global.h"


//kevin 07/28/2006 for uname
#include <sys/utsname.h>
int systemInit()
{

	AOS_INIT_LIST_HEAD(&sgSystemConfigList);
	return 0;
}

extern int systemIpAddress(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;

	OmnString rslt;
	OmnString interface;
	OmnString ip;
	OmnString mask;

	interface = parms->mStrings[0]; 
	ip = parms->mStrings[1]; 
	mask = parms->mStrings[2]; 

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		rslt << "Error: The interface is invalid!";
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
		return -1;
	}
	if(OmnIpAddr(ip) == OmnIpAddr::eInvalidIpAddr)
	{
		rslt << "Error: The IP is invalid!";
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
		return -1;
	}
	if(OmnIpAddr(mask) == OmnIpAddr::eInvalidIpAddr)
	{
		rslt << "Error: The netmask is invalid!";
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
		
		return -1;
	}
	OmnString systemCmd;

	//check the current config
	struct systemConfigListEntry * ptr;
	//struct systemConfigListEntry * nptr;	
	int flag = 0;
	//judge the dev ip is same
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
		systemCmd << "/sbin/ifconfig br0 ";
	}
	else 
	{
		systemCmd << "/sbin/ifconfig eth0 ";
	}
	// add the ipaddress to the specfic dev
	systemCmd << " " << ip << " netmask " << mask;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	*optlen = index;
	return 0;
}

extern int systemDns(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	OmnString dns;
	OmnString systemCmd;
	OmnString rslt;
	unsigned int index = 0;

    dns = parms->mStrings[0];
	//check the valid
	if(OmnIpAddr(dns) == OmnIpAddr::eInvalidIpAddr)
	{
		rslt << "Error: The DNS IP is invalid!";
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
		return -1;}


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

	*optlen = index;
	return 0;
}
extern int systemUname(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	OmnString id;
	OmnString rslt;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[256];
	id = parms->mStrings[0];
	
	//check the valid
	if((id != "sysname") && (id != "nodename") && (id != "release") && \
	 (id != "machine") && (id != "domainname") && (id != "version") && \
	 (id != "all"))
	{
		rslt << "Error: The Uname ID is invalid!" ;
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
		return -1;
	}
	
	struct utsname name;

	OmnString systemCmd;
	OmnString sysname;
	bool result;

	if (uname(&name) == -1)
	{
		rslt << "Error: Cannot get uname!\n";
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
        return -1;
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
		rslt << "The Ver & Build No :" << RELEASE << "\n";
		rslt << "The OS Name        :" << sysname << "\n";
		rslt << "The Node Name      :" << name.nodename << "\n";
		rslt << "The Machine Name   :" << name.machine << "\n";
		rslt << "The Kernel Release :" << name.release << "\n" ;
		rslt << "The Kernel Ver Info:" << name.version << "\n";
		rslt << "The Domain Name    :" << name.domainname << "\n" ;
	}
	else if (id == "sysname") 
	{
		rslt << "The OS Name is :" << sysname ;
	}
	else if (id == "nodename") 
	{
		rslt << "The Node Name is :" << name.nodename ;
	}
	else if (id == "release") 
	{
		rslt << "The Kernel Release is :" << name.release ;
	}
	else if (id == "machine") 
	{
		rslt << "The Machine Name is :" << name.machine ;
	}
	else if (id == "domainname") 
	{
		rslt << "The Domain Name is :" << name.domainname ;
	}
	else if (id == "version") 
	{
		rslt << "The Kernel Version Info is :" << name.version ;
	}
	else
	{
		rslt << "Error: The Uname ID is invalid!" ;
		strncpy(errmsg,rslt.data(),errlen-1);
        errmsg[errlen-1] = 0;
		return -1;
	}
	
	strncpy(local,rslt.data(),400);
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local)); 
	*optlen = index;
	return 0;
}

extern int clearSystemConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	*optlen = index;
	return 0;
}


extern int showSystemConfig(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];

	OmnString contents;
	OmnString rslt;
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
	
	rslt << "------config : system------\n" << contents;
	strncpy(local,rslt.data(),2048);
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local)); 
	*optlen = index;

	return 0;
}

int SystemConfig_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("system ip dns",systemDns);
	ret |= CliUtil_regCliCmd("system ip address",systemIpAddress);
	ret |= CliUtil_regCliCmd("system uname",systemUname);
	ret |= CliUtil_regCliCmd("system show config",showSystemConfig);
	ret |= CliUtil_regCliCmd("system clear config",clearSystemConfig);
	
	return ret;
}
