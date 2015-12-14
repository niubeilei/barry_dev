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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/Bitmap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/BitmapMgr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include <boost/pool/pool.hpp>

#define BITMAP_FILTER(a,b) (((a) & smMasks[(b)->mNodeLevel]) ^ (b)->mFilter)


static OmnMutex sgLock;
static int64_t sgTotalNumInstances;
static int64_t sgActiveNumInstances;

u64	AosBitmap::smMasks[8] = {0,0,0,0,0,0,0,0};

static u8 countU16Bit(const u16 number)
{
	u16 num = number;
	int count = 0;
	for(int i = 0;i < 16;i ++)
	{
		count += (num & 1);
		num = num >> 1;
	}
	return count;
}

static int sgInit = false;
static void initStatic()
{
	for(int i = 0;i < 65536;i++)
	{
		int j = i;
		int k = 0;
		while(j)
		{
			if((j&1) != 0)
			{
				break;
			}
			j = j >> 1;
			k++;
		} 
		AosBitmapNode::smBitFindingList[i] = k;

		int x = i;
		int y = -1;
		while(x)
		{
			x = x >> 1;
			y ++;
		}
		if(y>= 0)
		{
			AosBitmapNode::smBitRevFindingList[i] = y;
    	}		
			
		AosBitmapNode::smBitCountList[i] = countU16Bit(i);
	}
	AosBitmapNode::smBitFindingList[0] = 0;
	AosBitmapNode::smBitRevFindingList[0] = 0;
		
	AosBitmap::smMasks[0] = 0xffffffffffffc000ULL;//14 bit mask
	AosBitmap::smMasks[1] = 0xffffffffffc00000ULL;//22 bit mask
	AosBitmap::smMasks[2] = 0xffffffffc0000000ULL;//30 bit mask
	AosBitmap::smMasks[3] = 0xffffffc000000000ULL;//38 bit mask
	AosBitmap::smMasks[4] = 0xffffc00000000000ULL;//46 bit mask
	AosBitmap::smMasks[5] = 0xffc0000000000000ULL;//54 bit mask
	AosBitmap::smMasks[6] = 0xc000000000000000ULL;//62 bit mask 11000.........
	AosBitmap::smMasks[7] = 0ULL;
}

AosBitmap::AosBitmap()
:
mNode(0),
mMask(0),
mFilter(0),
mOpr(eAnd),
mSectionId(0),
mBitmapId(0),
mModifyId(0),
mIILLevel(-1),
mNodeLevel(-1),
mMemorySize(0),
mNumBits(0),
mCacheSize(1),
mCrtCachePos(0),
mNodeCache(0),
mLock(OmnNew OmnMutex())
{
	sgLock.lock();
	sgTotalNumInstances++;
	sgActiveNumInstances++;
/*	if (sgTotalNumInstances % 10000 == 0)
	{
		OmnScreen << "----------Num Bitmaps: " << sgTotalNumInstances 
			<< ":" << sgActiveNumInstances << endl;
	}
*/	sgLock.unlock();

    if (!sgInit)
	{
		sgInit = true;
		initStatic();
	}
}

AosBitmap::~AosBitmap()
{
	sgLock.lock();
	sgActiveNumInstances--;
	sgLock.unlock();

	if(mNode)
	{
		returnNode(mNode);
	}
	
	mBuff = 0;
	
	if(mNodeCache)
	{
		AosBitmapMgr::returnNodes(&(mNodeCache[mCrtCachePos]), mCacheSize - mCrtCachePos);
		delete []mNodeCache;
		mNodeCache = 0;
	}
	
}
	
		
void 
AosBitmap::appendDocidsToBuff(
		const u64* docs,
		const i64 &num_docs)
{
	clear();
	vector<u64> v;
	v.resize(num_docs*sizeof(u64));
	memcpy(   &(v[0]), docs, num_docs*sizeof(u64));
	std::sort(v.begin(), v.begin()+num_docs);
	
	u64 start = 0;
	u64 filter = v[0] & 0xffffffffffffc000ULL;
	u64 end = 0;
	
	char *value_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];

	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);	
	AosBitmapNode* cur_node = getNode(0);
	u64 pos = 0;
	for(i64 i = 1;i<num_docs;i++)
	{
		if(filter < (v[i] & 0xffffffffffffc000ULL))
		{
			// calc the node before
			end = i-1;
			if(start == end)
			{
				mBuff.getPtrNoLock()->setU8(2);
				mBuff.getPtrNoLock()->setU64(v[i]);
			}	
			else
			{
				cur_node->cleanContent(this);
				cur_node->mFilter = v[start] & 0xffffffffffffc000ULL;
				
				for(pos = start; pos <= end;pos++)
				{
					if(cur_node->mBitmap[(u8)( v[pos]>>6 )] & 1ULL << (v[pos] & 0x3f))
					{
					}
					else
					{
						cur_node->mBitmap[(u8)( v[pos]>>6 )] |= 1ULL << (v[pos] & 0x3f);
						cur_node->mNodeNum ++;
					}
				}
				cur_node->saveToBuff(mBuff,value_buff);				
			}		
			
			start = i;
			filter = v[i] & 0xffffffffffffc000ULL;
		}
	}	
	
	mBuff.getPtrNoLock()->setU8(0xff);
	if(cur_node)
	{
		returnNode(cur_node);
		cur_node = 0;
	}
	return;
}


void 
AosBitmap::appendDocids(
		const u64* docs,
		const i64 &num_docs)
{
	mLock->lock();
	expand();

	i64 i = 0;

	for (;i < num_docs;i++)
	{
		appendDocid(docs[i]);
	}
	
	
	
	mNumBits = 0;
	pack();
	mLock->unlock();
}

bool 
AosBitmap::checkDoc(const u64 docid)
{	
	expand();
	// This function checks whether 'docid' exists in this bitmap.
	if (!mNode) return false;

	if ((docid & mMask) != mFilter) return false;

	u64 value = docid >> 6;
	AosBitmapNode* cur_node = mNode;
	u8 expect_level = mNode->mNodeLevel;
	while(cur_node)
	{
		if(cur_node->mNodeLevel != expect_level)
		{
			if(BITMAP_FILTER(docid,cur_node))
			{
				return false;
			}
		}
		if(cur_node->mNodeLevel == 0)
		{
			return cur_node->mBitmap[((u8*)(&value))[0]] & (1ULL << (docid & 0x3f));
		}
		expect_level = cur_node->mNodeLevel -1;
		cur_node = cur_node->mRsltmap[((u8*)(&value))[cur_node->mNodeLevel]];
	}
	return false;
}

i64  
AosBitmap::getDocidCount()
{
	if(mNumBits)
	{
		return mNumBits;
	}

	expand();

	if(!mNode)
	{
		return 0;
	}
	
	i64 count = 0;
	AosBitmapNode * node;
	
	reset();
	while ((node=getNextLeafNode()) != NULL)
	{
		count += node->mNodeNum;
	}
	mNumBits = count;	
	return count;
}

void 
AosBitmap::appendDocid(const u64 docid)
{
	// This function sets the bit 'docid' to this bitmap.
	expand();
	mNumBits = 0;
	if (!mNode)
	{
		// The bitmap is empty now. Need to create a node.
		mMask = smMasks[0];
 		mFilter = docid & smMasks[0];

		mNode = getNode(0);
		mNode->mBitmap[(u8)(docid >> 6)] = (1ULL << (docid & 0x3f));
		mNode->mFilter = mFilter;
		mNode->mNodeNum = 1;
	
		return;
	}
	
	//if doc id is not in the level's region
	if((docid & mMask) != mFilter)
	{
		// new node
		u64 diff = BITMAP_FILTER(docid,mNode) >> 6;
		u8 first_level = 7;
		while(((u8*)(&diff))[first_level] == 0)
		{
			first_level --;
		}
//		first_level ++;
		// create new root node
		AosBitmapNode* new_node = getNode(first_level);
		
		// place old node
		diff = mFilter >> 6;
		new_node->mRsltmap[((u8*)(&diff))[first_level]] = mNode;
		
		// set new root		
		mNode = new_node;
		
		mFilter = docid & smMasks[first_level];

		mMask = smMasks[first_level];
		mNode->mFilter = mFilter;
		mNode->mNodeNum = 2;
			
		// create new leaf
		diff = docid >> 6;
		new_node = getNode(0);
		new_node->mNodeNum = 1;

		new_node->mFilter = docid & smMasks[0];

		new_node->mBitmap[(u8)diff] = 1ULL << (docid & 0x3f);
		mNode->mRsltmap[((u8*)(&diff))[first_level]]= new_node;	
		
		return;
	}

	u64 index = docid >> 6;

	if(mNode->mNodeLevel == 0)
	{
		// set into this node
		if(mNode->mBitmap[(u8)(index)] & 1ULL << (docid & 0x3f))
		{
			return;
		}
		mNode->mBitmap[(u8)(index)] |= 1ULL << (docid & 0x3f);
		mNode->mNodeNum ++;

		return;
	}

	// add docid into mNode
	AosBitmapNode* child_node = mNode;
	AosBitmapNode* parent_node = mNode;
//	u8 cur_level = mNode->mNodeLevel -1;
	while(1)
	{
		// child_node is not leaf
		child_node = parent_node->mRsltmap[((u8*)(&index))[parent_node->mNodeLevel]];
		// no node in the right place, create a level 0 node to be the child_node
		if(!child_node)
		{
			AosBitmapNode* new_node = getNode(0);
			new_node->mNodeNum = 1;

			new_node->mFilter = docid & smMasks[0];

			new_node->mBitmap[(u8)index] = 1ULL << (docid & 0x3f);
			parent_node->mRsltmap[((u8*)(&index))[parent_node->mNodeLevel]] = new_node;
			parent_node->mNodeNum ++;

			return;
		}
		// find the child_node but not what we need
		if(BITMAP_FILTER(docid,child_node))
		{
			// here we met some problem, now we need to 
			// 1. create a middle node, put the middle node under parent_node
			// 2. put the child_node under the middle node
			// 3. create a new level 0 node of docid to be under the middle one
			
			// 1. create a middle node, put the middle node under parent_node
			u64 diff = BITMAP_FILTER(docid,child_node) >> 6;
			u8 mid_level = 6;
			while(((u8*)(&diff))[mid_level] == 0)
			{
				mid_level --;
			}
			AosBitmapNode* new_node = getNode(mid_level);
			parent_node->mRsltmap[((u8*)(&index))[parent_node->mNodeLevel]] = new_node;

			new_node->mFilter = docid & smMasks[mid_level];
			new_node->mNodeNum = 2;

			// 2. put the child_node under the middle node
			diff = child_node->mFilter >> 6;
			new_node->mRsltmap[((u8*)(&diff))[mid_level]] = child_node;
			// 3. create a new level 0 node of docid to be under the middle one
			parent_node = new_node;
			new_node = getNode(0);
			parent_node->mRsltmap[((u8*)(&index))[mid_level]] = new_node;
			new_node->mNodeNum = 1;			
			new_node->mFilter = docid & smMasks[0];
			new_node->mBitmap[(u8)index] = 1ULL << (docid & 0x3f);
		
			return;
		}
		
		
		if(child_node->mNodeLevel == 0)
		{
			// set into this node
			if(child_node->mBitmap[(u8)(index)] & 1ULL << (docid & 0x3f))
			{
				return;
			}
			child_node->mBitmap[(u8)(index)] |= 1ULL << (docid & 0x3f);
			child_node->mNodeNum ++;
			return;
		}
		parent_node = child_node;
	}
}


