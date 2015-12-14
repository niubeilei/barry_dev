////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This is the normal IIL. It is a list of docids. The list may be 
// sorted based on docids or usage.
//
// Chen Ding, 06/05/201
// In the current implementations, IILHit does not maintain two lists
// but just one through mCompIIL (it used to maintain two). 
//
// Modification History:
// 	Created: 12/11/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILHit.h"

#include "DfmUtil/DfmDocIIL.h"
#include "IILMgr/IILMgr.h"
#include "IILUtil/IILSave.h"
#include "IILUtil/IILLog.h"

static u32 sgMaxIILLen = 0;

#define AosIILHitSanityCheck(x) true
// #define AosIILHitSanityCheck(x) (x)->sanityCheck()

AosIILHit::AosIILHit()
:
AosIIL(eAosIILType_Hit),
mSubiils(0),
mMinVals(0),
mMaxVals(0)
{
	mTotalIILs[eAosIILType_Hit]++;
}


AosIILHit::AosIILHit(
	const u64 &wordid,
	const u64 &iilid,
	const bool isPersis,
	const OmnString &iilname, 
	const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_Hit, isPersis, iilname),
mSubiils(0),
mMinVals(0),
mMaxVals(0)
{
	mTotalIILs[eAosIILType_Hit] ++;
}


AosIILHit::AosIILHit(
		const u64 &iilid, 
		const u32 siteid,
		const AosDfmDocIILPtr &doc, 		//Ketty 2012/11/15
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_Hit),
mSubiils(0),
mMinVals(0),
mMaxVals(0)
{
	mTotalIILs[eAosIILType_Hit] ++;
	mIILID = iilid;
	mDfmDoc = doc;		// Ketty 2013/01/15
	//mIsGood = loadFromFilePriv(buff, siteid, rdata);
	//mIsGood = loadFromFilePriv(doc, siteid, rdata);		// Ketty 2012/11/15
	mIsGood = loadFromFilePriv(iilid, rdata);		// Ketty 2012/11/15
}


AosIILHit::~AosIILHit()
{
	AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_Hit] --;
	if (mNumSubiils > 0)
	{
		for (i64 i=0; i<mNumSubiils; i++)
		{
			if (mSubiils[i]) AosIILMgrSelf->returnIILPublic(mSubiils[i].getPtr(), rdata);
			mSubiils[i] = 0;
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

		aos_assert(!mSubiils);
		if (mSubiils)
		{
			OmnDelete [] mSubiils;
			mSubiils = 0;
		}
	}

	mNumSubiils = 0;
	mMinVals = 0;
	mMaxVals = 0;
	mIILIds = 0;
	mSubiils = 0;
	mNumEntries = 0;

}


void    
AosIILHit::exportIILPriv(
		vector<u64>	&docids,
		const AosRundataPtr &rdata)
{
	docids.clear();
	if (isParentIIL())
	{
		AosIILHitPtr subiil;
		exportIILSinglePriv(docids, rdata);
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			subiil->exportIILSingleSafe(docids, rdata);
		}
		return;
	}
	return exportIILSinglePriv(docids,rdata);
}


void    
AosIILHit::exportIILSinglePriv(
		vector<u64>	&docids,
		const AosRundataPtr &rdata)
{
	for(i64 i=0; i<mNumDocs; i++)
	{
		docids.push_back(mDocids[i]);
	}
}


AosIILHitPtr
AosIILHit::getSubiilByIndex(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	// It retrieves the idx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil.
	//
	// The function assumes 'mLock' should have been locked.
	aos_assert_r(mIILIdx==0, 0);
	aos_assert_r(idx>=0 && idx<mNumSubiils, 0);
	if (mRootIIL)
	{
		aos_assert_r(mRootIIL.getPtr() == this, 0);
	}

	if (idx == 0)
	{
		AosIILHitPtr thisPtr(this, false);
		if (!mRootIIL) mRootIIL = thisPtr;
		return thisPtr;
	}

	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILHitPtr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[idx]) return mSubiils[idx];

	// Retrieve an IIL from IILMgr
	aos_assert_r(mIILIds[idx], 0);

	// Chen Ding, 05/05/2011
	// bool status;
	// AosIILObjPtr iil = AosIILMgrSelf->loadIILByIDSafe(mIILIds[idx],
	// 		AOS_INVWID, eAosIILType_Hit, status);
	AosIILType type = eAosIILType_Hit;
	AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
		mIILIds[idx], mSiteid, mSnapShotId, type, rdata);	
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[idx] << ":" << type << enderr;
		return 0;
	}

	if (!iil || iil->getIILType() != eAosIILType_Hit)
	{
		OmnAlarm << "Not a docid IIL: "
			<< mSubiils[idx]->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}
	mSubiils[idx] = (AosIILHit*)iil.getPtr();
	mSubiils[idx]->mRootIIL = this;
	mSubiils[idx]->mIILIdx = idx;

	aos_assert_r(mSubiils[0], 0);
	return mSubiils[idx];
}


void    
AosIILHit::failInSetCtntProtection()
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
	if (mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
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
	mNumSubiils = 0;
	mRootIIL = 0;
	mIILIdx = -1;
}


bool
AosIILHit::addDocPriv(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the
	// list. The list is sorted based on value, ascending.

//OmnScreen << "------IILHit addDocPriv."
//		<< "; iilid:" << mIILID 
//		<< "; iilname:" << mIILName
//		<< "; docid: " << docid
//		<< endl;

	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		AosIILHitPtr subiil;
		subiil = getSubiil(docid, rdata);
		aos_assert_r(subiil, false);
		
		if (subiil.getPtr() == this)
		{
			return insertDocid(docid, rdata);
		}

		bool rslt = subiil->addDocSafe(docid, rdata);
		aos_assert_r(rslt, false);
		return rslt;
	}

	bool rslt = insertDocid(docid, rdata);
	aos_assert_r(rslt, false);

	if (sgMaxIILLen < mNumDocs) 
	{
		sgMaxIILLen = mNumDocs;
	}
	return rslt;
}


AosIILHitPtr
AosIILHit::getSubiil(const u64 value, const AosRundataPtr &rdata)
{
	i64 index = getSubiilIndex(value);
	return getSubiilByIndex(index, rdata);
}


bool
AosIILHit::insertDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	//1. find which subiil we need to insert into
	//2. judge whether we need to split the iil
	//3. find out which iil we need to insert into after split
	//4. insert it
	//5. update max/min value

	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILHitPtr subiil = 0;
		bool rslt = splitListPriv(subiil, rdata);
		aos_assert_r(rslt, false);
		
		// judge whether we need to insert the docid into the sub iil
		aos_assert_r(subiil, false);
		aos_assert_r(mNumDocs > 0, false);
		
		if (docid > mDocids[mNumDocs-1])
		{
			return subiil->insertDocid(docid, rdata);
		}
	}

	i64 idx = 0;
	i64 left = 0;
	i64 right = mNumDocs-1;

	if (mNumDocs == 0)
	{
		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
		aos_assert_r(mDocids, false);
		mDocids[0] = docid;
		mNumDocs = 1;
		idx = 0;
		goto finished;
	}

	aos_assert_r(mDocids, false);
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
		if (mDocids[idx] == docid)
		{
			// It is possible for the same doc to appear multiple
			// times in a value IIL. In the current implementation,
			// this list will remember all the occurances of the
			// same doc and its corresponding idx.
		 	if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
			i64 numToMove = mNumDocs - idx - 1;
			memmove(&mDocids[idx+2], &mDocids[idx+1], (numToMove << 3));
			mDocids[idx+1] = docid;
			mNumDocs++;
			idx++;
			goto finished;
		}

		if (mDocids[idx] < docid) 
		{
			if (idx == mNumDocs-1)
			{
				// Append the docid
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
				mDocids[idx+1] = docid;
				mNumDocs++;
				idx ++;
				goto finished;
			}

			if (docid < mDocids[idx+1] || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
				i64 numToMove = mNumDocs - idx - 1;
				memmove(&mDocids[idx+2], &mDocids[idx+1], (numToMove << 3));
				mDocids[idx+1] = docid;
				mNumDocs++;
				idx ++;
				goto finished;
			}

			// This means "docid >= mDocids[idx+1]"
			left = idx+1;
	 	}
		else
		{
			if (idx == 0)
			{
				// Insert into the front
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
		 		memmove(&mDocids[1], mDocids, mNumDocs << 3);
		 		mDocids[0] = docid;
				mNumDocs++;
				goto finished;
			}

			if (mDocids[idx-1] < docid || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
				i64 numToMove = mNumDocs - idx;
				memmove(&mDocids[idx+1], &mDocids[idx], (numToMove << 3));
				mDocids[idx] = docid;
				mNumDocs++;
				goto finished;
			}

			right = idx-1;
		}
	}

	OmnShouldNeverComeHere;
	return false;

finished:
	// AosIILTransTester::addDocidByIIL(mIILID, docid); // Chen Ding, 01/28/2012, Test only	
	mIsDirty = true;

	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILHitPtr rootiil = (AosIILHit*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
	}

	return true;
}


i64
AosIILHit::getSubiilIndex(const u64 &value)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
	for (i64 i=0; i < mNumSubiils; i++)
	{
		if (mMaxVals[i] >= value)
		{
			// Found the sub-iil
			return i;
		}
	}
	return mNumSubiils - 1;
}


