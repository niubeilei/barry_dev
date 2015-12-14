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
#include "aosUtil/List.h"  
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

struct bridgeConfigEntry;
struct bridgeDevEntry;

extern struct aos_list_head sgBridgeConfigList;

int
AosCli::bridgeGroupAdd(OmnString &rslt)
{
    int curPos = 0;
    //int status = 0;
	OmnString tmpName;
	OmnString cmd;
	
	struct bridgeConfigEntry * ptr;

	curPos = mFileName.getWord(curPos, tmpName);
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
    	if(ptr->bridgeName == tmpName) 
   		{      
        	cout << "Error: The bridge already exists !" << endl;
			return -1;
      	}            
    }

	struct bridgeConfigEntry * data;
	if((data = new struct bridgeConfigEntry)==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	data->bridgeName = tmpName;
	
	cout<<"add new bridge name:"<<data->bridgeName<<endl;
	data->status = "down";
	//init array
	for(int i=0; i<16; i++)
	{
		data->bridgeDev[i].dev = 0;
		data->bridgeDev[i].ip = 0;
	}
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgBridgeConfigList);
	//AOS_INIT_LIST_HEAD(&bridgeDevList);

    cmd << "brctl addbr " << tmpName;
	cout << "command: "<<cmd << endl;
    OmnCliSysCmd::doShell(cmd, rslt);
     
    return 0;
}


int
AosCli::bridgeGroupMemberAdd(OmnString &rslt)
{
    int curPos = 0;
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpDev;
    //struct bridgeConfigEntry * pBridge;
    //struct bridgeDevEntry * pDev;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

    curPos = mFileName.getWord(curPos,tmpBridgeName);
	curPos = mFileName.getWord(curPos, tmpDev);
   // convert the outcard and incard to specific dev
   if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "eth1";
	}
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
		
		if (ptr->bridgeName == tmpBridgeName)
		{
			// do bl array
			for(int i=0; i<16; i++)
			{
			
				if(ptr->bridgeDev[i].dev != 0)
				{
					if(ptr->bridgeDev[i].dev == tmpDev)
					{
        				cout << "Error: The device already exists !" << endl;
						return -1;
					}	
				}
				else if(ptr->bridgeDev[i].dev == 0)
				{
		    		ptr->bridgeDev[i].dev = tmpDev;
					status = 1;
					break;
				}
			}	 
			if (status == 1)
	   		{
	   			break;//goto success;
	   		}
	   		else
      	    {
				status = -1;
				cout << "You have added 16 devices!";
				return -1;
	  	    }
		}
	}

	//else
	if(status == 0)
	{
        cout << "Error: The bridge does not exist !" << endl;
		return -1;
	}
	// success
	//add ethernet interface to bridge and promisc ethernet interface!
	cmd << "brctl addif " << ptr->bridgeName << " " \
		<< tmpDev << "&&" << "ifconfig " \
		<< tmpDev << " down &&" << "ifconfig "\
		<< tmpDev << " 0.0.0.0 up;";
   // cout<<"command: "<< cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
    return 0;
}


int
AosCli::bridgeGroupMemberDel(OmnString &rslt)
{
    int curPos = 0;
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpDev;
    int status = 0;
    int j = 0;
	
	struct bridgeConfigEntry * ptr;

    curPos = mFileName.getWord(curPos,tmpBridgeName);
	curPos = mFileName.getWord(curPos, tmpDev);
   // convert the outcard and incard to specific dev
   if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "eth1";
	}
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
		if (ptr->bridgeName == tmpBridgeName) 
		{
		// do bl array
			for(int i=0; i<16; i++)
			{
				if(ptr->bridgeDev[i].dev == tmpDev)
				{

					for(j=i; j< 15; j++)
					{
						 ptr->bridgeDev[j].dev = ptr->bridgeDev[j+1].dev; 		
						 ptr->bridgeDev[j].ip = ptr->bridgeDev[j+1].ip; 		
					}
					status = 1;
					ptr->bridgeDev[j].dev = "";
					ptr->bridgeDev[j].ip = "";

					break;
				}
				//else if(ptr->bridgeDev[i].dev == 0)
					
		    }
			if(status !=1)
			{
        		status = -1;
				cout << "Error: The dev does not exist !" << endl;
				return -1;
			}
    	} 
		
	   if (status == 1)
	   {
			break;
	   } 
		//goto success;
    }
