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
#include <string.h>

#include "Global.h"

// Kevin, 03/08/2007
// Global Para
struct dmzMachineEntry dmzMachine;
struct aos_list_head sgDmzMachineList;
struct dmzConfigEntry sgDmz;

int dmzInit()
{
	sgDmz.status = "off";

	AOS_INIT_LIST_HEAD(&sgDmzMachineList);
	return 0;
}


int dmzSetStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString stopdmzCmd;
	OmnString rslt;
	OmnString status;

	status = parms->mStrings[0];
	if ((status == "off") && (sgDmz.status == "off"))
	{
		//cout << the dmz's status is already "off"
		return -eAosRc_ServerAlreadyStopped;
	}
	else if ((status == "off") && (sgDmz.status == "on"))
	{
		// Clear the old dmz firewall rules
		dmzClearFwConfig(rslt);
		sgDmz.status = status;
	}
	else
	{
		// Clear the old dmz firewall rules
		dmzClearFwConfig(rslt);

		// add dmz firewall rule for each dmz entry
		struct dmzMachineEntry * ptr;

		aos_list_for_each_entry(ptr, &sgDmzMachineList, datalist)
		{
			// add the ip alias to the wan
			struct in_addr tmpaddr;
			char pub_ipStr[MAX_IP_ADDR_LEN];
			OmnString dev = "eth0"; 
			OmnString systemCmd = "/sbin/ifconfig "; 
			tmpaddr.s_addr= ptr->pub_ip;
			strcpy(pub_ipStr, inet_ntoa(tmpaddr));
			systemCmd << dev << ":" << ptr->alias << " " << pub_ipStr;
			//cout << "do : " << systemCmd << endl;
			OmnCliSysCmd::doShell(systemCmd, rslt);
			// add dnat firewall rule
			OmnString interface = "outcard";
			OmnString proto = "all";
			fwNatDnatIP(interface, proto, ptr->pub_ip, ptr->dmz_ip, rslt);

			// add ipfil firewall rule
			OmnString chain = "forward";
			OmnString smask = "255.255.255.255";
			OmnString action = "permit";
			OmnString log = "nolog";
			fwIpfilAll(chain, ptr->dmz_ip, inet_addr(smask), 0, 0, action, log, rslt);
			fwIpfilAll(chain, 0, 0, ptr->dmz_ip, inet_addr(smask), action, log, rslt);
		}
		sgDmz.status = status;
	}

	//OmnString systemCmd;
	//systemCmd << "/usr/sbin/dmz >/dev/null 2>&1 &";
	//OmnCliSysCmd::doShell(systemCmd, rslt);
	//sgDmz.status = "start";

	//CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;
}


int dmzShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	OmnString contents;
	OmnString machineList;

	struct dmzMachineEntry * ptr;

	struct in_addr tmpaddr;
	char pub_ipStr[MAX_IP_ADDR_LEN];
	char dmz_ipStr[MAX_IP_ADDR_LEN];

	strcpy(local,"");

	contents << "dmz information\n" 
		<< "--------------------------------" 
		<< "\n\t      status : " << sgDmz.status 
		<< "\n\tmachine list : " 
		<< "\n\n"; 

	aos_list_for_each_entry(ptr, &sgDmzMachineList, datalist)
	{
		tmpaddr.s_addr= ptr->pub_ip;
		strcpy(pub_ipStr, inet_ntoa(tmpaddr));
		tmpaddr.s_addr= ptr->dmz_ip;
		strcpy(dmz_ipStr, inet_ntoa(tmpaddr));
		machineList << "\t\t\t" << ptr->alias << " " << pub_ipStr << " " << dmz_ipStr << "\n";
	}
	contents << "\t\t\talias pub_ip dmz_ip\n\t\t\t-------------------\n" << machineList;

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;

	return 0;
}


int dmzSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[2048];
	OmnString contents;

	struct in_addr tmpaddr;
	char pub_ipStr[MAX_IP_ADDR_LEN];
	char dmz_ipStr[MAX_IP_ADDR_LEN];

	struct dmzMachineEntry * ptr;

	aos_list_for_each_entry(ptr, &sgDmzMachineList, datalist)
	{
		if (ptr->alias != "")
		{
			tmpaddr.s_addr = ptr->pub_ip;
			strcpy(pub_ipStr, inet_ntoa(tmpaddr));
			tmpaddr.s_addr = ptr->dmz_ip;
			strcpy(dmz_ipStr, inet_ntoa(tmpaddr));
			contents << "<Cmd>";
			contents << "dmz add machine "<< ptr->alias << " " << pub_ipStr << " " << dmz_ipStr;
			contents << "</Cmd>\n";
		}
	}

	contents << "<Cmd>";
	contents << "dmz set status "<< sgDmz.status;
	contents << "</Cmd>\n";

	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	//*optlen = strlen(local);
	return 0;
}


int dmzClearFwConfig(OmnString &rslt)
{
	struct dmzMachineEntry * ptr;

	aos_list_for_each_entry(ptr, &sgDmzMachineList, datalist)
	{
		//dell the ip alias to the wan
		struct in_addr tmpaddr;
		char pub_ipStr[MAX_IP_ADDR_LEN];
		OmnString dev = "eth0"; 
		OmnString systemCmd = "/sbin/ifconfig "; 
		tmpaddr.s_addr= ptr->pub_ip;
		strcpy(pub_ipStr, inet_ntoa(tmpaddr));
		systemCmd << dev << ":" << ptr->alias << " " << "down";
		//cout << "do : " << systemCmd << endl;
		OmnCliSysCmd::doShell(systemCmd, rslt);
		// del dnat firewall rule
		OmnString interface = "outcard";
		OmnString proto = "all";
		fwNatDelDnatIP(interface, proto, ptr->pub_ip, ptr->dmz_ip, rslt);
		// del ipfil firewall rule
		OmnString chain = "forward";
		OmnString smask = "255.255.255.255";
		OmnString action = "permit";
		OmnString log = "nolog";
		fwIpfilDelAll(chain, ptr->dmz_ip, inet_addr(smask), 0, 0, action, log, rslt);
		fwIpfilDelAll(chain, 0, 0, ptr->dmz_ip, inet_addr(smask), action, log, rslt);
	}
	return 0;
}


int dmzClearConfig(OmnString &rslt)
{
	struct dmzMachineEntry * ptr;
	struct dmzMachineEntry * tmp;

	// clear the firewall rule
	if(sgDmz.status == "on")
	{
		dmzClearFwConfig(rslt);
	}

	aos_list_for_each_entry_safe(ptr, tmp, &sgDmzMachineList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}

	AOS_INIT_LIST_HEAD(&sgDmzMachineList);
	sgDmz.status = "off";

	return 0;
}


int dmzClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	//char* buf = CliUtil_getBuff(data);

	dmzClearConfig(rslt);

	*optlen = index;
	return 0;
}