bool
AosIILHit::splitListPriv(
		AosIILHitPtr &subiilDocid, 
		const AosRundataPtr &rdata)
{
	// It cuts the current list in two. The first half stays 
	// with the current list and the second half is in a new
	// IIL list.
	// 1. Check whether it can create more subiils
	aos_assert_r(mNumSubiils < mMaxSubIILs-1, false);
	subiilDocid = 0;
	// 2. Create the subiil

	AosIILObjPtr subiil = AosIILMgrSelf->createSubIILSafe(
		mIILID, mSiteid, mSnapShotId, eAosIILType_Hit, mIsPersis, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->getIILType() == eAosIILType_Hit, false);
	AosIILHitPtr newsubiil = (AosIILHit*)subiil.getPtr();
	
	newsubiil->setSnapShotId(mSnapShotId);

	// 3. Set mRootIIL
	if (mRootIIL.isNull())
	{
		mRootIIL = this;
		mIILIdx = 0;
	}
	newsubiil->setRootIIL(mRootIIL);
	newsubiil->setIILIdx(mIILIdx + 1);

	// 4. Initialize the new subiil
	i64 startidx = mNumDocs / 2;
	const i64 len = mNumDocs - startidx;
	aos_assert_r(mRootIIL->getIILType() == eAosIILType_Hit, false);
	AosIILHitPtr rootiil = (AosIILHit*)mRootIIL.getPtr();
	aos_assert_r(mIILIdx >= 0, false);

	bool rslt = newsubiil->initSubiil((u64*)&mDocids[startidx], len, mIILIdx+1, rootiil);
	aos_assert_r(rslt, false);

	// 5. Shrink this IIL
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	mIsDirty = true;
	
	aos_assert_r(mNumDocs < mMaxNormIILSize, false);

	// 6. Add the subiil
	AosIILHitPtr thisptr(this, false);
	rslt = rootiil->addSubiil(thisptr, newsubiil, rdata);
	aos_assert_r(rslt, false);
	saveSanityCheckProtected(rdata);

	newsubiil->setDirty(true);
	subiilDocid = (AosIILHit*)subiil.getPtr();
	return true;
}


bool
AosIILHit::expandMem()
{
	i64 newcapacity = mMemCap + mExtraDocids;
	i64 newsize = newcapacity << 3;
	char *mem = getMemory(newsize, (char *)mDocids, (mNumDocs << 3));
	aos_assert_r(mem, false);
	mDocids = (u64 *)mem;
	mMemCap = newcapacity;
	newsize = newcapacity << 2;
	aos_assert_r(mem, false);
	return true;
}


bool
AosIILHit::updateIndexData(
		const i64 &idx,
		const bool changeMax,
		const bool changeMin,
		const AosRundataPtr &rdata)
{
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	aos_assert_r(isParentIIL(), false);
	aos_assert_r(idx >= 0 && mNumSubiils >0 && idx < mNumSubiils, false);
	aos_assert_r(mSubiils[idx], false);

	AosIILHitPtr subiil = mSubiils[idx];
	mNumEntries[idx] = subiil->getNumDocs();

	if (mNumEntries[idx] > 0 && changeMax)
	{
		mMaxVals[idx] = subiil->getMaxValue();
	}

	if (mNumEntries[idx] > 0 && changeMin)
	{
		mMinVals[idx] = subiil->getMinValue();
	}
	mIsDirty = true;
	return true;
}


bool 
AosIILHit::initSubiil(
		u64 *docids, 
		const i64 &numDocs,
		const i64 &subiilidx, 
		const AosIILHitPtr &rootiil) 
{
	aos_assert_r(mIILType == eAosIILType_Hit, false);
	aos_assert_r(rootiil, false);

	// 1. Set the root
	mRootIIL = rootiil.getPtr();
    mWordId = AOS_INVWID;

    // ModifyNumDocs
    mNumDocs = numDocs; 

	aos_assert_r(mNumDocs < mMaxNormIILSize, false);
    mFlag = 0;
    mHitcount = 0;
	//mTotalDocs = 0;
    //mIILDiskSize = 0;	// Ketty 2012/11/15
    //mOffset = 0;		// Ketty 2012/11/15
    //mSeqno = 0;		// Ketty 2012/11/15
	mCompressedSize = 0;

	// Prepare the memory
	bool rslt = prepareMemoryForReloading2();
	aos_assert_r(rslt, false);

	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool
AosIILHit::addSubiil(
		const AosIILHitPtr &crtsubiil,
		const AosIILHitPtr &newsubiil,
		const AosRundataPtr &rdata)
{
	// It inserts the new subiil 'newsubiil' after 'crtsubiil'. This function
	// must be called by the root IIL.
	aos_assert_r(mIILIdx == 0, false);
	aos_assert_r(mNumSubiils < mMaxSubIILs-1, false);
	if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
	i64 iilidx = crtsubiil->getIILIdx();
	aos_assert_r(iilidx>=0, false);

	i64 num_to_move = mNumSubiils - iilidx - 1;
	// 1. Insert the new subiil in mSubiils[]
	if (iilidx < mNumSubiils-1)
	{
		//ken 2012/11/12
		// this place must use for loop
		//memmove(&mSubiils[iilidx+2], &mSubiils[iilidx+1], sizeof(AosIILHitPtr) * (mNumSubiils-iilidx-1));
		//memset(&mSubiils[iilidx+1], 0, sizeof(AosIILHitPtr));
        for (int i=num_to_move; i>0; i--)
		{
			mSubiils[iilidx+i+1] = mSubiils[iilidx+i];
		}
		mSubiils[iilidx+1] = 0;

		memmove(&mMinVals[iilidx+2], &mMinVals[iilidx+1], sizeof(u64) * (mNumSubiils-iilidx-1));
		mMinVals[iilidx+1] = 0;

		memmove(&mMaxVals[iilidx+2], &mMaxVals[iilidx+1], sizeof(u64) * (mNumSubiils-iilidx-1));
		mMaxVals[iilidx+1] = 0;

		memmove(&mIILIds[iilidx+2], &mIILIds[iilidx+1], sizeof(u64) * (mNumSubiils-iilidx-1));
		mIILIds[iilidx+1] = 0;

		memmove(&mNumEntries[iilidx+2], &mNumEntries[iilidx+1], sizeof(i64) * (mNumSubiils-iilidx-1));
		mNumEntries[iilidx+1] = 0;

		// Modify the iil index
		for (i64 i=iilidx+2; i<mNumSubiils; i++)
		{
			if (mSubiils[i])
			{
				mSubiils[i]->setIILIdx(i);
			}
		}
	}	

	mSubiils[iilidx] = crtsubiil;
	mSubiils[iilidx+1] = newsubiil;
	newsubiil->setIILIdx(iilidx+1);
	mIILIds[iilidx+1] = newsubiil->getIILID();
	mNumSubiils++;
	if (mNumSubiils == 1) mNumSubiils++;

	// 2. Modify crtsubiil's and newsubiil's min/max values
	// 4. Set mIILIds[]
	mIILIds[iilidx+1] = newsubiil->getIILID();	

	// 4. If 'iilidx' is 0, it needs to set the first entry
	if (iilidx == 0)
	{
		mRootIIL = this;
		mSubiils[0] = this;
		mIILIdx = 0;
		mIILIds[0] = mIILID;
	}
	// 5. update all the mIILIdx in the subiils
	for(i64 i = iilidx; i < mNumSubiils;i++)
	{
		if (mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}

	bool rslt = updateIndexData(iilidx, true, true, rdata);
	aos_assert_r(rslt, false);
	
	rslt = updateIndexData(iilidx+1, true, true, rdata);
	aos_assert_r(rslt, false);

	aos_assert_r(mSubiils[0], false);

	mIsDirty = true;
	crtsubiil->setDirty(true);
	newsubiil->setDirty(true);
	return true;
}


const u64
AosIILHit::getMinValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mDocids[0];
}


const u64
AosIILHit::getMaxValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mDocids[mNumDocs-1];
}


bool
AosIILHit::prepareMemoryForReloading2()
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
	return prepareMemoryForReloading();
}

bool
AosIILHit::createSubiilIndex()
{
//	aos_assert_r(mNumSubiils == 0, false);
	aos_assert_r(!mSubiils, false);
	mSubiils = OmnNew AosIILHitPtr[mMaxSubIILs];
	aos_assert_r(mSubiils, false);

	aos_assert_r(!mMinVals, false);
	mMinVals = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mMinVals, false);
	memset(mMinVals, 0, sizeof(char*) * mMaxSubIILs);

	aos_assert_r(!mMaxVals, false);
	mMaxVals = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mMaxVals, false);
	memset(mMaxVals, 0, sizeof(char*) * mMaxSubIILs);

	aos_assert_r(!mIILIds, false);
	mIILIds = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * mMaxSubIILs);

	aos_assert_r(!mNumEntries, false);
	mNumEntries = OmnNew i64[mMaxSubIILs];
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(i64) * mMaxSubIILs);

	return true;
}


