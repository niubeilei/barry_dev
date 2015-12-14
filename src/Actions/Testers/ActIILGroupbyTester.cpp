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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/Testers/ActIILGroupbyTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "IILClient/Ptrs.h"
#include "IILClient/IILTransAssembler.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosActIILGroupbyTester::AosActIILGroupbyTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActIILGroupbyTester";
}


bool 
AosActIILGroupbyTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActIILGroupbyTester::basicTest()
{
	// Phase 1:
	// It creates two IILs:
	// 1. IIL1 [upn_cpn, numcalls]
	// 2. IIL2 [upn_prefix, ucitycode+utowncode]
	// Some phone numbers have matched prefix in IIL2 and some do not. 
	//
	// The joined results are an IIL:
	// 		IIL3: [cpn_upn_ucitycode_utowncode, numcalls]
	//
	// Phase 2:
	// 1. Retrieve IIL3
	// 2. Create IIL4: [cpn_prefix, ccitycode]
	// 3. Join LL3 and IIL4, generate the following IIL:
	// 		IIL5: [cpn, utowncode]
	//
	while (1)
	{
		// Phase 1
		resetData();
		createIIL1();
		createIIL2();
		runPhase1Action();
		verifyPhase1Results();

		// Phase 2
		createIIL4();
		runPhase2Action();
		verifyPhase2Results();
	}
	return true;
}


bool
AosActIILGroupbyTester::resetData()
{
	mSums.clear();
	mUPNPrefixMap.clear();
	mCPNPrefixMap.clear();
	mCallsMap.clear();
	mCPNTownMap.clear();

	// Create Assemblers
	// 	<config ...>	
	// 		<testers ...>
	// 			<sdoc ...>
	// 				<iil_assemblers>
	// 					<iil_assembler .../>
	// 					<iil_assembler .../>
	// 					...
	// 				</iil_assemblers>
	// 				...
	// 			</sdoc>
	// 			...
	// 		</testers>
	// 		...
	// 	</config>
	AosXmlTagPtr appconf = OmnApp::getAppConfig();
	aos_assert_r(appconf, false);
	AosXmlTagPtr conf = appconf->getFirstChild("testers");
	aos_assert_r(conf, false);
	AosXmlTagPtr test_tag = conf->getFirstChild("sdoc");
	aos_assert_r(test_tag, false);
	AosXmlTagPtr tag = test_tag->getFirstChild("iil_assemblers");
	aos_assert_r(tag, false);
	mTask = OmnNew AosTaskAction(false);

	// Create mAssembler1
	AosXmlTagPtr entry = tag->getFirstChild();
	aos_assert_r(entry, false);
	mAssembler1 = AosIILTransAssembler::createIILAssembler(entry, mRundata);

	// Create mAssembler2
	entry = tag->getNextChild();
	aos_assert_r(entry, false);
	mAssembler2 = AosIILTransAssembler::createIILAssembler(entry, mRundata);

	// Create mAssembler4
	entry = tag->getNextChild();
	aos_assert_r(entry, false);
	mAssembler4 = AosIILTransAssembler::createIILAssembler(entry, mRundata);

	mRundata->setSiteid(100);

	AosXmlTagPtr actions = test_tag->getFirstChild("actions");
	aos_assert_r(actions, false);

	// Create Action1
	mAction1Tag = actions->getFirstChild();
	aos_assert_r(mAction1Tag, false);
	mAction1 = AosSdocAction::createAction(mAction1Tag, mRundata);
	aos_assert_r(mAction1, false);
	mAction1->setTask(mTask);

	// Create Action2
	mAction2Tag = actions->getNextChild();
	aos_assert_r(mAction2Tag, false);
	mAction2 = AosSdocAction::createAction(mAction2Tag, mRundata);
	aos_assert_r(mAction2, false);
	mAction2->setTask(mTask);

	return true;
}


