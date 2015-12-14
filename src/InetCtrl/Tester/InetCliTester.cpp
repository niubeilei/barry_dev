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
//  app
//  1. inetctrl app name <name>
//  2. inetctrl app addflow ip <name> <tcp/udp> <dst ip> <dst mask> <start port> <end port>
//  3. inetctrl app addflow url <name> <tcp/udp> <url> <start port> <end port>
//  4. inetctrl app revflow ip <name> <tcp/udp> <dst ip> <dst mask> <start port> <end port>
//  5. inetctrl app revflow url <name> <tcp/udp> <url> <start port> <end port>
//  6. inetctrl app remove <name>
//  7. inetctrl app show config
//     inetctrl app clear all
//
//  Time Rule
//  8. inetctrl timerule name <name>
//  9. inetctrl timerule addrule time <name> <start time> <end time> <time days>
//  10. inetctrl timerule addrule online <name> <online type> <online minutes> <online days>
//  11. inetctrl timerule revrule time <name> <start time> <end time> <time days>
//  12. inetctrl timerule revrule online <name> <online type> <online minutes> <online days>
//  13. inetctrl timerule remove <name>
//  14. inetctrl timerule show config
//      inetctrl timerule clear all
//
//  Group
//  15. inetctrl group name <name>
//  16. inetctrl group adduser byname <name> <username>
//  17. inetctrl group adduser byip <name> <ip> <mask>
//  18. inetctrl group adduser bymac <name> <mac>
//  19. inetctrl group revuser byname <name> <username>
//  20. inetctrl group revuser byip <name> <ip> <mask>
//  21. inetctrl group revuser bymac <name> <mac>
//  22. inetctrl group addrule <name> <appname> <timename>
//  23. inetctrl group revrule <name> <appname> <timename>
//  24. inetctrl group defautpolicy <name> <access/deny>
//  25. inetctrl group remove <name>
//  26. inetctrl group show config
//      inetctrl group clear all
//
//  Switch
//  27. inetctrl on/off
//  28. inetctrl show config
//      inetctrl clear all
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "InetCtrl/Tester/InetCliTester.h"

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
//#include "Util/Random.h"
#include "Random/RandomUtil.h"
#include "Util1/Wait.h"
#include "UtilComm/Ptrs.h"
#include "KernelSimu/timer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
// 
// CLI Command selector definition
//
static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  11</Element>"		// Add app
	        "<Element>2,  10</Element>"		// Add an ip flow to an app
	        "<Element>3,  0</Element>"		// Add a URL flow to an app
	        "<Element>4,  1</Element>"		// Remove an ip flow from an app
	        "<Element>5,  0</Element>"		// Remove a URL flow from an app
	        "<Element>6,  1</Element>"		// Remove an app
	        "<Element>7,  0</Element>"		// Show all apps
	        "<Element>8,  0</Element>"		// Clear all apps

	        "<Element>9,  15</Element>"		// Add a time rule
	        "<Element>10, 17</Element>"		// Add a time limit to a time rule
	        "<Element>11, 0</Element>"		// Add an online time to a time rule
	        "<Element>12, 1</Element>"		// Remove a time limit from a time rule
	        "<Element>13, 0</Element>"		// Remove an online time from a time rule
	        "<Element>14, 1</Element>"		// Remove a time rule
	        "<Element>15, 0</Element>"		// Show all time rules
	        "<Element>16, 0</Element>"		// Clear all time rules

	        "<Element>17, 4</Element>"		// Add a group
	        "<Element>18, 0</Element>"		// Add a user by name to a group
	        "<Element>19, 11</Element>"		// Add a user by IP to a group
	        "<Element>20, 0</Element>"		// Add a user by mac to a group
	        "<Element>21, 0</Element>"		// Remove a user by name from a group
	        "<Element>22, 1</Element>"		// remove a user by IP from a group
	        "<Element>23, 0</Element>"		// Remove a user by mac from a group
	        "<Element>24, 24</Element>"		// Add an association to a group
	        "<Element>25, 1</Element>"		// Remove an association from a group
	        "<Element>26, 1</Element>"		// Remove a group
	        "<Element>27, 0</Element>"		// Show all groups
	        "<Element>28, 0</Element>"		// Clear all groups

	        "<Element>29, 0</Element>"		// Turn it on/off
	        "<Element>30, 0</Element>"		// log level
	        "<Element>31, 1</Element>"		// default policy
	        "<Element>32, 0</Element>"		// show config
	        "<Element>33, 0</Element>"		// save config
		"</Elements>";

const int sgSecPerWeek = 7 * 24 * 60;
const int sgSecPerDay  = 24 * 60;


AosInetCliTester::AosInetCliTester()
:
mNumApps(0),
mNumTimeRule(0),
mNumGroups(0),
mDefaultPolicy(true)
{
	mName = "InetClitTester";
	
}


AosInetCliTester::~AosInetCliTester()
{
}

bool AosInetCliTester::start()
{
	// 
	// Test default constructor
	//
	

	int cliCounts = RAND_INT(1,2000);
	basicTest(cliCounts);
	return true;
}


bool AosInetCliTester::basicTest(const u32 tries)
{
	OmnBeginTest << "Test SSL Server";
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef, 100);

	int count = 0;
	bool rslt;
	while (count < tries)
	{
		int index = cliSelector.nextInt(); 
		switch (index)
		{
		case eAddApp:
			 rslt = runAddApp();
			 break;

		case eAddIpFlow:
			 rslt = runAddIPFlow();
			 break;

		case eAddUrlFlow:
			 rslt = runAddUrlFlow();
			 break;

		case eRemoveIpFlow:
			 rslt = runRemoveIpFlow();
			 break;

		case eRemoveUrlFlow:
			 rslt = runRemoveUrlFlow();
			 break;

		case eRemoveApp:
			 rslt = runRemoveApp();
			 break;

		case eShowApps:
			 rslt = runShowApp();
			 break;
			 
		case eClearApps:
			 rslt = runClearApp();
			 break;

		case eAddTimeRule:
			 rslt = runAddTimeRule();
			 break;

		case eAddTimeLimit:
			 rslt = runAddTimeRangeCtr();
			 break;

		case eAddOnlineTime:
			 rslt = runAddTimeOnlineCtr();
			 break;

		case eRemoveTimeLimit:
			 rslt = runRemoveTimeRangeCtr();
			 break;

		case eRemoveOnlineTime:
			 rslt = runRemoveTimeOnlineCtr();
			 break;

		case eRemoveTimeRule:
			 rslt = runRemoveRuleRule();
			 break;

		case eShowTimeRules:
			 rslt = runShowTimeRules();
			 break;

		case eClearTimeRules:
			 rslt = runClearTimeRules();
			 break;
			 
		case eAddGroup:
			 rslt = runAddGroup();
			 break;

		case eAddUserByName:
			 rslt = runAddUserByName();
			 break;

		case eAddUserByIP:
			 rslt = runAddUserByIP();
			 break;

		case eAddUserByMac:
			 rslt = runAddUserByMac();
			 break;

		case eRemoveUserByName:
			 rslt = runRemoveUserByName();
			 break;

		case eRemoveUserByIP:
			 rslt = runRemoveUserByIP();
			 break;

		case eRemoveUserByMac:
			 rslt = runRemoveUserByMac();
			 break;

		case eAddAssociation:
			 rslt = runAddAssociation();
			 break;

		case eRemoveAssociation:
			 rslt = runRemoveAssociation();
			 break;

		case eRemoveGroup:
			 rslt = runRemoveGroup();
			 break;

		case eShowGroup:
			 rslt = runShowGroup();
			 break;
			
		case eClearGroup:
			 rslt = runClearGroup();
			 break;

		case eTurnOnOff:
			 rslt = runTurnOnOff();
			 break;
			 
		case eShowConfig:	
			 rslt = runShowConfig();
			 break;
			 		
		case eLogLevel:
			 rslt = runLogLevel();
			 break;
			 
		case eDefaultPolicy:
			 rslt = runDefaultPolicy();
			 break;
				
		case eSaveConfig:
			 rslt = runSaveConfig();
			 break;
       
		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			 break;
		}
		count++;
	}

	return true;
}


bool	
AosInetCliTester::runAddApp()
{
	// 
	// Determine whether to run a correct command or an incorrect command
	//
	// 1. Construct a correct app name
	// 2. Construct a uncorrect app name
	//
	
	OmnString name;
	OmnString cmd("inetctrl app name ");

	if (PERCENTAGE(90))
	{
		if (eMaxApps <= mNumApps)
			return true;

		genName(name, 1, eMaxNameLen);
		if (appExist(name))
			return true;

		cmd << name;
		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		// 
		// Add the application to our list
		//
		mApps[mNumApps].mName = name;
		mApps[mNumApps].mNumIpFlows = 0;
		mNumApps++; 
		return true;
	}

	// 
	// There are two error conditions: 
	//  1. Name too long
	//  2. Name already exist
	//
	if (PERCENTAGE(50) && mNumApps > 0)
	{
		// 
		// Select a name from exist name list
		//
		int index = RAND_INT(0, mNumApps-1);
		name = mApps[index].mName;
	}
	else
	{
		// 
		// Create a name that is too long
		//
		genName(name, eMaxNameLen+10, eMaxTooLongNameLen);
	}

	cmd << name;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);
	return true;
}


