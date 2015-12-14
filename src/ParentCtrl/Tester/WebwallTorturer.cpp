////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "ParentCtrl/Tester/WebwallTorturer.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "Util/Random.h"
//#include "UtilComm/Ptrs.h"


// CLI Command selector definition
//
static OmnString sgCliSelectorDef = 
        "<Elements>"
            "<Element>1,  10</Element>"     	// set proxy ip addr and port
            "<Element>2,  1</Element>"     		// start/stop proxy
            "<Element>3,  2</Element>"     		// proxy show config
       		"<Element>4,  25</Element>"    		// set group policy mode
       		"<Element>5,  25</Element>"     	// set group policy name
       		"<Element>6,  2</Element>"     		// group policy show
       		"<Element>7,  7</Element>"     		// creat group
       		"<Element>8,  3</Element>"     		// remove group
       		"<Element>9,  25</Element>"     	// set group policy naughtyness
       "</Elements>";
AosWebwallTorturer::group::group()
:
groupId(""),
mode(""),
alias(""),
naughtyness(0)
{
}


AosWebwallTorturer::group::~group()
{
}

AosWebwallTorturer::group& AosWebwallTorturer::group::operator = (const AosWebwallTorturer::group &grp)
{
	groupId             = grp.groupId        ;
	mode           		= grp.mode           ;
	alias            	= grp.alias          ;
	naughtyness         = grp.naughtyness    ;
	
	return *this;
}

void
AosWebwallTorturer::group::clear()
{
	groupId = "";
	mode    = "";
	alias   = "";
	naughtyness = 0;
}


AosWebwallTorturer::AosWebwallTorturer()
:
mGroupNum(0)
{
}


bool AosWebwallTorturer::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Begin the Webwall Torturer Testing";
	mTcNameRoot = "Webwall_Torturer";

	initGroupList();
	clitest();

	return true;
}

bool AosWebwallTorturer::clitest()
{
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef,100);
	
	int count = 0;
	bool rslt;
	while (count < 10000)
	{
		int index = cliSelector.nextInt();

		switch (index)
		{
			case eProxyAddress : 			
				 rslt = runProxyAddress();
				 break;
			case eProxyStatus :			
				 rslt = runProxyStatus();
				 break;
			case eProxyShowConfig : 		
				 rslt = runProxyShowConfig();
				 break;
			case eGroupPolicyMode :	
				 rslt = runGroupPolicyMode();
				 break;
			case eGroupPolicyName :	
				 rslt = runGroupPolicyName();
				 break;
			case eGroupPolicyShow : 	
				 rslt = runGroupPolicyShow();
				 break;
			case eGroupCreate :
				 rslt = runGroupCreate();
				 break;
			case eGroupRemove :	
				 rslt = runGroupRemove();
				 break;
			case eGroupNaughtynesslimit :	
				 rslt = runGroupNaughty();
				 break;
/*			case eVLanShowConfig : 	
				 rslt = runShowConfig();
				 break;
*/      	
			default:
				 OmnAlarm << "Invalid index: " << index << enderr;
				 break;
		}

		count++;
	}

	return true;		
}

bool AosWebwallTorturer::runProxyAddress()
{
	//
	//proxy address <IP> <port>
	//
	
	OmnString cmd = "proxy address ";
	OmnString ipaddr;
	int port;
	bool isCmdCorrect = true;
	
	//generate ipaddr (90% correct)
	if(RAND_INT(1,100) <= 90)
	{
		ipaddr = OmnRandom::nextStrIP();		
	}
	else
	{
		ipaddr = OmnRandom::nextNoSpaceStr(1,eMaxStrLen);
		isCmdCorrect = false;
	}
	// generate port (90% correct)
	if(RAND_INT(1,100) <= 90)
	{
		port = RAND_INT(1,65535);		
	}
	else
	{
		port = genBadPort();	
		isCmdCorrect = false;

	}
	
	cmd << ipaddr << " " << port;
	aosRunCli(cmd, isCmdCorrect);
	
	return true;
}

bool AosWebwallTorturer::runProxyStatus()
{
	//
	//proxy status <action>
	//
	OmnString cmd = "proxy status ";
	
	if(mProxyStat.isStatusOn == false)
	{
		cmd << "start";
		mProxyStat.isStatusOn = true;	
	}	
	else
	{
		cmd << "stop";
		mProxyStat.isStatusOn = false;	
	}
	
	aosRunCli(cmd, true);
	return true;
	
}

bool AosWebwallTorturer::runProxyShowConfig()
{
	//	
	//proxy show config
	//
	
	
	OmnString cmd = "proxy show config";
	OmnString rslt;
	
	aosRunCli_getStr(cmd, true, rslt);
	
	
	return true;
}

