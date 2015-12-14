////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 	Created: 12/11/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILBigHit.h"

#include "IILMgr/IILMgr.h"


bool		
AosIILBigHit::nextDocidPrivFind(
		AosIILIdx &idx,
		u64 &docid,
		u64 &cur_docid,
		const bool reverse,  
		bool &include_self,
		const AosRundataPtr &rdata) 
{
	// It searches the first docid that meets the condition:
	// 		mValues[i] opr value
	//
	// If 'idx' is not a new search, it starts from 'idx' next position. 
	// Otherwise, it searches starting from the beginning. 
	//
	// If an entry is found, 'docid' is set to the corresponding docid
	// and 'idx' points to it.
	//
	// Returns:
	// If any error occurred, it returns false. Otherwise, it returns
	// true, 'docid' is set to the one found, and 'iilidx' is set
	// to the iil in which the doc was found, if it is in a subiil.
	//
	// Performance Analysis:
	// This should be the same as the previous one.
	//
	// If this instance is a root iil, 'rootiil' is null. Otherwise, 
	// it is a subiil and it should be called from its root iil. 
	// 'rootiil' should not be null.
	
	// Check whether it is a root iil and it is segmented
	docid = AOS_INVDID;
	if (isLeafIIL())
	{
		i64 index = idx.getIdx(mLevel);
		bool rslt = nextDocidSinglePriv(index, reverse, docid, cur_docid, include_self, rdata);
		aos_assert_r(rslt, false);
		
		rslt = idx.setIdx(mLevel, index);
		aos_assert_r(rslt, false);
		
		return rslt;
	}
	
	AosIILBigHitPtr subiil = getSubiilByCondPriv(idx, docid, reverse, rdata);
	if (!subiil)
	{
		// This means there are no entries that satisfies the condition.
		docid = AOS_INVDID;
		return true;
	}

	bool rslt;
	while (subiil)
	{
		aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
		rslt = subiil->nextDocidSafeFind(idx, docid, cur_docid, reverse, include_self, rdata);
		aos_assert_r(rslt, false);

		if (idx.isValid(mLevel)) return true;
		if (!hasMoreMembers(idx))
		{
			idx.setInvalid(mLevel);
			return true;
		}
		// Find the next subiil.
		subiil = getSubiilByCondPriv(idx, docid, reverse, rdata);
	}
	idx.setInvalid(mLevel);
	return true;
}


bool		
AosIILBigHit::findPosPriv(
		const u64 &docid, 
		AosIILIdx &idx,
		const bool reverse,
		bool &include_self,
		const AosRundataPtr &rdata) 
{
	if (isLeafIIL())
	{
		bool rslt = findPosSinglePriv(docid, idx, reverse, include_self, rdata);
		aos_assert_r(rslt, false);				
		return true;
	}
	
	AosIILBigHitPtr subiil = getFirstSubiilByCondPriv(idx, docid, reverse, rdata);
	if (!subiil)
	{
		idx.setInvalid(mLevel);
		return true;
	}
	
	bool rslt = subiil->findPosPriv(docid,idx,reverse,include_self,rdata);
	aos_assert_r(rslt, false);
	
	if(!idx.isValid(mLevel-1))
	{
		idx.setInvalid(mLevel);
		return true;
	}
	
	return true;
}


AosIILBigHitPtr
AosIILBigHit::getSubiilByCondPriv(
		AosIILIdx &idx,
		const u64 &docid,
		const bool reverse,        
		const AosRundataPtr &rdata)
{
	if (idx.isStart(mLevel))
	{
		return getFirstSubiilByCondPriv(idx, docid, reverse, rdata);
	}
	return nextSubiilByCondPriv(idx, docid, reverse, rdata);
}

