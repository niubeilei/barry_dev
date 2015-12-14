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
#include "IILMgrBig/IILU64.h"

#include "IILMgrBig/IILMgr.h"


bool		
AosIILU64::staticInit(const AosXmlTagPtr &theconfig)
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
AosIILU64::returnSubIILPriv(
		const int idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < (int)mNumSubiils, false);

	AosIILU64Ptr subiil = mSubiils[idx];
	if (subiil.isNull()) return true;
	if (subiil->isDirty()) return true;

	bool subiilReturned = false;
	AosIILPtr iil = (AosIIL *)subiil.getPtr();                                                                                  
	bool rslt = AosIILMgrSelf->returnIILPublic(
			iil, iilmgrLocked, subiilReturned, rdata, false);
	aos_assert_r(rslt, false);
	aos_assert_r(subiilReturned, false);
	mSubiils[idx] = 0;
	return true;
}


bool
AosIILU64::prepareMemoryForReloading(const bool iilmgrLocked)
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
		memset(&mValues[mNumDocs + mExtraDocids], 0, sizeof(u64)*(mMemCap - mNumDocs - mExtraDocids));

		// IMPORTANT: the memory held in 'mValues[i]' are for 
		// the previous IIL, not this IIL's!!!
		return true;
	}

	// Grow 'mValues' now
	u32 newsize = mNumDocs + mExtraDocids;
	u64* mem = OmnNew u64[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(u64) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(u64) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	return true;
}

