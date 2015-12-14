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
//
//
//
// Modification History:
// 2011/01/15	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "HtmlServer/Testers/HtmlServerTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "HtmlServer/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"

#define PERCENT(n)   (rand()%100 < n)

extern int gStartDocid;

AosHtmlServerTester::AosHtmlServerTester()
{
	mName = "HtmlServerTester";
}


bool AosHtmlServerTester::start()
{
	cout << "    Start OmnString Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosHtmlServerTester::basicTest()
{
	//gStartDocid = 139354;
	//gStartDocid = 148955;
	//gStartDocid = 201940;
	//gStartDocid = 202840;
	//gStartDocid = 204556;
	//gStartDocid = 206619;
	//AosSengAdmin::getSelf()->checkHtmlGenerate("100", gStartDocid, 3000000);
	//AosSengAdmin::getSelf()->checkHtmlGenerate("100", 332206, 332207);
	//AosSengAdmin::getSelf()->checkHtmlGenerate("100", 325959, 325960);
	return true;
}

