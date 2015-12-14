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
// 11/11/2012:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/HashMapTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Util/HashMap.h"
#include "XmlUtil/XmlTag.h"
#include <map>


AosHashMapTester::AosHashMapTester() 
{
	mLock = OmnNew OmnMutex();
	mNumThreads = 0;
	mNumThreadsRunning = 0;
	mNumEntries = 0;
	mValues = OmnNew u64[eNumEntries];
	mStatus = OmnNew bool[eNumEntries];
	mName = "AosHashMapTester";
}


bool AosHashMapTester::start()
{
	// Test default constructor
	cout << "    Start AosHashMap Tester..." << endl;
	testPerformance1();
	// testMapPerformance();
	// basic();
	return true;
}


bool 
AosHashMapTester::basic()
{
	OmnBeginTest << "Test";

	// testIterator();
	int rounds = 1;
	for (int i=0; i<rounds; i++)
	{
		aos_assert_r(testOneRound(), false);
	}
	return true;
}


bool
AosHashMapTester::testIterator()
{
	/*
	mHashMap[100] = OmnNew AosTmpTest(100);
	mHashMap[200] = OmnNew AosTmpTest(200);
	mHashMap[300] = OmnNew AosTmpTest(300);
	mHashMap[400] = OmnNew AosTmpTest(400);
	mHashMap[500] = OmnNew AosTmpTest(500);
	mapitr_t itr = mHashMap.begin();
	aos_assert_r(itr != mHashMap.end(), false);
	aos_assert_r(itr->first == 100, false);
	aos_assert_r(itr->second->mData == 100, false);

	itr++;
	aos_assert_r(itr != mHashMap.end(), false);
	aos_assert_r(itr->first == 200, false);
	aos_assert_r(itr->second->mData == 200, false);

	itr++;
	aos_assert_r(itr != mHashMap.end(), false);
	aos_assert_r(itr->first == 300, false);
	aos_assert_r(itr->second->mData == 300, false);

	itr++;
	aos_assert_r(itr != mHashMap.end(), false);
	aos_assert_r(itr->first == 400, false);
	aos_assert_r(itr->second->mData == 400, false);

	itr++;
	aos_assert_r(itr != mHashMap.end(), false);
	aos_assert_r(itr->first == 500, false);
	aos_assert_r(itr->second->mData == 500, false);

	itr++;
	aos_assert_r(itr == mHashMap.end(), false);
	*/
	return true;
}


bool 
AosHashMapTester::testOneRound()
{
	int mNumThreads = OmnRandom::intByRange(1, 5, 50, 6, 10, 100, 11, 20, 50, 21, 50, 50);

	for (int i=0; i<eNumEntries; i++)
	{
		mValues[i] = OmnRandom::nextU64();
		mStatus[i] = false;
	}

	OmnThreadedObjPtr thisptr(this, false);
	mNumThreadsRunning = mNumThreads;
	for (int i=0; i<mNumThreads; i++)
	{
		mThreads[i] = OmnNew OmnThread(thisptr, "hashmaptester", i, false, false, __FILE__, __LINE__);
		mThreads[i]->start();
	}

	while (mNumThreadsRunning)
	{
		OmnScreen << "Num Threads Running: " << mNumThreadsRunning << endl;
		OmnSleep(1);
	}

	return true;
}


bool    
AosHashMapTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// It randomly generate an entry. 
	u64 tries = OmnRandom::nextU64() % 1000000;
	mapitr_t itr;
	for (u64 i=0; i<tries; i++)
	{
		if ((i % 100000) == 0)
		{
			OmnScreen << "Thread: " << thread->getLogicId() << ", trying: " << i << ":" << tries << endl;
		}
		u64 key = OmnRandom::nextU64() % eNumEntries;
		int opr = OmnRandom::nextInt() % 3;
		switch (opr)
		{
		case 0:
		case 1:
			 // Pick an entry. If it is not there, add it. If it is there, 
			 // verify it.
			 mLock->lock();
			 itr = mHashMap.find(key);
			 if (mStatus[key])
			 {
		 		// The entry is already in the map. 
	 			u64 value = mValues[key];
		 		mLock->unlock();
			 	aos_assert_r(itr != mHashMap.end(), false);
	 			aos_assert_r(itr->second->mData == value, false);
	 		 }
	 		 else
	 		 {
	 			// It is not in the entry. Need to add it.
				if (itr != mHashMap.end())
				{
					OmnAlarm << "Invalid" << enderr;
					itr = mHashMap.find(key);
				}
				else
				{
	 				mHashMap[key] = OmnNew AosTmpTest(mValues[key]);
					mStatus[key] = true;
				}
	 			mLock->unlock();
	 		 }
			 break;

		case 2:
			 // Find one to erase
			 mLock->lock();
			 itr = mHashMap.find(key);
			 if (mStatus[key])
			 {
		 		// The entry is already in the map. Erase it.
	 			u64 value = mValues[key];
				mHashMap.erase(key);
				mStatus[key] = false;
		 		mLock->unlock();
			 	aos_assert_r(itr != mHashMap.end(), false);
	 			aos_assert_r(itr->second->mData == value, false);
	 		 }
	 		 else
	 		 {
	 			// It is not in the entry. Need to add it.
				if (itr != mHashMap.end())
				{
					OmnAlarm << "Failed: " << enderr;
					itr = mHashMap.find(key);
				}
	 			mLock->unlock();
	 		 }
			 break;
		}
	}
		
	OmnScreen << "Thread: " << thread->getLogicId() << " finished" << endl;
	mLock->lock();
	mNumThreadsRunning--;
	mLock->unlock();
	aos_assert_r(thread->getLogicId() >= 0 && thread->getLogicId() < eMaxThreads, false);
	mThreads[thread->getLogicId()] = 0;
	return true;
}