bool
AosIILHit::removeDocPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	aos_assert_r(isParentIIL()||isSingleIIL(), false);
	
	if (mNumDocs == 0) return true;

	if (isSingleIIL())
	{
		return removeDocDirectPriv(docid, rdata);
	}

	// this is the root iil, find the sub and
	// call the direct func
	AosIILHitPtr curSub = getSubiil(docid, rdata);
	aos_assert_r(curSub, false);

	bool rslt = curSub->removeDocDirectPriv(docid, rdata);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosIILHit::removeDocDirectPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	i64 idx = -1;
	if (!getFirstDocIdx(docid, idx))
	{
		//OmnAlarm << "docid: " << docid << ":" << idx << ":" 
		//	<< mNumDocs << ":" << mIILID << enderr;
		//return false;
		// Ken Lee, 2013/03/30
		OmnScreen << "docid: " << docid << ":" << idx << ":"
			<< mNumDocs << ":" << mIILID << endl;
		return true;
	}

	if (idx < mNumDocs-1)
	{
		memmove(&mDocids[idx], &mDocids[idx+1], (mNumDocs-idx-1) << 3);
	}
	mNumDocs--;
	mIsDirty = true;
	
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILHitPtr rootiil = (AosIILHit*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
		if (mNumDocs < mMinIILSize)
		{
			rootiil->mergeSubiilPriv(mIILIdx, rdata);
		}
	}
	return true;
}


bool
AosIILHit::getFirstDocIdx(const u64 &docid, i64 &theidx)
{
	// The caller should have locked the class. It retrieves 
	// the index of the 'docid'. If not found, it returns false. 
	// Otherwise, the index is stored in 'theidx' and returns true.
	// Note that there can be multiple docs with the same
	// docids. Same docids are stored consecutively. If there are
	// multiple same docids, it returns the first one.
	if (!mDocids)
	{
		aos_assert_r(mNumDocs <= 0, false);
		theidx = -1;
		return false;
	}

	aos_assert_r(mDocids, false);
	aos_assert_r(mNumDocs > 0, false);

	i64 idx = 0;
	i64 left = (theidx >=0)?theidx:0; 
	i64 right = mNumDocs-1;

	if (mDocids[left] == docid)
	{
		// Chen Ding, 12/16/2012
		// idx = left;
		theidx = left;
		return true;
	}
		
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
		if (mDocids[idx] == docid)
		{
			// In the current implementation, we will do the linear
			// search for the first one with 'docid'.
			i64 prev = idx--;
			while (idx >= 0 && mDocids[idx] == docid) 
			{
				prev = idx;
				idx--;
			}

			if (idx < 0)
			{
				theidx = 0;
				return true;
			}

			theidx = idx+1;
			return true;
		}

		if (mDocids[idx] < docid)
		{
			left = idx+1;
		}
		else
		{
			right = idx-1;
		}
	}

	// It does not exist
	theidx = -1;
	return false;
}

