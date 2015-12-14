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
// pppoe Cli
//
// 1.	pppoe password set  25% 
// 2.	pppoe username set 25%  
// 3.	pppoe dns set 25%       
// 4.	pppoe stop 5%           
// 5.	pppoe start 5%          
// 6.	pppoe show config 15%   
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/PppoeTester/PppoeTorturer.h"

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

const int eMaxPppoeNameLen = 32;
const int eMaxPppoePasswordLen = 32;

static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  25</Element>"		// 	1.	pppoe password set  25%               
	        "<Element>2,  25</Element>"		// 	2.	pppoe username set 25%                
	        "<Element>3,  25</Element>"		// 	3.	pppoe dns set 25%       
	        "<Element>4,  5</Element>"		// 	4.	pppoe stop 5%           
	        "<Element>5,  5</Element>"		// 	5.	pppoe start 5%          
	        "<Element>6,  15</Element>"		// 	6.	pppoe show config 15%   
		"</Elements>";


AosPppoeTorturer::AosPppoeTorturer()
{
}


bool AosPppoeTorturer::start()
{
	OmnBeginTest << "Test SSL Server";
	
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef,100);

	int count = 0;
	bool rslt;
	while (count < 6000)
	{
		int index = cliSelector.nextInt();

		switch (index)
		{
		case ePppoePasswordSet:
		     rslt = runPppoePasswordSet();
		     break;
		case ePppoeUsernameSet:
			rslt = runPppoeUsernameSet();
			break;
		case ePppoeDnsSet:  
			rslt = runPppoeDnsSet();
			break;
		case ePppoeStop: 
		//	rslt = runPppoeStop();
			break;
		case ePppoeStart:
		//	rslt = runPppoeStart();
			break;
		case ePppoeShow:
			rslt = runPppoeShow();
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

bool AosPppoeTorturer::runPppoePasswordSet()
{
	//pppoe password set (password)
	OmnString passwd;
	OmnString rslt;
	OmnString cmd;
	genName(passwd, 1, eMaxPppoePasswordLen);
	cmd << "pppoe password set " << passwd;
	aosRunCli_getStr(cmd, true, rslt);
	pppoeSave.mPassword = passwd;
	return true;
}

bool AosPppoeTorturer::runPppoeUsernameSet()
{
	//pppoe username set (password)
	OmnString name;
	OmnString rslt;
	OmnString cmd;
	genName(name, 1, eMaxPppoeNameLen);
	cmd << "pppoe username set " << name;
	aosRunCli_getStr(cmd, true, rslt);
	pppoeSave.mUsername = name;
	return true;
}

bool AosPppoeTorturer::runPppoeDnsSet()
{
	//pppoe dns set (dns1) <dns2>
	//                                              
	// 1 one dns 60%
	// 1.1 correct
	//  valid 70%
	// 1.2 error
	//30%
	// 2 two dns 40%
	// 2.1 valid 70%                                                
	// 2.2  incorrect   
	// 30%                                        

	OmnString rslt;
	OmnString cmd; 
	OmnString ip1;
	OmnString ip2;
	cmd << "pppoe dns set ";
	//60% add one dns
	if (RAND_INT(0, 100) > 40 )
	{
		// generate a correct ipaddr
		 if (RAND_INT(0, 100) > 30)
		 {
			genIpAddr(true, ip1);
			cmd << ip1;
			aosRunCli_getStr(cmd, true, rslt);
			pppoeSave.mDns1 = ip1;
			return true;
		}	
		// generate a error ipaddr	
		if ( RAND_INT(0, 100) > 70 )
		{
			genIpAddr(false, ip1);
			cmd << ip1;
			aosRunCli_getStr(cmd, false, rslt);
			return true;
		}
		
	}
	
	//40% add two dns
	if (RAND_INT(0, 100) > 60 )
	{
		//70% dns1 is correct
		if (RAND_INT(0, 100) > 30)
		 {
			genIpAddr(true, ip1);
			if (RAND_INT(0, 100) > 30)
			{
				genIpAddr(true, ip2);
				cmd << ip1 << ip2 ;
				aosRunCli_getStr(cmd, true, rslt);
				pppoeSave.mDns1= ip1;
				pppoeSave.mDns2= ip2;
				return true;
			}
			
			genIpAddr(false, ip2);
			cmd << ip1 << ip2 ;
			aosRunCli_getStr(cmd, false, rslt);
			return true;
		}	
		// generate a error ipaddr	
		if ( RAND_INT(0, 100) > 70 )
		{
			genIpAddr(false, ip1);
			if (RAND_INT(0, 100) > 30)
			{
				genIpAddr(true, ip2);
				cmd << ip1 << ip2 ;
				aosRunCli_getStr(cmd, false, rslt);
				return true;
			}
			
			genIpAddr(false, ip2);
			cmd << ip1 << ip2 ;
			aosRunCli_getStr(cmd, false, rslt);
			return true;
		}
		
	}
	
} 
  
bool AosPppoeTorturer::runPppoeStop()
{
	//pppoe stop
	OmnString rslt;
	OmnString cmd;
	cmd << "pppoe stop";
	pppoeSave.mStatus = "stop";
	aosRunCli_getStr(cmd, true, rslt);	
	
}
	
bool AosPppoeTorturer::runPppoeStart()
{
	//pppoe start
	OmnString rslt;
	OmnString cmd;
	cmd << "pppoe start";
	pppoeSave.mStatus = "start";
	aosRunCli_getStr(cmd, true, rslt);
}

bool AosPppoeTorturer::runPppoeShow()
{
//
/*	//pppoe show config
	//
	OmnString cmd;
	OmnString rslt;
	char rbuffer[2048];
	char tmp[MAX_LEN];
	
	cmd = "pppoe show config";
	
	
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
	//strcpy(pppoe_info->status, tmp);
	pppoeShow.mStatus = tmp;
	//printf("            status : %s\n", pppoe_info->status, MAX_LEN);
    
	strcpy(tmp,"");
    readBlock(rbuffer, "username : ", "\n", tmp, MAX_LEN);
	//strcpy(pppoe_info->username, tmp);
	//printf("          username : %s\n", pppoe_info->username);

    pppoeShow.mUsername = tmp;
    
	strcpy(tmp,"");
    readBlock(rbuffer, "password : ", "\n", tmp, MAX_LEN);
	//strcpy(pppoe_info->password, tmp);
	//printf("          password : %s\n", pppoe_info->password);
	pppoeShow.mPassword = tmp;

	strcpy(tmp,"");
    readBlock(rbuffer, "dns1 : ", "\n", tmp, MAX_LEN);
	//strcpy(pppoe_info->dns1, tmp);
	//printf("              dns1 : %s\n", pppoe_info->dns1);
	pppoeShow.mDns1 = tmp;

    strcpy(tmp,"");
    readBlock(rbuffer, "dns2 : ", "\n", tmp, MAX_LEN);
	//strcpy(pppoe_info->dns2, tmp);
	//printf("              dns2 : %s\n", pppoe_info->dns2);
	pppoeShow.mDns2 = tmp;
  	
  	if ((pppoeSave.mUsername != pppoeShow.mUsername) || (pppoeSave.mPassword != pppoeShow.mPassword)\
  		|| (pppoeSave.mDns1 != pppoeShow.mDns1 ) || (pppoeSave.mDns2 != pppoeShow.mDns2))
  	{
  			cout << "error" << endl ;
  			
  	}
*/	
	return true;
}

bool AosPppoeTorturer::genName(OmnString &name, int minLen, int maxLen)
{
	name = OmnRandom::nextLetterStr(minLen, maxLen);
	return true;
}

bool AosPppoeTorturer::genIpAddr(bool isCorrect, OmnString &ip)
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


int AosPppoeTorturer::readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len)
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
