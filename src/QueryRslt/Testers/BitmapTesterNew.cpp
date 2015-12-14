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
#include "QueryRslt/Testers/BitmapTesterNew.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "BitmapMgr/Bitmap.h"
#include "Random/RandomUtil.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include <algorithm>

AosBitmapTesterNew::AosBitmapTesterNew()
{
}


AosBitmapTesterNew::~AosBitmapTesterNew()
{
}


bool 
AosBitmapTesterNew::start()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	if (conf)
	{
		AosXmlTagPtr tag = conf->getFirstChild("bitmap_tester");
		if (tag)
		{
			mSanityCheckValue = tag->getAttrInt("sanity_check_value", -1);
			mTries = tag->getAttrInt("tries", eDftTries);
		}
	}

	aos_assert_r(basicTest(), false);
	return true;
}


bool
AosBitmapTesterNew::basicTest()
{
//return checkGetDocidCountNew();
	aos_assert_r(initAndData(), false);
	aos_assert_r(initData(), false);

	bool rslt = false;
	aos_assert_r(sanityCheck(), false);
	mCrtTries = 0;

	for (; mCrtTries<mTries; mCrtTries++)
	{
		Operation opr = Operation(rand() % eNumOperations);
OmnScreen << opr << endl;

		switch (opr)
		{
		case eCheckDoc:
			 rslt = checkDoc();
			 break;

		case eRemoveDocid:
			 rslt = checkRemoveDocid();
			 break;
		
		case eAppendDocid:
			 rslt = checkAppendDocid();
			 break;
		
		case eSaveToBuff:
			 rslt = checkSaveToBuff();
			 break;
		
		case eLoadFromBuff:
			 rslt = checkLoadFromBuff();
			 break;
		
		case eNextDocid:
			 rslt = checkNextDocid();
			 break;
		
		case eCountAndAnotB:
			 rslt = true;
			 //checkCountAndAnotB();
			 break;
		
		case eCountAnd:
			 rslt = checkCountAnd();
			 break;
		
		case eCountOr:
			 rslt = checkCountOr();
			 break;
		
		case eCountAndBlock:
			 rslt = true;
			 //checkCountAndBlock();
			 break;
	
		case eCountOrBlock:
			 rslt = true;
			 //checkCountOrBlock();
			 break;
		
		case eClean:
			 rslt = true;
			 break;
		
		case eNextNDocid:
			 //rslt = checkNextNDocid();
			 rslt = true;
			 break;
		
		case eGetDocids:
			 rslt = checkGetDocids();
			 break;
		
		case eGetDocidCount:
			 rslt = checkGetDocidCount();
			 break;
		
		case eGetSections:
			 rslt = true;
			// checkGetSections();
			 break;

		default:
			 OmnAlarm << "internal error" << enderr;
		}
		aos_assert_r(checkNextDocid(), false);
		aos_assert_r(rslt, false);
		aos_assert_r(sanityCheck(), false);
	}
	aos_assert_r(clean(), false);
	return true;
}


bool
AosBitmapTesterNew::initAndData()
{
	u64 start = rand();
	u64 end = start + rand() % 1000;
	mAndData = OmnNew AosAndBlock(start, end);
	return true;
}

bool
AosBitmapTesterNew::initData()
{
	// This function create some bitmaps and these docids.
	mBitmaps.clear();
	mDocids.clear();
	mOrDocids.clear();
	
	for (int i=0; i<(int)eNumBitmaps; i++)
	{
		mBitmaps.push_back(OmnNew AosBitmap());
	}

	for (int i=0; i<(int)eNumBitmaps; i++)
	{
		vector<u64>docids;
		for (u64 j=mAndData->start; j<mAndData->end; j++)
		{
			mBitmaps[i]->appendDocid(j);
			docids.push_back(j);
		}
		
		int docids_num = rand() % 10000;
		for (int m=0; m<docids_num; m++)
		{
			u64 docid = pickDocid();
			mBitmaps[i]->appendDocid(docid);
			docids.push_back(docid);
		}
		//aos_assert_r(unique(docids), false);
		//sort(docids.begin(), docids.end());
		mDocids.push_back(docids);
	}
	return true;
}