void 
AosBitmap::removeDocid(const u64 docid)
{
	expand();
	mNumBits = 0;
	if(!mNode)return;
	if((docid & mMask) != mFilter) return;
	u64 value = docid >> 6;
	AosBitmapNode* cur_node = mNode;
	u8 expect_level = mNode->mNodeLevel;
	while(1)
	{
		if(cur_node->mNodeLevel != expect_level)
		{
			if(BITMAP_FILTER(docid,cur_node))
			{
				return;
			}
		}
		if(cur_node->mNodeLevel == 0)
		{
			break;
		}
		expect_level = cur_node->mNodeLevel -1;
		cur_node = cur_node->mRsltmap[((u8*)(&value))[expect_level+1]];
		if(!cur_node)return;
	}

	if(!( cur_node->mBitmap[((u8*)(&value))[0]] & (1ULL << (docid & 0x3f))))
	{
		// this bit is not 1, 
		return;
	}

	// found this bit
	cur_node->mBitmap[((u8*)(&value))[0]] ^= (1ULL << (docid & 0x3f));
	cur_node->mNodeNum --;

	if(cur_node->mNodeNum)
	{
		// the node is not empty
		return;
	}
	
	// merge node
//	cur_node->mNodeNum ++;

	// there are some nodes we need to remove
	// 1. get the node chain
	cur_node = mNode;
	AosBitmapNode*  nodes[8];
	u8 node_num = 1;
	nodes[0] = mNode;
	while(1)
	{
		if(cur_node->mNodeLevel == 0)
		{
			break;
		}
		expect_level = cur_node->mNodeLevel -1;
		cur_node = cur_node->mRsltmap[((u8*)(&value))[expect_level+1]];
		if(!cur_node)break;
		nodes[node_num++] = cur_node;
	}
	node_num --;

	// 2. check all the nodes if they should be removed
	u16 newnum = 0;
	while(node_num >= 0)
	{
		newnum = nodes[node_num]->mNodeNum;
		if(newnum > 1)return;
		if(newnum == 0)
		{
			// if not root node, remove from the parent node
			returnNode(nodes[node_num]);

			if(node_num > 0)
			{
				nodes[node_num-1]->mNodeNum --;
				nodes[node_num-1]->mRsltmap[((u8*)(&value))[(nodes[node_num-1]->mNodeLevel)]] = 0;
			}else // root node
			{
				mNode = 0;
				mMask = 0;
				mFilter = 0;
				return;
			}
		}
		else// newnum == 1
		{
			// should not be leaf node, cause only null leaf node can run here
			AosBitmapNode* cur_node = nodes[node_num];
			// find the last one, replace itself,then return
			for(u16 i = 0;i < 0xff;i++)
			{
				if(cur_node->mRsltmap[i])
				{
					// if current node is the mNode, .......
					if(node_num == 0)
					{
						mNode = cur_node->mRsltmap[i];
						mMask = smMasks[mNode->mNodeLevel];
						mFilter = mNode->mFilter;
						
					}
					else
					{
						nodes[node_num -1]->mRsltmap[ ((u8*)(&value))[  nodes[node_num -1]->mNodeLevel] ] 
							= cur_node->mRsltmap[i];
					}
					cur_node->mRsltmap[i] = 0;
					cur_node->mNodeNum = 0;

					returnNode(nodes[node_num]);
					return;
				}
			}
			// should never come here
			return;
		}
		node_num --;	
	}
	
	return;	
}


void
AosBitmap::saveToBuff(
		const AosBuffPtr &buff, 
		const bool reset_flag)
{
	mLock->lock();
	// for length, will update later.
	u64 orig_pos = buff->getCrtIdx();
	
	buff.getPtrNoLock()->setU64(0);

	buff.getPtrNoLock()->setU64(mSectionId);
	buff.getPtrNoLock()->setU64(mBitmapId);
	buff.getPtrNoLock()->setU64(mModifyId);
	buff.getPtrNoLock()->setInt(mIILLevel);
	buff.getPtrNoLock()->setInt(mNodeLevel);

	if(mBuff)
	{
		
		buff->setBuff(mBuff);
	}
	else
	{
		packToBuff(buff);
	}
	
	u64 new_pos = buff->getCrtIdx();
	u64 length = new_pos - orig_pos - sizeof(u64);
	buff.getPtrNoLock()->setCrtIdx(orig_pos);
	buff.getPtrNoLock()->setU64(length);
	buff.getPtrNoLock()->setCrtIdx(new_pos);
	
	if (reset_flag) buff->reset();
	mLock->unlock();
	return;
}


bool
AosBitmap::pack()
{
	if(!mBuff)
	{
		if(mNode)
		{
			AosBuffPtr new_buff =  OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			packToBuff(new_buff);
			returnNode(mNode);
			mBuff = new_buff;
			mNode = 0;
		}
	}
	return true;
}

void
AosBitmap::packToBuff(
		const AosBuffPtr &buff)
{
	// This function save the docids to buff;
	aos_assert(buff);
	reset();
	
	
	char *value_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];


	AosBitmapNode* cur_node = 0;
	while((cur_node=getNextLeafNode()))
	{
		cur_node->saveToBuff(buff,value_buff);
	}
	
	buff.getPtrNoLock()->setU8(0xff);

	delete []value_buff;
	return;
}


bool	
AosBitmap::loadFromBuff(
		const AosBuffPtr &buff, 
		const bool reset_flag)
{
	mLock->lock();
	cleanPriv();	
	aos_assert_r(buff, false);
	if (reset_flag) buff->reset();
	u64 length		= buff->getU64(0);

	mSectionId		= buff->getU64(0);
	mBitmapId		= buff->getU64(0);
	mModifyId		= buff->getU64(0);
	mIILLevel		= buff->getInt(0);
	mNodeLevel		= buff->getInt(0);

	length -= (sizeof(u64)*3);
	length -= (sizeof(int)*2);
	// copy to mBuff
	mBuff = OmnNew AosBuff(buff->getCrtPos(), length,length,true AosMemoryCheckerArgs);	
	buff->setCrtIdx(buff->getCrtIdx()+length);
	mLock->unlock();
	return true;
}

bool	
AosBitmap::expand()	
{
	if(mBuff)
	{
		mBuff->reset();
		aos_assert_r(!mNode,false);
		expand(mBuff);
		mBuff = 0;
	}
	return true;
}

bool	
AosBitmap::expand(const AosBuffPtr &buff)	
{
	// Chen Ding, 2013/06/27
	bool rslt = true;
	char *load_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];
	AosBitmapNode* cur_node = 0;

	while(rslt)
	{
		cur_node = getNode(0); 
		
		rslt = AosBitmapNode::loadFromBuff(buff,cur_node,load_buff);
		if(!rslt)
		{
			returnNode(cur_node);
			break;
		}
		addLeafNode(cur_node);			
	}	
		
	if(load_buff)
	{
		delete []load_buff;
		load_buff = 0;
	}
	return true;
	
}
	
void 
AosBitmap::addLeafNodeCopy(AosBitmapNode* leaf_node)
{
	expand();
	AosBitmapNode* new_node = getNode(0);
	AosBitmapNode::clone(new_node,leaf_node);
	addLeafNode(new_node);
}

