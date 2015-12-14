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
// Modification History:
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgClt/Tester/ShortMsgCltTester.h"

#include "AppMgr/App.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
// #include "Valueset/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "SEUtil/ValueDefs.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "XmlUtil/SeXmlParser.h"

static u32				 		sgTimer = 30;
static int				 		sgMax = 5;
static int				 		sgMesLen = 5;
const OmnString                sgTel[] = { 
											"15606201591",
											"13862399545",
											"15862309255",
											"15062319907",
											"15250026973"
										  };

AosShortMsgCltTester::AosShortMsgCltTester()
:
mShmClt(OmnNew AosShortMsgClt())
{
}


bool AosShortMsgCltTester::start()
{
	cout << "   Start AosShortMsgCltTester ..." << endl;
	basicTest();
	return true;
}


bool AosShortMsgCltTester::basicTest()
{
	// This function get which tortuer to do
	
	AosXmlTagPtr config_tag = OmnApp::getAppConfig();
	AosXmlTagPtr tortuer_tag;
	if(!config_tag || !(tortuer_tag = config_tag->getFirstChild(AOSTAG_TESTER)))
	{
		OmnAlarm << "Missing tortuer tag!" << enderr;
		return false;
	}
	OmnString tortuer = tortuer_tag->getAttrStr(AOSATTR_TORTUER);
	aos_assert_r(tortuer != "", false);

	int tries = mTestMgr->getTries();
	OmnScreen << "Tries: " << tries << endl;
	if (tries <= 0) tries = 1;
	for (int i=0; i<tries; i++)
	{
		aos_assert_r(sendShortMsg(tortuer), false);
	}
	return true;
}


bool
AosShortMsgCltTester::sendShortMsg(const OmnString &tortuer)
{
	// This function send a message.
	OmnString receiver = "15606201591";//"18662244085";
	aos_assert_r(receiver != "", false);
OmnScreen << "********" << receiver << " ******** " << endl;

	OmnString message = "12345";//getMessage();
	aos_assert_r(message != "",	false);
OmnScreen << "********" << message << " ******** " << endl;
	
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	aos_assert_r(rdata,	false);

	AosXmlTagPtr resp;
	
	if (tortuer == "shm_confirm") 
	{
		return mShmClt->confirmThrShortmsg(receiver,message, 
				"12345", "sdoc_shmconfirm", rdata);
	}
	if(tortuer == "shm_sendnoresp")
	{
		return mShmClt->sendShortMsg(receiver, message, rdata);
	}
	if(tortuer == "shm_sendandresp")
	{
		return mShmClt->sendShortMsg(receiver, message, resp, sgTimer, rdata);
	}
	return true;
}


OmnString 
AosShortMsgCltTester::getReceiver()
{
	// This function return a receiever of string.
	// It may be one receiver, or it will be not 
	// only one receiver. The string is splited by ",".
	
	OmnString receiver = "";
	int nn = random()%sgMax;
	for(int i=0;i<nn;i++)
	{
		int mm = random()%(sgMax-1);
		receiver << sgTel[mm] << ",";
	}
	int len = receiver.length();
	const char *p = receiver.data();
	OmnString rev(p,len-1);
	return rev; 
}


OmnString 
AosShortMsgCltTester::getMessage()
{
	//	This function retrieve a contents of a message
	int len = random()%sgMesLen;
	char message[len];
	AosRandomLetterStr(len,message);
	return OmnString(message);

}