bool
AosBitmapTesterNew::unique(const vector<u64> &docids)
{
	if (docids.size() <= 0) return true;

	size_t index = 0;
	while (index < docids.size())
	{
		u64 docid = docids[index];
		aos_assert_r(docid>0, false);
		int count = 0;
		for (u64 idx = 0; idx < docids.size(); idx++)
		{
			if(docid == docids[idx]) count++;	
		}
		aos_assert_r(count == 1, false);
	}
	return true;
}

u64 
AosBitmapTesterNew::pickDocid()
{
	u64 docid;
	if (mOrDocids.size() == 0) 
	{
		docid = mAndData->end + rand()%100 + 1;
		mOrDocids.push_back(docid);
		return docid;
	}
	
	docid = mOrDocids[mOrDocids.size()-1] + rand()%100 + 1;
	mOrDocids.push_back(docid);
	return docid;
}


bool
AosBitmapTesterNew::checkDoc()
{
	// This function randomly selects some docids that should be
	// in the bitmaps. It then checks whether the docids are 
	// in the bitmaps.
OmnScreen << "CheckDoc" << endl;
	// Try some docids in the bitmaps
	int idx = rand() % eNumBitmaps;
	int nn = rand() % 100;
	for (int i=0; i<nn; i++)
	{
		int mm = rand() % 100;
		if (mm > 40)
		{
			// Check doc exist
			int kk = rand() % mDocids[idx].size();
			u64 docid = mDocids[idx][kk];
			aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
		}
		else if (mm > 20)
		{
			// Check doc does not exist
			if (mDocids[idx].size() <= 0)
			{
				aos_assert_r(!mBitmaps[idx]->checkDoc(rand()), false);
			}
			else if (mDocids[idx].size() == 1)
			{
				u64 docid = rand();
				if (docid != mDocids[idx][0])
				{
					aos_assert_r(!mBitmaps[idx]->checkDoc(docid), false);
				}
				else
				{
					aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
				}
			}
			else
			{
				int kk = rand() % mDocids[idx].size()-1;
				u64 docid1 = mDocids[idx][kk];
				u64 docid2 = mDocids[idx][kk+1];
				if (docid1+1 == docid2)
				{
					aos_assert_r(mBitmaps[idx]->checkDoc(docid1), false);
					aos_assert_r(mBitmaps[idx]->checkDoc(docid2), false);
				}
				else
				{
					u64 delta = rand() % (docid2 - docid1);
					if (delta == 0) delta++;
					aos_assert_r(mBitmaps[idx]->checkDoc(docid1), false);
					aos_assert_r(mBitmaps[idx]->checkDoc(docid2), false);
					aos_assert_r(!mBitmaps[idx]->checkDoc(docid1+delta), false);
				}
			}
		}
		else
		{
			size_t kk = mDocids[idx].size()-1;
			u64 docid1 = mDocids[idx][0];
			u64 docid2 = mDocids[idx][kk];
			aos_assert_r(mBitmaps[idx]->checkDoc(docid1), false);
			aos_assert_r(mBitmaps[idx]->checkDoc(docid2), false);

		}
	}

	return true;
}


bool
AosBitmapTesterNew::checkAppendDocid()
{
OmnScreen << "CheckAppendDocid" << endl;
	int nn = OmnRandom::intByRange(1, 100, 20, 101, 999, 50, 1000, 9999, 30);
	for (int i=0; i<nn; i++)
	{
		int idx = rand() % eNumBitmaps;
		u64 docid = mOrDocids[mOrDocids.size()-1] + rand()%100 + 1;
		if (checkAndAdd(idx, docid))
		{
			// The docid exist
			aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
			mBitmaps[idx]->appendDocid(docid);
			aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
		}
		else
		{
			// The docid does not exist
			aos_assert_r(!mBitmaps[idx]->checkDoc(docid), false);
			mBitmaps[idx]->appendDocid(docid);
			aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
			mDocids[idx].push_back(docid);
		}
	}
	return true;
}


