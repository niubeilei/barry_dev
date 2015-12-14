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
// 2013/02/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "BitmapEngine/Testers/BitmapExecutorTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "BitmapEngine/Ptrs.h"
#include "BitmapEngine/BitmapEngine.h"
#include "BitmapEngine/BitmapEngineUtil.h"
#include "BitmapEngine/Testers/BitmapTest.h"
#include "BitmapEngine/BitmapExecutor.h"
#include "BitmapUtil/BitmapCache.h"
#include "BitmapUtil/BitmapStorageMgr.h"
#include "BitmapUtil/BitmapUtil.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"


AosBitmapExecutorTester::AosBitmapExecutorTester()
:
mLock(OmnNew OmnMutex()),
mNumNodes(eDftNumNodes),
mCallFinished(false)
{
	mNumMolds = 1000;
	mMolds = OmnNew vector<u64>[mNumMolds];
	for (int i=0; i<mNumMolds; i++)
	{
		int nn = rand() % 10000;
		for (int k=0; k<nn; k++)
		{
			mMolds[i].push_back(rand());
		}
	}

	mExecutor = OmnNew AosBitmapExecutor();
}


bool 
AosBitmapExecutorTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(AosGetDftSiteId());
	OmnScreen << "    Start Tester ..." << endl;

	config();
	init();
	// basicTest();
	grandTorturer();
	return true;
}


bool
AosBitmapExecutorTester::init()
{
	// aos_assert_r(mNumNodes > 0, false);
	// for (int i=0; i<mNumNodes; i++)
	// {
	// 	AosBitmapTestPtr bitmap = OmnNew AosBitmapTest();
	// 	aos_assert_r(bitmap, false);
	// 	u64 bitmap_id = rand() % eMaxBitmapId + 10000;
	// 	bitmap->setBitmapId(bitmap_id);
	// 	int level = (rand() % 10) + 2;
	// 	bitmap->setLevel(level);
	// 	mNodes.push_back(bitmap);
	// }

	return true;
}


bool
AosBitmapExecutorTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("bitmap_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	mNumNodes = tag->getAttrInt("num_nodes_to_create", eDftNumNodes);
	if (mNumNodes <= 0) mNumNodes = eDftNumNodes;

	return true;
}


bool 
AosBitmapExecutorTester::basicTest()
{
	return true;
}


bool 
AosBitmapExecutorTester::grandTorturer()
{
	vector<int> weights;
	int total = 100; 	weights.push_back(total); 	// Create Bitmaps
	total += 100; 	 	weights.push_back(total);	// Insert Bits
	total += 70;	 	weights.push_back(total);	// Remove Bits
	total += 50;		weights.push_back(total);	// rebuild bitmaps
	total += 2;			weights.push_back(total);	// finish 
	total += 2;			weights.push_back(total);	// get bitmap id
	total += 2;			weights.push_back(total);	// get iil bitmap id

	bool rslt;
	OmnScreen << "MMMMM: mTries: " << mTries << endl;
	for (int i=0; i<mTries; i++)
	{
		int opr = pickOperation(weights);
		rslt = false;
		switch (opr)
		{
		case 0:
			 OmnScreen << "MMMMM: " << i << ", Create Bitmap" << endl;
			 if (createLeaf()) rslt = true;
			 break;

		case 1: 
			 OmnScreen << "MMMMM: " << i << ", Insert Bits" << endl;
			 rslt = insertBits();
			 break;

		case 2: 
			 OmnScreen << "MMMMM: " << i << ", Remove Bits" << endl;
			 rslt = removeBits();
			 break;

		case 3: 
			 OmnScreen << "MMMMM: " << i << ", RebuildBitmap" << endl;
			 rslt = rebuildBitmap();
			 break;

		case 4: 
			 OmnScreen << "MMMMM: " << i << ", Finish" << endl;
			 rslt = finish();
			 break;

		case 5: 
			 OmnScreen << "MMMMM: " << i << ", Get Bitmap ID" << endl;
			 rslt = getBitmapId();
			 break;

		case 6: 
			 OmnScreen << "MMMMM: " << i << ", Get Bitmap ID by IILID" << endl;
			 rslt = getBitmapIdByIILID();
			 break;

		default:
			 OmnAlarm << "Invalid operator: " << opr << enderr;
			 rslt = false;
			 break;
		}

		if (!rslt)
		{
			OmnAlarm << "Failed operation: " << opr << enderr;
		}
	}
	return true;
}


