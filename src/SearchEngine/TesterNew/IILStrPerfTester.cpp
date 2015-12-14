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
// 2011/01/31	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/TesterNew/IILStrPerfTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IIL.h"
#include "IILClient/IILClient.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "RandomUtil/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"

#define PERCENT(n)   (rand()%100 < n)

AosIILStrPerfTester::AosIILStrPerfTester()
{
	mName = "DocServerTester";
}


bool AosIILStrPerfTester::start()
{
	cout << "    Start OmnString Tester ..." << endl;
	basicTest();
	return true;
}


bool AosIILStrPerfTester::basicTest()
{
	mRunData =  OmnApp::getRundata();
	// randomValue();
	sameValue();
	return true;
}


bool
AosIILStrPerfTester::randomValue()
{
	// This function creates an IIL and randomly add a given number of 
	// values. 
	int tries = mTestMgr->getTries();
	if (tries < 1) tries = 1;
//	const OmnString iilname = "RandomValueIIL";
//	AosIILPtr iil = AosIILMgr::getSelf()->retrieveIIL(
//				iilname.data(), 
//				iilname.length(), 
//				eAosIILType_Str, 
//				true);
	if(mIIL.isNull())
	{
		AosIILType iiltype = eAosIILType_Str;
		mIILID = AosIILMgr::getSelf()->createIILPublic(
					iiltype,
					false,
					mRunData);
		AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);
		mIIL = (AosIILStr*)iil.getPtr();
		aos_assert_r(mIIL, false);
		aos_assert_r(mIIL->getIILType() == eAosIILType_Str, false);
	}

	const int max_value_len = 80;
	char buff[max_value_len+1];
	for (int i=0; i<tries; i++)
	{
		int guard = 50;
		while (guard--)
		{
			AosRandWord(buff, max_value_len, 1, max_value_len);
			OmnString word(buff);
			if (word != "")
			{
				u64 docid = (u64)rand();
				u64 t1 = OmnGetTimestamp();
				aos_assert_r(
						AosIILMgr::getSelf()->addValueDoc(
							mIILID, 
							word, 
							docid, 
							false, 
							false,
							mRunData), false);
				u64 t2 = OmnGetTimestamp();
				if (i % 10000 == 0) cout << i << ":" << t2 - t1 << endl;
				if (t2 - t1 > 100000) cout << "Very long, i=" << i << ", time=" << t2 - t1 
					<< ", word=" << word << ", docid=" << docid << endl;
				break;
			}
		}
		aos_assert_r(guard, false);
	}

	return true;
}


bool
AosIILStrPerfTester::sameValue()
{
	// This function creates an IIL and add a given number of 
	// entries. It tortures the case of adding the same value.  
	int tries = mTestMgr->getTries();
	if (tries < 1) tries = 1;

//	const OmnString iilname = "Value1IIL";
//	AosIILPtr iil = AosIILMgr::getSelf()->retrieveIIL(iilname.data(), 
//			iilname.length(), eAosIILType_Str, true);
//	aos_assert_r(iil, false);
//	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
//	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
	if(mIIL2.isNull())
	{
		AosIILType iiltype = eAosIILType_Str;
		mIILID2 = AosIILMgr::getSelf()->createIILPublic(
					iiltype,
					false,
					mRunData);
		AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID2,iiltype,mRunData);
		mIIL2 = (AosIILStr*)iil.getPtr();
		aos_assert_r(mIIL2, false);
		aos_assert_r(mIIL2->getIILType() == eAosIILType_Str, false);
	}

	const int max_value_len = 80;
	char buff[max_value_len+1];
	AosRandWord(buff, max_value_len, 1, max_value_len);
	OmnString word(buff);
	for (int i=0; i<tries; i++)
	{
		u64 t1 = OmnGetTimestamp();
		aos_assert_r(AosIILMgr::getSelf()->addValueDoc(
					mIILID2,
					word, 
					(u64)rand(), 
					false, 
					false,
					mRunData), false);
		u64 t2 = OmnGetTimestamp();
		cout << i << ":" << t2 - t1 << ":" << mIIL2->getNumSubiils() << endl;
	}

	return true;
}

