////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//
// Modification History:
// 11/27/2006      Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/Tester/RandomGenIntTester.h"

#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "Random/RandomSelStr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosRandomGenIntTester::AosRandomGenIntTester()
{
}


bool AosRandomGenIntTester::start()
{
	u32 tries = 100;
	basicTest1(tries);
//	basicTest2(tries);
	return true;
}


bool
AosRandomGenIntTester::basicTest1(u32 tries)
{
	while (tries--)
	{
		AosValueRslt vv; 
		AosXmlTagPtr conf = OmnApp::getAppConfig(); 
		aos_assert_r(conf, false);
		conf = conf->getFirstChild(true);
		aos_assert_r(conf, false);

		AosRundataPtr rdata = OmnNew AosRundata();
		aos_assert_r(rdata, false);

		//mRandomInt = OmnNew AosRandomGenInt(1);
		AosJimoPtr jimo = AosCreateJimoByClassname(rdata.getPtr(), "AosRandomGenInt", 1);
		aos_assert_r(jimo->getJimoType() == AosJimoType::eRandomGen, false);
		mRandomInt = dynamic_cast<AosRandomGenObj *>(jimo.getPtr());
		aos_assert_r(mRandomInt, false);
		bool rslt = mRandomInt->config(rdata.getPtr(), conf);
		aos_assert_r(rslt, false);

		rslt = mRandomInt->generateData(rdata.getPtr(), vv);
		aos_assert_r(rslt, false);

		int len;
		int key= vv.getIntValue(rdata.getPtr());
		//OmnString sss(dd, len);
		OmnScreen << "int: " <<key << endl;
	}

    return true;

}


bool
AosRandomGenIntTester::basicTest2(const u32 tries)
{
    return true;
}


