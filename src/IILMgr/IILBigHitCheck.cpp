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
#include "IILMgr/IILBigHit.h"

#include "IILMgr/IILMgr.h"


static bool sgCheckFlag = false;


bool
AosIILBigHit::checkMemory() const
{
	if (!sgCheckFlag)return true;
	// It assumes the memory was allocated by memory pool, which 
	// has some extra space. In the first round, it sets the index
	// in the extra space for each entry. It then goes to the second
	// round to see whether any element's index is altered by 
	// someone else. If yes, it means the entry was shared by 
	// one elements.
	if (!mDocBitmap) return true;

	return true;
}


bool
AosIILBigHit::sanityTestForSubiils()
{
	if (!sgCheckFlag) return true;

	return true;
}


bool
AosIILBigHit::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	static AosIILObjPtr lsTestIIL = OmnNew AosIILBigHit();
	static OmnMutex lsLock;

	lsLock.lock();
	bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
	lsLock.unlock();
	return rslt;
}


bool
AosIILBigHit::entireCheckSafe(const AosRundataPtr &rdata)
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
	//u64 value = 0;
	u64 docid = 0;
	bool rslt = false;
	if (mNumDocs <= 0) return true;

	//rslt = entireCheckRecPriv(value, docid, rdata);
	rslt = entireCheckRecPriv(docid, rdata);
	
	OmnScreen << "===============================" << endl;
	OmnScreen << "= Finish Entire Check:        =" << endl;
	OmnScreen << "= current mNumDocs: " << mNumDocs << "  =" << endl;
	OmnScreen << "= current mNumSubiils: " << mNumSubiils << "  =" << endl;
	OmnScreen << "= current mIILID: " << mIILID << "  =" << endl;
	OmnScreen << "==============================="<< endl;

	return rslt;
}


//liuwei_hit
bool
AosIILBigHit::entireCheckRecPriv(
		u64 &docid,
		const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;
	
	if (isLeafIIL())
	{
		//return entireCheckSinglePriv(value, docid, rdata);
		return entireCheckSinglePriv(docid, rdata);
	}

	bool rslt = false;
	aos_assert_r(isParentIIL(), false);
	AosIILBigHitPtr subiil;
	for(i64 i = 0; i < mNumSubiils; i++)
	{
		subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);

		//rslt = subiil->entireCheckRecPriv(value, docid, rdata);
		rslt = subiil->entireCheckRecPriv(docid, rdata);
		aos_assert_r(rslt, false);

		rslt = AosIILMgr::getSelf()->returnIILPublic(subiil, rdata);
        aos_assert_r(rslt, false);
        mSubiils[i] = 0;
	}

	return rslt;
}

bool
AosIILBigHit::entireCheckSinglePriv(
		u64 &docid,
		const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	if (mNumDocs <= 0) return true;
		
	bool rslt = false;
	u64 doc;
	mDocBitmap->reset();
/*
	if (value == 0 && docid == 0)//liuwei_hit???
	{
	
	}
	else
	{
*/
	rslt = mDocBitmap->nextDocid(doc);
	aos_assert_r(rslt, false);
	if(docid == 0)
	{
		aos_assert_r(docid>=doc, false);
	}
	else
	{
		aos_assert_r(docid>doc, false);
	}
	//	}

	mDocBitmap->reset();
	rslt = mDocBitmap->prevDocid(doc);
	aos_assert_r(rslt, false);
	docid = doc;
	
	
	return true;
}

bool
AosIILBigHit::splitSanityCheck()
{
	if (!sgCheckFlag) return true;

	return true;
}


bool
AosIILBigHit::sanityCheckPriv(const AosRundataPtr &rdata)
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
AosIILBigHit::sanityCheckRecPriv(i64 &num_docs, const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel >= 0 && mLevel <= AosIILIdx::eMaxLevelIndex, false);
	if (isLeafIIL()) return sanityCheckSinglePriv(num_docs, rdata);

	i64 nn = 0;
	i64 sum = 0;
	bool rslt;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		AosIILBigHitPtr subiil = getSubiilByIndexPriv(i, rdata);
		aos_assert_r(subiil, false);
		aos_assert_r(subiil->mIILIdx >= 0 && subiil->mIILIdx == i, false);

		rslt = subiil->sanityCheckRecPriv(nn, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(nn > 0, false);
		
		aos_assert_r(mIILIds[i], false);
		sum += nn;

		if (i != 0)
		{
			//aos_assert_r(mMinVals[i-1] <= mMinVals[i], false);
			//aos_assert_r(mMaxVals[i-1] <= mMaxVals[i], false);

			//if (mMaxVals[i-1] == mMinVals[i])
			//{
				aos_assert_r(mMaxDocids[i-1] <= mMinDocids[i], false);
			//}

		}
		
		//aos_assert_r(mMinVals[i] <= mMaxVals[i], false);

		//if (mMinVals[i] == mMaxVals[i])
		//{
		//	aos_assert_r(mMinDocids[i] <= mMaxDocids[i], false);
		//}

		aos_assert_r(mMinDocids[i] <= mMaxDocids[i], false);
	}

	aos_assert_r(sum >= 0 && mNumDocs == sum, false);
	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}

bool
AosIILBigHit::sanityCheckSinglePriv(i64 &num_docs, const AosRundataPtr &rdata)
{
	if (!sgCheckFlag) return true;

	aos_assert_r(mLevel == 0, false);
	aos_assert_r(mNumDocs >= 0, false);
	aos_assert_r(mIILID, false);

	for (i64 i=1; i<mNumDocs; i++)
	{
		
		//aos_assert_r(mValues[i-1] <= mValues[i], false);

		//if (mValues[i-1] == mValues[i])
		//{
		//	aos_assert_r(mDocids[i-1] <= mDocids[i], false);
		//}
		
		aos_assert_r(mDocids[i-1] <= mDocids[i], false);
	}

	num_docs = mNumDocs;
	//OmnScreen << "SanityCheck success: " << mIILID << ":" << mLevel << endl;
	return true;
}


