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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "EmailClt/Client/EmailClientTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "EmailClt/EmailClient.h"
#include "EmailClt/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

#include <deque>
#include <vector>


AosEmailClientTester::AosEmailClientTester(const OmnString &idx)
:
mEmailClient(OmnNew AosEmailClient()),
mIndex(idx)
{
	mName = "EmailClientTester";
}


bool 
AosEmailClientTester::start()
{
	cout << "Start AosEmailClient Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosEmailClientTester::basicTest()
{
	AosXmlTagPtr config = OmnApp::getAppConfig()->getFirstChild("transaction");
	aos_assert_r(config, false);
	mEmailClient->start(config);

	OmnString log_str;
	AosXmlTagPtr log;
	

	int tries = 1;
	for (int i=0; i<tries; i++)
	{
		log_str = "";
		log_str << "<operate senderpasswd = \"4479@brian\" " 
				<< "rmailaddr=\"zhyawshhz@163.com\" "
				<< "idx = \"" << mIndex << "\">"
				<< "</operate>";
	OmnScreen << log_str << endl;
		AosXmlParser parser;
		log = parser.parse(log_str, "");
		aos_assert_r(log, false);
		AosRundataPtr rdata = OmnNew AosRundata();
		mEmailClient->receiveEmail(log,60,rdata);
	}

	return true;
}

