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
#include "IILMgr/IILBigI64.h"

#include "IILMgr/IILMgr.h"


AosIILBigI64::AosIILBigI64()
:
AosIIL(eAosIILType_BigI64),
mValues(0),
mMinVals(0),
mMinDocids(0),
mMaxVals(0),
mMaxDocids(0),
mSubiils(0),
mLevel(0),
mUpdateLock(OmnNew OmnMutex())
{
	mTotalIILs[eAosIILType_BigI64]++;
}


AosIILBigI64::AosIILBigI64(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_BigI64, isPersis, iilname),
mValues(0),
mMinVals(0),
mMinDocids(0),
mMaxVals(0),
mMaxDocids(0),
mSubiils(0),
mLevel(0),
mUpdateLock(OmnNew OmnMutex())
{
	mTotalIILs[eAosIILType_BigI64]++;
	// OmnScreen << "create iil, iilid:" << iilid 
	// 		<< ",total:" << mTotalIILs[eAosIILType_BigI64] << endl;
}


AosIILBigI64::AosIILBigI64(
		const u64 &iilid, 
		const u32 siteid, 
		const AosDfmDocIILPtr &doc,     //Ketty 2012/11/15
		const AosRundataPtr &rdata) 
:
AosIIL(eAosIILType_BigI64),
mValues(0),
mMinVals(0),
mMinDocids(0),
mMaxVals(0),
mMaxDocids(0),
mSubiils(0),
mLevel(0),
mUpdateLock(OmnNew OmnMutex())
{
	mTotalIILs[eAosIILType_BigI64]++;
	// OmnScreen << "create iil, iilid:" << iilid 
	// 		<< ",total:" << mTotalIILs[eAosIILType_BigI64] << endl;
	mIILID = iilid;
	mDfmDoc = doc;		// Ketty 2013/01/15
	//mIsGood = loadFromFilePriv(buff, siteid, iilmgrLocked, rdata);
	mIsGood = loadFromFilePriv(iilid, rdata);
}