bool
AosBitmapTesterNew::checkRemoveDocid()
{
OmnScreen << "CheckRemoveDocid" << endl;
	int nn = OmnRandom::intByRange(1, 100, 20, 101, 999, 50, 1000, 9999, 30);
	for (int i=0; i<nn; i++)
	{
		int idx = rand() % eNumBitmaps;
		u64 docid = rand();
		if (checkDocid(docid, mDocids[idx]))
		{
			// The docid exist
			aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
			mBitmaps[idx]->removeDocid(docid);
			checkAndRemove(idx, docid);
			aos_assert_r(!mBitmaps[idx]->checkDoc(docid), false);
		}
		else
		{
			// The docid does not exist
			aos_assert_r(!mBitmaps[idx]->checkDoc(docid), false);
			//mBitmaps[idx]->appendDocid(docid);
			//aos_assert_r(mBitmaps[idx]->checkDoc(docid), false);
			//mDocids[idx].push_back(docid);
		}
	}
	return true;
}


bool
AosBitmapTesterNew::checkSaveToBuff()
{
OmnScreen << "CheckSaveToBuff" << endl;
	int nn = OmnRandom::intByRange(1, 10, 80, 11, 99, 15, 100, 999, 5);
	for (int i=0; i<nn; i++)
	{
		int idx = rand() % eNumBitmaps;
		u64 docids_num = mBitmaps[idx]->getDocidCount();
		AosBuffPtr buff = OmnNew AosBuff(docids_num*sizeof(u64) AosMemoryCheckerArgs);
		aos_assert_r(buff, false);
		mBitmaps[idx]->saveToBuff(buff);
		AosBitmapObjPtr bitmap = OmnNew AosBitmap();
		aos_assert_r(bitmap, false);
		bitmap->loadFromBuff(buff);
		for (size_t j=0; j<mDocids[idx].size(); j++)
		{
			aos_assert_r(bitmap->checkDoc(mDocids[idx][j]), false);
		}
	}
	return true;
	
}

	
bool
AosBitmapTesterNew::checkLoadFromBuff()
{
OmnScreen << "CheckLoadFromBuff" << endl;
	int nn = OmnRandom::intByRange(1, 10, 80, 11, 99, 15, 100, 999, 5);
	for (int i=0; i<nn; i++)
	{
		int idx = rand() % eNumBitmaps;
		aos_assert_r(mBitmaps[idx], false);
		//aos_assert_r(unique(mDocids[idx]), false);
		AosBuffPtr buff = OmnNew AosBuff(mDocids[idx].size()*sizeof(u64) AosMemoryCheckerArgs);
		aos_assert_r(buff, false);
		mBitmaps[idx]->saveToBuff(buff);
		AosBitmapObjPtr bitmap = OmnNew AosBitmap();
		aos_assert_r(bitmap, false);
		bitmap->loadFromBuff(buff);
		aos_assert_r(bitmap->getDocidCount() == mBitmaps[idx]->getDocidCount(), false);
		
		/*
		for (size_t j=0; j<mDocids[idx].size(); j++)
		{
			u64 docid = buff->getU64((u64)eMaxDocid);
			aos_assert_r(docid != eMaxDocid, false);
			aos_assert_r(docid == mDocids[idx][j], false);
		}
		*/
	}
	return true;
	
}

	
bool
AosBitmapTesterNew::checkNextDocid()
{
OmnScreen << "CheckNextDocid" << endl;
	int nn = OmnRandom::intByRange(1, 10, 80, 11, 99, 15, 100, 999, 5);
	for (int i=0; i<nn; i++)
	{
		int idx = rand() % eNumBitmaps;
		aos_assert_r(mBitmaps[idx], false);
		
		aos_assert_r(mBitmaps[idx]->getDocidCount()==mDocids[idx].size(), false);
		mBitmaps[idx]->reset();
		for (size_t j=0; j<mDocids[idx].size(); j++)
		{
			u64 docid1 = mDocids[idx][j];
			u64 docid2 = mBitmaps[idx]->nextDocid();
			aos_assert_r(docid1 != eMaxDocid, false);
			aos_assert_r(docid1 == docid2, false);
		}
	}
	return true;
	
}

	
bool
AosBitmapTesterNew::checkCountAndAnotB()
{
OmnScreen << "CheckCountAndAnotB" << endl;
	// This function randomly picks two bitmaps, and then
	// not them. It then check the results.
	int idx1 = rand() % eNumBitmaps;
	int idx2 = rand() % eNumBitmaps;

	AosBitmapObjPtr bitmap1 = mBitmaps[idx1];
	AosBitmapObjPtr bitmap2 = mBitmaps[idx2];
	vector<u64> &docids1 = mDocids[idx1];
	vector<u64> &docids2 = mDocids[idx2];

	AosBitmapObjPtr bb = AosBitmap::countAndAnotB(bitmap1, bitmap2);
	aos_assert_r(bb, false);

	int nn = rand() % 10000;
	for (int i=0; i<nn; i++)
	{
		// 1. Pick a docid from bitmap1,
		// 2. Pick a docid from bitmap2, 
		// 3. Randomly pick a docid
		int mm = rand() % 3;
		if (mm == 0)
		{
			// 1. Pick a docid from bitmap1
			if (docids1.size() > 0)
			{
				int docid = rand() % docids1.size();
				if (checkDocid(docid, docids2))
				{
					aos_assert_r(!bb->checkDoc(docid), false);
				}
				else
				{
					aos_assert_r(bb->checkDoc(docid), false);
				}
			}

			for (size_t nn=0; nn<docids1.size(); nn++)
			{
				aos_assert_r(bb->checkDoc(docids1[nn]), false);
			}
			for (size_t nn=0; nn<docids2.size(); nn++)
			{
				aos_assert_r(!bb->checkDoc(docids2[nn]), false);
			}
		}
		else if (mm == 1)
		{
			// 2. Pick a docid from bitmap2
			if (docids2.size() > 0)
			{
				int docid = rand() % docids2.size();
				aos_assert_r(!bb->checkDoc(docid), false);
			}

			for (size_t i=0; i<docids2.size(); i++)
			{
				u64 docid = docids2[i];
				aos_assert_r(!bb->checkDoc(docid), false);
			}
		}
		else
		{
			u64 docid = rand();
			if (checkDocid(docid, docids1))
			{
				aos_assert_r(bb->checkDoc(docid), false);
			}
			else
			{
				aos_assert_r(!bb->checkDoc(docid), false);
			}
		}
	}

	return true;
}


