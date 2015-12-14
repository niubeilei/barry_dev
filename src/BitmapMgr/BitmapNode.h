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
#ifndef Aos_BitmapMgr_BitmapNode_h
#define Aos_BitmapMgr_BitmapNode_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "BitmapMgr/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <hash_map>
#include "SEInterfaces/BitmapObj.h"

class AosBitmap;
	
class AosBitmapNode : public OmnRCObject
{
	OmnDefineRCObject;
	enum
	{
//		eMaxLevel = 3,
		eMaxRsltSize = 0x100,
		eMaxRsltByteSize = sizeof(AosBitmapNode*)*eMaxRsltSize,
		eBitmapSize = 0x100,
		eBitmapByteSize = sizeof(u64)*eBitmapSize,
		eSaveRsltByteSize = sizeof(u32)*eMaxRsltSize,
//		eBitmapLen0 = 6,
//		eBitmapLen1 = 11,
//		eBitmapLen01 = 17,
//		eBitmapLen2 = 10,
//		eRsltAddrMask = 0x03ff,
//		eBitmapBitMask = 0x3f,
//		eBitmapAddrMask = 0x7ff,

	};

public:
	u8				mNodeLevel;
	u64*			mBitmap;
	AosBitmapNode**	mRsltmap;
	u64				mFilter;
	u16				mNodeNum;
	char			mBuff[8*256];
	static u8		smBitFindingList[0x10000];
	static u8		smBitRevFindingList[0x10000];
	static u8		smBitCountList[0x10000];

public:
	// Chen Ding, 2013/10/26
	AosBitmapNode(const u8 level);
	~AosBitmapNode();

	void 			init(const u8 &level)
	{
		memset (this,0,sizeof(AosBitmapNode));
		mNodeLevel = level;
		if(level == 0)
		{
			mBitmap = (u64*)(mBuff);
		}
		else
		{
			mRsltmap = (AosBitmapNode**)(mBuff);
		}
	}


	void			countOr(const AosBitmapNode* node);
	void			countAnd(const AosBitmapNode* node);
	
	bool 			countRsltOr(const AosBitmapObjPtr &b);
	AosBitmapNode* 	countRsltAnd(const AosBitmapObjPtr &b);
	bool 			include(const AosBitmapNode * node2)
	{
		aos_assert_r(mNodeLevel == 0 && node2->mNodeLevel == 0,false);
		if(mFilter != node2->mFilter) return false;
		for(int i = 0;i < 256;i++)
		{
			if(((~mBitmap[i])& (node2->mBitmap[i])) != 0)
			{
				return false;
			}
		}
		return true;
	}

	bool 			sameAs(const AosBitmapNode * node2)
	{
		aos_assert_r(mNodeLevel == 0 && node2->mNodeLevel == 0,false);
		if(mFilter != node2->mFilter) return false;
		for(int i = 0;i < 256;i++)
		{
			if(mBitmap[i] != node2->mBitmap[i])
			{
				return false;
			}
		}
		return true;
	}

	void cleanContent(AosBitmap* bmp);

	void clean(AosBitmap* bmp)
	{
		cleanContent(bmp);
		mFilter = 0;
	}

