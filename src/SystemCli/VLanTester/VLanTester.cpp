////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerTester.cpp
// Description:
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SystemCli/VLanTester/VLanTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "Util/Random.h"
#include "UtilComm/Ptrs.h"
#include "KernelSimu/timer.h"
#include <string.h>
#include <stdio.h>

//const int eMaxAppNameLen = 32;
//const int eMaxTooLongAppNameLen = 64;


// 
// CLI Command selector definition
//
static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  40</Element>"		//eVLanAddDev 		    
	        "<Element>2,  25</Element>"		//eVLanDelDev 		    
	        "<Element>3,  10</Element>"		//eVLanSetIp  		    
	        "<Element>4,  10</Element>"		//eVLanSwitchStatus 	
	        "<Element>5,  10</Element>"		//eVLanSwitchSet 		
	        "<Element>6,  2</Element>"		//eVLanSwtichReset	    
	        "<Element>7,  1</Element>"		//eVLanClearConfig 	    
            "<Element>8,  0</Element>"	    //eVLanSaveConfig 	    
	        "<Element>9,  0</Element>"		//eVLanLoadConfig 	    
	        "<Element>10, 2</Element>"		//eVLanShowConfig 	    
		"</Elements>";

AosVLanTester::vlanList::vlanList()
:
interface(""),
vlanid(0),
ip("")
{
}




AosVLanTester::vlanList::~vlanList()
{
}

AosVLanTester::vlanList& AosVLanTester::vlanList::operator = (const AosVLanTester::vlanList &devl)
{
	interface        = devl.interface      ;
	vlanid           = devl.vlanid         ;
	ip               = devl.ip             ;
	return *this;
}

void
AosVLanTester::vlanList::clear()
{
	interface      = "";
	vlanid         = 0;
	ip             = "";
}


AosVLanTester::AosVLanTester()
:
mDevNum(0),
mSaveNum(0),
mMinSwitchPort(eMinSwitchPort),
mMaxSwitchPort(eMaxSwitchPort)

{
	mName = "VLanTester";
}

void
AosVLanTester::setSwitchPorts(const int min, const int max)
{
	mMinSwitchPort = min;
	mMaxSwitchPort = max;
}

bool 
AosVLanTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	return true;
}

bool 
AosVLanTester::basicTest()
{
	OmnBeginTest << "Test VLan";
	
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef,100);

	int count = 0;
	bool rslt;
	while (count < 1000)
	{
		int index = cliSelector.nextInt();

		switch (index)
		{
			case eVLanAddDev : 			
				 rslt = runAddDev();
				 break;
			case eVLanDelDev :			
				 rslt = runDelDev();
				 break;
			case eVLanSetIp : 		
				 rslt = runSetIp();
				 break;
			case eVLanSwitchStatus :	
				 rslt = runSwitchStat();
				 break;
			case eVLanSwitchSet :	
				 rslt = runSwitchSet();
				 break;
			case eVLanSwitchReset : 	
				 rslt = runSwitchReset();
				 break;
			case eVLanClearConfig :
				 rslt = runClearConfig();
				 break;
			case eVLanSaveConfig :	
				 rslt = runSaveConfig();
				 break;
			case eVLanLoadConfig :	
				 rslt = runLoadConfig();
				 break;
			case eVLanShowConfig : 	
				 rslt = runShowConfig();
				 break;
        	
			default:
				 OmnAlarm << "Invalid index: " << index << enderr;
				 break;
		}
		
		count++;
		updateSeed();
	}

	return true;
}

bool 
AosVLanTester::runAddDev()
{
	// 
	// vlan device add <interface> <vlantag>
	//
	
	OmnString cmd = "vlan device add ";
	
	//gen correct cli(80%)
	if(RAND_INT(1,100) <= 80 && (mDevNum < eMaxDev))
	{
		OmnString interface;
		genInterface(interface);
		
		int id = RAND_INT(1, eMaxVlanTag);
		
		for(int i = 0;i < mDevNum - 1;i++)
	 	{
	 		if((mDevList[i].interface == interface) ||
	 			(mDevList[i].vlanid   == id))
	 		{
	 			return true;
	 		}
	 	}
		
		cmd << interface << " " << id;
		
		aosRunCli(cmd, true);
		
		//save to structure
		saveToDevList(interface, id);
		
	}	
	else
	{
		//50% generate an incorrect interface parm
		if(RAND_INT(1,100) <= 50)
		{
			OmnString interface;
			RAND_STR(interface,RAND_INT(1,32));
			if((interface == "incard") || (interface == "outcard"))	
			{
				return true;
			}
			int id = RAND_INT(1, eMaxVlanTag);
			cmd << interface << " " << id;
			aosRunCli(cmd, false);
		}
		//50% generate an incorrect id parm
		else
		{
			// 
			// Generate a correct interface but an incorrect vlan tag.
			// Incorrect VLAN Tags are:
			// 1. < 1
			// 2. > eMaxVlanTag
			// 3. Not an integer
			//
			OmnString interface;
			genInterface(interface);
			
			OmnString id = genIncorrectVlantag();

			cmd << interface << " " << id;
			aosRunCli(cmd, false);
		}
	
	}
	
	return true;
	
}