AosIILBigI64::~AosIILBigI64()
{
	AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_BigI64]--;
	// OmnScreen << "delete iil, iilid:" << mIILID 
	// 		<< ",total:" << mTotalIILs[eAosIILType_BigI64] << endl;

	if (mValues)
	{
		OmnDelete [] mValues;
		mValues = 0;
	}

	if (isParentIIL())
	{
		for (i64 i=0; i<mNumSubiils; i++)
		{
			// Chen Ding, 08/15/2011
			if (mSubiils[i]) AosIILMgr::getSelf()->returnIILPublic(mSubiils[i].getPtr(), rdata);
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
		if (mNumEntries)
		{
			OmnDelete [] mNumEntries;
			mNumEntries = 0;
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
AosIILBigI64::addDocPriv(
		const i64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	
	bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILBigI64::addDocRecPriv(
		const i64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return addDocSinglePriv(value, docid, value_unique, docid_unique, rdata);
	}
	
	aos_assert_r(isParentIIL(), false);
	// AosIILBigI64Ptr subiil = getSubiilPriv(value, docid, value_unique, docid_unique, 
	// 		false, false, rdata);
	// This function returns the subiil in which either the entry [value, docid]
	// exists or can be inserted into.
	AosIILBigI64Ptr subiil = getSubiil3Priv(value, docid, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
	
	return subiil->addDocRecSafe(value, docid, value_unique, docid_unique, rdata);
}


bool
AosIILBigI64::removeDocPriv(
		const i64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	bool rslt = removeDocRecPriv(value, docid, rdata);
	aos_assert_r(rslt, false);

	return rslt;
}


bool
AosIILBigI64::removeDocRecPriv(
		const i64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	if (isLeafIIL())
	{
		return removeDocSinglePriv(value, docid, rdata);
	}
	
	aos_assert_r(isParentIIL(), false);
	AosIILBigI64Ptr subiil = getSubiilByCondPriv(value, docid, eAosOpr_eq, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
	
	return subiil->removeDocRecSafe(value, docid, rdata);
}


bool
AosIILBigI64::modifyDocSafe(
		const i64 &oldvalue, 
		const i64 &newvalue, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	// This means that there shall be an entry in the IIL and the entry's
	// value should match 'oldvalue'. 
	//
	// IMPORTANT!!!!!!!!!!
	// There is a potential problem: if for some reason the system data
	// are inconsistent, there IS an entry but the entry's value does not
	// match 'oldvalue', it will not be deleted. A new entry will be 
	// added. Need to think about how to prevent this from happening.
	AOSLOCK(mLock);
	bool rslt1 = removeDocPriv(oldvalue, docid, rdata);
	if (!rslt1)
	{
		OmnAlarm << "Failed to remove: " << docid << ":" << oldvalue 
			<< ":" << newvalue << enderr;
	}
	
	bool rslt2 = addDocPriv(newvalue, docid, value_unique, docid_unique, rdata);
	if (!rslt2)
	{
		OmnAlarm << "Failed to add: " << docid << ":" << oldvalue 
			<< ":" << newvalue << enderr;
	}
	AOSUNLOCK(mLock);
	return rslt1 && rslt2;
}


bool
AosIILBigI64::incrementDocidPriv(
		const i64 &key,
		u64 &value,
		const u64 &inc_value,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return incrementDocidSinglePriv(key, value, inc_value, init_value, add_flag, rdata);
	}
	
	AosIILBigI64Ptr subiil = getSubiilByCondPriv(key, eAosOpr_eq, false, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, 0);

	return subiil->incrementDocidSafe(key, value, inc_value, init_value, add_flag, rdata);
}


i64
AosIILBigI64::getTotalPriv(
		const i64 &value, 
		const AosOpr opr, 
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return getTotalSinglePriv(value, opr, rdata);
	}

	// It returns the number of docids that match the condition:
	// 		mValues[i] opr value
	// If opr is '<', for instance, it returns the number of docids
	// that are:
	// 		mValues[i] < value
	//
	// Performance Analysis:
	// Since values are sorted, the search is log(n), which is fast
	// enough.

	switch (opr)
	{
	case eAosOpr_ne:
		 return getTotalNEPriv(value, rdata);

	case eAosOpr_gt:
	case eAosOpr_ge:    
	case eAosOpr_eq:    
	case eAosOpr_lt:    
	case eAosOpr_le:    
	case eAosOpr_an:    
		 break;

	case eAosOpr_Objid:
		 OmnNotImplementedYet;
		 return 0;

	// Chen Ding, CHENDING20130920
	// case eAosOpr_range: 
	// 	 OmnNotImplementedYet;
	// 	 return 0;

	case eAosOpr_epoch: 
		 OmnNotImplementedYet;
		 return 0;

	case eAosOpr_date:  
		 OmnNotImplementedYet;
		 return 0;

	default:
		 OmnAlarm << "unrecognized operator: " << opr << enderr;
		 return 0;
	}

	AosIILBigI64Ptr first_sub = getSubiilByCondPriv(value, opr, false, rdata);
	if (!first_sub)
	{
		return 0;
	}
	aos_assert_r(first_sub->mLevel >= 0 && first_sub->mLevel == mLevel-1, 0);
	
	AosIILBigI64Ptr last_sub = getSubiilByCondPriv(value, opr, true, rdata);
	aos_assert_r(last_sub, 0);
	aos_assert_r(last_sub->mLevel >= 0 && last_sub->mLevel == mLevel-1, 0);
	
	i64 total = first_sub->getTotalSafe(value, opr, rdata);
	if (first_sub.getPtr() == last_sub.getPtr())
	{
		return total;
	}

	total += last_sub->getTotalSafe(value, opr, rdata);
	i64 start_idx = first_sub->getIILIdx();
	i64 end_idx = last_sub->getIILIdx();
	for (i64 i=start_idx+1; i<end_idx; i++)
	{
		total += mNumEntries[i];
	}
	return total;

	/* Below is the old implementation of getTotal(...) for root
	 * Copied from getTotalSafe(const u64 &value, const AosOpr &opr);
	 *
	// It returns the number of docids that match the condition:
	// 		mValues[i] opr value
	// If opr is '<', for instance, it returns the number of docids
	// that are:
	// 		mValues[i] < value
	//
	// Performance Analysis:
	// Since values are sorted, the search is log(n), which is fast
	// enough.

	aos_assert_r(isLeafIIL(), rdata, 0);
	AOSLOCK(mLock);
	aos_assert_r(mNumDocs <= mMemCap, false);

	i64 total = 0;
	if (isRootIIL())
	{
		// This is a segmented IIL. Need to find the first subiil
		// based on 'value'. It then collects the total until 
		// the last subiil determined by 'value'.
		//
		// IMPORTANT: Since get total is not very important, for 
		// segmented IILs, we will estimate it by the min and max
		// values. 
		switch (opr)
		{
		case eAosOpr_le:
		case eAosOpr_lt:
			 // Find the first subiil that meets:
			 // 	mMinVals[i] <= value
			 for (i64 i=mNumSubiils-1; i>=0; i--)
			 {
				 if (AosIILUtil::valueMatch(mMinVals[i], opr, value.data()))
				 {
					 for (i64 k=0; k<i; k++)
					 {
						 total += mNumEntries[k];
					 }
					 // add total of subiil i
					 if (i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);
				 }
			 }
			 break;

		case eAosOpr_gt:
		case eAosOpr_ge:
			 // Find the first subiil that meets:
			 // 	value <= mMaxVals[i]
			 for (i64 i=0; i<mNumSubiils; i++)
			 {
				 if (AosIILUtil::valueMatch(value, opr, mMaxVals[i]))
				 {
					 for (i64 j=i+1; j<mNumSubiils; j++)
					 {
						 total += mNumEntries[j];
					 }
					 // add total of subiil i
					 if (i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);
				 }
			 }
			 break;

		case eAosOpr_eq:
			 // Find the first subiil that meets:
			 // 	mMinVals[i] <= value
			 // and then go over until:
			 // 	value <= mMaxVals[j]
			 for (i64 i=0; i<mNumSubiils; i++)
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value.data()))
				 {
					 // add total of subiil i
					 if (i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);

					 for (i64 j=i+1; j<mNumSubiils; j++)
					 {
						 if (AosIILUtil::valueMatch(mMaxVals[j], eAosOpr_eq, value.data()))
						 {
							 total += mNumEntries[j];
						 }
						 else
						 {
							 // add total of subiil j
							 if (j == 0)total += mSubiils[j]->getTotalPriv(value,opr);
							 else total += mSubiils[j]->getTotalSafe(value,opr);

							 break;
						 }
					 }
					 break;
				 }
			 }
			 break;
			 
		case eAosOpr_ne:
			 // Skip the ones that:
			 // 	mMinVals[i] == value or 
			 // 	mMaxVals[i] == value
			 for (i64 i=0; i<mNumSubiils; i++)
			 {
				 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value) &&
				 	 AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value))
				 {
					 // add total of subiil i
					 if (i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);
				 }
				 else
				 {
					 total += mNumEntries[i];
				 }
			 }
			 break;

		case eAosOpr_an:
			 for (i64 i=0; i<mNumSubiils; i++)
			 {
				 total += mNumEntries[i];
			 }
			 break;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 AOSUNLOCK(mLock);
			 return -1;
		}
		AOSUNLOCK(mLock);
		return total;
	}
	*/
}


bool
AosIILBigI64::setValueDocUniquePriv(
		const i64 &key, 
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	// shawn call subiil setDocid function instead of change memory directly
	// saperate single function and root function
	//
	// If the list contains [key, docid], override 'docid'. Otherwise, 
	// it adds an entry [key, docid].

	if (isLeafIIL()) 
	{
		return setValueDocUniqueSinglePriv(key, docid, must_same, rdata);
	}

	AosIILBigI64Ptr subiil = getSubiilPriv(key, docid, true, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, false);

	return subiil->setValueDocUniqueSafe(key, docid, must_same, rdata);
}


u64
AosIILBigI64::getMinDocid() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mDocids[0];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinDocids[0];
}


u64
AosIILBigI64::getMaxDocid() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mDocids[mNumDocs-1];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMaxDocids[mNumSubiils-1];
}


const i64
AosIILBigI64::getMinValue() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mValues[0];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinVals[0];
}


