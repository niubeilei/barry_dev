////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliQosTester.cpp
// Description:
//       
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliQosTester.h"

#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/RandomSelector.h"
#include "Porting/Sleep.h"
#include "QoS/qos_rule_cli.h"


extern int qos_rule_verify_mac_priority(char* mac,char *priority);
extern int qos_rule_verify_proto_priority(char* proto,char *priority);
extern int qos_rule_verify_vlan_priority(int tag,char *priority);
extern int qos_rule_verify_interface_priority(char* name,char *priority);
extern int qos_rule_verify_port_priority(char* direction,int port,char *priority);
extern int qos_rule_verify_general_priority(u32 srcIp,u16 srcPort,u32 dstIp,u16 dstPort,char* protocol,char* interface,char* priority);

AosCliQosTester::AosCliQosTester()
{
	mName = "AosCliQosTester";
}


bool 
AosCliQosTester::start()
{
	mNumMacs = 0; 
	mNumProtos = 0; 
	mNumPorts = 0; 
	mNumVLans = 0; 
	mNumInterfaces = 0; 
	mNumGenerals = 0; 
	AosQos_initRuleModule();
	test(100000);
//	cout << "mac" << endl;
//	testMacCli(5000000);
//	cout << "protocol" << endl;
//	testProtoCli(50000);
//	cout << "interface" << endl;
//	testInterfaceCli(50000);
//	cout << "vlan" << endl;
//	testVLanCli(50000);
//	cout << "port" << endl;
//	testPortCli(50000);
//	cout << "general" << endl;
//	testGeneralCli(50000);
	return true;
}

