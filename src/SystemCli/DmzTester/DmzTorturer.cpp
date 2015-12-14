////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosDmzTorturer.cpp
// Description:
// Dmz Cli
//
// 1.	dmz set status <on|off> 15% 
// 2.	dmz add machine <alias> <pub_ip> <dmz_ip> 30%  
// 3.	dmz del machine <alias>15%       
// 4.	dmz show config 15%           
// 5.	dmz clear config 15%          
// 6.	dmz save config 15%   
//# 7    dmz load config 10%
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/DmzTester/DmzTorturer.h"

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

const int eMaxDmzNameLen = 32;
const int eMaxDmzPasswordLen = 32;



static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  15</Element>"		// 	1.	dmz setstatus <on|off> 15%
			"<Element>2,  30</Element>"		// 	2.	dmz add machine <alias> <pub_ip> <dmz_ip> 30%
			"<Element>3,  15</Element>"		// 	3.	dmz del machine <alias>15%                    
	        "<Element>4,  15</Element>"		// 	4.	dmz show config 15%                           
	        "<Element>5,  15</Element>"		// 	5.	dmz clear config 15%                          
	        "<Element>6,  10</Element>"	    //  6.  dmz save config 15%                           
//	        "<Element>7,  5</Element>"		// 	7.	dmz load config 10%
		"</Elements>";


AosDmzTorturer::AosDmzTorturer()
{
	mStatus = "off";
}


bool AosDmzTorturer::start()
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
		case eDmzSetStatus:
		     rslt = runDmzSetStatus();
		     break;                                                                     
		case eDmzAddMachine:                        
			rslt = runDmzAddMachine();             
			break;                                   
		case eDmzDelMachine:                             
			rslt = runDmzDelMachine();                    
			break;                                   
		case eDmzShowConfig:                     
			rslt = runDmzShowConfig();           
			break;                               
		case eDmzClearConfig:                    
			rslt = runDmzClearConfig();          
			break;                               
		case eDmzSaveConfig:                     
			rslt = runDmzSaveConfig();
			break;  
//		case eDmzLoadConfig:
//			rslt = runDmzLoadConfig();
//			break;               
		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			break;
		}
		count++;
//		updateSeed();
	}

	return true;
}

bool AosDmzTorturer::runDmzSetStatus()
{
	//Dmz set stauts <on|off>;
	OmnString status;
	OmnString rslt = "";
	OmnString cmd;
	cmd = "dmz set status ";
    bool isCorrect = true;
	//sleep(3);	
	if (OmnRandom::nextInt(0, 100) > 40)
	{
		status = RAND_BOOL()?"on":"off";
	    if ((mStatus == "off") && (status == "off"))
		{
		   		isCorrect = false;
				cmd << status;
	    		aosRunCli_getStr(cmd, isCorrect, rslt);
	    }
//		if(((dmzSave.mStatus == "on") && (status == "off"))||((dmzSave.mStatus == "off") && (status == "on")))
		else
		{
			isCorrect = true;
			//cout <<"dmz status:" << dmzSave.mStatus <<endl;
			cmd << status;
			//cout << cmd << endl;
			//cout << status << endl;
	   		aosRunCli_getStr(cmd, isCorrect, rslt);
			mStatus = status;
		}
	}
	else
	{
	    status = OmnRandom::nextLetterStr(2, 6);
	    isCorrect = false;
	    aosRunCli_getStr(cmd, isCorrect, rslt);
	}

	return true;
}

bool AosDmzTorturer::runDmzAddMachine()
{
	//Dmz add machine  <alias> <pub_ip> <dmz_ip>

	OmnString public_Ip;
	OmnString dmz_Ip;
	OmnString alias;
	OmnString rslt;
	OmnString cmd;
	//gen a alias
	genName(alias, 1, eMaxDmzNameLen);
	
	bool ip1Status = RAND_INT(0, 100) < 80;
	genIpAddr(ip1Status, public_Ip);

	bool ip2Status = RAND_INT(0, 100) < 80;
	genIpAddr(ip2Status, dmz_Ip);

	//incorrect
	//1.alias is exist 	
	//2.public_ip error
	//3.dmz_ip error
	if (nameExist(alias))
	{
		cmd << alias << " " << public_Ip << " " << dmz_Ip  ;
		aosRunCli_getStr(cmd, false, rslt);
		return true;
	}
		
	cmd << alias << " " << public_Ip << " " << dmz_Ip  ;
	aosRunCli_getStr(cmd, ip1Status && ip2Status, rslt);
	if (ip1Status && ip2Status)
	{
		addToSave(alias, public_Ip, dmz_Ip);
	}
}

bool AosDmzTorturer::runDmzDelMachine()
{
	//dmz del machine <alias> 

	OmnString alias;
	OmnString rslt;
	OmnString cmd;
	genName(alias, 1, eMaxDmzNameLen);
	cmd << "dmz del machine " << alias;\
	
	if (RAND_INT(0, 100) > 30 && mNumDmz > 0)
	{

		int dmzIndex = RAND_INT(0, mNumDmz-1);

		alias = dmzMachine[dmzIndex].mAlias;

		cmd << alias;

		aosRunCli_getStr(cmd, true, rslt);
		
		delDmz(dmzIndex);

		return true;
	}
		
	// 2. remove a unexist name

	genName(alias, 1, eMaxDmzNameLen);
	
	if (nameExist(alias))
	{
		return true;
	}

	
	cmd << alias;
		
	aosRunCli_getStr(cmd, false, rslt);

	return true;
} 
  
