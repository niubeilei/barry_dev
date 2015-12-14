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
// 2013/02/07	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/Testers/IILStrTester.h"

#include "Rundata/Rundata.h"
#include "Tester/TestMgr.h"
#include "alarm_c/alarm.h"
#include "DbQuery/Query.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILMgrBig/IILMgr.h"
#include "IILMgrBig/IILStr.h"
#include "IILMgrBig/IIL.h"
#include "IILClient/IILClient.h"
#include "SEInterfaces/QueryObj.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "Util/BuffArray.h"
#include "Util/CompareFun.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"


AosIILStrTester::AosIILStrTester()
:
mSeedDataLen(eDftSeedDataLen),
mKeyLen(eDftKeyLen),
mRecordLen(mKeyLen + sizeof(u64)),
mBuffLen(eDftBuffLen)
{
}


bool 
AosIILStrTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	cout << "    Start OmnString Tester ..." << endl;
	config();
	basicTest();
	return true;
}


bool
AosIILStrTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("iilmgr_big_testers");
	if (!tag) return true;

	mTries = tag->getAttrU64("tries", eDftTries);
	if (mTries <= 0) mTries = eDftTries;

	return true;
}


bool 
AosIILStrTester::basicTest()
{
	return true;
}


bool
AosIILStrTester::tortureIndexing()
{
	// Indexes are in the form:
	// 		[key, docid]
	// It randomly creates an array of entries:
	// 		[key, docid]
	// 		[key, docid]
	// 		...
	// Sort them, and then add them into the IIL. 
	// This torturer assumes that both keys and docids can 
	// be duplicated. 
	//
	// It randomly generates a string, called Seed. It 
	// uses the seed to generate a set of strings.
	
	for (int i=0; i<mTries; i++)
	{
		indexOneTry();
		check();
	}
	return true;
}


