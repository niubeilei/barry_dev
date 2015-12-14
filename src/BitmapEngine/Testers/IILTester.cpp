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
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapEngine/Testers/IILTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "BitmapEngine/Ptrs.h"
#include "BitmapEngine/BitmapTrans.h"
#include "BitmapEngine/BitmapEngine.h"
#include "BitmapEngine/BitmapInstr.h"
#include "BitmapEngine/BitmapExecutor.h"
#include "BitmapEngine/BitmapInstr.h"
#include "BitmapEngine/BitmapEngineUtil.h"
#include "BitmapEngine/Testers/BitmapTest.h"
#include "BitmapTreeMgr/BitmapTree.h"
#include "BitmapUtil/BitmapCache.h"
#include "BitmapUtil/BitmapStorageMgr.h"
#include "BitmapUtil/BitmapUtil.h"
#include "SEUtil/IILIdx.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryBitmapObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"

static char *sgStrings[AosIILTester::eNumStrings];
static OmnString sgDftIILName = "__zt44_test";
static u64 sgDocid = 10000;

AosIILTester::AosIILTester()
:
mRecordLen(eDftRecordLen)
{
	mExecutor = OmnNew AosBitmapExecutor();
}


bool 
AosIILTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(AosGetDftSiteId());
	OmnScreen << "    Start Tester ..." << endl;

	config();
	init();
	grandTorturer();
	return true;
}


bool
AosIILTester::init()
{
	int len = (mRecordLen - sizeof(u64) - 1)/2;
	for (int i=0; i<eNumStrings; i++)
	{
		sgStrings[i] = OmnNew char[len+1];
		OmnRandom::letterDigitStr(sgStrings[i], len, len);
		sgStrings[i][len] = 0;
	}
	return true;
}


bool
AosIILTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("bitmap_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	mIILName = tag->getAttrStr("iilname", sgDftIILName);
	if (mIILName == "") mIILName = sgDftIILName;

	mRecordLen = tag->getAttrInt("record_len", eDftRecordLen);
	if (mRecordLen <= 0) mRecordLen = eDftRecordLen;

	mBatchSize = tag->getAttrInt("batch_size", eDftBatchSize);
	if (mBatchSize <= 0) mBatchSize = eDftBatchSize;
	
	return true;
}


bool 
AosIILTester::basicTest()
{
	return true;
}


