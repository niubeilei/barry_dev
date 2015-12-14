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
#include "SETorturer/UserGroupTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SearchEngine/Ptrs.h"
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


AosUserGroupTester::AosUserGroupTester()
{
}


AosUserGroupTester::~AosUserGroupTester()
{
}


bool
AosUserGroupTester::start()
{
	return basicTest();
}


bool
AosUserGroupTester::basicTest()
{
	const int tries = 10;
	for (int i=0; i<tries; i++)
	{
		int vv = rand() % 100;
		if (vv < eCreateWeight)
		{
			if (!createGroup()) return false;		
		}
		else if (vv < eDeleteWeight)
		{
			if (!deleteGroup()) return false;		
		}
		else if (vv < eAddMemberWeight)
		{
			if (!addMember()) return false;		
		}
		else if (vv < eDeleteMemberWeight)
		{
			if (!delMember()) return false;		
		}
		else 
		{
			if (!queryMember()) return false;		
		}
	}
	return true;
}


bool
AosUserGroupTester::createGroup()
{
	// This function creates a new group. Creating a group
	// needs to determine what members to include in the 
	// group. 
	
}