void 
AosBitmap::addLeafNode(AosBitmapNode* leaf_node)
{
	mNumBits = 0;
	u64 leafid = leaf_node->mFilter;
	
	if(!mNode)
	{
		mNode = leaf_node;
		mMask = smMasks[0];
		mFilter = leafid;
		return;
	}
	
	if((leafid & mMask) != mFilter)
	{
		// new node
		u64 diff = BITMAP_FILTER(leafid, mNode) >> 6;
		u8 first_level = 7;
		while(((u8*)(&diff))[first_level] == 0)
		{
			first_level --;
		}

		// create new root node
		AosBitmapNode* new_node = getNode(first_level);
		

		// place old node
		diff = mFilter >> 6;
		new_node->mRsltmap[((u8*)(&diff))[first_level]] = mNode;
		
		// set new root
		mNode = new_node;
		mFilter = leafid & smMasks[first_level];
		mMask = smMasks[first_level];
		mNode->mFilter = mFilter;
		mNode->mNodeNum = 2;

		// create new leaf
		diff = leafid >> 6;
		mNode->mRsltmap[((u8*)(&diff))[first_level]]= leaf_node;

		return;
	}
	
	// add docid into mNode
	AosBitmapNode* child_node = mNode;
	AosBitmapNode* parent_node = mNode;
	u8 cur_level = mNode->mNodeLevel;
	u64 index = leafid >> 6;
	while(1)
	{
		// if the node is there, merge to the original node
		if(child_node->mNodeLevel == 0)
		{
			mergeNode(child_node,leaf_node);

			returnNode(leaf_node);
			return;
		}
		// child_node is not leaf
		child_node = parent_node->mRsltmap[((u8*)(&index))[cur_level]];
		// no node in the right place, create a level 0 node to be the child_node
		if(!child_node)
		{
			parent_node->mRsltmap[((u8*)(&index))[cur_level]] = leaf_node;
			parent_node->mNodeNum ++;
			return;
		}

		// find the child_node but not what we need
		if(BITMAP_FILTER(leafid,child_node))
		{
			// here we met some problem, now we need to 
			// 1. create a middle node, put the middle node under parent_node
			// 2. put the child_node under the middle node
			// 3. create a new level 0 node of docid to be under the middle one
			
			// 1. create a middle node, put the middle node under parent_node
			u64 diff = BITMAP_FILTER(leafid,child_node) >> 6;
			u8 mid_level = 7;
			while(((u8*)(&diff))[mid_level] == 0)
			{
				mid_level --;
			}
//			mid_level ++;			
			AosBitmapNode* new_node = getNode(mid_level);
			parent_node->mRsltmap[((u8*)(&index))[cur_level]] = new_node;
			new_node->mFilter = leafid & smMasks[mid_level];
			new_node->mNodeNum = 2;
			
			// 2. put the child_node under the middle node
			diff = child_node->mFilter >> 6;
			new_node->mRsltmap[((u8*)(&diff))[mid_level]] = child_node;
			// 3. create a new level 0 node of docid to be under the middle one
			parent_node = new_node;
			//diff = leafid >> 6;
			parent_node->mRsltmap[((u8*)(&index))[mid_level]] = leaf_node;

			return;			
		}
		parent_node = child_node;
		cur_level = parent_node->mNodeLevel;
//		aos_assert_r(child_node->mNodeLevel != 0,false);

	}
	
	return;
}

void
AosBitmap::mergeNode(AosBitmapNode* node_a,AosBitmapNode* node_b)
{
	// assume the two node are the same place level 0 nodes
	u16 num = 0;
	for(u32 i = 0;i <= 0xff;i++)
	{
		node_a->mBitmap[i] |= node_b->mBitmap[i];
		if(node_a->mBitmap[i])
		{
			num += AosBitmapNode::countBit(node_a->mBitmap[i]);
		}
	}
	node_a->mNodeNum = num;
	return;
}

bool 
AosBitmap::lastDocid(u64 &docid)
{	
	expand();
	if(!mNode) return false;
	mItr = OmnNew AosBitmapItr();
	mItr->mNodes[0] = mNode;
	return mItr->lastDoc(docid);	
}



bool
AosBitmap::nextDocid(u64 &docid)
{	
	expand();
	if (!mNode) return false;
	if (!mItr)
	{
		mItr = OmnNew AosBitmapItr();
		mItr->mNodes[0] = mNode;
	}
	return mItr->nextDocid(docid);
}

bool
AosBitmap::prevDocid(u64 &docid)
{	
	expand();
	if (!mNode) return false;
	if (!mItr)
	{
		mItr = OmnNew AosBitmapItr();
		mItr->mNodes[0] = mNode;
	}
	return mItr->prevDocid(docid);
}


AosBitmapNode* 
AosBitmap::getLeafNode(const u64 &filter)
{
	expand();
    if(!mNode)return 0;
	if((filter & mMask) != mFilter) return 0;
	u64 value = filter >> 6;
	AosBitmapNode* cur_node = mNode;
	u8 expect_level = mNode->mNodeLevel;
	
	while(cur_node)	
    {
		if(cur_node->mNodeLevel != expect_level)
		{
			if(BITMAP_FILTER(filter,cur_node))
			{
				return 0;
			}
		}
		if(cur_node->mNodeLevel == 0)
		{            
			return cur_node;
		}		
        expect_level = cur_node->mNodeLevel -1;
		cur_node = cur_node->mRsltmap[((u8*)(&value))[cur_node->mNodeLevel]];
	}
	return 0;
}



void 
AosBitmap::clean()
{
	mLock->lock();
	cleanPriv();
	mLock->unlock();
}

void 
AosBitmap::cleanPriv()
{
	mItr = 0;
	if(mNode)
	{
		returnNode(mNode);
		mNode = 0;
	}
	mNumBits = 0;
	mOpr = eAnd;
	mBuff = 0;
}


// Added by Navy
bool 
AosBitmap::checkAndBitmap(
				const AosBitmapObjPtr &a,
	 			const AosBitmapObjPtr &b)
{
	if ( a.getPtr() == b.getPtr())
	{
		a->lock();
		bool rslt = (a->getNumBits() > 0);
		a->unlock();
		return rslt;
	}
	
	a->lock();
	b->lock();
	if(a->isPacked() || b->isPacked())
	{
		a->pack();
		b->pack();
		bool rslt = checkAndBitmapPacked(a,b);
		a->unlock();
		b->unlock();
		return rslt;
	}
	a->expand();
	b->expand();
		

	aos_assert_r(a, 0);
    aos_assert_r(b, 0);

	a->reset();
	b->reset();	

	AosBitmapNode* node1 = a->getNextLeafNode();
	AosBitmapNode* node2 = b->getNextLeafNode();
	AosBitmapNode* cur_node = getNode(0);			  
	while(node1 && node2)
	{		
		if(node1->mFilter == node2->mFilter)
		{						
			// proc node1 && node2			
		    AosBitmapNode::countAnd(cur_node,node1,node2);			
			if (cur_node->mNodeNum)
		    {   
				returnNode(cur_node);
				a->unlock();
				b->unlock();
				return true;
			}			
			node1 = a->getNextLeafNode();
			node2 = b->getNextLeafNode();
		}
		else if(node1->mFilter > node2->mFilter)
		{
			node2 = b->getNextLeafNode();
		}
		else // (node1->mFilter < node2->mFilter)
		{
			node1 = a->getNextLeafNode();			
		}
	}
	
	returnNode(cur_node);
	a->unlock();
	b->unlock();
	return false;
}

AosBitmapObjPtr 
AosBitmap::countAnd(
		const AosBitmapObjPtr &a, 
		const AosBitmapObjPtr &b)
{	
	if ( a.getPtr() == b.getPtr()) 
	{
		return a->cloneSafe(0);
	}
	
	a->lock();
	b->lock();
	if(a->isPacked() || b->isPacked())
	{
		a->pack();
		b->pack();
		AosBitmapObjPtr rslt = countAndPacked(a,b);
		a->unlock();
		b->unlock();
		return rslt;
	}
	a->expand();
	b->expand();

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
	
	aos_assert_r(a, 0);
    aos_assert_r(b, 0);

	a->reset();
	b->reset();	

	AosBitmapNode* node1 = a->getNextLeafNode();
	AosBitmapNode* node2 = b->getNextLeafNode();	
	AosBitmapNode* cur_node = bitmap->getNode(0);
	while(node1 && node2)
	{		
		if(node1->mFilter == node2->mFilter)
		{						
			// proc node1 && node2			
		    AosBitmapNode::countAnd(cur_node,node1,node2);			
			if (cur_node->mNodeNum)
		    {   
				 //OmnScreen << " orignal:node1->mFilter="<<node1->mFilter<<endl;
				 bitmap->addLeafNode(cur_node);
				 cur_node = bitmap->getNode(0);			  
			}			
			node1 = a->getNextLeafNode();
			node2 = b->getNextLeafNode();
		}
		else if(node1->mFilter > node2->mFilter)
		{
			node2 = b->getNextLeafNode();
		}
		else // (node1->mFilter < node2->mFilter)
		{
			node1 = a->getNextLeafNode();			
		}
	}
	
	if(cur_node)
	{
		bitmap->returnNode(cur_node);
		cur_node = 0;
	}
	a->unlock();
	b->unlock();
	return bitmap;
}

bool
AosBitmap::andBitmap(const AosBitmapObjPtr &bitmap)
{
	if (this == bitmap.getPtr()) return true;

	lock();
	bitmap->lock();
	
	mNumBits = 0;
	if(isPacked() || bitmap->isPacked())
	{
		pack();
		bitmap->pack();		
		bool rslt = andBitmapPacked(bitmap);
		unlock();
		bitmap->unlock();
		return rslt;
	}
	expand();
	bitmap->expand();


	AosBitmapObjPtr bitmap2 = AosGetBitmap(); 
	exchangeContent(this,bitmap2);

	bitmap->reset();
	bitmap2->reset();
	AosBitmapNode * node1   = bitmap->getNextLeafNode();
	AosBitmapNode * node2   = bitmap2->getNextLeafNode();

	AosBitmapNode* cur_node = getNode(0);
	while(node1 && node2)
	{		
		if(node1->mFilter == node2->mFilter)
		{						
			// proc node1 && node2			
		    AosBitmapNode::countAnd(cur_node,node1,node2);			
			if (cur_node->mNodeNum)
		    {   
				 //OmnScreen << " orignal:node1->mFilter="<<node1->mFilter<<endl;
				 addLeafNode(cur_node);
				 cur_node = getNode(0);			  
			}			
			node1 = bitmap->getNextLeafNode();
			node2 = bitmap2->getNextLeafNode();
		}
		else if(node1->mFilter > node2->mFilter)
		{
			node2 = bitmap2->getNextLeafNode();
		}
		else // (node1->mFilter < node2->mFilter)
		{
			node1 = bitmap->getNextLeafNode();			
		}
	}
	
	if(cur_node)
	{
		returnNode(cur_node);
		cur_node = 0;
	}
	unlock();
	bitmap->unlock();
	return true;
}

bool
AosBitmap::appendBitmap(const AosBitmapObjPtr &b)
{
	return orBitmap(b);
}

