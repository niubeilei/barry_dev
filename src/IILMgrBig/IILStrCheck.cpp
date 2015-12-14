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
#include "IILMgrBig/IILStr.h"

#include "IILMgrBig/IILMgr.h"


static bool sgCheckFlag = false;


bool
AosIILStr::checkMemory() const
{
	if (!sgCheckFlag)return true;
	// It assumes the memory was allocated by memory pool, which 
	// has some extra space. In the first round, it sets the index
	// in the extra space for each entry. It then goes to the second
	// round to see whether any element's index is altered by 
	// someone else. If yes, it means the entry was shared by 
	// one elements.
	if (!mValues) return true;
	for (u32 i=0; i<mNumDocs; i++)
	{
		aos_assert_r(mValues[i], false);
		char *ptr = &mValues[i][-4];
		int *size = (int*)ptr;
		u32 *idx = (u32*)&ptr[*size+AosMemPool::eUserDataOffset];
		*idx = i;
	}

	for (u32 i=0; i<mNumDocs; i++)
	{
		char *ptr = &mValues[i][-4];
		int *size = (int*)ptr;
		u32 *idx = (u32*)&ptr[*size+AosMemPool::eUserDataOffset];
		aos_assert_r(*idx == i, false);
	}
	return true;
}


bool
AosIILStr::sanityTestForSubiils()
{
	if(!sgCheckFlag) return true;

	return true;
}


bool
AosIILStr::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;

	static AosIILPtr lsTestIIL = OmnNew AosIILStr(false, true, rdata);
	static OmnMutex lsLock;

	lsLock.lock();
	bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
	lsLock.unlock();
	return rslt;
}


bool
AosIILStr::entireCheckSafe(const bool iilmgrLocked, const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;
	
	OmnScreen << "===============================" << endl;
	OmnScreen << "= Entire Check:               =" << endl;
	OmnScreen << "= current mNumDocs: " << mNumDocs << "  =" << endl;
	OmnScreen << "= current mNumSubiils: " << mNumSubiils<< "  =" << endl;
	OmnScreen << "= current mIILID: " << mIILID << "  =" << endl;
	OmnScreen << "==============================="<< endl;
	//1. return all the subiils, assume that all the iils is notdirty
	//2. get entries one by one to check
	OmnString value = "";
	u64 docid = 0;
	bool rslt = false;
	if(mNumDocs <= 0) return true;
	
	rslt = entireCheckRecPriv(value,docid,iilmgrLocked,rdata);
	
	OmnScreen << "===============================" << endl;
	OmnScreen << "= Finish Entire Check:        =" << endl;
	OmnScreen << "= current mNumDocs: " << mNumDocs << "  =" << endl;
	OmnScreen << "= current mNumSubiils: " << mNumSubiils<< "  =" << endl;
	OmnScreen << "= current mIILID: " << mIILID << "  =" << endl;
	OmnScreen << "==============================="<< endl;

	return rslt;
}


bool
AosIILStr::entireCheckRecPriv(
		OmnString &value,
		u64 &docid,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;
	
	if(isLeafIIL())
	{
		return entireCheckSinglePriv(value, docid, iilmgrLocked, rdata);
	}

	bool rslt = false;
	bool subIILReturned = false;
	aos_assert_r(isParentIIL(),false);
	AosIILStrPtr subiil;
	bool returnHeader = false;
	for(int i = 0; i < (int)mNumSubiils;i++)
	{
		subiil = getSubiilByIndexPriv(i, false, rdata);
		rslt = subiil->entireCheckRecPriv(value, docid, iilmgrLocked, rdata);
		aos_assert_r(rslt,false);
		rslt = AosIILMgr::getSelf()->returnIILPublic(subiil, iilmgrLocked, subIILReturned, rdata, returnHeader);
        aos_assert_r(rslt, false);
        aos_assert_r(subIILReturned, false);
        mSubiils[i] = 0;
	}

	return rslt;
}


