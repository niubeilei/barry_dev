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
// 	Created By Ken Lee, 2014/09/11
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/JimoTable.h"

#include "IILMgr/IILMgr.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/BitmapTreeMgrObj.h"
#include "SEInterfaces/IILExecutorObj.h"


bool 
AosJimoTable::splitCheck()
{
	if (mNumSubiils <= AosIIL::mMaxSubIILs)
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


AosJimoTablePtr
AosJimoTable::splitContentUtil(const AosRundataPtr &rdata)
{
	aos_assert_r(mCmpTag, 0);

	AosIILObjPtr iil = AosIILMgr::getSelf()->createSubIILSafe(
		mIILID, mSiteid, mSnapShotId,
		eAosIILType_JimoTable, mIsPersis, rdata);
	aos_assert_r(iil && iil->getIILType() == eAosIILType_JimoTable, 0);

	AosJimoTablePtr newiil = dynamic_cast<AosJimoTable *>(iil.getPtr());
	newiil->setCmpTag(mCmpTag, rdata);
	newiil->setSnapShotId(mSnapShotId);
	newiil->setIILLevel(mLevel);
	newiil->setSiteid(mSiteid);
	newiil->setDirty(true);
	
	mIsDirty = true;
	return newiil;
}


bool
AosJimoTable::splitListPriv(const AosRundataPtr &rdata)
{
	aos_assert_r(splitCheck(), false);
	aos_assert_r(isLeafIIL(), false);

	AosJimoTablePtr subiil = splitLeafContent(rdata);
	aos_assert_r(subiil, false);

	vector<AosJimoTablePtr> subiil_list;
	subiil_list.push_back(subiil);
	if (isSingleIIL())
	{
		return splitListSinglePriv(subiil_list, rdata);
	}
	
	aos_assert_r(mParentIIL, false);
	return mParentIIL->subiilSplited(mIILIdx, subiil_list, rdata);
}


AosJimoTablePtr
AosJimoTable::splitLeafContent(const AosRundataPtr &rdata)
{
	AosJimoTablePtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, 0);

	AosBuffArrayVarPtr array = mData->split(rdata);
	aos_assert_r(array, 0);

	bool rslt = subiil->initSubiilLeaf(array, rdata);
	aos_assert_r(rslt, 0);

	rslt = initSubiilLeaf(mData, rdata);
	aos_assert_r(rslt, 0);

	return subiil;	
}


bool 
AosJimoTable::initSubiilLeaf(
		const AosBuffArrayVarPtr &array,
		const AosRundataPtr &rdata)
{
	aos_assert_r(array, false);
	
	mData = array;
	mNumDocs = mData->getNumEntries();
	mMemSize = mData->getMemSize();
	mIsDirty = true;

	return true;
}


bool
AosJimoTable::splitListSinglePriv(
		const vector<AosJimoTablePtr> &subiil_list,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);

	AosJimoTablePtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->initSubiilLeaf(mData, rdata);
	aos_assert_r(rslt, false);

	mLevel = 1;

	rslt = createSubiilIndex();
	aos_assert_r(rslt, false);

	rslt = createMaxMinBuff(rdata);
	aos_assert_r(rslt, false);

	vector<AosJimoTablePtr> subiils;
	subiils.push_back(subiil);
	subiils.insert(subiils.end(), subiil_list.begin(), subiil_list.end());

	mNumDocs = 0;
	mMemSize = 0;
	mNumSubiils = 0;
	i64 num = subiils.size();

	int len = 0;
	AosJimoTablePtr thisPtr(this, false);
	for (i64 i=0; i<num; i++)
	{
		aos_assert_r(subiils[i], false);

		mSubiils[i] = subiils[i];
		mSubiils[i]->setParentIIL(thisPtr);	
		mSubiils[i]->setIILIdx(i);

		mNumDocs += mSubiils[i]->getNumDocs();
		mMemSize += mSubiils[i]->getMemSize();
		mIILIds[i] = mSubiils[i]->getIILID();
		mNumEntries[i] = mSubiils[i]->getNumDocs();
		mSubMemSize[i] = mSubiils[i]->getMemSize();

		const char *min_data = mSubiils[i]->getMinValue(len);
		aos_assert_r(min_data && len > 0, false);

		mMinData->insertEntry(i, min_data, len, rdata.getPtr());
		mMinData->setSorted(true);

		const char *max_data = mSubiils[i]->getMaxValue(len);
		aos_assert_r(max_data && len > 0, false);

		mMaxData->insertEntry(i, max_data, len, rdata.getPtr());
		mMaxData->setSorted(true);
	}

	mNumSubiils = num;
	mIsDirty = true;
	
	return true;	
}


