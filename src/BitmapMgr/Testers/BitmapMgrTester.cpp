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
// 01/06/2013       Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/Testers/BitmapMgrTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/BitmapMgr.h"
#include "Debug/Debug.h"
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

AosBitmapMgrTester::AosBitmapMgrTester()
{
	aos_assert(config());
}


AosBitmapMgrTester::~AosBitmapMgrTester()
{
}

bool 
AosBitmapMgrTester::config()
{
	mTries = 1000;
	mBitBlockSize = eDftBitBlockSize;
	mMaxDocid = eDftMaxDocid;	
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);

	AosXmlTagPtr tag = conf->getFirstChild("tester");
	if (!tag) return true;

	mTries = tag->getAttrU32("tries", 1000);
	if (mTries <= 0) mTries = 1000;

	mBitBlockSize = tag->getAttrU32("bit_block_size", eDftBitBlockSize);
	if (mBitBlockSize <= 0) mBitBlockSize = eDftBitBlockSize;

	mMaxDocid = tag->getAttrU32("max_docid", eDftMaxDocid);
	if (mMaxDocid <= 0) mMaxDocid = eDftMaxDocid;

	return true;
}

bool 
AosBitmapMgrTester::start()
{
	OmnScreen << "To start bitmap test" << endl;
	testSections();
	// aos_assert_r(basicTest(), false);
	return true;
}


bool
AosBitmapMgrTester::basicTest()
{
	// 1. Init a array
	// 2. Send the items of array to bitmap like this:
	//	  [pos, id]
	// 3. Store the returned the value, the value like this:
	//    [A, a, b, c, d]
	//    [B, c, d, f, a]
	//    ..........
	// 4. Get a random [pos_start, pos_end], and  send the 
	// 	  items of array to bitmap.
	// 5. Retrieve 'A' array from bitmap, and depend on items
	//    of 'A', init 'B' array, you should do that based
	//    on 3.
	// 6. check 'A' and 'B'.
	aos_assert_r(countStep1(), false);	
	aos_assert_r(countStep2(), false);	
	aos_assert_r(checkCountRslt(), false);	
	return true;
}


bool
AosBitmapMgrTester::countStep1()
{
	// 1. init docid
	// 2. put docid to 'mCountList'
	// 3. put docid to bitmap.
	/*
	aos_assert_r(clearEntryMap(), false);

	u64 docid = 0;
	int idx = 0;
	for (int i=0; i<mMaxListSize; i++)
	{
		docid = rand() % eMaxDocid;
		
		if (mSrcList.size() == 0)
		{
			mSrcList.push_back(docid);
		}
		else
		{
			list<u64>::iterator itr = mSrcList.begin();
			idx = rand() % (mSrcList.size()+1);
			while (idx--) ++itr;
			mSrcList.insert(itr, docid);
		}	
		// insert(idx, docid);
	}
	*/

	return true;
}


bool
AosBitmapMgrTester::clearEntryMap()
{
	// 1. get each entry
	// 2. delete the entry
	if (mEntryMap.size() == 0) return true;
	
	map<u64, Entry*>::iterator itr;
	for (itr=mEntryMap.begin(); itr!=mEntryMap.end(); ++itr)
	{
		if (itr->second)
		{
			delete itr->second;
		}
		else
		{
			OmnAlarm << "Inter error" << enderr;
		}
	}
	return true;
}

bool
AosBitmapMgrTester::addEntry(const u64 &target, const vector<u64> &list)
{
	// create an entry as target = mem1 + mem2 + mem3......   mems are from the list
	// add to entry list
	// 1. create entry
	Entry* cur_entry = OmnNew Entry();
	cur_entry->target = target;
	cur_entry->list = list;
	
	// 2. copy list to entry
	mEntryMap[target] = cur_entry;		
	return true;
}