bool
AosIILU64::returnSubIILsPriv(
		const bool iilmgrLocked,
		bool &returned,
		const AosRundataPtr &rdata,
		const bool returnHeader)
{
// only for root??? branch???
	returned = true;
	if(!isRootIIL())
	{
		return true;
	}

	AosIILU64Ptr subiil = 0;

	bool subIILReturned = false;
	bool rslt = false;

	// return all sub IIL except itself
	for(u32 i = 0;i < mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil != this,false);
		
		//ken 2011/9/2
		if(subiil.notNull() && !subiil->isDirty())
		{
			rslt = AosIILMgrSelf->returnIILPublic(subiil, iilmgrLocked, subIILReturned, rdata, returnHeader);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}


// Ketty 2013/01/15
// comp not support yet.
/*
AosIILPtr		
AosIILU64::createCompIIL(const AosRundataPtr &rdata)
{
	// shawn should be root function
	return AosIILMgr::getSelf()->createIILPublic1(mIILID, mSiteid, eAosIILType_CompU64, false, false, rdata);
}


AosIILCompU64Ptr 	
AosIILU64::retrieveCompIIL(
		const bool iilmgr_locked,
		const AosRundataPtr &rdata) 
{
// shawn  root function should not be called by leaf(try to be this) 
	if(isChildIIL())
	{
		aos_assert_r(mParentIIL,0);
		AosIILU64Ptr parentiil = (AosIILU64*)mParentIIL.getPtr();
		return parentiil->retrieveCompIIL(iilmgr_locked, rdata);
	}
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr compiil1 = AosIILMgr::getSelf()->loadIILByIDPublic(
								mIILID, 
								mSiteid,
								iiltype, 
								true, 
								iilmgr_locked, 
								rdata);
	aos_assert_r(compiil1, 0);
	AosIILCompU64Ptr compiil = (AosIILCompU64*)compiil1.getPtr();
	return compiil;
}


bool	
AosIILU64::returnCompIIL(const AosIILPtr &iil,const AosRundataPtr &rdata)
{
// shawn root func
	return AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
}	
*/

bool
AosIILU64::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = mNumDocs + mExtraDocids;
	if(newsize <= mMemCap)
	{
		return true;
	}
	
	u64* mem = OmnNew u64[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(u64) * newsize);

	if (mValues)
	{
		// Copy the contents from the old buffer to 'mem'
		// and release the old memory. Note that this function
		// may be called by loadFromFilePriv(buff). This function
		// will set mNumDocs based on the contents read from
		// the file. In this case, 'mNumDocs' does not really mean
		// that 'mValues[]' has that many values. For this reason,
		// we should copy only min(mNumDocs, mMemCap).
		memcpy(mem, mValues, sizeof(u64) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	aos_assert_r(checkMemory(), false);
	return true;
}

//////////////////////////



bool
AosIILU64::resetSubIILInfo(const AosRundataPtr &rdata)
{
	if(mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if(mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}

	if(mMinVals)
	{
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if(mMaxVals)
	{
		OmnDelete [] mMaxVals;
		mMaxVals = 0;
	}

	if(mMinDocids)
	{
		OmnDelete [] mMinDocids;
		mMinDocids = 0;
	}

	if(mMaxDocids)
	{
		OmnDelete [] mMaxDocids;
		mMaxDocids = 0;
	}

	if(mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}
	mRootIIL = 0;
	mNumSubiils = 0;
	mIILIdx = -1;
	return true;
}


bool
AosIILU64::resetSpec()
{
	if(mNumDocs == 0) return true;

	if(isLeafIIL())
	{
		aos_assert_r(mValues, false);
		memset(mValues,0,sizeof(u64)*mNumDocs);
	}
	return true;
}

bool
AosIILU64::saveSubIILToLocalFileSafe(
		const bool forcesafe, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if(isParentIIL())
	{
		// this iil is the root iil of the list
		for(u32 i = 0;i < mNumSubiils;i++)
		{
			if(mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToLocalFileSafe(forcesafe, rdata);
				aos_assert_r(rslt,false);
			}
		}
	}
	return true;
}

bool
AosIILU64::setContentsProtected(
		AosBuffPtr &buff, 
		const bool iilmgrLocked,
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

	if(isLeafIIL())
	{
		return setContentsSinglePriv(buff, iilmgrLocked, rdata);
	}	

	bool rslt = false;
//	int isNA = buff->getInt(0);
	buff->getInt(0);// isNA

	// 2. Retrieve Subiil Index Data, if needed
	mNumSubiils = buff->getInt(-1);
	//aos_assert_r(mNumSubiils >= 0, false);
	if(mNumSubiils <= 1 || mNumSubiils > mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	//now this place is used to be isNumAlpha flag;
	// int len;			// Chen Ding, 05/22/2012, not used any more
	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
		for (u32 i=0; i<mNumSubiils; i++)
		{
			// Chen Ding, 05/22/2012
			// mMinVals[i] = buff->getU64(len);
			mMinVals[i] = buff->getU64(0);

			// Chen Ding, 05/22/2012
			// mMaxVals[i] = buff->getU64(len);
			mMaxVals[i] = buff->getU64(0);

            mMinDocids[i] = buff->getU64(0);
            mMaxDocids[i] = buff->getU64(0);

            mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			mNumEntries[i] = buff->getInt(-1);
			aos_assert_r(mNumEntries[i] > 0, false);
		}
	}

	// load compIIL from file
//	u64 compIILID = buff->getU64(AOS_INVIILID);

	rslt =	AosIILU64SanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILU64::setContentsSinglePriv(
		AosBuffPtr &buff, 
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs <= mMaxIILSize, false);
	buff->getInt(0);// isNA

	aos_assert_r(prepareMemoryForReloading(iilmgrLocked), false);
	aos_assert_r(setDocidContents(buff), false);
	for (u32 i=0; i<mNumDocs; i++)
	{
		mValues[i] = buff->getU64(0);
	}
	return true;	
}


AosBuffPtr 
AosIILU64::getBodyBuffProtected() const
{
	if(isLeafIIL())
	{
		return getBodyBuffSinglePriv();
	}
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILU64SanityCheck(this), 0);
	//felicia, 2012/10/18
	//AosBuffPtr buff = OmnNew AosBuff(5000, 5000 AosMemoryCheckerArgs);
	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	// Save mDocids
	// Save Subiil Index Data, if needed
	buff->setInt(mLevel);
	
	buff->setInt(0);//isNA

	buff->setInt(mNumSubiils);
	// Save the subiil index data
	for (u32 i=0; i<mNumSubiils; i++)
	{
		buff->setU64(mMinVals[i]);
		buff->setU64(mMaxVals[i]);
		buff->setU64(mMinDocids[i]);
		buff->setU64(mMaxDocids[i]);
		buff->setU64(mIILIds[i]);
		buff->setInt(mNumEntries[i]);
		aos_assert_r(mIILIds[i]>0, 0);
	}

	return buff;
}

AosBuffPtr 
AosIILU64::getBodyBuffSinglePriv() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILU64SanityCheck(this), 0);
	//felicia,2012/10/18
	//AosBuffPtr buff = OmnNew AosBuff(5000, 5000 AosMemoryCheckerArgs);
	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	buff->setInt(mLevel);
	buff->setInt(0);//isNA
// shawn block copy
	for (u32 i=0; i<mNumDocs; i++)
	{
		buff->setU64(mDocids[i]);
	}
	
	// Save Values
	aos_assert_r(mNumDocs <= mMaxIILSize, 0);
	for (u32 i=0; i<mNumDocs; i++)
	{
		buff->setU64(mValues[i]);
	}

	return buff;
}


/*
bool
AosIILU64::saveSubIILToTransFileSafe(
		const AosDocTransPtr &docTrans, 
		const bool forcesafe, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if(isParentIIL())
	{
		// this iil is the root iil of the list
		for(u32 i = 0;i < mNumSubiils;i++)
		{
			if(mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToTransFileSafe(docTrans, forcesafe, rdata);
				aos_assert_r(rslt,false);
			}
		}
	}
	return true;
}
*/