bool
AosActIILGroupbyTester::createIIL1()
{
	// IIL is:
	// 		[upn_cpn, numcalls]
	// When generating this IIL, we will create two maps:
	// 	mCPNPrefixMap
	// 	mUPNPrefixMap
	//
	// For each call: [cpn_upn, numcalls], it 
	int num_entries = rand() % 10;
	for (int i=0; i<num_entries; i++)
	{
		OmnString upn = OmnRandom::digitStr(11, 11);
		mUPNs.push_back(upn);
		OmnString upn_prefix = upn;
		upn_prefix.setLength(7);
		int num_repeats = rand() % 20;
		u64 upn_location = 0;
		u32 citycode = 0;
		u32 towncode = 0;
		for (int k=0; k<num_repeats; k++)
		{
			OmnString key = upn;
			OmnString cpn = OmnRandom::digitStr(11, 11);
			u32 numcalls = rand() % 100;
			key << "|$|" << cpn;

			// A call statistics is generated:
			// 		[upn, cpn, numcalls]
			// Check whether it is already in the map. If yes, do not 
			// create it anymore.
			OmnString reverse_key = cpn;
			reverse_key << "|$|" << upn;
			AosStr2U64Itr_t callitr = mUPNPrefixMap.find(upn_prefix);
			if (callitr == mUPNPrefixMap.end())
			{
				// The location has not been determined for the prefix yet.
				// It is not there yet. Create it.
				citycode = pickUCitycode();
				towncode = rand();
				upn_location = ((u64)citycode << 32) + towncode;
				mUPNPrefixMap[upn_prefix] = upn_location;
			}

			mAssembler1->appendEntry(key, numcalls, mRundata);

			// We create a statistics:
			// 		[cpn, upn, numcalls]
			// We need to generate two prefix maps. Each map maps a prefix
			// to citycode + towncode. For cpn, the towncode is always 0.
			// In order to simulate the real situations, we will randomly
			// set cpn.citycode = upn.citycode.
			int using_same_city = rand() % 4;

			// Need to set their citycode the same.
			// Now it constructed an entry:
			// 		[upn, cpn, upn_location(citycode, towncode)]
			// Add the entry to mCallLocationsMap.
			mCallsMap[reverse_key] = numcalls;

			OmnString cpn_prefix = cpn;
			cpn_prefix.setLength(7);

			// Check whether the entry already exists.
			AosStr2U64Itr_t itr = mCPNPrefixMap.find(cpn_prefix);
			if (itr == mCPNPrefixMap.end())
			{
				// It is not there yet.
				if (using_same_city)
				{
					// Need to generate the same citycode
					u64 cpn_location = (((u64)citycode) << 32);
					mCPNPrefixMap[cpn_prefix] = cpn_location;
				}
				else
				{
					// Need to randomly generate a city code.
					u32 cc = pickCCitycode();
					cc = ((u64)cc << 32);
					mCPNPrefixMap[cpn_prefix] = cc; 
				}
			}
		}
	}

	mAssembler1->sendSafe(true, mRundata);
	return true;
}


bool
AosActIILGroupbyTester::createIIL2()
{
	for (u32 i=0; i<mUPNs.size(); i++)
	{
		OmnString upn = mUPNs[i];
		upn.setLength(7);
		AosStr2U64Itr_t itr = mUPNPrefixMap.find(upn);
		if (itr == mUPNPrefixMap.end())
		{
			u32 vv = rand() % 1000;
			mAssembler2->appendEntry(upn, vv, mRundata);
		}
	}

	mAssembler2->sendSafe(true, mRundata);
	return true;
}


bool
AosActIILGroupbyTester::runPhase1Action()
{
	return mAction1->run(mAction1Tag, mRundata);
}