int
AosBitmapExecutorTester::pickMold()
{
	return rand() % mNumMolds;
}


AosBitmapTestPtr
AosBitmapExecutorTester::createBitmap(
		const int node_level,
		const int iil_level)
{
	// This function creates a bitmap. It randomly picks two molds
	// and use the two to create a bitmap. Created bitmaps are 
	// saved in 'mBitmapsCreated'. There are only two levels: 
	// -1 and 0. 

	aos_assert_r(iil_level == 0 || iil_level == -1, 0);
	int idx1 = pickMold();
	int idx2 = pickMold();
	vector<u64> &mold1 = mMolds[idx1];
	vector<u64> &mold2 = mMolds[idx2];

	AosBitmapTestPtr bitmap1 = OmnNew AosBitmapTest();
	AosBitmapObjPtr bitmap2 = AosGetBitmap();
	aos_assert_r(bitmap2, 0);

	for (u32 i=0; i<mold1.size(); i++)
	{
		u64 offset = mold2[i%mold2.size()];
		bitmap1->appendDocid(mold1[i] + offset);
		bitmap2->appendDocid(mold1[i] + offset);
	}

	bitmap1->setNodeLevel(node_level);
	bitmap1->setIILLevel(iil_level);
	bitmap2->setNodeLevel(node_level);
	bitmap2->setIILLevel(iil_level);
	if (iil_level == -1)
	{
		// It is to create a temporary bitmap
		bool rslt = mExecutor->createTempBitmap(mRundata, bitmap2);
		aos_assert_r(rslt, 0);
		u64 bitmap_id = bitmap2->getBitmapId();
		bitmap1->setBitmapId(bitmap_id);
	}
	else
	{
		// It is to create a leaf
		u64 iilid = rand();
		u64 bitmap_id = mExecutor->getBitmapId(mRundata, iilid, node_level,iil_level);
		bitmap1->setBitmapId(bitmap_id);
		bitmap2->setBitmapId(bitmap_id);
		bool rslt = mExecutor->createLeaf(mRundata, bitmap2);
		aos_assert_r(rslt, 0);
	}

	return bitmap1;
}


AosBitmapTestPtr
AosBitmapExecutorTester::createLeaf()
{
	AosBitmapTestPtr bitmap = createBitmap(0,0);
	u64 bitmap_id = bitmap->getBitmapId();
	aos_assert_r(bitmap_id > 0, 0);
	mPendingLeaves.push_back(bitmap);
	return bitmap;
}


AosBitmapTestPtr
AosBitmapExecutorTester::pickNode(
		const int level, 
		const bool pick_leaf)
{
	// It picks a node at the level 'level'. If 'pick_leaf' is true, it uses 25%
	// chances to pick a leaf. If needed, it may create a leaf.
	if (pick_leaf)
	{
		int idx = rand() % 4;
		if (idx == 0)
		{
			// Pick a leaf. 
			idx = rand() % 2;
			if (idx == 0)
			{
				// Pick from mPendingLeaves.
				if (mPendingLeaves.size() > 0)
				{
					idx = rand() % mPendingLeaves.size();
					return mPendignLeaves[idx];
				}
			}

			// Pick from mProcessed Leaves
			if (mProcessedLeaves.size() > 0)
			{
				idx = rand() % mProcessedLeaves.size();
				return mProcessedLeaves[idx];
			}

			return createLeaf();
		}
	}

	// Pick a node
	if (mNodes.size() <= 0) return 0;
	int idx = rand() % mNodes.size();
	return mNodes[idx];
}


AosBitmapTestPtr
AosBitmapExecutorTester::pickPendingLeaf()
{
	if (mPendingLeaves.size() <= 0)
	{
		return createLeaf();
	}

	// Pick a leaf
	int idx = rand() % mPendingLeaves.size();
	return mPendingLeaves[idx];
}


