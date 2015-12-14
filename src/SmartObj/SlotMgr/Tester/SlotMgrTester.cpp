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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/SlotMgr/Tester/SlotMgrTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "ErrorMgr/ErrmsgId.h"
#include "SmartObj/SlotMgr/SobjSlotMgr.h"
#include "UtilHash/HashedObjU64.h"
#include "UtilHash/HashedObj.h"
#include "UtilHash/StrObjHash.h"
#include "RandomUtil/RandomUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"



AosSlotMgrTester::AosSlotMgrTester()
{
	bool rslt = init();
	if(!rslt)
	{
		OmnAlarm << "Faild to Construct AosSlotMgrTester" << enderr;
		exit(0);
	}
}


bool
AosSlotMgrTester::init()
{
	OmnString docstr = "<doc ";
	docstr << AOSTAG_OBJID << "=\"test123\" "
		<< AOSTAG_PARENTC << "=\"ctnr123\" "
		<< AOSTAG_MIN_START_FLAG << "=\"true\" "
		<< AOSTAG_MIN_START << "=\"10\" "
		<< AOSTAG_MAX_START_FLAG << "=\"true\" "
		<< AOSTAG_MAX_START << "=\"1000000000\" "
		<< AOSTAG_MIN_SIZE_FLAG << "=\"true\" "
		<< AOSTAG_MIN_SIZE << "=\"20\" "
		<< AOSTAG_MAX_SIZE_FLAG << "=\"true\" "
		<< AOSTAG_MAX_SIZE << "=\"100000\" "
		<< AOSTAG_LOG_ERRORS << "=\"false\" "
		<< AOSTAG_INVALID_START << "=\"1\" "
		<< AOSTAG_INVALID_SIZE << "=\"2\" "
		<< AOSTAG_LOG_OPERATIONS << "=\"false\" "
		<< AOSTAG_LOG_OPERATIONS << "=\"false\"/>";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	mRundata = OmnApp::getRundata();
	mSobjSlotMgr = OmnNew AosSobjSlotMgr(doc, mRundata);

	return true;
}


bool
AosSlotMgrTester::start()
{
	return basicTest();
}


bool
AosSlotMgrTester::basicTest()
{
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries:" << tries << endl;
	if (tries <= 0) tries = eDefaultTries;

	for(int i = 1; i <= tries; i++)
	{
	}
	return true;
}


