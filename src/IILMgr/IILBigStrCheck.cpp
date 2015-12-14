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


static bool sgCheckFlag = false;


bool
AosIILBigStr::checkMemory() const
{
	if (!sgCheckFlag)return true;
	// It assumes the memory was allocated by memory pool, which 
	// has some extra space. In the first round, it sets the index
	// in the extra space for each entry. It then goes to the second
	// round to see whether any element's index is altered by 
	// someone else. If yes, it means the entry was shared by 
	// one elements.
	if (!mValues) return true;
	for (i64 i=0; i<mNumDocs; i++)
	{
		aos_assert_r(mValues[i], false);
		char *ptr = &mValues[i][-4];
		int *size = (int*)ptr;
		u32 *idx = (u32*)&ptr[*size+AosMemPool::eUserDataOffset];
		*idx = i;
	}

	for (i64 i=0; i<mNumDocs; i++)
	{
		char *ptr = &mValues[i][-4];
		int *size = (int*)ptr;
		u32 *idx = (u32*)&ptr[*size+AosMemPool::eUserDataOffset];
		aos_assert_r(*idx == i, false);
	}
	return true;
}


bool
AosIILBigStr::sanityTestForSubiils()
{
	if (!sgCheckFlag) return true;

	return true;
}


bool
AosIILBigStr::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	static AosIILObjPtr lsTestIIL = OmnNew AosIILBigStr();
	static OmnMutex lsLock;

	lsLock.lock();
	bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
	lsLock.unlock();
	return rslt;
}


bool
AosIILBigStr::entireCheckSafe(const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;
	
	OmnScreen << "===============================" << endl;
	OmnScreen << "= Entire Check:               =" << endl;
	OmnScreen << "= current mNumDocs: " << mNumDocs << "  =" << endl;
	OmnScreen << "= current mNumSubiils: " << mNumSubiils << "  =" << endl;
	OmnScreen << "= current mIILID: " << mIILID << "  =" << endl;
	OmnScreen << "==============================="<< endl;
	//1. return all the subiils, assume that all the iils is notdirty
	//2. get entries one by one to check
	OmnString value = "";
	u64 docid = 0;
	bool rslt = false;
	if (mNumDocs <= 0) return true;
	
	rslt = entireCheckRecPriv(value, docid, rdata);
	
	OmnScreen << "===============================" << endl;
	OmnScreen << "= Finish Entire Check:        =" << endl;
	OmnScreen << "= current mNumDocs: " << mNumDocs << "  =" << endl;
	OmnScreen << "= current mNumSubiils: " << mNumSubiils << "  =" << endl;
	OmnScreen << "= current mIILID: " << mIILID << "  =" << endl;
	OmnScreen << "==============================="<< endl;

	return rslt;
}


bool
AosIILBigStr::entireCheckRecPriv(
		OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;
	
	if (isLeafIIL())
	{
		return entireCheckSinglePriv(value, docid, rdata);
	}

	bool rslt = false;
	aos_assert_r(isParentIIL(), false);
	AosIILBigStrPtr subiil;
	for(i64 i = 0; i < mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);

		rslt = subiil->entireCheckRecPriv(value, docid, rdata);
		aos_assert_r(rslt, false);

		rslt = AosIILMgr::getSelf()->returnIILPublic(subiil, rdata);
        aos_assert_r(rslt, false);
        mSubiils[i] = 0;
	}

	return rslt;
}


bool
AosIILBigStr::entireCheckSinglePriv(
		OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	if (mNumDocs <= 0) return true;
	bool rslt = false;
	u64 cmp_len = rdata->getCmpLen();
	if (value == "" && docid == 0)
	{
	
	}
	else
	{
		rslt = AosIILUtil::valueNMatch(mValues[0], mDocids[0], eAosOpr_ge, value, docid, mIsNumAlpha,cmp_len);
		aos_assert_r(rslt, false);						
	}
			
	for(i64 i = 1;i < mNumDocs;i++)
	{
		rslt = AosIILUtil::valueNMatch(mValues[i], mDocids[i], eAosOpr_ge, mValues[i-1], mDocids[i-1], mIsNumAlpha,cmp_len);
		aos_assert_r(rslt,false);						
	}

	value = mValues[mNumDocs-1];
	docid = mDocids[mNumDocs-1];
	return true;
}

bool
AosIILBigStr::splitSanityCheck()
{
	if (!sgCheckFlag) return true;

	return true;
}


bool
AosIILBigStr::sanityCheckPriv(const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel >= 0 && mLevel <= AosIILIdx::eMaxLevelIndex, false);
	i64 num_docs = 0;
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
	aos_assert_r(num_docs >= 0 && num_docs == mNumDocs, false);
	return true;
}


bool
AosIILBigStr::sanityCheckRecPriv(i64 &num_docs, const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel >= 0 && mLevel <= AosIILIdx::eMaxLevelIndex, false);
	if (isLeafIIL()) return sanityCheckSinglePriv(num_docs, rdata);

	i64 nn = 0;
	i64 sum = 0;
	bool rslt;
	u64 cmp_len = rdata->getCmpLen();
	for (i64 i=0; i<mNumSubiils; i++)
	{
		AosIILBigStrPtr subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);
		aos_assert_r(subiil->mIILIdx >= 0 && subiil->mIILIdx == i, false);

		rslt = subiil->sanityCheckRecPriv(nn, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(nn > 0, false);

		sum += nn;
		if (i != 0)
		{
			aos_assert_r(AosIILUtil::valueNMatch(mMinVals[i-1], eAosOpr_le, mMinVals[i], "", mIsNumAlpha,cmp_len), false);
			aos_assert_r(AosIILUtil::valueNMatch(mMaxVals[i-1], eAosOpr_le, mMaxVals[i], "", mIsNumAlpha,cmp_len), false);

			rslt = AosIILUtil::valueNMatch(mMaxVals[i-1], eAosOpr_eq, mMinVals[i], "", mIsNumAlpha,cmp_len);
			if (rslt)
			{
				aos_assert_r(mMaxDocids[i-1] <= mMinDocids[i], false);
			}

		}
		aos_assert_r(mIILIds[i], false);
		aos_assert_r(AosIILUtil::valueNMatch(mMinVals[i], eAosOpr_le, mMaxVals[i], "", mIsNumAlpha,cmp_len), false);

		rslt = AosIILUtil::valueNMatch(mMinVals[i], eAosOpr_eq, mMaxVals[i], "", mIsNumAlpha,cmp_len);
		if (rslt)
		{
			aos_assert_r(mMinDocids[i] <= mMaxDocids[i], false);
		}
	}

	aos_assert_r(sum >= 0 && mNumDocs == sum, false);
	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}


bool
AosIILBigStr::sanityCheckSinglePriv(i64 &num_docs, const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel == 0, false);
	aos_assert_r(mNumDocs >= 0, false);
	aos_assert_r(mIILID, false);

	int cmp_rslt;	
	u64 cmp_len = rdata->getCmpLen();
	for (i64 i=1; i<mNumDocs; i++)
	{
		cmp_rslt = AosIILUtil::valueNMatch(mValues[i-1], mValues[i], mIsNumAlpha,cmp_len);
		aos_assert_r(cmp_rslt <= 0, false);

		if (cmp_rslt == 0)
		{
			aos_assert_r(mDocids[i-1] <= mDocids[i], false);
		}
	}

	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}


