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
// Below are the list of all the CLI commands:
//
// Bridge 
// 1. bridge group add <bridge-name>                  
// 2. bridge group del <bridge-name>                  
// 3. bridge group set ip <bridge-name> <ip>          
// 4. bridge group member add <bridge-name> <dev-name>
// 5. bridge group member del <bridge-name> <dev-name>
// 6. bridge group member set ip <dev-name> <ip>
// 7. bridge show config
// 8. bridge clear config
// 9. bridge group up <bridge-name>
// 10.bridge group down <bridge-name>
	
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SystemCli/BridgeTester/BridgeTester.h"

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
#include "Util1/Wait.h"
#include "UtilComm/Ptrs.h"
#include "KernelSimu/timer.h"
#include <string.h>

const int eMaxAppNameLen = 32;
const int eMaxTooLongAppNameLen = 64;

int mNumName = 0;

//#define DEBUG

// 
// CLI Command selector definition
//
static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  20</Element>"		// 1)	bridge group add <bridge-name> 20%                  
	        "<Element>2,  10</Element>"		// 2)	bridge group del <bridge-name> 10%                  
	        "<Element>3,  10</Element>"		// 3)	bridge group set ip <bridge-name> <ip> 10%          
	        "<Element>4,  20</Element>"		// 4)	bridge group member add <bridge-name> <dev-name> 20%
	        "<Element>5,  10</Element>"		// 5)	bridge group member del <bridge-name> <dev-name> 10%
	        "<Element>6,  10</Element>"		// 6)	bridge group member set ip <dev-name> <ip>  10%     
	        "<Element>7,  5</Element>"		// 7)	bridge show config 5%                               
            "<Element>8,  5</Element>"	    // 8)	bridge clear config 5%                              
	        "<Element>9,  5</Element>"		// 9)	bridge group up <bridge-name> 5%                    
	        "<Element>10, 5</Element>"		// 10)	bridge group down <bridge-name>5%
		"</Elements>";

AosBridgeTester::AosBridgeTester()
{
	mName = "BridgeTester";
	mNumBridge = 0;
}


bool 
AosBridgeTester::start()
{
	// 
	// Test default constructor
	//
	initialize();
	runBridgeClearConfig();
	basicTest();
	return true;
}

bool 
AosBridgeTester::basicTest()
{
	OmnBeginTest << "Test Bridge";
	
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef,100);

	int count = 0;
	bool rslt;
	while (count < 100)
	{
		OmnWait::getSelf()->wait(0,2000);
		int index = cliSelector.nextInt();

		switch (index)
		{
		case eBridgeAdd : 			
			 rslt = runBridgeAdd();
			 break;
		case eBridgeDel :			
			 rslt = runBridgeDel();
			 break;
		case eBridgeSetIp : 		
			 rslt = runBridgeSetIp();
			 break;
		case eBridgeMemberAdd :	
			 rslt = runBridgeMemberAdd();
			 break;
		case eBridgeMemberDel :	
			 rslt = runBridgeMemberDel();
			 break;
		case eBridgeMemberSetIp : 	
			 rslt = runBridgeMemberSetIp();
			 break;
		case eBridgeShowConfig :
			 rslt = runBridgeShowConfig();
			 break;
		case eBridgeClearConfig :	
			 rslt = runBridgeClearConfig();
			 break;
		case eBridgeGroupUp :	
			 rslt = runBridgeGroupUp();
			 break;
		case eBridgeGroupDown : 	
			 rslt = runBridgeGroupDown();
			 break;

		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			 break;
		}
		if (!rslt)
		{
			cout << "Some error occur when run cli command!" << endl;
			return false;
		}

		count++;
	}

	return true;
}


bool 
AosBridgeTester::runBridgeAdd()
{
	//
	// bridge group add <bridge-name> 
	//
	OmnString name;
	OmnString rslt;
	OmnString cmd;
	cmd = "bridge group add ";
	if (OmnRandom::nextInt(0, 100) < 90)
	{
		// 
		//	Determin a name; 
		//

		genName(name, 1, eMaxBridgeNameLen);
		
		if (nameExist(name))
		{
			// 
			// It is already in the list. Ignore. 
			//
			return true;
		}
	
		cmd << name;
		aosRunCli_getStr(cmd, true, rslt);
		cout << cmd << endl;
		//add name to struct
		
		addBridgeName(name);
		return true;
	}
	
	// 
	// There are two error conditions: 
	//  1. Name too long
	//  2. Name already exist
	//
	if (mNumBridge > 0)
	{
		// 
		// Select a name from exist name list
		//
		int index = RAND_INT(0, mNumBridge-1);
		name = bridge[index].mBridgeName;
	}
	else
	{
		return true;
	}

	cmd << name;
	aosRunCli_getStr(cmd, false, rslt);

	return true;
}


