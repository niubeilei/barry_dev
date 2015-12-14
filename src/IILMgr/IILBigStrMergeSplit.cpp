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
#include "IILMgr/IILBigStr.h"

#include "IILMgr/IILMgr.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/BitmapTreeMgrObj.h"
#include "SEInterfaces/IILExecutorObj.h"


bool
AosIILBigStr::clearIILPriv(const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		for(i64 i=0; i<mNumDocs; i++)
		{
			aos_assert_r(mValues[i], false);
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			mValues[i] = 0;
		}
	}
	
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
AosIILBigStr::deleteIILPriv(
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	bool rslt = true;
	if (!isSingleIIL())
	{
		vector<u64> iilids;
		AosIILObjPtr iil;
		AosIILType iiltype;
		AosIILBigStrPtr subiil;
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
					if (iil) subiil = (AosIILBigStr *)(iil.getPtr());
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
AosIILBigStr::deleteIILRecPriv(
		vector<u64> &iilids,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL()) return true;

	bool rslt = true;
	AosIILObjPtr iil;
	AosIILType iiltype;
	AosIILBigStrPtr subiil;
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
			if (iil) subiil = (AosIILBigStr *)(iil.getPtr());
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
AosIILBigStr::splitListPriv(
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

	AosIILBigStrPtr subiil = splitLeafContent(rdata);
	aos_assert_r(subiil, false);

	vector<AosIILBigStrPtr> subiil_list;
	subiil_list.push_back(subiil);
	if (isSingleIIL())
	{
		return splitListSinglePriv(subiil_list, rdata);
	}
	
	aos_assert_r(mParentIIL, false);
	return mParentIIL->subiilSplited(mIILIdx, subiil_list, executor, rdata);
}


bool 
AosIILBigStr::splitCheck()
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


AosIILBigStrPtr
AosIILBigStr::splitLeafContent(const AosRundataPtr &rdata)
{
	AosIILBigStrPtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, 0);

	// Initialize the new subiil
	i64 startidx = mNumDocs / 2;
	const i64 len = mNumDocs - startidx;
	bool rslt = subiil->initSubiilLeaf(&mDocids[startidx], &mValues[startidx], len);
	aos_assert_r(rslt, 0);

	// 5. Shrink this IIL
	memset(&mValues[startidx], 0, sizeof(char *) * len);
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	aos_assert_r(mNumDocs < mMaxBigIILSize, 0);
	
	return subiil;	
}


bool 
AosIILBigStr::initSubiilLeaf(
		u64 *docids,
		char **values, 
		const i64 &num)
{
	aos_assert_r(mIILType == eAosIILType_BigStr, false);

    mWordId = AOS_INVWID;
	// ModifyNumDocs
	aos_assert_r(mNumDocs < mMaxBigIILSize, false);
    mNumDocs = num; 

	if (mMinVals)
	{
		for(i64 i=0; i<mNumSubiils; i++)
		{
			if (mMinVals[i])
			{
				OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
				mMinVals[i] = 0;
			}
		}
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
		for(i64 i=0; i<mNumSubiils; i++)
		{
			if (mMaxVals[i])
			{
				OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
				mMaxVals[i] = 0;
			}
		}
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

	memcpy(mValues, values, sizeof(char *) * mNumDocs);
	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool 
AosIILBigStr::initSubiilParent(
		u64 *minDocids,
		char **minValues, 
		u64 *maxDocids,
		char **maxValues, 
		u64 *iilids,
		i64 *numEntries,
		AosIILBigStrPtr *subiils,
		const i64 &numSubiils)
{
    mWordId = AOS_INVWID;
    mNumSubiils = numSubiils; 

	// set mMinVals
	if (mMinVals)
	{
		for(i64 i=0; i<mNumSubiils; i++)
		{
			if (mMinVals[i])
			{
				OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
				mMinVals[i] = 0;
			}
		}
	}
	else
	{
		mMinVals = OmnNew char*[mMaxSubIILs + eExtraSubIILsSpace];
		memset(mMinVals, 0, sizeof(char *) * (mMaxSubIILs + eExtraSubIILsSpace));
	}

	// set mMaxVals
	if (mMaxVals)
	{
		for(i64 i=0; i<mNumSubiils; i++)
		{
			if (mMaxVals[i])
			{
				OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
				mMaxVals[i] = 0;
			}
		}
	}
	else
	{
		mMaxVals = OmnNew char*[mMaxSubIILs + eExtraSubIILsSpace];
		// Chen Ding, 2013/11/15
		// memset(mMinVals, 0, sizeof(char *) * (mMaxSubIILs + eExtraSubIILsSpace));
		memset(mMaxVals, 0, sizeof(char *) * (mMaxSubIILs + eExtraSubIILsSpace));
	}

	aos_assert_r(mMinVals, false);
	aos_assert_r(mMaxVals, false);
	memset(mMinVals, 0, sizeof(char *) * mMaxSubIILs);
	memcpy(mMinVals, minValues, sizeof(char *) * mNumSubiils);
	memset(mMaxVals, 0, sizeof(char *) * mMaxSubIILs);
	memcpy(mMaxVals, maxValues, sizeof(char *) * mNumSubiils);

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
		//mSubiils = OmnNew AosIILBigStrPtr[mMaxSubIILs + eExtraSubIILsSpace];
	}
	aos_assert_r(mSubiils, false);

	//ken 2012/11/12
	// this place must use for loop
	//memset(mSubiils, 0, sizeof(AosIILBigStrPtr) * (mMaxSubIILs + eExtraSubIILsSpace));
	//memcpy(mSubiils, subiils, sizeof(AosIILBigStrPtr) * mNumSubiils);
	for(i64 i=0; i<mMaxSubIILs + (i64)eExtraSubIILsSpace; i++)
	{
		mSubiils[i] = 0;
	}
	for(i64 i=0; i<mNumSubiils; i++)
	{
		mSubiils[i] = subiils[i];
	}
	
	// update info in subiils
	AosIILBigStrPtr thisPtr(this, false);
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
AosIILBigStr::splitListSinglePriv(
		const vector<AosIILBigStrPtr> &subiil_list,
		const AosRundataPtr &rdata)
{
	// the 2nd subiil is created, now we need to :
	// 1. create a new subiil
	// 2. move content to new subiil
	// 3. make itself a root iil
	// 4. index 2 subiils
	
	// 1. create a new subiil
	//aos_assert_r(mNumDocs > 0, false);

	AosIILBigStrPtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, false);

	// 2. move content to new subiil
	// Initialize the new subiil
	bool rslt = subiil->initSubiilLeaf(mDocids, mValues, mNumDocs);
	aos_assert_r(rslt, false);

	memset(mValues, 0, sizeof(char *) * mNumDocs);
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
	
	AosIILBigStrPtr thisPtr(this, false);
	subiil->setParentIIL(thisPtr);	
	subiil->setIILIdx(0);

	// 4. index 2 subiils
	rslt = createSubiilIndex();
	aos_assert_r(rslt, false);
	
	const char* str;
	i64 length = 0;
	i64 len = 0;
	char* ptr;
	
	str = subiil->getMinValue();
	length = strlen(str);	
	len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
	ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
	aos_assert_r(ptr, false);
	mMinVals[0] = ptr;
	//strcpy(mMinVals[0], str);
	strncpy(mMinVals[0], str, len);
	mMinVals[0][len] = 0;

	str = subiil->getMaxValue();
	length = strlen(str);	
	len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
	ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
	aos_assert_r(ptr, false);
	mMaxVals[0] = ptr;
	//strcpy(mMaxVals[0], str);
	strncpy(mMaxVals[0], str, len);
	mMaxVals[0][len] = 0;

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

		str = mSubiils[i+1]->getMinValue();
		length = strlen(str);	
		len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMinVals[i+1] = ptr;
		//strcpy(mMinVals[i+1], str);
		strncpy(mMinVals[i+1], str, len);
		mMinVals[i+1][len] = 0;

		str = mSubiils[i+1]->getMaxValue();
		length = strlen(str);	
		len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMaxVals[i+1] = ptr;
		//strcpy(mMaxVals[i+1], str);
		strncpy(mMaxVals[i+1], str, len);
		mMaxVals[i+1][len] = 0;
	}

	return true;	
}


bool
AosIILBigStr::subiilSplited(
		const i64 &iilIdx,
		const vector<AosIILBigStrPtr> &subiil_list,
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
AosIILBigStr::subiilSplited(
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
	AosIILBigStrPtr newsubiil = splitParentContent(rdata);
	aos_assert_r(newsubiil, false);

	vector<AosIILBigStrPtr> new_subiil_list;
	new_subiil_list.push_back(newsubiil);

	//3. (need split) check whether it is root
	if (mParentIIL)
	{
#ifdef AOS_SUPPORT_BITMAP_QUERY
AOS_BITMAP_QUERY_START
		// split tree 
		executor->lock();
		AosBitmapTreeObjPtr new_tree = AosBitmapTreeMgrObj::getObject()->createTree(
			rdata, newsubiil->getIILID(), newsubiil->getLevel(), mMaxSubIILs);
		mBitmapTree->splitInnerTree(rdata, executor,
			mIILIds[mNumSubiils-1], newsubiil->getIILID(), new_tree);
		
		// insert node into parent bitmap tree
		newsubiil->setBitmapTree(new_tree,rdata);
//		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata,new_tree);
		
		vector<u64> nodes_list;
		nodes_list.push_back(newsubiil->getIILID());
	
		AosBitmapTreeObjPtr tree = mParentIIL->getBitmapTree(rdata);
		aos_assert_r(tree,false);
		tree->insertNodes(
			rdata, executor, mIILID, nodes_list,false);

		executor->unlock();
AOS_BITMAP_QUERY_END
#endif //#ifdef AOS_SUPPORT_BITMAP_QUERY

		return mParentIIL->subiilSplited(mIILIdx, new_subiil_list, executor, rdata);
	}
	
	// it is root iil, and it need to split
	return splitListRootPriv(new_subiil_list, executor, rdata);
}				  


AosIILBigStrPtr
AosIILBigStr::splitContentUtil(const AosRundataPtr &rdata)
{
	// 1. Create the subiil
	AosIILObjPtr subiil = AosIILMgr::getSelf()->createSubIILSafe(
		mIILID, mSiteid, mSnapShotId, eAosIILType_BigStr, mIsPersis, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->getIILType() == eAosIILType_BigStr, 0);
	AosIILBigStrPtr newsubiil = (AosIILBigStr*)subiil.getPtr();
	
	newsubiil->setSnapShotId(mSnapShotId);

	// 2. Set subiil attributes
//	subiil->setRootIIL(mRootIIL);
//	newsubiil->setParentIIL(mParentIIL);
//	if (mIILIdx < 0) mIILIdx = 0;
//	newsubiil->setIILIdx(mIILIdx+1);
	newsubiil->setIILLevel(mLevel);
	newsubiil->setSiteid(mSiteid);
	if (mIsNumAlpha) newsubiil->setNumAlpha();
	
	mIsDirty = true;
	newsubiil->setDirty(true);
	return newsubiil;
}


AosIILBigStrPtr
AosIILBigStr::splitParentContent(const AosRundataPtr &rdata)
{
	AosIILBigStrPtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, 0);

	// 4. Initialize the new subiil
	i64 startidx = mNumSubiils / 2;
	const i64 len = mNumSubiils - startidx;
	
	bool rslt = subiil->initSubiilParent(
		&mMinDocids[startidx], &mMinVals[startidx],	&mMaxDocids[startidx], &mMaxVals[startidx],
		&mIILIds[startidx], &mNumEntries[startidx], &mSubiils[startidx], len);
	aos_assert_r(rslt, 0);

	// 5. Shrink this IIL
	memset(&mMinVals[startidx], 0, sizeof(char *) * len);
	memset(&mMinDocids[startidx], 0, sizeof(u64) * len);
	memset(&mMaxVals[startidx], 0, sizeof(char *) * len);
	memset(&mMaxDocids[startidx], 0, sizeof(u64) * len);
	memset(&mIILIds[startidx], 0, sizeof(u64) * len);
	memset(&mNumEntries[startidx], 0, sizeof(i64) * len);

	//ken 2012/11/12
	// this place must use for loop
	//memset(&mSubiils[startidx], 0, sizeof(AosIILBigStrPtr) * len);
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
AosIILBigStr::splitListRootPriv(
		const vector<AosIILBigStrPtr> &subiil_list,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	// the 2nd subiil is created, now we need to :
	// 1. create a new subiil
	// 2. move content to new subiil
	// 3. make itself a root iil
	// 4. index 2 subiils
	
	// 1. create a new subiil
	AosIILBigStrPtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, false);

	// 2. move content to new subiil
	// Initialize the new subiil
	bool rslt = subiil->initSubiilParent(
		mMinDocids, mMinVals, mMaxDocids, mMaxVals, mIILIds,
		mNumEntries, mSubiils, mNumSubiils);
	aos_assert_r(rslt, false);

	memset(mMinVals, 0, sizeof(char *) * mNumSubiils);
	memset(mMinDocids, 0, sizeof(u64) * mNumSubiils);
	memset(mMaxVals, 0, sizeof(char *) * mNumSubiils);
	memset(mMaxDocids, 0, sizeof(u64) * mNumSubiils);
	memset(mIILIds, 0, sizeof(u64) * mNumSubiils);
	memset(mNumEntries, 0, sizeof(i64) * mNumSubiils);

	//ken 2012/11/12
	// this place must use for loop
	//memset(mSubiils, 0, sizeof(AosIILBigStrPtr) * mNumSubiils);
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
	
	AosIILBigStrPtr thisPtr(this, false);
	subiil->setParentIIL(thisPtr);	
	subiil->setIILIdx(0);

	// 4. index 2 subiils
	//ken 2012/04/25
	//rslt = createSubiilIndex();
	//aos_assert_r(rslt, false);
	
	const char* str;
	i64 length = 0;
	i64 len = 0;
	char* ptr;
	
	str = subiil->getMinValue();
	length = strlen(str);	
	len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
	ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
	aos_assert_r(ptr, false);
	mMinVals[0] = ptr;
	//strcpy(mMinVals[0], str);
	strncpy(mMinVals[0], str, len);
	mMinVals[0][len] = 0;

	str = subiil->getMaxValue();
	length = strlen(str);	
	len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
	ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
	aos_assert_r(ptr, false);
	mMaxVals[0] = ptr;
	//strcpy(mMaxVals[0], str);
	strncpy(mMaxVals[0], str, len);
	mMaxVals[0][len] = 0;

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
	
		str = mSubiils[i+1]->getMinValue();
		length = strlen(str);	
		len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMinVals[i+1] = ptr;
		//strcpy(mMinVals[i+1], str);
		strncpy(mMinVals[i+1], str, len);
		mMinVals[i+1][len] = 0;
	
		str = subiil_list[i]->getMaxValue();
		length = strlen(str);	
		len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMaxVals[i+1] = ptr;
		//strcpy(mMaxVals[i+1],str);
		strncpy(mMaxVals[i+1],str, len);
		mMaxVals[i+1][len] = 0;
	}

#ifdef AOS_SUPPORT_BITMAP_QUERY
AOS_BITMAP_QUERY_START
	// Bitmap processing
	// Note that currently we only support parent iil split into 2 pieces!!!!!!!!!!!
	// case 2: root iil splited
	// 1. the tree should be split into 2 trees
	// 2. this iil should keep the original node
	// 3. this iil should have a tree which contains 2 nodes
	// 4. the splited nodes should be sent to replace node and the subiil(subiil 2)
	executor->lock();
	if (mBitmapTree)
	{
		AosBitmapTreeObjPtr new_tree1 = AosBitmapTreeMgrObj::getObject()->createTree(rdata,
																					 subiil->getIILID(),
																					 subiil->getLevel(),
																					 mMaxSubIILs);
		AosBitmapTreeObjPtr new_tree2 = AosBitmapTreeMgrObj::getObject()->createTree(rdata,
																					 subiil_list[0]->getIILID(),
																					 subiil->getLevel(),
																					 mMaxSubIILs);
		mBitmapTree->splitRootTree(rdata,
								   executor,
								   subiil->mIILIds[subiil->mNumSubiils-1],//mIILIds[mNumSubiils-1],
								   subiil->getIILID(),
								   subiil_list[0]->getIILID(),
								   new_tree1,
								   new_tree2);// rebuild the root
		subiil->setBitmapTree(new_tree1,rdata);
		subiil_list[0]->setBitmapTree(new_tree2,rdata);
//		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata,mBitmapTree);
//		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata,new_tree1);
//		AosBitmapTreeMgrObj::getObject()->saveBitmapTree(rdata,new_tree2);
	}
	executor->unlock();
AOS_BITMAP_QUERY_END
#endif //#ifdef AOS_SUPPORT_BITMAP_QUERY

	return true;	
}


