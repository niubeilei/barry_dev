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
#include "IILMgrBig/IILStr.h"

#include "IILMgrBig/IILMgr.h"


AosIILStr::AosIILStr(
		const bool isPersis,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_Str, isPersis),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0),
mIsNumAlpha(false),
mLevel(0),
mDistrType(eNoDistr)
{
	mTotalIILs[eAosIILType_Str] ++;
}


AosIILStr::AosIILStr(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_Str, isPersis, iilname),
mValues(0),
mMinVals(0),
mMinDocids(0),
mMaxVals(0),
mMaxDocids(0),
mSubiils(0),
mIsNumAlpha(false),
mLevel(0),
mDistrType(eNoDistr)
{
	mTotalIILs[eAosIILType_Str]++;
	// OmnScreen << "create iil, iilid:" << iilid 
	// 		<< ",total:" << mTotalIILs[eAosIILType_Str] << endl;
}


AosIILStr::AosIILStr(
		const u64 &iilid, 
		const u32 siteid, 
		const AosDfmDocIILPtr &doc,     //Ketty 2012/11/15
		const bool iilmgrLocked,
		const AosRundataPtr &rdata) 
:
AosIIL(eAosIILType_Str, false),
mValues(0),
mMinVals(0),
mMinDocids(0),
mMaxVals(0),
mMaxDocids(0),
mSubiils(0),
mIsNumAlpha(false),
mLevel(0),
mDistrType(eNoDistr)
{
	mTotalIILs[eAosIILType_Str]++;
	// OmnScreen << "create iil, iilid:" << iilid 
	// 		<< ",total:" << mTotalIILs[eAosIILType_Str] << endl;
	mIILID = iilid;
	mDfmDoc = doc;		// Ketty 2013/01/15
	//mIsGood = loadFromFilePriv(buff, siteid, iilmgrLocked, rdata);
	mIsGood = loadFromFilePriv(iilid, iilmgrLocked, rdata);
}


AosIILStr::~AosIILStr()
{
	AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_Str]--;
	// OmnScreen << "delete iil, iilid:" << mIILID 
	// 		<< ",total:" << mTotalIILs[eAosIILType_Str] << endl;

	if (mValues)
	{
		for (u32 i=0; i<mMemCap; i++)
		{
			if (mValues[i])
				OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
		}
		OmnMemRemove(mValues,sizeof(mValues));
		OmnDelete [] mValues;
		mValues = 0;
	}

	if (isParentIIL())
	{
		for (u32 i=0; i<mNumSubiils; i++)
		{
			// Chen Ding, 08/15/2011
			if (mSubiils[i]) AosIILMgr::getSelf()->returnIILPublic(mSubiils[i].getPtr(), rdata);
			mSubiils[i] = 0;
		}
		if(mMinVals)
		{
			for (u32 i = 0;i < mNumSubiils;i++)
			{
				if(mMinVals[i])
				{
					OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
					mMinVals[i] = 0;
				}
			}
			OmnDelete [] mMinVals;
			mMinVals = 0;
		}
		if(mMaxVals)
		{
			for (u32 i = 0;i < mNumSubiils;i++)
			{
				if(mMaxVals[i])
				{
					OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
					mMaxVals[i] = 0;
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
	mMinVals = 0;
	mMaxVals = 0;
	mIILIds = 0;
	mSubiils = 0;
	mNumEntries = 0;
}


bool
AosIILStr::addDocPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	
	aos_assert_rr(value != "", rdata, false);
	bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Ketty 2013/01/15
	// comp not support yet.
	// add to compiil
	//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//rslt = compiil->addDocSafe(value, docid, false, false, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_r(rslt, false);
	return true;
}


bool
AosIILStr::addDocRecPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(value != "", false);
	if (isLeafIIL())
	{
		return addDocSinglePriv(value, docid, value_unique, docid_unique, rdata);
	}
	
	aos_assert_r(isParentIIL(), false);
	// AosIILStrPtr subiil = getSubiilPriv(value, docid, value_unique, docid_unique, 
	// 		false, false, rdata);
	// This function returns the subiil in which either the entry [value, docid]
	// exists or can be inserted into.
	AosIILStrPtr subiil = getSubiil3Priv(value, docid, false, rdata);
	aos_assert_rr(subiil, rdata, false);
	aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, rdata, false);
	
	return subiil->addDocRecSafe(value, docid, value_unique, docid_unique, rdata);
}