AosBitmapObjPtr 
AosBitmap::countOr(const AosBitmapObjPtr &a, const AosBitmapObjPtr &b)
{
	if (a.getPtr() == b.getPtr()) 
	{
		a->lock();
		AosBitmapObjPtr rslt = a->clone(0);
		a->unlock();
		return rslt;
	}
	
	a->lock();
	b->lock();

	if(a->isPacked() || b->isPacked())
	{
		a->pack();
		b->pack();
		AosBitmapObjPtr rslt = countOrPacked(a,b);
		a->unlock();
		b->unlock();
		return rslt;
	}
	a->expand();
	b->expand();


	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
	
	a->reset();
	b->reset();
	AosBitmapNode* node1 = a->getNextLeafNode();
	AosBitmapNode* node2 = b->getNextLeafNode();
	AosBitmapNode* cur_node = bitmap->getNode(0);

	while(node1 || node2)
	{
		if(!node1)
		{
			// proc node2
			bitmap->addLeafNodeCopy(node2);
			node2 = b->getNextLeafNode();
			continue;
		}
		
		if(!node2)
		{
			// proc node1
			bitmap->addLeafNodeCopy(node1);
			node1 = a->getNextLeafNode();
			continue;
		}
		
		if(node1->mFilter == node2->mFilter)
		{
			// proc node1 || node 2
			AosBitmapNode::countOr(cur_node,node1,node2);
			bitmap->addLeafNode(cur_node);
			cur_node = bitmap->getNode(0);
			node1 = a->getNextLeafNode();
			node2 = b->getNextLeafNode();
		}
		else if(node1->mFilter > node2->mFilter)
		{
			// proc node2
			bitmap->addLeafNodeCopy(node2);
			node2 = b->getNextLeafNode();
		}
		else // (node1->mFilter < node2->mFilter)
		{
			// proc node1
			bitmap->addLeafNodeCopy(node1);
			node1 = a->getNextLeafNode();
		}
	}

	if(cur_node)
	{
		bitmap->returnNode(cur_node);
		cur_node = 0;
	}
	a->unlock();
	b->unlock();
	return bitmap;
}


void
AosBitmap::getTypeFilter(AosBuff* buff,u8 &type, u64 &filter)
{
	type = buff->getU8(0xff);
	if(type!= 0xff)
	{
		filter = buff->getU64(0);
	}
}

