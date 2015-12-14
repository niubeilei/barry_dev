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
#include "IILMgr/IILBigU64.h"

#include "IILMgr/IILMgr.h"


static bool sgCheckFlag = false;


bool
AosIILBigU64::checkMemory() const
{
	if (!sgCheckFlag)return true;
	// It assumes the memory was allocated by memory pool, which 
	// has some extra space. In the first round, it sets the index
	// in the extra space for each entry. It then goes to the second
	// round to see whether any element's index is altered by 
	// someone else. If yes, it means the entry was shared by 
	// one elements.
	if (!mValues) return true;

	return true;
}


bool
AosIILBigU64::sanityTestForSubiils()
{
	if (!sgCheckFlag) return true;

	return true;
}


bool
AosIILBigU64::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	static AosIILObjPtr lsTestIIL = OmnNew AosIILBigU64();
	static OmnMutex lsLock;

	lsLock.lock();
	bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
	lsLock.unlock();
	return rslt;
}


bool
AosIILBigU64::entireCheckSafe(const AosRundataPtr &rdata)
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
	u64 value = 0;
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
AosIILBigU64::entireCheckRecPriv(
		u64 &value,
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
	AosIILBigU64Ptr subiil;
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
AosIILBigU64::entireCheckSinglePriv(
		u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	if (mNumDocs <= 0) return true;
	bool rslt = false;
	
	if (value == 0 && docid == 0)
	{
	
	}
	else
	{
		rslt = AosIILUtil::valueMatch(mValues[0], mDocids[0], eAosOpr_ge, value, docid);
		aos_assert_r(rslt, false);						
	}
			
	for(i64 i = 1;i < mNumDocs;i++)
	{
		rslt = AosIILUtil::valueMatch(mValues[i], mDocids[i], eAosOpr_ge, mValues[i-1], mDocids[i-1]);
		aos_assert_r(rslt,false);						
	}

	value = mValues[mNumDocs-1];
	docid = mDocids[mNumDocs-1];
	return true;
}

bool
AosIILBigU64::splitSanityCheck()
{
	if (!sgCheckFlag) return true;

	return true;
}


bool
AosIILBigU64::sanityCheckPriv(const AosRundataPtr &rdata)
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
AosIILBigU64::sanityCheckRecPriv(i64 &num_docs, const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel >= 0 && mLevel <= AosIILIdx::eMaxLevelIndex, false);
	if (isLeafIIL()) return sanityCheckSinglePriv(num_docs, rdata);

	i64 nn = 0;
	i64 sum = 0;
	bool rslt;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		AosIILBigU64Ptr subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);
		aos_assert_r(subiil->mIILIdx >= 0 && subiil->mIILIdx == i, false);

		rslt = subiil->sanityCheckRecPriv(nn, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(nn > 0, false);

		sum += nn;
		if (i != 0)
		{
			aos_assert_r(mMinVals[i-1] <= mMinVals[i], false);
			aos_assert_r(mMaxVals[i-1] <= mMaxVals[i], false);

			if (mMaxVals[i-1] == mMinVals[i])
			{
				aos_assert_r(mMaxDocids[i-1] <= mMinDocids[i], false);
			}

		}
		aos_assert_r(mIILIds[i], false);
		aos_assert_r(mMinVals[i] <= mMaxVals[i], false);

		if (mMinVals[i] == mMaxVals[i])
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
AosIILBigU64::sanityCheckSinglePriv(i64 &num_docs, const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel == 0, false);
	aos_assert_r(mNumDocs >= 0, false);
	aos_assert_r(mIILID, false);

	for (i64 i=1; i<mNumDocs; i++)
	{
		aos_assert_r(mValues[i-1] <= mValues[i], false);

		if (mValues[i-1] == mValues[i])
		{
			aos_assert_r(mDocids[i-1] <= mDocids[i], false);
		}
	}

	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}