//success:
	if(status == 0)
	{
        	cout << "Error: The bridge does not exist !" << endl;
			return -1;
	}

	//add ethernet interface to bridge and promisc ethernet interface!
    cmd << "brctl delif " << tmpBridgeName << " " << tmpDev;
	OmnCliSysCmd::doShell(cmd, rslt);
    return 0;
}


int
AosCli::bridgeGroupDel(OmnString &rslt)
{
    int curPos = 0;
	OmnString tmpName;
	OmnString cmd;
    int status = 0;	
	struct bridgeConfigEntry * ptr;

	curPos = mFileName.getWord(curPos, tmpName);
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
    	if(ptr->bridgeName == tmpName) 
   		{
			// modify
			__aos_list_del((ptr->datalist.prev), (ptr->datalist.next));
			// ...............
		    status = 1;
	    	cmd << "ifconfig " << tmpName << " down &&" << "brctl delbr " << tmpName;
        	OmnCliSysCmd::doShell(cmd, rslt);
     
			delete ptr;
		    break;
      	}   
    }
   if(status == 0)
   {
       cout << "Error: The bridge not exists !" << endl;
	   return -1;
   }
    return 0;
}


int
AosCli::bridgeGroupIp(OmnString &rslt)
{
    int curPos = 0;
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpIp;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

    curPos = mFileName.getWord(curPos,tmpBridgeName);
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
	
		if (ptr->bridgeName == tmpBridgeName) 
		{
			status = 1;
            curPos = mFileName.getWord(curPos, tmpIp);
			ptr->ip = tmpIp;

			break;
    	}

	}

	if(status ==0 )
	{
        cout << "Error: The bridge not exist!" << endl;
		return -1;
	}
	//set bridge ip !
     cmd << "ifconfig " << tmpBridgeName << " " << tmpIp ;
	 cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
    return 0;
}


int
AosCli::bridgeClearConfig(OmnString &rslt)
{
    //int curPos = 0;
    OmnString cmd;
	struct bridgeConfigEntry * ptr;

    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
       // down interface then del the bridge!
       // down interface then del the bridge!
	   if (ptr->bridgeName !=0)
	   {
       cmd << "ifconfig " << ptr->bridgeName << " down && " << "brctl delbr " << ptr->bridgeName;
  cout << cmd << endl;
	   OmnCliSysCmd::doShell(cmd, rslt);
		// do bl array
			for(int i=0; i<16; i++)
			{
					ptr->bridgeDev[i].dev = "";

		    }
		}
	}

	struct bridgeConfigEntry * tmp;

   	aos_list_for_each_entry_safe(ptr, tmp, &sgBridgeConfigList, datalist)
    {       
       	aos_list_del(&ptr->datalist);
        aos_free(ptr);
    }
	AOS_INIT_LIST_HEAD(&sgBridgeConfigList);
     
    return 0;
}


int
AosCli::bridgeGroupMemberIp(OmnString &rslt)
{
    int curPos = 0;
    OmnString cmd;
    OmnString tmpIp;
    OmnString tmpDev;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

    curPos = mFileName.getWord(curPos,tmpDev);
	if(tmpDev == "outcard")
	{
		tmpDev = "eth0";
	}
	else if(tmpDev == "incard")
	{
		tmpDev = "eth1";
	}
	curPos = mFileName.getWord(curPos, tmpIp);
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
		
		if (ptr->bridgeName != 0) 
		{
		// do bl array
			for(int i=0; i<16; i++)
			{	
				if(ptr->bridgeDev[i].dev == tmpDev)
				{
					ptr->bridgeDev[i].ip = tmpIp;
			        status = 1;
					break;
				}	
		 	}
            
	
		}
	   if (status == 1)
	   {
	   		break;//goto success;
	   }
    }
//success:
   if(status != 1)
	{
		status = -1;
        cout << "Error: The device not added in bridge or not exists !" << endl;
		return -1;
	}
	//add ethernet interface to bridge and promisc ethernet interface!
    cmd << "ifconfig  " << tmpDev << " " << tmpIp << " up ;";
    cout<<"command: "<<cmd<<endl;
	OmnCliSysCmd::doShell(cmd, rslt);
     
    return 0;
}


