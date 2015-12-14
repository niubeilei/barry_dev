//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemRouterCli.cpp
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
//struct routerEntry RouterListEntry;
struct aos_list_head sgRouterEntryList;
struct routerConfigEntry sgRouter;

int routerInit()
{
	sgRouter.status = "off";

	AOS_INIT_LIST_HEAD(&sgRouterEntryList);
	return 0;
}


int routerSetStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString stopRouterCmd;
	OmnString rslt;
	OmnString status;
	OmnString systemCmd;
	OmnString dev;
	struct in_addr tmpaddr;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	char gwStr[MAX_IP_ADDR_LEN];

	status = parms->mStrings[0];
	if ((status == "off") && (sgRouter.status == "off"))
	{
		//cout << the Router's status is already "off"
		return -eAosRc_ServerAlreadyStopped;
	}
	else if ((status == "off") && (sgRouter.status == "on"))
	{
		// Clear the old dmz firewall rules
		routerClearRTConfig(rslt);
		sgRouter.status = status;
	}
	else
	{
		routerClearRTConfig(rslt);
		sgRouter.status = status;

		// add Router router rule for each Router entry
		struct routerEntry * ptr;

		aos_list_for_each_entry(ptr, &sgRouterEntryList, datalist)
		{
			systemCmd = "";
			if(ptr->interface == "incard")
			{
				dev = "br0";
			}
			else if(ptr->interface == "outcard")
			{
				dev = "eth0";
			}
			else
			{
				dev = "";
			}
			// clear the route rule if the status == on
			if (sgRouter.status == "on")
			{
				tmpaddr.s_addr= ptr->dip;
				strcpy(dipStr, inet_ntoa(tmpaddr));
				tmpaddr.s_addr= ptr->dmask;
				strcpy(dmaskStr, inet_ntoa(tmpaddr));
				if(dev == "")
				{
					if(ptr->gateway != 0)
					{
						tmpaddr.s_addr= ptr->gateway;
						strcpy(gwStr, inet_ntoa(tmpaddr));
						systemCmd << "/sbin/route add -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr;
					}
					else
					{
						return -eAosRc_InvalidRouterEntry;
					}
				}
				else
				{
					if(ptr->gateway != 0)
					{
						tmpaddr.s_addr= ptr->gateway;
						strcpy(gwStr, inet_ntoa(tmpaddr));
						systemCmd << "/sbin/route add -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr << " dev " << dev;
					}
					else
					{
						systemCmd << "/sbin/route add -net " << dipStr <<" netmask " << dmaskStr << " dev " << dev;
					}

				}
			}
			//cout << "do : " << systemCmd << endl;
			OmnCliSysCmd::doShell(systemCmd, rslt);

		}
		sgRouter.status = status;
	}
	*optlen = index;
	return 0;
}

int routerShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	OmnString contents;
	OmnString entryList;

	struct routerEntry * ptr;

	struct in_addr tmpaddr;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	char gwStr[MAX_IP_ADDR_LEN];

	strcpy(local,"");

	contents << "router information\n" 
		<< "--------------------------------" 
		<< "\n\t    status : " << sgRouter.status 
		<< "\n\tentry list : " 
		<< "\n\n"; 

	aos_list_for_each_entry(ptr, &sgRouterEntryList, datalist)
	{
		tmpaddr.s_addr= ptr->dip;
		strcpy(dipStr, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= ptr->dmask;
		strcpy(dmaskStr, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= ptr->gateway;
		strcpy(gwStr, inet_ntoa(tmpaddr));
		entryList << "\t\t\t" << ptr->alias << " " << dipStr << " "<< dmaskStr << " " << gwStr << " " << ptr->interface << "\n";
	}
	contents << "\t\t\talias dip dmask gateway interface\n\t\t\t---------------------------------\n" << entryList;

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;

	return 0;
}


int routerSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	OmnString contents;

	struct in_addr tmpaddr;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	char gwStr[MAX_IP_ADDR_LEN];

	struct routerEntry * ptr;

	aos_list_for_each_entry(ptr, &sgRouterEntryList, datalist)
	{
		if (ptr->alias != "")
		{
			tmpaddr.s_addr = ptr->dip;
			strcpy(dipStr, inet_ntoa(tmpaddr));
			tmpaddr.s_addr = ptr->dmask;
			strcpy(dmaskStr, inet_ntoa(tmpaddr));
			tmpaddr.s_addr = ptr->gateway;
			strcpy(gwStr, inet_ntoa(tmpaddr));
			contents << "<Cmd>";
			contents << "router add entry "<< ptr->alias << " " << dipStr << " " << dmaskStr << " " << gwStr << " " << ptr->interface;
			contents << "</Cmd>\n";
		}
	}

	contents << "<Cmd>";
	contents << "router set status "<< sgRouter.status;
	contents << "</Cmd>\n";

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}