bool 
AosIILTester::grandTorturer()
{
	vector<int> weights;
	int total = 100; 	weights.push_back(total); 	// Create Add contents
	total += 10; 	 	weights.push_back(total);	// Check 

	bool rslt;
	OmnScreen << "MMMMM: mTries: " << mTries << endl;
	for (int i=0; i<mTries; i++)
	{
		int opr = pickOperation(weights);
		rslt = false;
		switch (opr)
		{
		case 0:
			 OmnScreen << "MMMMM: " << i << ", Add contents" << endl;
			 rslt = addContents();
			 break;

		case 1: 
			 OmnScreen << "MMMMM: " << i << ", Check " << endl;
			 rslt = checkResults();
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


bool
AosIILTester::addContents()
{
	// It randomly generates some contents and then add the contents
	// to the IIL.
	aos_assert_r(mBatchSize > 0, false);
	int nn = rand() % mBatchSize;
	int idx = rand() % eNumStrings;
	int delta = rand() % 10;
	char buff[mRecordLen+1];
	int str_len = (mRecordLen-sizeof(u64)-1)/2;

	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(
			mCompareFunc, true, false, nn * mRecordLen + 1000);
	for (int i=0; i<nn; i++)
	{
		memcpy(buff, sgStrings[idx], str_len);
		idx += delta; 
		if (idx >= eNumStrings) idx -= eNumStrings;
		memcpy(&buff[str_len], sgStrings[idx], str_len);
		idx += delta;
		if (idx >= eNumStrings) idx -= eNumStrings;
		buff[str_len*2] = 0;

		u64 docid = sgDocid++;
		memcpy(&buff[mRecordLen-sizeof(u64)], &docid, sizeof(u64));
		buff_array->addValue(buff, mRecordLen, mRundata);
	}

	buff_array->sort();

	// Add to the IIL.
	OmnString sdoc_str = "<sdoc ";
	sdoc_str << AOSTAG_ZKY_TYPE << "=\"" << AOSACTTYPE_STRIILBATCHADD
		<< "\">"
		<< "<iil " << AOSTAG_IILNAME << "=\"" << mIILName
		<< "\" " << AOSTAG_LENGTH << "=\"" << mRecordLen 
		<< "\"/>"
		<< "</sdoc>";
	AosXmlTagPtr action_xml = AosStr2Xml(mRundata, sdoc_str AosMemoryCheckerArgs);
	aos_assert_r(action_xml, false);
	AosBuffPtr bb = buff_array->getBuff();
	AosRundataParmPtr parm = (AosRundataParm*)mExecutor.getPtr();
	mRundata->setParm(parm);
	bool rslt = AosActionObj::runAction(mRundata, action_xml, bb);
	mRundata->removeParm(parm);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILTester::checkResults()
{
	// The IIL should be the same as 'mBuffArray'. It randomly
	// picks a range. It then queries the IIL. 

	// 1. Determine a range: [start, end]
	if (mBuffArray->size() <= 0) return true;
	int64_t start = OmnRandom::intByRange(
					0, 0, 50, 
					1, 100, 50,
					101, 10000, 50, 
					11000, mBuffArray->size()-1, 50);
	if (start >= mBuffArray->size() || start < 0) 
	{
		start = rand() % mBuffArray->size();
	}

	int64_t end = OmnRandom::intByRange(
					start, start, 50, 
					start+1, start+100, 50, 
					start+101, start+10000, 50, 
					start+11000, mBuffArray->size()-1, 50);
	if (end >= mBuffArray->size() || end < 0) 
	{
		end = rand() % mBuffArray->size();
	}
	if (start > end) 
	{
		int64_t dd = end;
		end = start;
		start = dd;
	}

	// 2. Find the starting record. This is the record so that
	//    its previous record's key is different from its key.
	int record_len;
	OmnString start_record = mBuffArray->getRecord(start, record_len);
	aos_assert_r(start_record != "", false);
	aos_assert_r(record_len == mRecordLen, false);
	// while (start > 0)
	// {
	// 	char *prev_record = start_record.data()-record_len;
	// 	if (strncmp(prev_record, start_record, mKeyLen) != 0) break;
	// 	start_record -= record_len;
	// 	start--;
	// }

	// 3. Find the end record. This is the record so that 
	//    its next record's key is different from its key.
	OmnString end_record = mBuffArray->getRecord(end, record_len);
	aos_assert_r(end_record != "", false);
	aos_assert_r(record_len == mRecordLen, false);
	// while (end < mBuffArray->size()-1)
	// {
	// 	char *next_record = end_record+record_len;
	// 	if (strncmp(next_record, end_record, mKeyLen) != 0) break;
	// 	end_record += record_len;
	// 	end++;
	// }

	// 4. Now we obtained two records:
	// 		[start_record, end_record].
	// These are the conditions. 
	return verifyContents();
}


bool
AosIILTester::verifyContents()
{
	// An IIL has been created. It is saved in 'mBuffArray':
	// 		[key, docid]
	// 		[key, docid]
	// 		...
	// The IIL has a bitmap tree. 
	// This function checks whether the bitmaps were created 
	// correctly. 
	
	// 1. Verify Leaves
	//    Loop over all the Level 0 sub-iils. For each Level 0
	//    sub-iil, it retrieves its leaf bitmap. Docids in the
	//    sub-iil must be in the bitmap. 
	//
	//    OR all the leaves to create Bitmap B1. Create a bitmap
	//    B2 based on 'mBuffArray'. B1 should be the same as B2.
	// 	
	// 2. Verify Level One Nodes
	//    For each leaf bitmap, it retrieves the leaf's parent
	//    from the bitmap tree. It then retrieves the level 1
	//    bitmap. All leaf bitmaps must be in a level one node.
	//    This is to verify Level One nodes were constructed
	//    correctly.
	// 	
	// 3. Verify Tree Structure
	//    The above step collected all the Level One nodes. Make
	//    should all Level One nodes are contained in a parent node.
	//    By doing so, collect all Level 2 nodes. 
	//
	//    Repeat the same process to verify the parent nodes until
	//    it reaches the root.
	//
	// 4. Verify Adding Bits
	//    Since last call of adding contents, it caused a series
	//    of bits being added to leaves. Each should climb up 
	//    along the tree path to add the same bits to its parent
	//    nodes. 
	//
	//    Create a bitmap B1 that is the OR of the addBits(...) 
	//    for leaves. Create another bitmap B2 that is the results
	//    of the last addContents(...) function call. B1 should equal
	//    to B2.
	//
	//    For each leaf addBits(...) instruction, it should have an
	//	  addBits(...) instruction to all its parent nodes. 
	//
	// 5. Verify Create Bitmaps
	//    For every bitmap (either leaf or temporary), there must be an instruction
	//    to create it. 
	//
	// 6. Verify Rebuild Node
	//    For each inner node, there shall be an instruction 'rebuildBitmap(...)'. 
	//    The last 'rebuildBitmap(...)' should create/rebuild the node 
	//    that is the same as the one retrieved from the bitmap tree.
	//
	//    When rebuilding a node, if the rebuilt is not a root, there
	//    should be some addBits(...) and removeBits(...) instructions. 

	aos_assert_r(verifyLeaves(), false);
	aos_assert_r(verifyAddBits(), false);
	aos_assert_r(verifyAddBitsPath(), false);
	aos_assert_r(verifyRebuildNodes(), false);
	aos_assert_r(verifyCreateBitmaps(), false);
	return true;	
}


bool 
AosIILTester::verifyLeaves()
{
	// Verify Leaves
	// Loop over all the Level 0 sub-iils. For each Level 0
	// sub-iil, it retrieves its leaf bitmap. Docids in the
	// sub-iil must be in the bitmap. 
	//
	// OR all the leaves to create Bitmap B1. Create a bitmap
	// B2 based on 'mBuffArray'. B1 should be the same as B2.
	//
	// Verify Level One Nodes
	// For each leaf bitmap, it retrieves the leaf's parent
	// from the bitmap tree. It then retrieves the level 1
	// bitmap. All leaf bitmaps must be in a level one node.
	// This is to verify Level One nodes were constructed
	// correctly.
	// 	
	if (!mBuffArray || mBuffArray->size() <= 0) return true;

	vector<u64> leaf_ids;
	aos_assert_r(verifyIILs(leaf_ids), false);
	aos_assert_r(verifyLeavesAndLevelOneNodes(leaf_ids), false);
	aos_assert_r(verifyTreeStructure(), false);
	return true;
}


bool
AosIILTester::verifyLeavesAndLevelOneNodes(
		const vector<u64> &leaf_ids)
{
	vector<u64> level_one_nodes;
	bool rslt = getLevelOneNodes(level_one_nodes);
	aos_assert_r(rslt, false);
	aos_assert_r(level_one_nodes.size() > 0, false);

	AosU64BoolHash level_one_map;
	aos_assert_r(verifyLeafParents(leaf_ids, level_one_map), false);
	aos_assert_r(verifyLevelOneNodes(leaf_ids, level_one_nodes, level_one_map), false);
	return true;
}


bool
AosIILTester::verifyLevelOneNodes(
		const vector<u64> &leaf_ids, 
		const vector<u64> &level_one_nodes, 
		AosU64BoolHash &level_one_map)
{
	// For each level one node, retrieve all its members. These
	// members must be defined in 'leaf_ids' and all leaves in
	// 'leaf_ids' must be defined by a level one node in 'level_one_nodes'.
	u32 num_leaves = leaf_ids.size();
	aos_assert_r(num_leaves > 0, false);
	bool flags[num_leaves];
	memset(flags, false, num_leaves);
	for (u32 i=0; i<level_one_nodes.size(); i++)
	{
		u64 node_id = level_one_nodes[i];
		vector<u64> member_ids;
		mTree->getMemberIds(mRundata, node_id, member_ids);
		for (u32 i=0; i<member_ids.size(); i++)
		{
			int idx = getLeafIdx(leaf_ids, member_ids[i]);
			aos_assert_r(idx >= 0 && (u32)idx < leaf_ids.size(), false);
			flags[idx] = true;
		}
	}

	// All flags should be set to true.
	for (u32 i=0; i<leaf_ids.size(); i++)
	{
		aos_assert_r(flags[i], false);
	}

	aos_assert_r(verifyLevelOneNodes(level_one_map, level_one_nodes), false);
	return true;
}


bool
AosIILTester::verifyLeafParents(
		const vector<u64> &leaf_ids,
		AosU64BoolHash &level_one_map)
{
	// Check whether all leaves have a level one node as its parent.
	// Create a map for all the level one nodes.
	u32 num_iils = leaf_ids.size();
	u64 leaf_parents[num_iils];
	memset(leaf_parents, 0, num_iils*sizeof(u64));
	for (u32 i=0; i<num_iils; i++)
	{
		// Retrieve the leaf's parent node id
		u64 leaf_id = leaf_ids[i];
		u64 parent_id = mTree->getParentId(mRundata, leaf_id);
		aos_assert_r(parent_id > 0, false);

		if (leaf_parents[i] > 0) 
		{
			aos_assert_r(leaf_parents[i] == parent_id, false);
			continue;
		}
		
		level_one_map[parent_id] = true;
		vector<u64> member_ids;
		aos_assert_r(mTree->getMemberIds(mRundata, parent_id, member_ids), false);
		aos_assert_r(member_ids.size() > 0, false);

		// The parent node 'parent_id' has a number of leaves: 'member_ids'. 
		// Mark the corresponding flags based on 'member_ids'. When finishing
		// the loops, all flags should be marked. This means that every leaf
		// is contained in a level one node.
		for (u32 k=0; k<member_ids.size(); k++)
		{
			u64 id = member_ids[k];
			bool found = false;
			for (u32 m=0; m<num_iils; m++)
			{
				if (leaf_ids[m] == id)
				{
					aos_assert_r(leaf_parents[m] == 0, false);
					leaf_parents[m] = parent_id;
					found = true;
					break;
				}
			}
			aos_assert_r(found, false);
		}
		aos_assert_r(leaf_parents[i], false);
	}

	// All leaves should have been parents, this means that all leaves are
	// contained in a level one node.
	for (u32 i=0; i<num_iils; i++)
	{
		aos_assert_r(leaf_parents[i], false);
	}
	return true;
}


bool
AosIILTester::verifyIILs(vector<u64> &leaf_ids)
{
	// Retrieve the IILs
	leaf_ids.clear();
	AosIILIdx idx;
	AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
	aos_assert_r(iilmgr, false);
	AosIILObjPtr iil = iilmgr->firstSubiil(mRundata, mIILName, idx);
	aos_assert_r(iil, false);

	// Step 1: Verify sub-iils and leaf bitmaps.
	// Loop over all the sub-iils. For each sub-iil, verify its contents
	// are correct. In addition, it constructs the bitmap and compares
	// the bitmap with the one created by the IIL. When finishing, all
	// leaf bitmap IDs are collected in 'leaf_ids'.
	mBuffArray->resetRecordLoop();
	while (iil)
	{
		aos_assert_r(verifyOneIIL(iil, leaf_ids), false);
		iil = iilmgr()->nextSubiil(mRundata, mIILName, idx);
	}
	aos_assert_r(!mBuffArray->hasMore(), false);
	return true;
}


bool
AosIILTester::verifyOneIIL(
		const AosIILObjPtr &iil, 
		vector<u64> &leaf_ids)
{
	u32 num_docs = iil->getNumDocs();
	char **keys = iil->getValues();
	u64 *docids = iil->getDocids();
	AosQueryBitmapObjPtr bitmap = AosGetBitmap();
	char *seed;
	u32 seqno;
	for (u32 k=0; k<num_docs; k++)
	{
		const char *key = keys[k];
		u64 docid = docids[k];
		aos_assert_r(mBuffArray->nextValue(&seed, seqno), false);
		aos_assert_r(seed, false);

		aos_assert_r(strcmp(key, getKey(seed, seqno, docid).data()) == 0, false);
		bitmap->appendDocid(docid);
	}

	u64 leaf_id = iil->getIILID();
	u64 parent_id = mTree->getParentId(mRundata, leaf_id);
	AosQueryBitmapObjPtr bb = retrieveLeaf(parent_id, leaf_id);
	aos_assert_r(bb, false);
	aos_assert_r(bb->compareBitmap(bitmap), false);
	leaf_ids.push_back(leaf_id);
	return true;
}


OmnString
AosIILTester::getKey(const char *seed, const u32 seqno, const u64 &docid)
{
	// 'record' is in the format:
	// 		seed + "_" + seqno + "_" + docid
	OmnString kk = seed;
	kk << "_" + seqno << "_" << docid;
	return kk;
}


bool
AosIILTester::verifyTreeStructure()
{
	// 'level_one_nodes' are all the level one nodes contained in 
	// the bitmap tree. This function checks that all the nodes
	// are constructed correctly.
	vector<u64> level_one_nodes;
	aos_assert_r(mTree->getLevelOneNodes(mRundata, level_one_nodes), false);
	vector<u64> running_ids = level_one_nodes;
	AosU64BoolHash map;
	while (running_ids.size() > 1)
	{
		for (u32 i=0; i<running_ids.size(); i++)
		{
			u64 parent_id = mTree->getParentId(mRundata, running_ids[i]);
			aos_assert_r(parent_id > 0, false);
			map[parent_id] = true;
		}

		aos_assert_r(map.size() > 0, false);
		running_ids.clear();
		AosU64BoolHashItr itr = map.begin();
		while (itr != map.end())
		{
			u64 parent_id = itr->first;
			running_ids.push_back(parent_id);
			itr++;
		}
	}

	aos_assert_r(map.size() == 1, false);
	AosU64BoolHashItr itr = map.begin();
	u64 root_id = itr->first;
	u64 rid = mTree->getRootId();
	aos_assert_r(root_id == rid, false);
	return true;
}


bool
AosIILTester::verifyLevelOneNodes(
		AosU64BoolHash &level_one_map, 
		const vector<u64> &level_one_nodes)
{
	// There are two lists of level one nodes:
	// 	'level_one_map': it contains all the level one nodes retrieved
	// 	from leaves.
	// 	'level_one_nodes': it contains all the level one nodes retrieved the tree.
	// These two should be the same.
	if (level_one_map.size() <= 0)
	{
		aos_assert_r(level_one_nodes.size() == 0, false);
		return true;
	}

	for (u32 i=0; i<level_one_nodes.size(); i++)
	{
		u64 node_id = level_one_nodes[i];
		AosU64BoolHashItr itr = level_one_map.find(node_id);
		aos_assert_r(itr != level_one_map.end(), false);
		level_one_map.erase(itr);
	}

	aos_assert_r(level_one_nodes.size() == 0, false);
	aos_assert_r(level_one_map.size() == 0, false);
	return true;
}


int
AosIILTester::getLeafIdx(
		const vector<u64> &leaf_bitmap_ids, 
		const u64 &leaf_id)
{
	for (u32 m=0; m<leaf_bitmap_ids.size(); m++)
	{
		if (leaf_bitmap_ids[m] == leaf_id) return m;
	}
	return -1;
}


bool
AosIILTester::getLevelOneNodes(vector<u64> &level_one_nodes)
{
	level_one_nodes.clear();
	queue<u64> running_ids;
	u64 root_id = mTree->getRootId();
	aos_assert_r(root_id > 0, false);

	running_ids.push(root_id);
	int level = mTree->getNodeLevel(mRundata, root_id);
	if (level == 0)
	{
		return true;
	}

	while (running_ids.size() > 0)
	{
		u64 node_id = running_ids.front();
		running_ids.pop();
		vector<u64> member_ids;
		int level = mTree->getNodeLevel(mRundata, node_id);
		aos_assert_r(level >= 1, false);
		if (level == 1)
		{
			level_one_nodes.push_back(node_id);
			continue;
		}

		mTree->getMemberIds(mRundata, node_id, member_ids);
		for (u32 i=0; i<member_ids.size(); i++)
		{
			running_ids.push(member_ids[i]);
		}
	}

	return true;
}


bool 
AosIILTester::verifyAddBits()
{
	// Verify Adding Bits
	// Since last call of adding contents, it caused a series
	// of bits being added to leaves. Each should climb up 
	// along the tree path to add the same bits to its parent
	// nodes. 
	//
	// Check 1:
	// Create a bitmap B1 that are the OR of the addBits(...) 
	// for leaves. Create another bitmap B2 that is the results
	// of the last addContents(...) function call. B1 should equal
	// to B2.

	// When adding contents, it causes a series of 'addBits(...)'
	// to the executor.
	vector<AosQueryBitmapObjPtr> leaf_add_bits;
	aos_assert_r(mExecutor->getLeafAddBits(mRundata, leaf_add_bits), false);
	vector<AosQueryBitmapObjPtr> temp_bits;
	aos_assert_r(mExecutor->getTempAddBits(mRundata, temp_bits), false);
	aos_assert_r(leaf_add_bits.size() > 0, false);
	AosQueryBitmapObjPtr bitmap = AosGetBitmap();
	for (u32 i=0; i<leaf_add_bits.size(); i++)
	{
		bitmap->orBitmap(leaf_add_bits[i]);
	}

	for (u32 i=0; i<temp_bits.size(); i++)
	{
		bitmap->orBitmap(temp_bits[i]);
	}

	aos_assert_r(bitmap->compareBitmap(mLastBitmap), false);
	return true;
}


bool
AosIILTester::verifyAddBitsPath()
{
	// For each leaf addBits(...) instruction, it should have an
	// addBits(...) instruction to all its parent nodes. 
	vector<AosBitmapInstr> instructions;
	aos_assert_r(mExecutor->getLeafAddBits(mRundata, instructions), false);
	aos_assert_r(instructions.size() > 0, false);
	for (u32 i=0; i<instructions.size(); i++)
	{
		// This is a leaf add bits. It should have an instruction going 
		// all the way to the root.
		u64 bitmap_id = instructions[i].getBitmapId();
		u64 child_id = instructions[i].getNodeId();
		
		while (!mTree->isRoot(child_id))
		{
			u64 parent_id = mTree->getParentId(mRundata, child_id);
			aos_assert_r(parent_id > 0, false);
			aos_assert_r(checkAddBits(parent_id, bitmap_id), false);
			child_id = parent_id;
		}
	}

	return true;
}


bool
AosIILTester::checkAddBits(
		const u64 &parent_id, 
		const u64 &bitmap_id)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosIILTester::verifyRebuildNodes()
{
	return true;
}


bool
AosIILTester::verifyCreateBitmaps()
{
	// For every bitmap (either leaf or temporary), there must be an instruction
	// to create it. It retrieves all the 'addBits' instructions.
	// For each instruction, there must be an instruction to create its bitmap.

	// Step 1: Retrieve all the create bitmap instructions. 
	// It checks each instruction creates one and only one bitmap 
	// (either leaf or temporary).
	//
	// It then creates a map that maps bitmap IDs to the instructions.
	vector<AosBitmapInstr> create_instrs;
	vector<AosBitmapInstr>::iterator create_instrs_itr;
	aos_assert_r(mExecutor->getCreateInstrs(mRundata, create_instrs), false);
	imap_t create_map;
	for (u32 i=0; i<create_instrs.size(); i++)
	{
		u64 bitmap_id = create_instrs[i].getBitmapId();
		imapitr_t itr = create_map.find(bitmap_id);
		aos_assert_r(itr == create_map.end(), false);
		AosBitmapInstr instr = create_instrs[i];
		create_map[bitmap_id] = instr;
	}

	// Step 2: for each add bit instruction, there shall be an instruction
	// to create the bitmap.
	vector<AosBitmapInstr> add_bits_instrs;
	aos_assert_r(mExecutor->getAddBits(mRundata, add_bits_instrs), false);
	for (u32 i=0; i<add_bits_instrs.size(); i++)
	{
		u64 bitmap_id = add_bits_instrs[i].getBitmapId();
		imapitr_t itr = create_map.find(bitmap_id);
		aos_assert_r(itr != create_map.end(), false);
	}

	return true;
}


AosQueryBitmapObjPtr 
AosIILTester::retrieveLeaf(
		const u64 &parent_id, 
		const u64 &leaf_id)
{
	AosBuffPtr buff = AosGetBitmapStorageMgr()->retrieveLeaf(
			mRundata, leaf_id, parent_id);
	AosQueryBitmapObjPtr bitmap = AosGetBitmap();
	bitmap->loadFromBuff(buff);
	return bitmap;
}