AosIILBigHitPtr
AosIILBigHit::getFirstSubiilByCondPriv(
		AosIILIdx &idx, 
		const u64 &docid,
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the first index (or last index if 'reverse' is true):
	//			[mMinVals[index], mMinDocids[index]] opr [value, docid]
	// If not found, it returns 0.
	idx.setStart(mLevel);
	aos_assert_r(!isLeafIIL(), 0);
	aos_assert_r(mNumSubiils > 0, 0);
	
	i64 index = reverse ? mNumSubiils-1 : 0;
	if (reverse)
	{
		if(mMaxDocids[mNumSubiils-1]<docid) index = -5;
	}
	else
	{
		if(mMinDocids[0] > docid)  index = -5;
	}

	if (index < 0 || index >= mNumSubiils)
	{
		idx.setInvalid(mLevel);	 
		return 0;
	}
	
	bool rslt = idx.setIdx(mLevel, index);
	aos_assert_r(rslt, 0);

	return getSubiilByIndexPriv(index, rdata);
}

AosIILBigHitPtr
AosIILBigHit::nextSubiilByCondPriv(
		AosIILIdx &idx, 
		const u64 &docid,
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function searches the next subiil starting from 'idx'
	// that satisifies the condition [value, opr]. For instance, 
	// if 'opr' is eq, it searches the next subiil that may contain
	// the values that are the same as 'value'.
	idx.next(mLevel, reverse);
	while (hasMoreMembers(idx))
	{
		bool keep_search;
		i64 iilidx = idx.getIdx(mLevel);
		if (containDocidPriv(mMinDocids[iilidx], mMaxDocids[iilidx], 
					docid, reverse, keep_search, rdata))
		{
			AosIILBigHitPtr subiil = getSubiilByIndexPriv(iilidx, rdata);
			aos_assert_r(subiil, 0);
			return subiil;
		}
		if (!keep_search) return 0;
		idx.next(mLevel, reverse);
	}
	return 0;
}

AosIILBigHitPtr
AosIILBigHit::getSubiilByIndexPriv(
		const u64 &iilid,
		const i64 &iil_idx,
		const AosRundataPtr &rdata)
{
	lockUpdate();
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILBigHitPtr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mNumSubiils <= iil_idx)
	{
		OmnAlarm << "error" << enderr;
		unlockUpdate();
		return 0;
	}

	i64 idx = iil_idx;
	if (mIILIds[idx] != iilid)
	{
		int i = idx + 1;
		for (; i<mNumSubiils; i++)	
		{
			if (mIILIds[i] == iilid)
			{
				idx = i;
				break;
			}
		}
		if (i == mNumSubiils)
		{
			OmnAlarm << "error" << enderr;
			unlockUpdate();
			return 0;
		}
	}

	AosIILBigHitPtr subiil = mSubiils[idx];
	unlockUpdate();

	if (!subiil)
	{
		AosIILType type= eAosIILType_BigHit;
		AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
			iilid, mSiteid, mSnapShotId, type, rdata);
		aos_assert_r(iil, 0);
		aos_assert_r(iil->getIILType() == eAosIILType_BigHit, 0);

		lockUpdate();
		bool rslt = setSubiilIndexPriv(iil, idx, rdata);
		unlockUpdate();

		//OmnScreen << "load subiil, iilid:" << iilid << ", idx:"
		//	<< idx << ", real_idx:" << iil->getIILIdx() << endl;

		aos_assert_r(rslt, 0);

		subiil = (AosIILBigHit *)(iil.getPtr());
	}

	return subiil;
}


bool
AosIILBigHit::setSubiilIndexPriv(
		const AosIILObjPtr &iil,
		const i64 &iil_idx, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_BigHit, false);
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(iil_idx >= 0 && iil_idx < mNumSubiils, false);

	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILBigHitPtr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	i64 idx = iil_idx;
	u64 iilid = iil->getIILID();
	if (mIILIds[idx] != iilid)
	{
		int i = idx + 1;
		for (; i<mNumSubiils; i++)	
		{
			if (mIILIds[i] == iilid)
			{
				idx = i;
				break;
			}
		}
		if (i == mNumSubiils)
		{
			OmnAlarm << "error" << enderr;
			return false;
		}
	}

	if (mSubiils[idx] && mSubiils[idx] != iil)
	{
		OmnAlarm << "idx error" << enderr;
		return false;
	}

	mSubiils[idx] = (AosIILBigHit *)(iil.getPtr());
	mSubiils[idx]->mParentIIL = this;
	mSubiils[idx]->mIILIdx = idx;
	return true;
}


