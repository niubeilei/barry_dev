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
// 11/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/BitmapNode.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "API/AosApiG.h"
#include "BitmapMgr/BitmapMgr.h"
#include "SEInterfaces/BitmapObj.h"

static OmnMutex sgLock;
static int sgNumInstances = 0;
static int sgNumActives = 0;

AosBitmapNode::AosBitmapNode(const u8 level)
	:
		mNodeLevel(level),
		mFilter(0),
		mNodeNum(0)
{
	sgLock.lock();
	sgNumInstances++;
	sgNumActives++;
	if ((sgNumInstances % 100000) == 0)
	{
		OmnScreen << "------- Num BitmapNodes: " << sgNumInstances << ":" << sgNumActives << endl;
	}
	sgLock.unlock();

	if(level == 0)
	{
		mBitmap = (u64*)(mBuff);
		memset(mBitmap,0,eBitmapByteSize);
		mRsltmap = 0;
	}
	else
	{
		mRsltmap = (AosBitmapNode**)(mBuff);
		memset(mRsltmap,0,eMaxRsltByteSize);
		mBitmap = 0;
	}
}


AosBitmapNode::~AosBitmapNode()
{
	sgLock.lock();
	sgNumActives--;
	sgLock.unlock();
}


u8 	AosBitmapNode::smBitFindingList[0x10000];
u8 	AosBitmapNode::smBitRevFindingList[0x10000];
u8 	AosBitmapNode::smBitCountList[0x10000];


void 
AosBitmapNode::cleanContent(AosBitmap* bmp)
{
	if(mNodeNum)
	{
		if(mBitmap)
		{
			memset(mBitmap,0,eBitmapByteSize);
		}
		if(mRsltmap)
		{
			for(u16 i = 0;i < eMaxRsltSize;i++)
			{
				if(mRsltmap[i])
				{
					bmp->returnNode(mRsltmap[i]);
					mRsltmap[i] = 0;
				}
			}
		}
		mNodeNum = 0;
	}
}


void
AosBitmapNode::countAnd(AosBitmapNode* noderslt,AosBitmapNode* node1,AosBitmapNode* node2)
{
	if(!noderslt || !node1 || !node2)
	{
		return;
	}
	noderslt->mNodeLevel = 0;
	u64 * rslt = noderslt->mBitmap;
	u64 * a = 	 node1->mBitmap;
	u64 * b = 	 node2->mBitmap;
	noderslt->mFilter = node1->mFilter;
	noderslt->mNodeNum = 0;
	for(int i = 0;i < eBitmapSize;i++)
	{
		if((rslt[i] = (a[i] & b[i])))
		{
			noderslt->mNodeNum += countBit(rslt[i]);
		}
	}
	return;
}

void 
AosBitmapNode::clone(AosBitmapNode* noderslt,AosBitmapNode* node)
{
	if (!noderslt || !node)
	{
		return;
	}
	aos_assert(node->mNodeLevel == 0);
	noderslt->mNodeLevel = 0;
	noderslt->mFilter = node->mFilter;
	noderslt->mNodeNum = node->mNodeNum;
	aos_assert(node->mBitmap);
	memcpy(noderslt->mBitmap,node->mBitmap,2048);

	return;
}

	void
AosBitmapNode::appendNode(const AosBitmapNode* node)
{
	if(!node)return;
	mNodeNum = 0;
	for(u16 i = 0;i <= 0xff;i++)
	{
		mBitmap[i] |= node->mBitmap[i];
		mNodeNum += countBit(mBitmap[i]);
	}
}

	void
AosBitmapNode::countOr(AosBitmapNode* noderslt,AosBitmapNode* node1,AosBitmapNode* node2)
{
	if(!noderslt || !node1 || !node2)
	{
		return;
	}
	noderslt->mNodeLevel = 0;
	u64 * rslt = noderslt->mBitmap;
	u64 * a = 	 node1->mBitmap;
	u64 * b = 	 node2->mBitmap;
	noderslt->mFilter = node1->mFilter;
	noderslt->mNodeNum = 0;
	for(int i = 0;i < eBitmapSize;i++)
	{
		if((rslt[i] = (a[i] | b[i])))
		{
			noderslt->mNodeNum += countBit(rslt[i]);
		}
	}
	return;
}

