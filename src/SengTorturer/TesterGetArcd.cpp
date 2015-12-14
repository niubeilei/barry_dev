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
// Modification History:
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterGetArcd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <stdlib.h>

/*
AosGetArcdTester::AosGetArcdTester(
		const OmnString &weight_tagname, 
		const bool regflag)
:
AosSengTester(weight_tagname, AosTesterId::eGetArcd, regflag)
{
}


AosGetArcdTester::~AosGetArcdTester()
{
}


bool 
AosGetArcdTester::test()
{
	// This function tests the request eGetAccessRecord
	// int tid = thread->getThreadId();
	// OmnString siteid = thread->getSiteid();
	OmnNotImplementedYet;
	return false;
}

*/