bool
AosBitmapExecutorTester::insertBits()
{
	// insertBits is called on either a leaf or a level one node.
	// After that, it should call all its parents to insert
	// the same bits.
	// 1. Pick a node
	u64 tt1 = OmnGetTimestamp();
	AosBitmapTestPtr node = pickNode(true);
	if (!node) return true;
	u64 node_id = node->getBitmapId();
	aos_assert_r(node_id > 0, false);

	// 2. Create a temporary bitmap
	AosBitmapTestPtr bitmap = createBitmap(-1,-1);
	aos_assert_r(bitmap, false);
	u64 bid = bitmap->getBitmapId();
	aos_assert_r(AosBitmapEngineUtil::isTempBitmapId(bid), false);

	// 3. Modify the node
	u64 tt2 = OmnGetTimestamp();
	node->orBitmap(bitmap);
	u64 tt3 = OmnGetTimestamp();

	// 4. Generate iil level
	int iil_level = 0;

	// 4. Insert the bits to node 
	bool rslt = mExecutor->insertBits(mRundata, node_id, node->getNodeLevel(), iil_level, bid);
	u64 tt4 = OmnGetTimestamp();
	aos_assert_r(rslt, false);

	// 5. Verify the changes
	if (node->getNodeLevel() <= 1) 
	{
		bool rslt = verifyLeaf(node);
		aos_assert_r(rslt, false);
	}

	u64 tt5 = OmnGetTimestamp();
	OmnScreen << "===== Insert Bits: " 
		<< "Total: " << tt5 - tt1 
		<< ", Prepare: " << tt2 - tt1 
		<< ", Bitmap OR: " << tt3 - tt2 
		<< ", Executor Insert: " << tt4 - tt3 
		<< ", Verify: " << tt5 - tt4 << endl;
	return true;
}


bool
AosBitmapExecutorTester::verifyLeaf(const AosBitmapTestPtr &node)
{
	// 'node' is the one maintained by this class. It retrieves the node
	// from the database and then compares it.
	u64 bitmap_id = node->getBitmapId();
	aos_assert_r(node->getNodeLevel() == 0, false);

	AosBitmapObjPtr bitmap = AosGetBitmapStorageMgr()->retrieveLeaf(
			mRundata, bitmap_id);
	aos_assert_r(bitmap, false);
	aos_assert_r(node->compareBitmap(bitmap), false);
	return true;
}


bool
AosBitmapExecutorTester::removeBits()
{
	// It creates a temporary bitmap first, and then pick a node to 
	// insert the bits.
	// 1. Pick a node
	u64 tt1 = OmnGetTimestamp();
	AosBitmapTestPtr node = pickNode(true);
	if (!node) return true;
	u64 node_id = node->getBitmapId();
	aos_assert_r(node_id > 0, false);

	// 2. Create a temporary bitmap
	AosBitmapTestPtr bitmap = createBitmap(-1,-1);
	aos_assert_r(bitmap, false);
	u64 bid = bitmap->getBitmapId();
	aos_assert_r(AosBitmapEngineUtil::isTempBitmapId(bid), false);

	// 3. Modify the node
	node->removeBits(bitmap);
	u64 tt2 = OmnGetTimestamp();

	// 4. Generate iil level
	int iil_level = 0;

	// 4. Insert the bits to node 
	bool rslt = mExecutor->removeBits(mRundata, node_id, node->getNodeLevel(), 
			iil_level, bid);
	aos_assert_r(rslt, false);
	u64 tt3 = OmnGetTimestamp();

	// 5. Verify the changes
	if (node->getNodeLevel() <= 1) 
	{
		rslt = verifyLeaf(node);
	}
	u64 tt4 = OmnGetTimestamp();

	OmnScreen << "===== Remove Bits: " << tt4 - tt1 << ":" << tt4 - tt3 << ":" 
		<< tt3 - tt2 << ":" << tt2 - tt1 << endl;
	return true;
}


