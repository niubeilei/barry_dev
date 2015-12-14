////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 	Created: 12/11/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILMgrBig/IILCompU64.h"

#include "AppMgr/App.h"
#include "IILMgrBig/IILMgr.h"
#include "IILUtil/IILLog.h"
#include "Porting/ThreadDef.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEUtil/Docid.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util1/MemMgr.h"
#include "WordClient/WordClient.h"


static bool sgCheckMemory = false;
//static AosMemChecker sgMemCheck;


AosIILCompU64::AosIILCompU64(
	const bool isPersis,
	const bool iilmgrLocked,
	const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_CompU64,isPersis),
mValues(0),
mMinDocids(0),
mMaxDocids(0),
mSubiils(0)
{
	mTotalIILs[eAosIILType_CompU64] ++;
}


AosIILCompU64::AosIILCompU64(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_CompU64, isPersis, iilname),
mValues(0),
mMinDocids(0),
mMaxDocids(0),
mSubiils(0)
{
	mTotalIILs[eAosIILType_CompU64] ++;
}


AosIILCompU64::AosIILCompU64(
	const u64 &iilid, 
	const u32 siteid, 
	AosBuff &buff, 
	const bool iilmgrLocked,
	const AosRundataPtr &rdata) 
:
AosIIL(eAosIILType_CompU64,false),
mValues(0),
mMinDocids(0),
mMaxDocids(0),
mSubiils(0)
{
	mTotalIILs[eAosIILType_CompU64] ++;
	mIILID = iilid;
	//mIsGood = loadFromFilePriv(buff, iilmgrLocked, rdata);
	mIsGood = loadCompFromFilePriv(buff, siteid, iilmgrLocked, rdata);
}


AosIILCompU64::~AosIILCompU64()
{
	AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_CompU64] --;

	if (mValues)
	{
		OmnDelete [] mValues;
		mValues = 0;
	}

	if (isRootIIL())
	{
		if(mNumSubiils > 0)
		{
			memset(mMinDocids,0,mNumSubiils* sizeof(u64));
			memset(mMaxDocids,0,mNumSubiils* sizeof(u64));
		}
		
		for (u32 i=0; i<mNumSubiils; i++)
		{
			if (mSubiils[i]) AosIILMgrSelf->returnIILPublic(mSubiils[i].getPtr(), rdata);
			mSubiils[i] = 0;
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
		if(mIILIds)
		{
			OmnDelete [] mIILIds;
			mIILIds = 0;
		}
		if(mSubiils)
		{
			OmnDelete [] mSubiils;
			mSubiils = 0;
		}
		if(mNumEntries)
		{
			OmnDelete [] mNumEntries;
			mNumEntries = 0;
		}
	}

	mNumSubiils = 0;
	mMinDocids = 0;
	mMaxDocids = 0;
	mIILIds = 0;
	mSubiils = 0;
	mNumEntries = 0;
}


bool		
AosIILCompU64::checkDocPriv(
		const u64 &docid,
		const u64 &value,
		const AosOpr opr,
		const AosRundataPtr &rdata)
{
	if(!isRootIIL())
	{
		bool keepSearch = false;
		return checkDocSinglePriv(docid,value,opr,keepSearch ,rdata);
	}

	//AosIILCompU64Ptr subiil = getSubIIL(-10, docid, true, rdata);
	AosIILCompU64Ptr subiil = getSubiil(docid, rdata);
	if (!subiil)
	{
		AOSUNLOCK(mLock);
		return false;
	}

	bool rslt = false;
	bool keepSearch = true;
	int subIILIdx = subiil->getIILIdx();
	aos_assert_r(subIILIdx>=0,false);
	while(keepSearch)
	{
		if(subiil == this)
		{
			rslt = subiil->checkDocSinglePriv(docid,value,opr,keepSearch, rdata);
		}
		else
		{
			rslt = subiil->checkDocSingleSafe(docid,value,opr,keepSearch, rdata);
		}
		// already found
		if(rslt)
		{
			return true;
		}
		// do not need to keep searching
		if(!keepSearch)
		{
			return false;
		}
		// find to the last subiil
		if(subIILIdx >= (int)mNumSubiils-1)
		{
			return false;
		}
		subIILIdx ++;
		subiil = getSubiilByIndex(subIILIdx, rdata);
	}
	return false; 
}


bool	
AosIILCompU64::checkDocSinglePriv(
		const u64 &docid,
		const u64 &value,
		const AosOpr opr,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	if (mNumDocs == 0) 
	{
		keepSearch = true;
		return false;
	}

	aos_assert_r(mNumDocs <= mMemCap, false);

	int startidx = 0; 

	aos_assert_r(firstDoc(startidx, false, docid), false);
	if (startidx < 0) 
	{
		keepSearch = false;
		return false;
	}

	for (int i=startidx; i<=(int)mNumDocs -1; i++)
	{
		if(mDocids[i] != docid)
		{
			keepSearch = false;
			return false;			
		}
		if(valueMatch(mValues[i],opr,value))
		{
			keepSearch = false;
			return true;
		}
	}
	keepSearch = true;
	return false;
}           


AosIILCompU64Ptr
AosIILCompU64::getSubiilByIndex(
		const int idx, 
		const AosRundataPtr &rdata)
{
	// It retrieves the idx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil. 
	//
	if (mRootIIL)
	{
		aos_assert_r(mRootIIL.getPtr() == this, 0);
	}

	if (idx == 0) 
	{
		aos_assert_r(mRootIIL,0);
		return this;
	}

	aos_assert_r(idx > 0 && idx < (int)mNumSubiils, 0);
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILCompU64Ptr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[idx]) return mSubiils[idx];

	// Retrieve an IIL from IILMgr
	aos_assert_r(mIILIds[idx], 0);

	// Chen Ding, 05/05/2011
	AosIILType type= eAosIILType_CompU64;
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
		mIILIds[idx], mSiteid, type, true, false, rdata);
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[idx] << ":" << type << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_CompU64)
	{
		OmnAlarm << "Not a string comp IIL: " 
			<< iil->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}
	mSubiils[idx] = (AosIILCompU64*)iil.getPtr();
	mSubiils[idx]->mRootIIL = this;
	mSubiils[idx]->mIILIdx = idx;
	aos_assert_r(mSubiils[0], 0);
	return mSubiils[idx];
}
            

// get sub iil functions for add and remove 
int
AosIILCompU64::getSubiilIndex(const u64 &docid)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
    int left = 0;
    int right = mNumSubiils - 1;
    int cur = 0;
    if(right == left)return right;
    bool leftnotfit = false;
	while(1)
    {
        if(right == left +1)
        {
			if(leftnotfit) return right;
            // if left fit, return left
            if(mMaxDocids[left] >= docid)
            {
                return left;
            }
            else
            {
                return right;
            }
        }

        cur = (right - left)/2 + left;

        if(mMaxDocids[cur] >= docid)
        {
			right = cur;
		}
		else
		{
			left = cur;
			leftnotfit = true;
		}
	}
	return mNumSubiils -1;
}
        

