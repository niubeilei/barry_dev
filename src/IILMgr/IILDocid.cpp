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
// Modification History:
// 	Created: 12/11/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILMgr/IILDocid.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IILMgr/IILMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "WordClient/WordClient.h"


static u32 sgMaxIILLen = 0;

AosIILDocid::AosIILDocid(
		const bool isPersis,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_Docid, isPersis),
mSubiils(0),
mMinVals(0),
mMaxVals(0)
{
	mTotalIILs[eAosIILType_Docid] ++;
}


AosIILDocid::AosIILDocid(
	const u64 &wordid,
	const u64 &iilid,
	const bool isPersis,
	const OmnString &iilname, 
	const bool iilmgrLocked,
	const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_Docid, isPersis, iilname),
mSubiils(0),
mMinVals(0),
mMaxVals(0)
{
	mTotalIILs[eAosIILType_Docid] ++;
}


AosIILDocid::AosIILDocid(
		const u64 &iilid, 
		AosBuff &buff, 
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_Docid, false),
mSubiils(0),
mMinVals(0),
mMaxVals(0)
{
	mTotalIILs[eAosIILType_Docid] ++;
	mIILID = iilid;
	mIsGood = loadFromFilePriv(buff, iilmgrLocked, rdata);
}



AosIILDocid::~AosIILDocid()
{
	AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_Docid] --;
	if (mNumSubiils > 0)
	{
		for (int i=0; i<mNumSubiils; i++)
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


bool        
AosIILDocid::queryPriv(
		const AosQueryRsltPtr& query_rslt,
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
		int num_docs = 0;
		for(int i=0; i<mNumSubiils; i++)
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
		AosIILDocidPtr subiil;
		for(int i=1; i<mNumSubiils; i++)
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
AosIILDocid::queryDirectPriv(const AosQueryRsltPtr& query_rslt,
							 const AosBitmapObjPtr& query_bitmap)
{
	if (mNumDocs <= 0)return true;

    // copy data
    bool rslt = copyDocidsPriv(query_rslt,query_bitmap, 0, mNumDocs-1);
    aos_assert_r(rslt,false);
    return true;	
}


bool
AosIILDocid::readCompIIL()
{
	return true;
}


bool
AosIILDocid::docExistSafe(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AOSLOCK(mLock);
	if (mNumSubiils > 0)
	{
		// this is the root iil and it has subiils, so we need
		// to judge which subill(or itself) we can use
		AosIILDocidPtr subiil = getSubiil(docid, rdata);
		aos_assert_rb(subiil, mLock, false);

		if (subiil == this)
		{
			rslt = docExistDirectPriv(docid);
			AOSUNLOCK(mLock);
			return rslt;
		}
	    rslt = subiil->docExistSafe(docid, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}
	rslt = docExistDirectPriv(docid);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILDocid::docExistDirectPriv(const u64 &docid)
{
	// This function uses binary search to determine whether
	// the given 'docid' exists in this IIL. The list must be
	// sorted (ascending).
	//
	if (!mDocids) 
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

	int idx = -1;
	bool rslt = getFirstDocIdx(docid, idx);
	return rslt;
}


bool
AosIILDocid::addDocPriv(
		const u64 &docid,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the
	// list. The list is sorted based on value, ascending.
	mIsDirty = true;
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		AosIILDocidPtr subiil;
		subiil = getSubiil(docid, rdata);
		aos_assert_r(subiil, false);
		if (subiil.getPtr() == this)
		{
			return insertDocid(docid, iilmgrLocked, rdata);
		}

		bool rslt = subiil->addDocSafe(docid, rdata);
		aos_assert_r(rslt, false);
		mIsDirty = true;
		return rslt;
	}

	bool rslt = insertDocid(docid, iilmgrLocked, rdata);

	aos_assert_r(rslt, false);
	mIsDirty = true;

	if (sgMaxIILLen < mNumDocs) 
	{
		sgMaxIILLen = mNumDocs;
	}
	
	return rslt;
}


bool
AosIILDocid::insertDocid(const u64 &docid, const bool iilmgrLocked, const AosRundataPtr &rdata)
{
	//1. find which subiil we need to insert into
	//2. judge whether we need to split the iil
	//3. find out which iil we need to insert into after split
	//4. insert it
	//5. update max/min value

	mIsDirty = true;
	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILDocidPtr subiil = 0;
		bool rslt = splitListPriv(iilmgrLocked, subiil, rdata);
		aos_assert_r(rslt, false);
		// judge whether we need to insert the docid into the sub iil
		aos_assert_r(subiil, false);
		aos_assert_r(mNumDocs > 0, false);
		if (docid > mDocids[mNumDocs-1])
		{
			return subiil->insertDocid(docid, iilmgrLocked, rdata);
		}
	}

	u32 idx = 0;
	int left = 0;
	int right = mNumDocs-1;

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
		 	if (mNumDocs >= mMemCap)
		 		aos_assert_r(expandMem(), false);
			int numToMove = mNumDocs - idx - 1;
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
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
				mDocids[idx+1] = docid;
				mNumDocs++;
				idx ++;
				goto finished;
			}

			if (docid < mDocids[idx+1] || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
				int numToMove = mNumDocs - idx - 1;
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
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
		 		memmove(&mDocids[1], mDocids, mNumDocs << 3);
		 		mDocids[0] = docid;
				mNumDocs++;
				goto finished;
			}

			if (mDocids[idx-1] < docid || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
				int numToMove = mNumDocs - idx;
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

	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILDocidPtr rootiil = (AosIILDocid*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
	}

	return true;
}

bool
AosIILDocid::insertDocidSeq(
		const u64 &docid, 
		int &iilidx,
		int &theidx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	//1. find which subiil we need to insert into
	//2. judge whether we need to split the iil
	//3. find out which iil we need to insert into after split
	//4. insert it
	//5. update max/min value

	mIsDirty = true;
	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILDocidPtr subiil = 0;
		bool rslt = splitListPriv(iilmgrLocked, subiil, rdata);
		aos_assert_r(rslt, false);
		// judge whether we need to insert the docid into the sub iil
		aos_assert_r(subiil, false);
		aos_assert_r(mNumDocs > 0, false);
		if (docid > mDocids[mNumDocs-1])
		{
			iilidx ++;
			return subiil->insertDocidSeq(docid, iilidx, theidx, iilmgrLocked, rdata);
		}
	}
	
	int idx = 0;
	int left = 0;
	int right = mNumDocs-1;
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
		 	if (mNumDocs >= mMemCap)
		 		aos_assert_r(expandMem(), false);
			int numToMove = mNumDocs - idx - 1;
			memmove(&mDocids[idx+2], &mDocids[idx+1], (numToMove << 3));
			mDocids[idx+1] = docid;
			mNumDocs++;
			idx++;
			goto finished;
		}

		if (mDocids[idx] < docid) 
		{
			if (idx == (int)mNumDocs-1)
			{
				// Append the docid
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
				mDocids[idx+1] = docid;
				mNumDocs++;
				idx ++;
				goto finished;
			}

			if (docid < mDocids[idx+1] || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
				int numToMove = mNumDocs - idx - 1;
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
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
		 		memmove(&mDocids[1], mDocids, mNumDocs << 3);
		 		mDocids[0] = docid;
				mNumDocs++;
				goto finished;
			}

			if (mDocids[idx-1] < docid || left == right)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
		 		if (mNumDocs >= mMemCap)
			 		aos_assert_r(expandMem(), false);
				int numToMove = mNumDocs - idx;
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

	theidx = idx;
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILDocidPtr rootiil = (AosIILDocid*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
	}

	return true;
}


AosBuffPtr
AosIILDocid::getBodyBuffProtected() const
{
	// This function is called when we need to save the IIL.
	AosBuffPtr buff = OmnNew AosBuff(5000, 5000);
	aos_assert_r(buff, 0);

	// the compiil of the subiil may be with no docid.
//	aos_assert_r(mDocids, false);

	buff->setU32(mNumDocs);
	//*buff << mNumDocs;
	for (u32 i=0; i<mNumDocs; i++)
	{
		//*buff << mDocids[i];
		buff->setU64(mDocids[i]);
	}
	aos_assert_r(mNumSubiils >= 0,false);
	//*buff << mNumSubiils;
	//*buff << mIILIdx;
	buff->setInt(mNumSubiils);
	buff->setInt(mIILIdx);

	if (isParentIIL())
	{
		for (int i=0; i<mNumSubiils; i++)
		{
			//*buff << mMinVals[i]
			//	<< mMaxVals[i]
			//	<< mIILIds[i]
			//	<< mNumEntries[i];
			buff->setU64(mMinVals[i]);
			buff->setU64(mMaxVals[i]);
			buff->setU64(mIILIds[i]);
			buff->setInt(mNumEntries[i]);
		}
	}

	return buff;
}


bool
AosIILDocid::setContentsProtected(AosBuffPtr &buff, const bool iilmgrLocked, const AosRundataPtr &rdata)
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
	aos_assert_r(prepareMemoryForReloading2(iilmgrLocked), false);

	// 1. Retrieve mDocids
//	aos_assert_r(setDocidContents(buff), false);
	mNumDocs = buff->getU32(0);
	aos_assert_r(mNumSubiils < eMaxSubiils,false);
	aos_assert_r(mNumDocs >= 0, false);
	for(int i = 0;i < (int)mNumDocs;i++)
	{
		mDocids[i] = buff->getU64(0);
	}

	// 2. Retrieve Subiil Index Data, if needed
	mNumSubiils = buff->getInt(0);
	
	if (mNumSubiils < 0 || mNumSubiils > eMaxSubiils)
	{
		// alarm
		OmnAlarm << "mNumSubiils wrong:" << mNumSubiils << enderr;

		mNumSubiils = 0;
		return false;
	}

	mIILIdx = buff->getInt(0);

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

		for (int i=0; i<mNumSubiils; i++)
		{
			mMinVals[i] = buff->getU64(0);
			mMaxVals[i] = buff->getU64(0);

			mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			mNumEntries[i] = buff->getInt(-1);
			if (mNumEntries[i] < 0)
			{
				OmnAlarm << "Failed to get mNumEntries[i]:i = " 
					<< i << " , mNumEntries[i] = " 
					<< mNumEntries[i] << enderr;
				return false;
			}
		}
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


extern int sgNumIILRefs;

void
AosIILDocid::resetSafe(const u64 &wordid, const u64 &iilid,const bool iilmgrLocked, const AosRundataPtr &rdata)
{
	//1. return all the subiil back
	//2. clean all the information about subiil
	AOSLOCK(mLock);
	if (isParentIIL())
	{
		bool returned = false;
		returnSubIILsPriv(false,returned, rdata);
		aos_assert_l(returned, mLock);
	}

	if (mNumEntries)
	{
		delete[]		mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
	{
		delete[]		mIILIds;
		mIILIds = 0;
	}

	if (mSubiils)
	{
		delete[]		mSubiils;
		mSubiils = 0;
	}

	if (mMinVals)
	{
		delete[]		mMinVals;
		mMinVals = 0;
	}

	if (mMaxVals)
	{
		delete[]		mMaxVals;
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



int			
AosIILDocid::getTotalSafe(
		const OmnString &value, 
		const AosOpr opr)
{
	OmnShouldNeverComeHere;
	return -1;
}


int			
AosIILDocid::getTotalSafe(
		const u64 &value, 
		const AosOpr opr)
{
	OmnShouldNeverComeHere;
	return -1;
}


bool
AosIILDocid::expandMem()
{
	int newcapacity = mMemCap + mExtraDocids;
	int newsize = newcapacity << 3;
	char *mem = getMemory(newsize, (char *)mDocids, (mNumDocs << 3));
	aos_assert_r(mem, false);
	mDocids = (u64 *)mem;
	mMemCap = newcapacity;
	newsize = newcapacity << 2;
	aos_assert_r(mem, false);
	return true;
}


bool		
AosIILDocid::removeUniqDocSafe(const u64 &docid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILDocid::removeUniqDocSafe(
		const u64 &docid,
		int &numFound,
		const AosRundataPtr &rdata)
{
	// This function should be called when the iil is the root iil, or the single iil
	// If it is single IIL, just call the direct function.
	// If it is root IIL, follow the steps:
	//1. find the first doc
	//2. remove the docid from this subiil
	//3. if we need to continue to delete in the next subiil, go next,
	bool rslt = false;
	aos_assert_r(isParentIIL()||isSingleIIL(), false);
	numFound = 0;

	bool hasmore = true;
	if (isSingleIIL())
	{
		return removeUniqDocDirectPriv(docid, numFound, hasmore, rdata);
	}

	aos_assert_r(isParentIIL(),false);
	mIsDirty = true;
	//1. find the first doc
	int curIndex = getSubiilIndex(docid);
	AosIILDocidPtr curSub = getSubiilByIndex(curIndex, rdata);
	aos_assert_r(curSub, false);

	int numFoundFromSub = 0;
	//2. remove the docid from this subiil
	//3. if we need to continue to delete in the next subiil, go next,
	//until we remove all.
	while(hasmore)
	{
		if (curSub->getNumDocs() > 0)
		{
			rslt = curSub->removeUniqDocDirectPriv(docid, numFoundFromSub, hasmore, rdata);
			aos_assert_r(rslt,false);
			numFound += numFoundFromSub;
			if (!hasmore)
			{
				break;
			}
		}
		// point to the next, if it is the last one, break;
		if (curIndex >= mNumSubiils-1)
		{
			break;
		}
		curIndex ++;
		curSub = getSubiilByIndex(curIndex, rdata);
		aos_assert_r(curSub, false);
	}
	mIsDirty = true;
	return true;
}

bool
AosIILDocid::removeUniqDocDirectPriv(
		const u64 &docid,
		int &numFound,
		bool &hasmore,
		const AosRundataPtr &rdata)
{
	// It assumes that 'docid' is unique, which means that there should
	// be just one entry in this iil. While this is what the caller
	// assumes, this function WILL check whether there is indeed only
	// one. If there are multiple, it will set 'numFound' to that 
	// number. 
	int dd = -1;
	numFound = 0;
	hasmore = false;
	bool rslt = getFirstDocIdx(docid, dd);
	aos_assert_r(rslt, false);

	if (mDocids[mNumDocs-1] == docid)
	{
		hasmore = true;
	}

	if (dd > 0) aos_assert_r(mDocids[dd-1] != docid, false);
	numFound = 1;
	if (dd < (int)mNumDocs-1 && mDocids[dd+1] == docid) 
	{
		// There are multiple entries. 
		numFound = 2;
		for (u32 i=dd+2; i<mNumDocs; i++)
		{
			if (mDocids[i] != docid) break;
			numFound++;
		}
	}
	memmove(&mDocids[dd], &mDocids[dd+1], (mNumDocs-dd-1) << 3);
	mNumDocs--;
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILDocidPtr rootiil = (AosIILDocid*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
	}

	mIsDirty = true;
	return true;
}


bool
AosIILDocid::removeDocPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	bool rslt = false;
	aos_assert_r(isParentIIL()||isSingleIIL(), false);

	if (isSingleIIL())
	{
		return removeDocDirectPriv(docid, rdata);
	}

	mIsDirty = true;
	// this is the root iil, find the sub and
	// call the direct func
	AosIILDocidPtr curSub = getSubiil(docid, rdata);
	aos_assert_r(curSub, false);

	rslt = curSub->removeDocDirectPriv(docid, rdata);
	aos_assert_r(rslt, false);

	mIsDirty = true;
	return true;
}

bool
AosIILDocid::removeDocSeqPriv(
		const u64 &docid,
		int &iilidx,
		int &idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	aos_assert_r(isParentIIL()||isSingleIIL(), false);

	if (!isParentIIL())
	{
		return removeDocSeqDirectPriv(docid,iilidx,idx, rdata);
	}

	mIsDirty = true;
	// this is the root iil, find the sub and
	// call the direct func
	int newiilidx = getSubiilIndexSeq(docid,iilidx);
	if (newiilidx != iilidx)
	{
		idx = -1;
	}
	iilidx = newiilidx;
	
	AosIILDocidPtr curSub = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(curSub, false);

	if (curSub.getPtr() == this)
	{
		rslt = removeDocSeqDirectPriv(docid,iilidx,idx, rdata);
	}
	else
	{
		rslt = removeDocSeqSafe(docid, iilidx, idx, iilmgrLocked, rdata);
	}
	aos_assert_r(rslt, false);

	mIsDirty = true;
	return true;
}

bool
AosIILDocid::removeDocDirectPriv(const u64 &docid, const AosRundataPtr &rdata)
{
	int idx = -1;
	if (!getFirstDocIdx(docid, idx))
	{
		OmnAlarm << "docid: " << docid << ":" << idx << enderr;
		return false;
	}

	if (idx < (int)mNumDocs-1)
	{
		memmove(&mDocids[idx], &mDocids[idx+1], (mNumDocs-idx-1) << 3);
	}
	mNumDocs--;
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILDocidPtr rootiil = (AosIILDocid*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
		if (mNumDocs < mMinSubIILSize)
		{
			rootiil->mergeSubiilPriv(true,mIILIdx, rdata);
		}
	}
	mIsDirty = true;
	
	return true;

}


bool
AosIILDocid::removeDocSeqDirectPriv(
		const u64 &docid,
		int &iilidx,
		int &idx,
		const AosRundataPtr &rdata)
{
	if (!getFirstDocIdx(docid, idx))
	{
		OmnAlarm << "docid: " << docid << ":" << idx << enderr;
		return false;
	}

	if (idx < (int)mNumDocs-1)
	{
		memmove(&mDocids[idx], &mDocids[idx+1], (mNumDocs-idx-1) << 3);
	}
	mNumDocs--;
	if (isParentIIL()||isLeafIIL())
	{
		aos_assert_r(mRootIIL,false);
		AosIILDocidPtr rootiil = (AosIILDocid*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, true, true, rdata);// now we update all the max and min
		if (mNumDocs < mMinSubIILSize)
		{
			rootiil->mergeSubiilPriv(true,mIILIdx, rdata);
			iilidx = -1;
			idx = -1;
		}
	}
	mIsDirty = true;
	
	return true;

}


bool
AosIILDocid::getFirstDocIdx(const u64 &docid, int &theidx)
{
	// The caller should have locked the class. It retrieves 
	// the index of the 'docid'. If not found, it returns false. 
	// Otherwise, the index is stored in 'theidx' and returns true.
	// Note that there can be multiple docs with the same
	// docids. Same docids are stored consecutively. If there are
	// multiple same docids, it returns the first one.
	aos_assert_r(mDocids, false);
	aos_assert_r(mNumDocs > 0, false);

	int idx = 0;
	int left = (theidx >=0)?theidx:0; 
	int right = mNumDocs-1;

	if (mDocids[left] == docid)
	{
		idx = left;
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
			int prev = idx--;
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

			theidx = (u32)(idx+1);
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
AosIILDocid::adjustMemoryProtected()
{
	u32 newsize = ((mNumDocs + mExtraDocidsInit) << 3);
	u32 oldsize = (mMemCap << 3);
	if (newsize < oldsize && newsize + eMemShreshold >= oldsize)
	{
		return true;
	}

	AosIIL::adjustMemoryProtected();
	return true;
}


bool		
AosIILDocid::checkDocSafe( 
		const AosOpr opr,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILDocid::checkDocSafe(
		const AosOpr opr,
		const u32 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILDocid::firstDoc1(
		int &idx, 
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
		u64 &docid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool		
AosIILDocid::firstDoc1(
		int &idx, 
		const bool reverse, 
		const AosOpr opr,
		const u32 &value,
		u64 &docid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILDocid::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
//	AOSLOCK(mLock);
	if (isParentIIL())
	{
		AosIILDocidPtr subiil = 0;
		for(int i = 1;i < mNumSubiils;i++)
		{
			aos_assert_r(mMinVals[i] >= mMaxVals[i-1],false);
		}
		for(int i = 0;i < mNumSubiils;i++)
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
//	static AosIILObjPtr lsTestIIL = OmnNew AosIILDocid();
//	static OmnMutex lsLock;
//
//	lsLock.lock();
//	bool rslt = AosIIL::saveSanityCheck(lsTestIIL);
//	lsLock.unlock();
//	return rslt;
	return true;
}

/*
int
AosIILDocid::resetSubiilsPriv(AosIILObjPtr subiils[AosIIL::eMaxSubiils])
{
	OmnShouldNeverComeHere;
	return -1;
}
*/

u64
AosIILDocid::getMinDocid() const
{
	aos_assert_r(mNumDocs > 0 && mDocids, AOS_INVDID);
	return mDocids[0];
}


u64
AosIILDocid::getMaxDocid() const
{
	aos_assert_r(mNumDocs > 0 && mDocids, AOS_INVDID);
	return mDocids[mNumDocs-1];
}


bool
AosIILDocid::resetData()
{
	mCompIIL = 0;
	return true;
}

bool
AosIILDocid::splitListPriv(
		const bool iilmgrLocked, 
		AosIILDocidPtr &subiilDocid, 
		const AosRundataPtr &rdata)
{
	// It cuts the current list in two. The first half stays 
	// with the current list and the second half is in a new
	// IIL list.
	// 1. Check whether it can create more subiils
	aos_assert_r(mNumSubiils < eMaxSubiils-1, false);
	subiilDocid = 0;
	// 2. Create the subiil

	AosIILObjPtr subiil = AosIILMgrSelf->createSubiilSafe(
			eAosIILType_Docid, mIsPersis, iilmgrLocked, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->getIILType() == eAosIILType_Docid, false);
	AosIILDocidPtr newsubiil = (AosIILDocid*)subiil.getPtr();
	
	// 3. Set mRootIIL
	if (mRootIIL.isNull())
	{
		mRootIIL = this;
		mIILIdx = 0;
	}
	newsubiil->setRootIIL(mRootIIL);
	newsubiil->setIILIdx(mIILIdx+1);

	// 4. Initialize the new subiil
	u32 startidx = mNumDocs / 2;
	const u32 len = mNumDocs - startidx;
	aos_assert_r(mRootIIL->getIILType() == eAosIILType_Docid, false);
	AosIILDocidPtr rootiil = (AosIILDocid*)mRootIIL.getPtr();


	aos_assert_r(mIILIdx >= 0, false);

	aos_assert_r(newsubiil->initSubiil((u64*)&mDocids[startidx],
									   len,
									   mIILIdx+1,
									   rootiil,
									   iilmgrLocked),
									   false);

	// 5. Shrink this IIL
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	aos_assert_r(mNumDocs < mMaxNormIILSize, false);

	// 6. Add the subiil
	AosIILDocidPtr thisptr(this, false);
	bool rslt = false;
	rslt = rootiil->addSubiil(thisptr, newsubiil, rdata);
	aos_assert_r(rslt, false);
	saveSanityCheckProtected(rdata);

	newsubiil->setDirty(true);
	if (!newsubiil->saveToFileSafe(false, rdata))
	{
		OmnAlarm << "Failed to save IIL: " << enderr;
	}
	subiilDocid = (AosIILDocid*)subiil.getPtr();
	return true;
}


bool 
AosIILDocid::initSubiil(
		u64 *docids, 
		const int numDocs,
		const int subiilid, 
		const AosIILDocidPtr &rootiil, 
		const bool iilmgrLocked) 
{
	aos_assert_r(mIILType == eAosIILType_Docid, false);
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
    mIILDiskSize = 0;
    mOffset = 0;
    mSeqno = 0;

	// Prepare the memory
	aos_assert_r(prepareMemoryForReloading2(iilmgrLocked), false);

	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool
AosIILDocid::addSubiil(
		const AosIILDocidPtr &crtsubiil,
		const AosIILDocidPtr &newsubiil,
		const AosRundataPtr &rdata)
{
	// It inserts the new subiil 'newsubiil' after 'crtsubiil'. This function
	// must be called by the root IIL.
	aos_assert_r(mIILIdx == 0, false);
	aos_assert_r(mNumSubiils < eMaxSubiils-1, false);
	if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
	int iilidx = crtsubiil->getIILIdx();

	// 1. Insert the new subiil in mSubiils[]
	if (iilidx < mNumSubiils-1)
	{
		// Make room for the new subiil
		memmove(&mSubiils[iilidx+2], &mSubiils[iilidx+1], 
			sizeof(AosIILDocidPtr) * (mNumSubiils-iilidx-1));
		memset(&mSubiils[iilidx+1], 0,
				sizeof(AosIILDocidPtr));

		memmove(&mMinVals[iilidx+2], &mMinVals[iilidx+1], 
			sizeof(u64) * (mNumSubiils-iilidx-1));
		memmove(&mMaxVals[iilidx+2], &mMaxVals[iilidx+1], 
			sizeof(u64) * (mNumSubiils-iilidx-1));
		memmove(&mIILIds[iilidx+2], &mIILIds[iilidx+1], 
			sizeof(u64) * (mNumSubiils-iilidx-1));
		memmove(&mNumEntries[iilidx+2], &mNumEntries[iilidx+1], 
			sizeof(int) * (mNumSubiils-iilidx-1));

		// Modify the iil index
		for (int i=iilidx+2; i<mNumSubiils; i++)
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
	bool rslt = false;

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
	for(int i = iilidx; i < mNumSubiils;i++)
	{
		if (mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}

	rslt = updateIndexData(iilidx,true,true, rdata);
	aos_assert_r(rslt, false);
	rslt = updateIndexData(iilidx+1,true,true, rdata);
	aos_assert_r(rslt, false);

	aos_assert_r(mSubiils[0], false);

	setDirty(true);
	crtsubiil->setDirty(true);
	newsubiil->setDirty(true);
	return true;
}

bool
AosIILDocid::createSubiilIndex()
{

//	aos_assert_r(mNumSubiils == 0, false);
	aos_assert_r(!mSubiils, false);
	mSubiils = OmnNew AosIILDocidPtr[eMaxSubiils];
	aos_assert_r(mSubiils, false);

	aos_assert_r(!mMinVals, false);
	mMinVals = OmnNew u64[eMaxSubiils];
	aos_assert_r(mMinVals, false);
	memset(mMinVals, 0, sizeof(char*) * eMaxSubiils);

	aos_assert_r(!mMaxVals, false);
	mMaxVals = OmnNew u64[eMaxSubiils];
	aos_assert_r(mMaxVals, false);
	memset(mMaxVals, 0, sizeof(char*) * eMaxSubiils);

	aos_assert_r(!mIILIds, false);
	mIILIds = OmnNew u64[eMaxSubiils];
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * eMaxSubiils);

	aos_assert_r(!mNumEntries, false);
	mNumEntries = OmnNew int[eMaxSubiils];
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(int) * eMaxSubiils);

	return true;
}


bool
AosIILDocid::removeSubiilIndex()
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

const u64
AosIILDocid::getMinValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mDocids[0];
}


const u64
AosIILDocid::getMaxValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mDocids[mNumDocs-1];
}


bool
AosIILDocid::prepareMemoryForReloading2(const bool iilmgrLocked)
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

AosIILDocidPtr
AosIILDocid::getSubiil(const u64 value, const AosRundataPtr &rdata)
{
	int index = getSubiilIndex(value);
	return getSubiilByIndex(index, rdata);
}

int
AosIILDocid::getSubiilIndex(const u64 value)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
	for (int i=0; i < mNumSubiils; i++)
	{
		if (mMaxVals[i] >= value)
		{
			// Found the sub-iil
			return i;
		}
	}
	return mNumSubiils-1;
}

int	 
AosIILDocid::getSubiilIndexSeq(const u64 value,const int iilidx)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
	for (int i=iilidx; i < mNumSubiils; i++)
	{
		if (mMaxVals[i] >= value)
		{
			// Found the sub-iil
			return i;
		}
	}
	return mNumSubiils-1;
}

AosIILDocidPtr
AosIILDocid::getSubiilByIndex(const int idx, const AosRundataPtr &rdata)
{
	// It retrieves the idx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil.
	//
	// The function assumes 'mLock' should have been locked.
	aos_assert_r(mIILIdx == 0, 0);
	if (mRootIIL)
	{
		aos_assert_r(mRootIIL.getPtr() == this, 0);
	}

	if (idx == 0)
	{
		if (!mRootIIL) mRootIIL = this;
		return this;
	}

	aos_assert_r(idx > 0 && idx < mNumSubiils, 0);
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILDocidPtr[eMaxSubiils];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[idx]) return mSubiils[idx];

	// Retrieve an IIL from IILMgr
	aos_assert_r(mIILIds[idx], 0);

	// Chen Ding, 05/05/2011
	// bool status;
	// AosIILObjPtr iil = AosIILMgrSelf->loadIILByIDSafe(mIILIds[idx],
	// 		AOS_INVWID, eAosIILType_Docid, status);
	AosIILType type = eAosIILType_Docid;
	AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(mIILIds[idx], type, rdata);	
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[idx] << ":" << type << enderr;
		return 0;
	}

	if (!iil || iil->getIILType() != eAosIILType_Docid)
	{
		OmnAlarm << "Not a docid IIL: "
			<< mSubiils[idx]->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}
	mSubiils[idx] = (AosIILDocid*)iil.getPtr();
	mSubiils[idx]->mRootIIL = this;
	mSubiils[idx]->mIILIdx = idx;

	aos_assert_r(mSubiils[0], 0);
	return mSubiils[idx];
}

bool
AosIILDocid::setMaxVals(int index,const u64 value)
{
	mMaxVals[index] = value;
	return true;
}

bool
AosIILDocid::setMinVals(int index,const u64 value)
{
	mMinVals[index] = value;
	return true;
}


bool
AosIILDocid::saveSubIILToFileSafe(const bool forcesave, const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mNumSubiils > 0 && mIILIdx == 0)
	{
		// this iil is the root iil of the list
		for(int i = 1;i < mNumSubiils;i++)
		{
			if (mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToFileSafe(forcesave, rdata);
				aos_assert_r(rslt,false);
			}
		}
	}
	return true;
}

bool
AosIILDocid::updateIndexData(
		const int idx,
		const bool changeMax,
		const bool changeMin,
		const AosRundataPtr &rdata)
{
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	aos_assert_r(isParentIIL(), false);
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < mNumSubiils, false);
	aos_assert_r(mSubiils[idx], false);

	AosIILDocidPtr subiil = mSubiils[idx];

	mNumEntries[idx] = subiil->getNumDocs();

	if (mNumEntries[idx] > 0 &&
	   changeMax)
	{
		mMaxVals[idx] = subiil->getMaxValue();
	}

	if (mNumEntries[idx] > 0 &&
	   changeMin)
	{
		mMinVals[idx] = subiil->getMinValue();
	}
	setDirty(true);
	return true;
}

bool
AosIILDocid::returnSubIILsPriv(
		const bool iilmgrLocked,
		bool &returned,
		const AosRundataPtr &rdata)
{
	returned = true;

	if (mNumSubiils <= 0) return true;
	if (!mSubiils[0])
	{
		OmnAlarm << "Root iil is null" << enderr;
	}

	AosIILDocidPtr subiil = 0;
	bool subIILReturned = false;
	bool rslt = false;

	// return all sub IIL except itself
	for(int i = 1;i < mNumSubiils;i++)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil != this,false);
		if (subiil.notNull())
		{
			rslt = AosIILMgrSelf->returnIILDocidPublic(subiil, iilmgrLocked, subIILReturned, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}

//int
//AosIILDocid::retrieveNewIILIDFromMgr()
//{
//	mIILID = AosIILMgrSelf->newIILID();
//	mIsDirty = true;
//	return mIILID;
//}

bool
AosIILDocid::resetSubIILInfo(const AosRundataPtr &rdata)
{
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
	bool returned = false;
	if (mSubiils)
	{
		for(int i = 1;i < mNumSubiils;i++)
		{
			if (mSubiils[i])
			{
				AosIILMgrSelf->returnIILDocidPriv(mSubiils[i], true, returned, rdata);
				mSubiils[i] = 0;
			}
		}

		OmnDelete [] mSubiils;
		mSubiils = 0;
	}

	return true;
}


u64
AosIILDocid::getDocIdSafe1(int &idx,int &iilidx, const AosRundataPtr &rdata)
{
	int did = 0;
	AOSLOCK(mLock);
	did = getDocIdPriv1(idx, iilidx, rdata);
	AOSUNLOCK(mLock);
	return did;
}

u64
AosIILDocid::getDocIdPriv1(int &idx, int &iilidx, const AosRundataPtr &rdata)
{
	if (!isParentIIL())
	{
		return getDocIdPriv(idx);
	}

	AosIILDocidPtr subiil = getSubiil(iilidx, rdata);
	u64 docid = 0;
	while (subiil)
	{
		if (subiil == this) docid = subiil->getDocIdPriv(idx);
		else docid = subiil->getDocIdSafe(idx);
//		docid = subiil->getDocIdSafe(idx);
		if (docid > 0)
		{
			return docid;
		}
		if (iilidx >= mNumSubiils -1)
		{
			return 0;
		}
		iilidx++;
		idx = 0;
		subiil = getSubiil(iilidx, rdata);
	}
	return 0;
}


// Chen Ding, 12/07/2010
bool
AosIILDocid::getDocidBody(const AosBuffPtr &buff) const
{
	for (u32 i=0; i<mNumDocs; i++)
	{
		//*buff << mDocids[i];
		buff->setU64(mDocids[i]);
	}
	return true;
}


bool 		
AosIILDocid::mergeSubiilPriv(const bool iilmgrLocked,const int iilidx, const AosRundataPtr &rdata)
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
	bool rslt = false;

	//1. If there is a iil around, two iils contains more than 	
	//   mMaxNormIILSize docs, merge some docs from that iil to this one.
	AosIILDocidPtr iil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iil,false);
	int numdoc1 = iil->getNumDocs();
	int numdoc2 = 0; // num doc prev
	int numdoc3 = 0; // num doc next
	int merge_size = mMaxNormIILSize/2 + mMinSubIILSize;
	if (iilidx > 0)
	{
		AosIILDocidPtr iil2 = getSubiilByIndex(iilidx - 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc2 = iil2->getNumDocs();

		//if (numdoc1 + numdoc2 >= (int)mMaxNormIILSize)
		if (numdoc1 + numdoc2 >= merge_size)
		{
			// iilidx-1 and iilidx are too 
			// move some docs from iil2 to iil1
			int nummove = (numdoc1 + numdoc2)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilForwardPriv(iilmgrLocked,iilidx -1,nummove, rdata);
			aos_assert_r(rslt, false);
			return rslt;
		}
	}
	
	if (iilidx < mNumSubiils - 1)
	{
		AosIILDocidPtr iil2 = getSubiilByIndex(iilidx + 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc3 = iil2->getNumDocs();

		//if (numdoc1 + numdoc3 >= (int)mMaxNormIILSize)
		if (numdoc1 + numdoc3 >= merge_size)
		{
			int nummove = (numdoc1 + numdoc3)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilBackwardPriv(iilmgrLocked,iilidx + 1,nummove, rdata);
			aos_assert_r(rslt, false);
			return true;
		}
	}
	
	// if it is the first docid, merge the 2nd one to the first one.		
	if (iilidx == 0)
	{
		// merge iil[1] to iil[0]
		rslt = mergeSubiilBackwardPriv(iilmgrLocked,1,numdoc3, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	

	// if it is the last docid, merge this iil to the one before.		
	if (iilidx == mNumSubiils -1)
	{
		rslt = mergeSubiilBackwardPriv(iilmgrLocked,iilidx,numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	
	
	// The iil is in the middle
	if (numdoc2 < numdoc3)
	{
		// merge to the one before
		rslt = mergeSubiilBackwardPriv(iilmgrLocked,iilidx,numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;		
	}
	else
	{
		//merge to the next one.
		rslt = mergeSubiilForwardPriv(iilmgrLocked,iilidx,numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;		
	}

	OmnShouldNeverComeHere;
	return true;
}

bool 		
AosIILDocid::mergeSubiilForwardPriv(
		const bool iilmgrLocked,
		const int iilidx,
		const int numDocToMove,
		const AosRundataPtr &rdata)
{
	// This function is used by root iil, it moves [numDocToMove] docs from 
	// subiil[iilidx] to subiil[iilidx+1]. If the original subiil mNumDocs finally 
	// turns to 0, we'll call another function to remove the subiil 
	// from the subiil list. To remove all the entries from the root 
	// iil(the first subiil) is forbidden. We'll check it before.
	
	// 1. validation(params checking)
	aos_assert_r(isParentIIL(),false);
	aos_assert_r(iilidx < mNumSubiils -1,false);
	aos_assert_r(mNumEntries[iilidx] >= numDocToMove,false);		
	if (iilidx == 0)
	{
		aos_assert_r(mNumEntries[iilidx] >  numDocToMove,false);		
	}

	// 2. move entries from a to b
	AosIILDocidPtr crtiil = getSubiilByIndex(iilidx, rdata);
	AosIILDocidPtr nextiil = getSubiilByIndex(iilidx+1, rdata);
	aos_assert_r(crtiil, false);
	aos_assert_r(nextiil, false);
		
	bool rslt = false;
	u64*    docidPtr = 0;
	// get pointers
	int offset = crtiil->getNumDocs() - numDocToMove;
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
		removeSubiil(iilidx,iilmgrLocked, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true, rdata);
	}
	return true;	
}


bool 		
AosIILDocid::mergeSubiilBackwardPriv(
		const bool iilmgrLocked,
		const int iilidx,
		const int numDocToMove,
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
//	aos_assert_r((int)mNumDocs >= numDocToMove,false);
	aos_assert_r(iilidx <= mNumSubiils -1,false);
	aos_assert_r(iilidx > 0,false);
	
	// 2. move entries from a to b
	AosIILDocidPtr previil = getSubiilByIndex(iilidx-1, rdata);
	AosIILDocidPtr crtiil =  getSubiilByIndex(iilidx, rdata);
	aos_assert_r(previil, false);
	aos_assert_r(crtiil, false);
		
	bool rslt = false;
	u64*    docidPtr = 0;
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
		removeSubiil(iilidx,iilmgrLocked, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true, rdata);
	}
	return true;	
}

bool		
AosIILDocid::getDocidPtr(u64*    &docidPtr,
					   const int offset)
{
	aos_assert_r(mDocids, false);
	aos_assert_r(offset >= 0, false);
	aos_assert_r(offset < (int)mNumDocs, false);
	docidPtr = &(mDocids[offset]);
	
	return true;
}


bool 
AosIILDocid::appendDocToSubiil(
		u64 *docids, 
		const int numDocs,
		const bool addToHead) 
{
	// Notice that we use memcpy to copy string point list to the new subiil, 
	// So we can NOT use delete in the original subiil, instead, we use 
	// memset to clear that memory.
	aos_assert_r(mIILType == eAosIILType_Docid, false);

	// ModifyNumDocs
	aos_assert_r(mNumDocs + numDocs < mMaxNormIILSize, false);
	if (numDocs == 0)
	{
		return true;
	}
	int newNumDocs = mNumDocs + numDocs; 
	aos_assert_r(newNumDocs > 0,false);	
	//1. enlarge mValues/mDocids space to hold all the values and docids, in the same time, 
	// move the original data and new data to the new place.	
	// if the number of the new list is less than mMemCap, do nothing to 		
	
	if (newNumDocs < (int)mMemCap)
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
AosIILDocid::removeDocFromSubiil(const int numRemove, const bool delFromHead)
{
	// Notice:
	// 1. When we move docs to another subiil, we use memcpy to copy string point list. 
	// 	  So we can NOT use delete in the original subiil, instead, we use 
	// 	  memset to clear that memory.
	// 2. We do not shrink memory when deleting elements.
	aos_assert_r(numRemove <= (int)mNumDocs, false);
	int newsize = mNumDocs - numRemove;
	if (delFromHead)
	{
		// copy docid
		int ss = sizeof(u64) * (newsize);
		OmnMemCheck(mDocids, ss); 
		memmove(mDocids, &(mDocids[numRemove]), ss);
		ss = sizeof(u64)*numRemove;
		OmnMemCheck(&mDocids[newsize], ss);
		memset(&(mDocids[newsize]), 0, ss);
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
AosIILDocid::removeSubiil(
		const int iilidx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilidx > 0 && iilidx < mNumSubiils,false);
	aos_assert_r(mNumSubiils > 1,false);
	bool rslt = false;
	//1. return the iil
	if (mSubiils[iilidx])
	{
		aos_assert_r(mSubiils[iilidx]->getNumDocs() == 0,false);
		bool subIILReturned = false;
		rslt = AosIILMgrSelf->returnIILPublic(mSubiils[iilidx], iilmgrLocked, subIILReturned, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(subIILReturned, false);
		mSubiils[iilidx] = 0;
	}

	//2. proc mMinVals and mMaxVals;
	if (iilidx < (int)mNumSubiils -1)
	{
		memmove(&(mMinVals[iilidx]),&(mMinVals[iilidx+1]),(sizeof(u64))*(mNumSubiils - iilidx - 1));
		memmove(&(mMaxVals[iilidx]),&(mMaxVals[iilidx+1]),(sizeof(u64))*(mNumSubiils - iilidx - 1));
	}
	mMinVals[mNumSubiils-1] = 0;
	mMaxVals[mNumSubiils-1] = 0;

	//3. proc mNumEntries mIILIds
	if (iilidx < (int)mNumDocs -1)
	{
		memmove(&(mNumEntries[iilidx]),&(mNumEntries[iilidx +1]),(sizeof(int)*(mNumSubiils - 1)));
		memmove(&(mIILIds[iilidx]),&(mIILIds[iilidx +1]),(sizeof(u64)*(mNumSubiils - 1)));
	}
	mNumEntries[mNumSubiils-1] = 0;
	mIILIds[mNumSubiils-1] = 0;
	
	//4. proc mSubiils
	for(int i = iilidx ;i < mNumSubiils - 1;i++)
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

void    
AosIILDocid::failInSetCtntProtection()
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
AosIILDocid::addDocSeqPriv(
		const u64 &docid,
		int &iilidx,
		int &idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the
	// list. The list is sorted based on value, ascending.
	mIsDirty = true;
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		AosIILDocidPtr subiil;
		int newiilidx = getSubiilIndexSeq(docid,iilidx);
		if (newiilidx != iilidx)
		{
			idx = -1;
		}
		iilidx = newiilidx;
		
		subiil = getSubiilByIndex(iilidx, rdata);
		 
		aos_assert_r(subiil, false);
		if (subiil.getPtr() == this)
		{
			return insertDocidSeq( docid, iilidx,idx,iilmgrLocked, rdata);
		}

		bool rslt = subiil->addDocSeqSafe(docid,iilidx,idx,iilmgrLocked, rdata);
		mIsDirty = true;
		return rslt;
	}

	// singleIIL or subiil
	bool rslt = insertDocidSeq( docid,iilidx,idx,iilmgrLocked, rdata);

	aos_assert_r(rslt, false);
	mIsDirty = true;

	if (sgMaxIILLen < mNumDocs) 
	{
		sgMaxIILLen = mNumDocs;
	}
	
	return rslt;
}


bool
AosIILDocid::saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos)
{
	OmnShouldNeverComeHere;
	return false;
}

void    
AosIILDocid::exportIILPriv(
				   	vector<u64>			&docids,
					const AosRundataPtr &rdata)
{
	docids.clear();
	if (isParentIIL())
	{
		AosIILDocidPtr subiil;
		exportIILSinglePriv(docids,rdata);
		for(int i = 1;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i,rdata);
			subiil->exportIILSingleSafe(docids,rdata);
		}
		return;
	}
	return exportIILSinglePriv(docids,rdata);
}

void    
AosIILDocid::exportIILSinglePriv(
				   	vector<u64>			&docids,
					const AosRundataPtr &rdata)
{
	for(int i = 0;i < (int)mNumDocs;i++)
	{
		docids.push_back(mDocids[i]);
	}
}


#endif