bool
AosActIILGroupbyTester::verifyPhase1Results()
{
	// Phase 1 will create an IIL:
	// 		IIL3: [cpn_upn_ucitycode_utowncode, numcalls]
	// It checks the following:
	// 1. ucitycode and utowncode matches the ones defined by upn prefix.
	// 2. The number of calls matches the one recorded in mCallsMap.
	
	AosQueryRsltPtr query_rslt = AosQueryRslt::getQueryRslt();
	AosQueryContextPtr query_context = AosQueryContext::getQueryContext();
	AosQueryColumn(mIILName3, query_rslt, 0, query_context, mRundata);

	bool finished;
	u64 docid; 
	OmnString value;
	vector<OmnString> values;
	while (1)
	{
		query_rslt->nextDocidValue(docid, value, finished, mRundata);
		if (finished) return true;

		// 'value' is in the form:
		// 		cpn|$|upn|$|ucitycode|$|utowncode
		AosStrSplit::splitStrByChar(value.data(), "|$|", values, 4);

		// Verify City and Town
		OmnString key = values[1];
		key.setLength(7);
		AosStr2U64Itr_t itr = mUPNPrefixMap.find(key);
		aos_assert_r(itr != mUPNPrefixMap.end(), false);
		u64 location = itr->second;
		u32 city1 = location >> 32;
		u32 town1 = (u32)location;

		u32 city2 = atoll(values[2].data());
		u32 town2 = atoll(values[3].data());
		aos_assert_r(city1 == city2, false);
		aos_assert_r(town1 == town2, false);

		// Verify numcalls
		key = values[0];
		key << "|$|" << values[1];
		AosStr2U32Itr_t itr2 = mCallsMap.find(key);
		aos_assert_r(itr2 != mCallsMap.end(), false);
		aos_assert_r(itr2->second == docid, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosActIILGroupbyTester::createIIL4()
{
	// IIL4 is:
	// 		[cpn_prefix, ccitycode]
	AosStr2U64Itr_t itr = mCPNPrefixMap.begin();
	for (AosStr2U64Itr_t itr = mCPNPrefixMap.begin(); itr != mCPNPrefixMap.end(); itr++)
	{
		mAssembler4->appendEntry(itr->first, itr->second, mRundata);
	}
	mAssembler4->sendSafe(true, mRundata);
	return true;
}


bool
AosActIILGroupbyTester::runPhase2Action()
{
	return mAction2->run(mAction2Tag, mRundata);
}


bool
AosActIILGroupbyTester::verifyPhase2Results()
{
	// Phase 1 will create an IIL:
	// 		IIL5: [cpn, utowncode]
	createCPNTowncode();

	AosQueryRsltPtr query_rslt = AosQueryRslt::getQueryRslt();
	AosQueryContextPtr query_context = AosQueryContext::getQueryContext();
	AosQueryColumn(mIILName3, query_rslt, 0, query_context, mRundata);

	bool finished;
	u64 docid; 
	OmnString value;
	vector<OmnString> values;
	while (1)
	{
		query_rslt->nextDocidValue(docid, value, finished, mRundata);
		if (finished) 
		{
			aos_assert_r(mCPNTownMap.size() == 0, false);
			return true;
		}

		// 'value' is in the form:
		// 		[cpn, utowncode]
		// Note that not all cpns have mapped towncode. 
		AosStr2U32Itr_t itr = mCPNTownMap.find(value);
		if (itr != mCPNTownMap.end())
		{
			// There is an entry in mCPNTownMap. Its towncode must be the 
			// same as docid.
			aos_assert_r(docid == itr->second, false);
			mCPNTownMap.erase(value);
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosActIILGroupbyTester::createCPNTowncode()
{
	// For each call in mCallsMap:
	// 	[cpn, upn, numcalls]
	// It looks up the ccitycode from mCPNPrefixMap and ucitycode and utowncode
	// from mUPNPrefixMap. If their citycodes do not match, ignore it. Otherwise,
	// it checks whether the entry [cpn, numcalls, towncode] in mCPNMaxCallsMap 
	// exists. If not, create it; otherwise, it checks whether the number of
	// calls is bigger than 'numcalls'. If yes, find the utowncode using
	// the prefix and update the entry.
	vector<OmnString> values;
	AosStr2U32_t max_call_map;
	mCPNTownMap.clear();	
	for (AosStr2U32Itr_t itr = mCallsMap.begin(); itr != mCallsMap.end(); itr++)
	{
		OmnString cpn_upn = itr->first;
		u32 numcalls = itr->second;
		AosStrSplit::splitStrByChar(cpn_upn.data(), "|$|", values, 2);
		OmnString cpn = values[0];
		OmnString upn = values[1];
		OmnString cpn_prefix = cpn;
		cpn_prefix.setLength(7);
		OmnString upn_prefix = upn;
		upn_prefix.setLength(7);

		// Retrieve ccitycode
		AosStr2U64Itr_t itr1 = mCPNPrefixMap.find(cpn_prefix);
		aos_assert_r(itr1 != mCPNPrefixMap.end(), false);
		u32 ccitycode = itr1->second >> 32;

		// Retrieve ucitycode and utowncode
		AosStr2U64Itr_t itr2 = mUPNPrefixMap.find(upn_prefix);
		aos_assert_r(itr2 != mUPNPrefixMap.end(), false);
		u64 location = itr2->second;
		u32 ucitycode = (u32)(location >> 32);
		u32 utowncode = (u32)location;

		// Check whether their citycodes are the same
		if (ucitycode != ccitycode) continue;

		// They are the same. Check the max calls.
		AosStr2U32Itr_t itr3 = max_call_map.find(values[0]);
		if (itr3 == max_call_map.end())
		{
			// It is not there yet, add it. 
			max_call_map[cpn] = numcalls;
			mCPNTownMap[cpn] = utowncode;
		}
		else
		{
			if (itr3->second < numcalls)
			{
				// Need to update
				max_call_map[cpn] = numcalls;
				mCPNTownMap[cpn] = utowncode;
			}
		}
	}
	return true;
}


u32 
AosActIILGroupbyTester::pickUCitycode()
{
	// All uciticode starts in range [1000]
	u32 code = rand() % 1000;
	return code + 1000;
}


u32 
AosActIILGroupbyTester::pickCCitycode()
{
	// All uciticode starts in range [2000]
	u32 code = rand() % 1000;
	return code + 2000;
}

#endif