bool
AosJimoTable::createSubiilIndex()
{
	i64 size = AosIIL::mMaxSubIILs + AosIIL::eExtraSubIILsSpace;
	
	if (!mSubiils) mSubiils = OmnNew AosJimoTablePtr[size];
	aos_assert_r(mSubiils, false);
	for (i64 i=0; i<size; i++)
	{
		mSubiils[i] = 0;
	}

	if (!mIILIds) mIILIds = OmnNew u64[size];
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * size);

	if (!mNumEntries) mNumEntries = OmnNew i64[size];
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(i64) * size);

	if (!mSubMemSize) mSubMemSize = OmnNew i64[size];
	aos_assert_r(mSubMemSize, false);
	memset(mSubMemSize, 0, sizeof(i64) * size);

	return true;
}


bool
AosJimoTable::createMaxMinBuff(const AosRundataPtr &rdata)
{
	mMinData = AosBuffArrayVar::create(mCmpTag, true, rdata);
	aos_assert_r(mMinData, false);

	mMaxData = AosBuffArrayVar::create(mCmpTag, true, rdata);
	aos_assert_r(mMaxData, false);
	
	return true;
}


bool
AosJimoTable::subiilSplited(
		const i64 &iilIdx,
		const vector<AosJimoTablePtr> &subiil_list,
		const AosRundataPtr &rdata)
{
	bool rslt = updateIndexData(iilIdx, true, true, rdata);
	aos_assert_r(rslt, false);

	rslt = addSubiils(iilIdx + 1, subiil_list, rdata);
	aos_assert_r(rslt, false);

	return subiilSplited(rdata);
}


bool
AosJimoTable::subiilSplited(const AosRundataPtr &rdata)
{
	if (mNumSubiils <= AosIIL::mMaxSubIILs)
	{
		// do not need to split
		return true;
	}
	
	AosJimoTablePtr subiil = splitParentContent(rdata);
	aos_assert_r(subiil, false);

	vector<AosJimoTablePtr> subiil_list;
	subiil_list.push_back(subiil);

	if (mParentIIL)
	{
		return mParentIIL->subiilSplited(mIILIdx, subiil_list, rdata);
	}
	
	return splitListRootPriv(subiil_list, rdata);
}				  