bool    
AosHashMapTester::signal(const int threadLogicId)
{
	return true;
}


bool
AosHashMapTester::testPerformance()
{
	u64 tries = 60000000;
	AosHashMap<u64, u64, std::u64_hash, std::u64_cmp, 1000000> themap;
	u64 start = OmnGetTimestamp();
	for (u64 i=0; i<tries; i++)
	{
		themap[i] = i;
	}
	OmnScreen << "Time used: " << OmnGetTimestamp() - start << endl;

	start = OmnGetTimestamp();
	hash_map<u64, u64, std::u64_hash, std::u64_cmp> stl_map;
	for (u64 i=0; i<tries; i++)
	{
		stl_map[i] = i;
	}
	OmnScreen << "Time used: " << OmnGetTimestamp() - start << endl;

	start = OmnGetTimestamp();
	map<u64, u64, std::u64_cmp> stl_map1;
	for (u64 i=0; i<tries; i++)
	{
		stl_map1[i] = i;
	}
	OmnScreen << "Time used: " << OmnGetTimestamp() - start << endl;
	
	return true;
}


bool
AosHashMapTester::testPerformance1()
{
	AosXmlTagPtr config = AosGetConfig();
	u64 tries = 10000000;
	u32 batch_size = 1000000;
	if (config)
	{
		AosXmlTagPtr tag = config->getFirstChild("tester");
		if (tag)
		{
			tries = tag->getAttrU64("tries", tries);
			batch_size = tag->getAttrU64("batch_size", batch_size);
		}
	}
	OmnScreen << "Trying : " << tries << endl;

	// AosHashMap<OmnString, u64, std::Omn_Str_hash, std::compare_str, 1000000> themap;
	AosHashMap<OmnString, u64, std::GenericStrHash, std::compare_str, 10000000> themap;
	u64 idx = 0;
	u32 batch_count = 0;
	u64 batch_start = OmnGetTimestamp();
	while (idx < tries)
	{
		u64 start = OmnGetTimestamp();
		for (u64 i=0; i<batch_size; i++)
		{
			OmnString ss;
			ss << idx << "_" << idx*23456 << "_" << idx;
			themap[ss] = idx;
			idx++;
		}
		batch_count++;
		OmnScreen << "Batch " << batch_count 
			<< ": " << AosConvertToScientific(OmnGetTimestamp() - start) << endl;
	}
	OmnScreen << "Total Entries: " << tries << ". Time used: " 
		<< AosConvertToScientific(OmnGetTimestamp() - batch_start) << endl;

	batch_start = OmnGetTimestamp();
	hash_map<OmnString, u64, std::GenericStrHash, std::compare_str> stl_map;
	idx = 0;
	batch_count = 0;
	while (idx < tries)
	{
		u64 start = OmnGetTimestamp();
		for (u64 i=0; i<batch_size; i++)
		{
			OmnString ss;
			ss << idx << "_" << idx*23456 << "_" << idx;
			stl_map[ss] = idx;
			idx++;
		}
		batch_count++;
		OmnScreen << "Batch " << batch_count
			<< ": " 
			<< AosConvertToScientific(OmnGetTimestamp() - start) << endl;
	}
	OmnScreen << "Total Entries: " << tries
		<< ". Time used: " << AosConvertToScientific(OmnGetTimestamp() - batch_start) << endl;

	map<OmnString, u64, std::str_less> stl_map1;
	idx = 0;
	batch_count = 0;
	batch_start = OmnGetTimestamp();
	while (idx < tries)
	{
		u64 start = OmnGetTimestamp();
		for (u64 i=0; i<tries; i++)
		{
			OmnString ss;
			ss << idx << "_" << idx*23456 << "_" << idx;
			stl_map1[ss] = idx;
			idx++;
		}
		batch_count++;
		OmnScreen << "Batch " << batch_count 
			<< ": " << AosConvertToScientific(OmnGetTimestamp() - start) << endl;
	}
	OmnScreen << "Total Entries: " << tries
		<< ". Time used: " << AosConvertToScientific(OmnGetTimestamp() - batch_start) << endl;
	
	/*
	// Check
	AosHashMap<OmnString, u64, std::GenericStrHash, std::compare_str, 10000000>::iterator itr1;
	batch_start = OmnGetTimestamp();
	for (u64 i=0; i<tries; i++)
	{
		u64 start = OmnGetTimestamp();
		OmnString ss;
		ss << i << "_" << i*23456 << "_" << i;
		itr1 = themap.find(ss);
		aos_assert_r(itr1 != themap.end(), false);
		aos_assert_r(itr1->second == i, false);
	}
	OmnScreen << "Time used: " << AosConvertToScientific(OmnGetTimestamp() - start) << endl;

	start = OmnGetTimestamp();
	hash_map<OmnString, u64, std::GenericStrHash, std::compare_str>::iterator itr2;
	for (u64 i=0; i<tries; i++)
	{
		OmnString ss;
		ss << i << "_" << i*23456 << "_" << i;
		itr2 = stl_map.find(ss);
		aos_assert_r(itr2->second == i, false);
	}
	OmnScreen << "Time used: " << AosConvertToScientific(OmnGetTimestamp() - start) << endl;

	start = OmnGetTimestamp();
	map<OmnString, u64, std::str_less>::iterator itr3;
	for (u64 i=0; i<tries; i++)
	{
		OmnString ss;
		ss << i << "_" << i*23456 << "_" << i;
		itr3 = stl_map1.find(ss);
		aos_assert_r(itr3->second == i, false);
	}
	OmnScreen << "Time used: " << AosConvertToScientific(OmnGetTimestamp() - start) << endl;
	*/

	return true;
}