int dmzAddMachine(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{

	unsigned int index = 0;
	OmnString rslt;
	char local[256];
	
	//check alias length
	/*OmnString tmpAlias = parms->mStrings[0];
	if ( tmpAlias.length() > 16)
	{
		return -eAosRc_AliasTooLong;
	}
*/
	struct dmzMachineEntry * ptr;
	aos_list_for_each_entry(ptr, &sgDmzMachineList, datalist)
	{       
		if(ptr->alias == parms->mStrings[0]) 
		{       
			strcpy(local,rslt);
			rslt = "Error: The alias already exists !";
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -eAosRc_DmzAliasExist;
		}            
	}

	struct dmzMachineEntry * machine;
	if((machine = (struct dmzMachineEntry *)aos_malloc(sizeof(struct dmzMachineEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(machine, 0, sizeof(struct dmzMachineEntry));
	machine->alias = parms->mStrings[0];
	machine->pub_ip = parms->mIntegers[0];
	machine->dmz_ip = parms->mIntegers[1];
	aos_list_add_tail((struct aos_list_head *)&machine->datalist, &sgDmzMachineList);

	// add the firewall rule if the status == on
	if (sgDmz.status == "on")
	{
		// add the ip alias to the wan
		struct in_addr tmpaddr;
		char pub_ipStr[MAX_IP_ADDR_LEN];
		OmnString dev = "eth0"; 
		OmnString systemCmd = "/sbin/ifconfig "; 
		tmpaddr.s_addr= machine->pub_ip;
		strcpy(pub_ipStr, inet_ntoa(tmpaddr));
		systemCmd << dev << ":" << machine->alias << " " << pub_ipStr;
		//cout << "do : " << systemCmd << endl;
		OmnCliSysCmd::doShell(systemCmd, rslt);
		// add dnat firewall rule
		OmnString interface = "outcard";
		OmnString proto = "all";
		fwNatDnatIP(interface, proto, machine->pub_ip, machine->dmz_ip, rslt);

		// add ipfil firewall rule
		OmnString chain = "forward";
		OmnString smask = "255.255.255.255";
		OmnString action = "permit";
		OmnString log = "nolog";
		fwIpfilAll(chain, machine->dmz_ip, inet_addr(smask), 0, 0, action, log, rslt);
		fwIpfilAll(chain, 0, 0, machine->dmz_ip, inet_addr(smask), action, log, rslt);
	}

	*optlen = index;
	return 0;
}

int dmzDelMachine(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	int existflag = 0;
	OmnString rslt;

	struct dmzMachineEntry * ptr;
	struct dmzMachineEntry * tmp;


	aos_list_for_each_entry_safe(ptr, tmp, &sgDmzMachineList, datalist)
	{       
		if( ptr->alias == parms->mStrings[0] ) 
		{       
			// clear the firewall rule if the status == on
			if (sgDmz.status == "on")
			{
				// del the ip alias to the wan
				struct in_addr tmpaddr;
				char pub_ipStr[MAX_IP_ADDR_LEN];
				OmnString dev = "eth0"; 
				OmnString systemCmd = "/sbin/ifconfig "; 
				tmpaddr.s_addr= ptr->pub_ip;
				strcpy(pub_ipStr, inet_ntoa(tmpaddr));
				systemCmd << dev << ":" << ptr->alias << " " << "down";
				//cout << "do : " << systemCmd << endl;
				OmnCliSysCmd::doShell(systemCmd, rslt);
				// del dnat firewall rule
				OmnString interface = "outcard";
				OmnString proto = "all";
				fwNatDelDnatIP(interface, proto, ptr->pub_ip, ptr->dmz_ip, rslt);

				// del ipfil firewall rule
				OmnString chain = "forward";
				OmnString smask = "255.255.255.255";
				OmnString action = "permit";
				OmnString log = "nolog";
				fwIpfilDelAll(chain, ptr->dmz_ip, inet_addr(smask), 0, 0, action, log, rslt);
				fwIpfilDelAll(chain, 0, 0, ptr->dmz_ip, inet_addr(smask), action, log, rslt);

			}
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
		return -eAosRc_DmzAliasNotExist;
	}

	*optlen = index;
	return 0;
}


int Dmz_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("dmz show config", dmzShowConfig);
	ret |= CliUtil_regCliCmd("dmz set status", dmzSetStatus);
	ret |= CliUtil_regCliCmd("dmz add machine", dmzAddMachine);
	ret |= CliUtil_regCliCmd("dmz del machine", dmzDelMachine);
	ret |= CliUtil_regCliCmd("dmz clear config", dmzClearConfig);
	ret |= CliUtil_regCliCmd("dmz save config", dmzSaveConfig);

	return ret;
}


