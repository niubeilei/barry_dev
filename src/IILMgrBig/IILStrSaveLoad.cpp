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
#include "IILMgrBig/IILStr.h"

#include "IILMgrBig/IILMgr.h"


bool		
AosIILStr::staticInit(const AosXmlTagPtr &theconfig)
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
AosIILStr::returnSubIILPriv(
		const int idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < (int)mNumSubiils, false);

	AosIILStrPtr subiil = mSubiils[idx];
	if (!subiil) return true;
	if (subiil->isDirty()) return true;

	bool subiilReturned = false;
	AosIILPtr iil = (AosIIL *)subiil.getPtr();                                                                                  
	bool rslt = AosIILMgr::getSelf()->returnIILPublic(iil, iilmgrLocked, subiilReturned, rdata, false);
	aos_assert_r(rslt, false);
	aos_assert_r(subiilReturned, false);
	mSubiils[idx] = 0;
	return true;
}


bool
AosIILStr::prepareMemoryForReloading(const bool iilmgrLocked)
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
		for (u32 i=mNumDocs + mExtraDocids; i<mMemCap; i++)
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
	u32 newsize = mNumDocs + mExtraDocids;
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char *) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(char *) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	return true;
}


bool
AosIILStr::returnSubIILsPriv(
		const bool iilmgrLocked,
		bool &returned,
		const AosRundataPtr &rdata,
		const bool returnHeader)
{
// only for root??? branch???
	returned = true;
	if(!isParentIIL())
	{
		return true;
	}

	AosIILStrPtr subiil = 0;
	bool rslt = false;
	bool subIILReturned = false;

	// return all sub IIL except itself
	for(u32 i = 0;i < mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil != this,false);
		
		if(subiil.notNull() && subiil->isParentIIL())
		{
			rslt = subiil->returnSubIILsPriv(iilmgrLocked, returned, rdata, returnHeader);
			aos_assert_r(rslt, false);
		}
		//ken 2011/9/2
		if(subiil.notNull() && !subiil->isDirty())
		{
			rslt = AosIILMgr::getSelf()->returnIILPublic(
				subiil, iilmgrLocked, subIILReturned, rdata, returnHeader);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}

/*
AosIILPtr	
AosIILStr::createCompIIL(const AosRundataPtr &rdata)
{
	// shawn should be root function
	return AosIILMgr::getSelf()->createIILPublic1(mIILID, mSiteid, eAosIILType_CompStr, false, false, rdata);
}


AosIILCompStrPtr 	
AosIILStr::retrieveCompIIL(
		const bool iilmgr_locked,
		const AosRundataPtr &rdata) 
{
// shawn  root function should not be called by leaf(try to be this) 
	if(isChildIIL())
	{
		aos_assert_r(mParentIIL,0);
		AosIILStrPtr parentiil = (AosIILStr*)mParentIIL.getPtr();
		return parentiil->retrieveCompIIL(iilmgr_locked, rdata);
	}
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr compiil1 = AosIILMgr::getSelf()->loadIILByIDPublic(
		mIILID, mSiteid, iiltype, true, iilmgr_locked, rdata);
	aos_assert_r(compiil1, 0);
	AosIILCompStrPtr compiil = (AosIILCompStr*)compiil1.getPtr();
	return compiil;
}


bool	
AosIILStr::returnCompIIL(const AosIILPtr &iil,const AosRundataPtr &rdata)
{
// shawn root func
	return AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
}	
*/

bool
AosIILStr::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = mNumDocs + mExtraDocids;
	if(newsize <= mMemCap)
	{
		return true;
	}
	
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char *) * newsize);

	if (mValues)
	{
		// Copy the contents from the old buffer to 'mem'
		// and release the old memory. Note that this function
		// may be called by loadFromFilePriv(buff). This function
		// will set mNumDocs based on the contents read from
		// the file. In this case, 'mNumDocs' does not really mean
		// that 'mValues[]' has that many values. For this reason,
		// we should copy only min(mNumDocs, mMemCap).
		memcpy(mem, mValues, sizeof(char *) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	aos_assert_r(checkMemory(), false);
	return true;
}

//////////////////////////



bool
AosIILStr::resetSubIILInfo(const AosRundataPtr &rdata)
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
		for(u32 i = 0;i < mNumSubiils;i++)
		{
			if(mMinVals[i])
			{
				OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			}
		}
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if(mMaxVals)
	{
		for(u32 i = 0;i < mNumSubiils;i++)
		{
			if(mMaxVals[i])
			{
				OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
			}
		}
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
		for(u32 i=0; i<mNumSubiils; i++)
		{
			if(mSubiils[i])
			{
				OmnAlarm << "subiil should be 0" << enderr;
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
AosIILStr::resetSpec()
{
	mIsNumAlpha = false;
	// Ken Lee, 2013/01/11
	if(mNumDocs != 0 && isLeafIIL())
	{
		aos_assert_r(mValues, false);
		for(u32 i = 0; i < mNumDocs;i++)
		{
			aos_assert_r(mValues[i],false);
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			mValues[i] = 0;
		}
	}
	
	OmnDelete [] mValues;
	mValues = 0;
	return true;
}


bool
AosIILStr::saveSubIILToLocalFileSafe(
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
				if(mSubiils[i]->isParentIIL())
				{
					mSubiils[i]->saveSubIILToLocalFileSafe(forcesafe,rdata);
				}
			}
		}
	}
	return true;
}


bool
AosIILStr::setContentsProtected(
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
	int isNA = buff->getInt(0);
	mIsNumAlpha = isNA == 1 ? true : false;

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
	int len;
	char *data;
	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
		for (u32 i=0; i<mNumSubiils; i++)
		{
            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && (u32)len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMinVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMinVals[i][len] = 0;

            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && (u32)len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMaxVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMaxVals[i][len] = 0;

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

	rslt =	AosIILStrSanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILStr::setContentsSinglePriv(
		AosBuffPtr &buff, 
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	int len;
	char *data;
	aos_assert_r(mNumDocs <= mMaxIILSize, false);
	int isNA = buff->getInt(0);
	mIsNumAlpha = isNA == 1 ? true : false;

	aos_assert_r(prepareMemoryForReloading(iilmgrLocked), false);
	aos_assert_r(setDocidContents(buff), false);
	for (u32 i=0; i<mNumDocs; i++)
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
AosIILStr::getBodyBuffProtected() const
{
	if(isLeafIIL())
	{
		return getBodyBuffSinglePriv();
	}
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILStrSanityCheck(this), 0);
	//felicia, 2012/10/18
	//AosBuffPtr buff = OmnNew AosBuff(5000, 5000 AosMemoryCheckerArgs);
	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	// Save mDocids
	// Save Subiil Index Data, if needed
	buff->setInt(mLevel);
	
	int isNA = mIsNumAlpha ? 1 : 0;
	buff->setInt(isNA);

	buff->setInt(mNumSubiils);
	// Save the subiil index data
	for (u32 i=0; i<mNumSubiils; i++)
	{
		if(!mMinVals[i]) 
		{
			OmnAlarm << "This is a serious problem: " << mNumSubiils << ":" << i << enderr;
		}
		if(!mMaxVals[i]) 
		{
			OmnAlarm << "This is a serious problem: " << mNumSubiils << ":" << i << enderr;
		}
		//buff->setCharStr1(mMinVals[i]);
		buff->setCharStr(mMinVals[i], strlen(mMinVals[i]));
		//buff->setCharStr1(mMaxVals[i]);
		buff->setCharStr(mMaxVals[i], strlen(mMaxVals[i]));
		buff->setU64(mMinDocids[i]);
		buff->setU64(mMaxDocids[i]);
		buff->setU64(mIILIds[i]);
		buff->setInt(mNumEntries[i]);
		aos_assert_r(mIILIds[i]>0, 0);
	}

	return buff;
}


AosBuffPtr 
AosIILStr::getBodyBuffSinglePriv() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILStrSanityCheck(this), 0);
	//felicia, 2012/10/18
	//AosBuffPtr buff = OmnNew AosBuff(5000, 5000 AosMemoryCheckerArgs);
	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	buff->setInt(mLevel);
	int isNA = mIsNumAlpha ? 1 : 0;
	buff->setInt(isNA);
// shawn block copy
	for (u32 i=0; i<mNumDocs; i++)
	{
		buff->setU64(mDocids[i]);
	}
	
	// Save Values
	aos_assert_r(mNumDocs <= mMaxIILSize, 0);
	for (u32 i=0; i<mNumDocs; i++)
	{
		if (!mValues[i] || strlen(mValues[i]) == 0)
		{
			OmnAlarm << "This is a serious problem: " << mNumDocs << ":" << i 
				<< ":" << mWordId << enderr;
		}
		else
		{
			//buff->setCharStr1(mValues[i]);
			buff->setCharStr(mValues[i], strlen(mValues[i]));
		}
	}

	return buff;
}


/*
bool
AosIILStr::saveSubIILToTransFileSafe(
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
				if(mSubiils[i]->isParentIIL())
				{
					mSubiils[i]->saveSubIILToTransFileSafe(docTrans,forcesafe,rdata);
				}
			}
		}
	}
	return true;
}
*/