bool 		
AosIILHit::mergeSubiilPriv(
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
	// This function should be called on mRootIIL. The iil 'iilidx' is too 
	// small. The function will re-arrange the contents as follows:
	// 1. If 'iilidx-1' and 'iilidx' together < mMaxNormIILSize, average them
	// 2. Otherwise, if 'iilidx' and 'iilidx+1' < mMaxNormIILSize, average them
	// 3. Otherwise, 
	// for torturer
	mIILMergeFlag = true;

	aos_assert_r(isParentIIL(),false);	
	aos_assert_r(mNumSubiils > 1,false);
	aos_assert_r(iilidx >= 0,false);

	//1. If there is a iil around, two iils contains more than 	
	//   mMaxNormIILSize docs, merge some docs from that iil to this one.
	AosIILHitPtr iil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iil,false);
	
	i64 numdoc1 = iil->getNumDocs();
	i64 numdoc2 = 0; // num doc prev
	i64 numdoc3 = 0; // num doc next
	i64 merge_size = mMaxNormIILSize/2 + mMinIILSize;
	bool rslt = false;
	if (iilidx > 0)
	{
		AosIILHitPtr iil2 = getSubiilByIndex(iilidx - 1, rdata);
		aos_assert_r(iil2,false);		
		
		numdoc2 = iil2->getNumDocs();

		if (numdoc1 + numdoc2 >= merge_size)
		{
			// iilidx-1 and iilidx are too 
			// move some docs from iil2 to iil1
			i64 nummove = (numdoc1 + numdoc2)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			
			rslt = mergeSubiilForwardPriv(iilidx -1, nummove, rdata);
			aos_assert_r(rslt, false);
			return rslt;
		}
	}
	
	if (iilidx < mNumSubiils - 1)
	{
		AosIILHitPtr iil2 = getSubiilByIndex(iilidx + 1, rdata);
		aos_assert_r(iil2,false);

		numdoc3 = iil2->getNumDocs();

		if (numdoc1 + numdoc3 >= merge_size)
		{
			i64 nummove = (numdoc1 + numdoc3)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			
			rslt = mergeSubiilBackwardPriv(iilidx + 1, nummove, rdata);
			aos_assert_r(rslt, false);
			return true;
		}
	}
	
	aos_assert_r(mNumSubiils > 0,false); 

	// if it is the first docid, merge the 2nd one to the first one.		
	if (iilidx == 0)
	{
		// merge iil[1] to iil[0]
		rslt = mergeSubiilBackwardPriv(1, numdoc3, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	

	// if it is the last docid, merge this iil to the one before.		
	if (iilidx == mNumSubiils -1)
	{
		rslt = mergeSubiilBackwardPriv(iilidx, numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	
	
	// The iil is in the middle
	if (numdoc2 < numdoc3)
	{
		// merge to the one before
		rslt = mergeSubiilBackwardPriv(iilidx, numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;		
	}
	else
	{
		//merge to the next one.
		rslt = mergeSubiilForwardPriv(iilidx, numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;		
	}

	OmnShouldNeverComeHere;
	return true;
}


bool 		
AosIILHit::mergeSubiilForwardPriv(
		const i64 &iilidx,
		const i64 &numDocToMove,
		const AosRundataPtr &rdata)
{
	// This function is used by root iil, it moves [numDocToMove] docs from 
	// subiil[iilidx] to subiil[iilidx+1]. If the original subiil mNumDocs finally 
	// turns to 0, we'll call another function to remove the subiil 
	// from the subiil list. To remove all the entries from the root 
	// iil(the first subiil) is forbidden. We'll check it before.
	
	// 1. validation(params checking)
	aos_assert_r(isParentIIL(),false);
	aos_assert_r(iilidx >= 0  && iilidx < mNumSubiils -1,false);
	aos_assert_r(mNumEntries[iilidx] >= numDocToMove,false);		
	if (iilidx == 0)
	{
		aos_assert_r(mNumEntries[iilidx] > numDocToMove,false);		
	}

	// 2. move entries from a to b
	AosIILHitPtr crtiil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(crtiil, false);
	
	AosIILHitPtr nextiil = getSubiilByIndex(iilidx+1, rdata);
	aos_assert_r(nextiil, false);
		
	bool rslt = false;
	u64* docidPtr = 0;
	// get pointers
	i64 offset = crtiil->getNumDocs() - numDocToMove;
	rslt = crtiil->getDocidPtr(docidPtr, offset);
	aos_assert_r(rslt, false);

	// append to new
	rslt = nextiil->appendDocToSubiil(docidPtr, numDocToMove, true);
	aos_assert_r(rslt, false);

	rslt = crtiil->removeDocFromSubiil(numDocToMove, false);
	aos_assert_r(rslt, false);
			
	// 3. update information in root iil,
	// remove the original subiil if it is empty now
	updateIndexData(iilidx+1, true, true, rdata);
	if (crtiil->getNumDocs() == 0)
	{
		removeSubiil(iilidx, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true, rdata);
	}
	return true;	
}


bool 		
AosIILHit::mergeSubiilBackwardPriv(
		const i64 &iilidx,
		const i64 &numDocToMove,
		const AosRundataPtr &rdata)
{
	// This function is used by root iil, it moves [numDocToMove] docs 
	// from subiil[iilidx] to subiil[iilidx-1]. If the original subiil 
	// mNumDocs finally turns to 0, we'll call another function to remove 
	// the subiil from the subiil list.
	// To remove all the entries from the root iil(the first subiil) 
	// is forbidden. We'll check it before.
	
	// 1. validation(params checking)
	aos_assert_r(isParentIIL(), false);
	aos_assert_r(iilidx > 0 && iilidx <= mNumSubiils -1,false);
	
	// 2. move entries from a to b
	AosIILHitPtr previil = getSubiilByIndex(iilidx-1, rdata);
	AosIILHitPtr crtiil =  getSubiilByIndex(iilidx, rdata);
	aos_assert_r(previil, false);
	aos_assert_r(crtiil, false);
		
	bool rslt = false;
	u64* docidPtr = 0;
	// get pointers
	rslt = crtiil->getDocidPtr(docidPtr,0);
	aos_assert_r(rslt, false);

	// Append 'numDocToMove' entries to the previous iil.
	rslt = previil->appendDocToSubiil(docidPtr, numDocToMove, false);
	aos_assert_r(rslt, false);

	// Remove the first 'numDocToMove' entries from the current IIL.
	rslt = crtiil->removeDocFromSubiil(numDocToMove, true);
	aos_assert_r(rslt, false);
			
	// 3. update information in root iil,
	// remove the original subiil if it is empty now
	updateIndexData(iilidx-1, true, true, rdata);

	if (crtiil->getNumDocs() == 0)
	{
		removeSubiil(iilidx, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true, rdata);
	}
	return true;	
}


bool		
AosIILHit::getDocidPtr(
		u64* &docidPtr,
		const i64 &offset)
{
	aos_assert_r(mDocids, false);
	aos_assert_r(offset >= 0, false);
	aos_assert_r(offset < mNumDocs, false);
	docidPtr = &(mDocids[offset]);
	return true;
}


bool 
AosIILHit::appendDocToSubiil(
		u64 *docids, 
		const i64 &numDocs,
		const bool addToHead) 
{
	// Notice that we use memcpy to copy string point list to the new subiil, 
	// So we can NOT use delete in the original subiil, instead, we use 
	// memset to clear that memory.
	aos_assert_r(mIILType == eAosIILType_Hit, false);

	// ModifyNumDocs
	aos_assert_r(mNumDocs + numDocs < mMaxNormIILSize, false);
	if (numDocs == 0)
	{
		return true;
	}
	
	i64 newNumDocs = mNumDocs + numDocs; 
	aos_assert_r(newNumDocs > 0,false);	
	//1. enlarge mValues/mDocids space to hold all the values and docids, in the same time, 
	// move the original data and new data to the new place.	
	// if the number of the new list is less than mMemCap, do nothing to 		
	
	if (newNumDocs < mMemCap)
	{
		if (addToHead)
		{
			// copy docid
			memmove(&(mDocids[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mDocids, docids, sizeof(u64) * numDocs);
		}	
		else
		{
			// copy docid
			memcpy(&mDocids[mNumDocs], docids, sizeof(u64) * numDocs);			
		}
	}
	else
	{
		// for docids
		u64 *mem2 = OmnNew u64[newNumDocs + mExtraDocids];
		aos_assert_r(mem2, false);
		memset(mem2 , 0, sizeof(u64) * (newNumDocs + mExtraDocids));
	
		mMemCap = newNumDocs;

		if (addToHead)
		{
			// copy docid
			memmove(&(mem2[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mem2,docids,sizeof(u64) * numDocs);
			
		}
		else
		{
			// copy docid
			memmove(mem2, mDocids, sizeof(u64) * mNumDocs);			
			memcpy(&(mem2[mNumDocs]), docids, sizeof(u64) * numDocs);			
		}
		OmnDelete [] mDocids;
			
		mDocids = mem2;
	}

	mNumDocs = newNumDocs;
	mIsDirty = true;
	return true;
}


bool 
AosIILHit::removeDocFromSubiil(const i64 &numRemove, const bool delFromHead)
{
	// Notice:
	// 1. When we move docs to another subiil, we use memcpy to copy string point list. 
	// 	  So we can NOT use delete in the original subiil, instead, we use 
	// 	  memset to clear that memory.
	// 2. We do not shrink memory when deleting elements.
	aos_assert_r(numRemove <= mNumDocs, false);
	i64 newsize = mNumDocs - numRemove;
	if (delFromHead)
	{
		// copy docid
		OmnMemCheck(mDocids, sizeof(u64) * (newsize)); 
		memmove(mDocids, &(mDocids[numRemove]), sizeof(u64) * (newsize));
		OmnMemCheck(&mDocids[newsize], sizeof(u64) * numRemove);
		memset(&(mDocids[newsize]), 0, sizeof(u64) * numRemove);
	}
	else // del from tail
	{
		OmnMemCheck(&mDocids[newsize] , sizeof(u64)*numRemove);
		memset(&(mDocids[newsize]),0,sizeof(u64)*numRemove);		
	}
	
	mNumDocs = newsize;
	mIsDirty = true;
	return true;
}


bool 		
AosIILHit::removeSubiil(
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilidx > 0 && iilidx < mNumSubiils,false);
	aos_assert_r(mNumSubiils > 1,false);
	bool rslt = false;
	//1. return the iil
	if (mSubiils[iilidx])
	{
		aos_assert_r(mSubiils[iilidx]->getNumDocs() == 0,false);
		rslt = AosIILMgrSelf->returnIILPublic(mSubiils[iilidx], rdata);
		aos_assert_r(rslt, false);
		mSubiils[iilidx] = 0;
	}

	//2. proc mMinVals and mMaxVals;
	if (iilidx < mNumSubiils -1)
	{
		memmove(&(mMinVals[iilidx]),&(mMinVals[iilidx+1]),(sizeof(u64))*(mNumSubiils - iilidx - 1));
		memmove(&(mMaxVals[iilidx]),&(mMaxVals[iilidx+1]),(sizeof(u64))*(mNumSubiils - iilidx - 1));
	}
	mMinVals[mNumSubiils-1] = 0;
	mMaxVals[mNumSubiils-1] = 0;

	//3. proc mNumEntries mIILIds
	if (iilidx < mNumDocs -1)
	{
		memmove(&(mNumEntries[iilidx]),&(mNumEntries[iilidx +1]),(sizeof(i64)*(mNumSubiils - 1)));
		memmove(&(mIILIds[iilidx]),&(mIILIds[iilidx +1]),(sizeof(u64)*(mNumSubiils - 1)));
	}
	mNumEntries[mNumSubiils-1] = 0;
	mIILIds[mNumSubiils-1] = 0;
	
	//4. proc mSubiils
	for(i64 i = iilidx ;i < mNumSubiils - 1;i++)
	{
		mSubiils[i] = mSubiils[i+1];// no matter it is null or not
		
		if (mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}
	mSubiils[mNumSubiils - 1] = 0;
	mNumSubiils --;

	//5. do some processing if the iil turns to be a single one 
	if (mNumSubiils == 1)
	{
		mRootIIL = 0;
		mNumSubiils = 0;
		mIILIdx = -1;
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
		if (mSubiils)
		{
			OmnDelete [] mSubiils;
			mSubiils = 0;
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
	}
	
	//6. set dirty flag
	mIsDirty = true;
	return true;	
}


bool        
AosIILHit::queryPriv(
		const AosQueryRsltObjPtr& query_rslt,
		const AosBitmapObjPtr& query_bitmap,
		const AosRundataPtr &rdata)
{
	// IILDocids are used as companion IILs. Entries are always
	// sorted based on docids. This function copies all its 
	// data into 'query_rslt'. This function should be called
	// by the root iil. Otherwise, it is an error.
    bool rslt = false;
	if (isParentIIL())
	{
		// Calculate the total entries (Note: this should have been
		// stored in the root IIL).
		i64 num_docs = 0;
		for(i64 i=0; i<mNumSubiils; i++)
		{
			num_docs += mNumEntries[i];
		}
		
		if (query_rslt.notNull())
		{
			// Chen Ding, 08/02/2011
			// query_rslt->setBlockSize(num_docs);
		}
		
		// Copy the root iil data
		rslt = queryDirectPriv(query_rslt,query_bitmap);
		aos_assert_r(rslt, false);

		// Copy all the subiils
		AosIILHitPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil,false);
			rslt = subiil->queryDirectSafe(query_rslt,query_bitmap);
			aos_assert_r(rslt, false);
		}
	}
	else// single
	{
		aos_assert_r(isSingleIIL(),false);

		if (query_rslt.notNull())
		{
			// Chen ding, 08/02/2011
			// query_rslt->setBlockSize(mNumDocs);
		}
		rslt = queryDirectPriv(query_rslt,query_bitmap);
	}

	aos_assert_r(rslt, false);

	// if (query_rslt.notNull())
	// {
	// 		query_rslt->setOrdered(true);
	// }
	return true;
}


bool        
AosIILHit::queryDirectPriv(
		const AosQueryRsltObjPtr& query_rslt,
		const AosBitmapObjPtr& query_bitmap)
{
	if (mNumDocs <= 0) return true;

    // copy data
    bool rslt = copyDocidsPriv(query_rslt, query_bitmap, 0, mNumDocs-1);
    aos_assert_r(rslt, false);
    return true;	
}


bool
AosIILHit::addDocSeqPriv(
		const u64 &docid,
		i64 &iilidx,
		i64 &idx,
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the
	// list. The list is sorted based on value, ascending.
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		AosIILHitPtr subiil;
		i64 newiilidx = getSubiilIndexSeq(docid, iilidx);
		if (newiilidx != iilidx)
		{
			idx = -1;
		}
		iilidx = newiilidx;
		
		subiil = getSubiilByIndex(iilidx, rdata);
		aos_assert_r(subiil, false);
		
		if (subiil.getPtr() == this)
		{
			return insertDocidSeq(docid, iilidx, idx, rdata);
		}

		return subiil->addDocSeqSafe(docid, iilidx, idx, rdata);
	}

	// singleIIL or subiil
	bool rslt = insertDocidSeq(docid, iilidx, idx, rdata);
	aos_assert_r(rslt, false);

	if (sgMaxIILLen < mNumDocs) 
	{
		sgMaxIILLen = mNumDocs;
	}
	return rslt;
}


bool
AosIILHit::removeDocSeqPriv(
		const u64 &docid,
		i64 &iilidx,
		i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(isParentIIL()||isSingleIIL(), false);

	if (!isParentIIL())
	{
		return removeDocSeqDirectPriv(docid,iilidx,idx, rdata);
	}

	// this is the root iil, find the sub and
	// call the direct func
	i64 newiilidx = getSubiilIndexSeq(docid,iilidx);
	if (newiilidx != iilidx)
	{
		idx = -1;
	}
	iilidx = newiilidx;
	
	AosIILHitPtr curSub = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(curSub, false);

	bool rslt = false;
	if (curSub.getPtr() == this)
	{
		rslt = removeDocSeqDirectPriv(docid,iilidx,idx, rdata);
	}
	else
	{
		rslt = removeDocSeqSafe(docid, iilidx, idx, rdata);
	}
	aos_assert_r(rslt, false);
	return true;
}


i64	 
AosIILHit::getSubiilIndexSeq(const u64 &value, const i64 &iilidx)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
	for (i64 i=iilidx; i < mNumSubiils; i++)
	{
		if (mMaxVals[i] >= value)
		{
			// Found the sub-iil
			return i;
		}
	}
	return mNumSubiils-1;
}


bool
AosIILHit::insertDocidSeq(
		const u64 &docid, 
		i64 &iilidx,
		i64 &theidx,
		const AosRundataPtr &rdata)
{
	//1. find which subiil we need to insert into
	//2. judge whether we need to split the iil
	//3. find out which iil we need to insert into after split
	//4. insert it
	//5. update max/min value

	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILHitPtr subiil = 0;
		bool rslt = splitListPriv(subiil, rdata);
		aos_assert_r(rslt, false);
		
		// judge whether we need to insert the docid into the sub iil
		aos_assert_r(subiil, false);
		aos_assert_r(mNumDocs > 0, false);
		if (docid > mDocids[mNumDocs-1])
		{
			iilidx ++;
			return subiil->insertDocidSeq(docid, iilidx, theidx, rdata);
		}
	}
	
	i64 idx = 0;
	i64 left = 0;
	i64 right = mNumDocs-1;
	left = (theidx >= 0)?theidx:0;
	if (theidx > right)
	{
		theidx = right;
		left = right;
	}

	if (mNumDocs == 0)
	{
		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
		aos_assert_r(mDocids, false);
		mDocids[0] = docid;
		mNumDocs = 1;
		idx = 0;
		goto finished;
	}

	if (mDocids[left] >= docid)
	{
		right = left;
	}
		
	aos_assert_r(mDocids, false);
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
		if (mDocids[idx] == docid)
		{
			// It is possible for the same doc to appear multiple
			// times in a value IIL. In the current implementation,
			// this list will remember all the occurances of the
			// same doc and its corresponding idx.
		 	if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
			i64 numToMove = mNumDocs - idx - 1;
			memmove(&mDocids[idx+2], &mDocids[idx+1], (numToMove << 3));
			mDocids[idx+1] = docid;
			mNumDocs++;
			idx++;
			goto finished;
		}

		if (mDocids[idx] < docid) 
		{
			if (idx == mNumDocs-1)
			{
				// Append the docid
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
				mDocids[idx+1] = docid;
				mNumDocs++;
				idx ++;
				goto finished;
			}

			if (docid < mDocids[idx+1] || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
				i64 numToMove = mNumDocs - idx - 1;
				memmove(&mDocids[idx+2], &mDocids[idx+1], (numToMove << 3));
				mDocids[idx+1] = docid;
				mNumDocs++;
				idx ++;
				goto finished;
			}

			// This means "docid >= mDocids[idx+1]"
			left = idx+1;
	 	}
		else
		{
			if (idx == 0)
			{
				// Insert into the front
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
		 		memmove(&mDocids[1], mDocids, mNumDocs << 3);
		 		mDocids[0] = docid;
				mNumDocs++;
				goto finished;
			}

			if (mDocids[idx-1] < docid || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap) aos_assert_r(expandMem(), false);
				i64 numToMove = mNumDocs - idx;
				memmove(&mDocids[idx+1], &mDocids[idx], (numToMove << 3));
				mDocids[idx] = docid;
				mNumDocs++;
				goto finished;
			}

			right = idx-1;
		}
	}

	OmnShouldNeverComeHere;
	return false;

finished:
	// AosIILTransTester::addDocidByIIL(mIILID, docid); // Chen Ding, 01/28/2012, Test only
	mIsDirty = true;
	theidx = idx;
	
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILHitPtr rootiil = (AosIILHit*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
	}
	return true;
}


bool
AosIILHit::removeDocSeqDirectPriv(
		const u64 &docid,
		i64 &iilidx,
		i64 &idx,
		const AosRundataPtr &rdata)
{
	if (!getFirstDocIdx(docid, idx))
	{
		OmnAlarm << "docid: " << docid << ":" << idx << enderr;
		return false;
	}

	if (idx < mNumDocs-1)
	{
		memmove(&mDocids[idx], &mDocids[idx+1], (mNumDocs-idx-1) << 3);
	}
	mNumDocs--;
	mIsDirty = true;
	
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILHitPtr rootiil = (AosIILHit*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
		if (mNumDocs < mMinIILSize)
		{
			rootiil->mergeSubiilPriv(mIILIdx, rdata);
			iilidx = -1;
			idx = -1;
		}
	}
	return true;
}


bool
AosIILHit::returnSubIILsPriv(
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
	returned = true;
	if (!isParentIIL())
	{
		return true;
	}

	AosIILHitPtr subiil = 0;
	bool subIILReturned = false;
	bool rslt = false;

	// return all sub IIL except itself
	for(i64 i = 1;i < mNumSubiils;i++)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil != this, false);

		//ken 2011/9/2
		if (subiil.notNull()  && !subiil->isDirty())
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


i64			
AosIILHit::getTotalSafe(
		const OmnString &value, 
		const AosOpr opr)
{
	OmnShouldNeverComeHere;
	return -1;
}


i64			
AosIILHit::getTotalSafe(
		const u64 &value, 
		const AosOpr opr)
{
	OmnShouldNeverComeHere;
	return -1;
}


bool
AosIILHit::adjustMemoryProtected()
{
	i64 newsize = ((mNumDocs + mExtraDocidsInit) << 3);
	i64 oldsize = (mMemCap << 3);
	if (newsize < oldsize && newsize + (i64)eMemShreshold >= oldsize)
	{
		return true;
	}

	AosIIL::adjustMemoryProtected();
	return true;
}


bool		
AosIILHit::checkDocSafe( 
		const AosOpr opr,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILHit::checkDocSafe(
		const AosOpr opr,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILHit::firstDoc1(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
		u64 &docid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILHit::firstDoc1(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr,
		const u64 &value,
		u64 &docid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILHit::sanityCheck()
{
	// It should be locked. 
	return true;
}


AosBuffPtr
AosIILHit::getBodyBuffProtected() const
{
	// This function is called when we need to save the IIL.
	i64 expect_size = mNumDocs * 60;
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	// the compiil of the subiil may be with no docid.
//	aos_assert_r(mDocids, false);

	//*buff << mNumDocs;
	buff->setI64(mNumDocs);

	// Ken Lee, 2013/04/18
	//for (i64 i=0; i<mNumDocs; i++)
	//{
	//	// *buff << mDocids[i];
	//	buff->setU64(mDocids[i]);
	//}
	bool rslt = buff->setU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, 0);

	aos_assert_r(mNumSubiils >= 0, 0);
	// *buff << mNumSubiils;
	// *buff << mIILIdx;
	buff->setI64(mNumSubiils);
	buff->setI64(mIILIdx);

	if (isParentIIL())
	{
		/*
		for (i64 i=0; i<mNumSubiils; i++)
		{
			// *buff << mMinVals[i]
			//	<< mMaxVals[i]
			//	<< mIILIds[i]
			//	<< mNumEntries[i];
			//buff->setU64(mMinVals[i]);
			//buff->setU64(mMaxVals[i]);
			//buff->setU64(mIILIds[i]);
			//buff->setI64(mNumEntries[i]);
		}*/

		rslt = buff->setU64s(mMinVals, mNumSubiils);
		aos_assert_r(rslt, 0);

		rslt = buff->setU64s(mMaxVals, mNumSubiils);
		aos_assert_r(rslt, 0);
		
		rslt = buff->setU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, 0);

		rslt = buff->setI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, 0);
	}
	return buff;
}


bool
AosIILHit::setContentsProtected(
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	// This function sets its contents from 'buff', which
	// was read from a file. 'buff' should be arranged
	// as:
	// [mNumSubiils][mIILIdx]
	// for each subiil [min_docid][max_docid][iilid][numEntries]
	// 	[value][value]...[value]
	// The number of docids is mNumDocs
	// Note that this class may be reused, which means it may
	// already have memory assigned.

//	aos_assert_r(mCompIIL, false);
	aos_assert_r(prepareMemoryForReloading2(), false);

	// 1. Retrieve mDocids
//	aos_assert_r(setDocidContents(buff), false);
	mNumDocs = buff->getI64(0);
	aos_assert_r(mNumSubiils < mMaxSubIILs,false);
	aos_assert_r(mNumDocs >= 0, false);

	//for(i64 i = 0; i<mNumDocs; i++)
	//{
	//	mDocids[i] = buff->getU64(0);
	//}

	// Ketty 2014/03/17
	aos_assert_r(mNumDocs <= mMemCap, false); 
	bool rslt = buff->getU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, false);

	// 2. Retrieve Subiil Index Data, if needed
	mNumSubiils = buff->getI64(0);
	
	if (mNumSubiils < 0 || mNumSubiils > mMaxSubIILs)
	{
		// alarm
		OmnAlarm << "mNumSubiils wrong:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	mIILIdx = buff->getI64(0);

	if (mNumSubiils > 0)
	{
		if (!mSubiils)
		{
			if (!createSubiilIndex())
			{
				removeSubiilIndex();
				OmnAlarm << "Failed to create subiil index" << enderr;
				mNumSubiils = 0;
				return false;
			}
		}

		/*
		for (i64 i=0; i<mNumSubiils; i++)
		{
			//mMinVals[i] = buff->getU64(0);
			//mMaxVals[i] = buff->getU64(0);

			//mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			//mNumEntries[i] = buff->getI64(0);
			if (mNumEntries[i] <= 0)
			{
				OmnAlarm << "Failed to get mNumEntries[i]:i = " 
					<< i << " , mNumEntries[i] = " 
					<< mNumEntries[i] << enderr;
				return false;
			}
		}*/

		rslt = buff->getU64s(mMinVals, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mMaxVals, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, false);

		mRootIIL = this;
		mSubiils[mIILIdx] = this;
	}
	else
	{
		// single iil
		mRootIIL 	= 0;
		mNumSubiils = 0;
		mNumEntries = 0;
		aos_assert_r(mIILIds  == 0,false);
		aos_assert_r(mSubiils == 0,false);
		aos_assert_r(mMaxVals == 0,false);
		aos_assert_r(mMinVals == 0,false);
	}
	return true;
}


bool
AosIILHit::removeSubiilIndex()
{
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

	if (mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}

	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}
	return true;
}


u64
AosIILHit::getDocIdPriv1(
				const i64 &idx, 
				const i64 &iilidx, 
				const AosRundataPtr &rdata)
{
	if (!isParentIIL())
	{
		return getDocIdPriv(idx,rdata);
	}

	AosIILHitPtr subiil = getSubiilByIndex(iilidx, rdata);
	if (subiil == this)
	{
		return subiil->getDocIdPriv(idx,rdata);
	}
	else
	{
		return subiil->getDocIdSafe(idx,rdata);
	}
	return 0;
}


bool
AosIILHit::getDocidBody(const AosBuffPtr &buff) const
{
	//for (i64 i=0; i<mNumDocs; i++)
	//{
	//	//*buff << mDocids[i];
	//	buff->setU64(mDocids[i]);
	//}
	
	bool rslt = buff->setU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, 0);

	return true;
}


bool
AosIILHit::resetSubIILInfo(const AosRundataPtr &rdata)
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

	if (mSubiils)
	{
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			if (mSubiils[i])
			{
				OmnAlarm << "Sub IIL " << i << " is not cleaned." << enderr;
			}
			mSubiils[i] = 0;
		}
	}
	OmnDelete[] mSubiils;
	mSubiils = 0;
	mIILIdx = -1;
	mRootIIL = 0;
	mNumSubiils = 0;
	return true;
}