AosIILBigHitPtr
AosIILBigHit::getSubiilByIndexPriv(
		const i64 &idx, 
		const AosRundataPtr &rdata)
{
	// It retrieves the iilidx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil. 
	//
	// The function assumes 'mLock' should have been locked.
	// Chen Ding, 12/14/2010
	// aos_assert_r(mIILIdx == 0, 0);
	aos_assert_r(!isLeafIIL(), 0);
	
	i64 iilidx = idx;
	if (idx == -10) iilidx = 0;

	aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, 0);
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILBigHitPtr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[iilidx]) return mSubiils[iilidx];

	AosIILType type= eAosIILType_BigHit;
	AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
		mIILIds[iilidx], mSiteid, mSnapShotId, type, rdata);
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[iilidx] << ":" << type << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_BigHit)
	{
		OmnAlarm << "Not a hit IIL: " 
			<< iil->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}
	mSubiils[iilidx] = (AosIILBigHit*)iil.getPtr();
	mSubiils[iilidx]->mParentIIL = this;
	mSubiils[iilidx]->mIILIdx = iilidx;

	return mSubiils[iilidx];
}


AosIILBigHitPtr
AosIILBigHit::getSubiilPriv( 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// This function finds the subiil in which the entry [value, docid]
	// may be inserted. This is normally used by adding an entry. 
	// This function shall be called by non-leaf IILs.
	//
	// 1. If 'value' is not in boundary, it returns the iil in which the
	//    entry [value, docid] may reside. 
	// 2. If 'value' is at boundary:
	// 	  a. If docid == 0, return.
	// 	  b. If docid != 0, if 
	// 2. If 'value_unique' is true, the function checks whether the value
	//    already exists. If yes, it returns 0. 
	// 2. If 'docid_unique' is true, the function checks whether [value, docid] 
	//    exists. If yes, it returns 0. 
	// 3. Otherwise, it returns the subiil in which the entry [value, docid]
	//    may resides. If it is to add, this is the subiil to add.
	aos_assert_r(!isLeafIIL(), 0);
	
	i64 idx = getSubiilIndex3Priv(docid, rdata);
	aos_assert_r(idx >= 0 && idx < mNumSubiils, 0);
	
	AosIILBigHitPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, 0);
	return subiil;
}

AosIILBigHitPtr
AosIILBigHit::getSubiil3Priv(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	i64 index = getSubiilIndex3Priv(docid, rdata);
	aos_assert_r((index >= 0) && (index <= mNumSubiils-1), 0);
	return getSubiilByIndexPriv(index, rdata);
}


