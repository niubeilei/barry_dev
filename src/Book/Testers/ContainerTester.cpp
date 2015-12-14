////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/04/2009:  by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Book/Testers/ContainerTester.h"

#include "Alarm/Alarm.h"
#include "Book/BookMgr.h"
#include "Book/Container.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

static AosBookMgr sgBookMgr;

bool 
AosContainerTester::start()
{
	cout << "    Start Container Tester ...";
	u32 tries = 1;
	basicTest(tries);
	return true;
}

bool 
AosContainerTester::basicTest(u32)
{
	// Create a container 
	AosContainer container;
	AosXmlRc errcode;
	OmnString errmsg;
	bool rslt = container.createContainer("cname", 
		"tag1|$|tag2|$|tag3", 
		"This is a test",
		"Chen Ding",
		errcode, errmsg);

	OmnCreateTc << (rslt) << endtc;
	return true;
}