const i64
AosIILBigI64::getMaxValue() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mValues[mNumDocs-1];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMaxVals[mNumSubiils-1];
}


bool
AosIILBigI64::checkDocidUnique(
		const bool rslt, 
		const i64 &idx, 
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	isunique = true;
	return true;
}


i64
AosIILBigI64::getTotalNEPriv(const i64 &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosIILBigI64::updateIndexData(
		const i64 &idx,
		const bool changeMax,
		const bool changeMin,
		const AosRundataPtr &rdata)
{
//shawn not finished
//shawn check if the idx is the last one, if (idx == mNumSubiils-1 && hasParent()), update it's parent
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);
	setDirty(true);

	AosIILBigI64Ptr subiil = mSubiils[idx];
	aos_assert_r(mSubiils[idx], false);

	// note: update mNumDocs as the sum of all the subiils doc num.
	i64 numdoc_new = subiil->getNumDocs();
	mNumDocs = mNumDocs + numdoc_new - mNumEntries[idx];
	mNumEntries[idx] = numdoc_new;
	
	i64 m = 0;	
	for(i64 i=0; i<mNumSubiils; i++)
	{
		m += mNumEntries[i];
	}

	//aos_assert_r(mNumDocs == m, false);
	if (mNumDocs != m)
	{
		OmnAlarm << "mNumDocs not match:" << mNumDocs << "," << m << enderr;
		mNumDocs = m;
	}
	
	if (changeMin)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		mMinVals[idx] = subiil->getMinValue();
		mMinDocids[idx] = subiil->getMinDocid();
	}

	if (changeMax)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		mMaxVals[idx] = subiil->getMaxValue();
		mMaxDocids[idx] = subiil->getMaxDocid();
	}

	if (mParentIIL)
	{
		bool rslt = mParentIIL->updateIndexData(mIILIdx, changeMax, changeMin, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


// Chen Ding, 04/22/2012
bool
AosIILBigI64::hasMoreMembers(const AosIILIdx &idx) const
{
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);
	i64 iilidx = idx.getIdx(mLevel);
	return (iilidx >= 0 && iilidx < mNumSubiils);
}


// Chen Ding, 04/22/2012
// New Functions
bool
AosIILBigI64::containValuePriv(
		const i64 &min_value, 
		const i64 &max_value,
		const i64 &value, 
		const AosOpr opr, 
		const bool reverse, 
		bool &keep_search,
		const AosRundataPtr &rdata)
{
	// This function checks whether this iil may contain the 
	// value [value, opr]. 
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);

	switch (opr)
	{
	case eAosOpr_gt:
	case eAosOpr_ge:
		 // This means that it wants to find the values that are greater
		 // than 'value'. For instance, for Age IIL, we want to find all 
		 // the docs whoseage is greater than 'value'. It returns true
		 // if its max value is greater than 'value'.
		 if (max_value >= value)
		 {
		 	return true;
		 }
		 // If it is normal order, it needs to keep on search. If it is 
		 // reverse, there is no need to search.
		 keep_search = !reverse;
		 return false;

	case eAosOpr_lt:
	case eAosOpr_le:
		 // This means we want to find the IILs whose values are less than
		 // 'value'. For instance, if this IIL is Age, and value is 30, we 
		 // want to find all the docs whose age is less than 30. It returns
		 // true if its min value is less than 'value'.
		 if (min_value <= value) return true;
		 
		 // If it is normal search, there is no need to continue searching.
		 // Otherwise, it should.
		 keep_search = reverse;
		 return false;

	case eAosOpr_eq:
		 if (min_value <= value)
		 {
			 if (value <= max_value)
			 {
				 return true;
			 }
			 
			 // This means 'value' > max_value, which means it is not
			 // possible for this subiil to contain 'value'. If it
			 // is normal order, there is no need to continue searching.
			 // Otherwise, it should.
			 keep_search = reverse;
			 return false;
		 }

		 // This means min_value > value, not possible for this subiil
		 // to contain 'value'. If it is normal order, there is no need
		 // to continue searching. Otherwise, it should.
		 keep_search = !reverse;
		 return false;

	case eAosOpr_ne:
		 keep_search = true;
		 return (min_value != value) || (max_value != value);

	case eAosOpr_an:
		 return true;

	case eAosOpr_Objid:
	case eAosOpr_date:
	case eAosOpr_epoch:
		 OmnShouldNeverComeHere;
		 return false;

	// Chen Ding, CHENDING20130920
	// case eAosOpr_range:
	
	case eAosOpr_like:
		 return true;

	default:
		 break;
	}
	 
	AosSetError(rdata, "unrecognized_opr") << ": " << opr;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


bool
AosIILBigI64::resetMaxMin(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r((idx >= 0) && (idx < mNumSubiils), false);

	i64 numdoc_new;
	i64 move_num;
	AosIILBigI64Ptr subiil;
	for(i64 i=mNumSubiils-1; i>=idx; i--)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil, false);
		
		numdoc_new = subiil->getNumDocs();
		mNumDocs = mNumDocs + numdoc_new - mNumEntries[i];
		if (numdoc_new > 0)
		{
			mMinVals[i] = subiil->getMinValue();
			mMinDocids[i] = subiil->getMinDocid();

			mMaxVals[i] = subiil->getMaxValue();
			mMaxDocids[i] = subiil->getMaxDocid();
		
			mNumEntries[i] = numdoc_new;
		}
		else
		{
			mMinVals[i] = 0;
			mMinDocids[i] = 0;

			mMaxVals[i] = 0;
			mMaxDocids[i] = 0;
			
			subiil->deleteIILSafe(true, rdata);
			//subiil->deleteIILRecSafe(rdata);
			AosIILMgr::getSelf()->returnIILPublic(subiil, rdata);
			
			mSubiils[i] = 0;
			mIILIds[i] = 0;
			mNumEntries[i] = 0;

			move_num = mNumSubiils - 1 - i;
			if (move_num != 0)
			{
				memmove(&mMinVals[i], &mMinVals[i+1], sizeof(i64) * move_num);
				memmove(&mMaxDocids[i], &mMaxDocids[i+1], sizeof(u64) * move_num);
				memmove(&mMaxVals[i], &mMaxVals[i+1], sizeof(i64) * move_num);
				memmove(&mMinDocids[i], &mMinDocids[i+1], sizeof(u64) * move_num);
				memmove(&mSubiils[i], &mSubiils[i+1], sizeof(AosIILBigI64Ptr) * move_num);
				memmove(&mIILIds[i], &mIILIds[i+1], sizeof(u64) * move_num);
				memmove(&mNumEntries[i], &mNumEntries[i+1], sizeof(i64) * move_num);
			}
			mNumSubiils--;
		}
	}
	return true;
}