bool
AosIILHit::saveSubIILToLocalFileSafe(const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mNumSubiils > 0 && mIILIdx == 0)
	{
		// this iil is the root iil of the list
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			if (mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToLocalFileSafe(rdata);
				aos_assert_r(rslt,false);
			}
		}
	}
	return true;
}


u64
AosIILHit::getMinDocid() const
{
	aos_assert_r(mNumDocs > 0 && mDocids, AOS_INVDID);
	return mDocids[0];
}


u64
AosIILHit::getMaxDocid() const
{
	aos_assert_r(mNumDocs > 0 && mDocids, AOS_INVDID);
	return mDocids[mNumDocs-1];
}

bool
AosIILHit::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
//	AOSLOCK(mLock);
	if (isParentIIL())
	{
		AosIILHitPtr subiil = 0;
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			aos_assert_r(mMinVals[i] >= mMaxVals[i-1],false);
		}
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			aos_assert_r(mMaxVals[i] >= mMinVals[i],false);
			subiil = mSubiils[i];
			if (subiil.notNull())
			{
				aos_assert_r(mMaxVals[i] == subiil->getMaxValue(),false);
				aos_assert_r(mMinVals[i] == subiil->getMinValue(),false);
			}
		}
	}
//	AOSUNLOCK(mLock);
//	static AosIILObjPtr lsTestIIL = OmnNew AosIILHit();
//	static OmnMutex lsLock;
//
//	lsLock.lock();
//	bool rslt = AosIIL::saveSanityCheck(lsTestIIL);
//	lsLock.unlock();
//	return rslt;
	return true;
}