bool
AosBitmapTesterNew::checkCountOr()
{
	// This function randomly picks two bitmaps, and then
	// AND them. It then check the results.
OmnScreen << "CheckCountOr" << endl;
	int idx1 = rand() % eNumBitmaps;
	int idx2 = rand() % eNumBitmaps;

	AosBitmapObjPtr bitmap1 = mBitmaps[idx1];
	AosBitmapObjPtr bitmap2 = mBitmaps[idx2];
	vector<u64> &docids1 = mDocids[idx1];
	vector<u64> &docids2 = mDocids[idx2];

	AosBitmapObjPtr bb = AosBitmap::countOr(bitmap1, bitmap2);
	aos_assert_r(bb, false);

	int nn = rand() % 10000;
	for (int i=0; i<nn; i++)
	{
		// 1. Pick a docid from bitmap1,
		// 2. Pick a docid from bitmap2, 
		// 3. Randomly pick a docid
		int mm = rand() % 3;
		if (mm == 0)
		{
			// 1. Pick a docid from bitmap1
			if (docids1.size() > 0)
			{
				u64 docid = docids1[rand() % docids1.size()];
				aos_assert_r(bb->checkDoc(docid), false);
			}

			for (size_t i=0; i<docids1.size(); i++)
			{
				u64 docid = docids1[i];
				aos_assert_r(bb->checkDoc(docid), false);
			}
		}
		else if (mm == 1)
		{
			// 2. Pick a docid from bitmap2
			if (docids2.size() > 0)
			{
				u64 docid = docids2[rand() % docids2.size()];
				aos_assert_r(bb->checkDoc(docid), false);
			}

			for (size_t i=0; i<docids2.size(); i++)
			{
				u64 docid = docids2[i];
				aos_assert_r(bb->checkDoc(docid), false);
			}
		}
		else
		{
			u64 docid = rand();
			if (checkDocid(docid, docids1) || checkDocid(docid, docids2))
			{
				aos_assert_r(bb->checkDoc(docid), false);
			}
			else
			{
				aos_assert_r(!bb->checkDoc(docid), false);
			}
		}
	}

	return true;
}