i64
AosIILBigHit::getSubiilIndex3Priv(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	// Or it is to find the first index that:
	// 		[value, docid] <= [mMaxVals[index], mMaxDocids[index]]
	//
	aos_assert_r(mSubiils > 0, 0);
    i64 left = 0;
    i64 right = mNumSubiils - 1;
    i64 cur = 0;
    if (right == left) return right;
	while(1)
    {
		if (right == left)
		{
			return left;
		}

        if (right == left +1)
        {
			// If the left satisfies the condition, return left.
			// Otherwise, return right.
			if(mMaxDocids[left]>=docid)
            {
                return left;
            }
            else
            {
                return right;
            }
        }

        cur = (right - left)/2 + left;

		if(mMaxDocids[cur]>=docid)
        {
			// Found a match: 
			// 		[value, docid] <= [mMaxVals[cur], mMaxDocids[cur]]
			// Move left. 
			right = cur;
		}
		else
		{
			// Not a match:
			// 		[value, docid] > [mMaxVals[cur], mMaxDocids[cur]]
			// Move right. 
			left = cur;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}

i64
AosIILBigHit::countNumRec(
		const AosIILIdx start_iilidx,
		const AosIILIdx end_iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return countNumSingle(start_iilidx, end_iilidx);
	}

	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);

	// Chen Ding, 07/12/2012
	if (end < 0) 
	{
		aos_assert_r(start == -5 && end == -5, 0);
		return 0;
	}

	aos_assert_r(end >= start && start >= 0 && end <= (mNumSubiils-1) , 0);

	AosIILBigHitPtr subiil;
	if (start == end)
	{
		subiil = getSubiilByIndexPriv(start, rdata);
		aos_assert_r(subiil, 0);

		return subiil->countNumRec(start_iilidx, end_iilidx, rdata);
	}
	
	// start != end
	subiil = getSubiilByIndexPriv(start, rdata);
	aos_assert_r(subiil, 0);

	i64 total = subiil->countNumToEndRec(start_iilidx, rdata);

	subiil = getSubiilByIndexPriv(end, rdata);
	aos_assert_r(subiil, 0);

	total += subiil->countNumFromStartRec(end_iilidx, rdata);

	for(i64 i = start+1;i < end;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}


i64
AosIILBigHit::countNumSingle(
		const AosIILIdx start_iilidx,
		const AosIILIdx end_iilidx)
{
	i64 start = start_iilidx.getIdx(0);
	i64 end = end_iilidx.getIdx(0);
	if (end < start) return 0;
	return end - start + 1;
}


i64
AosIILBigHit::countNumToEndSingle(const AosIILIdx start_iilidx)
{
	return mNumDocs - start_iilidx.getIdx(0);
}


i64
AosIILBigHit::countNumFromStartSingle(const AosIILIdx end_iilidx)
{
	return end_iilidx.getIdx(0) + 1;
}


i64
AosIILBigHit::countNumToEndRec(
		const AosIILIdx start_iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return countNumToEndSingle(start_iilidx);
	}

	i64 start = start_iilidx.getIdx(mLevel);
	aos_assert_r(start >= 0 && start <= (mNumSubiils-1) , 0);

	AosIILBigHitPtr subiil = getSubiilByIndexPriv(start, rdata);
	aos_assert_r(subiil, 0);

	i64 total = subiil->countNumToEndRec(start_iilidx, rdata);

	for(i64 i = start+1;i < mNumSubiils; i++)
	{
		total += mNumEntries[i];
	}
	return total;
}


i64
AosIILBigHit::countNumFromStartRec(
		const AosIILIdx end_iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return countNumFromStartSingle(end_iilidx);
	}

	i64 end = end_iilidx.getIdx(mLevel);
	aos_assert_r(end >= 0 && end <= (mNumSubiils-1) , 0);

	AosIILBigHitPtr subiil = getSubiilByIndexPriv(end, rdata);
	aos_assert_r(subiil, 0);

	i64 total = subiil->countNumFromStartRec(end_iilidx, rdata);

	for(i64 i = 0;i < end;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}				   				 


bool 
AosIILBigHit::prevQueryPosSingle(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	i64 idx = iilidx.getIdx(0);
	if (idx > 0)
	{
		idx --;
		iilidx.setIdx(0,idx);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}


bool 
AosIILBigHit::prevQueryPosL1(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	i64 idx0 = iilidx.getIdx(0);
	if (idx0 > 0)
	{
		idx0--;
		iilidx.setIdx(0,idx0);
		return true;
	}
	
	i64 idx1 = iilidx.getIdx(1);
	if (idx1 == 0)
	{
		return false;
	}
	idx1--;
	iilidx.setIdx(1,idx1);
	idx0 = mNumEntries[idx1] - 1;
	iilidx.setIdx(0,idx0);
	return true;
}


bool 
AosIILBigHit::prevQueryPosRec(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return prevQueryPosSingle(iilidx, rdata);
	}
	
	if (mLevel == 1)
	{
		return prevQueryPosL1(iilidx, rdata);
	}

	i64 idx = iilidx.getIdx(mLevel);
	if (idx < 0 || idx >= mNumSubiils) return false;

	AosIILBigHitPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);
	
	bool rslt = subiil->prevQueryPosRec(iilidx, rdata);
	if (rslt) return true;
	
	if (idx == 0)
	{
		return false;
	}

	idx--;
	subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);

	iilidx.setIdx(mLevel, idx);
	rslt = subiil->lastQueryPosRec(iilidx, rdata);
	aos_assert_r(rslt, false);

	return rslt;	
}


bool
AosIILBigHit::lastQueryPosSingle(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	iilidx.setIdx(0, mNumDocs-1);
	return true;
}