/*
void
AosIILHit::resetSafe(const u64 &wordid, const u64 &iilid,const bool iilmgrLocked, const AosRundataPtr &rdata)
{
	//1. return all the subiil back
	//2. clean all the information about subiil
	AOSLOCK(mLock);
	if (isParentIIL())
	{
		bool returned = false;
		returnSubIILsPriv(false, returned, rdata, true);
		aos_assert_l(returned, mLock);
	}

	if (mNumEntries)
	{
		delete[] mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
	{
		delete[] mIILIds;
		mIILIds = 0;
	}

	if (mSubiils)
	{
		delete[] mSubiils;
		mSubiils = 0;
	}

	if (mMinVals)
	{
		delete[] mMinVals;
		mMinVals = 0;
	}

	if (mMaxVals)
	{
		delete[] mMaxVals;
		mMaxVals = 0;
	}

	mIILIdx = -1;
	mNumSubiils = 0;
	mRootIIL = 0;

	resetSelf(iilmgrLocked, rdata);

	mWordId = wordid;
	mIILID = iilid;

	AOSUNLOCK(mLock);
}
*/

bool
AosIILHit::docExistPriv(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if (isParentIIL())
	{
		// this is the root iil and it has subiils, so we need
		// to judge which subill(or itself) we can use
		AosIILHitPtr subiil = getSubiil(docid, rdata);
		aos_assert_rb(subiil, mLock, false);

		if (subiil == this)
		{
			return docExistDirectPriv(docid,rdata);
		}
	    return subiil->docExistDirectSafe(docid, rdata);
	}
	return docExistDirectPriv(docid,rdata);
}


bool
AosIILHit::docExistDirectPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function uses binary search to determine whether
	// the given 'docid' exists in this IIL. The list must be
	// sorted (ascending).
	if (!mDocids || mNumDocs <= 0) 
	{
		return false;
	}

	if (docid < mDocids[0]) 
	{
		return false;
	}

	if (docid > mDocids[mNumDocs-1])
	{
		return false;
	}

	i64 idx = -1;
	return getFirstDocIdx(docid, idx);
}


u64
AosIILHit::getNextEntryPriv(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata)
{
	return getDocIdSafe1(idx, iilidx, rdata);
}

/*
bool 		
AosIILHit::modifyDocSafe(const u64 &docid, const u64 &value)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 		
AosIILHit::modifyDocSafe(const u64 &docid, const OmnString &value)
{
	OmnShouldNeverComeHere;
	return false;
}
*/
bool
AosIILHit::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	if (query_rslt)
	{
		query_rslt->setDataReverse(false);
	}
	aos_assert_r(query_context,false);	
	if (query_context->isCheckRslt())
	{
		return checkDocs(query_rslt,rdata);
	}
	
	// not check Doc
	
	//1. get the block start/end idx/iilidx
	//2. copy the data

	//1. get the block start/end idx/iilidx
	i64 block_start_idx 		= 0;
	i64 block_start_iilidx		= 0;
	i64 block_end_idx			= 0;
	i64 block_end_iilidx		= 0;
	
	bool has_data = true;
	bool rslt = queryPagingProc(query_rslt,
					query_bitmap,
					query_context,
					block_start_idx,
					block_start_iilidx,
					block_end_idx,
					block_end_iilidx,
					has_data,
					rdata);
	aos_assert_r(rslt,false);

	if (!has_data)
	{
		query_context->setFinished(true);
		return true;
	}
	
	if (!query_context->finished())
	{
		query_context->setCrtDocid(getDocIdPriv1(query_context->getIndex(),
					  						  query_context->getIILIndex(),
					  						  rdata));
	}	

	//2. copy the data
	rslt = copyData(query_rslt,
					query_bitmap,
					query_context,
					block_start_idx,
					block_start_iilidx,
					block_end_idx,
					block_end_iilidx,
					rdata);	
	if(query_rslt && query_rslt->isWithValues())
	{
		query_rslt->fillValueWithDocid();
	}
	return rslt;
}