bool
AosVLanTester::runDelDev()
{
	// 
	// vlan device del <interface> <vlantag>
	//
	
	OmnString cmd = "vlan device del ";
	
	//80%gen a correct parms
	if(RAND_INT(1,100) <=80 && (mDevNum > 0))
	{
		int index = RAND_INT(0,mDevNum-1);
		cmd << mDevList[index].interface << " " << mDevList[index].vlanid;
		
		aosRunCli(cmd, true);
		delFromDevList(index);
	}	
	//20% gen an incorrect parms
	else
	{
		
		if(RAND_INT(1,100) <= 50)
		{
			OmnString interface;
			RAND_STR(interface,RAND_INT(1,32));
			if((interface == "incard") || (interface == "outcard"))
			{
				return true;	
			}
			
			int id = RAND_INT(1, eMaxVlanTag);	
			
			cmd << interface << " " << id;
			aosRunCli(cmd, false);
		}		
		else
		{
			OmnString interface;
			genInterface(interface);
	
			OmnString id = genIncorrectVlantag();
			
			cmd << interface << " " << id;
			aosRunCli(cmd, false);	
		}
		
	}
	
	return true;
	
}

bool
AosVLanTester::runSetIp()
{
	// 
	// vlan set ip <interface> <vlantag> <addr>
	//
	
	OmnString cmd = "vlan set ip "	;
	
	//gen correct parms
	if(RAND_INT(1,100) <= 80 && (mDevNum > 0))
	{
		int index = RAND_INT(0,mDevNum-1);
		
		cmd << mDevList[index].interface << " "
			<< mDevList[index].vlanid	 << " ";
		
		OmnString ipaddr = OmnRandom::nextStrIP();
		
		cmd << ipaddr;
		aosRunCli(cmd, true);
		
		saveToDevList(index, ipaddr);
	}
	else
	{
		// 
		// Generate incorrect command. For each parm, we randomly
		// determine whether to generate a correct one or incorrect
		// one. 
		//
		bool commandCorrect = true;
		OmnString interface; 
		OmnString id = ""; 
		OmnString ipaddr;

		if (RAND_INT(1, 100) <= 60)
		{
			// 
			// Generate an incorrect interface
			//
			interface = OmnRandom::nextNoSpaceStr(1,32);
			commandCorrect = false;
		}
		else
		{
			// 
			// Generate a correct interface
			//
			int index = RAND_INT(0,mDevNum-1);
			interface = mDevList[index].interface;
			
		}

		if (RAND_INT(1, 100) <= 60)
		{
			// 
			// Generate an incorrect vlan tag 
			//
			
			id = genIncorrectVlantag();
			commandCorrect = false;
		}
		else
		{
			// 
			// Generate a correct vlan tag
			//
			int index = RAND_INT(0,mDevNum-1);
			id << mDevList[index].vlanid; 

		}

		if (RAND_INT(1, 100) <= 60)
		{
			// 
			// Generate an incorrect address
			//
			
			ipaddr = OmnRandom::nextNoSpaceStr(1,32);
			commandCorrect = false;
			
		}
		else
		{
			// 
			// Generate a correct interface
			//
			ipaddr = OmnRandom::nextStrIP();
		}

		if (commandCorrect)
		{
			// 
			// Select a pair (interface, vlantag) from the list
			// but generate an incorrect address
			//
			int index = RAND_INT(0,mDevNum-1);
		
			interface = mDevList[index].interface;
			id << mDevList[index].vlanid;
			
			ipaddr 	  = OmnRandom::nextNoSpaceStr(1,32);
		}

		cmd << interface << " " << id << " " << ipaddr;
		aosRunCli(cmd, false);
	}
	
	return true;
	
}

bool
AosVLanTester::runSwitchStat()
{
	//
	//vlan switch status <on/off>
	//
	
	OmnString cmd = "vlan switch status ";
	
	//generate correct cli
	if(RAND_INT(1,100) <= 80)
	{
		if(isSwitchStatus == false)	
		{
			cmd << "on";
			aosRunCli(cmd, true);
			isSwitchStatus = true;
		}
		else
		{
			cmd << "off";
			aosRunCli(cmd, true);
			isSwitchStatus = false;
		}
	
	}
	//else generate incorrect cli
	else
	{
		OmnString stat = OmnRandom::nextNoSpaceStr(1,32);	
		if(stat == "on" || stat == "off")
		{
			return true;
		}
		
		cmd << stat;
		aosRunCli(cmd, false);
	}
	
	return true;
}