bool
AosIILBigHit::lastQueryPosL1(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumSubiils > 0, false);
	aos_assert_r(mNumEntries[mNumSubiils-1] > 0, false);
	iilidx.setIdx(0,mNumEntries[mNumSubiils-1]-1);
	iilidx.setIdx(1,mNumSubiils-1);
	return true;
}


bool
AosIILBigHit::lastQueryPosRec(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return lastQueryPosSingle(iilidx, rdata);
	}
	
	if (mLevel == 1)
	{
		return lastQueryPosL1(iilidx, rdata);
	}
	
	i64 idx = mNumSubiils -1;
	iilidx.setIdx(mLevel, idx);

	AosIILBigHitPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);
	
	return subiil->lastQueryPosRec(iilidx, rdata);
}


bool
AosIILBigHit::nextQueryPosSingle(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	i64 idx = iilidx.getIdx(0);
	if (idx >= mNumDocs-1)
	{
		return false;
	}

	idx++;
	iilidx.setIdx(0,idx);
	return true;
}


bool
AosIILBigHit::nextQueryPosL1(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	// This function 'increments' 'iilidx', if possible.
	i64 idx0 = iilidx.getIdx(0);
	i64 idx1 = iilidx.getIdx(1);
	if (idx1 > mNumSubiils-1)
	{
		return false;
	}
	
	if (idx0 < mNumEntries[idx1]-1)
	{
		idx0 ++;
		iilidx.setIdx(0,idx0);
		return true;
	}
	
	if (idx1 >= mNumSubiils-1)
	{
		return false;
	}
	
	idx1 ++;
	iilidx.setIdx(1,idx1);
	iilidx.setIdx(0,0);
	return true;
}


bool
AosIILBigHit::nextQueryPosRec(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	// This function 'increments' 'iilidx', if possible.
	if (isLeafIIL())
	{
		return nextQueryPosSingle(iilidx, rdata);
	}
	
	if (mLevel == 1)
	{
		return nextQueryPosL1(iilidx, rdata);
	}

	// The IIL is Level > 1. This means it needs to get the current
	// IIL, and recursively ask the subiil to increase. If it cannot, 
	// need to find the next subiil. If it is already the last subiil, 
	// it means there are no more entries.
	i64 idx = iilidx.getIdx(mLevel);
	aos_assert_r(idx >= 0, false);

	if (idx >= mNumSubiils) return false;

	AosIILBigHitPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->nextQueryPosRec(iilidx, rdata);
	if (rslt) return true;
	
	// This means that the current subiil has no more entries. Check 
	// whether it is the last one.
	if (idx >= mNumSubiils-1)
	{
		// It is the last one.
		return false;
	}

	idx++;
	iilidx.setIdx(mLevel,idx);
	// set first pos(no need to use new function
	for(i64 i = 0;i < mLevel;i++)
	{
		iilidx.setIdx(i,0);
	}
	return true;	
}


