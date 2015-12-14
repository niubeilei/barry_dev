////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosRouterTorturer.cpp
// Description:
// Router Cli
//
// 1.	RouterSetStatus  		15%
// 2.	RouterAddEntry   		35%
// 3.	RouterDelEntry   		15%
// 4.	Routershowconfig 		10%
// 5.	RouterClearConfig		5%
// 6.	onfig 		10%
//# 7   RouterLoadConfig 		10%
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/RouterTester/RouterTorturer.h"

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
#define MAX_LEN	256

#define MIN(x,y) ((x)>(y)?(y):(x))

const int eMaxRouterNameLen = 32;




static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  15</Element>"		// 	1.	Router setstatus <on|off> 
			"<Element>2,  35</Element>"		// 	2.	Router add entry
			"<Element>3,  15</Element>"		// 	3.	Router del entry                 
	        "<Element>4,  10</Element>"		// 	4.	Router show config                         
	        "<Element>5,  5</Element>"		// 	5.	Router clear config                        
	        "<Element>6,  10</Element>"	    //  6.  Router save config                           
            "<Element>7,  10</Element>"		// 	7.	Router load config 
		"</Elements>";


AosRouterTorturer::AosRouterTorturer()
{
	mStatus = "off";
}


bool AosRouterTorturer::start()
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
		cout <<"========================== Now run the :"<<count+1<<" time =================================================="<<endl;
		int index = cliSelector.nextInt();

		switch (index)
		{
		case eRouterSetStatus:
		     rslt = runRouterSetStatus();
		     break;                                                                     
		case eRouterAddEntry:                        
			rslt = runRouterAddEntry();             
			break;                                   
		case eRouterDelEntry:                             
			rslt = runRouterDelEntry();                    
			break;                                   
		case eRouterShowConfig:                     
			rslt = runRouterShowConfig();           
			break;                               
		case eRouterClearConfig:                    
			rslt = runRouterClearConfig();          
			break;                               
		case eRouterSaveConfig:                
			rslt = runRouterSaveConfig();
			break;  
//		case eRouterLoadConfig:
//			rslt = runRouterLoadConfig();
//			break;               
		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			break;
		}
		count++;
	//	updateSeed();
	}

	return true;
}

bool AosRouterTorturer::runRouterSetStatus()
{
	//Router set stauts;
	OmnString status;
	OmnString rslt;
	OmnString cmd;
	bool isCorrect;
	cmd = "router set status ";
	isCorrect = true;
	
	if (OmnRandom::nextInt(0, 100) > 20)
	{
		status = RAND_BOOL()?"on":"off";
	    if ((mStatus == "off") && (status == "off"))
			isCorrect = false;
		else
			mStatus = status;
		
	    cmd << status ;
	    aosRunCli_getStr(cmd, isCorrect, rslt);
	}
	else
	{
	    status = OmnRandom::nextLetterStr(2, 6);
	    isCorrect = false;
	    cmd << status ;
	    aosRunCli_getStr(cmd, isCorrect, rslt);
	}

	return true;
}

bool AosRouterTorturer::runRouterAddEntry()
{
	//Router add Entry
	OmnString dIp;
	OmnString dMask;
	OmnString gateway;
	OmnString alias;
	OmnString dev;
	OmnString rslt;
	OmnString cmd;
	bool isCorrect;

	genName(alias, 1, eMaxRouterNameLen);
	cmd << "router add Entry ";

	//80% gen correct dip
	bool dIpStatus = RAND_INT(0, 100) < 80 ;
	genIpAddr(dIpStatus, dIp);

	//80% gen correct dmask
	bool dMaskStatus = RAND_INT(0, 100) < 80;
	genIpAddr(dMaskStatus, dMask);

	//80% gen correct gw
	bool gwStatus = RAND_INT(0, 100) < 80;
	genIpAddr(gwStatus, gateway);

	//80% gen correct dev	
	bool devStatus = RAND_INT(0, 100) < 80;
	genDevName(devStatus, dev);
		
	if (nameExist(alias))
	{
		cmd << alias << " " << dIp << " "<<dMask << " " << gateway;
		aosRunCli_getStr(cmd, false, rslt);
		return true;
	}
	
	isCorrect = dIpStatus && dMaskStatus && gwStatus;
	if (RAND_INT(0, 100) < 60)
	{
		cmd << alias << " " << dIp << " "<<dMask << " " << gateway;
		aosRunCli_getStr(cmd, isCorrect, rslt);
		//save the para
		if (isCorrect)
		{
			dev = "";
			addToSave(alias,dIp, dMask, gateway,dev);
			return true;
		}
	}

	isCorrect = dIpStatus && dMaskStatus && gwStatus && devStatus;
	cmd << alias << " " << dIp << " "<<dMask << " " << gateway << " " << dev ;
	aosRunCli_getStr(cmd, isCorrect, rslt);

	//save the params
	if (isCorrect)
	{
		addToSave(alias,dIp, dMask, gateway,dev);
		return true;
	}

	return true;
}