// get sub iil functions for add and remove 
AosIILCompU64Ptr
AosIILCompU64::getSubiil(const u64 &docid, const AosRundataPtr &rdata)
{
	int index = getSubiilIndex(docid);
	aos_assert_r(index >= 0 && index <= (int)mNumSubiils -1,0);
	return getSubiilByIndex(index, rdata);
}


bool
AosIILCompU64::firstDoc(
		int &idx, 
		const bool reverse, 
		const u64 &docid)
{
	// Starting from the first entry, it searches for the firsst
	// doc that matches the condition (opr, value). If found, 
	// the docid is set to 'docid' and 'idx' is set to the 
	// position. Otherwise, 'docid' is set to AOS_INVDID and
	// 'idx' is set to -1. 
	if (mNumDocs == 0) 
	{
		idx = -5;
		return true;
	}

	int reverse_flag;
	if (reverse)
	{
		reverse_flag = -1;
		idx = mNumDocs-1;
	}
	else
	{
		reverse_flag = 1;
		idx = 0;
	}
	
	 return nextDocidEQ(idx, reverse_flag, docid);
}

            
bool
AosIILCompU64::nextDocidEQ(
		int &idx, 
		const int reverse, 
		const u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	//
	// If success, 'docid' holds the docid found and 'idx' points to
	// the next element (IMPORTANT: It does not point to the current one).
	// if (mValues[idx] == value)

	if (mDocids[idx] == docid)
	{
		return true;
	}

	idx = (reverse == 1)?firstEQ(idx, docid):firstEQRev(idx, docid);
	if (idx == -1)
	{
		idx = -5;
		return true;
	}	
	return true;
}


int
AosIILCompU64::firstEQ(const int idx, const u64 &docid)
{
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	aos_assert_r(mNumDocs > 0,-1);

	if (mDocids[idx] == docid) return idx;
	if (mDocids[idx] > docid) return -1;
	if (mDocids[mNumDocs - 1] < docid) return -1;

	int left = idx; 
	int right = mNumDocs-1;
	if (left == right || left + 1 == right) 
	{
		if (mDocids[right] == docid) return right;
		return -1;
	}

	while (left < right)
	{
		int nn = (right + left) >> 1;

		if (mDocids[nn] < docid)
		{
			left = nn+1;
		}
		else if (mDocids[nn] > docid)
		{
			// mValues[nn] > value
			right = nn-1;
		}
		else
		{
			// mValues[nn] == value. Since it is to find the first match, 
			// it needs to check whether 'left == right'. If not, need 
			// to continue the searching
			right = nn;
			if (left == right) return right;
			if (left + 1 == right) 
			{
				// Chen Ding, 08/11/2010
				// if (strcmp(mValues[left], value.data()) == 0) return left;
				if (mDocids[left] == docid) return left;
				return right;
			}
		}
	}
	if (left == right && mDocids[left] == docid) return left;
	return -1;
}


int
AosIILCompU64::firstEQRev(const int idx, const u64 &docid)
{
	// IMPORTANT!!!!!!!!!!
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	
	if (mDocids[idx] == docid) return idx;
	// If mValues[idx] is less than 'value', it is not possible
	if (mDocids[idx] < docid) return -1;

	// If mValues[0] is greater than value, it is not possible
//	if (mDocids[idx] > docid) return -1;
		
	// Now, we are sure mValues[0] <= value <= mValues[idx].  
	int left = 0; 
	int right = idx;
	if (left == right) return -1;
	if (left + 1 == right) 
	{
		if (mDocids[right] == docid) return right;
		if (mDocids[left] == docid) return left;
		return -1;
	}

	while (left < right)
	{
		int nn = (right + left) >> 1;
		// Chen Ding, 08/11/2010
		// int rslt = strcmp(mValues[nn], value.data());
		if (mDocids[nn] < docid)
		{
			// mValues[nn] < value
			left = nn + 1;
		}
		else if (mDocids[nn] > docid)
		{
			right = nn -1;
		}
		else
		{
			// mValues[nn] == value
			left = nn;
			if (left == right) return left;
			if (left + 1 == right) 
			{
				if (mDocids[right] == docid) return right;
				return left;
			}
		}
	}

	if (left == right && (mDocids[left] == docid)) return left;
	return -1;
}


bool
AosIILCompU64::addDocPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	bool keepSearch = false;
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	if (isRootIIL())
	{
		// It is segmented and the iil is the root iil. 
		AosIILCompU64Ptr subiil;

		// Chen Ding, Bug1217
		int iilidx = getSubiilIndex(docid);
		// int iilidx = getSubIILIndex(-10, value, eAosOpr_ge, false);
		if (iilidx == -5) iilidx = 0;
		// End of Bug1217
	
		aos_assert_r(iilidx >= 0,false);
		aos_assert_r(mNumSubiils > 0,false);
		while (iilidx <= (int)mNumSubiils -1)
		{
			subiil = getSubiilByIndex(iilidx, rdata);
			aos_assert_r(subiil, false);
			keepSearch  = false;
			
			if (subiil.getPtr() == this)
			{
				rslt = addDocDirectPriv(
						value, docid, value_unique, docid_unique, iilmgrLocked, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->addDocDirectSafe(
						value, docid, value_unique, docid_unique, iilmgrLocked, keepSearch, rdata);
			}
			
			if (!keepSearch)
			{
				return rslt;
			}
			iilidx++;
		}
		// raise alarm  lxx
		return false;
	}

	aos_assert_r(isSingleIIL(),false);
	// James, 01/04/2011
	rslt = addDocDirectPriv(
			value, docid, value_unique, docid_unique, iilmgrLocked,keepSearch, rdata);
	return rslt;
}