bool
AosJimoTable::addSubiils(
		const i64 &iilIdx,
		const vector<AosJimoTablePtr> &subiil_list,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sanityCheckPriv(rdata), false);
	aos_assert_r(isParentIIL(), false);

	i64 num = subiil_list.size();
	if (num <= 0) 
	{
		return true;
	}

	aos_assert_r(iilIdx >= 0, false);
	aos_assert_r(mNumSubiils + num < (AosIIL::mMaxSubIILs + AosIIL::eExtraSubIILsSpace), false);

	for (i64 i=0; i<num; i++)
	{
		aos_assert_r(!mSubiils[mNumSubiils+i], false);
	}

	i64 num_to_move = mNumSubiils - iilIdx;
	if (iilIdx <= mNumSubiils-1)
	{
		for (int i=num_to_move; i>=0; i--)
		{
			mSubiils[iilIdx+num+i] = mSubiils[iilIdx+i];
			if (mSubiils[iilIdx+num+i].notNull())
			{
				mSubiils[iilIdx+num+i]->setIILIdx(iilIdx+num+i);
			}
		}

		for (int i=0; i<num; i++)
		{
			mSubiils[iilIdx+i] = 0;
		}

		memmove(&mIILIds[iilIdx+num], &mIILIds[iilIdx], sizeof(u64) * num_to_move);
		memset(&mIILIds[iilIdx], 0, sizeof(u64) * num);

		memmove(&mNumEntries[iilIdx+num], &mNumEntries[iilIdx], sizeof(i64) * num_to_move);
		memset(&mNumEntries[iilIdx], 0, sizeof(i64) * num);

		memmove(&mSubMemSize[iilIdx+num], &mSubMemSize[iilIdx], sizeof(i64) * num_to_move);
		memset(&mSubMemSize[iilIdx], 0, sizeof(i64) * num);
	}

	int len = 0;
	AosJimoTablePtr thisPtr(this, false);
	for (i64 i=iilIdx; i<iilIdx+num; i++)
	{
		aos_assert_r(subiil_list[i-iilIdx], false);

		mSubiils[i] = subiil_list[i-iilIdx];
		mSubiils[i]->setIILIdx(i);
		mSubiils[i]->setParentIIL(thisPtr);

		mNumDocs += mSubiils[i]->getNumDocs();
		mMemSize += mSubiils[i]->getMemSize();
		mIILIds[i] = mSubiils[i]->getIILID();
		mNumEntries[i] = mSubiils[i]->getNumDocs();
		mSubMemSize[i] = mSubiils[i]->getMemSize();
	
		const char *min_data = mSubiils[i]->getMinValue(len);
		aos_assert_r(min_data && len > 0, false);

		mMinData->insertEntry(i, min_data, len, rdata.getPtr());
		mMinData->setSorted(true);

		const char *max_data = mSubiils[i]->getMaxValue(len);
		aos_assert_r(max_data && len > 0, false);

		mMaxData->insertEntry(i, max_data, len, rdata.getPtr());
		mMaxData->setSorted(true);
	}

	mNumSubiils += num;
	mIsDirty = true;

	aos_assert_r(sanityCheckPriv(rdata), false);
	if (mParentIIL)
	{
		bool rslt = mParentIIL->updateIndexData(mIILIdx, true, true, rdata);
		aos_assert_r(rslt, false);
	}

	aos_assert_r(sanityCheckPriv(rdata), false);
	return true;
}


AosJimoTablePtr
AosJimoTable::splitParentContent(const AosRundataPtr &rdata)
{
	AosJimoTablePtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, 0);

	i64 num1 = mNumSubiils / 2;
	i64 num2 = mNumSubiils - num1;

	AosBuffArrayVarPtr minarray = mMinData->split(rdata);
	AosBuffArrayVarPtr maxarray = mMaxData->split(rdata);

	u64* iilids = mIILIds;
	i64* numEntries = mNumEntries;
	i64* subMemSize = mSubMemSize;
	AosJimoTablePtr* subiils = mSubiils;

	mIILIds = 0;
	mNumEntries = 0;
	mSubMemSize = 0;
	mSubiils = 0;

	bool rslt = subiil->initSubiilParent(num2, minarray, maxarray,
		&iilids[num1], &numEntries[num1], &subMemSize[num1], &subiils[num1]);
	aos_assert_r(rslt, 0);

	rslt = initSubiilParent(num1, mMinData, mMaxData, 
		iilids, numEntries, subMemSize, subiils);
	aos_assert_r(rslt, 0);

	delete [] iilids;
	delete [] numEntries;
	delete [] subMemSize;

	for (i64 i=0; i<num1 + num2; i++)
	{
		subiils[i] = 0;
	}
	delete [] subiils;

	return subiil;	
}