bool 
AosBridgeTester::runBridgeDel()
{
	//                
	//bridge group del <bridge-name>                  
	//
	
	//
	// 1. remove a exist name
	// 2. remove a unexist name
	//

	// 1. remove a exist name
	OmnString name;
	OmnString rslt;

	OmnString cmd("bridge group del ");
	if (RAND_INT(0, 100) < 80 && mNumBridge > 0)
	{

		int bridgeIndex = RAND_INT(0, mNumBridge-1);

		name = bridge[bridgeIndex].mBridgeName;

		cmd << name;

		aosRunCli_getStr(cmd, true, rslt);
		cout << cmd << endl;
		
		delBridgeName(bridgeIndex);

		return true;
	}
		
	// 2. remove a unexist name

	genName(name, 1, eMaxBridgeNameLen);
	
	if (nameExist(name))
	{
		return true;
	}

	
	cmd << name;
		
	aosRunCli_getStr(cmd, false, rslt);

	return true;
}

bool 
AosBridgeTester::runBridgeSetIp()
{
	//
	//bridge group set ip <bridge-name> <ip>  
	//
	
	
	
	//1. correct
	OmnString name;
	OmnString ip;
	OmnString cmd;
	OmnString rslt;
	
	cmd = "bridge group set ip ";
	
	if (RAND_INT(0, 100) > 30 && mNumBridge > 0)
	{
		//select a exist name 
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		
		// generate a ipaddr
		genIpAddr(true, ip);
		
		cmd << name << " " << ip;
		aosRunCli_getStr(cmd, false, rslt);
		cout << cmd << endl;
		
		addBridgeIp(bridgeIndex, ip);
		return true;
	}
	//2.  incorrect
	//2.1 name no exist
	//2.2 the ip address is invald
	int randNum = RAND_INT(0, 100);
	if (randNum > 70 && mNumBridge > 0)
	{
		genName(name, 1, eMaxBridgeNameLen);
	
		if (nameExist(name))
		{
			return true;
		}
		genIpAddr(true, ip);
	}
	else if (randNum > 40)
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		genIpAddr(false, ip);
	}
	else
	{
		genName(name, 1, eMaxBridgeNameLen);
	
		if (nameExist(name))
		{
			return true;
		}
		genIpAddr(false, ip);
	}

	cmd << name << " " << ip;
	aosRunCli_getStr(cmd, false, rslt);
	
	return true;
}

bool 
AosBridgeTester::runBridgeMemberAdd()
{
	//
	//bridge group member add <bridge-name> <dev-name>
	//
	
	//1. correct
	//2. incorrect
	
	
	//1. correct
	OmnString name;
	OmnString devName;
	OmnString cmd;
	OmnString rslt;
	
	cmd = "bridge group member add ";
	if (RAND_INT(0, 100) > 80 && mNumBridge > 0)
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		
		// generate a device name
		genDevName(devName, true);
		
		cmd << name << " " << devName;
		aosRunCli_getStr(cmd, true, rslt);
		cout << cmd << endl;
		
		addBridgeMem(bridgeIndex, devName);
		return true;
	}
	
	//
	//2. incorrect
	//
	//2.1. brigde name not exist
 	//2.2 dev not exist;
 	int randNum = RAND_INT(0, 100);
 	if (randNum > 70)
 	{
 		genName(name, 1, eMaxBridgeNameLen);
		
		if (nameExist(name))
		{
			return true;
		}
		genDevName(devName, true);
	}
	else if (randNum > 40)
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		
		genDevName(devName, false);
	}
	else
	{
		genName(name, 1, eMaxBridgeNameLen);
		
		if (nameExist(name))
		{
			return true;
		}
		genDevName(devName, false);
	} 	
	
	cmd << name << " " << devName;
	aosRunCli_getStr(cmd, false, rslt);
	return true;
}

