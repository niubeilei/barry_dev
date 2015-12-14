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
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>

//kevin 07/28/2006 for uname
#include <sys/utsname.h>
#include "KernelInterface_exe/version.h"

#define MAX_INTERFACE 16


extern struct aos_list_head sgMacList;

int
AosCli::macSet(OmnString &rslt)
{
	int curPos = 0;
	int curPos1 = 0;
	OmnString tmpDev;
	OmnString tmpMac;
	OmnString tmpOldMac;
	OmnString change;
	//char buff[128];
	curPos = mFileName.getWord(curPos, tmpDev);
	//OmnString tmpOldMac;
	struct macListEntry * ptr = NULL;
	struct macListEntry * tmp = NULL;

	aos_list_for_each_entry(tmp, &sgMacList, datalist) {
		if(tmp->dev == tmpDev)
		{
			ptr = tmp;
		}		
	}
	if (ptr == NULL)
	{
		if((ptr = new struct macListEntry())==NULL) 
		{
			return -1;
		}
		aos_list_add_tail((struct aos_list_head *)&ptr->datalist, &sgMacList);
	}/* else
	{
		cout << "You have changed your mac! Enter yes continue,Enter no cancel\n";
		while(1)
		{
    			scanf("%s",buff);
			if(strncmp(buff, "no", 2) == 0 ||strncmp(buff, "NO", 2) == 0)
			{
				return -1;	
			}
			else if (strncmp(buff, "yes", 3) == 0 ||strncmp(buff, "YES", 3) == 0)
			{
           			break;
			}	
    			else 
			{	
				cout << "You input is wrong,please try!\n";
			}
		}	

	}*/

    ptr->dev = tmpDev; 
    
    curPos = mFileName.getWord(curPos, ptr->mac);
	OmnString systemCmd;
   	systemCmd << "ifconfig " << ptr->dev << " up;" << "ifconfig |grep " << ptr->dev << "|awk \'$4=\"HWaddr\" {print $5}\'  ";
  	OmnCliSysCmd::doShell(systemCmd, rslt);
	
    curPos1 = rslt.getWord(curPos1,tmpOldMac); 
	ptr->oldMac = tmpOldMac;


	OmnString setCmd;
	setCmd << "ifconfig " << ptr->dev << " down;" << "ifconfig " << ptr->dev << " hw ether " <<  ptr->mac << " up \n";
  	OmnCliSysCmd::doShell(setCmd, rslt);


	return 0;
	
}


int 
AosCli::macBackSet(OmnString &rslt)
{
	int curPos = 0;
	//int curPos1 = 0;
	OmnString tmpDev;
	OmnString tmpMac;
	struct macListEntry * ptr = NULL;
	struct macListEntry * tmp = NULL;
    
	curPos =mFileName.getWord(curPos,tmpDev);
    aos_list_for_each_entry(tmp, &sgMacList, datalist)
	{
		if(tmp->dev == tmpDev)
		{
			ptr = tmp;
		}
	}
	
	if(!ptr)
	{
		cout << "You have not changed the device mac!";
		return -1;
	}

	ptr->dev = tmpDev;
    //curPos1 = ptr->oldMac.getWord(curPos1,tmpOldMac); 
    
    OmnString setCmd;
	setCmd << "ifconfig " << ptr->dev << " down;" << "ifconfig " << ptr->dev << " hw ether " <<  ptr->oldMac  << " up \n";
  	OmnCliSysCmd::doShell(setCmd, rslt);

  tmpMac = ptr->mac;
  ptr->mac = ptr->oldMac;
  ptr->oldMac = tmpMac;
	return 0;

}


//add by GB 12/04/06
int 
AosCli::saveOnlyMacConfig(OmnString &rslt)
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

	OmnString contents;
	int ret;
	saveMacConfig(rslt);
	contents = rslt;

	char local[200];
	sprintf(local, "------AosCliBlock: mac------\n");
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
AosCli::saveMacConfig(OmnString &rslt)
{
	
	OmnString contents;
	struct macListEntry * ptr;
	aos_list_for_each_entry(ptr,&sgMacList,datalist)
	{
		contents << "<Cmd> mac list ";
		contents << ptr->dev << " ";
		contents << ptr->mac << " ";
		contents << ptr->oldMac << " </Cmd>\n";

	}

	rslt = contents;

	return 0;
}


int
AosCli::macShowConfig(OmnString &rslt)
{
	OmnString contents;
    struct macListEntry *ptr;
	cout << "Ethernet interface mac information\n"  << "------------------------------------\n";
aos_list_for_each_entry(ptr, &sgMacList, datalist)
	{
		contents << "\t\t\t" << ptr->dev << " " << ptr->mac << " " << ptr->oldMac << "\n";
	}
	cout << "\t\t\tdev            mac           oldMac      \n\t\t\t----------------------------------------\n" << contents << endl;

	return 0;
}


int 
AosCli::loadMacConfig(OmnString &rslt)
{
    // Clear the old config
	struct macListEntry * ptr;
	struct macListEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgMacList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgMacList);
	

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

	OmnString contents, cmd, macPrefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "mac" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "mac" << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		OmnXmlItem config(contents);
		config.reset();
		
		while (config.hasMore())
		{
			OmnXmlItemPtr item = config.next();
			try
			{
				cmd = item->getStr();
				int curPos = 0;
				curPos = cmd.getWord(curPos,macPrefix);
				if (macPrefix == "mac")
				{
					curPos = cmd.getWord(curPos,subPrefix);
					if(subPrefix == "list")
					{
						struct macListEntry * ptr;
						if((ptr = new struct macListEntry())==NULL)	
						{
							return -1;
						}
								
						curPos = cmd.getWord(curPos, ptr->dev);
						curPos = cmd.getWord(curPos, ptr->mac);
						curPos = cmd.getWord(curPos, ptr->oldMac);
						aos_list_add_tail((struct aos_list_head *)&ptr->datalist, &sgMacList);
						continue;
					}
					else 
					{
						cout << "Error:The mac list SubPrefix is wrong!" << endl;
						continue;
					}
				}
				else
				{	
					cout << "Error:This is a bad mac prefix!" << endl;
					continue;
				}
			} catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				return -eAosAlarm_FailedToLoadConfig;
			}
		}
		
	} catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " << "mac" << endl;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = 0;

	return 0;
}
#endif