bool
AosIILStr::entireCheckSinglePriv(
		OmnString &value,
		u64 &docid,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;

	if(mNumDocs <= 0) return true;
	bool rslt = false;
	
	if(value == "" && docid == 0)
	{
	
	}
	else
	{
		rslt = AosIILUtil::valueMatch(mValues[0], mDocids[0], eAosOpr_ge, value, docid, mIsNumAlpha);
		aos_assert_r(rslt, false);						
	}
			
	for(int i = 1;i < (int)mNumDocs;i++)
	{
		rslt = AosIILUtil::valueMatch(mValues[i], mDocids[i], eAosOpr_ge, mValues[i-1], mDocids[i-1], mIsNumAlpha);
		aos_assert_r(rslt,false);						
	}

	value = mValues[mNumDocs-1];
	docid = mDocids[mNumDocs-1];
	return true;
}

bool
AosIILStr::splitSanityCheck()
{
	if(!sgCheckFlag) return true;

	return true;
}


bool
AosIILStr::sanityCheckPriv(const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;

	aos_assert_r(mLevel >= 0 && mLevel <= AosIILIdx::eMaxLevelIndex, false);
	int num_docs;
	bool rslt = true;
	if (isLeafIIL())
	{
		rslt = sanityCheckSinglePriv(num_docs, rdata);
	}
	else
	{
		rslt = sanityCheckRecPriv(num_docs, rdata);
	}
	aos_assert_r(rslt, false);
	aos_assert_r(num_docs >= 0 && (u32)num_docs == mNumDocs, false);
	return true;
}


bool
AosIILStr::sanityCheckRecPriv(int &num_docs, const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;

	aos_assert_r(mLevel >= 0 && mLevel <= AosIILIdx::eMaxLevelIndex, false);
	if (isLeafIIL()) return sanityCheckSinglePriv(num_docs, rdata);

	int nn;
	int sum = 0;
	bool rslt;
	for (u32 i=0; i<mNumSubiils; i++)
	{
		AosIILStrPtr subiil = getSubiilByIndexPriv(i, false, rdata);
		aos_assert_r(subiil, false);
		aos_assert_r(subiil->mIILIdx >= 0 && (u32)subiil->mIILIdx == i, false);

		rslt = subiil->sanityCheckRecPriv(nn, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(nn > 0, false);

		sum += nn;
		if (i != 0)
		{
			aos_assert_r(AosIILUtil::valueMatch(mMinVals[i-1], eAosOpr_le, mMinVals[i], mIsNumAlpha), false);
			aos_assert_r(AosIILUtil::valueMatch(mMaxVals[i-1], eAosOpr_le, mMaxVals[i], mIsNumAlpha), false);

			rslt = AosIILUtil::valueMatch(mMaxVals[i-1], eAosOpr_eq, mMinVals[i], mIsNumAlpha);
			if (rslt)
			{
				aos_assert_r(mMaxDocids[i-1] <= mMinDocids[i], false);
			}

		}
		aos_assert_r(mIILIds[i], false);
		aos_assert_r(AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, mMaxVals[i], mIsNumAlpha), false);

		rslt = AosIILUtil::valueMatch(mMinVals[i], eAosOpr_eq, mMaxVals[i], mIsNumAlpha);
		if (rslt)
		{
			aos_assert_r(mMinDocids[i] <= mMaxDocids[i], false);
		}
	}

	aos_assert_r(sum >= 0 && mNumDocs == (u32)sum, false);
	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}


bool
AosIILStr::sanityCheckSinglePriv(int &num_docs, const AosRundataPtr &rdata)
{
	if(!sgCheckFlag) return true;

	aos_assert_r(mLevel == 0, false);
	aos_assert_r(mNumDocs >= 0, false);
	aos_assert_r(mIILID, false);

	int rslt;	
	for (u32 i=1; i<mNumDocs; i++)
	{
		rslt = AosIILUtil::valueMatch(mValues[i-1], mValues[i], mIsNumAlpha);
		aos_assert_r(rslt <= 0, false);
		
		if (rslt == 0)
		{
			aos_assert_r(mDocids[i-1] <= mDocids[i], false);
		}
	}

	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}