bool
AosIILStrTester::indexOneTry()
{
	/*
	int num_seeds = OmnRandom::intByRange(
			1, 10, 50, 
			11, 50, 50,
			51, 500, 50);

	char data[mSeedDataLen+10];
	aos_assert_r(mRecordLen == mKeyLen + sizeof(u64), false);
	AosCompareFunPtr comp_func = OmnNew AosFunStrU642(mKeyLen+sizeof(u64));
	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(comp_func, true, false, mBuffLen);
	u64 num_entries = 0;
	for (int i=0; i<num_seeds; i++)
	{
		OmnRandom::nextLetterDigitStr(data, mSeedDataLen, true);
		data[mSeedDataLen] = 0;
		bool rslt = mSeeds->addValue(data, mSeedDataLen, mRundata);
		u32 id = mSeedVector.size();
		mSeedVector.push_back(data);

		generateEntries(buff_array, data, num_entries);

		if (num_entries > mMaxEntriesPerCall)
		{
			break;
		}
	}

	// Call the IIL function.
	buff_array->sort();
	
	// Create the action
	// 	<sdoc AOSTAG_ZKY_TYPE=AOSACTTYPE_STRIILBATCHADD>
	// 		<iil AOSTAG_IILNAME="xxx" AOSTAG_LENGTH="xxx"/>
	//	</sdoc>
	OmnString sdocstr = "<sdoc ";
	sdocstr << AOSTAG_ZKY_TYPE << "=\"" << AOSACTTYPE_STRIILBATCHADD
		<< "\"><iil " << AOSTAG_IILNAME << "=\"" << mIILName
		<< "\" " << AOSTAG_LENGTH << "=\"" << mRecordLen 
		<< "\"/></sdoc>";
	AosXmlTagPtr sdoc = AosStr2Xml(mRundata, sdocstr AosMemoryCheckerArgs);
	aos_assert_r(sdoc, false);
	aos_assert_r(num_entries == buff_array->size(), false);
	OmnScreen << "To add entries: " << num_entries << endl;

	bool rslt = AosActionObj::runAction(mRundata, sdoc, buff_array->getBuff());
	aos_assert_r(rslt, false);

	rslt = mSeeds->sort();
	aos_assert_r(rslt, false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


/*
bool
AosIILStrTester::generateEntries(
		const AosBuffArrayPtr &buff_array, 
		const u64 &seed_id,
		const char *seed, 
		u64 &num_entries)
{
	OmnString prefix;
	prefix << seed_id << "_";
	int prefix_len = prefix.length();
	char record[mKeyLen+sizeof(u64)+10];
	strncpy(record, prefix.data(), prefix_len);

	// Creating entries by substrings.
	int max_sub_len = mKeyLen - prefix_len;
	for (int substr_len = 2; substr_len<=max_sub_len; substr_len++)
	{
		int key_len = prefix_len + substr_len;
		record[key_len] = 0;
		// For each substr_len, it will loop 'mSeedDataLen' - 'substr_len'
		// number of times. Each loop will create: 
		// 		mMaxInc * substr_len
		// entries. 
		//
		// Example: if mSeedDataLen = 100, substr_len = 20, mMaxInc = 5, 
		// it will loop 100 - 10 = 90 times and it will generate:
		// 		90 * 5 * 20 = 900 entries
		for (int idx=0; idx<mSeedDataLen-substr_len; idx++)
		{
			// Obtained a substr: [idx, substr_len]
			OmnString substr(&seed[idx], substr_len);

			// 1. Create one entry using the substr
			strncpy(&record[prefix_len], &seed[idx], substr_len);
			record[key_len] = 0;
			u64 docid = AosStrHashFunc(record, key_len);
			*((u64 *)&record[mKeyLen]) = docid;
			num_entries++;
			buff_array->addValue(record, mKeyLen+sizeof(64));

			// 2. Create a substr by increasing each character 
			// For each substr, it creates mMaxInc * substr_len
			// number of entries. For example, if mMaxInc = 26 and 
			// substr_len is 20, it creates 520 entries.
			for (int inc=1, inc<mMaxInc; inc++)
			{
				// For each substr, it creates substr_len number
				// of entries.
				for (int mm=0; mm<substr_len; mm++)
				{
					char c = seed[idx+mm];
					seed[idx+mm] += inc;

					u64 docid = AosStrHashFunc(record, key_len);
					*((u64 *)&record[mKeyLen]) = docid;
					buff_array->addValue(record, mKeyLen+sizeof(64));
					num_entries++;

					seed[idx+mm] = c;
				}
			}
		}
	}

	return true;
	OmnNotImplementedYet;
	return false;
}
*/


bool
AosIILStrTester::check()
{
	/*
	// For each seed, there shall be:
	// 	dddd_xx
	// 	...
	// 	dddd_xxx
	// 	...
	// where 'dddd' is the seed id. 
	// It randomly picks a seed, retrieve the contents, and then
	// check the results. 
	
	if (mSeedVector.size() <= 0) return true;

	int tries = OmnRandomUtil::intByRange(
			5, 20, 50, 
			21, 50, 50, 
			51, 100, 20);

	for (int i=0; i<tries; i++)
	{
		u64 seed_id = rand() % mSeedVector.size();
		OmnString seed = mSeedVector[seed_id];
		AosFunStrU642Ptr comp_func = OmnNew AosFunStrU642(mKeyLen+sizeof(u64));
		AosBuffArrayPtr buff_array = OmnNew AosBuffArray(comp_func, true, false, mBuffLen);

		u64 num_entries = 0;
		bool rslt = generateEntries(buff_array, seed_id, seed.data(), num_entries);
		aos_assert_r(rslt, false);

		AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
		aos_assert_r(iilmgr, false);

		int64_t start_idx = 0;
		AosValueRslt value1;
		AosValueRslt value2;
		AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
		aos_assert_r(query_rslt, false);
		int psize;
		while (1)
		{
			rslt = AosQueryObj::getQueryObj()->runQuery(
					start_idx, psize, mIILName, value1, value2, eAosOpr_range_ge_lt, 
					query_rslt, false, true, mRundata);
			aos_assert_r(rslt, false);

			// Check the result.
		}
	}

	return true;
	*/
	OmnNotImplementedYet;
	return false;
}
