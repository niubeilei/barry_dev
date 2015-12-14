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
#include "RandomGen/Tester/RandomGenCellPhoneTester.h"

#include "Util/OmnNew.h"
#include "Random/Ptrs.h"
#include "Random/RandomSelStr.h"
#include "SEInterfaces/RandomGenObj.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosRandomGenCellPhoneTester::AosRandomGenCellPhoneTester()
{
}


bool AosRandomGenCellPhoneTester::start()
{
	u32 tries = 100;
	basicTest1(tries);
//	basicTest2(tries);
	return true;
}


bool
AosRandomGenCellPhoneTester::basicTest1(u32 tries)
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

		AosJimoPtr jimo = AosCreateJimoByClassname(rdata.getPtr(), "AosRandomGenCellPhone", 1);
		aos_assert_r(jimo->getJimoType() == AosJimoType::eRandomGen, false);
		mRandomStr = dynamic_cast<AosRandomGenObj *>(jimo.getPtr());
		aos_assert_r(mRandomStr, false);

		bool rslt = mRandomStr->config(rdata.getPtr(), conf);
		aos_assert_r(rslt, false);

		rslt = mRandomStr->generateData(rdata.getPtr(), vv);
		aos_assert_r(rslt, false);

		int len;
		const char *dd = vv.getCharStr(len);
		OmnString sss(dd, len);
		OmnScreen << "Cellphone number: " << sss << endl;
	}

    return true;

}


bool
AosRandomGenCellPhoneTester::basicTest2(const u32 tries)
{
    return true;
}