bool
AosIILBigStr::addSubiils(
		const i64 &iilidx,
		const vector<AosIILBigStrPtr> &subiil_list,
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
		//memmove(&mSubiils[iilidx+num], &mSubiils[iilidx], sizeof(AosIILBigStrPtr) * num_to_move);
		//memset(&mSubiils[iilidx], 0, sizeof(AosIILBigStrPtr) * num);
		for(int i=num_to_move; i>=0; i--)
		{
			mSubiils[iilidx+num+i] = mSubiils[iilidx+i];
		}
		for(int i=0; i<num; i++)
		{
			mSubiils[iilidx+i] = 0;
		}

        memmove(&mMinVals[iilidx+num], &mMinVals[iilidx], sizeof(char *) * num_to_move);
        memset(&mMinVals[iilidx], 0, sizeof(char *) * num);

        memmove(&mMinDocids[iilidx+num], &mMinDocids[iilidx], sizeof(u64) * num_to_move);
        memset(&mMinDocids[iilidx], 0, sizeof(u64) * num);

        memmove(&mMaxVals[iilidx+num], &mMaxVals[iilidx], sizeof(char *) * num_to_move);
        memset(&mMaxVals[iilidx], 0, sizeof(char *) * num);

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

	const char* str;
	i64 length = 0;
	i64 len = 0;
	char* ptr;

	AosIILBigStrPtr thisPtr(this, false);
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
		
		str = mSubiils[iilidx+i]->getMinValue();
		length = strlen(str);	
		len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMinVals[iilidx+i] = ptr;
		//strcpy(mMinVals[iilidx+i], str);
		strncpy(mMinVals[iilidx+i], str, len);
		mMinVals[iilidx+i][len] = 0;
	
		str = mSubiils[iilidx+i]->getMaxValue();
		length = strlen(str);	
		len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mMaxVals[iilidx+i] = ptr;
		//strcpy(mMaxVals[iilidx+i], str);
		strncpy(mMaxVals[iilidx+i], str, len);
		mMaxVals[iilidx+i][len] = 0;
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
AosIILBigStr::mergeSubiilPriv(
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
	return true;
}


bool
AosIILBigStr::createSubiilIndex()
{
//shawn when we create a branch node,call this function
	aos_assert_r(!mSubiils, false);

	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILBigStrPtr[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mSubiils, false);
	}
	
	if (!mMinVals)
	{
		mMinVals = OmnNew char*[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mMinVals, false);
		memset(mMinVals, 0, sizeof(char *) * (mMaxSubIILs + eExtraSubIILsSpace));
	}
	
	if (!mMaxVals)
	{
		mMaxVals = OmnNew char*[mMaxSubIILs + eExtraSubIILsSpace];
		aos_assert_r(mMaxVals, false);
		memset(mMaxVals, 0, sizeof(char *) * (mMaxSubIILs + eExtraSubIILsSpace));
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