bool
AosBitmapExecutorTester::rebuildBitmap()
{
	u64 tt1 = OmnGetTimestamp();

	AosBitmapTestPtr node = pickNode(false);
	aos_assert_r(mNumNodes > 0, false);
	if (mNodes.size() < (u32)mNumNodes)
	{
		if (rand() % 10 == 0)
		{
			node = 0;
		}
	}

	if (!node)
	{
		node = OmnNew AosBitmapTest();
		aos_assert_r(node, false);
		u64 bitmap_id = rand() % eMaxBitmapId + 10000;
		node->setBitmapId(bitmap_id);
		int node_level = (rand() % 10) + 2;
		node->setNodeLevel(node_level);
		mNodes.push_back(node);
	}
	aos_assert_r(node, false);
	int node_level = node->getNodeLevel();
	aos_assert_r(node_level > 1, false);
	u64 node_id = node->getBitmapId();
	aos_assert_r(node_id > 0, false);

	// Determine how many children to add
	int num_members = OmnRandom::intByRange(
			2, 5, 50, 
			6, 10, 50, 
			11, 20, 50, 
			21, 30, 30, 
			31, 50, 10, 
			51, 100, 2);
	vector<u64> member_ids;
	for (int i=0; i<num_members; i++)
	{
		AosBitmapTestPtr leaf = pickPendingLeaf();
		aos_assert_r(leaf, false);
		u64 id = leaf->getBitmapId();
		member_ids.push_back(id);

		bool rslt = mExecutor->addLeaf(rdata, 
	}
	u64 tt2 = OmnGetTimestamp();

	// shawn  bool rslt = mExecutor->rebuildBitmap(mRundata, node_id, node_level, member_ids);
	bool rslt = mExecutor->rebuildBitmap(mRundata, node_id, node_level, 1, member_ids);
	u64 tt3 = OmnGetTimestamp();
	aos_assert_r(rslt, false);
	OmnScreen << "===== Rebuild Bitmap: " << tt3 - tt1 
		<< ":" << tt3 - tt2 << ":" << tt2 - tt1 << endl;
	return true;
}


bool
AosBitmapExecutorTester::finish()
{
	mCallFinished = false;
	AosBmpExeCallerPtr thisptr(this, false);
	bool rslt = mExecutor->finish(mRundata, 0, thisptr);
	aos_assert_r(rslt, false);

	mLock->lock();
	if (!mCallFinished)
	{
		mCondVar->wait(mLock);
	}
	mLock->unlock();
	
	verifyResults();
	return true;
}


bool
AosBitmapExecutorTester::verifyResults()
{
	// There are a number of leaves and nodes. Leaves are 
	// stored in local storage and nodes are stored in remote
	// storage. This function verifies every leaf and node.
	for (u32 i=0; i<mLeaves.size(); i++)
	{
		AosBitmapTestPtr bitmap = mLeaves[i];
		u64 bitmap_id = bitmap->getBitmapId();
		AosBitmapObjPtr bb = AosGetBitmapStorageMgr()->retrieveBitmap(
				mRundata, bitmap_id);
		aos_assert_r(bb, false);
		bool rslt = bitmap->compareBitmap(bb);
		aos_assert_r(rslt, false);
	}

	for (u32 i=0; i<mNodes.size(); i++)
	{
		AosBitmapTestPtr bitmap = mNodes[i];
		if (bitmap->isEmpty()) continue;

		u64 bitmap_id = bitmap->getBitmapId();

		AosBitmapObj::map_t bitmaps;
		bitmap->getSections(mRundata, bitmaps);
		aos_assert_r(bitmaps.size() > 0, false);

		AosBitmapObj::mapitr_t tt = bitmaps.begin();
		while (tt != bitmaps.end())
		{
			u64 section_id = tt->first;
			int node_level = bitmap->getNodeLevel();
			int iil_level = bitmap->getIILLevel();

			AosBitmapObjPtr bb = AosGetBitmapEngine()->retrieveBitmap(
				mRundata, bitmap_id, section_id, node_level, iil_level);
			aos_assert_r(bb, false);
			bool rslt = bitmap->compareBitmap(bb);
			aos_assert_r(rslt, false);
			tt++;
		}
	}

	return true;
}


bool
AosBitmapExecutorTester::getBitmapId()
{
	return true;
}


bool
AosBitmapExecutorTester::getBitmapIdByIILID()
{
	return true;
}


bool
AosBitmapExecutorTester::check()
{
	return true;
}


void 
AosBitmapExecutorTester::reqProcessed(const AosRundataPtr &rdata)
{
	OmnScreen << "Call finished" << endl;
	mLock->lock();
	mCallFinished = true;
	mCondVar->signal();
	mLock->unlock();
}
#endif