bool 
AosBridgeTester::runBridgeMemberDel()
{
	//
	//bridge group member del <bridge-name> <dev-name>
	//
	
	//1. correct
	//2. incorrect
	
	
	//1. correct
	OmnString name;
	OmnString devName;
	OmnString cmd;
	OmnString rslt;
	
	cmd = "bridge group member del ";
	if (RAND_INT(0, 100) > 30 && mNumBridge > 0)
	{
		
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		
		if (bridge[bridgeIndex].mNumDevice > 0)
		{
			int devIndex = RAND_INT(0, bridge[bridgeIndex].mNumDevice-1);
			devName = bridge[bridgeIndex].mDev[devIndex].mDevName;
			name = bridge[bridgeIndex].mBridgeName;	
			
			cmd << name << " " << devName;
			aosRunCli_getStr(cmd, false, rslt);
		
			delBridgeMem(bridgeIndex, devIndex);
			return true;
		}
		
		return true;		
	}
	
	//
	//2. incorrect
	//
	//2.1. brigde name not exist
 	//2.2 dev not exist;
 	int randNum = RAND_INT(0, 100);
 	if (randNum > 60)
 	{
 		genName(name, 1, eMaxBridgeNameLen);
		
		if (nameExist(name))
		{
			return true;
		}
		
		int devIndex = RAND_INT(0, devNum-1);
		devName = devArray[devIndex];
	}
	else
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		
		Twice:
		int devIndex = RAND_INT(0, devNum-1);
		devName = devArray[devIndex];
		
		for (int i=0; i<bridge[bridgeIndex].mNumDevice-1; i++)
		{
			if (devName == bridge[bridgeIndex].mDev[i].mDevName)
				goto Twice;
		}
		genDevName(devName, false);	
	}
	
	cmd << name << " " << devName;
	aosRunCli_getStr(cmd, false, rslt);

	return true;
}

bool 
AosBridgeTester::runBridgeMemberSetIp()  
{
	//
	//bridge group member set ip <dev-name> <ip>
	//
	//1. correct
	// 1.1 select a correct bridge, and bridge must have dev, 
    // 
	//2.  incorrect
	//2.1 select a dev, no exist in all bridge
	//2.2 the ipaddr is invald
	

  
	OmnString dev;
	OmnString ip;
	OmnString cmd;
	OmnString name;
	OmnString rslt;
	
	cmd = "bridge member set ip ";
	
	if (RAND_INT(0, 100) > 30 && mNumBridge > 0)
	{
		//select a exist name 
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		
		if (bridge[bridgeIndex].mNumDevice > 0)
		{
			int devIndex = RAND_INT(0, bridge[bridgeIndex].mNumDevice-1);
			dev = bridge[bridgeIndex].mDev[devIndex].mDevName;
			
			// generate a ipaddr
			genIpAddr(true, ip);
		
			cmd << dev << " " << ip;
			aosRunCli_getStr(cmd, true, rslt);
			cout << cmd << endl;
			addMemberIp(bridgeIndex, devIndex, ip);
		}		
		
		
		return true;
	}
	//2.  incorrect
	//2.1 dev no exist
	//2.2 the ip address is invald
	int randNum = RAND_INT(0, 100);
	if (randNum > 70 && mNumBridge > 0)
	{
		genName(dev, 1, eMaxBridgDevLen);
	
		if (devExist(dev))
		{
			return true;
		}
		
		genIpAddr(true, ip);
	}
	else if (randNum > 40)
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		genIpAddr(false, ip);
	}
	else
	{
		genName(dev, 1, eMaxBridgDevLen);
	
		if (devExist(dev))
		{
			return true;
		}
		genIpAddr(false, ip);
	}

	cmd << name << " " << ip;
	aosRunCli_getStr(cmd, false, rslt);
	

	return true;
}