bool
AosHashMapTester::testMapPerformance()
{
	AosXmlTagPtr config = AosGetConfig();
	u64 tries = 10000000;
	if (config)
	{
		AosXmlTagPtr tag = config->getFirstChild("tester");
		if (tag)
		{
			tries = tag->getAttrU64("tries", tries);
		}
	}
	OmnScreen << "Trying : " << tries << endl;

	map<OmnString, u64, std::str_less> stl_map1;
	u32 batch_size = 1000000;
	int batch_num = 0;
	u64 test_start = OmnGetTimestamp();
	u64 idx = 0;

	OmnString second_entry;
	second_entry  << 1 << "_" << 26456 << "_" << 1;
	while (idx < tries)
	{
		batch_num++;
		u64 start = OmnGetTimestamp();
		for (u64 i=0; i<batch_size; i++)
		{
			OmnString ss;
			ss << idx << "_" << idx*26456 << "_" << idx;
			// stl_map1.insert(make_pair(ss, idx));
			stl_map1[ss] = idx;

			map<OmnString, u64, std::str_less>::iterator itr = stl_map1.find(ss);
			aos_assert_r(itr != stl_map1.end(), false);
			aos_assert_r(itr->second == idx, false);

			itr = stl_map1.find(second_entry);
			if (idx == 0)
			{
				aos_assert_r(itr == stl_map1.end(), false);
			}
			else
			{
				aos_assert_r(itr->second == 1, false);
			}
			idx++;
		}
		OmnScreen << "Batch " << batch_num << " time used: " 
			<< AosConvertToScientific(OmnGetTimestamp() - start) << endl;
	}
	OmnString ss;
	ss << OmnGetTimestamp() - test_start;
	ss.convertToScientific();

	OmnString ss2;
	ss2 << tries;
	ss2.convertToScientific();
	cout << "Total " << ss2 << ". Time used: " << ss << endl;

	// Check the results
	idx = 0;
	while (idx < tries)
	{
		batch_num++;
		u64 start = OmnGetTimestamp();
		for (u64 i=0; i<batch_size; i++)
		{
			OmnString ss;
			ss << idx << "_" << idx*26456 << "_" << idx;
			if (stl_map1[ss] != idx)
			{
				OmnAlarm << "Failed: " << idx << ":" << stl_map1[ss] << enderr;
				return false;
			}
			idx++;
		}
		OmnScreen << "Batch " << batch_num << " time used: " 
			<< AosConvertToScientific(OmnGetTimestamp() - start) << endl;
	}

	ss = "";
	ss << OmnGetTimestamp() - test_start;
	ss.convertToScientific();

	ss2 = "";
	ss2 << tries;
	ss2.convertToScientific();
	cout << "Total " << ss2 << ". Time used: " << ss << endl;
	return true;
}

