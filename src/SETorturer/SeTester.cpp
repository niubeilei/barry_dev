////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class runs as a separate thread. It randomly sleeps for a 
// a period of time. When it wakes up, it creates a query and query
// the system.
//
// Modification History:
// 08/28/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SETorturer/SeTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SearchEngine/Ptrs.h"
#include "SETorturer/UserGroupTester.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeXmlParser.h"
#include "SEClient/SqlClient.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "WordProc/WordFilter.h"
#include "WordProc/WordNorm.h"


AosSeTester::AosSeTester()
{
	basicTest();
}


AosSeTester::~AosSeTester()
{
}


bool
AosSeTester::basicTest()
{
	/*
	const int tries = 1;
	for (int i=0; i<tries; i++)
	{
		OperationType type = getNextOperation();
		AosSeCompTesterPtr tester = mTesters[type];
		bool rslt = tester->doOneTest();
		aos_assert_r(rslt, false);
	}
	*/

	AosUserGroupTester grouptester;
	bool rslt = grouptester.start();
	aos_assert_r(rslt, false);
	return true;
}