bool
AosIILCompU64::addDocDirectPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrLocked,
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	keepSearch = false;
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:

	// 	1. mDocids[mNumDocs-1] == docid
	// 	2. mValues[mNumDocs-1] < value

	// In this case, the doc may be inserted in the next subiil.
	bool rslt = false;
	u32 numBefore = mNumDocs;
	// OmnScreen << "IILCompU64 add: " << this << endl;
	u32 idx = 0;
	int left = 0; 
	int right = mNumDocs-1;
	if (mNumDocs == 0)
	{
		// Change mValues
		if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
		mDocids[0] = docid;
		mValues[0] = value;
		mNumDocs = 1;
		idx = 0;
		goto finished;
	}

	aos_assert_r(mNumDocs <= mMemCap, false);
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
//		rsltInt = valueMatch(mValues[idx], value);
		if (mDocids[idx] == docid)
		{
			// Chen Ding, 10/24/2010
			if (value_unique)
			{
				// The value must be unique. This fails the operation. 
				// if 'value_unique' is AOSIIL_FORCE_UNIQUE, it will 
				// override the entry. Otherwise, it returns an error. 
				if (rdata->needReportError())
				{
					rdata->setError()<< "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx];
					OmnAlarm << rdata->getErrmsg()<< enderr;
				}
				return false;
			}

			// Need to insert the doc based on docid
			rslt = insertDocPriv(
					idx, value, docid, value_unique, docid_unique, iilmgrLocked, keepSearch);
			if (!rslt) return rslt;
			if(!keepSearch)
			{
				goto finished;
			}
			return true;
		}

		// Chen Ding, 08/11/2010
		// if (strcmp(mValues[idx], value.data()) < 0) 
		if (mDocids[idx] < docid) 
		{
			if (idx == mNumDocs-1)
			{
				// Append the docid
				if (mNumDocs >= mMemCap)
					aos_assert_r(expandMemoryPriv(), false);
				mDocids[mNumDocs] = docid;
				// Change mValues
				mValues[mNumDocs] = value;
				incNumDocsNotSafe();
				idx = mNumDocs-1;
				goto finished;
			}

			if(mDocids[idx+1] > docid)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if(mDocids[idx+1] == docid)
			{
				if(value_unique)
				{
					if (rdata->needReportError())
					{
						rdata->setError() << "Value already exist: " << value << ":" << docid
							<< ":" << mDocids[idx];
						OmnAlarm << rdata->getErrmsg() << enderr;
					}
					return false;
				}
				idx++;
				rslt = insertDocPriv(
						idx, value, docid, value_unique, docid_unique, iilmgrLocked, keepSearch);
				aos_assert_r(rslt, false);
				if (!keepSearch) goto finished;
				return true;
			}
			left = idx+1;
	 	}
		else
		{
			if (idx == 0)
			{
				// Insert into the front
				insertBefore(idx, docid, value);
				goto finished;
			}

			if (mDocids[idx-1] < docid)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				idx--;
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if(mDocids[idx-1] == docid)
			{
				if(value_unique)
				{
					if (rdata->needReportError())
					{
						rdata->setError()<< "Value already exist: " << value << ":" << docid
							<< ":" << mDocids[idx];
						OmnAlarm << rdata->getErrmsg() << enderr;
					}
					return false;
				}
				idx --;
				rslt = insertDocPriv(idx, value, docid, value_unique, docid_unique,
							iilmgrLocked,keepSearch);
				aos_assert_r(rslt, false);
				if (!keepSearch) goto finished;
				return true;
			}
			right = idx-1;
		}
	}

	OmnShouldNeverComeHere;
	return false;

finished:
	// if we have subiil, set root iil max/min value and mNumEntries. Note that we may need change them both
	mIsDirty = true;

	aos_assert_r(numBefore + 1 == mNumDocs, false);
	if (!isSingleIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_CompU64, false);
		AosIILCompU64 *rootiil = (AosIILCompU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);
	}
	if (mNumDocs >= mMaxIILSize)
	{
		AosIILPtr subiil;
		rslt = splitListPriv(iilmgrLocked, subiil, rdata);
	}
//	sanityTestForSubiils();
//	AosIILCompU64Ptr r = (AosIILCompU64*)mRootIIL.getPtr();
//	if(r)r->splitSanityCheck();
	return true;
}


bool
AosIILCompU64::updateIndexData(const int idx,const bool changeMax,const bool changeMin)
{
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	mIsDirty = true;
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < (int)mNumSubiils, false);
	aos_assert_r(mSubiils[idx], false);

	AosIILCompU64Ptr subiil = mSubiils[idx];
	mNumEntries[idx] = subiil->getNumDocs();
	if(changeMax)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		mMaxDocids[idx] = subiil->getMaxDocid();
	}

	if(changeMin)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		mMinDocids[idx] = subiil->getMinDocid();
	}
	return true;
}


bool
AosIILCompU64::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = mNumDocs + mExtraDocids;
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


bool
AosIILCompU64::checkMemory() const
{
	// It assumes the memory was allocated by memory pool, which 
	// has some extra space. In the first round, it sets the index
	// in the extra space for each entry. It then goes to the second
	// round to see whether any element's index is altered by 
	// someone else. If yes, it means the entry was shared by 
	// one elements.
	if (!sgCheckMemory) return true;
	if (!mValues) return true;
	return true;
}


