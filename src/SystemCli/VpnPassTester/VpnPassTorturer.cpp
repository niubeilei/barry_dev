////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: VpnPassTester.cpp
// Description:
// VpnPass
//
// 1)	VpnPass set dev VpnPass     Percent 30   
// 2)	VpnPass back set  Percent 30         
// 3)	VpnPass show config  Percent 20      
// Modification History:           
// 	11/16/2006	Created by Chen Ding
//                                 
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/VpnPassTester/VpnPassTorturer.h"
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
	        "<Element>1,  35</Element>"		// 	1.	VpnPass set dev VpnPass     Percent 30                 
	        "<Element>2,  35</Element>"		// 	2.	VpnPass back set  Percent 30                       
	        "<Element>3,  30</Element>"		// 	3.	VpnPass show config  Percent 20      
		"</Elements>";


AosVpnPassTorturer::AosVpnPassTorturer()
{
}


bool AosVpnPassTorturer::start()
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
		case eFirewallL2tpPass:
		     rslt = runVpnL2tpPass();
		     break;
		case eFirewallPptpPass:
			 rslt = runVpnPptpPass();
			 break;
		case eFirewallIpsecPass:  
			 rslt = runVpnIpsecPass();
			 break;    
		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			 break;
		}
		count++;
	//	updateSeed();
	
	}                   
                        
	return true;        
}                                        

bool AosVpnPassTorturer::runVpnL2tpPass()
{
	
	OmnString com;
	OmnString rslt;
	
	OmnString cmd;
	cmd << "firewall l2tp_pass " ;
	
	bool comStatus = OmnRandom::nextInt(0, 100) < 70;
	//70% genrate a correct command
	genCom(comStatus, com);
	cmd << com;
    aosRunCli_getStr(cmd, comStatus, rslt);

	return true;
}

bool AosVpnPassTorturer::runVpnPptpPass()
{
	OmnString com;
	OmnString rslt;
	OmnString cmd;

	cmd << "firewall pptp_pass " ;
	
	bool comStatus = OmnRandom::nextInt(0, 100) < 70;
	//70% genrate a correct command
	genCom(comStatus, com);
	cmd << com;
    aosRunCli_getStr(cmd, comStatus, rslt);

	return true;

}

bool AosVpnPassTorturer::runVpnIpsecPass()
{
	OmnString com;
	OmnString rslt;
	OmnString cmd;

	cmd << "firewall ipsec_pass" ;
	
	bool comStatus = OmnRandom::nextInt(0, 100) < 70;
	//70% genrate a correct command
	genCom(comStatus, com);
	cmd << com;
    aosRunCli_getStr(cmd, comStatus, rslt);


}

bool AosVpnPassTorturer::genCom(bool isCorrect, OmnString & command)
{
	char *com[2];
	com[0] = "on";
	com[1] = "off";
	int  index;
	
	if (isCorrect)
	{
		index = RAND_INT(0, 2);
		index = RAND_INT(0, 1);
		command = com[index];
	}
	else
	{
	Again:
		command = OmnRandom::nextNoSpaceStr(1, 16);
		
		for (int i=0; i<2; i++)
		{
			if(command == com[i]) 
				goto Again;
		}
					
	}
	return true;
}	
