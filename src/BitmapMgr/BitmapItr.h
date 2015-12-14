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
#ifndef Aos_BitmapMgr_BitmapItr_h
#define Aos_BitmapMgr_BitmapItr_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/BitmapNode.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosBitmapItr : public OmnRCObject
{
	OmnDefineRCObject;
public:
	u8					mStatus; // 0: init 1: normal 2: finished
	AosBitmapNode* 		mNodes[8];
	u16 				mPos[8];
	u16 				mU64Pos;
	u8 					mBitPos;
	u8					mLayer;

public:
	AosBitmapItr()
	:
	mStatus(0),
	mU64Pos(0),
	mBitPos(0),
	mLayer(0)
	{
		memset(mNodes,0,8*sizeof(AosBitmapNode*));
		memset(mPos,0,8);
	}
	
	void reset()
	{
		mStatus = 0;
		mU64Pos = 0;
		mBitPos = 0;
		mLayer = 0;
		memset(mNodes,0,8*sizeof(AosBitmapNode*));
		memset(mPos,0,8);
	}

	bool lastDoc(u64 &docid)
	{
		if(mNodes[0] == 0)
		{
			mStatus = 2;
			return false;
		}
		mStatus = 1;
		AosBitmapNode* cur_node = lastLeaf();
		if(!cur_node)
		{
			mStatus = 2;
			return false;
		}
		// check if the cur_node is null
		bool rslt = lastDocInLeaf(cur_node, docid);
		while(!rslt)
		{
			cur_node = prevLeaf();
			if(!cur_node)
			{
				mStatus = 2;
				return false;
			}
			rslt = lastDocInLeaf(cur_node, docid);
		}
		return rslt;
	}

	// Chen Ding, 2013/08/30
	bool nextDocid(u64 &docid)
	{
		if(mStatus == 2) return false;
		if(mStatus == 0) 
		{
			return firstDoc(docid);
		}

		// status == 1
		// first , get doc from current node
		AosBitmapNode* cur_node = mNodes[mLayer];
		aos_assert_r(cur_node->mNodeLevel == 0, false);
		
		
		//find the next bit in the leaf node
		if (nextDocidInLeaf(cur_node, docid)) return true;
		
		//find next leaf
		cur_node = nextLeaf();
		while(1)
		{
			if(!cur_node)
			{
				mStatus = 2;
				return false;
			}
			if(cur_node->mNodeNum!=0)
			{
				break;
			}
			cur_node = nextLeaf();
		}
		return firstDocInLeaf(cur_node, docid);
	}

	// Chen Ding, 2013/08/30
	bool prevDocid(u64 &docid)
	{
		if(mStatus == 2) return false;
		if(mStatus == 0) 
		{
			return lastDoc(docid);
		}

		// status == 1
		// first , get doc from current node
		AosBitmapNode* cur_node = mNodes[mLayer];
		aos_assert_r(cur_node->mNodeLevel == 0, false);
		
		
		//find the next bit in the leaf node
		if (prevDocidInLeaf(cur_node, docid)) return true;
		
		//find next leaf
		cur_node = prevLeaf();
		while(1)
		{
			if(!cur_node)
			{
				mStatus = 2;
				return false;
			}
			if(cur_node->mNodeNum != 0)
			{
				break;
			}
			cur_node = prevLeaf();
		}
		return lastDocInLeaf(cur_node, docid);
	}

	AosBitmapNode* nextLeaf()
	{
		if(mStatus == 1)
		{
			while(mLayer > 0)
			{
				while(1)
				{
					if((++mPos[mLayer]) > 0xff)
					{
						break;
					}

					if(mNodes[mLayer-1]->mRsltmap[ mPos[mLayer] ])
					{
						// find the node
						mNodes[mLayer] = mNodes[mLayer-1]->mRsltmap[ mPos[mLayer] ];
						while(mNodes[mLayer]->mNodeLevel > 0)
						{
							mPos[mLayer +1] = 256;
							for(int i = 0;i <= 0xff;i++)
							{
								if(mNodes[mLayer]->mRsltmap[i])
								{
									mPos[mLayer +1] = i;
									mNodes[mLayer+1] = mNodes[mLayer]->mRsltmap[i];
									break;
								}
							}
							
							mLayer ++;
							aos_assert_r(mPos[mLayer] < 256,0);
						}
						return mNodes[mLayer];
					}
				}
				mLayer --;
			}
			mStatus = 2;
			return 0;
		}
		else if(mStatus == 2)
		{
			return 0;
		}
		else if(mStatus == 0)
		{
			if(!mNodes[0])
			{
				mStatus = 2;
				return 0;
			}
			mStatus = 1;
			return firstLeaf();
		}
		
		// should never come here.
		return 0;
	}

private:
	AosBitmapNode* lastLeaf()
	{
		AosBitmapNode* cur_node = 0;
		while(mNodes[mLayer]->mNodeLevel > 0)
		{
			cur_node = mNodes[mLayer];
			mNodes[mLayer+1] = 0;
			for(u16 i = 0xff;i >= 0;i--)
			{
				if(cur_node->mRsltmap[i])
				{
					mNodes[mLayer+1] = cur_node->mRsltmap[i];
					mPos[mLayer+1] = i;
					break;
				}
			}
			mLayer ++;
			aos_assert_r(mNodes[mLayer],0);
		}
		aos_assert_r(mNodes[mLayer]->mNodeLevel == 0, 0)
		return mNodes[mLayer];
	}

	AosBitmapNode* firstLeaf()
	{
		AosBitmapNode* cur_node = 0;
		while(mNodes[mLayer]->mNodeLevel > 0)
		{
			cur_node = mNodes[mLayer];
			mNodes[mLayer+1] = 0;
			for(u16 i = 0;i <= 0xff;i++)
			{
				if(cur_node->mRsltmap[i])
				{
					mNodes[mLayer+1] = cur_node->mRsltmap[i];
					mPos[mLayer+1] = i;
					break;
				}
			}
			mLayer ++;
			aos_assert_r(mNodes[mLayer],0);
		}
		aos_assert_r(mNodes[mLayer]->mNodeLevel == 0, 0)
		return mNodes[mLayer];
	}

	// Chen Ding, 2013/08/30
	bool firstDoc(u64 &docid)
	{
		if(mNodes[0] == 0)
		{
			mStatus = 2;
			return false;
		}
		mStatus = 1;
		AosBitmapNode* cur_node = firstLeaf();
		if(!cur_node)
		{
			mStatus = 2;
			return false;
		}
		// check if the cur_node is null
		bool rslt = firstDocInLeaf(cur_node, docid);
		while(!rslt)
		{
			cur_node = nextLeaf();
			if(!cur_node)
			{
				mStatus = 2;
				return false;
			}
			rslt = firstDocInLeaf(cur_node, docid);
		}
		return rslt;
	}


	AosBitmapNode* prevLeaf()
	{
		if(mStatus == 1)
		{
			while(mLayer > 0)
			{
				while(1)
				{
					if((--mPos[mLayer]) > 0xff)
					{
						break;
					}

					if(mNodes[mLayer-1]->mRsltmap[ mPos[mLayer] ])
					{
						// find the node
						mNodes[mLayer] = mNodes[mLayer-1]->mRsltmap[ mPos[mLayer] ];
						while(mNodes[mLayer]->mNodeLevel > 0)
						{
							mPos[mLayer +1] = 256;
							for(int i = 0xff;i >= 0;i--)
							{
								if(mNodes[mLayer]->mRsltmap[i])
								{
									mPos[mLayer +1] = i;
									mNodes[mLayer+1] = mNodes[mLayer]->mRsltmap[i];
									break;
								}
							}
							
							mLayer ++;
							aos_assert_r(mPos[mLayer] < 256,0);
						}
						return mNodes[mLayer];
					}
				}
				mLayer --;
			}
			mStatus = 2;
			return 0;
		}
		else if(mStatus == 2)
		{
			return 0;
		}
		else if(mStatus == 0)
		{
			if(!mNodes[0])
			{
				mStatus = 2;
				return 0;
			}
			mStatus = 1;
			return firstLeaf();
		}
		
		// should never come here.
		return 0;
	}

	bool firstDocInLeaf(const AosBitmapNode* cur_node, u64 &docid)
	{
		for(u16 i = 0;i <= 0xff;i++)
		{
			if(cur_node->mBitmap[i])
			{
				// found the first one
				mU64Pos = i;
				mBitPos = AosBitmapNode::findLastBit(cur_node->mBitmap[i]);
				docid = cur_node->mFilter|(i<<6)|mBitPos;
				return true;
			}
		}
		
		// should not come here
		//mStatus = 2;
		return false;
	}

	bool lastDocInLeaf(const AosBitmapNode* cur_node, u64 &docid)
	{
		for(i32 i = 0xff;i >=0 ;i--)
		{
			if(cur_node->mBitmap[i])
			{
				// found the first one
				mU64Pos = i;
				mBitPos = AosBitmapNode::findFirstBit(cur_node->mBitmap[i]);
				docid = cur_node->mFilter|(i<<6)|mBitPos;
				return true;
			}
		}
		
		// should not come here
		//mStatus = 2;
		return false;
	}

	bool nextDocidInLeaf(const AosBitmapNode* cur_node, u64 &docid)
	{
		if(mBitPos < 63)
		{
			u64 value = cur_node->mBitmap[mU64Pos];
			value = value >> (mBitPos+1);
			//1. find in the same u64
			if(value)
			{
				mBitPos += (AosBitmapNode::findLastBit(value) +1);
				docid = mNodes[mLayer]->mFilter|(mU64Pos<<6)|mBitPos;
				return true;
			}
		}
		//1. find in other u64
		while(1)
		{
			mU64Pos ++;
			
			if(mU64Pos > 0xff)
			{
				return false;
			}
			if(cur_node->mBitmap[mU64Pos])
			{
				mBitPos = AosBitmapNode::findLastBit(cur_node->mBitmap[mU64Pos]);
				docid = mNodes[mLayer]->mFilter|(mU64Pos<<6)|mBitPos;
				return true;
			}
		}
		// should never come here
		return false;
	}

	bool prevDocidInLeaf(const AosBitmapNode* cur_node, u64 &docid)
	{
		if(mBitPos > 0)
		{
			u64 value = cur_node->mBitmap[mU64Pos];
			u16 delta = 64 - mBitPos;
			value = value << delta;
			//1. find in the same u64
			if(value)
			{
				mBitPos = AosBitmapNode::findFirstBit(value) - delta;
				docid = mNodes[mLayer]->mFilter|(mU64Pos<<6)|mBitPos;
				return true;
			}
		}
		//1. find in other u64
		while(mU64Pos > 0)
		{
			mU64Pos --;
			if(cur_node->mBitmap[mU64Pos])
			{
				mBitPos = AosBitmapNode::findFirstBit(cur_node->mBitmap[mU64Pos]);
				docid = mNodes[mLayer]->mFilter|(mU64Pos<<6)|mBitPos;
				return true;
			}
		}
		mU64Pos --;
		// should never come here
		return false;
	}

	u64 crtDocidInLeaf(const AosBitmapNode* cur_node)
	{
		return mNodes[mLayer]->mFilter|(mU64Pos<<6)|mBitPos;
	}

	bool findLeafByFilter(const u64 &filter)
	{
		AosBitmapNode* cur_node = 0;
		while(mNodes[mLayer]->mNodeLevel > 0)
		{
			cur_node = mNodes[mLayer];
			mLayer ++;
			mPos[mLayer] = filter >> (14 + cur_node->mNodeLevel * 8) & 0xffULL;
			
			mNodes[mLayer] = cur_node->mRsltmap[mPos[mLayer]];
			
			if(!mNodes[mLayer])
			{
				return false;
			}
		}
		aos_assert_r(mNodes[mLayer]->mNodeLevel == 0, 0)
		return true; 
	}

	AosBitmapNode* nextLeaf(const u64 &filter)
	{
		mStatus = 1;
		AosBitmapNode* cur_node = 0;
		//1. find current pos
		while(mNodes[mLayer]->mNodeLevel > 0)
		{
			cur_node = mNodes[mLayer];
			mLayer ++;
			mPos[mLayer] = filter >> (14 + cur_node->mNodeLevel * 8) & 0xffULL;
			
			mNodes[mLayer] = cur_node->mRsltmap[mPos[mLayer]];
			
			if(!mNodes[mLayer])
			{
				break;
			}
		}
		//2. find next pos
		//2.1 find back if no more kids exist
		bool can_go_down = false;
		int i;
		while(!can_go_down)
		{
			for(i = mPos[mLayer];i < 256;i++)
			{
				if(cur_node->mRsltmap[i])
				{
					// found it
					mPos[mLayer] = i;
					mNodes[mLayer] = cur_node->mRsltmap[i];
					can_go_down = true;
					break;
				}
			}

			if(can_go_down)break;
			
			// no more kids
			mLayer --;
			if(mLayer <= 0)
			{
				mStatus = 2;
				return 0;
			}
		}

		while(mNodes[mLayer]->mNodeLevel > 0)
		{
			cur_node = mNodes[mLayer];
			mNodes[mLayer+1] = 0;
			for(u16 i = 0;i <= 0xff;i++)
			{
				if(cur_node->mRsltmap[i])
				{
					mNodes[mLayer+1] = cur_node->mRsltmap[i];
					mPos[mLayer+1] = i;
					break;
				}
			}
			mLayer ++;
			aos_assert_r(mNodes[mLayer],0);
		}
		aos_assert_r(mNodes[mLayer]->mNodeLevel == 0, 0)
		return mNodes[mLayer];
	}
public:
	bool getCrtDocid(u64 &docid)
	{
		if(mStatus == 2) return false;
		if(mStatus == 0) return false;

		docid = crtDocidInLeaf(mNodes[mLayer]);
		return true;
	}
	
	bool moveTo(const u64 &docid, u64 &cur_docid, const bool reverse)
	{
		mStatus = 1;

		u64 value = docid >> 6;
		int status = 0;// status: 0 match, 1 looking forward, 2 looking backward
		int index = 0;
		while(mNodes[mLayer]->mNodeLevel > 0)
		{
			if(status == 0)
			{
				index = ((u8*)(&value))[mNodes[mLayer]->mNodeLevel];
				if(!mNodes[mLayer]->mRsltmap[index])
				{
					status = 1;
					bool found = false;
					// look forward
					for(int x = index;x >= 0 && x <= 0xff;)
					{
						if(mNodes[mLayer]->mRsltmap[x])
						{
							found = true;
							index = x;
							break;
						}
						if(reverse)
						{
							x--;
						}
						else
						{
							x++;
						}
					}
					// look backward
					if(!found)
					{
						status = 2;
						for(; index >= 0 && index <= 0xff;)
						{
							if(mNodes[mLayer]->mRsltmap[index])
							{
								found = true;
								break;
							}
							if(reverse)
							{
								index++;
							}
							else
							{
								index--;
							}
						}
					}

					if(!found)
					{
						mStatus = 2;
						return false;
					}
				}
				mPos[mLayer+1] = index;
				mNodes[mLayer+1] = mNodes[mLayer]->mRsltmap[index];
				mLayer++;
				continue;
			}

			if((status == 1 && !reverse)||(status == 2 && reverse))
			{
				for(index = 0;index <= 0xff;index ++)
				{
					if(mNodes[mLayer]->mRsltmap[index])
					{
						break;
					}
				}
				if(index < 0 || index > 0xff)
				{
					mStatus = 2;
					return false;
				}

				mPos[mLayer+1] = index;
				mNodes[mLayer+1] = mNodes[mLayer]->mRsltmap[index];
				mLayer ++;
				continue;
			}

			if((status == 1 && reverse)||(status == 2 && !reverse))
			{
				for(index = 0xff;index >= 0;index --)
				{
					if(mNodes[mLayer]->mRsltmap[index])
					{
						break;
					}
				}
				if(index < 0 || index > 0xff)
				{
					mStatus = 2;
					return false;
				}

				mPos[mLayer+1] = index;
				mNodes[mLayer+1] = mNodes[mLayer]->mRsltmap[index];
				mLayer ++;
				continue;
			}
		}
		
		aos_assert_r(mNodes[mLayer]->mNodeLevel == 0,false);
		if(status == 0)
		{
			mU64Pos = (docid >> 6) & 0xff; 
			mBitPos = docid & 0x3f;
			if(mNodes[mLayer]->mBitmap[mU64Pos] & (1ULL << mBitPos))
			{
				cur_docid = docid;
				return true;
			}
		
			if(reverse)
			{
				return prevDocid(cur_docid);
			}
			else
			{
				return nextDocid(cur_docid);
			}	
		}

		if(status == 2)
		{
			if(reverse)
			{
				prevLeaf();
			}
			else
			{
				nextLeaf();
			}
			if(mStatus == 2)
			{
				return false;
			}
		}

		// found the next leaf
		if(reverse)
		{
			return lastDocInLeaf(mNodes[mLayer], cur_docid);
		}
		else
		{
			return firstDocInLeaf(mNodes[mLayer], cur_docid);				
		}
		
		return false;
	}
	

};
#endif