	void	saveToBuff(
				const AosBuffPtr &buff,
				char *value_buff)
	{
		// we have  types:
		// 1. 0 bit(nothing will be put into buff)
		// 2. 1 bit
		// 3. 1 u64
		// 4. n u64(blank node > 52)
		// 5. all u64
		// 6. n not_full(full node > 52)
		// 7. 1 not_full
		// 8. 1 bit is 0
		// 9. all full
		if(mNodeLevel == 0)
		{
			int i = 0;
			int count1 = 0;
			int count2 = 0;
			u16 pos = 0;

			for(;i <= 0xff;i++)
			{
				if(mBitmap[i] == 0)count1++;
				if(mBitmap[i] == 0xffffffffffffffffULL)count2++;
			}
			
			// 1. 0 bit
			if(count1 == 256)
			{
				return;
			}
			
			// 9. all full
			if(count2 == 256)
			{
				buff.getPtrNoLock()->setU8(9);
				buff.getPtrNoLock()->setU64(mFilter);
				return;
			}

			// 2. 1 bit
			// 3. 1 u64
			if(count1 == 255)
			{
				for(i = 0;i < 256;i++)
				{
					if(mBitmap[i] != 0)
					{
						if(countBit(mBitmap[i]) == 1)
						{
							// 2. 1 bit
							buff.getPtrNoLock()->setU8(2);
							pos = 0;
							while(1)
							{
								if(((mBitmap[i] >> pos) & 1) == 1)break;
								pos ++;
							}							
							pos = (i << 6) + pos;
							buff.getPtrNoLock()->setU64(mFilter | pos);
							return;
						}
						// 3. 1 u64
						buff.getPtrNoLock()->setU8(3);
						buff.getPtrNoLock()->setU64(mFilter);
						buff.getPtrNoLock()->setU8(i);
						buff.getPtrNoLock()->setU64(mBitmap[i]);
						return;											
					}
				}
			}
			
			// 7. 1 not_full
			// 8. 1 bit is 0
			if(count2 == 255)
			{
				for(i = 0;i < 256;i++)
				{
					if(mBitmap[i] != 0xffffffffffffffffULL)
					{
						if(countBit(mBitmap[i]) == 63)
						{
							// 2. 1 bit
							buff.getPtrNoLock()->setU8(8);
							pos = 0;

							// Chen Ding, 2013/11/03
							// while(1)
							// {
							// 	if(((mBitmap[i] >> pos) | 0) == 0)break;
							// 	pos ++;
							// }							
							while(pos < 64)
							{
								if(((mBitmap[i] >> pos) & 0x01) == 0)break;
								pos ++;
							}							
							pos = ((i << 6) + pos);
							buff.getPtrNoLock()->setU64(mFilter | pos);
							return;
						}
						// 3. 1 u64
						buff.getPtrNoLock()->setU8(7);
						buff.getPtrNoLock()->setU64(mFilter);
						buff.getPtrNoLock()->setU8(i);
						buff.getPtrNoLock()->setU64(mBitmap[i]);
						return;											
					}
				}
			}

			// 5. all u64
			if(count1 < 29 && count2 < 29)
			{
				buff.getPtrNoLock()->setU8(5);
				buff.getPtrNoLock()->setU64(mFilter);
				buff.getPtrNoLock()->setBuff((char*)mBitmap, (int)eBitmapByteSize);			
				return;
			}
				
			// 4. n u64(blank node > 52)
			if(count1 > count2)
			{
				buff.getPtrNoLock()->setU8(4);
				buff.getPtrNoLock()->setU64(mFilter);
				buff.getPtrNoLock()->setU8(256 - count1);
				for(i = 0;i < 256;i++)
				{
					if(mBitmap[i])
					{
						buff.getPtrNoLock()->setU8(i);
						buff.getPtrNoLock()->setU64(mBitmap[i]);
					}
				}
			}
			else// 6. n not_full(full node > 52)
			{
				buff.getPtrNoLock()->setU8(6);
				buff.getPtrNoLock()->setU64(mFilter);
				buff.getPtrNoLock()->setU8(256 - count2);
				for(i = 0;i < 256;i++)
				{
					if(mBitmap[i] != 0xffffffffffffffffULL)
					{
						buff.getPtrNoLock()->setU8(i);
						buff.getPtrNoLock()->setU64(mBitmap[i]);
					}
				}
			}			
		}
		else // not a leaf node
		{
			// for(u16 i = 0;i <= 0xff;i++)
			for(u16 i=0; i<eMaxRsltSize; i++)
			{
				if (mRsltmap[i])
				{
					mRsltmap[i]->saveToBuff(buff, value_buff);
				}
			}
		}
	}