AosBitmapObjPtr 
AosBitmap::countOrPacked(const AosBitmapObjPtr &a, const AosBitmapObjPtr &b)
{
	if (a.getPtr() == b.getPtr()) return a->clone(0);
	
	a->resetBuff();
	b->resetBuff();
	

	AosBuffPtr buff1 = a->getBuff();
	AosBuffPtr buff2 = b->getBuff();
	AosBuff* rbuff1 = buff1.getPtr();
	AosBuff* rbuff2 = buff2.getPtr();

	// case 1. buff1 is null
	// case 2. buff2 is null
	if(!rbuff1)
	{
		return b->clone(0);
	}
	
	if(!rbuff2)
	{
		return a->clone(0);
	}

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();	
	AosBuffPtr buff0 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosBuff* rbuff0 = buff0.getPtr();
	bitmap->setBuff(buff0);
	
	
	u8 type1 = 0;
	u8 type2 = 0;
	u64 filter1 = 0;
	u64 filter2 = 0;
	
	getTypeFilter(rbuff1,type1, filter1);
	getTypeFilter(rbuff2,type2, filter2);
	aos_assert_r(type1 != 0xff,0);
	aos_assert_r(type2 != 0xff,0);

	AosBitmapNode* node1 = bitmap->getNode(0); 
	AosBitmapNode* node2 = bitmap->getNode(0); 
	char *load_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];

	while(1)
	{
		if(type1 == 0xff)
		{
			if(type2 == 0xff)
			{
				break;
			}
			AosBitmapNode::copyNode(rbuff0, rbuff2,type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}
		
		if(type2 == 0xff)
		{
			AosBitmapNode::copyNode(rbuff0, rbuff1,type1, filter1);
			getTypeFilter(rbuff1,type1, filter1);
			continue;
		}
		
		// node 1 not null and node 2 not null 
		if( (filter1 & 0xffffffffffffc000ULL) > (filter2 & 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::copyNode(rbuff0, rbuff2,type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}		
		else if( (filter1 & 0xffffffffffffc000ULL) < (filter2& 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::copyNode(rbuff0, rbuff1,type1, filter1);
			getTypeFilter(rbuff1,type1, filter1);
			continue;
		}
		else
		{
			// do real or between two nodes
			node1->clean(0);
			node2->clean(0);
			AosBitmapNode::loadFromBuff(buff1, node1, load_buff, type1, filter1);
			AosBitmapNode::loadFromBuff(buff2, node2, load_buff, type2, filter2);
			node1->countOr(node2);
			if(node1->mNodeNum)
			{	
				node1->saveToBuff(buff0,load_buff);
			}
			getTypeFilter(rbuff1,type1, filter1);
			getTypeFilter(rbuff2,type2, filter2);
		}
	}
	buff0.getPtrNoLock()->setU8(0xff);
	if(node1)
	{
		bitmap->returnNode(node1);
		node1 = 0;
	}
	if(node2)
	{
		bitmap->returnNode(node2);
		node2 = 0;
	}

	if(load_buff)
	{
		delete []load_buff;
		load_buff = 0;
	}

	return bitmap;
}


AosBitmapObjPtr 
AosBitmap::countAndPacked(const AosBitmapObjPtr &a, const AosBitmapObjPtr &b)
{
	if (a.getPtr() == b.getPtr()) return a->clone(0);
	
	a->resetBuff();
	b->resetBuff();
	

	AosBuffPtr buff1 = a->getBuff();
	AosBuffPtr buff2 = b->getBuff();
	AosBuff* rbuff1 = buff1.getPtr();
	AosBuff* rbuff2 = buff2.getPtr();

	// case 1. buff1 is null
	// case 2. buff2 is null
	if(!rbuff1)
	{
		return a->clone(0);
	}
	
	if(!rbuff2)
	{
		return b->clone(0);
	}

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();	
	AosBuffPtr buff0 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bitmap->setBuff(buff0);
	
	
	u8 type1 = 0;
	u8 type2 = 0;
	u64 filter1 = 0;
	u64 filter2 = 0;
	
	getTypeFilter(rbuff1,type1, filter1);
	getTypeFilter(rbuff2,type2, filter2);
	aos_assert_r(type1 != 0xff,0);
	aos_assert_r(type2 != 0xff,0);

	AosBitmapNode* node1 = bitmap->getNode(0); 
	AosBitmapNode* node2 = bitmap->getNode(0); 
	char *load_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];

	while(1)
	{
		if(type1 == 0xff)
		{
			break;
		}
		
		if(type2 == 0xff)
		{
			break;
		}
		
		// node 1 not null and node 2 not null 
		if( (filter1 & 0xffffffffffffc000ULL) > (filter2 & 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::jumpNode(rbuff2 , type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}		
		else if( (filter1 & 0xffffffffffffc000ULL) < (filter2& 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::jumpNode(rbuff1, type1, filter1);
			getTypeFilter(rbuff1,type1, filter1);
			continue;
		}
		else
		{
			// do real and between two nodes
			node1->clean(0);
			node2->clean(0);
			AosBitmapNode::loadFromBuff(buff1, node1, load_buff, type1, filter1);
			AosBitmapNode::loadFromBuff(buff2, node2, load_buff, type2, filter2);
			node1->countAnd(node2);
			if(node1->mNodeNum)
			{	
				node1->saveToBuff(buff0,load_buff);
			}
			getTypeFilter(rbuff1,type1, filter1);
			getTypeFilter(rbuff2,type2, filter2);
		}
	}
	buff0.getPtrNoLock()->setU8(0xff);
	if(node1)
	{
		bitmap->returnNode(node1);
		node1 = 0;
	}
	if(node2)
	{
		bitmap->returnNode(node2);
		node2 = 0;
	}

	if(load_buff)
	{
		delete []load_buff;
		load_buff = 0;
	}

	return bitmap;
}


bool
AosBitmap::orBitmap(const AosBitmapObjPtr &bitmap)
{
	if (this == bitmap.getPtr()) return true;
	lock();
	bitmap->lock();
	mNumBits = 0;
	if(isPacked() || bitmap->isPacked())
	{
		pack();
		bitmap->pack();		
		bool rslt = orBitmapPacked(bitmap);
		unlock();
		bitmap->unlock();
		return rslt;
	}
	expand();
	bitmap->expand();


	bitmap->reset();

	AosBitmapNode * node_this   = 0;
	AosBitmapNode * node_bitmap = bitmap->getNextLeafNode();
	

	while (node_bitmap)
	{

		node_this = getLeafNode(node_bitmap->mFilter);
		if (!node_this)
		{
			this->addLeafNodeCopy(node_bitmap);
			node_bitmap = bitmap->getNextLeafNode();
			continue;
		}
		AosBitmapNode::countOr(node_this, node_this, node_bitmap);
		node_bitmap = bitmap->getNextLeafNode();
	}
	unlock();
	bitmap->unlock();
	return true;
}

bool
AosBitmap::orBitmapPacked(const AosBitmapObjPtr &b)
{
	if (b.getPtr() == this) return this->clone(0);
	
	resetBuff();
	b->resetBuff();
	

	AosBuffPtr buff1 = mBuff;
	AosBuffPtr buff2 = b->getBuff();
	AosBuff* rbuff1 = buff1.getPtr();
	AosBuff* rbuff2 = buff2.getPtr();

	// case 1. buff1 is null
	// case 2. buff2 is null
	if(!rbuff2)
	{
		return true;
	}

	if(!rbuff1)
	{
		mBuff = buff2->clone(AosMemoryCheckerArgsBegin);
		return true;
	}
	

	u8 type1 = 0;
	u8 type2 = 0;
	u64 filter1 = 0;
	u64 filter2 = 0;
	
	getTypeFilter(rbuff1,type1, filter1);
	getTypeFilter(rbuff2,type2, filter2);
	aos_assert_r(type1 != 0xff,false);
	aos_assert_r(type2 != 0xff,false);

	AosBitmapNode* node1 = getNode(0); 
	AosBitmapNode* node2 = getNode(0); 
	char *load_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];

	AosBuffPtr buff0 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosBuff* rbuff0 = buff0.getPtr();
	mBuff = buff0;

	while(1)
	{
		if(type1 == 0xff)
		{
			if(type2 == 0xff)
			{
				break;
			}
			AosBitmapNode::copyNode(rbuff0, rbuff2,type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}
		
		if(type2 == 0xff)
		{
			AosBitmapNode::copyNode(rbuff0, rbuff1,type1, filter1);
			getTypeFilter(rbuff1,type1, filter1);
			continue;
		}
		
		// node 1 not null and node 2 not null 
		if( (filter1 & 0xffffffffffffc000ULL) > (filter2 & 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::copyNode(rbuff0, rbuff2,type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}		
		else if( (filter1 & 0xffffffffffffc000ULL) < (filter2& 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::copyNode(rbuff0, rbuff1,type1, filter1);
			getTypeFilter(rbuff1,type1, filter1);
			continue;
		}
		else
		{
			// do real or between two nodes
			node1->clean(0);
			node2->clean(0);
			AosBitmapNode::loadFromBuff(buff1, node1, load_buff, type1, filter1);
			AosBitmapNode::loadFromBuff(buff2, node2, load_buff, type2, filter2);
			node1->countOr(node2);
			if(node1->mNodeNum)
			{
				node1->saveToBuff(buff0,load_buff);
			}
			getTypeFilter(rbuff1,type1, filter1);
			getTypeFilter(rbuff2,type2, filter2);
		}
	}
	buff0.getPtrNoLock()->setU8(0xff);
	if(node1)
	{
		returnNode(node1);
		node1 = 0;
	}
	if(node2)
	{
		returnNode(node2);
		node2 = 0;
	}

	if(load_buff)
	{
		delete []load_buff;
		load_buff = 0;
	}

	return true;
}

bool
AosBitmap::andBitmapPacked(const AosBitmapObjPtr &b)
{
	if (b.getPtr() == this) return this->clone(0);
	
	resetBuff();
	b->resetBuff();
	
	AosBuffPtr buff1 = mBuff;
	AosBuffPtr buff2 = b->getBuff();
	AosBuff* rbuff1 = buff1.getPtr();
	AosBuff* rbuff2 = buff2.getPtr();

	// case 1. buff1 is null
	// case 2. buff2 is null
	if(!rbuff1)
	{
		return true;
	}

	if(!rbuff2)
	{
		mBuff = 0;
		return true;
	}
	

	u8 type1 = 0;
	u8 type2 = 0;
	u64 filter1 = 0;
	u64 filter2 = 0;
	
	getTypeFilter(rbuff1,type1, filter1);
	getTypeFilter(rbuff2,type2, filter2);
	aos_assert_r(type1 != 0xff,false);
	aos_assert_r(type2 != 0xff,false);

	AosBitmapNode* node1 = getNode(0); 
	AosBitmapNode* node2 = getNode(0); 
	char *load_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];

	AosBuffPtr buff0 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mBuff = buff0;

	while(1)
	{
		if(type1 == 0xff)
		{
			break;
		}
		
		if(type2 == 0xff)
		{
			break;
		}
		
		// node 1 not null and node 2 not null 
		if( (filter1 & 0xffffffffffffc000ULL) > (filter2 & 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::jumpNode(rbuff2 , type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}		
		else if( (filter1 & 0xffffffffffffc000ULL) < (filter2& 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::jumpNode(rbuff1 , type1, filter1);
			getTypeFilter(rbuff1,type1, filter1);
			continue;
		}
		else
		{
			// do real or between two nodes
			node1->clean(0);
			node2->clean(0);
			AosBitmapNode::loadFromBuff(buff1, node1, load_buff, type1, filter1);
			AosBitmapNode::loadFromBuff(buff2, node2, load_buff, type2, filter2);
			node1->countAnd(node2);
			if(node1->mNodeNum)
			{
				node1->saveToBuff(buff0,load_buff);
			}
			getTypeFilter(rbuff1,type1, filter1);
			getTypeFilter(rbuff2,type2, filter2);
		}
	}
	buff0.getPtrNoLock()->setU8(0xff);
	if(node1)
	{
		returnNode(node1);
		node1 = 0;
	}
	if(node2)
	{
		returnNode(node2);
		node2 = 0;
	}

	if(load_buff)
	{
		delete []load_buff;
		load_buff = 0;
	}

	return true;
}

bool
AosBitmap::checkAndBitmapPacked(
				const AosBitmapObjPtr &a,
	 			const AosBitmapObjPtr &b)
{
	if (b.getPtr() == this) return !isEmpty();
	
	a->resetBuff();
	b->resetBuff();
	

	AosBuffPtr buff1 = a->getBuff();
	AosBuffPtr buff2 = b->getBuff();
	AosBuff* rbuff1 = buff1.getPtr();
	AosBuff* rbuff2 = buff2.getPtr();

	// case 1. buff1 is null
	// case 2. buff2 is null
	if(!rbuff1||!rbuff2)
	{
		return false;
	}

	u8 type1 = 0;
	u8 type2 = 0;
	u64 filter1 = 0;
	u64 filter2 = 0;
	
	getTypeFilter(rbuff1,type1, filter1);
	getTypeFilter(rbuff2,type2, filter2);
	aos_assert_r(type1 != 0xff,false);
	aos_assert_r(type2 != 0xff,false);

	AosBitmapNode* node1 = getNode(0); 
	AosBitmapNode* node2 = getNode(0); 
	char *load_buff = OmnNew char[AosBitmapNode::eBitmapByteSize];
	bool has_same = false;
	while(1)
	{
		if(type1 == 0xff)
		{
			break;
		}
		
		if(type2 == 0xff)
		{
			break;
		}
		
		// node 1 not null and node 2 not null 
		if( (filter1 & 0xffffffffffffc000ULL) > (filter2 & 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::jumpNode(rbuff2 , type2, filter2);
			getTypeFilter(rbuff2,type2, filter2);
			continue;
		}		
		else if( (filter1 & 0xffffffffffffc000ULL) < (filter2& 0xffffffffffffc000ULL) )
		{
			AosBitmapNode::jumpNode(rbuff1 , type1, filter1);
			getTypeFilter(rbuff1, type1, filter1);
			continue;
		}
		else
		{
			// do real or between two nodes
			node1->clean(0);
			node2->clean(0);
			AosBitmapNode::loadFromBuff(buff1, node1, load_buff, type1, filter1);
			AosBitmapNode::loadFromBuff(buff2, node2, load_buff, type2, filter2);
			node1->countAnd(node2);
			if(node1->mNodeNum)
			{
				has_same = true;
				break;
			}			
			getTypeFilter(rbuff1,type1, filter1);
			getTypeFilter(rbuff2,type2, filter2);
		}
	}
	if(node1)
	{
		returnNode(node1);
		node1 = 0;
	}
	if(node2)
	{
		returnNode(node2);
		node2 = 0;
	}

	if(load_buff)
	{
		delete []load_buff;
		load_buff = 0;
	}

	return has_same;
}

AosBitmapObjPtr 
AosBitmap::countAndBlock(
		const AosBitmapObjPtr* bitmaps,
		const u64 num_bitmaps )
{
	if (bitmaps == NULL)
	{
        OmnAlarm<< " bitmaps == NULL " << enderr;
		return 0;
	}

	bool all_packed = true;
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		aos_assert_r(bitmaps[i],0);
	}
	
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		bitmaps[i]->lock();
	}
	
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		if(!bitmaps[i]->isPacked())
		{
			all_packed = false;
			break;
		}
	}
	
	if(all_packed)
	{
		OmnScreen << "Can be better" << endl;
	}

	for(u64 i = 0;i < num_bitmaps;i++)
	{
		bitmaps[i]->expand();
	}

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
  
	if (num_bitmaps < 2)
	{
		for(u64 i = 0;i < num_bitmaps;i++)
		{
			bitmaps[i]->unlock();
		}
		OmnAlarm<< " num_bitmaps < 2 : " << num_bitmaps << enderr; 
		return bitmap;
	}
	
    //reset bitmaps	
    for (u64 i=0; i<num_bitmaps; i++)
    {
    	bitmaps[i]->reset();
    }
    
    AosBitmapNode** nodes = OmnNew AosBitmapNode*[num_bitmaps];
	if (nodes == NULL) 
	{
		for(u64 i = 0;i < num_bitmaps;i++)
		{
			bitmaps[i]->unlock();
		}
		OmnAlarm<< " nodes == NULL " << enderr;
		return bitmap;
	}
	for (u64 i=0; i<num_bitmaps; i++)
	{
		nodes[i] = NULL;		
	}
	
	//do a1&a2...&an
	AosBitmapNode* cur_node = bitmap->getNode(0);
    
    while (true)
    {
    	nodes[0] = bitmaps[0]->getNextLeafNode();
    	if (nodes[0] == NULL)
    	{
    		break;
    	}
    		
        u64 i = 0;
    	for (i=1; i<num_bitmaps; i++)
    	{
    		nodes[i] = bitmaps[i]->getLeafNode(nodes[0]->mFilter);
    		if (!(nodes[i]))
    		{
    			break;//go to whil
    		}
    	}
    	if (i<num_bitmaps || nodes[i-1] == NULL)
    	{
    		continue;
    	}
    	
    	AosBitmapNode::countAndBlock(cur_node, nodes, num_bitmaps);
    	if (cur_node->mNodeNum)
		{
			bitmap->addLeafNode(cur_node);
			cur_node = bitmap->getNode(0);
		}
    }


    //release memory
	delete []nodes;
	nodes = NULL;

	if (cur_node)
	{
		bitmap->returnNode(cur_node);
		cur_node = 0;
	}
		
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		bitmaps[i]->unlock();
	}
	return bitmap;
}


AosBitmapObjPtr 
AosBitmap::countOrBlock(
		const AosBitmapObjPtr* bitmaps,
		const u64 num_bitmaps)
{
	if (!bitmaps)
	{
        OmnAlarm<< " bitmaps == NULL " << enderr;
		return 0;
	}

	bool all_packed = true;
	
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		aos_assert_r(bitmaps[i],0);
	}

	for(u64 i = 0;i < num_bitmaps;i++)
	{
		bitmaps[i]->lock();
	}
	
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		if(!bitmaps[i]->isPacked())
		{
			all_packed = false;
			break;
		}
	}
	if(all_packed)
	{
		OmnScreen << "Can be better" << endl;
	}
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		bitmaps[i]->expand();
	}

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
  
	if (num_bitmaps < 1)
	{
		OmnAlarm<< " num_bitmaps < 1 : " << num_bitmaps << enderr; 
		for(u64 i = 0;i < num_bitmaps;i++)
		{
			bitmaps[i]->unlock();
		}

		return bitmap;
	}
	if (num_bitmaps == 1)
	{
		bitmap = bitmaps[0]->clone(0);
		for(u64 i = 0;i < num_bitmaps;i++)
		{
			bitmaps[i]->unlock();
		}
		return bitmap;
	}
	
	
    //reset bitmaps	
    for (u64 i=0; i<num_bitmaps; i++)
    {
    	bitmaps[i]->reset();
    }
    
    AosBitmapNode** filter_equal_nodes = OmnNew AosBitmapNode*[num_bitmaps];
    u64 num_filter_equal_nodes = 0;
    u64 min_filter = 0;
    
    AosBitmapNode** nodes = OmnNew AosBitmapNode*[num_bitmaps];
	if (!nodes) 
	{
		OmnAlarm<< " nodes == NULL " << enderr;
		for(u64 i = 0;i < num_bitmaps;i++)
		{
			bitmaps[i]->unlock();
		}
		return bitmap;
	}
	
	for (u64 i=0; i<num_bitmaps; i++)
	{
		nodes[i] = bitmaps[i]->getNextLeafNode();
	}
	
	//do a1||a2...||an
	AosBitmapNode* cur_node = bitmap->getNode(0);
    
    while (true)
    {
    	//get the min mFilter in all the bitmaps
    	min_filter = INVALIDATEID;
		for (u64 i=0; i<num_bitmaps; i++)
		{
			if (!nodes[i]) continue;

			if (min_filter == INVALIDATEID)
			{
				min_filter = nodes[i]->mFilter;
			}
			else
			{
				min_filter = AOS_MIN(min_filter, nodes[i]->mFilter);
			}
		}
		
		if (min_filter == INVALIDATEID) break;
		
		//get all the min filter nodes from groups
		num_filter_equal_nodes = 0;
	    for (u64 i=0; i<num_bitmaps; i++)
	    {
			if (!nodes[i]) continue;

  		    if (nodes[i]->mFilter == min_filter)
		    {
				filter_equal_nodes[num_filter_equal_nodes++] = nodes[i];     
			    nodes[i] = bitmaps[i]->getNextLeafNode();
		    }
	    }
		if (num_filter_equal_nodes == 0)
		{
			//never come here
			OmnAlarm<< " num_filter_equal_nodes == 0 " << enderr;
			break;
		}
		
		AosBitmapNode::countOrBlock(cur_node, filter_equal_nodes, num_filter_equal_nodes);
		if (cur_node->mNodeNum)
		{
			bitmap->addLeafNode(cur_node);
			cur_node = bitmap->getNode(0);
		}		 
    }


    //release memory
	delete []nodes;
	nodes = NULL;

    delete []filter_equal_nodes;
    filter_equal_nodes = NULL;
    
	if (cur_node)
	{
		bitmap->returnNode(cur_node);
		cur_node = 0;
	}		
	for(u64 i = 0;i < num_bitmaps;i++)
	{
		bitmaps[i]->unlock();
	}
	return bitmap;
}



AosBitmapObjPtr 
AosBitmap::countGroupOrAnd(
         const AosBitmapObjPtr** bitmap_groups,
		 const u64* num_bitmaps,
		 const u64  num_groups)
{
	// no lock
	OmnShouldNeverComeHere;
    for(u64 i=1; i<num_groups; i++)
    {
	    for(u64 j=0; j<num_bitmaps[i]; j++)
    	{
			bitmap_groups[i][j]->expand();
		}
	}

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
  
	if(num_groups < 1)
	{
		OmnAlarm<< " num_groups < 1 : " << num_groups << enderr; 
		return bitmap;
	}
	if(num_bitmaps == NULL)
	{
        OmnAlarm<< " num_bitmaps == NULL " << enderr;
		return bitmap;
	}
	for(u64 i=0; i<num_groups; i++)
	{
		if(num_bitmaps[i] < 1)
		{
			OmnAlarm<< " num_bitmaps[ " << i <<" ] < 1 : " << num_bitmaps[i] << enderr;
			return bitmap;
		}
		if(bitmap_groups[i] == NULL)
		{
			OmnAlarm<< " bitmap_groups[ " << i << " ] == NULL " << enderr;
			return bitmap;
		}
	}

	AosBitmapNode** group0_nodes = OmnNew AosBitmapNode*[num_bitmaps[0]];
	if(group0_nodes == NULL) 
	{
		OmnAlarm<< " group0_nodes == NULL " << enderr;
		return bitmap;
	}
	u64 min_filter = 0;

	AosBitmapNode** rslt_or_nodes;
	rslt_or_nodes = OmnNew AosBitmapNode*[num_groups];
	if(rslt_or_nodes == NULL) 
	{
		OmnAlarm<< " rslt_or_nodes == NULL " << enderr;
		return bitmap;
	}
	for(u64 i=0; i<num_groups; i++)
	{
		rslt_or_nodes[i] = bitmap->getNode(0);
	}

	u64* num_filter_equal_nodes = new u64[num_groups];
	if(num_filter_equal_nodes == NULL) 
	{
		OmnAlarm<< " num_filter_equal_nodes == NULL " << enderr;
		return bitmap;
	}
	AosBitmapNode*** filter_equal_nodes = OmnNew AosBitmapNode**[num_groups];
	if(filter_equal_nodes == NULL) 
	{
		OmnAlarm<< " filter_equal_nodes == NULL " << enderr;
		return bitmap;
	}
    for(u64 i=0; i<num_groups; i++)
	{
		filter_equal_nodes[i] = OmnNew AosBitmapNode*[num_bitmaps[i]];
		if(filter_equal_nodes[i] == NULL) 
		{
			OmnAlarm<< " filter_equal_nodes[ " << i << " ] == NULL " << enderr;
			return bitmap;
		}
		num_filter_equal_nodes[i] = 0;
	}

	//get all the nodes from the first bitmap 
	for(u64 i=0; i<num_bitmaps[0]; i++)
	{
		group0_nodes[i] = bitmap_groups[0][i]->getNextLeafNode();
	}

	AosBitmapNode* cur_node = bitmap->getNode(0);
	u64 i = 0;
	while(true)
	{
	    //get the min mFilter from group0
     	min_filter = INVALIDATEID;
		for(i=0; i<num_bitmaps[0]; i++)
		{
			if(group0_nodes[i] == NULL)
			{
				continue;
			}

			if(min_filter == INVALIDATEID)
			{
				min_filter = group0_nodes[i]->mFilter;
			}
			else
			{
				min_filter = AOS_MIN(min_filter, group0_nodes[i]->mFilter);
			}
		}
		if(min_filter == INVALIDATEID) 
		{
			break;
		}

		//get all the min filter nodes from group0
		num_filter_equal_nodes[0] = 0;
	    for(i=0; i<num_bitmaps[0]; i++)
	    {
			if(group0_nodes[i] == NULL)
			{
				continue;
			}

  		    if(group0_nodes[i]->mFilter == min_filter)
		    {
				filter_equal_nodes[0][num_filter_equal_nodes[0]++] = group0_nodes[i];     
			    group0_nodes[i] = bitmap_groups[0][i]->getNextLeafNode();
		    }
	    }
		if(num_filter_equal_nodes[0] == 0)
		{
			//never come here
			OmnAlarm<< " num_filter_equal_nodes[0] == 0 " << enderr;
			break;
		}

		//
        AosBitmapNode* node;
	    for(i=1; i<num_groups; i++)
	    {
			num_filter_equal_nodes[i] = 0;
		    for(u64 j=0; j<num_bitmaps[i]; j++)
	    	{
				node = bitmap_groups[i][j]->getLeafNode(min_filter);
			    if(node == NULL)
				{
					continue;
				}
				filter_equal_nodes[i][num_filter_equal_nodes[i]++] = node;
		    }
		    if(num_filter_equal_nodes[i] == 0)
			{
				break;//need go to while
			}
	    }
		if((i != num_groups) && (num_filter_equal_nodes[i] == 0))//breaked in above for()
		{
			continue;
		}

		for(i=0; i<num_groups; i++)
		{
			AosBitmapNode::countOrBlock(rslt_or_nodes[i], filter_equal_nodes[i], num_filter_equal_nodes[i]);
		}

		 AosBitmapNode::countAndBlock(cur_node, rslt_or_nodes, num_groups);
		 if(cur_node->mNodeNum)
		 {
			 bitmap->addLeafNode(cur_node);
			 cur_node = bitmap->getNode(0);
		 }
	}//end of while

	//release memory
	delete []group0_nodes;
	group0_nodes = NULL;

	for(i=0; i<num_groups; i++)
	{
	    bitmap->returnNode(rslt_or_nodes[i]);
	    rslt_or_nodes[i] = NULL;
	}
	delete []rslt_or_nodes;
	rslt_or_nodes = NULL;

	for(u64 i=0; i<num_groups; i++)
	{
		delete [](filter_equal_nodes[i]);
		filter_equal_nodes[i] = NULL;
	}
	delete []filter_equal_nodes;
	filter_equal_nodes = NULL;
	
	return bitmap;
}

AosBitmapNode*
AosBitmap::getNextLeafNode()
{
	expand();
	if(!mNode) return 0;
	if(!mItr)
	{
		mItr = OmnNew AosBitmapItr();

		// get the first doc
		mItr->mNodes[0] = mNode;
	}
	return mItr->nextLeaf();	
}


bool
AosBitmap::getDocids(vector<u64> &v)
{
	expand();
	reset();
	AosBitmapNode* node = getNextLeafNode();
	while (node)
	{
		node->getDocids(v);
		node = getNextLeafNode();
	}
	return true;
}


OmnString
AosBitmap::dumpToStr(int num_per_line)
{
	expand();
	vector<u64> v;
	getDocids(v);
	OmnString ss;
	int line = 0;
	for (size_t i=0; i<v.size(); i++)
	{
		ss << "," << v[i];
		if (line++ >= num_per_line)
		{
			line = 0;
			ss << ",\n";
		}
	}
	ss << ",";

	if (v.size() == 0)
	{
		ss << "Bitmap is empty\n";
		return ss;
	}
	ss << "\n";

	return ss;
}


bool 
AosBitmap::toString()
{
	expand();
	vector<u64> v;
	getDocids(v);
	OmnScreen << "total: " << v.size() << endl;
	for (size_t i=0; i<v.size(); i++)
	{
		cout << v[i] << ",";
	}
	if (v.size() == 0)
	{
		OmnScreen << "Bitmap is empty" << endl;
		return true;
	}
	cout << endl;

	return true;
}


u64 
AosBitmap::getMemorySize() 
{
	expand();
	if (mMemorySize > 0) return mMemorySize;
	if (!mNode) return 0;
	mNode->getMemorySize(mMemorySize);
	return mMemorySize;
}


// Chen Ding, 12/27/2012
AosBitmapObjPtr 
AosBitmap::countAndAnotB(
		const AosBitmapObjPtr &a, 
		const AosBitmapObjPtr &b)
{
	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
	if (a.getPtr() == b.getPtr()) return bitmap;

	a->lock();
	b->lock();

	if(a->isPacked() && b->isPacked())
	{
		//AosBitmapObjPtr rslt = countAndAnotBPacked(a,b);
		//a->unlock();
		//b->unlock();
		//return rslt;
		OmnScreen << "Can be better" << endl;
	}
	a->expand();
	b->expand();

	// This function is the same as "countAnd(a, b)" except that
	// if 'need_negate', it is:
	// 		A AND !B:  find one bit in A but not in B
	

	a->reset();
	b->reset();	
   
	AosBitmapNode* node1 = a->getNextLeafNode();
	AosBitmapNode* node2 = b->getNextLeafNode();
	AosBitmapNode* cur_node = bitmap->getNode(0);

	if (!node2)
	{
		AosBitmapObjPtr rslt = a->clone(0);
		a->unlock();
		b->unlock();
		return rslt;
	}
	node2 = 0 ;	

	while(node1)
	{			    				
		node2 = b->getLeafNode(node1->mFilter);
		if (node2)   //some bits in a is same with the ones in b
		{
			 //nScreen << "negateBitmap" << endl;
             AosBitmapNode::negateBitmap(cur_node,node1,node2);			 		    			    
		}
		else 			
		{	
			 //nScreen << " node is null!" << endl;
             AosBitmapNode::clone(cur_node,node1);             
		}
        if (cur_node->mNodeNum)
	    {                 
		   bitmap->addLeafNode(cur_node);
		   cur_node = bitmap->getNode(0);			  
		}
		node1 = a->getNextLeafNode();		
	}
	
	if(cur_node)
	{
		bitmap->returnNode(cur_node);
		cur_node = 0;
	}
	 	
	a->unlock();
	b->unlock();
	return bitmap;
}

bool
AosBitmap::getSections(
		const AosRundataPtr &rdata,
		map_t &bitmaps)
{
	lock();
	if(isPacked())
	{
		return getSectionsPacked(rdata,bitmaps);
	}

	bitmaps.clear();

	if (!mNode) 
	{
		unlock();
		return true;
	}
	reset();
	AosBitmapNode* cur_node = getNextLeafNode();
	u16 section_id = 0;
	mapitr_t itr;
	while(cur_node)
	{
		section_id = docid2SectionId(cur_node->mFilter);
		itr = bitmaps.find(section_id);
		if (itr == bitmaps.end())
		{
			AosBitmapObjPtr bmp = AosGetBitmap();
			bmp->setSectionId(section_id);
			bmp->addLeafNodeCopy(cur_node);
			bitmaps[section_id] = bmp;
		}
		else
		{
			itr->second->addLeafNodeCopy(cur_node);
		}
		
		cur_node = getNextLeafNode();
	}
	unlock();
	return true;
}


bool
AosBitmap::getSectionsPacked(
		const AosRundataPtr &rdata,
		map_t &bitmaps)
{
	bitmaps.clear();

	u8 type = 0;
	u64 filter = 0;
	u16 section_id = 0;
	if(!mBuff)
	{
		return true;
	}

	AosBuffPtr cur_buff;
	mapitr_t itr;

	mBuff->reset();
	while(1)
	{
		getTypeFilter(mBuff.getPtrNoLock(),type, filter);
		if(type == 0xff)
		{
			break;
		}
		section_id = docid2SectionId(filter);
		itr = bitmaps.find(section_id);
		if (itr == bitmaps.end())
		{
			AosBitmapObjPtr bmp = AosGetBitmap();
			bmp->setSectionId(section_id);
			cur_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			bmp->setBuff(cur_buff);
			AosBitmapNode::copyNode(cur_buff.getPtrNoLock(), mBuff.getPtrNoLock(),type, filter);
			bitmaps[section_id] = bmp;
		}		
		else
		{
			AosBitmapNode::copyNode(itr->second->getBuff().getPtrNoLock(), mBuff.getPtrNoLock(),type, filter);
		}
	}
	// set finish flag 0xff
	for (itr = bitmaps.begin(); itr != bitmaps.end(); itr++)
	{
		itr->second->getBuff()->setU8(0xff);
	}		
	
	
	return true;
}
	
bool
AosBitmap::getSections(const AosRundataPtr &rdata, vector<u64> &sections)
{
	lock();
	expand();

	sections.clear();
	map_t bitmaps;

	if (!mNode) 
	{
		unlock();
		return true;
	}
	bool rslt = mNode->getSections(bitmaps, rdata);
	aos_assert_r(rslt, false);

	mapitr_t itr = bitmaps.begin();
	while (itr != bitmaps.end())
	{
		sections.push_back(itr->first);
		itr++;
	}
	unlock();
	return true;
}


u64
AosBitmap::nextNDocid(
		u64* list,
		int &count)
{
	OmnNotImplementedYet;
	return 0;
}


AosBitmapObjPtr 
AosBitmap::createBitmap(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, 0);
	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
	aos_assert_rr(bitmap, rdata, 0);
	bool rslt = bitmap->loadFromBuff(buff);
	aos_assert_rr(rslt, rdata, 0);
	return bitmap;
}


AosBitmapObjPtr
AosBitmap::createBitmap() const
{
	// Chen Ding, 2013/11/10
	// return AosBitmapMgr::getSelf()->getBitmap();
	return OmnNew AosBitmap();
}


void
AosBitmap::returnBitmap(const AosBitmapObjPtr &bitmap)
{
	return AosBitmapMgr::getSelf()->returnBitmap(bitmap);
}



bool 
AosBitmap::removeBits(const AosBitmapObjPtr &b)
{
	if(isPacked() && b->isPacked())
	{
		OmnScreen << "Can be better" << endl;
	}
	expand();
	b->expand();

	mNumBits = 0;
	reset();
	b->reset();
	AosBitmapNode* node1 = getNextLeafNode();
	AosBitmapNode* node2 = b->getNextLeafNode();
	while(node2)
	{
		if(!node1)
		{
			// proc node2
			node2 = b->getNextLeafNode();
			continue;
		}
		
		if(node1->mFilter == node2->mFilter)
		{
			// proc node1 || node 2
			node1->removeBits(node2);
			node1 = getNextLeafNode();
			node2 = b->getNextLeafNode();
		}
		else if(node1->mFilter > node2->mFilter)
		{
			node2 = b->getNextLeafNode();
		}
		else // (node1->mFilter < node2->mFilter)
		{
			// proc node1
			node1 = getNextLeafNode();
		}
	}

	return true;
}


bool 
AosBitmap::includeBitmap(const AosBitmapObjPtr &bitmap)
{
	if (this == bitmap.getPtr()) return true;

	lock();
	bitmap->lock();
	
	if(isPacked() && bitmap->isPacked())
	{
		OmnScreen << "Can be better" << endl;
	}
	expand();
	bitmap->expand();

	reset();
	bitmap->reset();	

	AosBitmapNode* node1 = 0;
	AosBitmapNode* node2 = 0;	
	while(1)
	{
		node1 = getNextLeafNode();
		node2 = bitmap->getNextLeafNode();
		if(!node2) 
		{
			unlock();
			bitmap->unlock();
			return true;
		}
		
		while(1)
		{
			if(!node1)
			{
				unlock();
				bitmap->unlock();
				return false;
			}
			if(node1->mFilter > node2->mFilter)
			{
				unlock();
				bitmap->unlock();
				return false;
			}
			else if(node1->mFilter == node2->mFilter)
			{
				break;
			}
			else
			{
				node1 = getNextLeafNode();
			}
		}


		if(!node1->include(node2))
		{
			unlock();
			bitmap->unlock();
			return false;
		}
	}
	unlock();
	bitmap->unlock();
	return false;
}

bool 
AosBitmap::compareBitmap(const AosBitmapObjPtr &bitmap)
{
	if (this == bitmap.getPtr()) return true;

	lock();
	bitmap->lock();
	
	if(isPacked() && bitmap->isPacked())
	{
		OmnScreen << "Can be better" << endl;
	}
	expand();
	bitmap->expand();

	reset();
	bitmap->reset();	

	AosBitmapNode* node1 = 0;
	AosBitmapNode* node2 = 0;	
	while(1)
	{
		node1 = getNextLeafNode();while(node1 && node1->mNodeNum == 0)node1 = getNextLeafNode();
		node2 = bitmap->getNextLeafNode();while(node2 && node2->mNodeNum == 0)node2 = bitmap->getNextLeafNode();
		if(!node1)
		{
			if(!node2)
			{
				unlock();
				bitmap->unlock();
				return true;
			}
			else
			{
				unlock();
				bitmap->unlock();
				return false;
			}
		}
		if(!node2)
		{
			unlock();
			bitmap->unlock();
			return false;
		}
		if(!node1->sameAs(node2))
		{
			unlock();
			bitmap->unlock();
			return false;
		}
	}
	unlock();
	bitmap->unlock();
	return false;
}

AosBitmapObjPtr 
AosBitmap::countAndStatic(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b)
{
	return AosBitmap::countAnd(a,b);
}

AosBitmapObjPtr 
AosBitmap::countOrStatic(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b)
{
	return AosBitmap::countOr(a,b);
}


AosBitmapObjPtr
AosBitmap::clone(const AosRundataPtr &rdata)
{
	AosBitmapObjPtr bmp = AosGetBitmap();
	bmp->setSectionId(mSectionId);
	bmp->setBitmapId(mBitmapId);
	bmp->setModifyId(mModifyId);
	bmp->setIILLevel(mIILLevel);
	bmp->setNodeLevel(mNodeLevel);

	if(mBuff)
	{
		aos_assert_r(!mNode,0);
		//bmp->loadFromBuff(mBuff,true);
		bmp->setBuff(mBuff->clone(AosMemoryCheckerArgsBegin));
		return bmp;
	}
	reset();
	AosBitmapNode * node_bitmap = getNextLeafNode();

	
	int i = 0;
	while (node_bitmap)
	{
		aos_assert_r(node_bitmap->mBitmap,0);
		for(i = 0;i < 256;i++)
		{
			if(node_bitmap->mBitmap[i])
			{
				// there is something in the node
				bmp->addLeafNodeCopy(node_bitmap);
				break;
			}
		}
		node_bitmap = getNextLeafNode();
	}
	return bmp;
}


bool
AosBitmap::printDocids()
{
	expand();
	vector<u64> docids;
	getDocids(docids);
	OmnScreen << "Bitmap: " << mBitmapId << endl;
	char buff[100];
	for (u32 i=0; i<docids.size(); i++)
	{
		sprintf(buff, "%010lu", docids[i]);
		cout << buff << ", ";
	}
	return true;
}

i64  
AosBitmap::getNumBits()
{
	return getDocidCount();
}


bool 
AosBitmap::isEmpty() 
{
	if(isPacked())
	{
		OmnScreen << "Can be better" << endl;
	}
	expand();

	if (!mNode) return true;

	// return mNode->isEmpty();
    reset();
	AosBitmapNode* node = getNextLeafNode();
	while(node)
	{
		if(!node->isEmpty())
		{
			return false;
		}
		node = getNextLeafNode();
	}
	return true;
}


int
AosBitmap::getNumInstances()
{
	sgLock.lock();
	int nn = sgActiveNumInstances;
	sgLock.unlock();
	return nn;
}
	
bool 
AosBitmap::splitBitmap(const int num, vector<AosBitmapObjPtr> &bitmaps)
{
	lock();
	expand();
	aos_assert_r(num > 1,false);
	u64 total = getNumBits();
	u64 sizes[num];
	u64 num_each = total/num;
	u64 odds = total - num_each*num;
	u64 cur_docid = 0;
	
	AosBitmapObjPtr cur_bmp;
	reset();

	for(int i = 0;i < num;i++)
	{
		if(odds)
		{
			sizes[i] = num_each + 1;
			odds --;
		}
		else
		{
			sizes[i] = num_each;
		}
		cur_bmp = AosGetBitmap();
		bitmaps.push_back(cur_bmp);
		for(u64 j = 0;j < sizes[i];j++)
		{
			nextDocid(cur_docid);
			cur_bmp->appendDocid(cur_docid);
		}
	}
	unlock();
	return true;	
}
	
bool 
AosBitmap::splitBitmapByValues(const vector<u64> &values,vector<AosBitmapObjPtr> &bitmaps)
{
	lock();
	expand();
	int num = values.size();
	aos_assert_r(num > 0, false);
	
	AosBitmapObjPtr cur_bmp;
	reset();
	u64 cur_docid = 0;
	bool rslt = nextDocid(cur_docid);
	
	for(int i = 0;i < num;i++)
	{
		cur_bmp = AosGetBitmap();
		bitmaps.push_back(cur_bmp);
		while(rslt && cur_docid < values[i])
		{
			cur_bmp->appendDocid(cur_docid);
			rslt = nextDocid(cur_docid);
		}
	}
	
	// last bmp
	cur_bmp = AosGetBitmap();
	bitmaps.push_back(cur_bmp);
	while(rslt)
	{
		cur_bmp->appendDocid(cur_docid);
		rslt = nextDocid(cur_docid);
	}
	unlock();
	return true;	
}

bool 
AosBitmap::cloneFrom(const AosBitmapObjPtr &source)
{
	lock();
	cleanPriv();
	
	setSectionId(source->getSectionId());
	setBitmapId(source->getBitmapId());
	setModifyId(source->getModifyId());
	setIILLevel(source->getIILLevel());
	setNodeLevel(source->getNodeLevel());
	
	AosBuffPtr buff = source->getBuff();
	if(buff)
	{
		mBuff = buff->clone(AosMemoryCheckerArgsBegin);
		//loadFromBuff(buff,true);
		unlock();
		return true;
	}
	
	
	source->reset();
	AosBitmapNode * node_bitmap = source->getNextLeafNode();
	int i = 0;
	while (node_bitmap)
	{
		aos_assert_r(node_bitmap->mBitmap,0);

		for(i = 0;i < 256;i++)
		{
			if(node_bitmap->mBitmap[i])
			{
				// there is something in the node
				addLeafNodeCopy(node_bitmap);
				break;
			}
		}
		node_bitmap = source->getNextLeafNode();
	}
	unlock();
	return true;
}

bool 
AosBitmap::moveTo(const u64 &id, u64 &cur_docid, const bool reverse)
{
	expand();
	if(!mNode) return false;
	mItr = OmnNew AosBitmapItr();
	mItr->mNodes[0] = mNode;
	return mItr->moveTo(id,cur_docid, reverse);	
}

bool 
AosBitmap::isPacked()
{
	if(mBuff)
	{
		return true;
	}
	
	return !mNode;
}
/*u64
AosBitmap::getPage(const u64 page_size,const AosBitmapObjPtr &rslt_bmp,const bool reverse)
{
	aos_assert_r(rslt_bmp,false);
	if(page_size == 0)
	{
		return getCrtDocid();
	}
	
	u64 cur_size = 0;
	u64 cur_docid = getCrtDocid();
	while(cur_size < page_size && cur_docid)
	{
		rslt_bmp->appendDocid(cur_docid);
		cur_size ++;
		if(reverse)
		{
			cur_docid = prevDocid();
		}
		else
		{
			cur_docid = nextDocid();
		}
	}
	return cur_docid;
}
*/

bool
AosBitmap::getCrtDocid(u64 &docid)
{
	expand();
	if(!mNode) return false;
	if(!mItr)
	{
		return false;
	}
	return mItr->getCrtDocid(docid);
}

/*bool	
AosBitmap::testGetNum()
{
	reset();
	AosBitmapNode* cur_node = getNextLeafNode();
	while(cur_node)
	{
		aos_assert_r(cur_node->mNodeNum, false);
		cur_node = getNextLeafNode();
	}

	if(mNumBits == 0)
	{
		return true;
	}
	u64 num = mNumBits;
	mNumBits = 0;
	u64 num2 = getNumBits();
	aos_assert_r(num == num2,false);

	return true;
}
*/

AosBitmapNode* 
AosBitmap::getNode(const u8 level)
{
	//1. make sure the cache exist
	//2. get one node from cache
	//3. if the cache is all used, clean it, make the size double

	//1. make sure the cache exist
	aos_assert_r(mCacheSize > 0 , 0);
	if(!mNodeCache)
	{
		// get nodes for cache
		mNodeCache = OmnNew AosBitmapNode*[mCacheSize];
		AosBitmapMgr::getNodes(mNodeCache, mCacheSize);
		mCrtCachePos = 0;
	}
	//2. get one node from cache
	AosBitmapNode*	node = mNodeCache[mCrtCachePos++];
	//3. if the cache is all used, clean it, make the size double
	if(mCrtCachePos >= mCacheSize)
	{
		delete []mNodeCache;
		mNodeCache = 0;
		if(mCacheSize < eMaxCacheSize)
		{
			mCacheSize *= 2;
		}
	}
	node->init(level);
	return node;
}

void 
AosBitmap::returnSingleNode(AosBitmapNode *node)
{
	if(mCacheSize <= 2)
	{
		AosBitmapMgr::returnNodes(&node,1);
		return;
	}

	// 1. put it back to cache
	// 	1.1 if the cache is 0,  handle it
	if(!mNodeCache)
	{
		mCacheSize = mCacheSize / 2;
		mNodeCache = OmnNew AosBitmapNode*[mCacheSize];
		mCrtCachePos = mCacheSize-1;
		mNodeCache[mCrtCachePos] = node; 
	}
	else
	{
		// 	1.2 if the cache is not 0,  .......
		aos_assert(mCrtCachePos > 0);
		mCrtCachePos --;
		mNodeCache[mCrtCachePos] = node;			
	}
	// 2. if the cache is full, return half   (size +1)/2   back.  if the size is 1, return it back
	if(mCrtCachePos == 0)
	{
		u64 return_size = mCacheSize/2;
		AosBitmapMgr::returnNodes(mNodeCache, return_size);
		mCrtCachePos += return_size;
	}
}

u64 
AosBitmap::getNumLeaf()
{
	u64 num = 0;
	expand();
	if(!mNode) return 0;
	reset();
	AosBitmapNode* cur_node = getNextLeafNode();
	while(cur_node)
	{
		num++;
		cur_node = getNextLeafNode();
	}
	return num;
}