bool AosWebwallTorturer::runGroupPolicyMode()
{
	//	
	//webwall group policy mode <group_id> <mode>
	//
	
	OmnString cmd = "webwall group policy mode ";
	OmnString group_id;
	OmnString mode;
	int index;
	bool isCmdCorrect = true;
	
	//generate group id (90% correct)
	if(RAND_INT(1,100) <= 90)
	{
		index = RAND_INT(0, mGroupNum-1);
		group_id = mGroupList[index].groupId;
	}
	else
	{
		group_id = genBadGroupId();
		isCmdCorrect = false;
	}
	//generaate mode (90% correct)
	if(RAND_INT(1,100) <= 90)
	{
		genMode(mode);
	}
	else
	{		
		mode = OmnRandom::nextNoSpaceStr(1,16);
			
		if(mode == "banned" || mode == "filtered" || mode == "unfiltered")
		{
			mode = "thisisawrongmode";
		}	
		
		isCmdCorrect = false;
	}
	
	cmd << group_id << " " << mode;
	aosRunCli(cmd, isCmdCorrect && (mGroupNum > 0));
	
	//if the cmd is correct excuted, save the result to structure
	if(isCmdCorrect && (mGroupNum > 0))
	{	
		mGroupList[index].mode = mode;
	}
	
	return true;	
}	
	
bool AosWebwallTorturer::runGroupPolicyName()
{
	//
	//webwall group policy name <group_id> <group_name>
	//
	
	OmnString cmd = "webwall group policy name ";
	OmnString group_id;
	OmnString name;
	int index;
	bool isCmdCorrect = true;
	
	//generate group id (90% correct)
	if(RAND_INT(1,100) <= 90)
	{
		index = RAND_INT(0, mGroupNum-1);
		group_id = mGroupList[index].groupId;
	}
	else
	{
		group_id = genBadGroupId();
		isCmdCorrect = false;
	}
	//generate name (90% correct)
	if(RAND_INT(1,100) <= 90)
	{
		name = OmnRandom::nextNoSpaceStr(1, eMaxStrLen);
	}
	else
	{		
		name = OmnRandom::nextNoSpaceStr(eMaxStrLen+1, eMaxStrLen+16);
		isCmdCorrect = false;
	}
	
	cmd << group_id << " " << name;
	aosRunCli(cmd, isCmdCorrect && (mGroupNum > 0));
	
	//if the cmd is correct excuted, save the result to structure
	if(isCmdCorrect && (mGroupNum > 0))
	{	
		mGroupList[index].alias = name;
	}
	
	return true;	
}
	
bool AosWebwallTorturer::runGroupPolicyShow()
{
	//	
	//webwall group policy show config <group_id>
	//
	
	OmnString cmd = "webwall group policy show config ";
	OmnString group_id;
	bool isCmdCorrect = true;
	
	//80% generate a valid group_id
	if(RAND_INT(1,100) <= 80 && mGroupNum > 0)
	{	
		int index = RAND_INT(0,mGroupNum-1);
		group_id = mGroupList[index].groupId;
	}
	//20% generate invalid group_id
	else
	{
		group_id = genBadGroupId();
		isCmdCorrect = false;		
	}
	
	cmd << group_id;
	aosRunCli(cmd, isCmdCorrect && (mGroupNum > 0));
	
	return true;
}

bool AosWebwallTorturer::runGroupCreate()
{
	//
	//webwall group create <group_id>
	//

	OmnString cmd = "webwall group create ";
	OmnString group_id;
	bool isCmdCorrect = true;
	
	//80% valid parms
	if((RAND_INT(1,100) <= 80) && (mGroupNum < eMaxGroupCount))
	{
		while(!genGroupId(group_id))
		{
		}
	}
	//20% invalid parms
	else
	{
		group_id = OmnRandom::nextNoSpaceStr(1, eMaxStrLen);
			
		if(group_id == "group1" || group_id == "group2" ||
		   group_id == "group3" || group_id == "group4" ||
		   group_id == "group5" || group_id == "group6" ||
		   group_id == "group7" || group_id == "group8" ||
		   group_id == "group9")
		{
			group_id = "thisisawronggroupid";	
		}

		isCmdCorrect = false;		
	}
	
	cmd << group_id;
	aosRunCli(cmd, isCmdCorrect && (mGroupNum < eMaxGroupCount))
	
	if(isCmdCorrect && (mGroupNum < eMaxGroupCount))
	{
		saveToGroupList(group_id);
	}
	
	return true;	
	
}