int routerClearRTConfig(OmnString &rslt)
{
	struct routerEntry * ptr;
	OmnString systemCmd;
	OmnString dev;
	struct in_addr tmpaddr;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	char gwStr[MAX_IP_ADDR_LEN];

	aos_list_for_each_entry(ptr, &sgRouterEntryList, datalist)
	{       
		systemCmd = "";
		if(ptr->interface == "incard")
		{
			dev = "br0";
		}
		else if(ptr->interface == "outcard")
		{
			dev = "eth0";
		}
		else
		{
			dev = "";
		}
		// clear the route rule if the status == on
		tmpaddr.s_addr= ptr->dip;
		strcpy(dipStr, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= ptr->dmask;
		strcpy(dmaskStr, inet_ntoa(tmpaddr));
		if(dev == "")
		{
			if(ptr->gateway != 0)
			{
				tmpaddr.s_addr= ptr->gateway;
				strcpy(gwStr, inet_ntoa(tmpaddr));
				systemCmd << "/sbin/route del -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr;
			}
			else
			{
				return -eAosRc_InvalidRouterEntry;
			}
		}
		else
		{
			if(ptr->gateway != 0)
			{
				tmpaddr.s_addr= ptr->gateway;
				strcpy(gwStr, inet_ntoa(tmpaddr));
				systemCmd << "/sbin/route del -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr << " dev " << dev;
			}
			else
			{
				systemCmd << "/sbin/route del -net " << dipStr <<" netmask " << dmaskStr << " dev " << dev;
			}

		}
		//cout << "do : " << systemCmd << endl;
		OmnCliSysCmd::doShell(systemCmd, rslt);
	}
	return 0;
}

int routerClearConfig(OmnString &rslt)
{
	struct routerEntry * ptr;
	struct routerEntry * tmp;
	OmnString systemCmd;
	OmnString dev;
	struct in_addr ;

	routerClearRTConfig(rslt);
	aos_list_for_each_entry_safe(ptr, tmp, &sgRouterEntryList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}

	AOS_INIT_LIST_HEAD(&sgRouterEntryList);
	sgRouter.status = "off";

	return 0;
}


int routerClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	//char* buf = CliUtil_getBuff(data);

	routerClearConfig(rslt);

	*optlen = index;
	return 0;
}