bool 
AosBridgeTester::runBridgeShowConfig() 
{
	//
	//bridge show config
	//
/*	
	OmnString cmd;
	OmnString rslt;
	char showBuf[1024];
	char *tok = "\n";
	char *p;
	cmd = "bridge show config";
	
	char ip[20];
	char bridgeName[64];
	bool status;
	char devName[64];
	char mIp[20];
	char devIp[20];
		
	
	mShowNumBridge = 0;
	
	aosRunCli_getStr(cmd, true, rslt);
	return true;

	memset(showBuf, 0, sizeof(showBuf));
	
	strncpy(showBuf, rslt.getBuffer(), sizeof(showBuf));
	
	p = strtok(showBuf, tok);
	 
	if (!strcmp("bridge       information", p))
		return false;
	
	p = strtok(NULL, tok);
	
	while (p)
	{
		if (!strstr(p, "----------------------------"))
		{
			cout << "nothing to do\n" << endl;		
		}
		else if (!strstr(p, "            bridge name: "))
		{
			sscanf(p, "%*sbridge name:%s", bridgeName);
			showBridge[mShowNumBridge].mBridgeName = bridgeName;
			showBridge[mShowNumBridge].mNumDevice = 0;
			mShowNumBridge++;
		}
		else if (!strstr(p, "                    ip : "))
		{
			sscanf(p, "%*sip :%s", ip);
			showBridge[mShowNumBridge].mBridgeIp = ip;
		}
		else if (!strstr(p,  "                status : "))
		{
			sscanf(p, "%*sstatus :%s", status);
			showBridge[mShowNumBridge].mStatus = status;
		}
		else if (!strstr(p, "\t\t\tDev\tIP"))
		{
			cout << "nothing to do\n" << endl;
		}
		else if (!strstr(p, "\t\t\t----------------------------"))
		{
			cout << "nothing to do\n" << endl;
		}
		else 
		{
			sscanf(p, "%s %s", devName, devIp);
			showBridge[mShowNumBridge].mDev[showBridge[mShowNumBridge].mNumDevice].mDevName = devName;	
			showBridge[mShowNumBridge].mDev[showBridge[mShowNumBridge].mNumDevice].mIp = mIp;
			showBridge[mShowNumBridge].mNumDevice++;
		}
  	
  		p = strtok(NULL, tok);	  		
  	}
  	
  	
  	if (mNumBridge != mShowNumBridge)
  	{
  		cout << "error\n" << endl;
  		return false;
  	}

	for(int i=0; i<mNumBridge-1; i++)
	{
		for (int j=0; j<mShowNumBridge; j++)
		{
			if (bridge[i].mBridgeName == showBridge[j].mBridgeName)
			{
				bridge[i].mBridgeName = showBridge[j].mBridgeName = "";
			}
		}
	}


	for(int i=0; i<mNumBridge-1; i++)
	{
		if (bridge[i].mBridgeName != "")
		{
			cout << "error!\n" << endl;
		}
	}
	
	for (int j=0; j<mShowNumBridge; j++)
	{
		if (showBridge[j].mBridgeName != "")
		{
			cout << "error!\n" << endl;
		}
	}
*/			
	return true;
}

bool 
AosBridgeTester::runBridgeClearConfig()  
{
	OmnString rslt;
	//bridge clear config
	//
	OmnString cmd("bridge clear config");
	aosRunCli_getStr(cmd, true, rslt);

	cout << cmd << endl;
	return true;
}

bool 
AosBridgeTester::runBridgeGroupUp()
{
	//
	//bridge group up <bridge-name>
	//
	
	//1. correct
	//2. iscorrect
	
	OmnString name;
	OmnString cmd;
	OmnString rslt;
	
	cmd = "bridge group up ";
	if (RAND_INT(0, 100) > 30 && mNumBridge > 0)
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		
		bridge[bridgeIndex].mStatus = true;
		aosRunCli_getStr(cmd, true, rslt);
		return true;
	}
	
	
	genName(name, 1, eMaxBridgeNameLen);
	
	if (nameExist(name))
	{
		return true;
	}
	
	aosRunCli_getStr(cmd, true, rslt);
	cout << cmd << endl;

	return true;	
}

bool 
AosBridgeTester::runBridgeGroupDown()
{
	//
	//bridge group down <bridge-name>
	//
		
	//1. correct
	//2. iscorrect
	
		//1. correct
	//2. iscorrect
	
	OmnString name;
	OmnString cmd;
	OmnString rslt;
	
	cmd = "bridge group down ";
	if (RAND_INT(0, 100) < 80 && mNumBridge > 0)
	{
		int bridgeIndex = RAND_INT(0, mNumBridge-1);
		name = bridge[bridgeIndex].mBridgeName;
		
		bridge[bridgeIndex].mStatus = false;
		aosRunCli_getStr(cmd, true, rslt);
		return true;
	}
	
	
	genName(name, 1, eMaxBridgeNameLen);
	
	if (nameExist(name))
	{
		return true;
	}
	
	aosRunCli_getStr(cmd, true, rslt);
	cout << cmd << endl;

	return true;
}


bool
AosBridgeTester::genName(OmnString &name, int minLen, int maxLen)
{
	name = OmnRandom::nextLetterStr(minLen, maxLen);
	return true;
}

