////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MacTester.cpp
// Description:
// mac
//
// 1)	mac set dev mac     Percent 30   
// 2)	mac back set  Percent 30         
// 3)	mac show config  Percent 20      
// Modification History:           
// 	11/16/2006	Created by Chen Ding
//                                 
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/MacTester/MacTorturer.h"
#include "KernelInterface/CliSysCmd.h"

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

int devNum = 0;

static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  35</Element>"		// 	1.	mac set dev mac     Percent 30                 
	        "<Element>2,  35</Element>"		// 	2.	mac back set  Percent 30                       
	        "<Element>3,  30</Element>"		// 	3.	mac show config  Percent 20      
		"</Elements>";


AosMacTorturer::AosMacTorturer()
{
}


bool AosMacTorturer::start()
{
	OmnBeginTest << "Test SSL Server";
	
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef,100);

	int count = 0;
	bool rslt;
	while (count < sgCount)
	{
		cout <<"========================== Now run the :"<<count<<" time =================================================="<<endl;
		int index = cliSelector.nextInt();
    
		switch (index)
		{
		case eMacSet:
		     rslt = runMacSet();
		     break;
		case eMacBackSet:
			 rslt = runMacBackSet();
			 break;
		case eMacShowConfig:  
			 rslt = runMacShowConfig();
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

bool AosMacTorturer::runMacSet()
{
	//mac set dev mac
	OmnString devname;
	OmnString rslt;
	OmnString mac;
	OmnString oldmac;
	OmnString cmd;
	bool isCorrect;

	cmd << "mac set " ;
	//80% gen correct dev
	bool devStatus = OmnRandom::nextInt(0, 100) < 80;
	genDevName(devStatus, devname);
	
	//80% gen correct mac 
	bool macStatus = OmnRandom::nextInt(0, 100) < 80;
	genMac(macStatus, mac);

	isCorrect = devStatus && macStatus;
	cmd << devname << " " << mac;
	aosRunCli_getStr(cmd, isCorrect, rslt);

	if (isCorrect)
	{
		getCurrentMac(devname, oldmac);
		//add  to struct
		addToArry(devname, mac, oldmac);
		return true;
	}

	return true;
}

bool AosMacTorturer::runMacBackSet()
{
	//mac back set devname
	//correct 70%
	//choose a dev from macSave Array 
	OmnString cmd;
	OmnString dev;
	OmnString rslt;
	OmnString tmpMac;
	cmd << "mac back set ";
	//cout << "run" << cmd << endl;
	int index;
	if (OmnRandom::nextInt(0, 100) > 30 && devNum >0)
	{
	     // gen a correct dev
        index = OmnRandom::nextInt(0, devNum-1);
       	dev = macSave[index].mDev;
 		cmd << dev;
 		tmpMac = macSave[index].mOldMac;
 		macSave[index].mOldMac = macSave[index].mMac;
 		macSave[index].mMac = tmpMac;
		aosRunCli_getStr(cmd, true, rslt);
		
		return true;
	}
		
	while(1)
	{
		bool isEqual = false ;
		dev = OmnRandom::nextNoSpaceStr(1, 16);
		for (int i=0; i<devNum; i++)
		{
			if(dev == macSave[i].mDev) 
				isEqual = true;
		}

		if(!isEqual)
		break;
	}
	aosRunCli_getStr(cmd, false, rslt);
		
	return true;
}

bool AosMacTorturer::runMacShowConfig()
{
	//
	//
	//mac show config
	//
/*	OmnString cmd;
	OmnString rslt;
	char showBuf[1024];
	char *tok = "\n";
	char *p;
	cmd = "mac show config";
	
	showDevNum = 0;
	char mac[24];
	char oldMac[24];
	char devName[32];
	//cout << "Run " << cmd << endl;
	aosRunCli_getStr(cmd, true, rslt);
	memset(showBuf, 0, sizeof(showBuf));
	
	strncpy(showBuf, rslt.getBuffer(), sizeof(showBuf));
	
	p = strtok(showBuf, tok);
	if ( p == "")
	{
		return false;
	}
	 
	if (strcmp(p, "Ethernet interface mac information"))
		return false;
	
	p = strtok(NULL, tok);
	
	while (p)
	{
	
		if (!strstr(p, "------------------------------------"))
		{
			//cout << "nothing to do\n" << endl;		
		}
		else if (!strstr(p, "\t\t\tdev            mac           oldMac"))
		{
			//cout << "nothing to do\n" << endl;		
		}
		else if (!strstr(p, "\t\t\t----------------------------------------"))
		{
			//cout << "nothing to do\n" << endl;		
		}
		else 
		{
			sscanf(p, "%s %s %s", devName, mac, oldMac);
			macShow[showDevNum].mDev = devName;
			macShow[showDevNum].mDev = devName;
			macShow[showDevNum].mDev = devName;
			
			showDevNum++;
		}
  	
  		p = strtok(NULL, tok);	  		
  	} 	
  	
  	if ( devNum != showDevNum)
  	{
  		//cout << "error\n" << endl;
  		return false;
  	}

	for(int i=0; i<devNum-1; i++)
	{
		for (int j=0; j<showDevNum; j++)
		{
			if (macSave[i].mDev == macShow[j].mDev)
			{
				macSave[i].mDev = macShow[j].mDev = "";
			}
		}
	}

	for(int i=0; i<devNum-1; i++)
	{
		if (macSave[i].mDev != "")
		{
			cout << "error!\n" << endl;
		}
	}
	
	for (int j=0; j<showDevNum; j++)
	{
		if (macShow[j].mDev != "")
		{
			cout << "error!\n" << endl;
		}
	}
*/	
	return true;
}

bool AosMacTorturer::genDevName( bool isCorrect, OmnString &devName)
{
	char *dev[2];
	dev[0] = "outcard";
	dev[1] = "outcard";
	int  index;
	
	if (isCorrect)
	{
		index = RAND_INT(0, 1);
		devName = dev[index];
	}
	else
	{
		devName = OmnRandom::nextNoSpaceStr(1, 16);
		for (int i=0; i<2; i++)
		{
			if(devName == dev[i]) 
			devName = "wrong dev";
		}
					
	}
	return true;
}

bool AosMacTorturer::genMac( bool isCorrect, OmnString &mac)
{
      if (isCorrect)
      { 
              mac = OmnRandom::nextNetMac();
//cout << "MAC:" << mac << endl;
	} 
      else
      { 
              mac = OmnRandom::nextNoSpaceStr(3, 20);
              
      } 
      return true;
}

bool AosMacTorturer::getCurrentMac(OmnString &devname, OmnString & oldmac)
{
	 OmnString systemCmd;
	 OmnString rslt;
	 int curPos = 0;
     systemCmd << "ifconfig |grep "
               << devname   << "|awk \'$4=\"HWaddr\" {print $5}\'  ";
     OmnCliSysCmd::doShell(systemCmd, rslt);
     curPos = rslt.getWord(curPos,oldmac);    
     return true;  
}                                             
    
bool AosMacTorturer::addToArry(OmnString &devname, OmnString &mac, OmnString &oldmac)
{
	OmnString tmpMac;
	for (int i=0;i<4;i++)
	{
		if (macSave[i].mDev = devname)
		{
			tmpMac = macSave[i].mOldMac;
			macSave[i].mOldMac = oldmac;
			macSave[i].mMac = mac;
		}
	
		for (int i=0;i<4;i++)
		{
			if ( macSave[i].mDev == "")
			{
				macSave[i].mDev = devname;
				macSave[i].mOldMac = oldmac;
				macSave[i].mMac = mac;
				devNum++;
				return true;
			}
		}
		
	}
	return true;
}
  
