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
#include "IILMgr/IILBigStr.h"

#include "IILMgr/IILMgr.h"


bool		
AosIILBigStr::staticInit(const AosXmlTagPtr &theconfig)
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
AosIILBigStr::returnSubIILPriv(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	AosIILBigStrPtr subiil = mSubiils[idx];
	if (!subiil) return true;
	if (subiil->isDirty()) return true;

	AosIILObjPtr iil = dynamic_cast<AosIIL *>(subiil.getPtr());
	bool rslt = AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	mSubiils[idx] = 0;
	return true;
}


bool
AosIILBigStr::prepareMemoryForReloading()
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
		for (i64 i=mNumDocs + mExtraDocids; i<mMemCap; i++)
		{
			if (!mValues[i]) break;
			
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			mValues[i] = 0;
		}

		// IMPORTANT: the memory held in 'mValues[i]' are for 
		// the previous IIL, not this IIL's!!!
		return true;
	}

	// Grow 'mValues' now
	i64 newsize = mNumDocs + mExtraDocids;
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char*) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(char *) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;

	bool rslt = expandDocidMem();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigStr::returnSubIILsPriv(
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

	AosIILBigStrPtr subiil = 0;
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


/*
AosIILObjPtr
AosIILBigStr::createCompIIL(const AosRundataPtr &rdata)
{
	// shawn should be root function
	return AosIILMgr::getSelf()->createIILPublic1(mIILID, mSiteid, eAosIILType_CompStr, false, false, rdata);
}


AosIILCompStrPtr
AosIILBigStr::retrieveCompIIL(
		const bool iilmgr_locked,
		const AosRundataPtr &rdata) 
{
// shawn  root function should not be called by leaf(try to be this) 
	if (isChildIIL())
	{
		aos_assert_r(mParentIIL,0);
		AosIILBigStrPtr parentiil = (AosIILBigStr*)mParentIIL.getPtr();
		return parentiil->retrieveCompIIL(iilmgr_locked, rdata);
	}
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILObjPtr compiil1 = AosIILMgr::getSelf()->loadIILByIDPublic(
		mIILID, mSiteid, iiltype, true, iilmgr_locked, rdata);
	aos_assert_r(compiil1, 0);
	AosIILCompStrPtr compiil = (AosIILCompStr*)compiil1.getPtr();
	return compiil;
}


bool	
AosIILBigStr::returnCompIIL(const AosIILObjPtr &iil,const AosRundataPtr &rdata)
{
// shawn root func
	return AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
}	
*/

bool
AosIILBigStr::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	i64 newsize = mNumDocs + mExtraDocids;
	if (newsize <= mMemCap)
	{
		return true;
	}
	
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char*) * newsize);

	if (mValues)
	{
		// Copy the contents from the old buffer to 'mem'
		// and release the old memory. Note that this function
		// may be called by loadFromFilePriv(buff). This function
		// will set mNumDocs based on the contents read from
		// the file. In this case, 'mNumDocs' does not really mean
		// that 'mValues[]' has that many values. For this reason,
		// we should copy only min(mNumDocs, mMemCap).
		memcpy(mem, mValues, sizeof(char*) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	aos_assert_r(checkMemory(), false);
	return true;
}

//////////////////////////



bool
AosIILBigStr::resetSubIILInfo(const AosRundataPtr &rdata)
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
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mMinVals[i])
			{
				OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			}
		}
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if (mMaxVals)
	{
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mMaxVals[i])
			{
				OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
			}
		}
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
		for(i64 i=0; i<mNumSubiils; i++)
		{
			if (mSubiils[i])
			{
				OmnScreen << "subiil should be 0" << endl;
				mSubiils[i] = 0;
			}
		}
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
AosIILBigStr::resetSpec()
{
	mIsNumAlpha = false;
	// Ken Lee, 2013/01/11
	if (mNumDocs != 0 && isLeafIIL())
	{
		aos_assert_r(mValues, false);
		for(i64 i=0; i<mNumDocs; i++)
		{
			aos_assert_r(mValues[i], false);
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			// mValues[i] = 0;
		}
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
AosIILBigStr::saveSubIILToLocalFileSafe(const AosRundataPtr &rdata)
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
AosIILBigStr::setContentsProtected(
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

	mIsNumAlpha = buff->getU8(0);

	// 2. Retrieve Subiil Index Data, if needed
	mNumSubiils = buff->getI64(0);
	if (mNumSubiils <= 1 || mNumSubiils > mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	//now this place is used to be isNumAlpha flag;
	int len;
	char *data;
	bool rslt;
	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);

		// Ken Lee, 2013/04/18
		/*for (i64 i=0; i<mNumSubiils; i++)
		{
            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMinVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMinVals[i][len] = 0;

            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMaxVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMaxVals[i][len] = 0;

            //mMinDocids[i] = buff->getU64(0);
            //mMaxDocids[i] = buff->getU64(0);

            //mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			//mNumEntries[i] = buff->getI64(0);
			aos_assert_r(mNumEntries[i] > 0, false);
		}*/

		for (i64 i=0; i<mNumSubiils; i++)
		{
            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
			aos_assert_r(AosCopyMemStr(&mMinVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMinVals[i][len] = 0;

            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMaxVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMaxVals[i][len] = 0;
		}

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
//	u64 compIILID = buff->getU64(AOS_INVIILID);

	rslt = AosIILBigStrSanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILBigStr::setContentsSinglePriv(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs <= mMaxBigIILSize, false);
	int len;
	char *data;

	mIsNumAlpha = buff->getU8(0);

	bool rslt = prepareMemoryForReloading();
	aos_assert_r(rslt, false);

	rslt = setDocidContents(buff);
	aos_assert_r(rslt, false);

	for (i64 i=0; i<mNumDocs; i++)
	{
		data = buff->getCharStr(len);
		if (len <= 0 || !data)
		//if (len < 0 || !data)
		{
			// u32 len;
			// char *ww = AosWordClient::getSelf()->getWord(mWordId, len);
			// OmnString word(ww, len);
			OmnAlarm << "Failed to retrieve the data. Data possibly corrupted: " << enderr;
			
			// This means it failed parsing the retrieved data. 
			// Need to set 'mNumDocs' to whatever has been parsed.

			// Chen Ding, 07/31/2012
			setNumDocsNotSafe(0);
			//setValue(i, 0, 0);
		}
		else if (!setValue(i, data, len))
		{
			// u32 len;
			// char *ww = AosWordClient::getSelf()->getWord(mWordId, len);
			// OmnString word(ww, len);
			OmnAlarm << "Failed to set dta " << i << ":" << enderr;
			setNumDocsNotSafe(0);
			return false;
		}
	}

	return true;	
}


AosBuffPtr 
AosIILBigStr::getBodyBuffProtected() const
{
	if (isLeafIIL())
	{
		return getBodyBuffSinglePriv();
	}
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILBigStrSanityCheck(this), 0);

	i64 expect_size = mNumSubiils * 120; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	// Save mDocids
	// Save Subiil Index Data, if needed
	buff->setInt(mLevel);
	buff->setU8(mIsNumAlpha);
	buff->setI64(mNumSubiils);

	// Save the subiil index data
	// Ken Lee, 2013/04/18
	/*for (i64 i=0; i<mNumSubiils; i++)
	{
		if (!mMinVals[i]) 
		{
			OmnAlarm << "This is a serious problem: " << mNumSubiils << ":" << i << enderr;
		}
		if (!mMaxVals[i]) 
		{
			OmnAlarm << "This is a serious problem: " << mNumSubiils << ":" << i << enderr;
		}
		//buff->setCharStr1(mMinVals[i]);
		buff->setCharStr(mMinVals[i], strlen(mMinVals[i]));
		//buff->setCharStr1(mMaxVals[i]);
		buff->setCharStr(mMaxVals[i], strlen(mMaxVals[i]));
		//buff->setU64(mMinDocids[i]);
		//buff->setU64(mMaxDocids[i]);
		//buff->setU64(mIILIds[i]);
		//buff->setI64(mNumEntries[i]);
		aos_assert_r(mIILIds[i]>0, 0);
	}*/

	for (i64 i=0; i<mNumSubiils; i++)
	{
		buff->setCharStr(mMinVals[i], strlen(mMinVals[i]));
		buff->setCharStr(mMaxVals[i], strlen(mMaxVals[i]));
	}

	bool rslt = buff->setU64s(mMinDocids, mNumSubiils);
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
AosIILBigStr::getBodyBuffSinglePriv() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILBigStrSanityCheck(this), 0);

	i64 expect_size = mNumDocs * 60; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	aos_assert_r(mNumDocs <= mMaxBigIILSize, 0);

	buff->setInt(mLevel);
	buff->setU8(mIsNumAlpha);

// shawn block copy
	// Ken Lee, 2013/04/18
	//for (i64 i=0; i<mNumDocs; i++)
	//{
	//	buff->setU64(mDocids[i]);
	//}
	bool rslt = buff->setU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, 0);
	
	// Save Values
	for (i64 i=0; i<mNumDocs; i++)
	{
		if (!mValues[i] || strlen(mValues[i]) == 0)
		{
			OmnAlarm << "This is a serious problem: " << mNumDocs << ":" << i 
				<< ":" << mWordId << enderr;
		}
		else
		{
			buff->setCharStr(mValues[i], strlen(mValues[i]));
		}
	}

	return buff;
}


void 
AosIILBigStr::countDirtyRec(
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
AosIILBigStr::saveLeafToFileSafe(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = saveLeafToFilePriv(numToSave, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosIILBigStr::saveLeafToFilePriv(
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
AosIILBigStr::saveLeafToFileRecSafe(
		int &numToSave,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = saveLeafToFileRecPriv(numToSave, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosIILBigStr::saveLeafToFileRecPriv(
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

