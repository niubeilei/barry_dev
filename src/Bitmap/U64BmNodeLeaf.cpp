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
#include "Bitmap/U64BmNodeLeaf.h"


bool
AosU64BmNodeLeaf::setDocid(const u16 docid)
{
	mLock->lock();
	u32 size;
	u16 vv = (docid >> eBitsToShift);
	switch (mMode)
	{
	case ePacked:
		 size = mIndex.size();
		 if (size == 0)
		 {
			 mIndex.push_back(vv);
			 mSubnodes.push_back(sgBit[docid & 0x3f]);
			 mLock->unlock();
			 return true;
		 }

		 if (size < eLinearThreshold)
		 {
			 // Will do linear search
		 	 for (u32 i=0; i<size; i++)
			 {
		 		if (mIndex[i] == vv)
		 		{
					mSubnodes[i] |= sgBit[docid & 0x3f];
					mLock->unlock();
					return true;
				}
				else if (mIndex[i] > vv)
				{
					// Not there. Need to insert it.
					insertBeforeLocked(i, docid);
					mLock->unlock();
					return true;
				}
			 }
			 insertAfterLocked(i, docid);
			 mLock->unlock();
			 return true;
		 }
		 else
		 {
			 // Do bineary search
			 u32 vv2 = mIndex[0];
			 if (vv2 == vv)
			 {
				 mSubnodes[0] |= sgBit[docid & 0x3f];
				 mLock->unlock();
				 return true;
			 }

			 if (vv2 > vv)
			 {
				 // The value is not in the range. 
				 node = insertBeforeLocked(0, docid);
				 mLock->unlock();
				 return true;
			 }

			 int right = mIndex[size-1];
			 vv2 = mIndex[right];
			 if (vv2 == vv)
			 {
				 mSubnodes[right] |= sgBit[docid & 0x3f];
				 mLock->unlock();
				 return true;
			 }

			 if (vv2 < vv)
			 {
				 insertAfterLocked(right, docid);
				 mLock->unlock();
				 return true;
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
				 	mSubnodes[nn] |= sgBit[docid & 0x3f];
					mLock->unlock();
					return true;
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
				mSubnodes[right] |= sgBit[docid & 0x3f];
			 	mLock->unlock();
			 	return true;
			 }

			 if (vv < vv2)
			 {
				insertBforeLocked(left, docid);
				mLock->unlock();
				return true;
			 }
			 
			 insertAfterLocked(left, docid);
			 mLock->unlock();
			 return true;
		 }
		 break;

	case eFull:
		 mSubnodes[vv] |= sgBit[docid & 0x3f];
		 mLock->unlock();
		 return true;

	default:
		 OmnAlarm << "Unrecognized mode: " << mMode << enderr;
		 break;
	}
	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}


bool
AosU64BmNodeLeaf::insertBeforeLocked(const u16 idx, const u32 docid)
{
	if (mIndex.size() > mTooManyForPacked)
	{
		vector<u64> vec(eNumSubnodes, 0);
		u32 size = mIndex.size();
		for (u32 i=0; i<size; i++)
		{
			vec[mIndex[i]] = mSubnodes[mIndex[i]];
		}
		mSubnodes = vec;
		mIndex.clear();
		mMode = eFull;
		mSubnodes[idx] = sgBit[docid & 0x3f];
		return true;
	}

	mIndex.insert(mIndex.begin() + idx, (docid >> eBitsToShift));
	mSubnodes.insert(mSubnodes.begin() + idx, sgBit[docid & 0x3f]);
	return true;
}


bool
AosU64BmNodeLeaf::insertAfterLocked(const u16 idx, const u32 docid)
{
	if (mIndex.size() > mTooManyForPacked)
	{
		vector<u64> vec(eNumSubnodes, 0);
		u32 size = mIndex.size();
		for (u32 i=0; i<size; i++)
		{
			vec[mIndex[i]] = mSubnodes[mIndex[i]];
		}
		mSubnodes = vec;
		mIndex.clear();
		mMode = eFull;
		mSubnodes[idx] = sgBit[docid & 0x3f];
		return true;
	}

	if (idx + 1 == mIndex.size())
	{
		// It is append.
		mIndex.push_back((docid >> eBitsToShift));
		mSubnodes.push_back(sgBit[docid & 0x3f]);
		return true;
	}

	mIndex.insert(mIndex.begin() + idx + 1, (docid >> eBitsToShift));
	mSubnodes.insert(mSubnodes.begin() + idx + 1, sgBit[docid & 0x3f]);
	return true;
}