bool 	
AosIILCompU64::insertBefore(
		const int nn, 
		const u64 &docid, 
		const u64 &value)
{
	// It inserts an entry in front of 'nn'. 
	if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(nn >= 0, false);

	// Need to move pointers starting at 'nn'
	memmove(&mDocids[nn+1], &mDocids[nn], sizeof(u64) * (mNumDocs - nn));
	memmove(&mValues[nn+1], &mValues[nn], sizeof(u64) * (mNumDocs - nn));

	// Change mValues
	mValues[nn] = value;
	mDocids[nn] = docid;

	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


bool 	
AosIILCompU64::insertAfter(
		const int nn, 
		const u64 &docid, 
		const u64 &value)
{
	// It inserts an entry after 'nn'. 
	if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(nn >= 0, false);

	memmove(&mDocids[nn+2], &mDocids[nn+1], sizeof(u64) * (mNumDocs - nn - 1));
	memmove(&mValues[nn+2], &mValues[nn+1], sizeof(u64) * (mNumDocs - nn - 1));
	mDocids[nn+1] = docid;
	mValues[nn+1] = value;
	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


// Description:
// This function assumes mDocid[idx] == docid. For entries whose
// docids are the same, they are sorted based on value. 
// 1. If mValues[idx] == value, the doc is inserted @idx.
// 2. If mValues[idx] < value, the doc is inserted after idx
// 3. Otherwise, the doc is inserted before idx.
//
// IMPORTANT!!!!!!!!!!
// It assumes mDocids[idx] == docid
bool
AosIILCompU64::insertDocPriv(
		u32 &idx, 
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrLocked,
		bool &keepSearch) 
{
	// It inserts the doc at position 'idx'. If:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// the doc may need to be inserted in the next subiil.
	aos_assert_r(mNumDocs > 0, false);

	int left = firstEQ(0, docid);
	int right = firstEQRev(mNumDocs-1, docid);

	aos_assert_r(left >= 0 && (u32)left < mNumDocs, false);
	aos_assert_r(right >= left && (u32)right < mNumDocs, false);

	if (value == mValues[left])
	{
		if (docid_unique)
		{
			OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			return false;
		}

		aos_assert_r(insertBefore(left, docid, value), false);
		idx = left;
		return true;
	}

	if (value < mValues[left])
	{
		aos_assert_r(insertBefore(left, docid, value), false);
		idx = left;
		return true;
	}

	if (value == mValues[right])
	{
		if (docid_unique)
		{
			OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			return false;
		}
		aos_assert_r(insertAfter(right, docid, value), false);
		idx = right+1;
		return true;
	}

	if (value > mValues[right])
	{
		// It is possible that the entry should be inserted
		// in the next subiil. Need to verify:
		// 1. Whether there is a subiil
		// 2. Whether the first value in the subiil equals 'value'
		// 3. Whether the first docid is bigger than 'docid'
		if ((u32)right == mNumDocs-1 && mIILIdx >= 0 && mRootIIL &&
			mRootIIL->getNumSubiils() > 0 &&
			mIILIdx != (int)mRootIIL->getNumSubiils() -1)
		{
			keepSearch = true;
			return true;
		}

		aos_assert_r(insertAfter(right, docid, value), false);
		idx = right+1;
		return true;
	}

	// mDocids[left] < value < mDocids[right], this means that 
	// the new doc must be inserted in between (left, right), 
	// or after left and before right.
	while (left < right)
	{
		if (left + 1 == right)
		{
			aos_assert_r(insertAfter(left, docid, value), false);
			idx = left+1;
			return true;
		}

		int nn = (left + right) >> 1;
		if (mValues[nn] == value)
		{
			if (docid_unique)
			{
				OmnAlarm << "Not unique: " << docid 
					<< ":" << value << ":" << mIILID << enderr;
				return false;
			}
			aos_assert_r(insertBefore(nn, docid, value), false);
			idx = nn;
			return true;
		}
		else if (mValues[nn] < value) left = nn;
		else right = nn;
	}

	OmnShouldNeverComeHere;
	return false;
}



bool
AosIILCompU64::splitListPriv(const bool iilmgrLocked,AosIILPtr &subiil, const AosRundataPtr &rdata)
{
	if(mRootIIL && mRootIIL->getNumSubiils() > mMaxSubIILs)
	{
		OmnAlarm << "too many subiils : " << mRootIIL->getNumSubiils()<< enderr;
	}

	// It cuts the current list in two. The first half stays 
	// with the current list and the second half is in a new
	// IIL list.

	// 1. Check whether it can create more subiils
	aos_assert_r(mNumSubiils < mMaxSubIILs-1, false);

	// 2. Create the subiil
	subiil = AosIILMgrSelf->createSubIILSafe(
				mIILID, mSiteid, eAosIILType_CompU64, mIsPersis, iilmgrLocked, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->getIILType() == eAosIILType_CompU64, false);
	AosIILCompU64Ptr newsubiil = (AosIILCompU64*)subiil.getPtr();
	
	newsubiil->saveFullHeader(rdata);
	//ken 2011/07/25	
	// 3. Set mRootIIL
	if(mRootIIL.isNull())
	{
		mRootIIL = this;
		mIILIdx = 0;
	}
	subiil->setRootIIL(mRootIIL);
	subiil->setIILIdx(mIILIdx+1);

	// 4. Initialize the new subiil
	u32 startidx = mNumDocs / 2;
	const u32 len = mNumDocs - startidx;
	aos_assert_r(mRootIIL->getIILType() == eAosIILType_CompU64, false);
	AosIILCompU64 *rootiil = (AosIILCompU64*)mRootIIL.getPtr();
	aos_assert_r(mIILIdx >= 0, false);
	aos_assert_r(newsubiil->initSubiil(&mDocids[startidx], &mValues[startidx], 
			len, mIILIdx+1, rootiil, iilmgrLocked), false);

	// 5. Shrink this IIL
	memset(&mValues[startidx], 0, sizeof(u64) * len);
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	mIsDirty = true;
	aos_assert_r(mNumDocs < mMaxIILSize, false);

	// 6. Add the subiil
	AosIILCompU64Ptr thisptr(this, false);
	aos_assert_r(rootiil->addSubiil(thisptr, newsubiil), false);
	newsubiil->setDirty(true);
//	AosIILCompU64Ptr r = (AosIILCompU64*)mRootIIL.getPtr();
//	r->splitSanityCheck();
	return true;
}


bool 
AosIILCompU64::initSubiil(
		u64 *docids,
		u64 *values, 
		const int numDocs,
		const int subiilid, 
		const AosIILCompU64Ptr &rootiil, 
		const bool iilmgrLocked) 
{
	aos_assert_r(mIILType == eAosIILType_CompU64, false);
	aos_assert_r(rootiil, false);

	mRootIIL = rootiil.getPtr();
    mWordId = AOS_INVWID;
	// ModifyNumDocs
	aos_assert_r(mNumDocs < mMaxIILSize, false);
    mNumDocs = numDocs; 

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
	if(mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}
	if(mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}
	if(mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

    mFlag = 0;
    mHitcount = 0;
    mIILDiskSize = 0;
    mOffset = 0;
    mSeqno = 0;
	mCompressedSize = 0;

    mNumSubiils = 0;
    mIILIdx = 0;

	// Prepare the memory
	aos_assert_r(prepareMemoryForReloading(iilmgrLocked), false);
	memcpy(mValues, values, sizeof(u64) * mNumDocs);
	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool
AosIILCompU64::addSubiil(
		const AosIILCompU64Ptr &crtsubiil,
		const AosIILCompU64Ptr &newsubiil)
{
	// It inserts the new subiil 'newsubiil' after 'crtsubiil'. This function
	// must be called by the root IIL.
	aos_assert_r(mIILIdx == 0, false);
	aos_assert_r(mNumSubiils < mMaxSubIILs-1, false);
	if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
	int iilidx = crtsubiil->getIILIdx();
	aos_assert_r(iilidx >= 0,false);

	// 1. Insert the new subiil in mSubiils[]
	if (iilidx < (int)mNumSubiils-1)
	{
        memmove(&mSubiils[iilidx+2], &mSubiils[iilidx+1],
                sizeof(AosIILCompU64Ptr) * (mNumSubiils-iilidx-1));
        memset(&mSubiils[iilidx+1],0,sizeof(AosIILCompU64Ptr));

        memmove(&mMinDocids[iilidx+2], &mMinDocids[iilidx+1],
                sizeof(u64) * (mNumSubiils-iilidx-1));
        memset(&mMinDocids[iilidx+1],0,sizeof(u64));

        memmove(&mMaxDocids[iilidx+2], &mMaxDocids[iilidx+1],
                sizeof(u64) * (mNumSubiils-iilidx-1));
        memset(&mMaxDocids[iilidx+1],0,sizeof(u64));

        memmove(&mIILIds[iilidx+2], &mIILIds[iilidx+1],
                sizeof(u64) * (mNumSubiils-iilidx-1));
        mIILIds[iilidx+1] = 0;

        memmove(&mNumEntries[iilidx+2], &mNumEntries[iilidx+1],
                sizeof(int) * (mNumSubiils-iilidx-1));
        mNumEntries[iilidx+1] = 0;


		for (u32 i=(u32)iilidx+2; i<mNumSubiils; i++)
		{
			if(mSubiils[i].notNull())
			{
				mSubiils[i]->setIILIdx(i);
			}
		}
	}	


	mSubiils[iilidx+1] = newsubiil;
	mIILIds[iilidx+1] = newsubiil->getIILID();
	mSubiils[iilidx] = crtsubiil;
	mIILIds[iilidx] = crtsubiil->getIILID();

	mNumSubiils++;
	if (mNumSubiils == 1) mNumSubiils++;

	newsubiil->setIILIdx(iilidx+1);

	// 4. Set mIILIds[]
	mIILIds[iilidx+1] = newsubiil->getIILID();	

	// 4. If 'iilidx' is 0, it needs to set the first entry
	if (iilidx == 0)
	{
		mSubiils[0] = this;
	}

	// 5. update all the mIILIdx in the subiils
	for(u32 i = iilidx; i < mNumSubiils;i++)
	{
		if(mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}

	updateIndexData(iilidx,true,true);
	updateIndexData(iilidx+1,true,true);

	mIsDirty = true;
	crtsubiil->setDirty(true);
	newsubiil->setDirty(true);

	aos_assert_r(mSubiils[0], false);
	return true;
}


bool
AosIILCompU64::createSubiilIndex()
{
	aos_assert_r(!mSubiils, false);

	mSubiils = OmnNew AosIILCompU64Ptr[mMaxSubIILs];
	aos_assert_r(mSubiils, false);

	mMinDocids = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mMinDocids, false);
	memset(mMinDocids, 0, sizeof(u64) * mMaxSubIILs);

	mMaxDocids = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mMaxDocids, false);
	memset(mMaxDocids, 0, sizeof(u64) * mMaxSubIILs);

	mIILIds = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * mMaxSubIILs);

	mNumEntries = OmnNew int[mMaxSubIILs];
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(int) * mMaxSubIILs);

	return true;
}

bool
AosIILCompU64::removeDocPriv(
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	bool keepSearch = true;
	bool rslt = false;
	// Check whether it is a root iil and it is segmented
	if (isRootIIL())
	{
		// It is segmented and the iil is the root iil. 
		// If 'iilidx' is within [0, mNumSubiils-1], 
		// use it.
		AosIILCompU64Ptr subiil;
		subiil = getSubiil(docid, rdata);
		aos_assert_r(subiil, false);
		while(subiil)
		{
			if (subiil.getPtr() == this)
			{
				rslt = removeDocDirectPriv(value, docid, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->removeDocDirectSafe(value, docid, keepSearch, rdata);		
			}
			if(rslt) return true;
			if(!keepSearch) break;
			// get next subiil
			subiil = subiil->getNextSubIIL(true, rdata);
		}
		// found to the last subiil
		return false;
	}
	
	// The IIL is not segmented
	rslt = removeDocDirectPriv(value, docid,keepSearch, rdata);
	return rslt;
}


AosIILCompU64Ptr
AosIILCompU64::getNextSubIIL(const bool &forward, const AosRundataPtr &rdata)
{
	bool rslt = false;
	if(mRootIIL.isNull())
	{
		return 0;
	}

	if(forward)
	{
		aos_assert_r(mRootIIL->getNumSubiils() > 0,0);
		rslt = mIILIdx < ((int)mRootIIL->getNumSubiils() - 1);
	}
	else
	{
		rslt = mIILIdx > 0;
	}

	if(!rslt)
	{
		return 0;
	}

	aos_assert_r(mRootIIL->getIILType() == eAosIILType_CompU64,0);
	AosIILCompU64Ptr rootIIL = (AosIILCompU64*)mRootIIL.getPtr();

	if(forward)
	{
		return rootIIL->getSubiilByIndex(mIILIdx+1, rdata);
	}else
	{
		return rootIIL->getSubiilByIndex(mIILIdx-1, rdata);
	}
}

bool 		
AosIILCompU64::removeDocDirectPriv(
		const u64 &value, 
		const u64 &docid,
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	// It assumes the doc is in this list and the lock has been locked
	// It uses 'value' to locate the start and end idx. It then loops
	// through [startidx, endidx] to check for 'docid' and remove 
	// the entries that match 'docid'.
	//
	// Performance Improvements:
	// Entries whose values are the same should be sorted based on 
	// docids. When searching, use the binary search.
	
	// Chen Ding, 12/07/2010
	//aos_assert_r(mNumDocs > 0, false);
	keepSearch = true;
	if (mNumDocs <= 0) return false;

	keepSearch = false;

	//int startidx = -10;
//	int startidx = 0;
//	int endidx = mNumDocs -1;
//	int idx = -1;
//	u64 did;

	int startidx = firstEQ(0, docid);
	if(startidx < 0)
	{
		// not found
		return false;
	}
	
	int endidx = firstEQRev(mNumDocs-1, docid);
	aos_assert_r(endidx >= 0,false);
	
	int idx = startidx;
	while(idx <= endidx)
	{
		if (mValues[idx] == value)
		{
			goto removeEntry;
		}
		else if(mValues[idx] > value)
		{
			// not found
			break;
		}
		idx ++;
	}

	if(idx > (int)mNumDocs -1)
	{
		keepSearch = true;
	}
	else
	{
		keepSearch = false;
	}
	
	return false;

removeEntry:
	// If it is a subiil, need to update the index values
	// as needed.
	// Remove the entry 'idx'. Need to move all the entries.
	aos_assert_r(idx >= 0 && (u32)idx < mNumDocs, false);

	// Change mValues
	mValues[idx] = 0;
	if ((u32)idx < mNumDocs-1)
	{
		int ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx+1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx+1], ss);

		ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mValues[idx], ss);	
		OmnMemCheck (&mValues[idx+1], ss);	
		memmove(&mValues[idx], &mValues[idx+1], ss);
	}
	decNumDocsNotSafe();

	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	mIsDirty = true;
	
	if (isRootIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = ((u32)idx >= mNumDocs-1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_CompU64, false);
		AosIILCompU64 *rootiil = (AosIILCompU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);

		if(mNumDocs < mMinIILSize)
		{
			rootiil->mergeSubiilPriv(true,mIILIdx, rdata);
		}
	}
	return true;
}