bool
AosIILHit::queryPagingProc(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		i64 &start_idx,
		i64 &start_iilidx,
		i64 &end_idx,
		i64 &end_iilidx,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	//1. get the start idx/iilidx
	//2. get the end idx/iilidx
	//3. get the cur idx/iilidx
	//4. get the block start/end  idx/iilidx
           

	bool reverse = false;	
	has_data = true;
	if (query_context->finished())
	{
		has_data = false;
		return true;
	}

	//1. get the start idx/iilidx
	start_idx = 0;
	start_iilidx = 0;

	//2. get the end idx/iilidx	

	if (isSingleIIL())
	{
		end_iilidx = 0;
		end_idx = mNumDocs -1;
	}
	else
	{
		aos_assert_r(isParentIIL(),false);
		end_iilidx = mNumSubiils -1;
		end_idx = mNumEntries[mNumSubiils-1] -1;
	}

	//3. get the cur idx/iilidx
	reverse = query_context->isReverse();
	i64 cur_idx = query_context->getIndex();
	i64 cur_iilidx	= query_context->getIILIndex();
	u64 cur_docid = query_context->getCrtDocid();
	
	if (cur_idx == -10 || cur_iilidx == -10)
	{
		//no need to change current position
	}
	else
	{
		aos_assert_r(cur_idx >= 0 && cur_iilidx >= 0,false);
		nextIndexFixErrorIdx(cur_idx,cur_iilidx,cur_docid,reverse,has_data,rdata);
		if (!has_data)
		{
			return true;
		}
		
		if (!reverse)
		{
			start_idx = cur_idx;
			start_iilidx = cur_iilidx;
		}
		else
		{
			end_idx = cur_idx;
			end_iilidx = cur_iilidx;
		}
	}
	
	if (end_iilidx < start_iilidx || ((end_iilidx == start_iilidx) &&(end_idx < start_idx)))
	{
		has_data = false;
		return true;
	}
	
	//4. get the block start/end  idx/iilidx
	i64 bsize = query_context->getBlockSize();
	if (bsize < 0)
	{
		// it means we need to get all the docids from the iil
		query_context->setFinished(true);
		query_context->setIndex(0);
		query_context->setIILIndex(0);
		return true;		
	}
	
	i64 new_cur_idx = 0;
	i64 new_cur_iilidx = 0;
	if (reverse)
	{
		if (isSingleIIL())
		{
			new_cur_idx = end_idx - bsize + 1;

			if (new_cur_idx < start_idx)
			{
				query_context->setFinished(true);
				query_context->setIndex(0);
				query_context->setIILIndex(0);
				return true;
			}
			
			query_context->setIndex(new_cur_idx);
			query_context->setIILIndex(0);
			start_idx = new_cur_idx+1;
			return true;
		}
		
		// root iil
		new_cur_idx = end_idx;
		new_cur_iilidx = end_iilidx;
		
		while(bsize > 0 && new_cur_iilidx >= start_iilidx)
		{
			if (new_cur_iilidx > start_iilidx)
			{
				if (new_cur_idx +1 >= bsize)
				{
					// we have enough docs to get from this subiil
					new_cur_idx -= bsize;
					bsize = 0;
				}
				else
				{
					// we don't have enough docs to get from this subiil
					bsize -= (new_cur_idx+1);
					new_cur_idx = -1;
				}
			}
			else if (new_cur_iilidx == start_iilidx)
			{
				if (new_cur_idx - start_idx +1 > bsize)
				{
					// we have enough docs to get from this subiil
					new_cur_idx -= bsize;
					bsize = 0;
					break;
				}
				else
				{
					// we have enough docs to get from this subiil
					new_cur_idx = start_idx-1;
					bsize = 0;
					query_context->setFinished(true);
					query_context->setIndex(0);
					query_context->setIILIndex(0);
					return true;
				}
			}
			
			//adjust cur_pos
			if (new_cur_idx < 0)
			{
				new_cur_iilidx --;
				if (new_cur_iilidx < 0)
				{
					new_cur_idx = 0;
				}
				else
				{
					new_cur_idx = mNumEntries[new_cur_iilidx] -1;
				}
			}
		}
		nextQueryPos(start_idx,start_iilidx);
		query_context->setIndex(new_cur_idx);
		query_context->setIILIndex(new_cur_iilidx);
		
		start_idx = new_cur_idx;
		start_iilidx = new_cur_iilidx;
		return true;
	}
	else// not reverse
	{
		if (isSingleIIL())
		{
			new_cur_idx = start_idx + bsize;

			if (new_cur_idx > end_idx)
			{
				query_context->setFinished(true);
				query_context->setIndex(0);
				query_context->setIILIndex(0);
				return true;
			}
			query_context->setIndex(new_cur_idx);
			query_context->setIILIndex(0);
			end_idx = new_cur_idx-1;
			return true;
		}
		
		// root iil
		new_cur_idx = start_idx;
		new_cur_iilidx = start_iilidx;
		i64 num_doc_subiil = 0;
		while(bsize > 0 && new_cur_iilidx <= end_iilidx)
		{
			if (new_cur_iilidx < end_iilidx)
			{
				num_doc_subiil = mNumEntries[new_cur_iilidx];
				if (num_doc_subiil - new_cur_idx >= bsize)
				{
					// we have enough docs to get from this subiil
					new_cur_idx += bsize;
					bsize = 0;
				}
				else
				{
					// we don't have enough docs to get from this subiil
					bsize -= (num_doc_subiil - new_cur_idx);
					new_cur_idx = num_doc_subiil;
				}
			}
			else if (new_cur_iilidx == end_iilidx)
			{
				if (end_idx - new_cur_idx +1 > bsize)
				{
					// we have enough docs to get from this subiil
					new_cur_idx += bsize;
					bsize = 0;
					break;
				}
				else
				{
					// we have enough docs to get from this subiil
					//new_cur_idx = end_idx+1;
					new_cur_idx = end_idx;
					query_context->setFinished(true);
					query_context->setIndex(0);
					query_context->setIILIndex(0);
					break;
				}
			}
			
			//adjust cur_pos
			if (new_cur_idx >= num_doc_subiil)
			{
				new_cur_iilidx ++;
				new_cur_idx = 0;
			}
		}
		
		query_context->setIndex(new_cur_idx);
		query_context->setIILIndex(new_cur_iilidx);
		end_idx = new_cur_idx;
		end_iilidx = new_cur_iilidx;
		if(!query_context->finished())
		{
			prevQueryPos(end_idx,end_iilidx);
		}
		return true;
	}
	return true;
}

void
AosIILHit::nextIndexFixErrorIdx(
		i64 &cur_idx,
		i64 &cur_iilidx,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	aos_assert(cur_idx >= 0 && cur_iilidx >=0);
	has_data = true;
	// single IIL
	if (isSingleIIL())
	{
		//1. index out of bound
		if (cur_iilidx > 0 || cur_idx > mNumDocs -1)
		{
			locateIdx(cur_idx,cur_iilidx,cur_docid,reverse,has_data,rdata);
			return;
		}
		//2. value changed
		if (mDocids[cur_idx] != cur_docid)
		{
			locateIdx(cur_idx,cur_iilidx,cur_docid,reverse,has_data,rdata);
			return;
		}
		//3. nothing changed		
		if (reverse)
		{
			prevQueryPos(cur_idx,cur_iilidx);
		}
		else
		{
			nextQueryPos(cur_idx,cur_iilidx);
		}
		return;
	}
	
	// root IIL
	if (cur_iilidx > mNumSubiils -1 ||
	   ((cur_iilidx == (mNumSubiils -1 )) && (cur_idx > (mNumEntries[mNumSubiils -1]-1))))
	{
		locateIdx(cur_idx,cur_iilidx,cur_docid,reverse,has_data,rdata);
		return;
	}

	if (cur_idx > (mNumEntries[cur_iilidx]-1))
	{
		locateIdx(cur_idx,cur_iilidx,cur_docid,reverse,has_data,rdata);
		return;
	}
	
	AosIILHitPtr subiil = getSubiilByIndex(cur_iilidx,rdata);
	if (!subiil)
	{
		// should not happen
		has_data = false;
		return;
	}
	if (!subiil->verifyIdx(cur_idx,cur_docid))
	{
		locateIdx(cur_idx,cur_iilidx,cur_docid,reverse,has_data,rdata);
		return;
	}

	if (reverse)
	{
		prevQueryPos(cur_idx,cur_iilidx);
	}
	else
	{
		nextQueryPos(cur_idx,cur_iilidx);
	}
	
	return;
}

bool
AosIILHit::locateIdx(
		i64 &cur_idx,
		i64 &cur_iilidx,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)	
{
	if (reverse)
	{
		// get the first doc which less than cur_docid backward
		if (cur_docid < mDocids[0])
		{
			cur_iilidx = 0;
			cur_idx = 0;
			has_data = false;
			return true;
		}
	}
	else // normal order
	{
		// get the first doc which greater than cur_docid
		u64 max_docid = mDocids[mNumDocs-1];
		if (isParentIIL()) max_docid = mMaxVals[mNumSubiils-1];
		if (cur_docid > max_docid)
		{
			cur_iilidx = 0;
			cur_idx = 0;
			has_data = false;
			return true;
		}
	}
	
	if (isSingleIIL())
	{
		cur_iilidx = 0;
		if (reverse)
		{
			cur_idx = firstLTRev(cur_docid);
			aos_assert_r(cur_idx >= 0,false);
		}
		else
		{
			cur_idx = firstGT(cur_docid);
			aos_assert_r(cur_idx <= mNumDocs -1,false);
		}
		return true;
	}
	// root iil
	AosIILHitPtr subiil;
	if (reverse)
	{
		cur_iilidx = firstSubLTRev(cur_docid);
		aos_assert_r(cur_iilidx >= 0,false);
		subiil = getSubiilByIndex(cur_iilidx,rdata);
		if (!subiil)
		{
			// should not happen
			has_data = false;
			return true;
		}
		cur_idx = subiil->firstLTRev(cur_docid);
	}
	else
	{
		cur_iilidx = firstSubGT(cur_docid);
		aos_assert_r(cur_iilidx <= mNumSubiils-1,false);
		subiil = getSubiilByIndex(cur_iilidx,rdata);
		if (!subiil)
		{
			// should not happen
			has_data = false;
			return true;
		}
		cur_idx = subiil->firstGT(cur_docid);
	}
	
	return true;
}			

