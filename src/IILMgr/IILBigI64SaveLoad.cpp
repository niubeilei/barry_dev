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
// Leaf IILs and Non-Leaf IILs member data are different.
//
// Leaf IILs:
// 		mValues			// The values for the IIL
// 		mDocids			// The docids for the IIL
// 		mMinVals		// The minimum value for the IIL
// 		mMaxVals		// The maximum value for the IIL
//
// Non-Leaf IILs:
// 		mValues			// Should not be used
// 		mDocids			// The max docid for each subiil
//		mMinVals		// The minimum value for each subiil
//		mMaxVals		// The maximum value for each subiil
//		mNumEntries		// The number of entries for each subiil
//		mIILIDs			// The IILID for each subiil
//
// Modification History:
// 	Created: 12/11/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILBigI64.h"

#include "IILMgr/IILMgr.h"


bool		
AosIILBigI64::staticInit(const AosXmlTagPtr &theconfig)
{
	// Its configuration is in the subtag "AOSCONFIG_IILMGR"
	aos_assert_r(theconfig, false);
	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;
	
	OmnMemMgr::getSelf();
	// mMaxStrLen = config->getAttrInt(AOSCONFIG_MAX_STRLEN, mMaxStrLen);
	return true;
}


bool
AosIILBigI64::returnSubIILPriv(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	AosIILBigI64Ptr subiil = mSubiils[idx];
	if (!subiil) return true;
	if (subiil->isDirty()) return true;

	AosIILObjPtr iil = dynamic_cast<AosIIL *>(subiil.getPtr());
	bool rslt = AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	mSubiils[idx] = 0;
	return true;
}