bool
AosBridgeTester::nameExist(OmnString &name)
{
	for (int i=0; i<mNumBridge; i++)
	{
		if (bridge[i].mBridgeName == name)
		{
			return true;
		}
	}
	return false;
}

bool
AosBridgeTester::addBridgeName(OmnString &name)
{
	if (mNumBridge >= eMaxBridgeNum)
	{           
		return false;
	}           
                
	bridge[mNumBridge].mBridgeName = name;
	bridge[mNumBridge].mNumDevice = 0;
	mNumBridge++; 
	return true;
}


bool
AosBridgeTester::addBridgeIp(int bridgeIndex, OmnString &ip)
{
	bridge[mNumBridge].mBridgeIp = ip;
	return true;	
}


bool
AosBridgeTester::genIpAddr(bool isCorrect, OmnString &ip)
{
	if (isCorrect)
	{
		ip = OmnRandom::nextStrIP();
	}
	else
	{
		ip = OmnRandom::nextNoSpaceStr(1, 20);
	}
	return true;	
}



bool
AosBridgeTester::genDevName(OmnString &devName, bool isCorrect)
{
	if (isCorrect)
	{
		//read form the proc file system
		int devIndex = RAND_INT(0, devNum-2);
		devName = devArray[devIndex];
	}
	else
	{
	Again:
		devName = OmnRandom::nextNoSpaceStr(1, 16);
	
	
		for (int i=0; i<devNum; i++)
		{
			if(devName == devArray[i]) 
				goto Again;
		}
					
	}
	return true;
}

bool
AosBridgeTester::initialize()
{
	devNum = 0;
	char readBuf[512];
	char devBuf[32], dev[8];
	int j, n, i;
	i = 0;
	memset(readBuf, 0, sizeof(readBuf));
	memset(devBuf, 0, sizeof(devBuf));
	memset(dev, 0, sizeof(dev));
	FILE *fp = fopen("/proc/net/dev", "r");
	fgets(readBuf, sizeof(readBuf), fp);
	
	while(fgets(readBuf, sizeof(readBuf), fp))
	{
		n = 0;
		i++;
		if (i < 2)
			continue;
		sscanf(readBuf,"%[^:]:%*s", devBuf);
		if (strstr(devBuf, "lo"))
			continue;

		for (j=0; j<sizeof(devBuf); j++)
		{
			if (devBuf[j] != ' ')
			{
				dev[n] = devBuf[j];
				n++;
			}
		}

		cout << dev << endl;
		devArray[devNum] = dev;
		devNum++;
		memset(readBuf, 0, sizeof(readBuf));
		memset(devBuf, 0, sizeof(devBuf));
		memset(dev, 0, sizeof(dev));
	} 
	devArray[devNum] = "incard";
	devNum++;
	devArray[devNum] = "outcard";
	devNum++;
		
	return true;
}		


bool
AosBridgeTester::addBridgeMem(int bridgeIndex, OmnString &devName)
{
	int devIndex = bridge[bridgeIndex].mNumDevice;
	bridge[bridgeIndex].mDev[devIndex].mDevName = devName;
	bridge[bridgeIndex].mNumDevice++;

	return true;
}

bool
AosBridgeTester::addMemberIp(int bridgeIndex, int devIndex, OmnString &ip)
{
	bridge[bridgeIndex].mDev[devIndex].mIp = ip;
	return true;
}

bool
AosBridgeTester::devExist(OmnString &devName)
{
	for(int i=0; i<mNumBridge-1; i++)
	{
		for(int j=0; j<bridge[i].mNumDevice-1; j++)
		{
			if (devName == bridge[i].mDev[j].mDevName)
				return true;
		}
	}
	return false;
}


bool
AosBridgeTester::delBridgeMem(int bridgeIndex, int devIndex)
{
	int index = bridge[bridgeIndex].mNumDevice;
	for(int i=devIndex; i<index-1; i++)
	{
		bridge[bridgeIndex].mDev[i] = bridge[bridgeIndex].mDev[i+1];
	}
	return true;
}

bool
AosBridgeTester::delBridgeName(int bridgeIndex)
{
	for (int i=bridgeIndex; i<(mNumBridge-1); i++)
	{
		bridge[i] = bridge[i+1];
	}
	mNumBridge--;
}

AosBridgeTester::Device::Device()
{
}

AosBridgeTester::Device::~Device()
{
}

AosBridgeTester::Bridge::~Bridge()
{
}

AosBridgeTester::Bridge::Bridge()
:
mNumDevice(0),
mStatus(false)
{
}