bool AosWebwallTorturer::runGroupRemove()
{
	//
	//webwall group remove <group_id>
	//
 
    OmnString cmd = "webwall group remove ";
	OmnString group_id;
	int index;
	bool isCmdCorrect = true;
	    
    if(RAND_INT(1,100) <= 80)
    {
    	index = RAND_INT(0,mGroupNum-1);
    	group_id = mGroupList[index].groupId;
    }
    else
    {
    	group_id = genBadGroupId();
    	isCmdCorrect = false;
    }
    
    cmd << group_id;
    aosRunCli(cmd, isCmdCorrect && (mGroupNum > 0));
    
    if(isCmdCorrect && (mGroupNum > 0))
    {
        delFromGroupList(index);	
	}
	
    return true;		
}
	
bool AosWebwallTorturer::runGroupNaughty()
{
	//	
	//webwall group naughtnesslimit <group_id> <counts>
	//

	
	OmnString cmd = "webwall group naughtnesslimit ";
	OmnString group_id;
	int counts;
	int index;
	bool isCmdCorrect = true;
	
	if(RAND_INT(1,100) <= 80)
	{
		index = RAND_INT(0, mGroupNum-1);
		group_id = mGroupList[index].groupId;
	}
	else
	{
		group_id = genBadGroupId();
		isCmdCorrect = false;	
	}
	
	if(RAND_INT(1,100) <= 80)
	{
		counts = RAND_INT(1,4096);
	} 	
	else
	{
		counts = RAND_INT(-32768,0);
		isCmdCorrect = false;
	}
	
	cmd << group_id << " " << counts;
	aosRunCli(cmd, isCmdCorrect && (mGroupNum > 0));
	
	if(isCmdCorrect && (mGroupNum > 0))
	{
		mGroupList[index].naughtyness = counts;	
	}
	
	return true; 	
	
}


bool AosWebwallTorturer::initGroupList()
{
	OmnString cmd = "webwall clear config";
	aosRunCli(cmd, true);
	
	for(int i = 0;i < mGroupNum-1;i++)
	{
		mGroupList[i].clear();
	}
	
	return true;
}	

bool AosWebwallTorturer::genMode(OmnString &mode)
{
	//
	//randomly generate a valid group policy mode
	//
	OmnString modeList[3];
	
	modeList[0] = "banned";
	modeList[1] = "filtered";
	modeList[2] = "unfiltered";
		
	int index = RAND_INT(0,2);
	
	mode = modeList[index];
	
	return true;
}
	
bool AosWebwallTorturer::genGroupId(OmnString &name)
{
	OmnString nameList[9];
	
	nameList[0] = "group1";	
	nameList[1] = "group2"; 
    nameList[2] = "group3"; 
    nameList[3] = "group4"; 
    nameList[4] = "group5"; 
    nameList[5] = "group6"; 
    nameList[6] = "group7"; 
    nameList[7] = "group8"; 
    nameList[8] = "group9";
    
    int index = RAND_INT(0,8);
    
    name = nameList[index];
    
    for(int i = 0;i < mGroupNum;i++)
    {
    	if(name == mGroupList[i].groupId)
    	{
    		return false;
    	}
    
    }
    
    return true;    
             
}            
	         
bool AosWebwallTorturer::saveToGroupList(OmnString &group_id)
{
	for(int i = 0;i < mGroupNum; i++)
	{
		if(mGroupList[i].groupId == group_id)
		{
			return false;	
		}
	}			
	
	mGroupList[mGroupNum].groupId = group_id;		
	mGroupList[mGroupNum].mode    = "filtered";		
	mGroupList[mGroupNum].alias   = "";		
	mGroupList[mGroupNum].naughtyness = 50;			

	mGroupNum++;
	return true;
	
}	         
	         

bool AosWebwallTorturer::delFromGroupList(const int &index)
{
	if(index < 0 || index > mGroupNum - 1)
    {
        return false;
    }
    
    for(int i = index;i < mGroupNum - 1;i++)
    {

        mGroupList[i] = mGroupList[i+1];
    }
    mGroupList[mGroupNum-1].clear();
    mGroupNum--;
    
    return true;		
}	        

int AosWebwallTorturer::genBadPort()
{
	//
	//randomly generate an incorrect port
	// 1.-65535 to 0
	// 2.65536 to 300000
	
	int port;
	
	if(RAND_INT(1,100) <= 50)
	{
		port = RAND_INT(-65535,0);		
	}	
	else
	{
		port = RAND_INT(65536,300000);	
	}
	
	return port;
}

OmnString AosWebwallTorturer::genBadGroupId()
{
	//
	//generate an incorrect group id
	//
	
	OmnString group_id = OmnRandom::nextNoSpaceStr(1, eMaxStrLen);
		
	for(int i = 0;i < mGroupNum-1;i++)
	{
		if(group_id == mGroupList[i].groupId)
		{
			group_id = "thisisawronggroupid";	
		}
	}	
	
	return group_id;
}
	         
	         
	         