bool	
AosInetCliTester::runAddIPFlow()
{
	// 
	// inetctrl app addflow ip <name> <tcp/udp> <dst ip> <dst mask> <start port> <end port>
	//
	// We will determine whether to run a correct command or incorrect command. For a 
	// correct command, every parm should be correct. 
	//
	// For incorrect command, we will consider the following:
	// 1, name exist
	// 		1. Add a flow that is already in the app
	//   	2. Construct a flow, including:
	// 			  1. Not tcp/udp
	// 			  2. Incorrect IP address
	// 			  3. Incorrect mask
	// 			  4. Incorrect start port
	// 			  5. Incorrect end port
	// 2. <name> not exist
	//    Construct a syntactically incorrect command, including:
	// 	  1. Not tcp/udp
	// 	  2. Incorrect IP address
	// 	  3. Incorrect mask
	// 	  4. Incorrect start port
	// 	  5. Incorrect end port
	//
	
	// 
	// Determine whether to use an existing name or not
	//
	OmnString appName;
	OmnString cmd("inetctrl app addflow ip ");
	bool isCorrect = true;
	int	 index, flowIndex;

	//select a app name
	if (PERCENTAGE(90) && mNumApps > 0)
	{
		index = RAND_INT(0, mNumApps-1);
		appName = mApps[index].mName;
	}
	else
	{
		selectNonExistName(appName, "app");
		isCorrect = false;
	}

	// 
	// Determine whether to add a flow that is already in the app
	//
		
	if (PERCENTAGE(20) && isCorrect)
	{
		if (mApps[index].mNumIpFlows <= 0)
			return true;
		// 
		// Randomly select a flow
		//
		flowIndex = RAND_INT(0, mApps[index].mNumIpFlows-1);

		// 
		// Construct the command
		//
		cmd << appName << " "
			<< mApps[index].mIpFlows[flowIndex].mProto     << " "
			<< mApps[index].mIpFlows[flowIndex].mDestIP    << " "
			<< mApps[index].mIpFlows[flowIndex].mMask      << " "
			<< mApps[index].mIpFlows[flowIndex].mStartPort << " "
			<< mApps[index].mIpFlows[flowIndex].mEndPort;

		//aosRunCli(cmd, false);
		sendCmd(cmd,mRhcConn);

		return true;
	}
		
	// 
	// Construct a syntactically incorrect command
	//
	// 1. Determine the protocol
	//
	OmnString proto;
	genProto(proto, isCorrect);

	// 
	// 2. Determine the destination IP
	//
	OmnString dstIP;
	genDstIp(dstIP, isCorrect);

	// 
	// 3. Determine the mask
	//
	OmnString mask;
	genMask(mask, isCorrect);

	// 
	// 4. Determine start end port
	//
	int startPort;
	int endPort;
	genStartEndPort(startPort, endPort, isCorrect); 

	if (isCorrect)
	{
		flowIndex = mApps[index].mNumIpFlows;
		for (int i=0; i<flowIndex; i++) 
		{
			if (mApps[index].mIpFlows[i].mProto == proto &&
				mApps[index].mIpFlows[i].mDestIP == dstIP &&
				mApps[index].mIpFlows[i].mMask == mask &&
				mApps[index].mIpFlows[i].mStartPort == startPort &&
				mApps[index].mIpFlows[i].mEndPort == endPort)
				return true;
		}
	}
	
	if (isCorrect)
	{
		if (mApps[index].mNumIpFlows >= eMaxIPFlows)
			return true;
	}

	//
	//save the ipflow
	//
	if (isCorrect)
	{
		int ipFlowsIndex = mApps[index].mNumIpFlows;
		mApps[index].mIpFlows[ipFlowsIndex].mProto = proto;
		mApps[index].mIpFlows[ipFlowsIndex].mDestIP = dstIP;
		mApps[index].mIpFlows[ipFlowsIndex].mMask = mask;
		mApps[index].mIpFlows[ipFlowsIndex].mStartPort = startPort;
		mApps[index].mIpFlows[ipFlowsIndex].mEndPort = endPort;
		mApps[index].mNumIpFlows++;
	}

	// 
	// Construct the command
	//
	
	cmd << appName << " " << proto << " " << dstIP << " " << mask << " " << startPort << " " << endPort;
	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);
	if (isCorrect)
		cout << cmd << endl;

	return true;
}


bool	
AosInetCliTester::runAddUrlFlow()
{
	return true;
}


bool	
AosInetCliTester::runRemoveIpFlow()
{
	// 
	// inetctrl app revflow ip <name> <tcp/udp> <dst ip> <dst mask> <start port> <end port>
	//
	//Correct : random select a exist flow to remove
	//InCorrect:
	//           random select a app name
	//           random create a proto, ipaddress, start port, end porto
	//           and make sure this flow not a exist flow
	//
	int index, flowIndex;
	OmnString appName;
	OmnString cmd("inetctrl app revflow ip "); 
	bool isCorrect = true;
	
	//select a app name
	if (PERCENTAGE(90) && mNumApps > 0)
	{
		index = RAND_INT(0, mNumApps-1);
		appName = mApps[index].mName;
	}
	else
	{
		selectNonExistName(appName, "app");
		isCorrect = false;
	}
	
	// Run a correct command
	if (PERCENTAGE(80) && isCorrect)
	{
		if (mApps[index].mNumIpFlows <= 0)
			return true;

		int flowIndex = RAND_INT(0, mApps[index].mNumIpFlows-1);

		// 
		// Construct the command
		//
		cmd << mApps[index].mName.data() << " "
			<< mApps[index].mIpFlows[flowIndex].mProto     << " "
			<< mApps[index].mIpFlows[flowIndex].mDestIP    << " "
			<< mApps[index].mIpFlows[flowIndex].mMask      << " "
			<< mApps[index].mIpFlows[flowIndex].mStartPort << " "
			<< mApps[index].mIpFlows[flowIndex].mEndPort;

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		// 
		// Remove the flow from the app
		//
		for (int i=flowIndex; i<mApps[index].mNumIpFlows-1; i++)
		{
			mApps[index].mIpFlows[i] = mApps[index].mIpFlows[i+1];
		}
		mApps[index].mNumIpFlows--;
		return true;
	}	

	// 
	// Determine Protocol
	//
	OmnString proto;
	genProto(proto, isCorrect);
	

	// 
	// Determine destination IP
	//
	OmnString destIP;
	genDstIp(destIP, isCorrect);
	
	// 
	// Determine mask
	//
	OmnString mask;
	genMask(mask, isCorrect);
	
	// 
	// Determine Start End Port
	//
	int startPort;
	int endPort;
	
	genStartEndPort(startPort, endPort, isCorrect);

	// Determine the flow no exist
	if (isCorrect)
	{
		int flowIndex = mApps[index].mNumIpFlows;
		for (int i=0; i<flowIndex; i++)
			if (mApps[index].mIpFlows[i].mProto == proto && 
				mApps[index].mIpFlows[i].mDestIP == destIP &&
				mApps[index].mIpFlows[i].mMask == mask &&
				mApps[index].mIpFlows[i].mStartPort == startPort &&
				mApps[index].mIpFlows[i].mEndPort == endPort)
				return true;
	}

	// 
	// Construct the command
	//
	cmd << appName      << " "
		<< proto     << " "
		<< destIP    << " "
		<< mask      << " "
		<< startPort << " "
		<< endPort;

	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);
	
	return true;
}


bool	
AosInetCliTester::runRemoveUrlFlow()
{
	return true;
}


bool	
AosInetCliTester::runRemoveApp()
{
	//
	// inetctrl app remove <name>
	//

	//
	// 1. remove a exist name
	// 2. remove a unexist name
	//

	// 1. remove a exist name
	OmnString name;
	OmnString cmd = "inetctrl app remove ";

	//correct remove a app  
	if (PERCENTAGE(80) && mNumApps > 0)
	{

		int appIndex = RAND_INT(0, mNumApps-1);
		name = mApps[appIndex].mName;
		cmd << name;

		if (isInUsed(name, 1))
		{
			//aosRunCli(cmd, false);
			sendCmd(cmd,mRhcConn);
			return true;
		}

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		//save the remove
		for (int i=appIndex; i<(mNumApps-1); i++)
		{
			mApps[i] = mApps[i+1];
		}
		mNumApps--;

		return true;
	}
		
	// 2. remove a unexist name

	genName(name, 1, eMaxNameLen);
	
	if (appExist(name))
	{
		return true;
	}

	cmd = "inetctrl app remove ";
	cmd << name;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);

	return true;
}


bool	
AosInetCliTester::runShowApp()
{
	return true;
}


bool	
AosInetCliTester::runClearApp()
{
	return true;
}


bool	
AosInetCliTester::runAddTimeRule()
{
	//
	//inetctrl timerule name <name>
	//
	//1. add a correct name
	//2. add a uncorect name
	//   1. name exist
	//   2. name genentor error
	//
	OmnString name;
	OmnString cmd("inetctrl timerule name ");
	if (PERCENTAGE(90))
	{
		genName(name, 1, eMaxNameLen);
		cmd << name;

		if (eMaxTimeRule <= mNumTimeRule)
			return true;

		if (timeRuleExist(name))
			return true;

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		// 
		// save the timerule
		//
		timeRule[mNumTimeRule].mName = name;
		timeRule[mNumTimeRule].mNumTimeLimits = 0;
		timeRule[mNumTimeRule].mNumOnLineLimits = 0;
		mNumTimeRule++;
		return true;
	}

	// 
	// There are two error conditions: 
	//  1. Name too long
	//  2. Name already exist
	//
	if (PERCENTAGE(70) && mNumTimeRule > 0)
	{
		// 
		// Select a name from exist name list
		//
		int index = RAND_INT(0, mNumTimeRule-1);
		name = timeRule[index].mName;
	}
	else
	{
		// 
		// Create a name that is too long
		//
		genName(name, eMaxNameLen+10, eMaxTooLongNameLen);
	}

	cmd << name;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);

	return true;
}