bool
AosBitmapTesterNew::checkCountAndBlock()
{
	// This function randomly picks some bitmaps, and then
	// AND them. It then check the results.
OmnScreen << "checkCountAndBlock" << endl;
	int idx = rand() % eNumBitmaps;
	AosBitmapObjPtr bitmaps[idx];
	for (int i=0; i<idx; i++)
	{
		bitmaps[i] = mBitmaps[i];
	}

	AosBitmapObjPtr bb = AosBitmap::countAndBlock(bitmaps, idx);
	aos_assert_r(bb, false);
	
	for (size_t i=mAndData->start; i<mAndData->end; i++)
	{
		u64 docid = bb->nextDocid();
		aos_assert_r(docid == i, false);
	}
	return true;
}


bool
AosBitmapTesterNew::checkCountOrBlock()
{
	// This function randomly picks some bitmaps, and then
	// OR them. It then check the results.
OmnScreen << "checkCountOrBlock" << endl;
	
	int idx = rand() % eNumBitmaps;
	AosBitmapObjPtr bitmaps[idx];
	for (int i=0; i<idx; i++)
	{
		bitmaps[i] = mBitmaps[i];
	}
	AosBitmapObjPtr bb = AosBitmap::countOrBlock(bitmaps, idx);
	aos_assert_r(bb, false);

	int size = mAndData->end - mAndData->start + mOrDocids.size();
	int count = bb->getDocidCount();
	aos_assert_r(size == count, false);
	
	for (u64 i=mAndData->start; i<mAndData->end; i++)
	{
		aos_assert_r(bb->checkDoc(i), false);
	}
	for (u32 i=0; i<mOrDocids.size(); i++)
	{
		aos_assert_r(bb->checkDoc(mOrDocids[i]), false);
	}
	return true;
}


bool
AosBitmapTesterNew::checkNextNDocid()
{
	int idx = rand() % 10000;
	for (int i=0; i<idx; i++)
	{
		int idx = rand()%eNumBitmaps;
		int num = rand() % mBitmaps[idx]->getDocidCount();
		u64* docids = OmnNew u64[num];
		mBitmaps[idx]->nextNDocid(docids, num);
		for (int j=0; j<num; j++)
		{
			aos_assert_r(checkDocid(docids[j], mDocids[idx]), false); 
			aos_assert_r(mBitmaps[idx]->checkDoc(docids[j]), false); 
		}
		OmnDelete[] docids;
	}
	return true;
}


