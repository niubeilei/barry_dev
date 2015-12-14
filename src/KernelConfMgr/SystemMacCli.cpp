////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemMacCli.cpp
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

// Global Para
struct aos_list_head sgMacList;


int macCloningInit()
{
	AOS_INIT_LIST_HEAD(&sgMacList);
	return 0;
}
int	macSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	int curPos = 0;
	OmnString tmpDev;
	OmnString tmpMac;
	tmpMac = parms->mStrings[1];
	tmpDev = parms->mStrings[0];
	if (tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if (tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	else
	{
		rslt << "The device should be outcard";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}
	OmnString tmpOldMac;
	//	OmnString change;
	//char buff[128];
	//OmnString tmpOldMac;
	struct macListEntry * ptr = NULL;
	struct macListEntry * tmp = NULL;
	
		
	OmnString systemCmd;
	systemCmd << "ifconfig " << tmpDev << " up;"
		<< "ifconfig |grep "
		<< tmpDev    << "|awk \'$4=\"HWaddr\" {print $5}\'  ";
        //cout << systemCmd << endl;
	OmnCliSysCmd::doShell(systemCmd, rslt);
	int ret = system(systemCmd);
	if (ret != 0) 
	{
		rslt << "System error! Can not get the device current mac!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemError;
	}
	curPos = rslt.getWord(curPos,tmpOldMac); 
	
	/*if (strlen(tmpOldMac) <= 0) 
	{
		*optlen = 0;
		return -1;
	}*/
		
	OmnString setCmd;
	setCmd << "ifconfig " << tmpDev << " down;"
		<< "ifconfig " << tmpDev << " hw ether " 
		<<  tmpMac << " up \n";
       //	OmnCliSysCmd::doShell(setCmd, rslt);
        //cout << setCmd << endl;
        ret = system(setCmd);
	//judge the rslt	
	if (ret != 0) 
	{
		rslt << "System error! Can not changed the device mac!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemError;
	}
	aos_list_for_each_entry(tmp, &sgMacList, datalist) {
		if(tmp->dev == tmpDev)
		{
			ptr = tmp;
			break;
		}		
	}
	
	if (ptr == NULL)
	{
		if((ptr = new struct macListEntry()) == NULL) 
		{
			return -eAosRc_MallocError;
		}	
		aos_list_add_tail(&(ptr->datalist), &sgMacList);

	}
	ptr->dev = tmpDev; 
	ptr->mac = tmpMac;
	ptr->oldMac = tmpOldMac;
	
	*optlen = index;
	
	return 0;

}


int macBackSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnString tmpDev;
	OmnString tmpMac;
	struct macListEntry * ptr = NULL;
	struct macListEntry * tmp = NULL;

	tmpDev = parms->mStrings[0];
	if (tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	if (tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	//judge the dev exsit or not
	aos_list_for_each_entry(tmp, &sgMacList, datalist)
	{
		if(tmp->dev == tmpDev)
		{
			ptr = tmp;
		}
	}

	if(!ptr)
	{
		rslt << "You have not changed the device mac!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_MacNotChanged;
	}

	ptr->dev = tmpDev;

	OmnString setCmd;
	setCmd << "ifconfig " << ptr->dev << " down;"
		<< "ifconfig " << ptr->dev << " hw ether "
		<<  ptr->oldMac  << " up \n";
	//OmnCliSysCmd::doShell(setCmd, rslt);
	int ret = system(setCmd);
	if (ret != 0) 
	{
		rslt << "System error! Can not rollback the device mac!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemError;
	}
	tmpMac = ptr->mac;
	ptr->mac = ptr->oldMac;
	ptr->oldMac = tmpMac;

	*optlen = index;
	return 0;

}

int	macShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	char local[256];

	OmnString contents;
	OmnString macList;
	struct macListEntry *ptr;
	contents << "Ethernet interface mac information\n" 
		<< "------------------------------------\n";

	aos_list_for_each_entry(ptr, &sgMacList, datalist)
	{
		if (ptr != NULL)
			macList << "\t\t\t " << ptr->dev << " " 
				<< ptr->mac << "  "      << ptr->oldMac << "\n";
	}
	contents << "\t\t\tdev            mac           oldMac" 
		<< "\n\t\t\t----------------------------------------\n" << macList;

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;

	return 0;

}

int macSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString contents;
	OmnString tmpDev;
	char local[4096];
	struct macListEntry *ptr;

	aos_list_for_each_entry(ptr, &sgMacList, datalist)
	{
		if (ptr->dev != "")
		{

		if (OmnString("eth0") == ptr->dev)
			{
				tmpDev = "outcard";
				contents = "<Cmd>";
				contents << "mac set "<< tmpDev << " " << ptr->mac;
				contents << "</Cmd>\n";

			}
		}
	}

	strcpy(local, contents.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;

	return 0;
}

int macClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	struct macListEntry * ptr;
	struct macListEntry * tmp;

	aos_list_for_each_entry_safe(ptr, tmp, &sgMacList, datalist)
	{    
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}   
	AOS_INIT_LIST_HEAD(&sgMacList);

	dhcpInit();


	macCloningInit();

	*optlen = index;
	return 0;

}

extern int 	macGetCurrent(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	OmnString rslt;
	char local[256];
	int curPos = 0;
	OmnString tmpDev;
	OmnString tmpMac;
	tmpDev = parms->mStrings[0];
	if (tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if (tmpDev == "incard")
	{
		tmpDev = "br0";
	}
	else
	{
		rslt << "The device should be outcard";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;

	}
	OmnString tmpOldMac;
	
	OmnString systemCmd;
	systemCmd << "ifconfig " << tmpDev << " up;"
		<< "ifconfig |grep "
		<< tmpDev    << "|awk \'$4=\"HWaddr\" {print $5}\'  ";
        //cout << systemCmd << endl;
	OmnCliSysCmd::doShell(systemCmd, rslt);
	int ret = system(systemCmd);
	if (ret != 0) 
	{
		rslt << "System error! Can not get the device current mac!";
		strncpy(errmsg,rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_SystemError;
	}
	curPos = rslt.getWord(curPos,tmpOldMac); 
	strcpy(local, tmpOldMac.data());

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;
	return 0;


}

int MacClone_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("mac set",macSet );
	ret |= CliUtil_regCliCmd("mac back set", macBackSet);
	ret |= CliUtil_regCliCmd("mac get current", macGetCurrent);
	ret |= CliUtil_regCliCmd("mac show config", macShowConfig);
	ret |= CliUtil_regCliCmd("mac save config", macSaveConfig);
	ret |= CliUtil_regCliCmd("mac clear config", macClearConfig);

	return ret;
}