bool 
AosJimoTable::initSubiilParent(
		const i64 &num,
		const AosBuffArrayVarPtr &minarray,
		const AosBuffArrayVarPtr &maxarray,
		const u64 *iilids,
		const i64 *numEntries,
		const i64 *subMemSize,
		const AosJimoTablePtr *subiils)
{
	mNumDocs = 0;
	mMemSize = 0;

	mMinData = minarray;
	mMaxData = maxarray;

	mNumSubiils = num;
	mIsDirty = true;

	bool rslt = createSubiilIndex();
	aos_assert_r(rslt, false);

	aos_assert_r(mIILIds, false);
	memcpy(mIILIds, iilids, sizeof(u64) * mNumSubiils);

	aos_assert_r(mNumEntries, false);
	memcpy(mNumEntries, numEntries, sizeof(i64) * mNumSubiils);

	aos_assert_r(mSubMemSize, false);
	memcpy(mSubMemSize, subMemSize, sizeof(i64) * mNumSubiils);
	
	aos_assert_r(mSubiils, false);
	AosJimoTablePtr thisPtr(this, false);
	for (i64 i=0; i<mNumSubiils; i++)
	{
		mNumDocs += mNumEntries[i];
		mMemSize += mSubMemSize[i];	
		mSubiils[i] = subiils[i];
		if (!mSubiils[i].isNull())
		{
			mSubiils[i]->setIILIdx(i);
			mSubiils[i]->setParentIIL(thisPtr);
		}
	}
	
	return true;
}


bool
AosJimoTable::splitListRootPriv(
		const vector<AosJimoTablePtr> &subiil_list,
		const AosRundataPtr &rdata)
{
	AosJimoTablePtr subiil = splitContentUtil(rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->initSubiilParent(mNumSubiils, mMinData,
		mMaxData, mIILIds, mNumEntries, mSubMemSize, mSubiils);
	aos_assert_r(rslt, false);

	mMinData = 0;
	mMaxData = 0;

	mLevel++;
	
	rslt = createSubiilIndex();
	aos_assert_r(rslt, false);

	rslt = createMaxMinBuff(rdata);
	aos_assert_r(rslt, false);

	vector<AosJimoTablePtr> subiils;
	subiils.push_back(subiil);
	subiils.insert(subiils.end(), subiil_list.begin(), subiil_list.end());

	mNumDocs = 0;
	mMemSize = 0;
	mNumSubiils = 0;
	i64 num = subiils.size();

	int len = 0;
	AosJimoTablePtr thisPtr(this, false);
	for (i64 i=0; i<num; i++)
	{
		aos_assert_r(subiils[i], false);

		mSubiils[i] = subiils[i];
		mSubiils[i]->setParentIIL(thisPtr);
		mSubiils[i]->setIILIdx(i);

		mNumDocs += mSubiils[i]->getNumDocs();
		mMemSize += mSubiils[i]->getMemSize();
		mIILIds[i] = mSubiils[i]->getIILID();
		mNumEntries[i] = mSubiils[i]->getNumDocs();
		mSubMemSize[i] = mSubiils[i]->getMemSize();

		const char *min_data = mSubiils[i]->getMinValue(len);
		aos_assert_r(min_data && len > 0, false);

		mMinData->insertEntry(i, min_data, len, rdata.getPtr());
		mMinData->setSorted(true);

		const char *max_data = mSubiils[i]->getMaxValue(len);
		aos_assert_r(max_data && len > 0, false);

		mMaxData->insertEntry(i, max_data, len, rdata.getPtr());
		mMaxData->setSorted(true);
	}

	mNumSubiils = num;
	mIsDirty = true;

	return true;	
}




/*
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
AosIILBigStr::mergeSubiilPriv(
		const i64 &iilIdx,
		const AosRundataPtr &rdata)
{
	return true;
}

*/