bool	
AosInetCliTester::runAddTimeRangeCtr()
{
	//
	//inetctrl timerule addrule time <name> <start time> <end time> <time days>
	//
	//1. select a correct name
	//
	//2. select a uncorrect name
	//  1. name is noexist
	//  2. name is too long
	
	OmnString name;
	OmnString cmd("inetctrl timerule addrule time ");
	bool isCorrect = true;
	int index, ruleIndex;

	//
	//Determine whether to select a corret name 
	//
	if (PERCENTAGE(90) && mNumTimeRule > 0)
	{
		index = RAND_INT(0, mNumTimeRule-1);
		name = timeRule[index].mName;
	}
	else
	{
		selectNonExistName(name, "timerule");
		isCorrect = false;
	}

	// 
	// Determine whether to add a flow that is already in the timerule 
	//
	if (PERCENTAGE(20) && isCorrect)
	{
		if (timeRule[index].mNumTimeLimits <= 0)
			return true;

		ruleIndex = RAND_INT(0, timeRule[index].mNumTimeLimits-1);

		// 
		// Construct the command
		//
		cmd << timeRule[index].mTimeLimits[ruleIndex].mStartTime << " "
			<< timeRule[index].mTimeLimits[ruleIndex].mEndTime << " "
			<< timeRule[index].mTimeLimits[ruleIndex].mDayOfWeek;

		//aosRunCli(cmd, false);
		sendCmd(cmd,mRhcConn);
		return true;
	}

	// 
	// Construct a syntactically incorrect command
	//
	// 1. Determine the startTime, 95% can gennetor a correct starttime
	//
	OmnString startTime;
	OmnString endTime;
	
	genStartEndTime(startTime, endTime, isCorrect);

	//
	//3, Determine the dayOfWeak,
	//
	OmnString dayOfWeak;
	genDayOfWeek(dayOfWeak, isCorrect);

	// declare the rule is not exist
	if (isCorrect)
	{
		ruleIndex = timeRule[index].mNumTimeLimits;
		for (int i=0; i<ruleIndex; i++)
		if (timeRule[index].mTimeLimits[i].mStartTime == startTime &&
			timeRule[index].mTimeLimits[i].mEndTime == endTime &&
			timeRule[index].mTimeLimits[i].mDayOfWeek == dayOfWeak)
			return true;
	}

	if (isCorrect)
	{
		if (timeRule[index].mNumTimeLimits >= eMaxRangeLimits)
			return true;
	}

	//
	//save the time rule 
	//
	if (isCorrect)
	{
		int timeLimitIndex = timeRule[index].mNumTimeLimits;
		timeRule[index].mTimeLimits[timeLimitIndex].mStartTime = startTime;
		timeRule[index].mTimeLimits[timeLimitIndex].mEndTime = endTime;
		timeRule[index].mTimeLimits[timeLimitIndex].mDayOfWeek = dayOfWeak;
		timeRule[index].mNumTimeLimits++;
	}

	// 
	// Construct the command
	//
	cmd << name << " " << startTime << " " << endTime << " " << dayOfWeak;

	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);
	if (isCorrect)
		cout << cmd << endl;
	return true;
}


bool	
AosInetCliTester::runAddTimeOnlineCtr()
{
	//
	//inetctrl timerule addrule online <name> <online type> <online minutes> <online days>
	//
	int index, onLineIndex;
	bool isCorrect = true;
	OmnString name;
	OmnString cmd("inetctrl timerule addrule online "); 
	//
	//1, Determine the name,
	//
	if (PERCENTAGE(90) && mNumTimeRule > 0)
	{
			
		index = RAND_INT(0, mNumTimeRule-1);
		name = timeRule[index].mName;
	}
	else
	{
		selectNonExistName(name, "timerule");
		isCorrect = false;
	}

	// 
	// Determine whether to add a online rule that is already in the timerule 
	//
	if (PERCENTAGE(20) && isCorrect)
	{
		if (timeRule[index].mNumOnLineLimits <= 0)
			return true;
		// 
		// Randomly select a ruleIndex 
		//
		onLineIndex = RAND_INT(0, timeRule[index].mNumOnLineLimits-1);

		// 
		// Construct the command
		//
		cmd << timeRule[index].mName 			    				  << " " 
			<< timeRule[index].mOnLineLimits[onLineIndex].mOnLineType << " "
			<< timeRule[index].mOnLineLimits[onLineIndex].mOnLineMin  << " "
			<< timeRule[index].mOnLineLimits[onLineIndex].mOnLineDay;


		//aosRunCli(cmd, false);
		sendCmd(cmd,mRhcConn);
		return true;
	}

	//
	//2, Determine the onLineType,
	//
	OmnString onLineType;
	genOnLineType(onLineType, isCorrect);

	//
	//3. Determine the onLineMin
	//
	int onLineMin;
	genOnLineMin(onLineMin, isCorrect);

	//
	//4. Determine the onLineDay.
	//
	OmnString onLineDay;
	genDayOfWeek(onLineDay, isCorrect);

	//declare the rule not exist
	if (isCorrect)
	{
		onLineIndex = timeRule[index].mNumOnLineLimits;
		for (int i=0; i<onLineIndex; i++)
			if (timeRule[index].mOnLineLimits[i].mOnLineType == onLineType &&
				timeRule[index].mOnLineLimits[i].mOnLineMin == onLineMin &&
				timeRule[index].mOnLineLimits[i].mOnLineDay == onLineDay)
				return true;
	}

	if (isCorrect)
	{
		if (timeRule[index].mNumOnLineLimits >= eMaxOnLineLimits)
			return true;
	}

	//
	//save online 
	//
	if (isCorrect)
	{
		int timeLimitIndex = timeRule[index].mNumOnLineLimits;
		timeRule[index].mOnLineLimits[timeLimitIndex].mOnLineType = onLineType;
		timeRule[index].mOnLineLimits[timeLimitIndex].mOnLineMin = onLineMin;
		timeRule[index].mOnLineLimits[timeLimitIndex].mOnLineDay= onLineDay;
		timeRule[index].mNumOnLineLimits++;
	}

	// 
	// Construct the command
	//
	cmd << name << " "
		<< onLineType << " "
		<< onLineMin  << " "
		<< onLineDay;
			
	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);
	if (isCorrect)
		cout << cmd << endl;
	return true;
}


bool	
AosInetCliTester::runRemoveTimeRangeCtr()
{
	//
	//inetctrl timerule revrule time <name> <start time> <end time> <time days>
	//Correct: 
	//
	// Incorrect:
	// 1. timeRule not exist
	// 2. Limit not exist
	// 3. Command not correct:
	//    1. start time 
	//    2. end time 
	//    4. time days 
	//
	
	OmnString name;
	OmnString cmd("inetctrl timerule revrule time ");
	int index, timeIndex;
	bool isCorrect = true;

	if (PERCENTAGE(90) && mNumTimeRule > 0)
	{
			
		index = RAND_INT(0, mNumTimeRule-1);
		name = timeRule[index].mName;
	}
	else
	{
		selectNonExistName(name, "timerule");
		isCorrect = false;
	}

	//remove a exist timerange ctrl
	if (PERCENTAGE(90) && isCorrect)
	{
		if (timeRule[index].mNumTimeLimits <= 0)
			return true;

		int timeIndex = RAND_INT(0, timeRule[index].mNumTimeLimits-1);

		// 
		// Construct the command
		//
		cmd << name << " "
			<< timeRule[index].mTimeLimits[timeIndex].mStartTime << " "
			<< timeRule[index].mTimeLimits[timeIndex].mEndTime << " "
			<< timeRule[index].mTimeLimits[timeIndex].mDayOfWeek;

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		// 
		// Remove the time from the timerule 
		//
		for (int i=timeIndex; i<((timeRule[index].mNumTimeLimits)-1); i++)
		{
			timeRule[index].mTimeLimits[i].mStartTime = timeRule[index].mTimeLimits[i+1].mStartTime; 
			timeRule[index].mTimeLimits[i].mEndTime = timeRule[index].mTimeLimits[i+1].mEndTime;
			timeRule[index].mTimeLimits[i].mDayOfWeek = timeRule[index].mTimeLimits[i+1].mDayOfWeek;
		}
		timeRule[index].mNumTimeLimits--;
		return true;
	}	

	//
	// 1. Determine the startTime
	//
	OmnString startTime, endTime;
	genStartEndTime(startTime, endTime, isCorrect);

	//
	//3, Determine the dayOfWeak,
	//
	OmnString dayOfWeak;
	genDayOfWeek(dayOfWeak, isCorrect);

	//declare the timerule not exist
	if (isCorrect)
	{
		for (int i=0; i<timeRule[index].mNumTimeLimits; i++)
			if (timeRule[index].mTimeLimits[i].mStartTime == startTime &&
				timeRule[index].mTimeLimits[i].mEndTime == endTime &&
				timeRule[index].mTimeLimits[i].mDayOfWeek == dayOfWeak)
				return true;
	}

	// Construct the command
	cmd << name << " "
		<< startTime << " "
		<< endTime   << " "
		<< dayOfWeak;

	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);
	if (isCorrect)
		cout << cmd << endl;
	
	return true;
}