i64 
AosIILHit::firstLTRev(const u64 &docid)
{
	if (mDocids[0] >= docid) return -1;
	i64 left = 0; 
	i64 right = mNumDocs-1;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (mDocids[nn] < docid) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}
			
i64 
AosIILHit::firstGT(const u64 &docid)
{
	if (mDocids[mNumDocs-1] <= docid) return -1;

	i64 left = 0;
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mDocids[nn] <= docid) left = nn; else right = nn;
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool
AosIILHit::verifyIdx(
		i64 &cur_idx,
		const u64 &cur_docid)
{
	return (cur_idx < mNumDocs && mDocids[cur_idx] == cur_docid);
}

bool
AosIILHit::checkDocs(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	AosQueryRsltObjPtr new_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->reset();
	u64 docid = 0;
	bool finished = false;
	query_rslt->nextDocid(docid,finished,rdata);
	//query_rslt->nextDocid();
	while(!finished && docid)
	{
		if (docExistPriv(docid,rdata))
		{
			new_rslt->appendDocid(docid);
		}
		//docid = query_rslt->nextDocid();
		aos_assert_r(query_rslt->nextDocid(docid,finished,rdata),false);
	}
	
	AosQueryRsltObj::exchangeContentStatic(query_rslt,new_rslt);
	return true;
}


bool	
AosIILHit::copyData(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &start_iilidx,
				const i64 &end_idx,
				const i64 &end_iilidx,
				const AosRundataPtr &rdata)
{
	if (query_rslt && query_bitmap)
	{
		return copyDataWithCheck(query_rslt,query_bitmap,query_context,start_idx,start_iilidx,end_idx,end_iilidx,rdata);
	}
	
	if (isSingleIIL())
	{
		return copyDataSingle(
					query_rslt,
					query_bitmap,
					query_context,
					start_idx,
					end_idx,
					rdata);
	}
	
	// rootIIL
	i64 start = 0;
	i64 end = 0;
	bool reverse = false;
	if (query_context->isReverse()) reverse = true;
	if (reverse)
	{
		query_rslt->setDataReverse(true);
	}
	for(i64 i = start_iilidx;i <= end_iilidx;i++)
	{
		start = 0;
		end = mNumEntries[i] -1;
		if (i == start_iilidx) start = start_idx;
		if (i == end_iilidx) end = end_idx;
		if (i == 0)
		{
			copyDataSingle(
				query_rslt,
				query_bitmap,
				query_context,
				start,
				end,
				rdata);
		}
		else
		{
			getSubiilByIndex(i,rdata)->copyDataSingle(
				query_rslt,
				query_bitmap,
				query_context,
				start,
				end,
				rdata);
		}
	}
		
	return true;
}

bool	
AosIILHit::copyDataSingle(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
 				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &end_idx,
				const AosRundataPtr &rdata)
{
	i64 size = end_idx - start_idx +1;
	aos_assert_r(size >= 0,false);
	if (size == 0)return true;

	if (query_rslt)
	{
		query_rslt->appendBlock(&(mDocids[start_idx]),size);
	}
	
	if (query_bitmap)
	{
		query_bitmap->appendDocids(&(mDocids[start_idx]),size);
	}
	return true;
}

bool	
AosIILHit::copyDataWithCheck(
				const AosQueryRsltObjPtr &query_rslt,
				const AosBitmapObjPtr &query_bitmap,
				const AosQueryContextObjPtr &query_context,
				const i64 &start_idx,
				const i64 &start_iilidx,
				const i64 &end_idx,
				const i64 &end_iilidx,
				const AosRundataPtr &rdata)
{
	bool reverse = query_context->isReverse();
	bool keep_search = true;
	if (isSingleIIL())
	{
		return copyDataSingleWithCheck(
					query_rslt,
					query_bitmap,
					query_context,
					start_idx,
					end_idx,
					reverse,
					keep_search,
					rdata);
	}

	i64 start = 0;
	i64 end = 0;
	if (reverse)
	{
		for(i64 i = end_iilidx;i >= start_iilidx;i--)
		{
			start = 0;
			end = mNumEntries[i] -1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;
			copyDataSingleWithCheck(
				query_rslt,
				query_bitmap,
				query_context,
				start,
				end,
				reverse,
				keep_search,
				rdata);
			if (query_rslt->getNumDocs() >= query_context->getPageSize())
			{
				return true;
			}
		}
		if (keep_search)
		{
			query_context->setIndex(0);
			query_context->setIILIndex(0);
			query_context->setFinished(true);
		}
	}
	else //normal order
	{
		for(i64 i = start_iilidx;i <= end_iilidx;i++)
		{
			start = 0;
			end = mNumEntries[i] -1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;
			copyDataSingleWithCheck(
				query_rslt,
				query_bitmap,
				query_context,
				start,
				end,
				reverse,
				keep_search,
				rdata);
			if (query_rslt->getNumDocs() >= query_context->getPageSize())
			{
				return true;
			}
		}
		if (keep_search)
		{
			query_context->setIndex(0);
			query_context->setIILIndex(0);
			query_context->setFinished(true);
		}
	}
	return true;
}

bool	
AosIILHit::nextQueryPos(i64 &idx,i64 &iilidx)
{
	if (iilidx < 0)
	{
		iilidx = 0;
		idx = 0;
		return true;
	}
	idx ++;
	if (isParentIIL() && idx >= mNumEntries[iilidx])
	{
		iilidx ++;
		idx = 0;
	}
	
	return true;
}

bool	
AosIILHit::prevQueryPos(i64 &idx,i64 &iilidx)
{
	if (isSingleIIL() && iilidx > 0)
	{
		iilidx = 0;
		idx = mNumDocs -1;
		return true;
	}

	idx --;

	if (isParentIIL() && idx < 0)
	{
		iilidx --;
		aos_assert_r(iilidx >= 0,false);
		idx = mNumEntries[iilidx] -1;
	}
	
	return true;
}

bool	
AosIILHit::copyDataSingleWithCheck(
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const i64 &start_idx,
			const i64 &end_idx,
			const bool reverse, 
			bool  keep_search,
			const AosRundataPtr &rdata)
{
	return false;
}			

i64
AosIILHit::firstSubLTRev(const u64 &docid)
{
	aos_assert_r(mSubiils, 0);
	for (i64 i=mNumSubiils-1; i >=0 ; i--)
	{
		if (mMinVals[i] < docid)
		{
			// Found the sub-iil
			return i;
		}
	}
	return 0;
}

i64
AosIILHit::firstSubGT(const u64 &docid)
{
	aos_assert_r(mSubiils, 0);
	for (i64 i=0; i < mNumSubiils; i++)
	{
		if (mMaxVals[i] > docid)
		{
			// Found the sub-iil
			return i;
		}
	}
	return mNumSubiils-1;
}


bool
AosIILHit::deleteIILPriv(
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return deleteIILSinglePriv(rdata);
	}
	
	// remove from IILMgr IDHash
	AosIILMgr::getSelf()->removeIILFromHashPublic(mIILID, mSiteid, rdata);

	AosIILHitPtr subiil;
	for(i64 i = 1;i < mNumSubiils;i++)
	{
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil, false);

		subiil->deleteIILSafe(true_delete, rdata);
		mSubiils[i] = 0;
	}
	return true;
}


bool	
AosIILHit::deleteIILSinglePriv(const AosRundataPtr &rdata)
{
	//1. remove from IILMgr IDHash
	AosIILMgr::getSelf()->removeIILFromHashPublic(mIILID, mSiteid, rdata);
	//2. remove from disk
	deleteFromLocalFilePriv(rdata);
	return true;
}


bool	
AosIILHit::preQueryPriv(
			const AosQueryContextObjPtr &query_context,
			const AosRundataPtr &rdata)
{
	if (isSingleIIL()) 
	{
		query_context->setTotalDocInRslt(mNumDocs);
	}
	else
	{
		i64 num = 0;
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			num += mNumEntries[i];
		}
		query_context->setTotalDocInRslt(num);
	}
	return true;	
}


void
AosIILHit::setSnapShotId(const u64 &snap_id)
{
	AOSLOCK(mLock);
	setSnapShotIdPriv(snap_id);
	AOSUNLOCK(mLock);
}


void
AosIILHit::setSnapShotIdPriv(const u64 &snap_id)
{
	aos_assert(mSnapShotId == 0 || snap_id == mSnapShotId);
	mSnapShotId = snap_id;

	if (isParentIIL())
	{
		AosIILHitPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->setSnapShotIdPriv(snap_id);
		}
	}
}


void
AosIILHit::resetSnapShotId()
{
	AOSLOCK(mLock);
	resetSnapShotIdPriv();
	AOSUNLOCK(mLock);
}


void
AosIILHit::resetSnapShotIdPriv()
{
	mSnapShotId = 0;	
	if (isParentIIL())
	{
		AosIILHitPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->resetSnapShotIdPriv();
		}
	}
}


bool
AosIILHit::resetIIL(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}