bool
AosBitmapTesterNew::checkGetDocids()
{
OmnScreen << "checkGetDocids" << endl;
	int num = rand() % (eNumBitmaps + 100);
	for (int i=0; i<num; i++)
	{
		vector<u64> docids;
		int idx = rand()%eNumBitmaps;
		aos_assert_r(mBitmaps[idx]->getDocidCount()==mDocids[idx].size(), false);
		aos_assert_r(mBitmaps[idx]->getDocids(docids), false);
		for (size_t j=0; j<docids.size(); j++)
		{
			aos_assert_r(checkDocid(docids[j], mDocids[idx]), false); 
			aos_assert_r(mBitmaps[idx]->checkDoc(docids[j]), false); 
		}
	}
	return true;
}

	
bool
AosBitmapTesterNew::checkGetDocidCount()
{
OmnScreen << "checkGetDocidCount" << endl;
	int idx = rand() % 10000;
	for (int i=0; i<idx; i++)
	{
		int index = rand()%eNumBitmaps;
		u64 count = mBitmaps[index]->getDocidCount();
		u64 size = mDocids[index].size();
		aos_assert_r(sanityCheck(mDocids[index], mBitmaps[index]), false);
		aos_assert_r(count == size, false);
	}
	return true;
}

bool
AosBitmapTesterNew::checkGetDocidCountNew()
{
	AosBitmapObjPtr bitmap = OmnNew AosBitmap();
	aos_assert_r(bitmap, false);
	vector<u64>docids;
	for (int i=0; i<1000000; i++)
	{
		u64 docid = rand();
		if (docids.size() == 0) 
		{
			docids.push_back(docid);
			bitmap->appendDocid(docid);
		}
		else
		{
			bool bfind = false;
			for (u32 j=0; j<docids.size(); j++)
			{
				if (docids[j] == docid) 
				{
					bfind = true;
					break;
				}
				//bitmap->appendDocid(docid);
				//docids.push_back(docid);
			}
			if (bfind == true)
			{
				bitmap->appendDocid(docid);
				docids.push_back(docid);
			}
		}
	}
	aos_assert_r(bitmap->getDocidCount() == docids.size(), false);
OmnScreen << "Successfull-----------" << docids.size() << "-----------------" << endl;
	return true;
}

bool
AosBitmapTesterNew::checkCountAnd()
{
OmnScreen << "checkCountAnd" << endl;
	// This function randomly picks two bitmaps, and then
	// AND them. It then check the results.
	int idx1 = rand() % eNumBitmaps;
	int idx2 = rand() % eNumBitmaps;

	AosBitmapObjPtr bitmap1 = mBitmaps[idx1];
	AosBitmapObjPtr bitmap2 = mBitmaps[idx2];
	vector<u64> &docids1 = mDocids[idx1];
	vector<u64> &docids2 = mDocids[idx2];

	vector<u64> rsltdocids;
	for (u32 i=0; i<docids1.size(); i++)
	{
		for (u32 j=0; j<docids2.size(); j++)
		{
			if (docids1[i] == docids2[j]) rsltdocids.push_back(docids1[i]);
		}
	}
	AosBitmapObjPtr bb = AosBitmap::countAnd(bitmap1, bitmap2);
	aos_assert_r(bb, false);

	int nn = rand() % 10;
	for (int i=0; i<nn; i++)
	{
OmnScreen << i << endl;
		// 1. Pick a docid from bitmap1,
		// 2. Pick a docid from bitmap2, 
		// 3. Randomly pick a docid
		int mm = rand() % 3;
		if (mm == 0)
		{
			// 1. Pick a docid from bitmap1
			if (docids1.size() > 0)
			{
				int docid = rand() % docids1.size();
				if (checkDocid(docid, docids2))
				{
					aos_assert_r(bb->checkDoc(docid), false);
				}
				else
				{
					aos_assert_r(!bb->checkDoc(docid), false);
				}
			}

			for (size_t i=0; i<docids1.size(); i++)
			{
				u64 docid = docids1[i];
				if (checkDocid(docid, docids2))
				{
					aos_assert_r(bitmap1->checkDoc(docid), false);
					aos_assert_r(bitmap2->checkDoc(docid), false);
					aos_assert_r(checkDocid(docid, rsltdocids), false);
					u64 ssss = rsltdocids.size();
					u64  cccc = bb->getDocidCount();
					aos_assert_r(bb->checkDoc(docid), false);

				}
				else
				{
					aos_assert_r(!bb->checkDoc(docid), false);
				}
			}
		}
		else if (mm == 1)
		{
			// 2. Pick a docid from bitmap2
			if (docids2.size() > 0)
			{
				int docid = rand() % docids2.size();
				if (checkDocid(docid, docids1))
				{
					aos_assert_r(bb->checkDoc(docid), false);
				}
				else
				{
					aos_assert_r(!bb->checkDoc(docid), false);
				}
			}

			for (size_t i=0; i<docids2.size(); i++)
			{
				u64 docid = docids2[i];
				if (checkDocid(docid, docids1))
				{
					aos_assert_r(bb->checkDoc(docid), false);
				}
				else
				{
					aos_assert_r(!bb->checkDoc(docid), false);
				}
			}
		}
		else
		{
			u64 docid = rand();
			if (checkDocid(docid, docids1))
			{
				if (checkDocid(docid, docids2))
				{
					aos_assert_r(bb->checkDoc(docid), false);
				}
				else
				{
					aos_assert_r(!bb->checkDoc(docid), false);
				}
			}
			else
			{
				aos_assert_r(!bb->checkDoc(docid), false);
			}
		}
	}

	return true;
}