bool	
AosInetCliTester::runRemoveTimeOnlineCtr()
{

	//
	//inetctrl timerule revrule online <name> <online type> <online minutes> <online days>
	//
	// Incorrect:
	// 1. timeRule not exist
	// 2. Limit not exist
	// 3. Command not correct:
	//    1. type 
	//    2. online min 
	//    4. online day
	
	OmnString name;
	OmnString cmd("inetctrl timerule revrule online ");
	int index, onLineIndex;
	bool isCorrect = true;
	
	//create the name
	if (PERCENTAGE(90) && mNumTimeRule > 0)
	{
			
		index = RAND_INT(0, mNumTimeRule-1);
		name = timeRule[index].mName;
	}
	else
	{
		selectNonExistName(name, "timerule");
		isCorrect = false;
	}

	if (RAND_INT(0, 100) < 80 && isCorrect)
	{
		if (timeRule[index].mNumOnLineLimits <= 0)
			return true;

		onLineIndex = RAND_INT(0, timeRule[index].mNumOnLineLimits-1);

		// 
		// Construct the command
		//
		cmd << name << " "
			<< timeRule[index].mOnLineLimits[onLineIndex].mOnLineType << " "
			<< timeRule[index].mOnLineLimits[onLineIndex].mOnLineMin  << " "
			<< timeRule[index].mOnLineLimits[onLineIndex].mOnLineDay;

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		// 
		// Remove the time from the timerule 
		//
		for (int i=onLineIndex; i<(timeRule[index].mNumOnLineLimits-1); i++)
		{
			timeRule[index].mOnLineLimits[i].mOnLineType = timeRule[index].mOnLineLimits[i+1].mOnLineType; 
			timeRule[index].mOnLineLimits[i].mOnLineDay = timeRule[index].mOnLineLimits[i+1].mOnLineDay;
			timeRule[index].mOnLineLimits[i].mOnLineMin = timeRule[index].mOnLineLimits[i+1].mOnLineMin;
		}
		timeRule[index].mNumOnLineLimits--;

		return true;
	}	

	//
	//1, Determine the onLineType,
	//
	OmnString onLineType;
	genOnLineType(onLineType, isCorrect);

	//
	//2. Determine the onLineMin
	//
	int onLineMin;
	genOnLineMin(onLineMin, isCorrect);

	//
	//4. Determine the onLineDay.
	//
	OmnString onLineDay;
	genDayOfWeek(onLineDay, isCorrect);

	// 
	// Construct the command
	//
	cmd << name			<< " "
		<< onLineType   << " "
		<< onLineMin    << " "
		<< onLineDay;

	if (isCorrect)
	{
		for (int i=0; i<timeRule[index].mNumTimeLimits; i++)
			if (timeRule[index].mOnLineLimits[i].mOnLineType == onLineType &&
				timeRule[index].mOnLineLimits[i].mOnLineDay == onLineDay &&
				timeRule[index].mOnLineLimits[i].mOnLineMin == onLineMin)
				return true;
	}
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);
	return true;
}


bool	
AosInetCliTester::runRemoveRuleRule()
{

	//
	//inetctrl timerule remove <name>
	//
	//correct name, exist name
	//no exist name
	// 1, correct name
	// 2. error name
	
	OmnString name;
	OmnString cmd("inetctrl timerule remove ");
	bool isCorrect = true;

	//
	//Determine remove a exist name
	//
	if (RAND_INT(0, 100) > 20 && mNumTimeRule > 0) 
	{
		int timeIndex = RAND_INT(0, mNumTimeRule-1);
		name = timeRule[timeIndex].mName;
		cmd << name;

		if (isInUsed(name, 2))
		{
			//aosRunCli(cmd, false);
			sendCmd(cmd,mRhcConn);
			return true;
		}
		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		for (int i=timeIndex; i<(mNumTimeRule-1); i++)
			timeRule[i] = timeRule[i+1];
		mNumTimeRule--;
		return true;
	}
	else
	{
		genName(name, 1, eMaxNameLen);
		if (timeRuleExist(name))
		{
			return true;
		}
	}
		
	cmd << name;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);

	return true;
}


bool	
AosInetCliTester::runShowTimeRules()
{
	return true;
}


bool	
AosInetCliTester::runClearTimeRules()
{
	return true;
}


bool	
AosInetCliTester::runAddGroup()
{
	//
	//inetctrl group name <name>
	// 
	// Determine whether to run a correct command or an incorrect command
	//
	OmnString name;
	OmnString cmd("inetctrl group name ");
	
	if (RAND_INT(0, 100) > 10)
	{
		// 
		// Determine the name length and construct the command
		//
		genName(name, 1, eMaxNameLen);

		if (groupExist(name))
			return true;

		cmd << name;

		if (mNumGroups >= eMaxGroups)
			return true;

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		//aosRunCli(cmd, false);
		sendCmd(cmd,mRhcConn);

		cout << cmd << endl;
		// 
		// Add the application to our list
		//
		group[mNumGroups].mName = name;
		group[mNumGroups].mNumUser = 0;
		group[mNumGroups].mNumRule = 0;
		mNumGroups++;

		return true;
	}

	// 
	// There are two error conditions: 
	//  1. Name too long
	//  2. Name already exist
	//
	if (RAND_INT(0, 100) > 70 && mNumGroups > 0)
	{
		int index = RAND_INT(0, mNumGroups-1);
		name = group[index].mName;
	}
	else
	{
		genName(name, eMaxNameLen+10, eMaxTooLongNameLen);
	}

	cmd << name;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);

	return true;
}


bool	
AosInetCliTester::runAddUserByName()
{
	//inetctrl group adduser byname <name> <username>
	return true;
}


bool	
AosInetCliTester::runAddUserByIP()
{
	//
	//inetctrl group adduser byip <name> <ip> <mask>
	//
	//1. username is exist
	//	1. the ip address 
	//	2. the mask
	//2. username no exist
	//  1. the ip
	//  2. the mask
	//
	OmnString name;
	OmnString cmd("inetctrl group adduser byip ");
	bool isCorrect = true;
	int groupIndex, i;

	//
	//select a group name
	//
	if (RAND_INT(1, 100) < 90 && mNumGroups > 0)
	{
		groupIndex = RAND_INT(0, mNumGroups-1);
		name = group[groupIndex].mName;
	}
	else
	{
		isCorrect = false;
		selectNonExistGroup(name);
	}

	OmnString ipAddr;
	genSrcIp(ipAddr, isCorrect);

	OmnString mask;
	genMask(mask, isCorrect);

	cmd << name   << " " << ipAddr << " " << mask;
		
	//declare the user already exist
	if (isCorrect)
	{
		if (group[groupIndex].mNumUser >= eMaxUsersPerGroup)
			return true;
		if (userAlreadyExist(ipAddr, mask))
			isCorrect = false;
	}

	//save ipaddr, mask
	if (isCorrect)
	{
		i = group[groupIndex].mNumUser;
		group[groupIndex].groupUsers[i].ip = ipAddr;
		group[groupIndex].groupUsers[i].mask = mask;
		group[groupIndex].mNumUser++;
	}
			
	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);

	if (isCorrect)
		cout << cmd << endl;
		
	return true;
}


bool	
AosInetCliTester::runAddUserByMac()
{
	//
	//inetctrl group adduser bymac <name> <mac>
	//
	return true;
}


bool	
AosInetCliTester::runRemoveUserByName()
{
	//
	//inetctrl group revuser byname <name> <username>
	//
	return true;
}


bool	
AosInetCliTester::runRemoveUserByIP()
{
	//
	//inetctrl group revuser byip <name> <ip> <mask>
	//
	//1. the name is exist
	//2. the name no exist
	//
	OmnString name;
	OmnString cmd("inetctrl group revuser byip ");
	bool isCorrect = true;
	int groupIndex;

	//select a group name
	if (RAND_INT(1, 100) < 90 && mNumGroups > 0)
	{
		groupIndex = RAND_INT(0, mNumGroups-1);
		name = group[groupIndex].mName;
	}
	else
	{
		isCorrect = false;
		selectNonExistGroup(name);
	}

	//select a user
	if (RAND_INT(0, 100) < 90 && isCorrect) 
	{
		if (group[groupIndex].mNumUser <= 0)
			return true;

		int userIndex = RAND_INT(0, group[groupIndex].mNumUser-1);
		cmd << group[groupIndex].mName                  		<< " "
			<< group[groupIndex].groupUsers[userIndex].ip       << " "
			<< group[groupIndex].groupUsers[userIndex].mask;
				
		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		//remove ipaddr, mask
		for (int i=userIndex; i<(group[groupIndex].mNumUser-1); i++)
		{
			group[groupIndex].groupUsers[i].ip = group[groupIndex].groupUsers[i+1].ip; 
			group[groupIndex].groupUsers[i].mask = group[groupIndex].groupUsers[i+1].mask;
		}
		group[groupIndex].mNumUser--;

		return true;
	}
	//crate a user
	else
	{
		OmnString ipAddr;
		genSrcIp(ipAddr, isCorrect);

		OmnString mask;
		genMask(mask, isCorrect);

		if (isCorrect)
		{	
			for (int i=0; i<group[groupIndex].mNumUser; i++)
				if (group[groupIndex].groupUsers[i].ip == ipAddr &&
					group[groupIndex].groupUsers[i].mask == mask)
					return true;
		}

		cmd << name << " " << ipAddr << " " << mask;
		//aosRunCli(cmd, false);
		sendCmd(cmd,mRhcConn);
	}
	
	return true;
}


bool	
AosInetCliTester::runRemoveUserByMac()
{
	//
	//inetctrl group revuser bymac <name> <mac>
	//
	return true;
}