bool 		
AosIILCompU64::mergeSubiilPriv(const bool iilmgrLocked,const int iilidx, const AosRundataPtr &rdata)
{
	// This function should be called on mRootIIL. The iil 'iilidx' is too 
	// small. The function will re-arrange the contents as follows:
	// 1. If 'iilidx-1' and 'iilidx' together < mMaxIILSize, average them
	// 2. Otherwise, if 'iilidx' and 'iilidx+1' < mMaxIILSize, average them
	// 3. Otherwise, 
	mIILMergeFlag = true;

	aos_assert_r(isRootIIL(),false);	
	aos_assert_r(mNumSubiils > 1,false);
	bool rslt = false;

	//1. If there is a iil around, two iils contains more than 	
	//   mMaxIILSize docs, merge some docs from that iil to this one.
	AosIILCompU64Ptr iil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iil,false);
	int numdoc1 = iil->getNumDocs();
	int numdoc2 = 0; // num doc prev
	int numdoc3 = 0; // num doc next
	int merge_size = mMaxIILSize/2 + mMinIILSize;
	if(iilidx > 0)
	{
		AosIILCompU64Ptr iil2 = getSubiilByIndex(iilidx - 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc2 = iil2->getNumDocs();

		//if(numdoc1 + numdoc2 >= (int)mMaxIILSize)
		if(numdoc1 + numdoc2 >= merge_size)
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
	
	aos_assert_r(mNumSubiils > 0 ,false);
	if(iilidx < (int)mNumSubiils - 1)
	{
		AosIILCompU64Ptr iil2 = getSubiilByIndex(iilidx + 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc3 = iil2->getNumDocs();

		//if(numdoc1 + numdoc3 >= (int)mMaxIILSize)
		if(numdoc1 + numdoc3 >= merge_size)
		{
			int nummove = (numdoc1 + numdoc3)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilBackwardPriv(iilmgrLocked,iilidx + 1,nummove, rdata);
			aos_assert_r(rslt, false);
			return true;
		}
	}
	
	// if it is the first docid, merge the 2nd one to the first one.		
	if(iilidx == 0)
	{
		// merge iil[1] to iil[0]
		rslt = mergeSubiilBackwardPriv(iilmgrLocked,1,numdoc3, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	

	// if it is the last docid, merge this iil to the one before.		
	aos_assert_r(mNumSubiils > 0, false);
	if(iilidx == (int)mNumSubiils -1)
	{
		rslt = mergeSubiilBackwardPriv(iilmgrLocked,iilidx,numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	
	
	// The iil is in the middle
	if(numdoc2 < numdoc3)
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
AosIILCompU64::mergeSubiilForwardPriv(
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
	aos_assert_r(isRootIIL(),false);
	aos_assert_r(mNumSubiils > 1 && iilidx < (int)mNumSubiils -1,false);
	aos_assert_r(mNumEntries[iilidx] >= numDocToMove,false);		
	if(iilidx == 0)
	{
		aos_assert_r(mNumEntries[iilidx] >  numDocToMove,false);		
	}

	// 2. move entries from a to b
	AosIILCompU64Ptr crtiil = getSubiilByIndex(iilidx, rdata);
	AosIILCompU64Ptr nextiil = getSubiilByIndex(iilidx+1, rdata);
	aos_assert_r(crtiil, false);
	aos_assert_r(nextiil, false);
		
	bool rslt = false;
	u64* 	valuePtr = 0;
	u64*    docidPtr = 0;
	// get pointers
	int offset = crtiil->getNumDocs() - numDocToMove;
	rslt = crtiil->getValueDocidPtr(valuePtr, docidPtr, offset);
	aos_assert_r(rslt, false);

	// append to new
	rslt = nextiil->appendDocToSubiil(valuePtr, docidPtr, numDocToMove, true);
	aos_assert_r(rslt, false);
	rslt = crtiil->removeDocFromSubiil(numDocToMove, false);
	aos_assert_r(rslt, false);
			
	// 3. update information in root iil,
	// remove the original subiil if it is empty now
	updateIndexData(iilidx+1, true, true);
	if(crtiil->getNumDocs() == 0)
	{
		removeSubiil(iilidx,iilmgrLocked, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true);
	}
	return true;	
}


bool 		
AosIILCompU64::mergeSubiilBackwardPriv(
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
	aos_assert_r(isRootIIL(), false);
	aos_assert_r(iilidx > 0 && iilidx <= (int)mNumSubiils -1,false);
	
	// 2. move entries from a to b
	AosIILCompU64Ptr previil = getSubiilByIndex(iilidx-1, rdata);
	AosIILCompU64Ptr crtiil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(previil, false);
	aos_assert_r(crtiil, false);
		
	bool rslt = false;
	u64* 	valuePtr = 0;
	u64*    docidPtr = 0;
	// get pointers
	rslt = crtiil->getValueDocidPtr(valuePtr,docidPtr,0);
	aos_assert_r(rslt, false);

	// Append 'numDocToMove' entries to the previous iil.
	rslt = previil->appendDocToSubiil(valuePtr, docidPtr, numDocToMove, false);
	aos_assert_r(rslt, false);

	// Remove the first 'numDocToMove' entries from the current IIL.
	rslt = crtiil->removeDocFromSubiil(numDocToMove, true);
	aos_assert_r(rslt, false);
			
	// 3. update information in root iil,
	// remove the original subiil if it is empty now
	updateIndexData(iilidx-1, true, true);

	if(crtiil->getNumDocs() == 0)
	{
		removeSubiil(iilidx,iilmgrLocked, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true);
	}
	return true;	
}


bool		
AosIILCompU64::getValueDocidPtr(
		u64* &valuePtr,
		u64*    &docidPtr,
		const int offset)
{
	aos_assert_r(mValues, false);
	aos_assert_r(mDocids, false);
	aos_assert_r(offset >= 0, false);
	aos_assert_r(offset < (int)mNumDocs, false);
	valuePtr = &(mValues[offset]);
	docidPtr = &(mDocids[offset]);
	return true;
}


bool 
AosIILCompU64::appendDocToSubiil(
		u64 *values, 
		u64 *docids, 
		const int numDocs,
		const bool addToHead) 
{
	// Notice that we use memcpy to copy string point list to the new subiil, 
	// So we can NOT use delete in the original subiil, instead, we use 
	// memset to clear that memory.
	aos_assert_r(mIILType == eAosIILType_CompU64, false);

	// ModifyNumDocs
	aos_assert_r(mNumDocs + numDocs < mMaxIILSize, false);
	if(numDocs == 0)
	{
		return true;
	}
	int newNumDocs = mNumDocs + numDocs; 
	aos_assert_r(newNumDocs > 0,false);	
	//1. enlarge mValues/mDocids space to hold all the values and docids, in the same time, 
	// move the original data and new data to the new place.	
	// if the number of the new list is less than mMemCap, do nothing to 		
	
	if(newNumDocs < (int)mMemCap)
	{
		if(addToHead)
		{
			// copy value
			memmove(&(mValues[numDocs]), mValues, sizeof(u64) * mNumDocs);
			memcpy(mValues, values, sizeof(u64) * numDocs);

			// copy docid
			memmove(&(mDocids[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mDocids, docids, sizeof(u64) * numDocs);
		}	
		else
		{
			// copy value
			memcpy(&mValues[mNumDocs], values, sizeof(u64) * numDocs);			

			// copy docid
			memcpy(&mDocids[mNumDocs], docids, sizeof(u64) * numDocs);			
		}
	}
	else
	{
		// for values
		OmnAlarm << "mem expand: should not happen " << enderr;
		u64 *mem = OmnNew u64[newNumDocs + mExtraDocids];
		aos_assert_r(mem, false);
		memset(mem, 0, sizeof(u64) * (newNumDocs + mExtraDocids));
		// for docids
		u64 *mem2 = OmnNew u64[newNumDocs + mExtraDocids];
		aos_assert_r(mem2, false);
		memset(mem2 , 0, sizeof(u64) * (newNumDocs + mExtraDocids));
	
		mMemCap = newNumDocs;

		if(addToHead)
		{
			// copy value
			memmove(&(mem[numDocs]), mValues, sizeof(u64) * mNumDocs);
			memcpy(mem,values,sizeof(u64) * numDocs);
			// copy docid
			memmove(&(mem2[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mem2,docids,sizeof(u64) * numDocs);
			
		}
		else
		{
			// copy value
			memmove(mem, mValues, sizeof(u64) * mNumDocs);			
			memcpy(&(mem[mNumDocs]), values, sizeof(u64) * numDocs);			
			// copy docid
			memmove(mem2, mDocids, sizeof(u64) * mNumDocs);			
			memcpy(&(mem2[mNumDocs]), docids, sizeof(u64) * numDocs);			
		}
		OmnDelete [] mValues;
		OmnDelete [] mDocids;
			
		mValues = mem;
		mDocids = mem2;
	}
	mNumDocs = newNumDocs;
	mIsDirty = true;
	return true;
}


bool 
AosIILCompU64::removeDocFromSubiil(const int numRemove, const bool delFromHead)
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
		// copy value
		int ss = sizeof(u64) * (newsize);
		OmnMemCheck(mValues, ss);
		memmove(mValues, &(mValues[numRemove]), ss);
		ss = sizeof(u64)*numRemove;
		OmnMemCheck(&mValues[newsize], ss);
		memset(&(mValues[newsize]), 0, ss);

		// copy docid
		ss = sizeof(u64) * (newsize);
		OmnMemCheck(mDocids, ss); 
		memmove(mDocids, &(mDocids[numRemove]), ss);
		ss = sizeof(u64)*numRemove;
		OmnMemCheck(&mDocids[newsize], ss);
		memset(&(mDocids[newsize]), 0, ss);
	}
	else // del from tail
	{
		OmnMemCheck(&mValues[newsize], sizeof(u64)*numRemove);
		memset(&(mValues[newsize]),0,sizeof(u64)*numRemove);
		OmnMemCheck(&mDocids[newsize] , sizeof(u64)*numRemove);
		memset(&(mDocids[newsize]),0,sizeof(u64)*numRemove);		
	}
	mNumDocs = newsize;
	mIsDirty = true;
	return true;
}


bool
AosIILCompU64::removeSubiil(
		const int iilidx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilidx > 0 && iilidx < (int)mNumSubiils,false);
	aos_assert_r(mNumSubiils > 1,false);
	bool rslt = false;
	//1. return the iil
	if(mSubiils[iilidx])
	{
		aos_assert_r(mSubiils[iilidx]->getNumDocs() == 0,false);
		bool subIILReturned = false;
		rslt = AosIILMgrSelf->returnIILPublic(mSubiils[iilidx], iilmgrLocked, subIILReturned, rdata, true);
		aos_assert_r(rslt, false);
		aos_assert_r(subIILReturned, false);
		mSubiils[iilidx] = 0;
	}

	//2. proc mMinDocids and mMaxDocids;
	mMinDocids[iilidx] = 0;
	mMaxDocids[iilidx] = 0;

	if(iilidx < (int)mNumSubiils -1)
	{
		memmove(&(mMinDocids[iilidx]),&(mMinDocids[iilidx+1]),(sizeof(u64))*(mNumSubiils - iilidx - 1));
		memmove(&(mMaxDocids[iilidx]),&(mMaxDocids[iilidx+1]),(sizeof(u64))*(mNumSubiils - iilidx - 1));
	}
	mMinDocids[mNumSubiils-1] = 0;
	mMaxDocids[mNumSubiils-1] = 0;

	//3. proc mNumEntries mIILIds
	if(iilidx < (int)mNumDocs -1)
	{
		memmove(&(mNumEntries[iilidx]),&(mNumEntries[iilidx +1]),(sizeof(int)*(mNumSubiils - 1)));
		memmove(&(mIILIds[iilidx]),&(mIILIds[iilidx +1]),(sizeof(u64)*(mNumSubiils - 1)));
	}
	mNumEntries[mNumSubiils-1] = 0;
	mIILIds[mNumSubiils-1] = 0;
	
	//4. proc mSubiils
	for(int i = iilidx ;i < (int)mNumSubiils - 1;i++)
	{
		mSubiils[i] = mSubiils[i+1];// no matter it is null or not
		
		if(mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}
	mSubiils[mNumSubiils - 1] = 0;
	mNumSubiils --;

	//5. do some processing if the iil turns to be a single one 
	if(mNumSubiils == 1)
	{
		
		mRootIIL = 0;
		mNumSubiils = 0;
		mIILIdx = -1;
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
		if(mSubiils)
		{
			OmnDelete [] mSubiils;
			mSubiils = 0;
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
	}
	//6. set dirty flag
	mIsDirty = true;
	return true;	
}


bool
AosIILCompU64::resetSubIILInfo(const AosRundataPtr &rdata)
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
/*
bool
AosIILCompU64::saveIILsToFileSafe(const OmnFilePtr &file, u32 &crt_pos, const AosRundataPtr &rdata)
{
	// This function saves all the dirty subiils to 'file', starting
	// from 'crt_pos'. The entry format is:
	// 		length		(4 bytes)
	// 		contents	(variable)
	// 		length		
	// 		contents
	// 		...
	AOSLOCKTRACE(mLock);
	aos_assert_rl(isRootIIL(), mLock, false);
	
	bool rslt;
	for(int i=1; i<mNumSubiils; i++)
	{
		if(mSubiils[i].notNull())
		{
			rslt = mSubiils[i]->saveToFileSafe(file, crt_pos, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
	}

	AOSLOCKTRACE(mLock);
	if (!mIsDirty)
	{
		AOSUNLOCK(mLock);
		return true;
	}

	rslt = saveToFilePriv(file, crt_pos, rdata);
	aos_assert_rl(rslt, mLock, false);
	AOSUNLOCK(mLock);
	return true;
}
*/
AosBuffPtr 
AosIILCompU64::getBodyBuffProtected() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILStrSanityCheck(this), 0);
	//felicia, 2012/10/18
	//AosBuffPtr buff = OmnNew AosBuff(5000, 5000 AosMemoryCheckerArgs);
	AosBuffPtr buff = OmnNew AosBuff(5000 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	buff->setU32(mNumDocs);
	// Save mDocids
	for (u32 i=0; i<mNumDocs; i++)
	{
		buff->setU64(mDocids[i]);
	}

	// Save Subiil Index Data, if needed
	buff->setInt(mNumSubiils);
	// save it but not use it.
	buff->setInt(mIILIdx);
	if (isRootIIL())
	{
		// Save the subiil index data
		for (u32 i=0; i<mNumSubiils; i++)
		{
			buff->setU64(mMinDocids[i]);
			buff->setU64(mMaxDocids[i]);
			buff->setU64(mIILIds[i]);
			buff->setInt(mNumEntries[i]);
			aos_assert_r(mIILIds[i]>0, 0);
		}
	}

	// Save Values
	aos_assert_r(mNumDocs <= mMaxIILSize, 0);
	for (u32 i=0; i<mNumDocs; i++)
	{
		buff->setU64(mValues[i]);
	}
	return buff;
}


bool
AosIILCompU64::setContentsProtected(
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
	
	bool rslt = false;
	mNumDocs = buff->getU32(0);

	aos_assert_r(mNumDocs <= mMaxIILSize, false);
	aos_assert_r(prepareMemoryForReloading(iilmgrLocked), false);
	
	// 1. Retrieve mDocids
	aos_assert_r(setDocidContents(buff), false);

	// 2. Retrieve Subiil Index Data, if needed
	mNumSubiils = buff->getInt(-1);
	aos_assert_r(mNumSubiils >= 0, false);
	if(mNumSubiils < 0 || mNumSubiils > mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	// This place used to save IILIdx, now we get it out but not use it
	if(mNumSubiils > 1)
	{
		mIILIdx = 0;
	}
	else
	{
		mIILIdx = -1;
	}
	buff->getInt(-1);
	
	if(mNumSubiils > 1)
	{
		mRootIIL = this;
	}

	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
		mSubiils[0] = this;

		for (u32 i=0; i<mNumSubiils; i++)
		{
			mMinDocids[i] = buff->getU64(0);

			mMaxDocids[i] = buff->getU64(0);

            mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			mNumEntries[i] = buff->getInt(-1);
			aos_assert_r(mNumEntries[i] >= 0, false);
		}
	}

	for (u32 i=0; i<mNumDocs; i++)
	{
		mValues[i] = buff->getU64(0);
	}

	rslt =	AosIILStrSanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILCompU64::prepareMemoryForReloading(const bool iilmgrLocked)
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
			mValues[i] = 0;
		}

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


u64			
AosIILCompU64::getMinDocid() const
{
	aos_assert_r(mNumDocs > 0, AOS_INVDID);
	return mDocids[0];
}


u64			
AosIILCompU64::getMaxDocid() const
{
	aos_assert_r(mNumDocs > 0, AOS_INVDID);
	return mDocids[mNumDocs -1];
}


bool
AosIILCompU64::resetSpec()
{
	if (!mValues)
	{
		aos_assert_r(mNumDocs == 0, false);
		return true;
	}
	
	aos_assert_r(mValues, false);
	memset(mValues, 0, sizeof(u64)*mNumDocs);
	return true;
}


bool
AosIILCompU64::returnSubIILsPriv(
		const bool iilmgrLocked,
		bool &returned,
		const AosRundataPtr &rdata,
		const bool returnHeader)
{
	returned = true;
	if(!isRootIIL())
	{
		return true;
	}

	AosIILCompU64Ptr subiil = 0;

	bool subIILReturned = false;
	bool rslt = false;

	// return all sub IIL except itself
	for(u32 i = 1;i < mNumSubiils;i++)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil != this,false);
		if(subiil.notNull() && !subiil->isDirty())
		{
			rslt = AosIILMgrSelf->returnIILPublic(subiil,iilmgrLocked,subIILReturned, rdata, returnHeader);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}


bool
AosIILCompU64::saveSubIILToTransFileSafe(
		const AosDocTransPtr &docTrans, 
		const bool forcesafe, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if(isRootIIL())
	{
		// this iil is the root iil of the list
		for(u32 i = 1;i < mNumSubiils;i++)
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


bool
AosIILCompU64::saveSubIILToLocalFileSafe(
		const bool forcesafe, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if(isRootIIL())
	{
		// this iil is the root iil of the list
		for(u32 i = 1;i < mNumSubiils;i++)
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
AosIILCompU64::copyData(
        u64 values[AOSSENG_TORTURER_MAX_DOCS],
        u64 *docids,
        const int buff_len,
        int &num_docs,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if(!isRootIIL())
	{
		aos_assert_r(isSingleIIL(),false);
		return copyDataSingle(values,docids,buff_len,num_docs);
	}
	// rootiil
	int doc_limit = buff_len;
	int doc_copied;
    u32 idx = 0;
    for (u32 ss=0; ss<mNumSubiils; ss++)
    {
        AosIILCompU64Ptr subiil = getSubiilByIndex(ss, rdata);

//		doc_copied = doc_limit;
        rslt = subiil->copyDataSingle(&(values[idx]),&(docids[idx]),doc_limit,doc_copied);
		aos_assert_r(rslt, false);
		idx += doc_copied;
		doc_limit -= doc_copied;
	
    }
    num_docs = idx;
    return true;
}


bool
AosIILCompU64::copyDataSingle(
        u64 values[AOSSENG_TORTURER_MAX_DOCS],
        u64 *docids,
        const int buff_len,
        int &num_docs)
{
    u32 idx = 0;
    for (u32 mm=0; mm<mNumDocs; mm++)
    {
		aos_assert_r((int)idx < buff_len, false);
        values[idx] = mValues[mm];
        docids[idx] = mDocids[mm];
        idx++;
    }
    num_docs = idx;
    return true;
}


bool
AosIILCompU64::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	query_rslt->reset();
	u64 value = query_context->getU64Value();
	AosOpr opr = query_context->getOpr();

//	bool finished = false;
//	query_rslt->nextDocid(docid,finished,rdata);

	u64 docid = 0;
	int num_doc = query_rslt->getNumDocs();
	u64 *docids = query_rslt->getDocidsPtr();
	int doc_idx = 0;

	int subIILIdx = 0;

	bool keepSearch = false;
	
	if(!isRootIIL())
	{
		for(int i = 0;i <num_doc;i++)
		{
			docid = docids[i];
			if(checkDocSinglePriv(docid,value,opr,keepSearch ,rdata))
			{
				docids[doc_idx++] = docid;
			}
		}
		query_rslt->setNumDocs(doc_idx);
	}
	else // Root IIL
	{
		AosIILCompU64Ptr subiil;
		bool rslt = false;
		for(int i = 0;i <num_doc;i++)
		{
			docid = docids[i];
			subiil = getSubiil(docid, rdata);
			if (!subiil)
			{
				continue;
			}
			
			subIILIdx = subiil->getIILIdx();

			keepSearch = true;
			while(keepSearch)
			{
				if(subiil == this)
				{
					rslt = subiil->checkDocSinglePriv(docid,value,opr,keepSearch, rdata);
				}
				else
				{
					rslt = subiil->checkDocSingleSafe(docid,value,opr,keepSearch, rdata);
				}
				// already found
				if(rslt)
				{
					docids[doc_idx++] = docid;
					break;
				}
				
				if(!keepSearch)
				{
					break;
				}
				if(subIILIdx >= (int)mNumSubiils-1)
				{	
					break;
				}
				subIILIdx ++;
				subiil = getSubiilByIndex(subIILIdx, rdata);
				if (!subiil)
				{
					break;
				}
			}
		}
		query_rslt->setNumDocs(doc_idx);
	}

//	AosQueryRslt::exchangeContent(query_rslt,new_rslt);
	return true;
}


// Chen Ding, 01/03/2013
bool 
AosIILCompU64::retrieveQueryProcBlock(
		AosBuffPtr &buff, 
		const AosQueryContextObjPtr &context, 
		const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 01/03/2013
bool 
AosIILCompU64::retrieveNodeList(
		AosBuffPtr &buff, 
		const AosQueryContextObjPtr &context, 
		const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2013/01/14
bool 
AosIILCompU64::retrieveIILBitmap(
		const OmnString &iilname,
		AosBitmapObjPtr &bitmap, 
		const AosBitmapObjPtr &partial_bitmap, 
		const AosBitmapTreeObjPtr &bitmap_tree, 
		AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
#endif