bool
AosBitmapTesterNew::clean()
{
	for (size_t i=0; i<mBitmaps.size(); i++)
	{
		mBitmaps[i]->clean();
	}
	return true;
}


bool
AosBitmapTesterNew::checkAndAdd(
		const int idx, 
		const u64 &docid)
{
	// This function checks whether docid is in mBitmaps[idx].
	// If yes, it returns true. Otherwise, it adds the docid
	// to mDocids[idx] and returns false.
	for (size_t i=0; i<mDocids[idx].size(); i++)
	{
		if (mDocids[idx][i] == docid) return true;
	}

	return false;
}

bool
AosBitmapTesterNew::checkAndRemove(
		const int idx,
		const u64 &docid)
{
	vector<u64>::iterator itr = mDocids[idx].begin();
	vector<u64>::iterator itr_end = mDocids[idx].end();
	while (itr != itr_end)
	{
		if (*itr == docid)
		{
			mDocids[idx].erase(itr);
			return true;
		}
		itr++;
	}

	return false;
}

bool
AosBitmapTesterNew::checkDocid(
		const u64 &docid, 
		const vector<u64> &docids)
{
	// This function checks whether 'docid' is in 'docids'. 
	// If yes, it returns true. Otherwise, it returns false.
	for (size_t i=0; i<docids.size(); i++)
	{
		if (docid == docids[i]) return true;
	}
	return false;
}


bool
AosBitmapTesterNew::sanityCheck()
{
	//if (mSanityCheckValue < 0 || mCrtTries < mSanityCheckValue) return true;
OmnScreen << "SanityCheck" << endl;
 
	for (u32 i=0; i<eNumBitmaps; i++)
	{
		aos_assert_r(mDocids[i].size() == mBitmaps[i]->getDocidCount(), false);
		aos_assert_r(sanityCheck(mDocids[i], mBitmaps[i]), false);
	}
	return true;
}


bool
AosBitmapTesterNew::sanityCheck(
		const vector<u64> &docids,
		const AosBitmapObjPtr &bitmap)
{
	aos_assert_r(docids.size()==bitmap->getDocidCount(), false);
	for (u32 i=0; i<docids.size(); i++)
	{
		aos_assert_r(bitmap->checkDoc(docids[i]), false);
	}
	return true;
}