int routerAddEntry(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{

	unsigned int index = 0;
	OmnString rslt;
	char local[256];

	struct routerEntry * ptr;
	OmnString systemCmd;
	struct in_addr tmpaddr;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	char gwStr[MAX_IP_ADDR_LEN];
	

	OmnString alias = parms->mStrings[0];
	/*if ( alias.length() > 16)
	{
		return -eAosRc_AliasTooLong;
	}
	*/
	OmnString interface = parms->mStrings[1];
	OmnString dev;
	u32 dip = parms->mIntegers[0];
	u32 dmask = parms->mIntegers[1];
	u32 gateway = parms->mIntegers[2];

	aos_list_for_each_entry(ptr, &sgRouterEntryList, datalist)
	{       
		if(ptr->alias == parms->mStrings[0]) 
		{       
			strcpy(local,rslt);
			rslt = "Error: The alias already exists !";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_EntryExist;
		}            
	}

	struct routerEntry * entry;
	if((entry = (struct routerEntry *)aos_malloc(sizeof(struct routerEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(entry, 0, sizeof(struct routerEntry));
	entry->alias = alias;
	entry->interface = interface;
	entry->dip = dip;
	entry->dmask = dmask;
	entry->gateway = gateway;
	aos_list_add_tail((struct aos_list_head *)&entry->datalist, &sgRouterEntryList);

	if(interface == "incard")
	{
		dev = "br0";
	}
	else if(interface == "outcard")
	{
		dev = "eth0";
	}
	else
	{
		dev = "";
	}

	// add the firewall rule if the status == on
	if (sgRouter.status == "on")
	{
		tmpaddr.s_addr= dip;
		strcpy(dipStr, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= dmask;
		strcpy(dmaskStr, inet_ntoa(tmpaddr));
		if(dev == "")
		{
			if(gateway != 0)
			{
				tmpaddr.s_addr= gateway;
				strcpy(gwStr, inet_ntoa(tmpaddr));
				systemCmd << "/sbin/route add -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr;
			}
			else
			{
				return -eAosRc_InvalidRouterEntry;
			}
		}
		else
		{
			if(gateway != 0)
			{
				tmpaddr.s_addr= gateway;
				strcpy(gwStr, inet_ntoa(tmpaddr));
				systemCmd << "/sbin/route add -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr << " dev " << dev;
			}
			else
			{
				systemCmd << "/sbin/route add -net " << dipStr <<" netmask " << dmaskStr << " dev " << dev;
			}

		}
	}
	//cout << "do : " << systemCmd << endl;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	*optlen = index;
	return 0;
}

int routerDelEntry(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int existflag = 0;
	OmnString rslt;

	struct routerEntry * ptr;
	struct routerEntry * tmp;

	OmnString systemCmd;
	OmnString dev;
	struct in_addr tmpaddr;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	char gwStr[MAX_IP_ADDR_LEN];

	aos_list_for_each_entry_safe(ptr, tmp, &sgRouterEntryList, datalist)
	{       
		systemCmd = "";
		if( ptr->alias == parms->mStrings[0] ) 
		{       	
			if(ptr->interface == "incard")
			{
				dev = "br0";
			}
			else if(ptr->interface == "outcard")
			{
				dev = "eth0";
			}
			else
			{
				dev = "";
			}
			// clear the route rule if the status == on
			if (sgRouter.status == "on")
			{
				tmpaddr.s_addr= ptr->dip;
				strcpy(dipStr, inet_ntoa(tmpaddr));
				tmpaddr.s_addr= ptr->dmask;
				strcpy(dmaskStr, inet_ntoa(tmpaddr));
				if(dev == "")
				{
					if(ptr->gateway != 0)
					{
						tmpaddr.s_addr= ptr->gateway;
						strcpy(gwStr, inet_ntoa(tmpaddr));
						systemCmd << "/sbin/route del -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr;
					}
					else
					{
						return -1;
					}
				}
				else
				{
					if(ptr->gateway != 0)
					{
						tmpaddr.s_addr= ptr->gateway;
						strcpy(gwStr, inet_ntoa(tmpaddr));
						systemCmd << "/sbin/route del -net " << dipStr <<" netmask " << dmaskStr <<" gw " << gwStr << " dev " << dev;
					}
					else
					{
						systemCmd << "/sbin/route del -net " << dipStr <<" netmask " << dmaskStr << " dev " << dev;
					}

				}
			}
			//cout << "do : " << systemCmd << endl;
			OmnCliSysCmd::doShell(systemCmd, rslt);
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
		return -eAosRc_EntryNotExist;
	}

	*optlen = index;
	return 0;
}


int Router_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("router show config", routerShowConfig);
	ret |= CliUtil_regCliCmd("router set status", routerSetStatus);
	ret |= CliUtil_regCliCmd("router add entry", routerAddEntry);
	ret |= CliUtil_regCliCmd("router del entry", routerDelEntry);
	ret |= CliUtil_regCliCmd("router clear config", routerClearConfig);
	ret |= CliUtil_regCliCmd("router save config", routerSaveConfig);

	return ret;
}