bool AosDmzTorturer::runDmzSaveConfig()
{
	//Dmz save config
	OmnString rslt;
	OmnString cmd;
	cmd << "dmz save config";

	aosRunCli_getStr(cmd, true, rslt);	
	return true;
	
}
	
bool AosDmzTorturer::runDmzLoadConfig()
{
	//Dmz load config
	OmnString rslt;
	OmnString cmd;
	cmd << "dmz load config";
	
	aosRunCli_getStr(cmd, true, rslt);
	return true;
}

bool AosDmzTorturer::runDmzClearConfig()
{
	//Dmz clear config
	OmnString rslt;
	OmnString cmd;
	cmd << "dmz clear config";
	
	aosRunCli_getStr(cmd, true, rslt);
	//init dmz
	mStatus = "off";
	for (int i=0; i<Max_Num-1; i++)
	{
		dmzMachine[i].mAlias = "";
		dmzMachine[i].mPub_ip = "";
		dmzMachine[i].mDmz_ip = "";
	}
	return true;
}

bool AosDmzTorturer::runDmzShowConfig()
{
//
/*	//Dmz show config
	//
	OmnString cmd;
	OmnString rslt;
	char rbuffer[2048];
	char tmp[MAX_LEN];
	
	cmd = "Dmz show config";
	
	
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
	//strcpy(Dmz_info->status, tmp);
	DmzShow.mStatus = tmp;
	//printf("            status : %s\n", Dmz_info->status, MAX_LEN);
    
	strcpy(tmp,"");
    readBlock(rbuffer, "username : ", "\n", tmp, MAX_LEN);
	//strcpy(Dmz_info->username, tmp);
	//printf("          username : %s\n", Dmz_info->username);

    DmzShow.mUsername = tmp;
    
	strcpy(tmp,"");
    readBlock(rbuffer, "password : ", "\n", tmp, MAX_LEN);
	//strcpy(Dmz_info->password, tmp);
	//printf("          password : %s\n", Dmz_info->password);
	DmzShow.mPassword = tmp;

	strcpy(tmp,"");
    readBlock(rbuffer, "dns1 : ", "\n", tmp, MAX_LEN);
	//strcpy(Dmz_info->dns1, tmp);
	//printf("              dns1 : %s\n", Dmz_info->dns1);
	DmzShow.mDns1 = tmp;

    strcpy(tmp,"");
    readBlock(rbuffer, "dns2 : ", "\n", tmp, MAX_LEN);
	//strcpy(Dmz_info->dns2, tmp);
	//printf("              dns2 : %s\n", Dmz_info->dns2);
	DmzShow.mDns2 = tmp;
  	
  	if ((DmzSave.mUsername != DmzShow.mUsername) || (DmzSave.mPassword != DmzShow.mPassword)\
  		|| (DmzSave.mDns1 != DmzShow.mDns1 ) || (DmzSave.mDns2 != DmzShow.mDns2))
  	{
  			cout << "error" << endl ;
  			
  	}
*/	
	return true;
}

bool AosDmzTorturer::genName(OmnString &name, int minLen, int maxLen)
{
	name = OmnRandom::nextLetterStr(minLen, maxLen);
	return true;
}

bool AosDmzTorturer::genIpAddr(bool isCorrect, OmnString &ip)
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


int AosDmzTorturer::readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len)
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

bool AosDmzTorturer::addToSave(OmnString alias, OmnString &ip1, OmnString &ip2)
{
	if (mNumDmz >= Max_Num)
	{           
		return false;
	}           
               
		
	dmzMachine[mNumDmz].mAlias = alias;
	dmzMachine[mNumDmz].mPub_ip = ip1;
	dmzMachine[mNumDmz].mDmz_ip = ip2;
	
	mNumDmz++; 
	return true;
	

}

bool AosDmzTorturer::nameExist(OmnString &name)
{
	for (int i=0; i<mNumDmz; i++)
	{
		if (dmzMachine[i].mAlias == name)
		{
			return true;
		}
	}
	return true;
}

bool AosDmzTorturer::delDmz(int DmzIndex)
{
	for (int i=DmzIndex; i<(mNumDmz-1); i++)
	{
		dmzMachine[i]= dmzMachine[i+1];
	}
	mNumDmz--;
}

AosDmzTorturer::dmzMachineEntry & AosDmzTorturer::dmzMachineEntry::operator = (const AosDmzTorturer::dmzMachineEntry & simDmzMachineEntry)
{
	mAlias = simDmzMachineEntry.mAlias;
	mPub_ip = simDmzMachineEntry.mPub_ip;
	mDmz_ip = simDmzMachineEntry.mDmz_ip;

}

AosDmzTorturer & AosDmzTorturer::operator = (const AosDmzTorturer & simDmzTorturer)
{
	mStatus = simDmzTorturer.mStatus;
	

	for (int i=0; i<Max_Num-1; i++)
	{
		dmzMachine[i] = simDmzTorturer.dmzMachine[i];
	}
}

AosDmzTorturer::dmzMachineEntry::dmzMachineEntry()
{
}

AosDmzTorturer::dmzMachineEntry::~dmzMachineEntry()
{
}