bool	
AosInetCliTester::runAddAssociation()
{
	//
	//inetctrl group addrule <name> <appname> <timename>
	//
	
	OmnString cmd("inetctrl group addrule ");
	OmnString groupName, appName, timeName;
	bool isCorrect =  true;
	int groupIndex, ruleIndex, i;

	//
	//create group name
	//
	if (RAND_INT(1, 100) < 90 && mNumGroups > 0)
	{
		groupIndex = RAND_INT(0, mNumGroups-1);
		groupName = group[groupIndex].mName;
	}
	else
	{
		isCorrect = false;
		selectNonExistGroup(groupName);
	}

	//
	//crate app name
	//
	if (RAND_INT(1, 100) < 90 && mNumApps > 0)
	{
		int appIndex = RAND_INT(0,mNumApps-1);
		appName = mApps[appIndex].mName;
	}
	else
	{
		genName(appName, 1, eMaxNameLen);
		for (i=0; i<mNumApps; i++)
			if (mApps[i].mName == appName)
				return true;
		isCorrect = false;
	}

	//
	//crate time rule name
	//
	if (RAND_INT(1, 100) < 90 && mNumTimeRule > 0)
	{
		int timeRuleIndex = RAND_INT(0,mNumTimeRule-1);
		timeName = timeRule[timeRuleIndex].mName;
	}
	else
	{
		genName(timeName, 1, eMaxNameLen);
		for (i=0; i<mNumTimeRule; i++)
			if (timeRule[i].mName == timeName)
				return true;
		isCorrect = false;
	}
	
	//
	//declare the association 
	//
	if (isCorrect)
	{
		ruleIndex = group[groupIndex].mNumRule;
		for (int i = 0; i < ruleIndex; i++)
		{
			if ((group[groupIndex].ruleList[i].appName == appName) && 
				(group[groupIndex].ruleList[i].timeName == timeName))
			{
				isCorrect = false;
				break;
			}
		}
	}

	if (isCorrect)
	{
		if (group[groupIndex].mNumRule >= eMaxRuleList)
			return true;
	}

	//
	//save the association
	//
	if (isCorrect)
	{
		ruleIndex = group[groupIndex].mNumRule;
		group[groupIndex].ruleList[ruleIndex].appName = appName;
		group[groupIndex].ruleList[ruleIndex].timeName = timeName;
		group[groupIndex].mNumRule++;
	}

	cmd << groupName << " " << 	appName	<< " " << timeName;
	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);
		
	return true;
}


bool	
AosInetCliTester::runRemoveAssociation()
{
	//
	//inetctrl group revrule <name> <appname> <timename>
	//
	int groupIndex, ruleIndex;
	bool isCorrect = true;
	OmnString groupName, appName, timeName;
	OmnString cmd("inetctrl group revrule ");

	//
	//create group name
	//
	if (RAND_INT(1, 100) < 90 && mNumGroups > 0)
	{
		groupIndex = RAND_INT(0, mNumGroups-1);
		groupName = group[groupIndex].mName;
	}
	else
	{
		isCorrect = false;
		selectNonExistGroup(groupName);
	}

	//
	//declare the association
	//
	if (RAND_INT(1, 100) < 90 && isCorrect)
	{
		if (group[groupIndex].mNumRule <= 0)
			return true;

		ruleIndex = RAND_INT(0, group[groupIndex].mNumRule-1);
		appName = group[groupIndex].ruleList[ruleIndex].appName;	
		timeName = group[groupIndex].ruleList[ruleIndex].timeName;

		cmd << groupName << " " << appName << " " << timeName;
		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);

		cout << cmd << endl;
		//remove the association
		
		for (int i=ruleIndex; i<group[groupIndex].mNumRule; i++)
		{
			group[groupIndex].ruleList[i].appName = group[groupIndex].ruleList[i+1].appName;	
			group[groupIndex].ruleList[i].timeName = group[groupIndex].ruleList[i+1].timeName;
		}
		group[groupIndex].mNumRule--;

		return true;
	}

	genName(appName, 1, eMaxNameLen);
	genName(timeName, 1, eMaxNameLen);

	if (isCorrect)
	{
		ruleIndex = group[groupIndex].mNumRule;
		for (int i = 0; i < ruleIndex; i++)
		{
			if ((group[groupIndex].ruleList[i].appName == appName) && 
				(group[groupIndex].ruleList[i].timeName == timeName))
				return true;
		}
	}

	cmd << groupName << " " << appName << " " << timeName;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);
	return true;
}


bool	
AosInetCliTester::runRemoveGroup()
{
	//
	// inetctrl group remove <name>
	//

	//
	// 1. remove a exist name
	// 2. remove a unexist name
	//

	// 1. remove a exist name
	OmnString name;
	OmnString cmd = "inetctrl group remove ";

	if (PERCENTAGE(70) && mNumGroups > 0)
	{
		int groupIndex = RAND_INT(0, mNumGroups-1);
		name = group[groupIndex].mName;
		cmd << name;

		//aosRunCli(cmd, true);
		sendCmd(cmd,mRhcConn);
		cout << cmd << endl;

		for (int i=groupIndex; i<(mNumGroups-1); i++)
			group[i] = group[i+1];
		mNumGroups--;

		return true;
	}
		
	// 2. remove a unexist name
	genName(name, 1, eMaxNameLen);
	
	if (groupExist(name))
		return true;

	cmd = "inetctrl app remove ";
	cmd << name;
	//aosRunCli(cmd, false);
	sendCmd(cmd,mRhcConn);

	return true;
}


bool	
AosInetCliTester::runShowGroup()
{
	return true;
}


bool	
AosInetCliTester::runClearGroup()
{
	return true;
}


bool	
AosInetCliTester::runTurnOnOff()
{
	return true;
}


bool
AosInetCliTester::appExist(const OmnString &name)
{
	for (int i=0; i<mNumApps; i++)
	{
		if (!strcasecmp(mApps[i].mName.data(), name.data()))
			return true;
	}

	return false;
}


bool
AosInetCliTester::selectNonExistName(OmnString &name, const OmnString &selectType)
{
	int count = 0;
	while (count < 50)
	{
		if (selectType == "app")
		{
			genName(name, 1, eMaxNameLen);

			if (!appExist(name))
			{
				return true;
			}
		}
		else if(selectType == "timerule")
		{
			genName(name, 1, eMaxNameLen);

			if (!timeRuleExist(name))
			{
				return true;
			}
		}
		else
		{
			OmnAlarm << "don't have this type" << enderr;
			return false;
		}

		count++;
	}

	// 
	// This should never happen
	//
	//OmnAlarm << "Something is wrong" << enderr;
	return false;
}


bool
AosInetCliTester::timeRuleExist(const OmnString &name)
{
	for (int i=0; i<mNumTimeRule; i++)
	{
		if (!strcasecmp(timeRule[i].mName.data(), name.data()))
			return true;
	}

	return false;
}





bool
AosInetCliTester::groupExist(const OmnString &name)
{
	for (int i=0; i<mNumGroups; i++)
	{
		if (!strcasecmp(group[i].mName.data(), name.data()))
			return true;
	}

	return false;
}



bool
AosInetCliTester::selectNonExistGroup(OmnString &name)
{
	int count = 0;
	while (count < 50)
	{
		name = OmnRandom::nextNoSpaceStr(1, eMaxNameLen);

		if (!groupExist(name))
		{
			return true;
		}
	}
}

bool
AosInetCliTester::selectNonExistUser(OmnString &username, int groupIndex)
{
	return true;
}


void
AosInetCliTester::genName(OmnString &name, int minLen, int maxLen)
{
	// 1, all
//	OmnRandom::nextNoSpaceStr(name, minLen, maxLen);

	// 2.[a-z], [A-Z], [0, 9] 
//	OmnRandom::nextLetterStr(name, minLen, maxLen);

	//3. [a-z], [A-Z], [0, 9], '_'.
	name = OmnRandom::nextVarName(minLen, maxLen, true);
	return;
}


void
AosInetCliTester::genProto(OmnString &proto, bool &isCorrect)
{
	if (RAND_INT(0, 100) > 5)
	{
		// 
		// Determine TCP or UDP
		//
		proto = "tcp";

//		if (OmnRandom::nextBool())
//		{
//			proto = "tcp";
//		}
//		else
//		{
//			proto = "udp";
//		}
	}
	else
	{
		OmnRandom::nextVarName(proto.getBuffer(), 2, 10, true);
		if (proto != "tcp" && proto != "udp") 
			isCorrect = false;
	}
	return;
}


void
AosInetCliTester::genDstIp(OmnString &dstIP, bool &isCorrect)
{
	if (RAND_INT(0, 100) > 5)
	{
		dstIP = getRandomDip();
	}
	else
	{
		dstIP = OmnRandom::nextNoSpaceStr(1, 20);
		if (!OmnIpAddr::isValidAddr(dstIP)) 
			isCorrect = false;
	}
	return;
}


void
AosInetCliTester::genSrcIp(OmnString &dstIP, bool &isCorrect)
{
	if (RAND_INT(0, 100) > 5)
	{
		dstIP = getRandomSip();
	}
	else
	{
		dstIP = OmnRandom::nextNoSpaceStr(1, 20);
		if (!OmnIpAddr::isValidAddr(dstIP)) 
			isCorrect = false;
	}
	return;
}


void
AosInetCliTester::genMask(OmnString &mask, bool &isCorrect)
{
	if (RAND_INT(0, 100) > 5)
	{
		mask = aos_next_mask_str(32,32);
	}
	else
	{
		mask = OmnRandom::nextNoSpaceStr(3, 20);
		isCorrect = false;
	}
	return;
}


void
AosInetCliTester::genStartEndPort(int &startPort, int &endPort, bool &isCorrect)
{
	//
	//create startPort
	//
	if (RAND_INT(0, 100) > 5)
	{
		// 
		// Use a correct port
		//
		startPort = RAND_INT(3000, 7999);
	}
	else
	{
		startPort = RAND_INT2(-10000, 0, 65536, 0x8fffffff);
		isCorrect = false;
	}

	//
	//create endPort
	//
	if (RAND_INT(0, 100) > 5 && (startPort > 0) && (startPort <= 65535))
	{
		int num = RAND_INT(1, 100);
		if (num < 70)
		{
			endPort = RAND_INT(startPort, 8000);
		}
		else if (num < 80)
		{
			endPort = RAND_INT(1, (startPort)-1);
			isCorrect = false;
		}
		else
		{
			endPort = 0;
			isCorrect = false;
		}
	}
	else
	{
		endPort = RAND_INT2(-10000, 0, 65536, 0x8fffffff);
		isCorrect = false;
	}
	return;
}