// GB, 12/06/2006
int
AosCli::bridgeShowConfig(OmnString &rslt)
{
	OmnString contents;
	OmnString bridgeInfo;
	struct bridgeConfigEntry * ptr;
    int i = 0;
    int status = 0;
 	//struct bridgeDevEntry* ptr1;
 
	cout << "bridge       information" << endl
		<< "----------------------------" << endl;
	aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
	{
	    if(0 != status)
		{
 			cout << "----------------------------" << endl;
		}
  		cout << "            bridge name: " << ptr->bridgeName << endl;
  		cout << "                    ip : " << ptr->ip << endl;
  		cout << "                status : " << ptr->status << endl;
  		cout << "\t\t\tDev\tIP" << endl;
  		cout << "\t\t\t----------------------------" << endl;
  		for (i = 0; i < 16; i++)
  		{
   			if (ptr->bridgeDev[i].dev == 0)
  		 	{	
    			break;
  		 	}
   			else
  		 	{
   				 cout << "                        "
		   			 	<< ptr->bridgeDev[i].dev << "\t"
		    			<< ptr->bridgeDev[i].ip << endl;
  		 	}
 	 	}	
		status++;
 	}
    return 0;

}

// CHK 2006-12-12
int 
AosCli::saveOnlyBridgeConfig(OmnString &rslt)
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

	saveBridgeConfig(rslt);
	contents = rslt;

	char local[200];
	sprintf(local, "------AosCliBlock: bridge------\n");
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

// CHK 2006-12-12
int 
AosCli::saveBridgeConfig(OmnString &rslt)
{
	OmnString contents;
	OmnString bridgeInfo;
	struct bridgeConfigEntry * ptr;
	aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
	{
		contents << "<Cmd>bridge name " << ptr->bridgeName <<"</Cmd>\n";
		contents << "<Cmd>bridge ip " << ptr->ip << "</Cmd>\n";
		contents << "<Cmd>bridge status " << ptr->status << "</Cmd>\n";
		for (int i = 0; i < 16; i++)
		{
			if (ptr->bridgeDev[i].dev.length() <= 0)
			{	
				break;
			}
			else
			{
				contents << "<Cmd>device " << i + 1 <<"</Cmd>\n";
				contents << "<Cmd>device name \t" << ptr->bridgeDev[i].dev <<"</Cmd>\n";
				contents << "<Cmd>device ip \t\t" << ptr->bridgeDev[i].ip << "</Cmd>\n";
				contents << "<Cmd>device end</Cmd>\n";
			}
		}
		contents << "<Cmd>bridge end</Cmd>\n";
	}
	rslt = contents;
	
	return 0;
}