bool
AosIILBigI64::prepareMemoryForReloading()
{
	// The IIL may have been reused, which means that it
	// may have allocated some memory. This function checks 
	// whether it has enough memory (based on 'mNumDocs'). 
	// If not enough, it will allocate. 
	// Otherwise, it may thrink the memory.
	//
	// Note that 'mNumDocs' is the number of docs this new
	// IIL has, not the one used by the previous IIL. But
	// 'mMemCap' is the one set by the previous IIL. 

	if (mNumDocs < mMemCap)
	{
		// It has enough memory. If it has more than what we need, 
		// thrink it. We only need 'mNumDocs + mExtraDocids'. Release
		// the extra ones.
		memset(&mValues[mNumDocs + mExtraDocids], 0, sizeof(i64)*(mMemCap - mNumDocs - mExtraDocids));

		// IMPORTANT: the memory held in 'mValues[i]' are for 
		// the previous IIL, not this IIL's!!!
		return true;
	}

	// Grow 'mValues' now
	i64 newsize = mNumDocs + mExtraDocids;
	i64* mem = OmnNew i64[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(i64) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(i64) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;

	bool rslt = expandDocidMem();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigI64::returnSubIILsPriv(
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
// only for root??? branch???
	returned = true;
	if (!isParentIIL())
	{
		return true;
	}

	AosIILBigI64Ptr subiil = 0;
	bool rslt = false;
	bool subIILReturned = false;

	// return all sub IIL except itself
	for(i64 i = 0;i < mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		
		//ken 2011/9/2
		if (subiil.notNull() && !subiil->isDirty())
		{
			rslt = AosIILMgrSelf->returnIILPublic(
				subiil, subIILReturned, returnHeader, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}



bool
AosIILBigI64::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	i64 newsize = mNumDocs + mExtraDocids;
	if (newsize <= mMemCap)
	{
		return true;
	}
	
	i64* mem = OmnNew i64[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(i64) * newsize);

	if (mValues)
	{
		// Copy the contents from the old buffer to 'mem'
		// and release the old memory. Note that this function
		// may be called by loadFromFilePriv(buff). This function
		// will set mNumDocs based on the contents read from
		// the file. In this case, 'mNumDocs' does not really mean
		// that 'mValues[]' has that many values. For this reason,
		// we should copy only min(mNumDocs, mMemCap).
		memcpy(mem, mValues, sizeof(i64) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	aos_assert_r(checkMemory(), false);
	return true;
}

//////////////////////////



bool
AosIILBigI64::resetSubIILInfo(const AosRundataPtr &rdata)
{
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

	if (mMinVals)
	{
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if (mMaxVals)
	{
		OmnDelete [] mMaxVals;
		mMaxVals = 0;
	}

	if (mMinDocids)
	{
		OmnDelete [] mMinDocids;
		mMinDocids = 0;
	}

	if (mMaxDocids)
	{
		OmnDelete [] mMaxDocids;
		mMaxDocids = 0;
	}

	if (mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}

	mRootIIL = 0;
	mNumSubiils = 0;
	mIILIdx = -1;

	//ken 2012/06/15
	mLevel = 0;
	mParentIIL = 0;
	return true;
}


bool
AosIILBigI64::resetSpec()
{
	if (mNumDocs != 0 && isLeafIIL())
	{
		aos_assert_r(mValues, false);
		memset(mValues, 0, sizeof(i64) * mNumDocs);
	}
	
	OmnDelete [] mValues;
	mValues = 0;
	if(mBitmapTree)
	{
		mBitmapTree = 0;
	}
	return true;
}


bool
AosIILBigI64::saveSubIILToLocalFileSafe(const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (isParentIIL())
	{
		// this iil is the root iil of the list
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToLocalFileSafe(rdata);
				aos_assert_r(rslt,false);
				if (mSubiils[i]->isParentIIL())
				{
					mSubiils[i]->saveSubIILToLocalFileSafe(rdata);
				}
			}
		}
	}
	return true;
}


bool
AosIILBigI64::setContentsProtected(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// This function sets its contents from 'buff', which 
	// was read from a file. 'buff' should be arranged
	// as:
	// 	[min_docid][max_docid][docid][docid]...[docid]
	// 	[value][value]...[value]
	// The number of docids is mNumDocs
	// Note that this class may be reused, which means it may
	// already have memory assigned.
	
	mLevel = buff->getInt(0);
	aos_assert_r(mLevel >= 0, false);

	if (isLeafIIL())
	{
		return setContentsSinglePriv(buff, rdata);
	}	

	// 2. Retrieve Subiil Index Data, if needed
	mNumSubiils = buff->getI64(0);
	if (mNumSubiils <= 1 || mNumSubiils > mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	bool rslt;
	//now this place is used to be isNumAlpha flag;
	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);


		rslt = buff->getI64s(mMinVals, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getI64s(mMaxVals, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mMinDocids, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mMaxDocids, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, false);
	}

	// load compIIL from file
	rslt = AosIILBigI64SanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigI64::setContentsSinglePriv(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs <= mMaxBigIILSize, false);

	bool rslt = prepareMemoryForReloading();
	aos_assert_r(rslt, false);

	rslt = setDocidContents(buff);
	aos_assert_r(rslt, false);

	rslt = buff->getI64s(mValues, mNumDocs);
	aos_assert_r(rslt, false);

	return true;	
}


AosBuffPtr 
AosIILBigI64::getBodyBuffProtected() const
{
	if (isLeafIIL())
	{
		return getBodyBuffSinglePriv();
	}
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILBigI64SanityCheck(this), 0);

	i64 expect_size = mNumSubiils * 60; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	// Save mDocids
	// Save Subiil Index Data, if needed
	buff->setInt(mLevel);

	buff->setI64(mNumSubiils);

	bool rslt = buff->setI64s(mMinVals, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64s(mMaxVals, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setU64s(mMinDocids, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setU64s(mMaxDocids, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setU64s(mIILIds, mNumSubiils);
	aos_assert_r(rslt, 0);

	rslt = buff->setI64s(mNumEntries, mNumSubiils);
	aos_assert_r(rslt, 0);

	return buff;
}


AosBuffPtr 
AosIILBigI64::getBodyBuffSinglePriv() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILBigI64SanityCheck(this), 0);

	i64 expect_size = mNumDocs * 30; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	aos_assert_r(mNumDocs <= mMaxBigIILSize, 0);

	buff->setInt(mLevel);

	bool rslt = buff->setU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, 0);
	
	rslt = buff->setI64s(mValues, mNumDocs);
	aos_assert_r(rslt, 0);

	return buff;
}


void 
AosIILBigI64::countDirtyRec(
		int &parent_changed, 
		int &leaf_changed,
		const AosRundataPtr &rdata)
{
	if (mLevel == 0)
	{
		if (mIsDirty) leaf_changed++;
		return;
	}	
	
	// is parent
	if (mIsDirty) parent_changed++;
	for (i64 i = 0; i < mNumSubiils; i++)
	{
		if (mSubiils[i])
		{
			mSubiils[i]->countDirtyRec(parent_changed, leaf_changed, rdata);
		}
	}
}


bool		
AosIILBigI64::saveLeafToFileSafe(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = saveLeafToFilePriv(numToSave, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosIILBigI64::saveLeafToFilePriv(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	if (isRootIIL())
	{
		rdata->setSiteid(mSiteid);
	}
	return saveLeafToFileRecPriv(numToSave, rdata);
}


bool		
AosIILBigI64::saveLeafToFileRecSafe(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = saveLeafToFileRecPriv(numToSave, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosIILBigI64::saveLeafToFileRecPriv(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mLevel > 1)
	{
		for (i64 i = 0; i < mNumSubiils; i++)
		{
			if (mSubiils[i])
			{
				mSubiils[i]->saveLeafToFileRecSafe(numToSave, rdata);
				if (numToSave <= 0) return true;
			}
		}
		return true;
	}
	
	if (mLevel == 0)
	{
		saveToLocalFileSafe(rdata);
		return true;
	}
	
	if (mLevel == 1)
	{
		for (i64 i = 0;i < mNumSubiils;i++)
		{
			if (mSubiils[i])
			{
				if (mSubiils[i]->isDirty())
				{
					rslt = mSubiils[i]->saveToLocalFileSafe(rdata);
					aos_assert_r(rslt, false);
					numToSave --;
				}
				
				// return iil
				AosIILObjPtr iil = dynamic_cast<AosIIL *>(mSubiils[i].getPtr());
				bool rslt = AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
				aos_assert_r(rslt,false);				
				mSubiils[i] = 0;
				
				if (numToSave <= 0)
				{
					return true;
				}
			}
		}
	}
	return true;
}