void
AosInetCliTester::genDayOfWeek(OmnString &dayOfWeak, bool &isCorrect)
{
	char *strDayOfWeek[] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};
	int indexOfStr, i;
	char strDay[32];
	char partDay[5];

	if (RAND_INT(0, 100) > 5)
	{
		int numOfDay = RAND_INT(1, 7);
			
		indexOfStr = RAND_INT(0, 6);
		sprintf(strDay, "%s", strDayOfWeek[indexOfStr]);
		strDayOfWeek[indexOfStr] = "";

		for(i = 0; i < numOfDay-1; i++)
		{
			indexOfStr = RAND_INT(0, 6);

			if (!strcmp(strDayOfWeek[indexOfStr], ""))
			{
				i--;
			}
			else
			{
				sprintf(partDay,"|%s", strDayOfWeek[indexOfStr]);
				strcat(strDay, partDay);
				strDayOfWeek[indexOfStr] = "";
			}
		}

		dayOfWeak = strDay;
	}
	else
	{
		OmnRandom::nextVarName(dayOfWeak.getBuffer(), 3, 12, true);
		isCorrect = false;
	}
	return;
}


void
AosInetCliTester::genOnLineType(OmnString &onLineType, bool &isCorrect)
{
	if (RAND_INT(0, 100) > 5)
	{
		// 
		// Determine day or week
		//
		if (OmnRandom::nextBool())
			onLineType = "day";
		else
			onLineType = "week";
	}
	else
	{
		onLineType = OmnRandom::nextMixedString(1, 10);
		if (onLineType != "day" && onLineType != "week")
			isCorrect = false;
	}
}


void
AosInetCliTester::genStartEndTime(OmnString &startTime, OmnString &endTime, bool &isCorrect)
{
	int startHour, startMin, startSec;
	bool isCorrectStartTime;
	startTime = "";
	if (PERCENTAGE(90))
	{
		startHour = RAND_INT(0, 23);
		startMin = RAND_INT(0, 59);
		startSec = RAND_INT(0, 59);
		startTime << startHour << ":" << startMin << ":" << startSec;
		isCorrectStartTime = true;
	}
	else
	{
		startTime = OmnRandom::nextLetterStr(1, 10);
		isCorrect = isCorrectStartTime = false;
	}
	// 
	// 2. Determine the endTime, 90% can gennetor a correct endTime 
	//
	
	int endHour, endMin, endSec;
	int randNum = RAND_INT(1, 100);
	if ((randNum < 70) && isCorrectStartTime)
	{
		endHour = RAND_INT(startHour, 23);
		endMin = RAND_INT(startMin, 59);
		endSec = RAND_INT(startSec, 59);

		endTime << endHour << ":" << endMin << ":" << endSec;
	}
	else if ((randNum < 90 ) && isCorrectStartTime)
	{
		endHour = RAND_INT(0, startHour);
		endMin = RAND_INT(0, startMin);
		endSec = RAND_INT(0, startSec);
		endTime << endHour << ":" << endMin << ":" << endSec;
	
		isCorrect = false;
	}
	else
	{
		endTime = OmnRandom::nextLetterStr(1, 10);
		isCorrect = false;
	}

	return;
}


void
AosInetCliTester::genOnLineMin(int &onLineMin, bool &isCorrect)
{
	if (RAND_INT(0, 100) > 5)
	{
		onLineMin = RAND_INT(0, sgSecPerDay);
	}
	else
	{
		if (OmnRandom::nextBool())
		{
			onLineMin = RAND_INT(sgSecPerWeek, sgSecPerDay + 10000);
		}
		else
		{
			onLineMin = RAND_INT(-100000, 0);
		}
		isCorrect = false;
	}
}


bool
AosInetCliTester::runShowConfig()
{
	return true;
}


bool
AosInetCliTester::runLogLevel()
{
	OmnString level;
	OmnString cmd("inetctrl log level ");
	bool isCorrect = true;

	if (RAND_INT(1, 100) > 80)
	{
		int i = RAND_INT(1, 3);
		switch (i)
		{
			case 1:
				level = "no";
				break;
			case 2:
				level = "block";
				break;
			case 3:
				level = "all";
				break;
			default:
				return false;
		}
	}
	else
	{
		level = OmnRandom::nextLetterStr(1, 10);
		if (level != "all" ||
			level != "block" ||
			level != "all")
			isCorrect = false;
	}

	cmd << level;
	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);
	return true;
}
	

bool
AosInetCliTester::runDefaultPolicy()
{
	OmnString cmd("inetctrl default policy ");
	OmnString policy;
	bool isCorrect = true;

	if (RAND_INT(1, 100) < 80)
	{
		policy = (RAND_BOOL())?"allow":"deny";
		mDefaultPolicy = (policy == "allow")?true:false;
	}
	else
	{
		policy = OmnRandom::nextLetterStr(1, 10);
		if (policy == "allow" || policy == "deny")
		{
			policy == "incorrectpolicy";
		}
		isCorrect = false;
	}

	cmd << policy;
	//aosRunCli(cmd, isCorrect);
	sendCmd(cmd,mRhcConn);
	return true;
}
	

bool
AosInetCliTester::runSaveConfig()
{
	return true;
}


bool
AosInetCliTester::config(const int maxCommands)
{
	int numCommands = OmnRandom::nextInt(1, maxCommands);
	return basicTest(numCommands);
}


bool AosInetCliTester::isInUsed(const OmnString &name, const int type)
{
	int i, j;
	switch (type)
	{
		case 1:
			for (i=0; i<mNumGroups; i++)
			{	
				for (j=0; j<group[i].mNumRule; j++)
				{
					if (group[i].ruleList[j].appName == name)
						return true;
				}
			}
			break;

		case 2:
			for (i=0; i<mNumGroups; i++)
			{	
				for (j=0; j<group[i].mNumRule; j++)
				{
					if (group[i].ruleList[j].timeName == name) 
						return true;
				}
			}
			break;

		default:
			return false;
	}

	return false;
}


bool AosInetCliTester::userAlreadyExist(const OmnString &ipAddr, const OmnString &mask)
{
	int netmask, ip, i, j;
	int userIP, userMask; 

	if (mask == "255.255.255.255")
	{
		ip = (int)inet_addr(ipAddr.data());
		for (i = 0; i<mNumGroups; i++) 
		{
			for (j=0; j<group[i].mNumUser; j++)
			{
				userIP = (int)inet_addr(group[i].groupUsers[j].ip.data());
				userMask = (int)inet_addr(group[i].groupUsers[j].mask.data());
				if ((userIP & userMask) == (ip & userMask))
					return true;
			}
		}
	}
	else
	{
		for (i = 0; i<mNumGroups; i++) 
		{
			for (j=0; j<group[i].mNumUser; j++)
			{
				if (group[i].groupUsers[j].ip == ipAddr &&
					group[i].groupUsers[j].mask == mask)
					return true;
			}
		}
	}

	return false;
}


bool
AosInetCliTester::isIPInGroup(const int ip)
{
	int i, j, userIP, userMask;

	for (i = 0; i<mNumGroups; i++) 
	{
		for (j=0; j<group[i].mNumUser; j++)
		{
			userIP = (int)inet_addr(group[i].groupUsers[j].ip.data());
			userMask = (int)inet_addr(group[i].groupUsers[j].mask.data());

			if ((ip & userMask) == (userIP & userMask))
				return true;
		}
	}

	return false;
}


bool
AosInetCliTester::isApp(const int port, const OmnString &proto)
{
	int i, j, n, m,userIP, userMask;
	OmnString appName;

	for (i = 0; i<mNumGroups; i++) 
	{
		for (j=0; j<group[i].mNumRule; j++)
		{
			appName = group[i].ruleList[j].appName;
			for (n = 0; n <mNumApps; n++)
			{
				if (mApps[n].mName == appName)
				{
					for (m=0; m<mApps[n].mNumIpFlows; m++)
					{
						if ( port > mApps[n].mIpFlows[m].mStartPort && 
							port < mApps[n].mIpFlows[m].mEndPort &&
							mApps[n].mIpFlows[m].mProto == proto)
							return true;  
					}
				}
			}
		}
	}

	return false;
}


bool    
AosInetCliTester::groupAppExist(const int localIP, const int remotePort, const OmnString &proto)
{
	int i, j, n, m, h, userIP, appIp;
	OmnString appName;

	for (i = 0; i<mNumGroups; i++) 
	{
		for (j=0; j<group[i].mNumUser; j++)
		{
			if (group[i].mNumRule <= 0)
				continue;

			userIP = (int)inet_addr(group[i].groupUsers[j].ip.data());
			if (localIP != userIP)
				continue;

			for (n=0; n<group[i].mNumRule; n++)
			{
				appName = group[i].ruleList[n].appName;

				for (m=0; m<mNumApps; m++)
				{
					if (mApps[m].mName != appName)
						continue;

					for (h=0; h<mApps[m].mNumIpFlows; h++)
					{
						if ( remotePort > mApps[m].mIpFlows[h].mStartPort && 
							remotePort < mApps[m].mIpFlows[h].mEndPort &&
							mApps[m].mIpFlows[h].mProto == proto) 
							return true;
					}
				}
			}
		}
	}

	return false;
}


bool
AosInetCliTester::getDefaultPolicy()
{
	return mDefaultPolicy;
}