void 
AosBitmapNode::countAndBlock(AosBitmapNode* noderslt, AosBitmapNode** nodes, const u64 num_nodes)
{
	OmnNotImplementedYet;
	if(!noderslt || !nodes || num_nodes<2)
	{
		return;
	}
	for(u64 i=0; i<num_nodes; i++)
	{
		if(!nodes[i])
			return;
	}

	noderslt->mNodeLevel = 0;
	noderslt->mNodeNum = 0;
	noderslt->mFilter = nodes[0]->mFilter;
	u64 * rslt = noderslt->mBitmap;
	for(u64 i=0; i<eBitmapSize; i++)
	{
		rslt[i] = nodes[0]->mBitmap[i];
		for(u64 index_node=1; index_node<num_nodes; index_node++)
		{
			rslt[i] = rslt[i] & (nodes[index_node]->mBitmap[i]);
			if(rslt[i] == 0)
				break;
		}

		if(rslt[i])
			noderslt->mNodeNum += countBit(rslt[i]);
	}

}

void 
AosBitmapNode::countOrBlock(AosBitmapNode* noderslt, AosBitmapNode** nodes, const u64 num_nodes)
{
	OmnNotImplementedYet;
	if(!noderslt || !nodes || num_nodes<1)
	{
		OmnAlarm << "Error: " << num_nodes << enderr;
		return;
	}

	if (num_nodes == 1)
	{
		noderslt = nodes[0];
		return;
	}

	for(u64 i=0; i<num_nodes; i++)
	{
		if(!nodes[i]) return;
	}

	noderslt->mNodeLevel = 0;
	noderslt->mNodeNum = 0;
	noderslt->mFilter = nodes[0]->mFilter;
	u64 * rslt = noderslt->mBitmap;
	for(u64 i=0; i<eBitmapSize; i++)
	{
		rslt[i] = nodes[0]->mBitmap[i];
		for(u64 index_node=1; index_node<num_nodes; index_node++)
		{
			rslt[i] = rslt[i] | (nodes[index_node]->mBitmap[i]);
		}

		if(rslt[i]) noderslt->mNodeNum += countBit(rslt[i]);
	}
}

bool
AosBitmapNode::getDocids(vector<u64> &docids)
{
	aos_assert_r(mNodeLevel == 0, false);
	int j = 0;
	u64 dd = 0;
	for(int i = 0;i < 256;i++)
	{
		if(mBitmap[i])
		{
			dd = mBitmap[i];
			for(j = 0;j < 64;j++)
			{
				if((dd >> j) & 1)
				{
					docids.push_back(mFilter + (i<<6) + j);
				}
			}
		}
	}                                                                                                                          

	return true;
}

u64
AosBitmapNode::getMemorySize(u64 &size) const
{
	if (mNodeLevel == 0)
	{
		size += sizeof(u64) * eBitmapSize;
		return size;
	}
	size += sizeof(AosBitmapNode*[eMaxRsltSize]);
	for (int i=0; i<eMaxRsltSize; i++)
	{
		if (mRsltmap[i]) mRsltmap[i]->getMemorySize(size);
	}
	return size;
}

bool
AosBitmapNode::getSections(
		AosBitmapObj::map_t &bitmaps,
		const AosRundataPtr &rdata)
{

	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
	if (mNodeLevel == 0)
	{
		if (mNodeNum <= 0) 
		{
			return true;
		}

		u32 section_id = AosBitmapObj::docid2SectionId(mFilter);
		AosBitmapNode *node = bitmap->getNode(0);
		clone(node, this);
		AosBitmapObjPtr bitmap = AosGetBitmap();
		bitmap->addLeafNode(node);

		AosBitmapObj::mapitr_t itr = bitmaps.find(section_id);
		if(itr == bitmaps.end())
		{
			bitmaps[section_id] = bitmap;
		}
		else
		{
			AosBitmapObjPtr bb = AosBitmap::countOr(bitmaps[section_id], bitmap);
			AosBitmap::exchangeContent(bb, bitmaps[section_id]);
			AosBitmapMgr::getSelf()->returnBitmap(bb);
		}

		return true;
	}

	for (int i=0; i<eMaxRsltSize; i++)
	{
		if (mRsltmap[i]) 
		{
			if (mRsltmap[i]->mNodeNum > 0)
			{
				mRsltmap[i]->getSections(bitmaps, rdata);
			}
		}
	}

	return true;
}