// CHK 2006-12-12
int 
AosCli::loadBridgeConfig(OmnString &rslt)
{
	struct bridgeConfigEntry * bcePtr = NULL;
	struct bridgeConfigEntry * tmpPtr = NULL;
	struct bridgeConfigEntry bceTmp;
	//int nDevNum = 0;

	// 1. Delete the bridge config data in memory
	aos_list_for_each_entry_safe(bcePtr, tmpPtr, &sgBridgeConfigList, datalist)
	{
		aos_list_del(&bcePtr->datalist);
		aos_free(bcePtr);
	}
	AOS_INIT_LIST_HEAD(&sgBridgeConfigList);

	// 2. Open the bridge config file safely
	if (!mFile)
	{
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return -eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return -eAosAlarm_FailedToOpenFile;
		}
	}

	OmnString contents, cmd, bridgePrefix, subPrefix;
	OmnString strSubContent;
	OmnString start = "------AosCliBlock: ";
	start << "bridge" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;
	int curPos = 0;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "bridge" << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		OmnXmlItem config(contents);
		OmnXmlItemPtr itemPtr;
		config.reset();

		while (config.hasMore())
		{
			itemPtr = config.next();
			cmd = itemPtr->getStr();
			curPos = 0;
			curPos = cmd.getWord(curPos,subPrefix);
			if(OmnString("bridge") == subPrefix)
			{
				curPos = cmd.getWord(curPos,subPrefix);
				if(OmnString("name") == subPrefix)
				{
					curPos = cmd.getWord(curPos,bceTmp.bridgeName);
				}
				else if(OmnString("ip") == subPrefix)
				{
					curPos = cmd.getWord(curPos,bceTmp.ip);
				}
				else if(OmnString("status") == subPrefix)
				{
					curPos = cmd.getWord(curPos,bceTmp.status);
				}
				else if(OmnString("end") == subPrefix)
				{
					// OmnString strTmpCmd;
					bcePtr = new struct bridgeConfigEntry;
					bcePtr->bridgeName = bceTmp.bridgeName;
					bcePtr->ip = bceTmp.ip;
					bcePtr->status = bceTmp.status;
					for(int i = 0; i < 16; i++)
					{
						bcePtr->bridgeDev[i] = bceTmp.bridgeDev[i];
					}
					aos_list_add_tail((struct aos_list_head *)&bcePtr->datalist, &sgBridgeConfigList);
				}
				else 
				{
					OmnString err = "This is a bad bridge config command";
					err << subPrefix;
					throw OmnExcept(OmnFileLine, OmnErrId::E(-eAosAlarm_FailedToLoadConfig), err);
				}
			}
			else if(OmnString("device") == subPrefix)
			{
				curPos = cmd.getWord(curPos,subPrefix);
				int nDevNum = atoi(subPrefix) ;
				if(nDevNum > 0 && nDevNum < 17)
				{
					while(config.hasMore())
					{
						itemPtr = config.next();
						cmd = itemPtr->getStr();
						
						curPos = 0;
						curPos = cmd.getWord(curPos,subPrefix);
						curPos = cmd.getWord(curPos,subPrefix);
						if(OmnString("ip") == subPrefix)
						{
							curPos = cmd.getWord(curPos,strSubContent);
							bceTmp.bridgeDev[nDevNum - 1].ip = strSubContent;
						}
						else if(OmnString("name") == subPrefix)
						{
							curPos = cmd.getWord(curPos,strSubContent);
							bceTmp.bridgeDev[nDevNum - 1].dev = strSubContent;
						}
						else if(OmnString("end") == subPrefix)
						{
							break;
						}
						else
						{
							OmnString err = "The device config syntax is wrong ";
							err << subPrefix;
							// OmnAlarm << err << enderr;
							throw OmnExcept(OmnFileLine, OmnErrId::E(-eAosAlarm_FailedToLoadConfig), err);
						}
					}
				}
				else
				{
					OmnString err = "The device config number SubPrefix is wrong: [";
					err << subPrefix << "]";
					// OmnAlarm << err << enderr;
					throw OmnExcept(OmnFileLine, OmnErrId::E(-eAosAlarm_FailedToLoadConfig), err);
				}
			}
			else
			{
				OmnString err = "The 1st SubPrefix is wrong: [";
				err << subPrefix << "]";
				// OmnAlarm << err << enderr;
				//throw OmnExcept(OmnFileLine, rslt.getErrId(), err);
				throw OmnExcept(OmnFileLine, OmnErrId::E(-eAosAlarm_FailedToLoadConfig), err);
			}
		}
	}
	catch (const OmnExcept &e)
	{
		mFile->closeFile();
		mFile = NULL;
		cout << "XMLParse error: " << e.toString() << endl;
		// OmnAlarm << "XMLParse error: " << e.toString() << enderr;
		return -eAosAlarm_FailedToLoadConfig;
	}

	mFile->closeFile();
	mFile = NULL;

	// synchronizing the config value and the environment value
	

	return 0;
}


int
AosCli::bridgeGroupUp(OmnString &rslt)
{
    int curPos = 0;
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpIp;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

    curPos = mFileName.getWord(curPos,tmpBridgeName);
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
	
		if (ptr->bridgeName == tmpBridgeName) 
		{
			status = 1;
            //set the bridge status is up
            ptr->status = "up";

			break;
    	}

	}

	if(status ==0 )
	{
        cout << "Error: The bridge not exist!" << endl;
		return -1;
	}
	//set bridge ip !
     cmd << "ifconfig " << tmpBridgeName << " " << "up && "<< "brctl stp " << tmpBridgeName << " on";
	 cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
    return 0;
}


int
AosCli::bridgeGroupDown(OmnString &rslt)
{
    int curPos = 0;
    OmnString cmd;
    OmnString tmpBridgeName;
    OmnString tmpIp;
    int status = 0;
	
	struct bridgeConfigEntry * ptr;

    curPos = mFileName.getWord(curPos,tmpBridgeName);
    aos_list_for_each_entry(ptr, &sgBridgeConfigList, datalist)
    {       
	
		if (ptr->bridgeName == tmpBridgeName) 
		{
			status = 1;
            //set the bridge status is up
            ptr->status = "down";

			break;
    	}

	}

	if(status ==0 )
	{
        cout << "Error: The bridge not exist!" << endl;
		return -1;
	}
	//set bridge ip !
     cmd << "ifconfig " << tmpBridgeName << " " << "down" ;
	 cout << cmd << endl;
	OmnCliSysCmd::doShell(cmd, rslt);
    return 0;
}

#endif