bool
AosCliQosTester::test(int tries)
{
	
	int OprIndex[] = {0,1,2,3,4,5};
	u16 OprProb[] = {17,17,17,17,16,16};
	AosRandomSelector oprSelector(OprIndex, OprProb, 6);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		int times = OmnRandom::nextInt(3,5);
		switch (cmdSelector)
		{
		case 0:
			 testMacCli(times);
			 break;

		case 1:
			 testProtoCli(times);
			 break;

		case 2:
			 testPortCli(times);
			 break;

		case 3:
			 testVLanCli(times);
			 break;

		case 4:
			 testInterfaceCli(times);
			 break;
	
		case 5:
			 testGeneralCli(times);
			 break;
	
		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}



static const char* sgPriority[3] = {"high","medium","low"};

bool
AosCliQosTester::testProtoCli(int tries)
{
	int OprIndex[] = {0,1,2,3,4};
	u16 OprProb[] = {35,25,20,10,10};
	AosRandomSelector oprSelector(OprIndex, OprProb, 5);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		switch (cmdSelector)
		{
		case 0:
			 runAddProto();
			 break;

		case 1:
			 runRemoveProto();
			 break;

		case 2:
			 runShowProto();
			 break;

		case 3:
			 runClearProto();
			 break;

		case 4:
			 runModifyProto();
			 break;
	
		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}

bool
AosCliQosTester::testGeneralCli(int tries)
{
	int OprIndex[] = {0,1,2,3,4};
	u16 OprProb[] = {35,25,20,10,10};
	AosRandomSelector oprSelector(OprIndex, OprProb, 5);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		switch (cmdSelector)
		{
		case 0:
			 runAddGeneral();
			 break;

		case 1:
			 runRemoveGeneral();
			 break;

		case 2:
			 runShowGeneral();
			 break;

		case 3:
			 runClearGeneral();
			 break;

		case 4:
			 runModifyGeneral();
			 break;
	
		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}


bool
AosCliQosTester::testVLanCli(int tries)
{
	int OprIndex[] = {0,1,2,3,4};
	u16 OprProb[] = {35,25,20,10,10};
	AosRandomSelector oprSelector(OprIndex, OprProb, 5);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		switch (cmdSelector)
		{
		case 0:
			 runAddVLan();
			 break;

		case 1:
			 runRemoveVLan();
			 break;

		case 2:
			 runShowVLan();
			 break;

		case 3:
			 runClearVLan();
			 break;

		case 4:
			 runModifyVLan();
			 break;
	
		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}

bool
AosCliQosTester::testPortCli(int tries)
{
	int OprIndex[] = {0,1,2,3,4};
	u16 OprProb[] = {35,25,20,10,10};
	AosRandomSelector oprSelector(OprIndex, OprProb, 5);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		switch (cmdSelector)
		{
		case 0:
			 runAddPort();
			 break;

		case 1:
			 runRemovePort();
			 break;

		case 2:
			 runShowPort();
			 break;

		case 3:
			 runClearPort();
			 break;

		case 4:
			 runModifyPort();
			 break;
	
		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}

bool
AosCliQosTester::testInterfaceCli(int tries)
{
	int OprIndex[] = {0,1,2,3,4};
	u16 OprProb[] = {35,25,20,10,10};
	AosRandomSelector oprSelector(OprIndex, OprProb, 5);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		switch (cmdSelector)
		{
		case 0:
			 runAddInterface();
			 break;

		case 1:
			 runRemoveInterface();
			 break;

		case 2:
			 runShowInterface();
			 break;

		case 3:
			 runClearInterface();
			 break;

		case 4:
			 runModifyInterface();
			 break;
	
		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}

bool
AosCliQosTester::testMacCli(int tries)
{
	int OprIndex[] = {0,1,2,3,4};
	u16 OprProb[] = {35,25,20,10,10};
	AosRandomSelector oprSelector(OprIndex, OprProb, 5);
	while (tries)
	{
		// Determine which operations to run
		int cmdSelector = oprSelector.next();
		switch (cmdSelector)
		{
		case 0:
			 runAddMac();
			 break;

		case 1:
			 runRemoveMac();
			 break;

		case 2:
			 runShowMac();
			 break;

		case 3:
			 runClearMac();
			 break;

		case 4:
			 runModifyMac();
			 break;

		default:
			 OmnAlarm << "Invalid command Index: " << cmdSelector << enderr;
			 return false;
		}
		tries--;
	}
	return true;
}

static const char* sgProtos[2] = {"tcp","udp"};


bool 
AosCliQosTester::runAddVLan()
{
	//
	//
	//test cli command "qos vlanrule add VLan priority"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new VLan or a existed one
	//	if a new VLan
	//		cmd will return true
	//	else (it's a existed VLan)
	//		cmd will return false
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2);
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {60,30,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int tag;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new vlan
			if (mNumVLans > eMaxVLan-1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,1);
			cmd = "";
			tag = OmnRandom::nextInt(0,4095);
			//cout << "tag= "<< tag<<endl; 
			cmd << "qos vlanrule add " << tag << " " << sgPriority[priority] ;
			if(isNewVLan(tag))
			{
				//cout << cmd << endl;
				aosRunCli(cmd,true);
				mVLans[mNumVLans].tag = tag;
				mVLans[mNumVLans].priority = sgPriority[priority];
				ret = checkVLan(mVLans[mNumVLans]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumVLans++;
				//cout << mNumVLans << endl;
			}
			break;
		
		case 1:         //run a valid cmd with a existed VLan
			if (mNumVLans < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumVLans-1);
			cmd = "";
			cmd << "qos vlanrule add " << mVLans[r1].tag << " " << sgPriority[priority] ;
			//cout << cmd << endl;
			//cout << "VLan= "<< VLan<<endl; 
			aosRunCli(cmd,false);
			ret = checkVLan(mVLans[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


		
bool 
AosCliQosTester::runRemoveVLan()
{
	//
	//
	//test cli command "remove vlan vlanAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new vlan or a exister vlan
	//	if not a new vlan
	//		cmd will return true
	//	else (it's a existed vlan)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int tag;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new vlan
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			tag = OmnRandom::nextInt(0,4095);
			cmd << "qos vlanrule remove " << tag ;
			if(isNewVLan(tag))
			{
				aosRunCli(cmd,false);
				for (int i=0;i<mNumVLans;i++)
				{
					ret = checkVLan(mVLans[i]);
					OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				}
			}
			break;
		
		case 1:         //run a valid cmd with a existed vlan
			if (mNumVLans < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumVLans-1);
			cmd = "";
			cmd << "qos vlanrule remove " << mVLans[r1].tag;
			ret = checkVLan(mVLans[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,true);
			mVLans[r1] = mVLans[mNumVLans-1];
			mNumVLans--;
			if (r1 == mNumVLans)
			{
				r1--;
			}
			if (mNumVLans > 0)
			{
				ret = checkVLan(mVLans[r1]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			//	cout << mvlanAddrs[r1].vlan << endl;
			}
			else
			{
				ret = checkVLan(mVLans[0]);
				OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			}

			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

		
bool 
AosCliQosTester::runShowVLan()
{
	OmnString cmd;
	bool ret;
	cmd << "qos vlanrule show" ;
	if (mNumVLans > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumVLans;i++)
		{
			ret = checkVLan(mVLans[i]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
		}
	}
	return true;
}

bool 
AosCliQosTester::runClearVLan()
{
	OmnString cmd;
	bool ret;
	cmd << "qos vlanrule clear" ;
	if (mNumVLans > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumVLans;i++)
		{
			ret = checkVLan(mVLans[i]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		}
		mNumVLans = 0;
	}
	return true;
}

bool 
AosCliQosTester::runModifyVLan()
{
	//
	//
	//test cli command "remove vlan vlanAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new vlan or a exister vlan
	//	if not a new vlan
	//		cmd will return true
	//	else (it's a existed vlan)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int tag;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new vlan
			if (mNumVLans >= eMaxVLan)
			{
				return true;
			}
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			tag = OmnRandom::nextInt(0,4095);
			cmd << "qos vlanrule modify " << tag << " " << sgPriority[priority];
			if(isNewVLan(tag))
			{
				aosRunCli(cmd,true);
				mVLans[mNumVLans].tag = tag;
				mVLans[mNumVLans].priority = sgPriority[priority];
				ret = checkVLan(mVLans[mNumVLans]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumVLans++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed vlan
			if (mNumVLans < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumVLans-1);
			cmd = "";
			cmd << "qos vlanrule modify " << mVLans[r1].tag << " " << sgPriority[priority];
			aosRunCli(cmd,true);
			mVLans[r1].priority = sgPriority[priority];
			ret = checkVLan(mVLans[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

static const char* sgDirection[2] = {"sending","receiving"};

bool 
AosCliQosTester::runAddPort()
{
	//
	//
	//test cli command "qos Portrule add Port priority"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new Port or a existed one
	//	if a new Port
	//		cmd will return true
	//	else (it's a existed Port)
	//		cmd will return false
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2);
	int r1Case[3] = {0,1,1};
	u16 r1Prob[3] = {60,30,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int r2;
	int port;
	OmnString app;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new Port
			if (mNumPorts > eMaxPort-1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,1);
			r2 = OmnRandom::nextInt(0,1);
			cmd = "";
			port = OmnRandom::nextInt(1,65535);
			app = OmnRandom::nextMixedString(1,8);
			//app = "a";
			//cout << "tag= "<< tag<<endl; 
			cmd << "qos portrule add " << sgDirection[r2] << " " << port << " " << app << " " << sgPriority[priority];
			if(isNewPort(port,sgDirection[r2]))
			{
				//cout << cmd << endl;
				aosRunCli(cmd,true);
				mPorts[mNumPorts].port = port;
				mPorts[mNumPorts].app = app;
				mPorts[mNumPorts].direction = sgDirection[r2];
				mPorts[mNumPorts].priority = sgPriority[priority];
				ret = checkPort(mPorts[mNumPorts]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumPorts++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed Port
			if (mNumPorts < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumPorts-1);
			r2 = OmnRandom::nextInt(0,1);
			app = OmnRandom::nextMixedString(1,8);
			//app = "a";
			cmd = "";
			cmd << "qos portrule add " << mPorts[r1].direction << " " << mPorts[r1].port << " " << app << " " << sgPriority[priority] ;
			//cout << cmd << endl;
			//cout << "Port= "<< Port<<endl; 
			ret = checkPort(mPorts[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,false);
			ret = checkPort(mPorts[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


		
bool 
AosCliQosTester::runRemovePort()
{
	//
	//
	//test cli command "remove Port PortAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new Port or a exister Port
	//	if not a new Port
	//		cmd will return true
	//	else (it's a existed Port)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int r2;
	int port;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new Port
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			r2 = OmnRandom::nextInt(0,1);
			port = OmnRandom::nextInt(1,65535);
			cmd << "qos portrule remove " << sgDirection[r2] << " " << port ;
			if(isNewPort(port,sgDirection[r2]))
			{
				aosRunCli(cmd,false);
				for (int i=0;i<mNumPorts;i++)
				{
					ret = checkPort(mPorts[i]);
					OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				}
			}
			break;
		
		case 1:         //run a valid cmd with a existed Port
			if (mNumPorts < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumPorts-1);
			cmd = "";
			cmd << "qos portrule remove " << mPorts[r1].direction << " " <<mPorts[r1].port;
			ret = checkPort(mPorts[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,true);
			mPorts[r1] = mPorts[mNumPorts-1];
			mNumPorts--;
			if (r1 == mNumPorts)
			{
				r1--;
			}
			if (mNumPorts > 0)
			{
				ret = checkPort(mPorts[r1]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			//	cout << mPortAddrs[r1].Port << endl;
			}
			else
			{
				ret = checkPort(mPorts[0]);
				OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			}

			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

		
bool 
AosCliQosTester::runShowPort()
{
	OmnString cmd;
	bool ret;
	cmd << "qos portrule show" ;
	if (mNumPorts > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumPorts;i++)
		{
			ret = checkPort(mPorts[i]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
		}
	}
	return true;
}

bool 
AosCliQosTester::runClearPort()
{
	OmnString cmd;
	bool ret;
	cmd << "qos portrule clear" ;
	if (mNumPorts > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumPorts;i++)
		{
			ret = checkPort(mPorts[i]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		}
		mNumPorts = 0;
	}
	return true;
}

bool 
AosCliQosTester::runModifyPort()
{
	//
	//
	//test cli command "remove Port PortAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new Port or a exister Port
	//	if not a new Port
	//		cmd will return true
	//	else (it's a existed Port)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int r2;
	int port;
	OmnString cmd;
	OmnString app;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new Port
			if (mNumPorts >= eMaxPort)
			{
				return true;
			}
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			r2 = OmnRandom::nextInt(0,1);
			app = OmnRandom::nextMixedString(1,8);
			//app = "a";
			port = OmnRandom::nextInt(1,65535);
			cmd << "qos portrule modify " << sgDirection[r2] << " " << port << " " << app << " " << sgPriority[priority];
			if(isNewPort(port,sgDirection[r2]))
			{
				aosRunCli(cmd,true);
				mPorts[mNumPorts].port = port;
				mPorts[mNumPorts].app = app;				
				mPorts[mNumPorts].direction = sgDirection[r2];
				mPorts[mNumPorts].priority = sgPriority[priority];
				ret = checkPort(mPorts[mNumPorts]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumPorts++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed Port
			if (mNumPorts < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumPorts-1);
			r2 = OmnRandom::nextInt(0,1);
			app = OmnRandom::nextMixedString(1,8);
			//app = "a";
			cmd = "";
			cmd << "qos portrule modify " << mPorts[r1].direction << " " << mPorts[r1].port << " " << app << " " << sgPriority[priority];
			aosRunCli(cmd,true);
			mPorts[mNumPorts].app = app;				
			mPorts[mNumPorts].direction = sgDirection[r2];
			mPorts[r1].priority = sgPriority[priority];
			ret = checkPort(mPorts[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

static const char* sgInterface[2] = {"eth0","eth1"};

bool 
AosCliQosTester::runAddInterface()
{
	//
	//
	//test cli command "qos ifrule add Interface priority"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new Interface or a existed one
	//	if a new Interface
	//		cmd will return true
	//	else (it's a existed Interface)
	//		cmd will return false
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2);
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {60,30,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString name;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new Interface
			if (mNumInterfaces > eMaxInterface-1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,1);
			cmd = "";
			name = sgInterface[r1];
			//cout << "name= "<< name<<endl; 
			cmd << "qos ifrule add " << name << " " << sgPriority[priority] ;
			if(isNewInterface(name))
			{
				//cout << cmd << endl;
				aosRunCli(cmd,true);
				mInterfaces[mNumInterfaces].name = name;
				mInterfaces[mNumInterfaces].priority = sgPriority[priority];
				ret = checkInterface(mInterfaces[mNumInterfaces]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumInterfaces++;
				//cout << mNumInterfaces << endl;
			}
			break;
		
		case 1:         //run a valid cmd with a existed Interface
			if (mNumInterfaces < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumInterfaces-1);
			cmd = "";
			cmd << "qos ifrule add " << mInterfaces[r1].name << " " << sgPriority[priority] ;
			//cout << cmd << endl;
			//cout << "Interface= "<< Interface<<endl; 
			aosRunCli(cmd,false);
			ret = checkInterface(mInterfaces[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


		
bool 
AosCliQosTester::runRemoveInterface()
{
	//
	//
	//test cli command "remove Interface InterfaceAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new Interface or a exister Interface
	//	if not a new Interface
	//		cmd will return true
	//	else (it's a existed Interface)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString name;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new Interface
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			name = sgInterface[r1];
			cmd << "qos ifrule remove " << name ;
			if(isNewInterface(name))
			{
				aosRunCli(cmd,false);
				for (int i=0;i<mNumInterfaces;i++)
				{
					ret = checkInterface(mInterfaces[i]);
					OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				}
			}
			break;
		
		case 1:         //run a valid cmd with a existed Interface
			if (mNumInterfaces < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumInterfaces-1);
			cmd = "";
			cmd << "qos ifrule remove " << mInterfaces[r1].name;
			ret = checkInterface(mInterfaces[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,true);
			mInterfaces[r1] = mInterfaces[mNumInterfaces-1];
			mNumInterfaces--;
			if (r1 == mNumInterfaces)
			{
				r1--;
			}
			if (mNumInterfaces > 0)
			{
				ret = checkInterface(mInterfaces[r1]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			//	cout << mInterfaceAddrs[r1].Interface << endl;
			}
			else
			{
				ret = checkInterface(mInterfaces[0]);
				OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			}

			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

		
bool 
AosCliQosTester::runShowInterface()
{
	OmnString cmd;
	bool ret;
	cmd << "qos ifrule show" ;
	if (mNumInterfaces > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumInterfaces;i++)
		{
			ret = checkInterface(mInterfaces[i]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
		}
	}
	return true;
}

bool 
AosCliQosTester::runClearInterface()
{
	OmnString cmd;
	bool ret;
	cmd << "qos ifrule clear" ;
	if (mNumInterfaces > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumInterfaces;i++)
		{
			ret = checkInterface(mInterfaces[i]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		}
		mNumInterfaces = 0;
	}
	return true;
}

bool 
AosCliQosTester::runModifyInterface()
{
	//
	//
	//test cli command "remove Interface InterfaceAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new Interface or a exister Interface
	//	if not a new Interface
	//		cmd will return true
	//	else (it's a existed Interface)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString name;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new Interface
			if (mNumInterfaces >= eMaxInterface)
			{
				return true;
			}
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			name = sgInterface[r1];
			cmd << "qos ifrule modify " << name << " " << sgPriority[priority];
			if(isNewInterface(name))
			{
				aosRunCli(cmd,true);
				mInterfaces[mNumInterfaces].name = name;
				mInterfaces[mNumInterfaces].priority = sgPriority[priority];
				ret = checkInterface(mInterfaces[mNumInterfaces]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumInterfaces++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed Interface
			if (mNumInterfaces < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumInterfaces-1);
			cmd = "";
			cmd << "qos ifrule modify " << mInterfaces[r1].name << " " << sgPriority[priority];
			aosRunCli(cmd,true);
			mInterfaces[r1].priority = sgPriority[priority];
			ret = checkInterface(mInterfaces[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


bool
AosCliQosTester::isNewInterface(const OmnString &name)
{
	for (int i=0;i<mNumInterfaces;i++)
	{
		if (mInterfaces[i].name == name)
		{
			return false;
		}
	}
	return true;
}


bool 
AosCliQosTester::runAddProto()
{
	//
	//
	//test cli command "qos protorule add proto priority"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new proto or a existed one
	//	if a new proto
	//		cmd will return true
	//	else (it's a existed proto)
	//		cmd will return false
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2);
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {60,30,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString proto;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new mac
			if (mNumProtos > eMaxProto-1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,1);
			cmd = "";
			proto = sgProtos[r1];
			//cout << "proto= "<< proto<<endl; 
			cmd << "qos protorule add " << proto << " " << sgPriority[priority] ;
			if(isNewProto(proto))
			{
				//cout << cmd << endl;
				aosRunCli(cmd,true);
				mProtos[mNumProtos].proto = proto;
				mProtos[mNumProtos].priority = sgPriority[priority];
				ret = checkProto(mProtos[mNumProtos]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumProtos++;
				//cout << mNumProtos << endl;
			}
			break;
		
		case 1:         //run a valid cmd with a existed proto
			if (mNumProtos < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumProtos-1);
			cmd = "";
			cmd << "qos protorule add " << mProtos[r1].proto << " " << sgPriority[priority] ;
			//cout << cmd << endl;
			//cout << "proto= "<< proto<<endl; 
			aosRunCli(cmd,false);
			ret = checkProto(mProtos[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


		
bool 
AosCliQosTester::runRemoveProto()
{
	//
	//
	//test cli command "remove mac macAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new mac or a exister mac
	//	if not a new mac
	//		cmd will return true
	//	else (it's a existed mac)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString proto;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new mac
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			proto = sgProtos[r1];
			cmd << "qos protorule remove " << proto ;
			if(isNewProto(proto))
			{
				aosRunCli(cmd,false);
				for (int i=0;i<mNumProtos;i++)
				{
					ret = checkProto(mProtos[i]);
					OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				}
			}
			break;
		
		case 1:         //run a valid cmd with a existed mac
			if (mNumProtos < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumProtos-1);
			cmd = "";
			cmd << "qos protorule remove " << mProtos[r1].proto;
			ret = checkProto(mProtos[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,true);
			mProtos[r1] = mProtos[mNumProtos-1];
			mNumProtos--;
			if (r1 == mNumProtos)
			{
				r1--;
			}
			if (mNumProtos > 0)
			{
				ret = checkProto(mProtos[r1]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			//	cout << mMacAddrs[r1].mac << endl;
			}
			else
			{
				ret = checkProto(mProtos[0]);
				OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			}

			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

		
bool 
AosCliQosTester::runShowProto()
{
	OmnString cmd;
	bool ret;
	cmd << "qos protorule show" ;
	if (mNumProtos > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumProtos;i++)
		{
			ret = checkProto(mProtos[i]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
		}
	}
	return true;
}

bool 
AosCliQosTester::runClearProto()
{
	OmnString cmd;
	bool ret;
	cmd << "qos protorule clear" ;
	if (mNumProtos > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumProtos;i++)
		{
			ret = checkProto(mProtos[i]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		}
		mNumProtos = 0;
	}
	return true;
}

bool 
AosCliQosTester::runModifyProto()
{
	//
	//
	//test cli command "remove mac macAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new mac or a exister mac
	//	if not a new mac
	//		cmd will return true
	//	else (it's a existed mac)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString proto;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new mac
			if (mNumProtos >= eMaxProto)
			{		
				return true;
			}
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			proto = sgProtos[r1];
			cmd << "qos protorule modify " << proto << " " << sgPriority[priority];
			if(isNewProto(proto))
			{
				aosRunCli(cmd,true);
				mProtos[mNumProtos].proto = proto;
				mProtos[mNumProtos].priority = sgPriority[priority];
				ret = checkProto(mProtos[mNumProtos]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumProtos++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed mac
			if (mNumProtos < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumProtos-1);
			cmd = "";
			cmd << "qos protorule modify " << mProtos[r1].proto << " " << sgPriority[priority];
			aosRunCli(cmd,true);
			mProtos[r1].priority = sgPriority[priority];
			ret = checkProto(mProtos[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

bool 
AosCliQosTester::runAddMac()
{
	//
	//
	//test cli command "add mac macAddr priority"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new mac or a exister mac
	//	if a new mac
	//		cmd will return true
	//	else (it's a existed mac)
	//		cmd will return false
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {60,30,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString macAddr;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new mac
			if (mNumMacs >= eMaxMac)
			{
				return true;
			}
			cmd = "";
			macAddr = getMacAddr();
			cmd << "qos macrule add " << macAddr << " " << sgPriority[priority] ;
			if(isNewMac(macAddr))
			{
				aosRunCli(cmd,true);
				mMacAddrs[mNumMacs].mac = macAddr;
				mMacAddrs[mNumMacs].priority = sgPriority[priority];
				ret = checkMac(mMacAddrs[mNumMacs]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumMacs++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed mac
			if (mNumMacs < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumMacs-1);
			cmd = "";
			cmd << "qos macrule add " << mMacAddrs[r1].mac << " " << sgPriority[priority] ;
			//cout << cmd << endl;
			aosRunCli(cmd,false);
			ret = checkMac(mMacAddrs[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

bool
AosCliQosTester::isNewMac(const char* mac)
{
	OmnString temp(mac,17);
	for (int i=0;i<mNumMacs;i++)
	{
		if (mMacAddrs[i].mac == temp)
		{
			return false;
		}
	}
	return true;
}


bool
AosCliQosTester::isNewProto(const OmnString &proto)
{
	for (int i=0;i<mNumProtos;i++)
	{
		if (mProtos[i].proto == proto)
		{
			return false;
		}
	}
	return true;
}

bool
AosCliQosTester::isNewVLan(int tag)
{
	for (int i=0;i<mNumVLans;i++)
	{
		if (mVLans[i].tag == tag)
		{
			return false;
		}
	}
	return true;
}

bool
AosCliQosTester::isNewPort(int port,const OmnString &direction)
{
	for (int i=0;i<mNumPorts;i++)
	{
		if (mPorts[i].port == port && mPorts[i].direction == direction)
		{
			return false;
		}
	}
	return true;
}

OmnString
AosCliQosTester::getMacAddr()
{
	int num[6];
	for (int i=0;i<6;i++)
	{
		num[i] = OmnRandom::nextInt(0,255);
	}
	OmnString ret;
	ret = "";
	char c[2];
	int r1 = OmnRandom::nextInt(0,0);
	if (r1 == 0)
	{
		for (int i=0;i<5;i++)
		{
			sprintf(c,"%02x",num[i]);
			ret << c << ":"; 
		}
		sprintf(c,"%02x",num[5]);
		ret << c;
		return ret;
	}
	if (r1 == 1)
	{
		for (int i=0;i<5;i++)
		{
			sprintf(c,"%02X",num[i]);
			ret << c << ":"; 
		}
		sprintf(c,"%02X",num[5]);
		ret << c;
		return ret;
	}
	return 0;
}


bool
AosCliQosTester::isNewGeneral(qosGeneralInfo general)
{
	for (int i=0;i<mNumGenerals;i++)
	{
		if (	mGenerals[i].srcIp == general.srcIp && 
				mGenerals[i].srcPort ==	general.srcPort && 
				mGenerals[i].dstIp == general.dstIp && 
				mGenerals[i].dstPort ==  general.dstPort && 
				mGenerals[i].protocol == general.protocol && 
				mGenerals[i].interface == general.interface
			)
		{
			return false;
		}
	}
	return true;
}

AosCliQosTester::qosGeneralInfo
AosCliQosTester::getGeneral()
{
	qosGeneralInfo general;
	general.srcIp << OmnRandom::nextInt(0,255) << "."  << OmnRandom::nextInt(0,255) << "." << OmnRandom::nextInt(0,255) << "." << OmnRandom::nextInt(0,255);
	general.srcPort = OmnRandom::nextInt(1,65535);
	general.dstIp << OmnRandom::nextInt(0,255) << "."  << OmnRandom::nextInt(0,255) << "." << OmnRandom::nextInt(0,255) << "." << OmnRandom::nextInt(0,255);
	general.dstPort = OmnRandom::nextInt(1,65535);
	general.protocol = sgProtos[OmnRandom::nextInt(0,1)];
	general.interface = sgInterface[OmnRandom::nextInt(0,1)];
	general.priority = sgPriority[OmnRandom::nextInt(0,2)];
	return general;
}


bool
AosCliQosTester::runRemoveMac()
{
	
	//
	//
	//test cli command "remove mac macAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new mac or a exister mac
	//	if not a new mac
	//		cmd will return true
	//	else (it's a existed mac)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString macAddr;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new mac
			cmd = "";
			macAddr = getMacAddr();
			cmd << "qos macrule remove " << macAddr ;
			if(isNewMac(macAddr))
			{
				aosRunCli(cmd,false);
			}
			break;
		
		case 1:         //run a valid cmd with a existed mac
			if (mNumMacs < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumMacs-1);
			cmd = "";
			cmd << "qos macrule remove " << mMacAddrs[r1].mac;
			ret = checkMac(mMacAddrs[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,true);
			ret = checkMac(mMacAddrs[r1]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			//cout << mMacAddrs[r1].mac << endl;
			mMacAddrs[r1] = mMacAddrs[mNumMacs-1];
			mNumMacs--;
			if (r1 == mNumMacs)
			{
				r1--;
			}
			if (mNumMacs > 0)
			{
				ret = checkMac(mMacAddrs[r1]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			//	cout << mMacAddrs[r1].mac << endl;
			}
			else
			{
				ret = checkMac(mMacAddrs[0]);
				OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			}
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

bool
AosCliQosTester::runClearMac()
{
	OmnString cmd;
	bool ret;
	cmd << "qos macrule clear" ;
	if (mNumMacs > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumMacs;i++)
		{
			ret = checkMac(mMacAddrs[i]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		}
		mNumMacs = 0;
	}
	return true;
}


bool
AosCliQosTester::runShowMac()
{
	OmnString cmd;
	bool ret;
	cmd << "qos macrule show" ;
	if (mNumMacs > 0)
	{
		//cout << cmd << endl;
		for (int i=0;i<mNumMacs;i++)
		{
			ret = checkMac(mMacAddrs[i]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
		}
		aosRunCli(cmd,true);
	}
	return true;
}

bool
AosCliQosTester::runModifyMac()
{
	//
	//
	//test cli command "remove mac macAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new mac or a exister mac
	//	if not a new mac
	//		cmd will return true
	//	else (it's a existed mac)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	OmnString macAddr;
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new mac
			if (mNumMacs >= eMaxMac)
			{
				return true;
			}
			cmd = "";
			macAddr = getMacAddr();
			cmd << "qos macrule modify " << macAddr << " " << sgPriority[priority];
			if(isNewMac(macAddr))
			{
				aosRunCli(cmd,true);
				mMacAddrs[mNumMacs].mac = macAddr;
				mMacAddrs[mNumMacs].priority = sgPriority[priority];
				ret = checkMac(mMacAddrs[mNumMacs]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumMacs++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed mac
			if (mNumMacs < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumMacs-1);
			cmd = "";
			cmd << "qos macrule modify " << mMacAddrs[r1].mac << " " << sgPriority[priority];
			aosRunCli(cmd,true);
			mMacAddrs[r1].priority = sgPriority[priority];
			ret = checkMac(mMacAddrs[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


bool
AosCliQosTester::checkMac(qosMacInfo mac)
{
	char addr[30];
	char priority[30];
	strcpy(addr,mac.mac);
	strcpy(priority,mac.priority);
	int ret = qos_rule_verify_mac_priority(addr,priority);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

bool
AosCliQosTester::checkProto(qosProtoInfo proto)
{
	char protoId[30];
	char priority[30];
	strcpy(protoId,proto.proto);
	strcpy(priority,proto.priority);
	int ret = qos_rule_verify_proto_priority(protoId,priority);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

bool
AosCliQosTester::checkVLan(qosVLanInfo vLan)
{
	int tag = vLan.tag;
	char priority[30];
	strcpy(priority,vLan.priority);
	int ret = qos_rule_verify_vlan_priority(tag,priority);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

bool
AosCliQosTester::checkInterface(qosInterfaceInfo interface)
{
	char name[30];
	char priority[30];
	strcpy(name,interface.name);
	strcpy(priority,interface.priority);
	int ret = qos_rule_verify_interface_priority(name,priority);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

bool
AosCliQosTester::checkPort(qosPortInfo port)
{
	char direction[30];
	char priority[30];
	strcpy(direction,port.direction);
	strcpy(priority,port.priority);
	int ret = qos_rule_verify_port_priority(direction,port.port,priority);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

bool
AosCliQosTester::checkGeneral(qosGeneralInfo general)
{
	u32 srcIp = OmnIpAddr(general.srcIp).toInt();
	u32 dstIp = OmnIpAddr(general.dstIp).toInt();
	u16 srcPort = general.srcPort;
	u16 dstPort = general.dstPort;
	char protocol[30];
	char interface[30];
	char priority[30];
	strcpy(protocol,general.protocol);
	strcpy(interface,general.interface);
	strcpy(priority,general.priority);
	int ret = qos_rule_verify_general_priority(srcIp,srcPort,dstIp,dstPort,protocol,interface,priority);
	if (ret == 0)
	{
		return true;
	}
	return false;
}

bool
AosCliQosTester::runLoadConfig()
{
	return true;
}

bool
AosCliQosTester::runSaveConfig()
{
	return true;
}

bool
AosCliQosTester::runResetConfig()
{
	return true;
}


bool 
AosCliQosTester::runAddGeneral()
{
	//
	//
	//test cli command "qos Generalrule add General priority"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new General or a existed one
	//	if a new General
	//		cmd will return true
	//	else (it's a existed General)
	//		cmd will return false
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2);
	int r1Case[3] = {0,1,1};
	u16 r1Prob[3] = {60,30,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int r2;
	qosGeneralInfo general; 
	OmnString cmd;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new General
			if (mNumGenerals > eMaxGeneral-1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,1);
			r2 = OmnRandom::nextInt(0,1);
			cmd = "";
			general = getGeneral();
			cmd << "qos generalrule add " 
				<< general.srcIp << " " 
				<< general.srcPort << " " 
				<< general.dstIp << " " 
				<< general.dstPort << " "
				<< general.protocol << " "
				<< general.interface << " "
				<< general.priority;
			if(isNewGeneral(general))
			{
				//cout << cmd << endl;
				aosRunCli(cmd,true);
				mGenerals[mNumGenerals] = general;
				ret = checkGeneral(mGenerals[mNumGenerals]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumGenerals++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed General
			if (mNumGenerals < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumGenerals-1);
			r2 = OmnRandom::nextInt(0,1);
			general = mGenerals[r1];
			//app = "a";
			cmd = "";
			cmd << "qos generalrule add " 
				<< general.srcIp << " " 
				<< general.srcPort << " " 
				<< general.dstIp << " " 
				<< general.dstPort << " "
				<< general.protocol << " "
				<< general.interface << " "
				<< sgPriority[priority];
			ret = checkGeneral(mGenerals[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,false);
			ret = checkGeneral(mGenerals[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}


		
bool 
AosCliQosTester::runRemoveGeneral()
{
	//
	//
	//test cli command "remove General GeneralAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new General or a exister General
	//	if not a new General
	//		cmd will return true
	//	else (it's a existed General)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int r2;
	OmnString cmd;
	qosGeneralInfo general;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new General
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			r2 = OmnRandom::nextInt(0,1);
			general = getGeneral();
			cmd << "qos generalrule remove " 
				<< general.srcIp << " " 
				<< general.srcPort << " " 
				<< general.dstIp << " " 
				<< general.dstPort << " "
				<< general.protocol << " "
				<< general.interface;
			if(isNewGeneral(general))
			{
				aosRunCli(cmd,false);
				for (int i=0;i<mNumGenerals;i++)
				{
					ret = checkGeneral(mGenerals[i]);
					OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				}
			}
			break;
		
		case 1:         //run a valid cmd with a existed General
			if (mNumGenerals < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumGenerals-1);
			cmd = "";
			general = mGenerals[r1];
			cmd << "qos generalrule remove " 
				<< general.srcIp << " " 
				<< general.srcPort << " " 
				<< general.dstIp << " " 
				<< general.dstPort << " "
				<< general.protocol << " "
				<< general.interface;
			ret = checkGeneral(mGenerals[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			aosRunCli(cmd,true);
			mGenerals[r1] = mGenerals[mNumGenerals-1];
			mNumGenerals--;
			if (r1 == mNumGenerals)
			{
				r1--;
			}
			if (mNumGenerals > 0)
			{
				ret = checkGeneral(mGenerals[r1]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			//	cout << mGeneralAddrs[r1].General << endl;
			}
			else
			{
				ret = checkGeneral(mGenerals[0]);
				OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
			}

			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}

		
bool 
AosCliQosTester::runShowGeneral()
{
	OmnString cmd;
	bool ret;
	cmd << "qos generalrule show" ;
	if (mNumGenerals > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumGenerals;i++)
		{
			ret = checkGeneral(mGenerals[i]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
		}
	}
	return true;
}

bool 
AosCliQosTester::runClearGeneral()
{
	OmnString cmd;
	bool ret;
	cmd << "qos generalrule clear" ;
	if (mNumGenerals > 0)
	{
		//cout << cmd << endl;
		aosRunCli(cmd,true);
		for (int i=0;i<mNumGenerals;i++)
		{
			ret = checkGeneral(mGenerals[i]);
			OmnTC(OmnExpected<bool>(false), OmnActual<bool>(ret)) << endtc;
		}
		mNumGenerals = 0;
	}
	return true;
}

bool 
AosCliQosTester::runModifyGeneral()
{
	//
	//
	//test cli command "remove General GeneralAddr"
	//first we will decide whether to run a valible cmd or not 
	//if to run a valible cmd 
	//	we will select whether to add a new General or a exister General
	//	if not a new General
	//		cmd will return true
	//	else (it's a existed General)
	//		cmd will return true
	//else (not a valible cmd)
	//	cmd should return false
	//
	//
	
	int priority = OmnRandom::nextInt(0,2); 
	int r1Case[3] = {0,1,2};
	u16 r1Prob[3] = {30,60,10};
	AosRandomSelector r1Selector(r1Case,r1Prob, 3);
	int r1 = r1Selector.next();
	int r2;
	OmnString cmd;
	qosGeneralInfo general;
	bool ret;
	switch (r1)
	{
		case 0: 		//run a valid cmd with a new General
			if (mNumGenerals >= eMaxGeneral)
			{
				return true;
			}
			cmd = "";
			r1 = OmnRandom::nextInt(0,1);
			r2 = OmnRandom::nextInt(0,1);
			general = getGeneral();
			cmd << "qos generalrule modify " 
				<< general.srcIp << " " 
				<< general.srcPort << " " 
				<< general.dstIp << " " 
				<< general.dstPort << " "
				<< general.protocol << " "
				<< general.interface << " "
				<< general.priority;
			if(isNewGeneral(general))
			{
				aosRunCli(cmd,true);
				mGenerals[mNumGenerals] = general;
				ret = checkGeneral(mGenerals[mNumGenerals]);
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
				mNumGenerals++;
			}
			break;
		
		case 1:         //run a valid cmd with a existed General
			if (mNumGenerals < 1)
			{
				return true;
			}
			r1 = OmnRandom::nextInt(0,mNumGenerals-1);
			r2 = OmnRandom::nextInt(0,1);
			general = mGenerals[r1];
			cmd = "";
			cmd << "qos generalrule modify " 
				<< general.srcIp << " " 
				<< general.srcPort << " " 
				<< general.dstIp << " " 
				<< general.dstPort << " "
				<< general.protocol << " "
				<< general.interface << " "
				<< sgPriority[priority];
			aosRunCli(cmd,true);
			mGenerals[r1].priority = sgPriority[priority];
			ret = checkGeneral(mGenerals[r1]);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret)) << endtc;
			break;

		case 2:         //run a invalid cmd
			break;

		default:
			break;
	}
	return true;
}