bool
AosIILStr::removeDocPriv(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(value != "", false);
	
	bool rslt = removeDocRecPriv(value, docid, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Ketty 2013/01/15
	// comp not support yet.
	// remove from compiil
	//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//rslt = compiil->removeDocSafe(value, docid, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_r(rslt, false);
	return rslt;
}


bool
AosIILStr::removeDocRecPriv(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(value != "", false);
	if (isLeafIIL())
	{
		return removeDocSinglePriv(value, docid, rdata);
	}
	
	aos_assert_r(isParentIIL(), false);
	AosIILStrPtr subiil = getSubiilByCondPriv(value, docid, eAosOpr_eq, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, rdata, false);
	
	return subiil->removeDocRecSafe(value, docid, rdata);
}


bool
AosIILStr::modifyDocSafe(
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
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
AosIILStr::incrementDocidPriv(
		const OmnString &key,
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
	
	AosIILStrPtr subiil = getSubiilByCondPriv(key, eAosOpr_eq, false, rdata);
	aos_assert_rr(subiil, rdata, 0);
	aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, rdata, 0);

	return subiil->incrementDocidSafe(key, value, inc_value, init_value, add_flag, rdata);
}


u64
AosIILStr::getTotalPriv(
		const OmnString &value, 
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

	case eAosOpr_range: 
		 OmnNotImplementedYet;
		 return 0;

	case eAosOpr_epoch: 
		 OmnNotImplementedYet;
		 return 0;

	case eAosOpr_date:  
		 OmnNotImplementedYet;
		 return 0;

	case eAosOpr_prefix:
		 return getTotalPrefixPriv(value, rdata);

	case eAosOpr_like:
		 return getTotalLikePriv(value, rdata);

	default:
		 OmnAlarm << "unrecognized operator: " << opr << enderr;
		 return 0;
	}

	AosIILStrPtr first_sub = getSubiilByCondPriv(value, opr, false, rdata);
	if (!first_sub)
	{
		return 0;
	}
	aos_assert_rr(first_sub->mLevel >= 0 && first_sub->mLevel == mLevel-1, rdata, 0);
	
	AosIILStrPtr last_sub = getSubiilByCondPriv(value, opr, true, rdata);
	aos_assert_rr(last_sub, rdata, 0);
	aos_assert_rr(last_sub->mLevel >= 0 && last_sub->mLevel == mLevel-1, rdata, 0);
	
	u64 total = first_sub->getTotalSafe(value, opr, rdata);
	if (first_sub.getPtr() == last_sub.getPtr())
	{
		return total;
	}

	total += last_sub->getTotalSafe(value, opr, rdata);
	int start_idx = first_sub->getIILIdx();
	int end_idx = last_sub->getIILIdx();
	for (int i=start_idx+1; i<end_idx; i++)
	{
		total += mNumEntries[i];
	}
	return total;

	/* Below is the old implementation of getTotal(...) for root
	 * Copied from getTotalSafe(const OmnString &value, const AosOpr &opr);
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

	int total = 0;
	if(isRootIIL())
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
			 for (int i=mNumSubiils-1; i>=0; i--)
			 {
				 if (AosIILUtil::valueMatch(mMinVals[i], opr, value.data(), mIsNumAlpha))
				 {
					 for (int k=0; k<i; k++)
					 {
						 total += mNumEntries[k];
					 }
					 // add total of subiil i
					 if(i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);
				 }
			 }
			 break;

		case eAosOpr_gt:
		case eAosOpr_ge:
			 // Find the first subiil that meets:
			 // 	value <= mMaxVals[i]
			 for (u32 i=0; i<mNumSubiils; i++)
			 {
				 if (AosIILUtil::valueMatch(value, opr, mMaxVals[i], mIsNumAlpha))
				 {
					 for (u32 j=i+1; j<mNumSubiils; j++)
					 {
						 total += mNumEntries[j];
					 }
					 // add total of subiil i
					 if(i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);
				 }
			 }
			 break;

		case eAosOpr_eq:
			 // Find the first subiil that meets:
			 // 	mMinVals[i] <= value
			 // and then go over until:
			 // 	value <= mMaxVals[j]
			 for (u32 i=0; i<mNumSubiils; i++)
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value.data(), mIsNumAlpha))
				 {
					 // add total of subiil i
					 if(i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);

					 for (u32 j=i+1; j<mNumSubiils; j++)
					 {
						 if(AosIILUtil::valueMatch(mMaxVals[j], eAosOpr_eq, value.data(), mIsNumAlpha))
						 {
							 total += mNumEntries[j];
						 }
						 else
						 {
							 // add total of subiil j
							 if(j == 0)total += mSubiils[j]->getTotalPriv(value,opr);
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
			 for (u32 i=0; i<mNumSubiils; i++)
			 {
				 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value, mIsNumAlpha) &&
				 	 AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, mIsNumAlpha))
				 {
					 // add total of subiil i
					 if(i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);
				 }
				 else
				 {
					 total += mNumEntries[i];
				 }
			 }
			 break;

		case eAosOpr_an:
			 for (u32 i=0; i<mNumSubiils; i++)
			 {
				 total += mNumEntries[i];
			 }
			 break;

		case eAosOpr_like:
			 // Ken, 2011/5/25
			 for (u32 i=0; i<mNumSubiils; i++)
			 {
				 // add total of subiil i
				 if(i == 0) total += mSubiils[i]->getTotalPriv(value, opr);
				 else total += mSubiils[i]->getTotalSafe(value, opr);
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
AosIILStr::setValueDocUniquePriv(
		const OmnString &key, 
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

	AosIILStrPtr subiil = getSubiilPriv(key, docid, true, false, false, rdata);
	aos_assert_rr(subiil, rdata, false);
	aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, rdata, false);

	return subiil->setValueDocUniqueSafe(key, docid, must_same, rdata);
}


u64
AosIILStr::getMinDocid() const
{
	if(isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mDocids[0];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinDocids[0];
}


u64
AosIILStr::getMaxDocid() const
{
	if(isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mDocids[mNumDocs-1];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMaxDocids[mNumSubiils-1];
}


const char *
AosIILStr::getMinValue() const
{
	if(isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mValues[0];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinVals[0];
}


const char *
AosIILStr::getMaxValue() const
{
	if(isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mValues[mNumDocs-1];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMaxVals[mNumSubiils-1];
}


bool
AosIILStr::checkDocidUnique(
		const bool rslt, 
		const int idx, 
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	isunique = true;
	return true;
}


u64
AosIILStr::getTotalNEPriv(const OmnString &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


u64
AosIILStr::getTotalLikePriv(const OmnString &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


u64
AosIILStr::getTotalPrefixPriv(const OmnString &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosIILStr::updateIndexData(
		const int idx,
		const bool changeMax,
		const bool changeMin,
		const AosRundataPtr &rdata)
{
//shawn not finished
//shawn check if the idx is the last one, if (idx == mNumSubiils-1 && hasParent()), update it's parent
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	aos_assert_r(idx >= 0 && idx < (int)mNumSubiils, false);
	setDirty(true);

	AosIILStrPtr subiil = mSubiils[idx];
	aos_assert_r(mSubiils[idx], false);

	// note: update mNumDocs as the sum of all the subiils doc num.
	int numdoc_new = subiil->getNumDocs();
	mNumDocs = mNumDocs + numdoc_new - mNumEntries[idx];
	mNumEntries[idx] = numdoc_new;
	
	int m = 0;	
	for(u32 i=0; i<mNumSubiils; i++)
	{
		m += mNumEntries[i];
	}
	aos_assert_r(mNumDocs == (u32)m, false);
	
	if(changeMin)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		AosCopyMemStr(&mMinVals[idx], subiil->getMinValue(), __FILE__, __LINE__);
		aos_assert_r(mMinVals[idx], false);
		mMinDocids[idx] = subiil->getMinDocid();
	}

	if(changeMax)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		AosCopyMemStr(&mMaxVals[idx], subiil->getMaxValue(), __FILE__, __LINE__);
		aos_assert_r(mMaxVals[idx], false);
		mMaxDocids[idx] = subiil->getMaxDocid();
	}

	if(mParentIIL)
	{
		bool rslt = mParentIIL->updateIndexData(mIILIdx, changeMax, changeMin, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


// Chen Ding, 04/22/2012
bool
AosIILStr::hasMoreMembers(const AosIILIdx &idx) const
{
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);
	int iilidx = idx.getIdx(mLevel);
	return (iilidx >= 0 && (u32)iilidx < mNumSubiils);
}


// Chen Ding, 04/22/2012
// New Functions
bool
AosIILStr::containValuePriv(
		const char *min_value, 
		const char *max_value,
		const OmnString &value, 
		const AosOpr opr, 
		const bool reverse, 
		bool &keep_search,
		const AosRundataPtr &rdata)
{
	// This function checks whether this iil may contain the 
	// value [value, opr]. 
	aos_assert_rr(!isLeafIIL(), rdata, false);
	aos_assert_rr(mNumSubiils > 0, rdata, false);

	bool rslt;
	int nn;
	switch (opr)
	{
	case eAosOpr_gt:
	case eAosOpr_ge:
		 // This means that it wants to find the values that are greater
		 // than 'value'. For instance, for Age IIL, we want to find all 
		 // the docs whoseage is greater than 'value'. It returns true
		 // if its max value is greater than 'value'.
		 rslt = AosIILUtil::valueMatch(max_value, opr, value.data(), mIsNumAlpha);
		 if (rslt) return true;

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
		 rslt = AosIILUtil::valueMatch(min_value, opr, value.data(), mIsNumAlpha);
		 if (rslt) return true;
		 
		 // If it is normal search, there is no need to continue searching.
		 // Otherwise, it should.
		 keep_search = reverse;
		 return false;

	case eAosOpr_eq:
		 if (AosIILUtil::valueMatch(min_value, eAosOpr_le, value.data(), mIsNumAlpha))
		 {
			 if (AosIILUtil::valueMatch(value.data(), eAosOpr_le, max_value, mIsNumAlpha))
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
		 return AosIILUtil::valueMatch(min_value, eAosOpr_ne, value.data(), mIsNumAlpha) ||
		 		AosIILUtil::valueMatch(max_value, eAosOpr_ne, value.data(), mIsNumAlpha);

	case eAosOpr_an:
		 return true;

	case eAosOpr_Objid:
	case eAosOpr_range:
	case eAosOpr_date:
	case eAosOpr_epoch:
		 OmnShouldNeverComeHere;
		 return false;

	case eAosOpr_prefix:
		 // Assume 'aaa' is the prefix, if the min_value is bigger than 'aaa'
		 // or the max_value is less than 'aaa', it returns false. 
		 // Otherwise, it returns true. Note that returning true does not
		 // mean the subiil [min_value, max_value] definitely contains 
		 // the value, but only possible.
		 nn = strncmp(min_value, value.data(), value.length());
		 if (nn > 0) 
		 {
			 // min_value > value, not possible.
			 // if it is normal order, no need to continue searching.
			 keep_search = reverse;
			 return false;
		 }

		 // This means min_value <= value, it is possible, but need to 
		 // check the max_value.
		 nn = strncmp(max_value, value.data(), value.length());
		 if (nn < 0) 
		 {
			 // max_value < value, not possible. 
			 // If it is normal order, need to continue searching.
			 keep_search = !reverse;
			 return false;
		 }

		 // This means min_value <= value && value <= max_value.
		 return true;

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
AosIILStr::resetMaxMin(
		const int idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	aos_assert_r((idx >= 0) && (idx < (int)mNumSubiils), false);

	int numdoc_new;
	int move_num;
	AosIILStrPtr subiil;
	for(int i=mNumSubiils-1; i>=idx; i--)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil, false);
		
		numdoc_new = subiil->getNumDocs();
		mNumDocs = mNumDocs + numdoc_new - mNumEntries[i];
		if(numdoc_new > 0)
		{
			AosCopyMemStr(&mMinVals[i], subiil->getMinValue(), __FILE__, __LINE__);
			aos_assert_r(mMinVals[i], false);
			mMinDocids[i] = subiil->getMinDocid();
		
			AosCopyMemStr(&mMaxVals[i], subiil->getMaxValue(), __FILE__, __LINE__);
			aos_assert_r(mMaxVals[i], false);
			mMaxDocids[i] = subiil->getMaxDocid();
		
			mNumEntries[i] = numdoc_new;
		}
		else
		{
			OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			mMinVals[i] = 0;
			mMinDocids[i] = 0;
			
			OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			mMaxVals[i] = 0;
			mMaxDocids[i] = 0;

			subiil->deleteIILRecSafe(iilmgrLocked, rdata);
			AosIILMgr::getSelf()->returnIILPublic(subiil, iilmgrLocked, rdata);
			mSubiils[i] = 0;
			mIILIds[i] = 0;
			mNumEntries[i] = 0;

			move_num = mNumSubiils - 1 - i;
			if(move_num != 0)
			{
				memmove(&mMinVals[i], &mMinVals[i+1], sizeof(char*) * move_num);
				memmove(&mMaxDocids[i], &mMaxDocids[i+1], sizeof(u64) * move_num);
				memmove(&mMaxVals[i], &mMaxVals[i+1], sizeof(char*) * move_num);
				memmove(&mMinDocids[i], &mMinDocids[i+1], sizeof(u64) * move_num);
				memmove(&mSubiils[i], &mSubiils[i+1], sizeof(AosIILStrPtr*) * move_num);
				memmove(&mIILIds[i], &mIILIds[i+1], sizeof(u64) * move_num);
				memmove(&mNumEntries[i], &mNumEntries[i+1], sizeof(u64) * move_num);
			}
			mNumSubiils--;
		}
	}
	return true;
}





/*


bool
AosIILStr::removeFirstValueDocPriv(
		const OmnString &value,
		u64 &docid,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	// This function removes the first entry whose value matches 'value'. 
	if (isLeafIIL()) return removeFirstValueDocSinglePriv(value, docid, reverse, rdata);

	AosIILStrPtr subiil = getSubiilByValuePriv(key, reverse, rdata);
	aos_assert_rr(subiil, rdata, false);
	aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, rdata, false);
	return subiil->removeFirstValueDoc(value, docid, reverse, rdata);
}





*/
