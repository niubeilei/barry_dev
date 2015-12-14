////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 2012/04/16 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILBigI64.h"

#include "IILMgr/IILMgr.h"


bool
AosIILBigI64::clearIILPriv(const AosRundataPtr &rdata)
{
	bool rslt = resetSubIILInfo(rdata);
	aos_assert_r(rslt, false);

	mNumDocs = 0;
	mFlag = AOS_ACTIVE_FLAG;
	mHitcount = 0;
	mRootIIL = 0;
	mLevel = 0;

	mMemCap = 0;
	if (mDocids)
	{
		OmnDelete [] mDocids;
		mDocids = 0;
	}
	
	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}

	return true;
}


bool
AosIILBigI64::deleteIILPriv(
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (!isSingleIIL())
	{
		vector<u64> iilids;
		AosIILObjPtr iil;
		AosIILType iiltype;
		AosIILBigI64Ptr subiil;
		for (i64 i = 0; i < mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (true_delete)
			{
				iilids.push_back(mIILIds[i]);

				if (!subiil && mLevel >= 2)
				{
					subiil = getSubiilByIndexPriv(i, rdata);
					aos_assert_r(subiil, false);
				}

				if (!subiil)
				{
					iil = AosIILMgr::getSelf()->getIILFromHashPublic(
						mIILIds[i], mSiteid, iiltype, rdata);
					if (iil) subiil = (AosIILBigI64 *)(iil.getPtr());
				}

				if (subiil)
				{
					rslt = subiil->deleteIILRecSafe(iilids, rdata);
					aos_assert_r(rslt, false);
				}
			}

			if (!subiil) continue;
			subiil->setDirty(false);
			rslt = AosIILMgr::getSelf()->returnIILPublic(subiil, rdata);
			aos_assert_r(rslt, false);
			
			mSubiils[i] = 0;
		}

		if (true_delete)
		{
			for (u32 i=0; i<iilids.size(); i++)
			{
				rslt = deleteFromLocalFileByID(iilids[i], mSnapShotId, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}


	rslt = clearIILPriv(rdata);
	aos_assert_r(rslt, false);
	
	mIsDirty = true;
	rslt = saveToFilePriv(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigI64::deleteIILRecPriv(
		vector<u64> &iilids,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL()) return true;

	bool rslt = true;
	AosIILObjPtr iil;
	AosIILType iiltype;
	AosIILBigI64Ptr subiil;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		iilids.push_back(mIILIds[i]);

		subiil = mSubiils[i];
		if (!subiil && mLevel >= 2)
		{
			subiil = getSubiilByIndexPriv(i, rdata);
			aos_assert_r(subiil, false);
		}
				
		if (!subiil)
		{
			iil = AosIILMgr::getSelf()->getIILFromHashPublic(
				mIILIds[i], mSiteid, iiltype, rdata);
			if (iil) subiil = (AosIILBigI64 *)(iil.getPtr());
		}

		if (!subiil) continue;
			
		rslt = subiil->deleteIILRecSafe(iilids, rdata);
		aos_assert_r(rslt, false);
			
		subiil->setDirty(false);
		rslt = AosIILMgr::getSelf()->returnIILPublic(subiil, rdata);
		aos_assert_r(rslt, false);
			
		mSubiils[i] = 0;
	}

	return true;
}


bool
AosIILBigI64::splitListPriv(
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	//case 1: level 0 add doc over 5000, split
	//case 2: level 1 has multi level 0, level 0 over 5000
	//case 3: level 1 has 5000 level 0, level 0 over 5000
	//case 4: level 2 has multi level 1, level 1 has multi level 0, level 0 needs split
	//case 5: level 2 has multi level 1, level 1 has 5000 level 0, level 0 needs split
	
	// level 0:
	// 		no parent:
	//				1. create 2 new iils with iilids
	//				2. split conents to 2 new iils,
	//				3. change self to level 1, 
	//				4. index subiil info.
	// 		has parent:step 
	//				1: split 
	//				2: call parent function to notify parent.
	// level 1-n(notified by level 0):
	// 1. add sub, update sub info.
	// 2. if, too many subs:
	//    2.1 no parent:
	//			create 2 new iils in same level with iilids
	//			split content(subiil info) to 2 new iils(update all subiils' root)
	//			change self to level++
	//			index 2 new iils as its subs.
	//	  2.2 has parent:
	//			split
	//			update subiils to know new root
	//			notify parent

	aos_assert_r(splitCheck(), false);
	aos_assert_r(isLeafIIL(), false);

	AosIILBigI64Ptr subiil = splitLeafContent(rdata);
	aos_assert_r(subiil, false);

	vector<AosIILBigI64Ptr> subiil_list;
	subiil_list.push_back(subiil);
	if (isSingleIIL())
	{
		return splitListSinglePriv(subiil_list, rdata);
	}
	
	aos_assert_r(mParentIIL, false);
	return mParentIIL->subiilSplited(mIILIdx, subiil_list, executor, rdata);
}


bool 
AosIILBigI64::splitCheck()
{
	if (mNumSubiils <= mMaxSubIILs)
	{
		return true;
	}
	if (mParentIIL)
	{
		return mParentIIL->splitCheck();
	}
	if (mLevel == AosIILIdx::eMaxLevelIndex)
	{
		return false;
	}
	return true;
}


AosIILBigI64Ptr
AosIILBigI64::splitLeafContent(const AosRundataPtr &rdata)
{
	AosIILBigI64Ptr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, 0);

	// Initialize the new subiil
	i64 startidx = mNumDocs / 2;
	const i64 len = mNumDocs - startidx;
	bool rslt = subiil->initSubiilLeaf(&mDocids[startidx], &mValues[startidx], len);
	aos_assert_r(rslt, 0);

	// 5. Shrink this IIL
	memset(&mValues[startidx], 0, sizeof(i64) * len);
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	aos_assert_r(mNumDocs < mMaxBigIILSize, 0);
	
	return subiil;	
}


bool 
AosIILBigI64::initSubiilLeaf(
		u64 *docids,
		i64 *values, 
		const i64 &num)
{
	aos_assert_r(mIILType == eAosIILType_BigI64, false);

    mWordId = AOS_INVWID;
	// ModifyNumDocs
	aos_assert_r(mNumDocs < mMaxBigIILSize, false);
    mNumDocs = num; 

	if (mMinVals)
	{
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if (mMinDocids)
	{
		OmnDelete [] mMinDocids;
		mMinDocids = 0;
	}

	if (mMaxVals)
	{
		OmnDelete [] mMaxVals;
		mMaxVals = 0;
	}

	if (mMaxDocids)
	{
		OmnDelete [] mMaxDocids;
		mMaxDocids = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}
	if (mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}
	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

    mFlag = 0;
    mHitcount = 0;
	//mTotalDocs = 0;
    //mIILDiskSize = 0;
    //mOffset = 0;
    //mSeqno = 0;
	mCompressedSize = 0;

    mNumSubiils = 0;
    mIILIdx = 0;

	bool rslt = prepareMemoryForReloading();
	aos_assert_r(rslt, false);

	memcpy(mValues, values, sizeof(i64) * mNumDocs);
	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool 
AosIILBigI64::initSubiilParent(
		u64 *minDocids,
		i64 *minValues, 
		u64 *maxDocids,
		i64 *maxValues, 
		u64 *iilids,
		i64 *numEntries,
		AosIILBigI64Ptr *subiils,
		const i64 &numSubiils)
{
    mWordId = AOS_INVWID;
    mNumSubiils = numSubiils; 

	// set mMinVals
	if (mMinVals)
	{
		memset(mMinVals, 0, sizeof(i64) * mMaxSubIILs);
	}
	else
	{
		mMinVals = OmnNew i64[mMaxSubIILs + eExtraSubIILsSpace];
		memset(mMinVals, 0, sizeof(i64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}

	// set mMaxVals
	if (mMaxVals)
	{
		memset(mMaxVals, 0, sizeof(i64) * mMaxSubIILs);
	}
	else
	{
		mMaxVals = OmnNew i64[mMaxSubIILs + eExtraSubIILsSpace];
		memset(mMinVals, 0, sizeof(i64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}

	aos_assert_r(mMinVals, false);
	aos_assert_r(mMaxVals, false);
	memset(mMinVals, 0, sizeof(i64) * mMaxSubIILs);
	memcpy(mMinVals, minValues, sizeof(i64) * mNumSubiils);
	memset(mMaxVals, 0, sizeof(i64) * mMaxSubIILs);
	memcpy(mMaxVals, maxValues, sizeof(i64) * mNumSubiils);

	// set mMinDocids
	if (!mMinDocids)
	{
		mMinDocids = OmnNew u64[mMaxSubIILs + eExtraSubIILsSpace];
	}
	// set mMaxDocids
	if (!mMaxDocids)
	{
		mMaxDocids = OmnNew u64[mMaxSubIILs + eExtraSubIILsSpace];
	}

	aos_assert_r(mMinDocids, false);
	aos_assert_r(mMaxDocids, false);
	memset(mMinDocids, 0, sizeof(u64) * (mMaxSubIILs + eExtraSubIILsSpace));
	memcpy(mMinDocids, minDocids, sizeof(u64) * mNumSubiils);
	memset(mMaxDocids, 0, sizeof(u64) * (mMaxSubIILs + eExtraSubIILsSpace));
	memcpy(mMaxDocids, maxDocids, sizeof(u64) * mNumSubiils);

	// set mIILIds
	if (!mIILIds)
	{
		mIILIds = OmnNew u64[mMaxSubIILs + eExtraSubIILsSpace];
	}
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * (mMaxSubIILs + eExtraSubIILsSpace));
	memcpy(mIILIds, iilids, sizeof(u64) * mNumSubiils);

	// set mNumEntries
	if (!mNumEntries)
	{
		mNumEntries = OmnNew i64[mMaxSubIILs + eExtraSubIILsSpace];
	}
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(i64) * (mMaxSubIILs + eExtraSubIILsSpace));
	memcpy(mNumEntries, numEntries, sizeof(i64) * mNumSubiils);
	
	mNumDocs = 0;
	for(i64 i=0; i<mNumSubiils; i++)
	{
		mNumDocs += mNumEntries[i];
	}
	
	// set mSubiils
	if (!mSubiils)
	{
		bool rslt = createSubiilIndex();
		aos_assert_r(rslt, false);
		//mSubiils = OmnNew AosIILBigI64Ptr[mMaxSubIILs + eExtraSubIILsSpace];
	}
	aos_assert_r(mSubiils, false);

	//ken 2012/11/12
	// this place must use for loop
	//memset(mSubiils, 0, sizeof(AosIILBigI64Ptr) * (mMaxSubIILs + eExtraSubIILsSpace));
	//memcpy(mSubiils, subiils, sizeof(AosIILBigI64Ptr) * mNumSubiils);
	for(i64 i=0; i<mMaxSubIILs + (i64)eExtraSubIILsSpace; i++)
	{
		mSubiils[i] = 0;
	}
	for(i64 i=0; i<mNumSubiils; i++)
	{
		mSubiils[i] = subiils[i];
	}
	
	// update info in subiils
	AosIILBigI64Ptr thisPtr(this, false);
	for(i64 i=0; i<mNumSubiils; i++)
	{
		if (mSubiils[i])
		{
			mSubiils[i]->setIILIdx(i);
			mSubiils[i]->setParentIIL(thisPtr);
		}
	}

    mFlag = 0;
    mHitcount = 0;
    //mIILDiskSize = 0;
    //mOffset = 0;
    //mSeqno = 0;
	mCompressedSize = 0;

    mIILIdx = 0;
	mIsDirty = true;
	return true;
}


bool
AosIILBigI64::splitListSinglePriv(
		const vector<AosIILBigI64Ptr> &subiil_list,
		const AosRundataPtr &rdata)
{
	// the 2nd subiil is created, now we need to :
	// 1. create a new subiil
	// 2. move content to new subiil
	// 3. make itself a root iil
	// 4. index 2 subiils
	
	// 1. create a new subiil
	//aos_assert_r(mNumDocs > 0, false);

	AosIILBigI64Ptr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, false);

	// 2. move content to new subiil
	// Initialize the new subiil
	bool rslt = subiil->initSubiilLeaf(mDocids, mValues, mNumDocs);
	aos_assert_r(rslt, false);

	memset(mValues, 0, sizeof(i64) * mNumDocs);
	memset(mDocids, 0, sizeof(u64) * mNumDocs);

	// 3. make itself a root iil
	if (mValues)
	{
		OmnDelete [] mValues;
		mValues = 0;
	}
	if (mDocids)
	{
		OmnDelete [] mDocids;
		mDocids = 0;
	}
	
	mLevel = 1;
	
	AosIILBigI64Ptr thisPtr(this, false);
	subiil->setParentIIL(thisPtr);	
	subiil->setIILIdx(0);

	// 4. index 2 subiils
	rslt = createSubiilIndex();
	aos_assert_r(rslt, false);
	
	mMinVals[0] = subiil->getMinValue();
	mMaxVals[0] = subiil->getMaxValue();

	mSubiils[0] = subiil;
	mMinDocids[0] = subiil->getMinDocid();
	mMaxDocids[0] = subiil->getMaxDocid();
	mIILIds[0] = subiil->getIILID();
	mNumEntries[0] = subiil->getNumDocs();
	mNumDocs = subiil->getNumDocs();
	mNumSubiils = subiil_list.size() + 1;
	
	for(u32 i=0; i<subiil_list.size(); i++)
	{
		aos_assert_r(subiil_list[i], false);

		mSubiils[i+1] = subiil_list[i];
		mSubiils[i+1]->setParentIIL(thisPtr);
		mSubiils[i+1]->setIILIdx(i+1);
		
		mMinDocids[i+1] = mSubiils[i+1]->getMinDocid();
		mMaxDocids[i+1] = mSubiils[i+1]->getMaxDocid();
		mIILIds[i+1] = mSubiils[i+1]->getIILID();
		mNumEntries[i+1] = mSubiils[i+1]->getNumDocs();
		mNumDocs += mSubiils[i+1]->getNumDocs();

		mMinVals[i+1] = mSubiils[i+1]->getMinValue();
		mMaxVals[i+1] = mSubiils[i+1]->getMaxValue();
	}

	return true;	
}


bool
AosIILBigI64::subiilSplited(
		const i64 &iilIdx,
		const vector<AosIILBigI64Ptr> &subiil_list,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	//1. update subiil info
	bool rslt = updateIndexData(iilIdx, true, true, rdata);
	aos_assert_r(rslt, false);

	rslt = addSubiils(iilIdx + 1, subiil_list, rdata);
	aos_assert_r(rslt, false);

	return subiilSplited(executor, rdata);
}				  


bool
AosIILBigI64::subiilSplited(
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	//1. check whether it need to split
	//2. split the content
	//3. (need split) check whether it is root

	//1. check whether it need to split
	if (mNumSubiils <= mMaxSubIILs)
	{
		// do not need to split
		return true;
	}
	
	//2. split the content
	AosIILBigI64Ptr newsubiil = splitParentContent(rdata);
	aos_assert_r(newsubiil, false);

	vector<AosIILBigI64Ptr> new_subiil_list;
	new_subiil_list.push_back(newsubiil);

	//3. (need split) check whether it is root
	if (mParentIIL)
	{
		return mParentIIL->subiilSplited(mIILIdx, new_subiil_list, executor, rdata);
	}
	
	// it is root iil, and it need to split
	return splitListRootPriv(new_subiil_list, executor, rdata);
}				  


AosIILBigI64Ptr
AosIILBigI64::splitContentUtil(const AosRundataPtr &rdata)
{
	// 1. Create the subiil
	AosIILObjPtr subiil = AosIILMgrSelf->createSubIILSafe(
		mIILID, mSiteid, mSnapShotId, eAosIILType_BigI64, mIsPersis, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->getIILType() == eAosIILType_BigI64, 0);
	AosIILBigI64Ptr newsubiil = (AosIILBigI64*)subiil.getPtr();
	
	newsubiil->setSnapShotId(mSnapShotId);

	// 2. Set subiil attributes
//	subiil->setRootIIL(mRootIIL);
//	newsubiil->setParentIIL(mParentIIL);
//	if (mIILIdx < 0) mIILIdx = 0;
//	newsubiil->setIILIdx(mIILIdx+1);
	newsubiil->setIILLevel(mLevel);
	newsubiil->setSiteid(mSiteid);
	
	mIsDirty = true;
	newsubiil->setDirty(true);
	return newsubiil;
}


AosIILBigI64Ptr
AosIILBigI64::splitParentContent(const AosRundataPtr &rdata)
{
	AosIILBigI64Ptr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, 0);

	// 4. Initialize the new subiil
	i64 startidx = mNumSubiils / 2;
	const i64 len = mNumSubiils - startidx;
	
	bool rslt = subiil->initSubiilParent(
		&mMinDocids[startidx], &mMinVals[startidx],	&mMaxDocids[startidx], &mMaxVals[startidx],
		&mIILIds[startidx], &mNumEntries[startidx], &mSubiils[startidx], len);
	aos_assert_r(rslt, 0);

	// 5. Shrink this IIL
	memset(&mMinVals[startidx], 0, sizeof(i64) * len);
	memset(&mMinDocids[startidx], 0, sizeof(u64) * len);
	memset(&mMaxVals[startidx], 0, sizeof(i64) * len);
	memset(&mMaxDocids[startidx], 0, sizeof(u64) * len);
	memset(&mIILIds[startidx], 0, sizeof(u64) * len);
	memset(&mNumEntries[startidx], 0, sizeof(i64) * len);

	//ken 2012/11/12
	// this place must use for loop
	//memset(&mSubiils[startidx], 0, sizeof(AosIILBigI64Ptr) * len);
	for(i64 i=0; i<len; i++)
	{
		mSubiils[startidx + i] = 0;
	}

	// ModifyNumDocs
	mNumSubiils = startidx;
	mNumDocs = 0;
	for(i64 i=0; i<mNumSubiils; i++)
	{
		mNumDocs += mNumEntries[i];
	}
	return subiil;	
}


bool
AosIILBigI64::splitListRootPriv(
		const vector<AosIILBigI64Ptr> &subiil_list,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	// the 2nd subiil is created, now we need to :
	// 1. create a new subiil
	// 2. move content to new subiil
	// 3. make itself a root iil
	// 4. index 2 subiils
	
	// 1. create a new subiil
	AosIILBigI64Ptr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, false);

	// 2. move content to new subiil
	// Initialize the new subiil
	bool rslt = subiil->initSubiilParent(
		mMinDocids, mMinVals, mMaxDocids, mMaxVals, mIILIds,
		mNumEntries, mSubiils, mNumSubiils);
	aos_assert_r(rslt, false);

	memset(mMinVals, 0, sizeof(i64) * mNumSubiils);
	memset(mMinDocids, 0, sizeof(u64) * mNumSubiils);
	memset(mMaxVals, 0, sizeof(i64) * mNumSubiils);
	memset(mMaxDocids, 0, sizeof(u64) * mNumSubiils);
	memset(mIILIds, 0, sizeof(u64) * mNumSubiils);
	memset(mNumEntries, 0, sizeof(i64) * mNumSubiils);

	//ken 2012/11/12
	// this place must use for loop
	//memset(mSubiils, 0, sizeof(AosIILBigI64Ptr) * mNumSubiils);
	for(int i=0; i<mNumSubiils; i++)
	{
		mSubiils[i] = 0;
	}

	// 3. make itself a root iil
	if (mValues)
	{
		OmnDelete [] mValues;
		mValues = 0;
	}
	if (mDocids)
	{
		OmnDelete [] mDocids;
		mDocids = 0;
	}
	
	mLevel++;
	
	AosIILBigI64Ptr thisPtr(this, false);
	subiil->setParentIIL(thisPtr);	
	subiil->setIILIdx(0);

	// 4. index 2 subiils
	//ken 2012/04/25
	//rslt = createSubiilIndex();
	//aos_assert_r(rslt, false);
	
	mMinVals[0] = subiil->getMinValue();
	mMaxVals[0] = subiil->getMaxValue();

	mSubiils[0] = subiil;
	mMinDocids[0] = subiil->getMinDocid();
	mMaxDocids[0] = subiil->getMaxDocid();
	mIILIds[0] = subiil->getIILID();
	mNumEntries[0] = subiil->getNumDocs();
	mNumDocs = subiil->getNumDocs();
	mNumSubiils = subiil_list.size() + 1;

	for(u32 i=0; i<subiil_list.size(); i++)
	{
		aos_assert_r(subiil_list[i], false);
		
		mSubiils[i+1] = subiil_list[i];
		mSubiils[i+1]->setParentIIL(thisPtr);
		mSubiils[i+1]->setIILIdx(i+1);
	
		mMinDocids[i+1] = mSubiils[i+1]->getMinDocid();
		mMaxDocids[i+1] = mSubiils[i+1]->getMaxDocid();
		mIILIds[i+1] = mSubiils[i+1]->getIILID();
		mNumEntries[i+1] = mSubiils[i+1]->getNumDocs();
		mNumDocs += mSubiils[i+1]->getNumDocs();
	
		mMinVals[i+1] = mSubiils[i+1]->getMinValue();
		mMaxVals[i+1] = mSubiils[i+1]->getMaxValue();
	}

	return true;	
}


bool
AosIILBigI64::addSubiils(
		const i64 &iilidx,
		const vector<AosIILBigI64Ptr> &subiil_list,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sanityCheckPriv(rdata), false);
	aos_assert_r(isParentIIL(),false);
	// It inserts the new subiil 'newsubiil' after 'crtsubiil'. This function
	// must be called by the root IIL.
	if (!mSubiils)
	{
		bool rslt = createSubiilIndex();
		aos_assert_r(rslt, false);
	}

	i64 num = subiil_list.size();
	if (num <= 0) 
	{
		return true;
	}

	aos_assert_r(mNumSubiils + num < mMaxSubIILs + (i64)eExtraSubIILsSpace, false);
	aos_assert_r(iilidx >= 0, false);

	// 1. Insert the new subiil in mSubiils[]
	for(i64 i=0; i<num; i++)
	{
		aos_assert_r(!mSubiils[mNumSubiils+i], false);
	}

	i64 num_to_move = mNumSubiils - iilidx;
	if (iilidx <= mNumSubiils-1)
	{
		//ken 2012/11/12
		// this place must use for loop
		//memmove(&mSubiils[iilidx+num], &mSubiils[iilidx], sizeof(AosIILBigI64Ptr) * num_to_move);
		//memset(&mSubiils[iilidx], 0, sizeof(AosIILBigI64Ptr) * num);
		for(int i=num_to_move; i>=0; i--)
		{
			mSubiils[iilidx+num+i] = mSubiils[iilidx+i];
		}
		for(int i=0; i<num; i++)
		{
			mSubiils[iilidx+i] = 0;
		}

        memmove(&mMinVals[iilidx+num], &mMinVals[iilidx], sizeof(i64) * num_to_move);
        memset(&mMinVals[iilidx], 0, sizeof(i64) * num);

        memmove(&mMinDocids[iilidx+num], &mMinDocids[iilidx], sizeof(u64) * num_to_move);
        memset(&mMinDocids[iilidx], 0, sizeof(u64) * num);

        memmove(&mMaxVals[iilidx+num], &mMaxVals[iilidx], sizeof(i64) * num_to_move);
        memset(&mMaxVals[iilidx], 0, sizeof(i64) * num);

        memmove(&mMaxDocids[iilidx+num], &mMaxDocids[iilidx], sizeof(u64) * num_to_move);
        memset(&mMaxDocids[iilidx], 0, sizeof(u64) * num);

        memmove(&mIILIds[iilidx+num], &mIILIds[iilidx], sizeof(u64) * num_to_move);
		memset(&mIILIds[iilidx], 0, sizeof(u64) * num);

        memmove(&mNumEntries[iilidx+num], &mNumEntries[iilidx], sizeof(i64) * num_to_move);
		memset(&mNumEntries[iilidx], 0, sizeof(i64) * num);

		for (i64 i=iilidx+num; i<mNumSubiils+num; i++)
		{
			if (mSubiils[i].notNull())
			{
				mSubiils[i]->setIILIdx(i);
			}
		}
	}	

	AosIILBigI64Ptr thisPtr(this, false);
	for(i64 i=0; i<num; i++)
	{
		aos_assert_r(subiil_list[i], false);

		mSubiils[iilidx+i] = subiil_list[i];
		mSubiils[iilidx+i]->setIILIdx(iilidx+i);
		mSubiils[iilidx+i]->setParentIIL(thisPtr);
		
		mIILIds[iilidx+i] = mSubiils[iilidx+i]->getIILID();
		mMinDocids[iilidx+i] = mSubiils[iilidx+i]->getMinDocid();
		mMaxDocids[iilidx+i] = mSubiils[iilidx+i]->getMaxDocid();
		mNumEntries[iilidx+i] = mSubiils[iilidx+i]->getNumDocs();
		mNumDocs += mSubiils[iilidx+i]->getNumDocs();
		
		mMinVals[iilidx+i] = mSubiils[iilidx+i]->getMinValue();
		mMaxVals[iilidx+i] = mSubiils[iilidx+i]->getMaxValue();
	}
	mNumSubiils += num;

	aos_assert_r(sanityCheckPriv(rdata), false);
	if (mParentIIL)
	{
		bool rslt = mParentIIL->updateIndexData(mIILIdx, true, true, rdata);
		aos_assert_r(rslt, false);
	}

	mIsDirty = true;
	aos_assert_r(sanityCheckPriv(rdata), false);
	return true;
}


bool	
AosIILBigI64::mergeSubiilPriv(
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosIILBigI64::createSubiilIndex()
{
//shawn when we create a branch node,call this function
	aos_assert_r(!mSubiils, false);

	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILBigI64Ptr[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mSubiils, false);
	}
	
	if (!mMinVals)
	{
		mMinVals = OmnNew i64[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mMinVals, false);
		memset(mMinVals, 0, sizeof(i64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}
	
	if (!mMaxVals)
	{
		mMaxVals = OmnNew i64[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mMaxVals, false);
		memset(mMaxVals, 0, sizeof(i64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}
	
	if (!mMinDocids)
	{
		mMinDocids = OmnNew u64[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mMinDocids, false);
		memset(mMinDocids, 0, sizeof(u64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}

	if (!mMaxDocids)
	{
		mMaxDocids = OmnNew u64[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mMaxDocids, false);
		memset(mMaxDocids, 0, sizeof(u64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}
	
	if (!mIILIds)
	{
		mIILIds = OmnNew u64[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mIILIds, false);
		memset(mIILIds, 0, sizeof(u64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}
	
	if (!mNumEntries)
	{
		mNumEntries = OmnNew i64[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mNumEntries, false);
		memset(mNumEntries, 0, sizeof(i64) * (mMaxSubIILs + eExtraSubIILsSpace));
	}
	
	return true;
}

