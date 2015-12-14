////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosBridgeTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_coreTester/aosBridgeTester.h"

#include "aos/aosBridge.h"

#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"



bool OmnAosBridgeTester::start()
{
	// 
	// Test default constructor
	//
	aosBridge_init();
	testCli();
	return true;
}


bool OmnAosBridgeTester::testCli()
{
	OmnString rslt;
	bool rt;

	OmnBeginTest << "Test Bridge CLI";
	mTcNameRoot = "Bridge_CLI";

	OmnString cmd = "show bridge";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

	cmd = "bridge add 10 wrr eth0 10 eth1 20";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

	cmd = "show bridge 10";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

	cmd = "show bridge";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

	cmd = "remove bridge 10";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) 
		<< cmd << ". " << rslt << endtc;

	// 
	// Add and remove a bridge 10101 times to make sure there is no 
	// memory leak.
	//
	for (int i=0; i<1010101; i++)
	{
		cmd = "bridge add 10 wrr eth0 10 eth1 20";
		rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
		// cout << rslt << endl;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

		cmd = "bridge device add 10 left eth2 101";
		rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
		// cout << rslt << endl;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) 
			<< cmd << ". " << rslt << endtc;

		cmd = "bridge device add 10 right eth3 200";
		rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
		// cout << rslt << endl;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) 
			<< cmd << ". " << rslt << endtc;

		cmd = "remove bridge 10";
		rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
		// cout << rslt << endl;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) 
			<< cmd << ". " << rslt << endtc;
	}

	return true;
}