bool
AosIILBigHit::moveToRec(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						u64 &page_start,
						const bool &reverse,
						bool &has_data,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return moveToSingle(start_iilidx,
						end_iilidx,
						page_start,
						reverse,
						has_data,
						copytype, 
						rdata);
	}

	// parent iil
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);
	bool handle_first = false;
	bool handle_last = false;
	
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}
	else
	{
		start ++;
	}
	
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumSubiils - 1;
	}
	else
	{
		end --;
	}
	
	if (copytype == AosIILUtil::eCopyNormal || 
	   copytype == AosIILUtil::eCopyToHead ||
	   copytype == AosIILUtil::eCopyToTail)
	{
		handle_first = true;
	}

	if (copytype == AosIILUtil::eCopyNormal || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		handle_last = true;
	}
		
	bool rslt = false;

	// hand first 
	if(handle_first)
	{
		if(!reverse)
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(start-1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyToTail, 
								rdata);
			aos_assert_r(rslt,false);
			start_iilidx.setIdx(mLevel,start-1);
		}
		else // reverse
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(end+1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyToHead, 
								rdata);
			aos_assert_r(rslt,false);
			end_iilidx.setIdx(mLevel,end+1);
	
		}
	}
	
	if(page_start == 0 && has_data)
	{
		return true;
	}

	if(start <= end)
	{
		has_data = true;
		if(!reverse)
		{
			for(int i = start;i <= end;i++)
			{
				if((i64)page_start >= mNumEntries[i])
				{
					page_start -= mNumEntries[i];
				}
				else 
				{
					rslt = getSubiilByIndexPriv(i, rdata)->moveToRec(
										start_iilidx,
										end_iilidx,
										page_start,
										reverse,
										has_data,
										AosIILUtil::eCopyAll, 
										rdata);
					aos_assert_r(rslt,false);
					start_iilidx.setIdx(mLevel,i);
					return true;
				}
			}
		}
		else// reverse
		{
			for(int i = end;i >= start;i--)
			{
				if((i64)page_start >= mNumEntries[i])
				{
					page_start -= mNumEntries[i];
				}
				else
				{
					rslt = getSubiilByIndexPriv(i, rdata)->moveToRec(
										start_iilidx,
										end_iilidx,
										page_start,
										reverse,
										has_data,
										AosIILUtil::eCopyAll, 
										rdata);
					aos_assert_r(rslt,false);
					start_iilidx.setIdx(mLevel,i);
					return true;
				}
			}
		}
	}
	
	if(page_start == 0 && has_data)
	{
		return true;
	}

	// hand last
	if(handle_last)
	{
		if(!reverse)
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(end+1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyFromHead, 
								rdata);
			aos_assert_r(rslt,false);
			start_iilidx.setIdx(mLevel,end+1);
		}
		else // reverse
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(start-1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyToHead, 
								rdata);
			aos_assert_r(rslt,false);
			end_iilidx.setIdx(mLevel,start-1);
	
		}
	}
	return true;
}

bool
AosIILBigHit::moveToSingle(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						u64 &page_start,
						const bool &reverse,
						bool &has_data,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata)
{
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}

	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumDocs - 1;
	}
	
	int num_in_range = end - start +1;
	if(end - start +1 <= (i64)page_start)
	{
		page_start -= num_in_range;
		has_data = false;
		return true;
	} 
	
	if(!reverse)
	{
		start += page_start;
		page_start = 0;
		start_iilidx.setIdx(0,start);
	}
	else
	{
		end -= page_start;
		page_start = 0;
		end_iilidx.setIdx(0,end);
	}
	return true;
}

bool	
AosIILBigHit::getDocidByIdxRecPriv(
				AosIILIdx &idx,
				u64 &docid,
				const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return getDocidByIdxSinglePriv(idx,docid,rdata);
	}
	
	i64 index = idx.getIdx(mLevel);
	aos_assert_r(index >= 0 && index < mNumSubiils, false);
	AosIILBigHitPtr subiil = getSubiilByIndexPriv(index, rdata);
	aos_assert_r(subiil,false);	
	return subiil->getDocidByIdxRecPriv(idx, docid, rdata);
}

bool	
AosIILBigHit::getDocidByIdxSinglePriv(
				AosIILIdx &idx,
				u64 &docid,
				const AosRundataPtr &rdata)
{
	i64 cur_idx = idx.getIdx(0);
	aos_assert_r(cur_idx >= 0 && cur_idx < mNumDocs, false);
	bool rslt = false;
/*
	rslt = mDocBitmap->getDocByIdx(cur_idx, docid);
	aos_assert_r(rslt, false);
*/
//////////////////////
mDocBitmap->reset();
for(int i=0; i<=cur_idx; i++)
{
	rslt = mDocBitmap->nextDocid(docid);
	aos_assert_r(rslt, false);
}
//////////////////////
		
	return true;
}
/*
bool	
AosIILBigHit::getMaxMinIdx(
				AosIILIdx &idx,
				bool reverse,
				const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		if(reverse) return 0;
		return mNumDoc-1;
	}
	
	i64 index;
	if(reverse)
	{
		index = 0;
	}
	else
	{
		index = mNumSubiils-1;
	}

	idx.setIdx(mLevel, mNumSubiils-1);
	AosIILBigHitPtr subiil = getSubiilByIndexPriv(index, rdata);
	while(subiil)
	{
		subiil->getMaxMinIdx(idx, reverse, rdata);
	}
}
*/