bool
AosBitmapMgrTester::countStep2()
{
	// This function count [pos_start, pos_end].
	int startpos = rand() % mSrcList.size();
	int endpos = rand() % (mSrcList.size()-startpos) + startpos;
	
	mSEList.clear();
	list<u64>::iterator itr1 = mSrcList.begin();
	list<u64>::iterator itr2 = mSrcList.begin();
	while (startpos --) ++itr1;
	while (endpos --) ++itr2;
	mSEList.insert(mSEList.begin(), itr1, itr2);
	aos_assert_r(mSEList.size() == (size_t)(endpos - startpos), false);

	//	Call bitmap function to get mCList
	int idx = 0;
	while (true)
	{
		list<u64>::iterator itr1 = mRList.begin();
		while (idx --) ++itr1;
		u64 docid = *(itr1);
		aos_assert_r(docid>0, false);
		map<u64, Entry*>::iterator itr2 = mEntryMap.find(docid);
		if (itr2 != mEntryMap.end())
		{
			for (int i=0; i<(int)(itr2->second)->list.size(); i++)
			{
				mRList.push_back((itr2->second)->list[i]);
			}
			mRList.erase(itr1);
			continue;
		}

		idx ++;
	}
	return true;
}


bool
AosBitmapMgrTester::addEntry(const u64 &docid)
{
	aos_assert_r(docid > 0, false);
	mRList.push_back(docid);
	return true;
}


bool
AosBitmapMgrTester::checkCountRslt()
{
	aos_assert_r(mRList.size() == mSEList.size(), false);
	
	list<u64>::iterator itr;
	for (itr = mRList.begin(); itr != mRList.end(); ++itr)
	{
		u64 docid = *itr;
		aos_assert_r(docid > 0, false);
		aos_assert_r(isInList(mSEList, docid), false);
	}

	return true;
}


bool
AosBitmapMgrTester::isInList(const list<u64> &l, const u64 &docid)
{
	 list<u64>::const_iterator itr;
	for (itr = l.begin(); itr != l.end(); ++itr)
	{
		if (*itr == docid) return true;
	}
	return false;
}


bool
AosBitmapMgrTester::testSections()
{
	aos_assert_r(mTries > 0, false);
	aos_assert_r(mBitBlockSize > 0, false);
	for (u32 i=0; i<mTries; i++)
	{
		hash_map<u32, vector<u64>, u64_hash, u64_cmp> docid_map;
		hash_map<u32, vector<u64>, u64_hash, u64_cmp>::iterator itr;
		OmnScreen << "Trying " << i << endl;
		AosBitmapObjPtr bitmap = AosGetBitmap();

		int nn = rand() % mBitBlockSize;
		for (int k=0; k<nn; k++)
		{
			u64 docid = (rand() % mMaxDocid) + 1;
			// if (docid == 12080) OmnScreen << "=========== Docid: " << docid << endl;
			u32 section_id = AosBitmapObj::docid2SectionId(docid);

			itr = docid_map.find(section_id);
			if (itr == docid_map.end())
			{
				vector<u64> docids;
				docids.push_back(docid);
				docid_map[section_id] = docids;
			}
			else
			{
				itr->second.push_back(docid);
			}
			bitmap->appendDocid(docid);
		}

		AosBitmapObj::map_t sections;
		bool rslt = bitmap->getSections(mRundata, sections);
		aos_assert_r(rslt, false);

		// AosBitmapObj::mapitr_t tt = sections.begin();
		// while (tt != sections.end())
		// {
		// 	u32 section_id = tt->first;
		// 	if (section_id == 0)
		// 	{
		// 		OmnScreen << "Section: " << tt->first << endl;
		// 		tt->second->toString();
		// 	}
		// 	tt++;
		// }

		// Verify the sections
		itr = docid_map.begin();
		while (itr != docid_map.end())
		{
			u32 section_id = itr->first;
			vector<u64> &docids = itr->second;

			AosBitmapObj::mapitr_t tt = sections.find(section_id);
			aos_assert_r(tt != sections.end(), false);
			AosBitmapObjPtr bmp = tt->second;

			for (u32 m=0; m<docids.size(); m++)
			{
				if (!bmp->checkDoc(docids[m]))
				{
					OmnScreen << "Docid: " << docids[m] << "; Section ID: " << section_id
						<< " not in the bitmap!" << endl;
					OmnAlarm << "Failed tetsing" << enderr;
					return false;
				}
			}

			itr++;
		}
	}

	return true;
}