	static u8	findLastBit(const u64 &value)
	{
		const u16* ptr = (const u16*)&value;
		if(ptr[0])return smBitFindingList[ptr[0]];
		if(ptr[1])return smBitFindingList[ptr[1]]+16;
		if(ptr[2])return smBitFindingList[ptr[2]]+32;
		if(ptr[3])return smBitFindingList[ptr[3]]+48;
		return 0;
	}

	static u8	findFirstBit(const u64 &value)
	{
		const u16* ptr = (const u16*)&value;
		if(ptr[3])return smBitRevFindingList[ptr[3]]+48;
		if(ptr[2])return smBitRevFindingList[ptr[2]]+32;
		if(ptr[1])return smBitRevFindingList[ptr[1]]+16;
		if(ptr[0])return smBitRevFindingList[ptr[0]];
		return 0;
	}

    //added by navy to do (A AND !B)
	//To find the only bits in A but not in B.
    static void negateBitmap(AosBitmapNode* noderslt,AosBitmapNode* node1,AosBitmapNode* node2)
    {   
		if (!noderslt || !node1 || !node2)
		{
           return ;
		}
	    noderslt->mNodeLevel = 0;
 	    u64 * rslt = noderslt->mBitmap;
	    u64 * a = 	 node1->mBitmap;
	    u64 * b = 	 node2->mBitmap;	   
         				
	    noderslt->mFilter = node1->mFilter;
	    noderslt->mNodeNum = 0;

	    for(int i = 0;i < eBitmapSize;i++)	    
		{
			rslt[i] = a[i]&(~b[i]);
		    if(rslt[i])
		    {
			   noderslt->mNodeNum += countBit(rslt[i]);
		    }
	    }

		return ; 
    }

	static u8 countBit(const u64 &value)
	{
		return smBitCountList[ ((u16*)(&(value)))[0]]
				+ smBitCountList[ ((u16*)(&(value)))[1]]
				+ smBitCountList[ ((u16*)(&(value)))[2]]
				+ smBitCountList[ ((u16*)(&(value)))[3]];
	}

	void appendNode(const AosBitmapNode* node);
	void removeBits(const AosBitmapNode* node);

	static void countAnd(AosBitmapNode* noderslt,AosBitmapNode* node1,AosBitmapNode* node2);
	static void countOr(AosBitmapNode* noderslt,AosBitmapNode* node1,AosBitmapNode* node2);

	static void countAndBlock(AosBitmapNode* noderslt, AosBitmapNode** nodes, const u64 num_nodes);
	static void countOrBlock(AosBitmapNode* noderslt, AosBitmapNode** nodes, const u64 num_nodes);
    
	static void countAndBlock(AosBitmapNode* noderslt, const vector<AosBitmapNode*> &nodes) {};
	static void countOrBlock(AosBitmapNode* noderslt, const vector<AosBitmapNode*> &nodes) {};

	static void clone(AosBitmapNode* noderslt, AosBitmapNode* node);

	
    bool getSections(
		AosBitmapObj::map_t &bitmaps,
		const AosRundataPtr &rdata);
			

	
	
	


private:
	static void returnNode(AosBitmapNode* node);

public:
	// Chen Ding, 2013/02/11
	u64 getMemorySize(u64 &size) const;
	bool getDocids(vector<u64> &docids);
	bool isEmpty() const;
	static int getNumInstances();
	static bool loadFromBuff(const AosBuffPtr &buff,AosBitmapNode* cur_node, char* load_buff);
	static bool loadFromBuff(const AosBuffPtr &buff,AosBitmapNode* cur_node, char* load_buff, const u8 &type, const u64 &filter);
	static void copyNode(AosBuff* target,AosBuff* source,const u8 &type,const u64 &filter);
	static void jumpNode(AosBuff* source,const u8 &node_type,const u64 &filter);
};
#endif

