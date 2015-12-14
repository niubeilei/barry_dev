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
#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
//#include "Util/IpAddr.h"
//#include "Util/OmnNew.h"
//#include "XmlParser/XmlItem.h"

#include <string.h>

#include "Global.h"

struct aos_list_head sgSystemConfigList;

int systemArpProxyAdd(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnString systemCmd;
	OmnString interface;
	interface = parms->mStrings[0];

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
		systemCmd << "/bin/echo 1 > /proc/sys/net/ipv4/conf/br0/proxy_arp";
	}
	else 
	{
		systemCmd << "/bin/echo 1 > /proc/sys/net/ipv4/conf/eth0/proxy_arp";
	}
	OmnCliSysCmd::doShell(systemCmd, rslt);

	*optlen = index;
	return 0;
}

int systemArpProxyDel(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnString error;
	OmnString interface;
	interface = parms->mStrings[0];

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
        error = "Error: The rule not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
    }
    else if (interface=="incard")
	{
		systemCmd << "/bin/echo 0 > /proc/sys/net/ipv4/conf/br0/proxy_arp";
	}
	else 
	{
		systemCmd << "/bin/echo 0 > /proc/sys/net/ipv4/conf/eth0/proxy_arp";
	}
    OmnCliSysCmd::doShell(systemCmd, rslt);

	*optlen = index;
	return 0;
}
	
int ArpProxy_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("system dev route", systemDevRoute);
	ret |= CliUtil_regCliCmd("system del dev route", systemDelDevRoute);
	ret |= CliUtil_regCliCmd("system arp proxy add", systemArpProxyAdd);
	ret |= CliUtil_regCliCmd("system arp proxy del", systemArpProxyDel);
	
	return ret;
}