bool
AosVLanTester::runSwitchSet()
{
	//
	//vlan switch set <vlantag> <switch_port>
	//
	
	OmnString cmd = "vlan switch set ";
	
	//gen correct parms
	if(RAND_INT(1,100) <= 80 && (mDevNum > 0))
	{
		int index = RAND_INT(0,mDevNum-1);
		
		unsigned int port = RAND_INT(mMinSwitchPort, mMaxSwitchPort);

		cmd << mDevList[index].vlanid << " " << port;
		aosRunCli(cmd, true);
		
	}
	//generate incorrect parms
	else
	{
		//50% incorrect vlantag
		if(RAND_INT(1,100) <= 50)
		{
			OmnString id = genIncorrectVlantag();
			
			unsigned int port = RAND_INT(mMinSwitchPort, mMaxSwitchPort);
			
			cmd << id << " " << port;
			aosRunCli(cmd,false);
		}
		//50% incorrect switch port
		else
		{
			int index = RAND_INT(0,mDevNum-1);
			
			unsigned int port = RAND_INT(mMaxSwitchPort+1, 32767);
			
			cmd << mDevList[index].vlanid << " " << port;
			aosRunCli(cmd,false);
		}
		
	}
	
	return true;
}


bool
AosVLanTester::runSwitchReset()
{
	//
	//vlan switch reset
	//
		
	OmnString cmd = "vlan switch reset";
	aosRunCli(cmd, true);
	
	return true;
}

bool
AosVLanTester::runClearConfig()
{
	//
	//vlan clear config
	//
	
	OmnString cmd = "vlan clear config";
	aosRunCli(cmd, true);
	
	//clear current structure
	clearDevList();	
	
	return true;
}

bool
AosVLanTester::runSaveConfig()
{
	//
	//vlan save config
	//
	
	OmnString cmd = "vlan save config";
	aosRunCli(cmd, true);
	
	//save current devlist to saved-devlist
	saveDevList();
	
	return true;	
	
}

bool
AosVLanTester::runLoadConfig()
{
	//
	//vlan load config
	//
	
	OmnString cmd = "vlan load config";
	aosRunCli(cmd, true);
	
	//load saved-devlist to current devlist
	loadDevList();	
	
	return true;
	
}

bool
AosVLanTester::runShowConfig()
{
	//
	//vlan show config
	//
	
	OmnString cmd = "vlan show config";
	aosRunCli(cmd, true);
	
	return true;	
	
}


bool 
AosVLanTester::genInterface(OmnString &interface)
{
	//
	//randomly generate a interface from "incard" and "outcard"
	//
	
    OmnString intf[2];
    intf[0] = "incard";
    intf[1] = "outcard";
    
    int i = RAND_INT(0,1);
    interface = intf[i];
    
    return true;    
}

bool
AosVLanTester::saveToDevList(OmnString &intf, int &id)
{
	//
	//save from parms to devlist
	//
	
	for(int i = 0;i < mDevNum; i++)
	{
		if((mDevList[i].interface == intf) && (mDevList[i].vlanid == id))
		{
			return true;	
		}
	}			
	
	mDevList[mDevNum].interface = intf;
	mDevList[mDevNum].vlanid    = id  ;		
	
	mDevNum++;
	return true;
	
}

bool
AosVLanTester::saveToDevList(const int &index, OmnString &ipaddr)
{
	mDevList[index].ip = ipaddr;
	
	return true;
}

bool
AosVLanTester::delFromDevList(const int &index)
{

    // move back the rule
    for(int i = index;i < mDevNum - 1;i++)
    {
        mDevList[i] = mDevList[i+1];
    }
    mDevList[mDevNum-1].clear();
    mDevNum -= 1;
    
    return true;		
}

bool
AosVLanTester::saveDevList()
{
	for(int i = 0;i < eMaxDev; i++)
	{	
		mSaveList[i] = mDevList[i];
	}
	
	mSaveNum = mDevNum;
	
	return true;
	
}
	
bool
AosVLanTester::loadDevList()
{
	for(int i = 0;i < eMaxDev;i++)
	{
		mDevList[i] = mSaveList[i];
	}
	
	mDevNum = mSaveNum;
	return true;	
	
}

bool
AosVLanTester::clearDevList()
{
	for(int i = 0;i < mDevNum - 1;i++)
	{
		mDevList[i].clear();
	}
	
	mDevNum = 0;
	
	return true;
	
	
}

OmnString
AosVLanTester::genIncorrectVlantag()
{
	//
	//generate an incorrect vlantag,randomly select from the following conditions
    //   1. vlantag < 0
	//   2. vlantag > 4095
	//   3. a non-space string		
	int index = RAND_INT(1,3);
	long i;
	OmnString vlantag = "";
	switch(index)
	{
	case 1:
		i = RAND_INT(-32768,0);
		vlantag << i; 
		break;
	case 2:
		i = RAND_INT(eMaxVlanTag+1,32767);
		vlantag << i;
		break;
	case 3:
		vlantag = OmnRandom::nextNoSpaceStr(1,32);
		break;
	}	
	
	return vlantag;	
}

	