void
AosBitmapNode::removeBits(const AosBitmapNode* node)
{
	if(!node)return;
	mNodeNum = 0;
	for(u16 i = 0;i <= 0xff;i++)
	{
		mBitmap[i] &= (~(node->mBitmap[i]));
		mNodeNum += countBit(mBitmap[i]);
	}
}

bool
AosBitmapNode::isEmpty() const
{
	if(mBitmap)
	{
		for(int i = 0;i < 256;i ++)
		{
			if(mBitmap[i])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}


bool
AosBitmapNode::loadFromBuff(const AosBuffPtr &buff,AosBitmapNode* cur_node, char* load_buff)
{
	u8 node_type = buff->getU8(255);
	if(node_type == 0xff)
	{
		return false;
	}
	u64 filter = buff->getU64(0xffffffffffffffffULL);
	return loadFromBuff(buff,cur_node, load_buff, node_type, filter);
}

bool
AosBitmapNode::loadFromBuff(const AosBuffPtr &buff,AosBitmapNode* cur_node, char* load_buff, const u8 &node_type, const u64 &filter)
{	
	cur_node->mFilter = filter;
	bool rslt = true;
	u8 bytes = 0;
	u8* cursor_a = 0;
	u64* cursor_b = 0;
	u16 i = 0;
	u16 count = 0;

	// 1. 0 bit(nothing will be put into buff)
	// 2. 1 bit
	// 3. 1 u64
	// 4. n u64(blank node > 29)
	// 5. all u64
	// 6. n not_full(full node > 29)
	// 7. 1 not_full
	// 8. 1 bit is 0
	// 9. all full
	switch(node_type)
	{
		case 1:
			OmnShouldNeverComeHere;
			break;
		case 9:
			// 9. all full
			memset(cur_node->mBitmap,0xff,2048);
			cur_node->mNodeNum = 16384;
			break;
		
		case 2:
			// 2. 1 bit
			memset(cur_node->mBitmap,0,2048);
			cur_node->mBitmap[((cur_node->mFilter >> 6) & 0xffULL)] = (1ULL << (cur_node->mFilter & 0x3fULL));
			cur_node->mFilter &= 0xffffffffffffc000ULL;
			cur_node->mNodeNum = 1;
			break;
		case 3:
			// 3. 1 u64
			i = buff->getU8(0);
			//memset(cur_node->mBitmap,0,2048);
			cur_node->mBitmap[i] = buff->getU64(0);
			cur_node->mNodeNum = AosBitmapNode::countBit(cur_node->mBitmap[i]);
			break;

		case 8:
			// 8. 1 bit is 0
			memset(cur_node->mBitmap,0xff,2048);
			cur_node->mBitmap[(cur_node->mFilter >> 6 & 0xffULL)] = (~(1ULL << (cur_node->mFilter & 0x3fULL)));
			cur_node->mFilter &= 0xffffffffffffc000ULL;
			cur_node->mNodeNum = 16383;
			break;
		case 7:
			// 7. 1 not_full
			i = buff->getU8(0);
			memset(cur_node->mBitmap,0xff,2048);
			cur_node->mBitmap[i] = buff->getU64(0);
			cur_node->mNodeNum = 16320 + AosBitmapNode::countBit(cur_node->mBitmap[i]);
			break;
			
		case 5:
			// 5. all u64
			rslt = buff->getBuff((char*)cur_node->mBitmap,(int)AosBitmapNode::eBitmapByteSize);
			aos_assert_r(rslt, false);
			count = 0;
			for(i = 0;i < 256;i++)
			{
				count += AosBitmapNode::countBit(cur_node->mBitmap[i]);
			}
			cur_node->mNodeNum = count;
			break;
			
		case 4:
			// 4. n u64(blank node > 29)
			//memset(cur_node->mBitmap,0,2048);
			bytes = buff->getU8(0);
			rslt = buff->getBuff((char*)load_buff,9*bytes);
			cursor_a = (u8*)load_buff;
			cursor_b = (u64*)(load_buff+1);
			count = 0;
			for(i=0;i<bytes;i++)
			{
				cur_node->mBitmap[(*cursor_a)] 
					= (*cursor_b);
				count += AosBitmapNode::countBit((*cursor_b));
				cursor_a += 9;
				cursor_b =(u64*)((u8*)cursor_b + 9);
			}				
			cur_node->mNodeNum = count;
			break;
	
		case 6:
			// 6. n not_full(full node > 29)
			memset(cur_node->mBitmap,0xff,2048);
			bytes = buff->getU8(0);
			rslt = buff->getBuff((char*)load_buff,9*bytes);
			cursor_a = (u8*)load_buff;
			cursor_b = (u64*)(load_buff+1);
			count = 0;
			for(i=0;i<bytes;i++)
			{
				cur_node->mBitmap[(*cursor_a)] 
					= (*cursor_b);
				count += AosBitmapNode::countBit((*cursor_b));
				cursor_a += 9;
				cursor_b =(u64*)((u8*)cursor_b + 9);
			}				
			cur_node->mNodeNum = count + ((256-bytes) << 6);
			break;
			
		default:
			aos_assert_r(false,false);
	}
	
	return true;
}

void
AosBitmapNode::copyNode(AosBuff* target,AosBuff* source,const u8 &node_type,const u64 &filter)
{
	target->setU8(node_type);
	target->setU64(filter);
	
	switch(node_type)
	{
		case 1:
			OmnShouldNeverComeHere;
			break;
		case 9:
			// 9. all full
			return;

		case 2:
			// 2. 1 bit
			return;

		case 3:
			// 3. 1 u64
			target->setBuff(source->getCrtPos(),9);
			source->incIndex(9);
			return;

		case 8:
			// 8. 1 bit is 0
			return;

		case 7:
			// 7. 1 not_full
			target->setBuff(source->getCrtPos(),9);
			source->incIndex(9);
			return;
			
		case 5:
			// 5. all u64
			target->setBuff(source->getCrtPos(),AosBitmapNode::eBitmapByteSize);
			source->incIndex(AosBitmapNode::eBitmapByteSize);
			return;
			
		case 4:
			// 4. n u64(blank node > 29)
			//memset(cur_node->mBitmap,0,2048);
			{
				u8 bytes = source->getU8(0);
				target->setU8(bytes);
				target->setBuff(source->getCrtPos(),bytes*9);
				source->incIndex(bytes*9);
				return;
			}
		case 6:
			// 6. n not_full(full node > 29)
			{
				u8 bytes = source->getU8(0);
				target->setU8(bytes);
				target->setBuff(source->getCrtPos(),bytes*9);
				source->incIndex(bytes*9);
				return;
			}

		default:
			aos_assert(false);
	}
	
	return;
}

void
AosBitmapNode::jumpNode(AosBuff* source,const u8 &node_type,const u64 &filter)
{
	switch(node_type)
	{
		case 1:
			OmnShouldNeverComeHere;
			break;
		case 9:
			// 9. all full
			return;

		case 2:
			// 2. 1 bit
			return;

		case 3:
			// 3. 1 u64
			source->incIndex(9);
			return;

		case 8:
			// 8. 1 bit is 0
			return;

		case 7:
			// 7. 1 not_full
			source->incIndex(9);
			return;
			
		case 5:
			// 5. all u64
			source->incIndex(AosBitmapNode::eBitmapByteSize);
			return;
			
		case 4:
			// 4. n u64(blank node > 29)
			//memset(cur_node->mBitmap,0,2048);
			{
				u8 bytes = source->getU8(0);
				source->incIndex(bytes*9);
				return;
			}
		case 6:
			// 6. n not_full(full node > 29)
			{
				u8 bytes = source->getU8(0);
				source->incIndex(bytes*9);
				return;
			}

		default:
			aos_assert(false);
	}
	
	return;
}


void			
AosBitmapNode::countOr(const AosBitmapNode* node)
{
	mNodeNum = 0;
	for(int i = 0;i < 256;i++)
	{
		mBitmap[i] |= node->mBitmap[i];
		mNodeNum += countBit(mBitmap[i]);
	}
}

void			
AosBitmapNode::countAnd(const AosBitmapNode* node)
{
	mNodeNum = 0;
	for(int i = 0;i < 256;i++)
	{
		mBitmap[i] &= node->mBitmap[i];
		mNodeNum += countBit(mBitmap[i]);
	}
}