bool AosRouterTorturer::runRouterDelEntry()
{
	OmnString alias;
	OmnString rslt;
	OmnString cmd;
	genName(alias, 1, eMaxRouterNameLen);
	cmd << "router del Entry " << alias;\
	
	if (RAND_INT(0, 100) > 30 && mNumRouter > 0)
	{

		int RouterIndex = RAND_INT(0, mNumRouter-1);

		alias = Router[RouterIndex].mAlias;

		cmd << alias;

		aosRunCli_getStr(cmd, true, rslt);
		
		delRouter(RouterIndex);

		return true;
	}
		
	// 2. remove a unexist name

	genName(alias, 1, eMaxRouterNameLen);
	
	if (nameExist(alias))
	{
		return true;
	}

	cmd << alias;
	aosRunCli_getStr(cmd, false, rslt);

	return true;
} 
  
bool AosRouterTorturer::runRouterSaveConfig()
{
	//Router save config
	OmnString rslt;
	OmnString cmd;
	cmd << "router save config";

	aosRunCli_getStr(cmd, true, rslt);	
	return true;
	
}
	
bool AosRouterTorturer::runRouterLoadConfig()
{
	//Router load config
	OmnString rslt;
	OmnString cmd;
	cmd << "router load config";
	
	aosRunCli_getStr(cmd, true, rslt);
	return true;
}

bool AosRouterTorturer::runRouterClearConfig()
{
	//Router clear config
	OmnString rslt;
	OmnString cmd = "router clear config";
	
	aosRunCli_getStr(cmd, true, rslt);
	mStatus = "off";
	for (int i=0; i<Max_Num-1; i++)
	{
		Router[i].mAlias = "";     
		Router[i].mDip = "";       
		Router[i].mDmask = "";     
		Router[i].mGateWay = "";  
		
	}
	
	return true;
}

bool AosRouterTorturer::runRouterShowConfig()
{
//
/*	//Router show config
	//
	OmnString cmd;
	OmnString rslt;
	char rbuffer[2048];
	char tmp[MAX_LEN];
	
	cmd = "Router show config";
	
	
	char dns1[32];
	char dns2[32];
	char status[4];
	char username[32];
	char password[32];
	
	
	aosRunCli_getStr(cmd, true, rslt);
	memset(rbuffer, 0, sizeof(rbuffer));
	
	strncpy(rbuffer, rslt.getBuffer(), sizeof(rbuffer));
	
	strcpy(tmp,"");
    readBlock(rbuffer, "status :", "\n\tusername", tmp, MAX_LEN);
	//strcpy(Router_info->status, tmp);
	RouterShow.mStatus = tmp;
	//printf("            status : %s\n", Router_info->status, MAX_LEN);
    
	strcpy(tmp,"");
    readBlock(rbuffer, "username : ", "\n", tmp, MAX_LEN);
	//strcpy(Router_info->username, tmp);
	//printf("          username : %s\n", Router_info->username);

    RouterShow.mUsername = tmp;
    
	strcpy(tmp,"");
    readBlock(rbuffer, "password : ", "\n", tmp, MAX_LEN);
	//strcpy(Router_info->password, tmp);
	//printf("          password : %s\n", Router_info->password);
	RouterShow.mPassword = tmp;

	strcpy(tmp,"");
    readBlock(rbuffer, "dns1 : ", "\n", tmp, MAX_LEN);
	//strcpy(Router_info->dns1, tmp);
	//printf("              dns1 : %s\n", Router_info->dns1);
	RouterShow.mDns1 = tmp;

    strcpy(tmp,"");
    readBlock(rbuffer, "dns2 : ", "\n", tmp, MAX_LEN);
	//strcpy(Router_info->dns2, tmp);
	//printf("              dns2 : %s\n", Router_info->dns2);
	RouterShow.mDns2 = tmp;
  	
  	if ((mUsername != RouterShow.mUsername) || (mPassword != RouterShow.mPassword)\
  		|| (mDns1 != RouterShow.mDns1 ) || (mDns2 != RouterShow.mDns2))
  	{
  			cout << "error" << endl ;
  			
  	}
*/	
	return true;
}

