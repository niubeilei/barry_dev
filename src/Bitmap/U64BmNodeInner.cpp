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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Bitmap/U64BmNodeInner.h"

#include "Bitmap/U64BmNodeLeaf.h"


AosU64BmNodeInner::~AosU64BmNodeInner()
{
	u32 size = mSubnodes.size();
	for (u32 i=0; i<size; i++)
	{
		if (mSubnodes[i])
		{
			if (mSubnodes[i]->isLeafNode()) 
			{
				AosU64BmNodeLeaf::returnNode((AosU64BmNodeLeaf*)mSubnodes[i]);
			}
			else
			{
				returnNode((AosU64BmNodeInner*)mSubnodes[i]);
			}
		}
	}
	mSubnodes.clear();
	mIndex.clear();
}


AosU64BmNode *
AosU64BmNodeInner::setDocid(const u8 vv, const bool insert_leaf)
{
	mLock->lock();
	AosU64BmNode *node = 0;
	u32 size = 0;
	switch (mMode)
	{
	case AosBitmapUtil::ePacked:
		 size = mIndex.size();
		 if (size == 0)
		 {
			 mIndex.push_back(vv);
			 if (insert_leaf)
			 {
				 node = AosU64BmNodeLeaf::getNode();
			 }
			 else
			 {
				 node = AosU64BmNodeInner::getNode();
			 }
			 mSubnodes.push_back(node);
			 mLock->unlock();
			 return node;
		 }

		 if (size < eLinearThreshold)
		 {
			 // Will do linear search
		 	 for (u32 i=0; i<size; i++)
			 {
		 		if (mIndex[i] == vv)
		 		{
					node = mSubnodes[i];
					mLock->unlock();
					return node;
				}
				else if (mIndex[i] > vv)
				{
					// Not there. Need to insert it.
					node = insertBeforeLocked(i, vv, insert_leaf);
					mLock->unlock();
					return node;
				}
			 }
			 node = insertAfterLocked(i, vv, insert_leaf);
			 mLock->unlock();
			 return node;
		 }
		 else
		 {
			 // Do bineary search
			 u32 vv2 = mIndex[0];
			 if (vv2 == vv)
			 {
				 node = mSubnodes[0];
				 mLock->unlock();
				 return node;
			 }

			 if (vv2 > vv)
			 {
				 // The value is not in the range. 
				 node = insertBeforeLocked(0, vv, insert_leaf);
				 mLock->unlock();
				 return node;
			 }

			 int right = mIndex[size-1];
			 vv2 = mIndex[right];
			 if (vv2 == vv)
			 {
				 node = mSubnodes[right];
				 mLock->unlock();
				 return node;
			 }

			 if (vv2 < vv)
			 {
				 node = insertAfterLocked(right, vv, insert_leaf);
				 mLock->unlock();
				 return node;
			 }

			 int left = 0;
			 while (left < right)
			 {
				int nn = (right + left) >> 1;
				vv2 = mIndex[nn];
				if (vv2 < vv)
				{
					left = nn+1;
				}
				else if (vv2 > vv)
				{
					right = nn-1;
				}
				else
				{
				 	node = mSubnodes[nn];
					mLock->unlock();
					return node;
				}
			 }

			 if (left > right)
			 {
				 OmnAlarm << "Internal error: " << left << ":" << right << enderr;
				 mLock->unlock();
				 return 0;
			 }

			 vv2 = mIndex[right];
			 if (vv2 == vv)
			 {
				node = mSubnodes[right];
			 	mLock->unlock();
			 	return node;
			 }

			 if (vv < vv2)
			 {
				node = insertBforeLocked(left, vv, insert_leaf);
				mLock->unlock();
				return node;
			 }
			 
			 node = insertAfterLocked(left, vv, insert_leaf);
			 mLock->unlock();
			 return node;
		 }
		 break;

	case AosBitmapUtil::eFull:
		 aos_assert_r(mSubnodes, false);
		 if (mSubnodes[vv]) 
		 {
			 node = mSubnodes[vv];
		 }
		 else
		 {
		 	// The node is not there yet. Need to create it.
		 	if (insert_leaf)
		 	{
		 		node = AosU64BmNodeLeaf::getNode();
		 	}
		 	else
		 	{
		 		node = AosU64BmNodeInner::getNode();
		 	}
			mSubnodes[vv] = node;
		 }
		 break;

	default:
		 OmnAlarm << "Unrecognized mode: " << mMode << enderr;
		 break;
	}
	mLock->unlock();
	return node;
}


AosU64BmNode*
AosU64BmNodeInner::insertBeforeLocked(const u16 idx, const u8 value, const bool leaf)
{
	AosU64BmNode *node = (leaf)?AosU64BmNodeLeaf::getNode():AosU64BmNodeInner::getNode();
	if (mIndex.size() > mTooManyForPacked)
	{
		vector<AosU64BmNode*> vec(eNumSubnodes, 0);
		u32 size = mIndex.size();
		for (u32 i=0; i<size; i++)
		{
			vec[mIndex[i]] = mSubnodes[mIndex[i]];
		}
		mSubnodes = vec;
		mIndex.clear();
		mMode = AosBitmapUtil::eFull;
		mSubnodes[idx] = node;
		return true;
	}

	mIndex.insert(mIndex.begin() + idx, value);
	AosU64BmNode *node = 0;
	mSubnodes.insert(mSubnodes.begin() + idx, node);
	return node;
}


AosU64BmNode*
AosU64BmNodeInner::insertAfterLocked(const u16 idx, const u8 value, const bool leaf)
{
	node = (leaf)?AosU64BmNodeLeaf::getNode():AosU64BmNodeInner::getNode();
	if (mIndex.size() > mTooManyForPacked)
	{
		vector<AosU64Node*> vec(eNumSubnodes, 0);
		u32 size = mIndex.size();
		for (u32 i=0; i<size; i++)
		{
			vec[mIndex[i]] = mSubnodes[mIndex[i]];
		}
		mSubnodes = vec;
		mIndex.clear();
		mMode = AosBitmapUtil::eFull;
		mSubnodes[value] = node;
		return node;
	}

	if (idx + 1 == mIndex.size())
	{
		// It is append.
		mIndex.push_back(value);
		mSubnodes.push_back(node);
		return node;
	}

	mIndex.insert(mIndex.begin() + idx + 1, value);
	mSubnodes.insert(mSubnodes.begin() + idx + 1, node);
	return node;
}

