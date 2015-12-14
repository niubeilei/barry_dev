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
#include "Bitmap/U64BmNodeRoot.h"

AosU64BmNodeInner *
AosU64BmNodeRoot::setDocid(const u64 &docid)
{
	u32 vv = docid >> 32;
	AosU64BmNodeInner *node = 0;
	switch (mMode)
	{
	case AosBmUtil::ePacked:
		 mLock->lock();
		 if (mSubnodes.size() == 0)
		 {
			 node = AosU64BmNodeInner::getNode(vv);
			 mSubnodes.push_back(node);
			 mIndex.push_back(vv);
			 mLock->unlock();
			 return node;
		 }

		 if (mSubnodes.size() < eLinearThreshold)
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
					node = insertBeforeLocked(i, vv);
					mLock->unlock();
					return node;
				}
			 }
			 node = insertAfterLocked(i, vv);
			 mLock->unlock();
			 return node;
		 }
		 else
		 {
			 // Will do binary search
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
				 node = insertNodeBeforeLocked(0, vv);
				 mLock->unlock();
				 return node;
			 }

			 int right = mSubnodes.size()-1;
			 vv2 = mIndex[right];
			 if (vv2 == vv)
			 {
				 node = mSubnodes[right];
				 mLock->unlock();
				 return node;
			 }

			 if (vv2 < vv)
			 {
				 node = insertNodeAfterLocked(right, vv);
				 mLock->unlock();
				 return node;
			 }

			 int left = 0;
			 int idx;
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
				node = insertNodeBforeLocked(left, vv);
				mLock->unlock();
				return node;
			 }
			 
			 node = insertNodeAfterLocked(left, vv);
			 mLock->unlock();
			 return node;
		 }
		 break;

	case AosBmUtil::eFull:
		 aos_assert_r(mSubnodes, false);
		 if (mSubnodes[vv]) 
		 {
			 node = mSubnodes[vv];
			 mLock->unlock();
			 return node;
		 }
		 node = AosU64BmNodeInner::getNode();
		 mSubnodes[vv] = node;
		 mLock->unlock();
		 return node;

	default:
		 mLock->unlock();
		 OmnAlarm << "Unrecognized mode: " << mMode << enderr;
		 return 0;
	}

	mLock->unlock();
	OmnShouldNeverComeHere;
	return 0;
}


AosU64BmNodeInner *
AosU64BmNodeRoot::insertNodeBeforeLocked(const int idx, const u32 value)
{
	AosU64BmNodeInner *node = AosU64BmNodeInner::getNode();
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
		mMode = eFull;
		mSubnodes[idx] = node;
		return true;
	}

	mIndex.insert(mIndex.begin() + idx, value);
	AosU64BmNode *node = 0;
	mSubnodes.insert(mSubnodes.begin() + idx, node);
	return node;
}


AosU64BmNodeInner *
AosU64BmNodeRoot::insertNodeAfterLocked(const u32 idx, const u32 value)
{
	node = AosU64BmNodeInner::getNode();
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
		mMode = eFull;
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