bool AosRouterTorturer::genName(OmnString &name, int minLen, int maxLen)
{
	name = OmnRandom::nextLetterStr(minLen, maxLen);
	return true;
}

bool AosRouterTorturer::genIpAddr(bool isCorrect, OmnString &ip)
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


int AosRouterTorturer::readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len)
{
        int buffer_len,start_len,end_len;
        char *p_start, *p_end;

        if(!buffer || !start || !end || result_len<2)
                return -1;

        start_len = strlen(start);
        end_len = strlen(end);
        buffer_len = strlen(buffer);

        result[0]=0;

        if( buffer_len <= start_len+end_len )
                return -1;

        p_start = strstr( buffer, start );
        if ( !p_start )
                return -1;

        p_end = strstr( p_start+start_len, end );
        if ( !p_end )
                return -1;

	strncpy(result, p_start+start_len, MIN(result_len-1,p_end-p_start-start_len));
	result[MIN(result_len-1,p_end-p_start-start_len)]=0;
        return 0;
 }

bool  AosRouterTorturer::addToSave(OmnString &alias, OmnString &ip1, OmnString &mask, OmnString &ip2, OmnString &dev)
{
	if (mNumRouter >= Max_Num)
	{           
		return false;
	}           
              	
	Router[mNumRouter].mAlias = alias;
	Router[mNumRouter].mDip = ip1;
	Router[mNumRouter].mDmask = mask;
	Router[mNumRouter].mGateWay = ip2;
	Router[mNumRouter].mInterface = dev;
	
	mNumRouter++; 
	return true;
	

}

bool AosRouterTorturer::nameExist(OmnString &name)
{
	for (int i=0; i<mNumRouter; i++)
	{
		if (Router[i].mAlias == name)
		{
			return true;
		}
	}
	return true;
}

bool AosRouterTorturer::delRouter(int RouterIndex)
{
	for (int i=RouterIndex; i<(mNumRouter-1); i++)
	{
		Router[i]= Router[i+1];
	}
	mNumRouter--;
}

AosRouterTorturer::RouterEntry & AosRouterTorturer::RouterEntry::operator = (const AosRouterTorturer::RouterEntry & simRouterEntry)
{
	mAlias = simRouterEntry.mAlias;
	mDip = simRouterEntry.mDip;
	mDmask = simRouterEntry.mDmask;
	mGateWay = simRouterEntry.mGateWay;
	mInterface = simRouterEntry.mInterface;

}

AosRouterTorturer & AosRouterTorturer::operator = (const AosRouterTorturer & simRouterTorturer)
{
	mStatus = simRouterTorturer.mStatus;
	

	for (int i=0; i<Max_Num-1; i++)
	{
		Router[i] = simRouterTorturer.Router[i];
	}
}

AosRouterTorturer::RouterEntry::RouterEntry()
{
}

AosRouterTorturer::RouterEntry::~RouterEntry()
{
}

bool AosRouterTorturer::genDevName( bool isCorrect, OmnString &devName)
{
	char *dev[2];
	dev[0] = "outcard";
	dev[1] = "incard";
	int  index;
	
	if (isCorrect)
	{
		index = RAND_INT(0, 1);
		devName = dev[index];
	}
	else
	{
		// gen a not exist dev
		while(1)
		{
			int i = 0;
			devName = OmnRandom::nextNoSpaceStr(1, 16);
		
			for (int i=0; i<2; i++)
			{
				if(devName == dev[i]) 
				i++;	
			}
			if (i == 0)
			break;
		}				
	}
	return true;
}