bool getIpPortInGroupApp(OmnString& sIP, OmnString& dIP, int &dport, OmnString &protocol, OmnString &startTime, OmnString &endTime, OmnString& dayOfWeak)
{/*
	int i, j, n, m, h, userip, usermask, appip, appmask;
	int isOk;

	for (i = 0; i<mNumGroups; i++) 
	{
		if (group[i].mNumRule<= 0)
			continue;

		int j = RAND_INT(0, group[i].mNumUser);
		sIP = group[i].groupUsers[j].ip;

		for (n=0; n<group[i].mNumRule; n++)
		{
			isOK = 0;
			OmnString appName = group[i].ruleList[n].appName;
			OmnString timeName = group[i].ruleList[n].timeName;

			for (m=0; m<mNumApps; m++)
			{
				if (mApps[m].mName != appName)
					continue;
				if (mApps[m].mNumIpFlows <= 0)
					continue;

				h = RAND_INT(0, mApps[m].mNumIpFlows);
				dIP = mApps[m].mIpFlows[h].mDestIP;
				protocol = mApps[m].mIpFlows[h].mProto; 
				dport = RAND_INT(mApps[m].mIpFlows[h].mStartPort, mApps[m].mIpFlows[h].mEndPort);
				isOK = 1;
			}

			for (y=0; y<mNumTimeRule; y++)
			{
				if (timeRule[y].mName != timeName)
					continue;
				if (timeRule[y].mNumTimeLimits <= 0)	
					continue;

				int r = RAND_INT(0, timeRule[y].mNumTimeLimits);
				startTime = timeRule[y].mOnLineLimits[r].mStartTime;
				endTime = timeRule[y].mOnLineLimits[r].mEndTime;
				dayOfWeak = timeRule[y].mOnLineLimits[r].mDayOfWeek;
				if (isOK == 1)
					return true;
			}
		}
	}
	*/
	return false;

}

///////////////////////////////
//
//This function to generate a IP address, this IP address hit one groups
//
bool
AosInetCliTester::onlyHitGroup(u32 &sip)
{
	int i, j, count;
	OmnString sourceIP, sourceMask;
	struct in_addr s_ip;
	OmnString ip;

	if (mNumGroups <= 0)
	{
		return false;
	}

	count = 0;
	while (count < 50) {
		count++;
		i = RAND_INT(0, mNumGroups-1);
		if (group[i].mNumUser <= 0)
		{
			continue;
		}

		//
		//Find a group IP address and mask 
		//
		j =	RAND_INT(0, group[i].mNumUser-1);
		sourceIP = group[i].groupUsers[j].ip;
		sourceMask = group[i].groupUsers[j].mask;

		//
		//Get the IP address
		//
		ip = genInRangeIp(OmnIpAddr(sourceIP), OmnIpAddr(sourceMask));

		memset(&s_ip, 0, sizeof(struct in_addr));
		inet_aton(ip.data(), &s_ip);
		sip = s_ip.s_addr;

		return true;
	}

	return false;
}

//
//This function generate IP address, port, and protocol hit application
//
bool
AosInetCliTester::onlyHitApp(u32 &dip, int &dport, OmnString &protocol)
{
	int i, j, count;
	OmnString app_ip, app_mask, proto;
	struct in_addr s_ip; 
	OmnString ip;
	
	if (mNumApps <= 0)
	{
		return false;	
	}

	count = 0;
	while (count < 1000) {
		count++;

		i = RAND_INT(0, mNumApps-1);
		if (mApps[i].mNumIpFlows <= 0)
		{
			continue;
		}
		
		//
		//Get Application flow, IP address, mask, port and protocol
		//
		j = RAND_INT(0, mApps[i].mNumIpFlows-1);
		app_ip = mApps[i].mIpFlows[j].mDestIP;
		app_mask = mApps[i].mIpFlows[j].mMask;
		dport = RAND_INT(mApps[i].mIpFlows[j].mStartPort, mApps[i].mIpFlows[j].mEndPort);
		protocol = mApps[i].mIpFlows[j].mProto;

		//
		//generate IP, port and protocol
		//
		ip = genInRangeIp(OmnIpAddr(app_ip), OmnIpAddr(app_mask));
		memset(&s_ip, 0, sizeof(struct in_addr));
		inet_aton(ip.data(), &s_ip);
		dip = s_ip.s_addr;
		return true;
	}
	return false;	
}	

bool
AosInetCliTester::notHitGroupApp(u32 &sip, u32 &dip, int &dport, OmnString &protocol)
{
	int ip, mask;
	int port;
	int i, j, m, n, h;
	OmnString source_ip, source_mask, dest_ip, dest_mask, proto;
	struct in_addr ip_t;

loop:
	//Get sip, and not hit group.
	OmnString sipStr = getRandomSip();
	sip = OmnIpAddr(sipStr).toInt(); 

	for (i=0; i<mNumGroups; i++) {
		for (h=0; h<group[i].mNumUser; h++) {
			source_ip = group[i].groupUsers[h].ip;
			source_mask = group[i].groupUsers[h].mask;

			memset(&ip_t, 0, sizeof(struct in_addr));
			inet_aton(source_ip.data(), &ip_t);
			ip = ip_t.s_addr;
			memset(&ip_t, 0, sizeof(struct in_addr));
			inet_aton(source_mask.data(), &ip_t);
			mask = ip_t.s_addr;
			
			if ((ip&mask) == (sip&mask))
			{
				goto loop;
			}
		}
	}
		
again:
	//Generate dip, dport and protocol, not hit application.
	OmnString dipStr = getRandomDip();
	dip =  OmnIpAddr(dipStr).toInt();

	dport = RAND_INT(3000, 8000);
	protocol = "tcp";
			
	for (n=0; n<mNumApps; n++) {
			for (m=0; m<mApps[n].mNumIpFlows; m++) {
				dest_ip = mApps[n].mIpFlows[m].mDestIP;
				dest_mask = mApps[n].mIpFlows[m].mMask;
				proto = mApps[n].mIpFlows[m].mProto;

				memset(&ip_t, 0, sizeof(struct in_addr));
				inet_aton(dest_ip.data(), &ip_t);
				ip = ip_t.s_addr;
				memset(&ip_t, 0, sizeof(struct in_addr));
				inet_aton(dest_mask.data(), &ip_t);
				mask = ip_t.s_addr;
				if (((ip&mask) == (sip&mask)) && (proto == protocol)
						&& (dport > mApps[n].mIpFlows[m].mStartPort)
						&& (dport < mApps[n].mIpFlows[m].mEndPort))
					goto again;
			}
	}

	return true;	
}	

bool
AosInetCliTester::hitGroupApp(u32 &sip, u32 &dip, int &dport, OmnString &protocol)
{
	int groupIndex, userIndex, appIndex, flowIndex;
	OmnString userip, appip;
	OmnString sIP, sMask, dIP, dMask;
	struct in_addr ip;
	int count;
	count = 0;

	if(mNumGroups <= 0 || mNumApps <= 0)
	{
		return false;
	}

	while (count < 50) 
	{
		count++;
		groupIndex = RAND_INT(0, mNumGroups-1);

		if ((group[groupIndex].mNumRule<= 0) || (group[groupIndex].mNumUser <= 0))
		{
			continue;
		}

		//Get group IP, Mask
		userIndex = RAND_INT(0, group[groupIndex].mNumUser-1);
		sIP = group[groupIndex].groupUsers[userIndex].ip;
		sMask = group[groupIndex].groupUsers[userIndex].mask;
		
		// Randomly select a rule from the group
		int ruleIndex = RAND_INT(0, group[groupIndex].mNumRule-1);
		OmnString appName = group[groupIndex].ruleList[ruleIndex].appName;
		if ((appIndex = getAppIndex(appName)) < 0)
		{
			OmnAlarm << "Application not found: " << appName << enderr;
			return false;
		}
		
		if(mApps[appIndex].mNumIpFlows <= 0)
		{
			continue;
		}
		flowIndex = RAND_INT(0, mApps[appIndex].mNumIpFlows - 1);
		//Get application IP, Mask, port, protocol
		dIP = mApps[appIndex].mIpFlows[flowIndex].mDestIP;
		dMask = mApps[appIndex].mIpFlows[flowIndex].mMask;
		protocol = mApps[appIndex].mIpFlows[flowIndex].mProto; 
		dport = RAND_INT(mApps[appIndex].mIpFlows[flowIndex].mStartPort,
			   	mApps[appIndex].mIpFlows[flowIndex].mEndPort);
			
		//create the sip, dip, dport, protocol, and the sip is generate by IP&Mask 
		appip = genInRangeIp(OmnIpAddr(dIP), OmnIpAddr(dMask));
//		memset(&ip, 0, sizeof(struct in_addr));
//		inet_aton(appip.data(), &ip);
//		dip = ip.s_addr;
		dip = OmnIpAddr(appip).toInt();
		
		userip = genInRangeIp(OmnIpAddr(sIP), OmnIpAddr(sMask));
//		memset(&ip, 0, sizeof(struct in_addr));
//		inet_aton(userip.data(), &ip);
//		sip = ip.s_addr;
		sip = OmnIpAddr(userip).toInt();
		
		return true;
	}

	return false;

}

//check the time
static bool timeInRange(OmnString &startTime, OmnString &endTime, OmnString &dayOfWeek, OmnString &ctrTime)
{
	char time[128], week[128];
	int hour, min, sec, s_hour, s_min, s_sec, e_hour, e_min, e_sec;

	memset(week, 0, sizeof(week));
	memset(time, 0, sizeof(time));
	memcpy(time, startTime.data(), sizeof(time));
	sscanf(time, "%d:%d:%d", &s_hour, &s_min, &s_sec);
//cout << s_hour << " : " << s_min << " : " << s_sec << endl;
	memset(time, 0, sizeof(time));
	memcpy(time, endTime.data(), sizeof(time));
	sscanf(time, "%d:%d:%d", &e_hour, &e_min, &e_sec);
//cout << e_hour << " : " << e_min << " : " << e_sec << endl;
//cout << "int time is" << ctrTime << endl;
//	time_t curTime = ctrTime;
//cout << "time_t time is" << ctrTime << endl;
	memcpy(time, ctrTime, sizeof(time));
	sscanf(time, "%s %*s %*d %d:%d:%d %*d", &week, &hour, &min, &sec);
//cout << week << " : " << hour << " : " << min << " : " << sec  << endl;
	week[0] += 32;
//cout << "dayofweek is:" << dayOfWeek << endl;
	if (strstr(dayOfWeek.data(), week)) {
		
		if (((hour*3600+min*60+sec) < (e_hour*3600+e_min*60+e_sec))
				&& ((hour*3600+min*60+sec) > (s_hour*3600+s_min*60+s_sec)))
		{
			return true;
		}		
	}

	return false;
}

//Find user form the group list
bool AosInetCliTester::findUser(u32 &sip, int &groupIndex)
{
	int i, j; 
	u32 ip, mask;
	OmnString s_ip, s_mask;
	struct in_addr ip_t;
	
	for (i=0; i<mNumGroups; i++) {
		for (j=0; j<group[i].mNumUser; j++) {
			s_ip = group[i].groupUsers[j].ip;
			s_mask = group[i].groupUsers[j].mask;

//			memset(&ip_t, 0, sizeof(struct in_addr));
//			inet_aton(s_ip.data(), &ip_t);
//			ip = ip_t.s_addr;

//			memset(&ip_t, 0, sizeof(struct in_addr));
//			inet_aton(s_mask.data(), &ip_t);
//			mask = ip_t.s_addr;
			
			ip = OmnIpAddr(s_ip).toInt();
			mask = OmnIpAddr(s_mask).toInt();


			if ((sip&mask) == (ip&mask)) {
		//		cout << "Find User." << endl;
				groupIndex = i;
				return true;
			}
		}
	}
	return false;
}

// This function find the application for the application list,
// and the application match the dip, dport, protocol
bool AosInetCliTester::findApp(u32& dip, int& dport, OmnString& protocol, int& groupIndex, int& ruleIndex)
{
	int i, j, k;
	OmnString appName, d_ip, d_mask, proto;
	u32 ip, mask;
	struct in_addr ip_t;

	for (i=0; i<group[groupIndex].mNumRule; i++) {
		appName = group[groupIndex].ruleList[i].appName;
		for (j=0; j<mNumApps; j++) {
				if(mApps[j].mNumIpFlows <= 0)
				{
					continue;
				}
			if (mApps[j].mName == appName) {
				
				
				for (k=0; k<mApps[j].mNumIpFlows; k++) {
					d_ip = mApps[j].mIpFlows[k].mDestIP;
					d_mask = mApps[j].mIpFlows[k].mMask;
					proto = mApps[j].mIpFlows[k].mProto;
					
					ip = OmnIpAddr(d_ip).toInt();
					mask = OmnIpAddr(d_mask).toInt();

					if (((ip&mask) == (dip&mask)) && (protocol == proto)
						&&(dport >= mApps[j].mIpFlows[k].mStartPort)
						&&(dport <= mApps[j].mIpFlows[k].mEndPort)) {
					//	cout << "find in app" << endl;
						ruleIndex = i;
						return true;
					}
				}
			}
		}
	}

	return false;
}


// This function match the time if in the time range control
bool 
AosInetCliTester::timeMatch(int& groupIndex, int& ruleIndex, OmnString &crtTime) 
{
	int i, j;
	OmnString timeName;
	OmnString startTime, endTime, dayOfWeek;

	timeName = group[groupIndex].ruleList[ruleIndex].timeName;
	
	if(mNumTimeRule <= 0)
	{
		return false;
	}

	for (i=0; i<mNumTimeRule; i++) {
		if (timeRule[i].mName == timeName) {
			if(timeRule[i].mNumTimeLimits <= 0)
			{
				continue;
			}
			
			for (j=0; j<timeRule[i].mNumTimeLimits; j++) {
				startTime = timeRule[i].mTimeLimits[j].mStartTime;
				endTime = timeRule[i].mTimeLimits[j].mEndTime;
				dayOfWeek = timeRule[i].mTimeLimits[j].mDayOfWeek;

				if (timeInRange(startTime, endTime, dayOfWeek, crtTime))
				{
				   	return true; 
				}	
			}
		}
	}
	return false;
}

bool
AosInetCliTester::isConnPass(u32 &sip, int &sport,
							 u32 &dip, int &dport,
							 OmnString &protocol, OmnString &crtTime)
{
	int groupIndex, ruleIndex;
	//find user, if not finded, go default policy
	if (!findUser(sip, groupIndex)) 
	{
	return getDefaultPolicy();	
	}
	//find application ,if not filded, go defautl policy
	if (!findApp(dip, dport, protocol, groupIndex, ruleIndex))
	{
		return getDefaultPolicy();	
	}
	// time match, if in the time range, go access, other deny
	return timeMatch(groupIndex, ruleIndex, crtTime);
}

bool
AosInetCliTester::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
{
	OmnString strRslt;
	return sendCmd(cmd,dstServer,strRslt);
}

bool
AosInetCliTester::sendCmd(const OmnString &cmd,
					 const OmnTcpClientPtr &dstServer,
					 OmnString &strRslt)
{
	OmnString errmsg;
	OmnRslt rslt;
	
	int sec = 5;
	bool timeout = false;
	bool connBroken;
	
	if (!dstServer->isConnGood())
	{
		dstServer->closeConn();
		dstServer->connect(errmsg);
		if (!dstServer->isConnGood())
		{
			//alarm and return;
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
			return false;
		}
	}

	OmnString sendBuff = cmd;
	sendBuff << "\n\n";
	int sendLen = sendBuff.length();
	dstServer->writeTo(sendBuff, sendLen);

	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!dstServer->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		//alarm and return
//        showCmd(cmd,revBuff);
        OmnAlarm << "cannot successfully get message from cli server" << rslt.getErrmsg() << enderr;
        return false;
	}	
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))
    {
//       showCmd(cmd,revBuff);
    }

	strRslt.assign(revBuff->getBuffer(),revBuff->getDataLength());
    
	return true;
}

OmnString
AosInetCliTester::genInRangeIp(const OmnIpAddr &netaddr, const OmnIpAddr &netmask)
{
	//
	//Generate an IP in subnet(netaddr, netmask)
	//
	
	u32 n = netaddr.getIPv4();
	u32 m = netmask.getIPv4();

	OmnIpAddr ip = OmnIpAddr(htonl(aos_next_int(0, ntohl(0xffffffff & ~m)) + ntohl(n & m)));
	return ip.toString();
}

OmnString
AosInetCliTester::getRandomDip()
{
	//
	//Random generate an IP in the IP range that defined by IpProxy
	//
	OmnString remoteIP = genInRangeIp(OmnIpAddr(mRemoteIpRange), OmnIpAddr(mRemoteNetmask));
	
	return remoteIP;	
}

OmnString
AosInetCliTester::getRandomSip()
{
	//
	//Random generate an IP in the IP range that defined by IpProxy
	//
	OmnString localIP = genInRangeIp(OmnIpAddr(mLocalIpRange), OmnIpAddr(mLocalNetmask));
	
	return localIP;	
}


bool
AosInetCliTester::init()
{
	mRhc_Ip			= OmnIpAddr("172.22.155.122");
	mRhc_Port		= 28000;
	mRhcConn		= new OmnTcpClient("Rhc",mRhc_Ip,mRhc_Port,1,OmnTcp::eNoLengthIndicator);

	mLocalIpRange = "12.1.0.0";
	mLocalNetmask = "255.255.0.0";
	mRemoteIpRange = "11.1.0.0";
	mRemoteNetmask = "255.255.0.0";

	sendCmd("inetctrl clear config",mRhcConn);
	sendCmd("inetctrl on", mRhcConn);	
	sendCmd("inetctrl default policy allow", mRhcConn);
	return true;	
}

bool
AosInetCliTester::printfAll()
{
	int groupIndex, appIndex, timeIndex, userIndex, ruleIndex, flowIndex, rangeIndex;

#define gr group[groupIndex].ruleList[ruleIndex]
#define gu group[groupIndex].groupUsers[userIndex]
	cout << "\n\n--------------------group---------------------\n" << endl;
	for (groupIndex =0; groupIndex<mNumGroups; groupIndex++) 
	{
		cout << group[groupIndex].mName << endl;
		for (ruleIndex=0; ruleIndex<group[groupIndex].mNumRule; ruleIndex++)
			cout <<gr.appName << " " << gr.timeName << endl;

		for (userIndex=0; userIndex<group[groupIndex].mNumUser; userIndex++)
			cout << gu.ip << " " << gu.mask << endl; 
	}	

	cout << "\n\n--------------------app-----------------------\n" << endl;
#define af mApps[appIndex].mIpFlows[flowIndex]
	for (appIndex=0; appIndex<mNumApps; appIndex++) 
	{
		cout << mApps[appIndex].mName << endl;
		for (flowIndex=0; flowIndex<mApps[appIndex].mNumIpFlows; flowIndex++)
			cout << af.mProto << " " << af.mDestIP << " " << af.mMask << " " << af.mStartPort << " "<< af.mEndPort << endl;
	}

	cout << "\n\n--------------------time---------------------\n" << endl;
#define tr timeRule[timeIndex].mTimeLimits[rangeIndex]
	for (timeIndex=0; timeIndex<mNumTimeRule; timeIndex++)
	{
		cout << timeRule[timeIndex].mName << endl;
		for (rangeIndex=0; rangeIndex<timeRule[timeIndex].mNumTimeLimits; rangeIndex++)
			cout << tr.mStartTime << " " << tr.mEndTime << " " << tr.mDayOfWeek << endl;
	}	
	cout << "\n\n";

	return true;
}


int 
AosInetCliTester::getAppIndex(const OmnString &appName)
{
	for (int i=0; i<mNumApps; i++)
	{
		if (mApps[i].mName == appName)
		{ 
			return i;
		}

		
	}


	OmnAlarm << "Application not found: " << appName << enderr;
	return -1;
}

bool
AosInetCliTester::clearAll()
{
	//
	//reset all stat
	//
	
	sendCmd("inetctrl clear config",mRhcConn);	
	
	mNumApps = 0;
	mNumTimeRule = 0;
	mNumGroups = 0;
	
	return true;	
	
}

