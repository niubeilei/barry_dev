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
#include "IILMgr/IILBigHit.h"

#include "IILMgr/IILMgr.h"


AosIILBigHit::AosIILBigHit()
:
AosIIL(eAosIILType_BigHit),
mMinDocids(0),
mMaxDocids(0),
mSubiils(0),
mLevel(0),
mUpdateLock(OmnNew OmnMutex())
{
	mTotalIILs[eAosIILType_BigHit]++;
	mDocBitmap = AosGetBitmap(); 
}

//liuwei_hit
AosIILBigHit::AosIILBigHit(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_BigHit, isPersis, iilname),
mMinDocids(0),
mMaxDocids(0),
mSubiils(0),
mLevel(0),
mUpdateLock(OmnNew OmnMutex())
{
	mTotalIILs[eAosIILType_BigHit]++;
	mDocBitmap = AosGetBitmap(); 
	// OmnScreen << "create iil, iilid:" << iilid 
	// 		<< ",total:" << mTotalIILs[eAosIILType_BigHit] << endl;
}

//liuwei_hit
AosIILBigHit::AosIILBigHit(
		const u64 &iilid, 
		const u32 siteid, 
		const AosDfmDocIILPtr &doc,     //Ketty 2012/11/15
		const AosRundataPtr &rdata) 
:
AosIIL(eAosIILType_BigHit),
mMinDocids(0),
mMaxDocids(0),
mSubiils(0),
mLevel(0),
mUpdateLock(OmnNew OmnMutex())
{
	mTotalIILs[eAosIILType_BigHit]++;
	mDocBitmap = AosGetBitmap(); 
	// OmnScreen << "create iil, iilid:" << iilid 
	// 		<< ",total:" << mTotalIILs[eAosIILType_BigHit] << endl;
	mIILID = iilid;
	mDfmDoc = doc;		// Ketty 2013/01/15
	//mIsGood = loadFromFilePriv(buff, siteid, iilmgrLocked, rdata);
	mIsGood = loadFromFilePriv(iilid, rdata);
}

//liuwei_hit
AosIILBigHit::~AosIILBigHit()
{
	AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_BigHit]--;
	// OmnScreen << "delete iil, iilid:" << mIILID 
	// 		<< ",total:" << mTotalIILs[eAosIILType_BigHit] << endl;

	if (isParentIIL())
	{
		for (i64 i=0; i<mNumSubiils; i++)
		{
			// Chen Ding, 08/15/2011
			if (mSubiils[i]) AosIILMgr::getSelf()->returnIILPublic(mSubiils[i].getPtr(), rdata);
			mSubiils[i] = 0;
		}

/* 		if (mMinVals)
		{
			OmnDelete [] mMinVals;
			mMinVals = 0;
		}
		if (mMaxVals)
		{
			OmnDelete [] mMaxVals;
			mMaxVals = 0;
		} 
*/

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
	mIILIds = 0;
	mSubiils = 0;
	mNumEntries = 0;
}

//liuwei_hit
bool
AosIILBigHit::addDocPriv(
		//const u64 &value, 
		const u64 &docid, 
		//const bool value_unique, 
		//const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	
	//bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, rdata);
	bool rslt = addDocRecPriv(docid, rdata);
	aos_assert_r(rslt, false);

	// Ketty 2013/01/15
	// comp not support yet.
	// add to compiil
	//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//rslt = compiil->addDocSafe(value, docid, false, false, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_r(rslt, false);
	return true;
}

//liuwei_hit
bool
AosIILBigHit::addDocRecPriv(
		//const u64 &value, 
		const u64 &docid, 
		//const bool value_unique, 
		//const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		//return addDocSinglePriv(value, docid, value_unique, docid_unique, rdata);
		return addDocSinglePriv(docid, rdata);
	}
	
	aos_assert_r(isParentIIL(), false);
	// AosIILBigHitPtr subiil = getSubiilPriv(value, docid, value_unique, docid_unique, 
	// 		false, false, rdata);
	// This function returns the subiil in which either the entry [value, docid]
	// exists or can be inserted into.
	
	//AosIILBigHitPtr subiil = getSubiil3Priv(value, docid, rdata);
	AosIILBigHitPtr subiil = getSubiil3Priv(docid, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
	
	//return subiil->addDocRecSafe(value, docid, value_unique, docid_unique, rdata);
	return subiil->addDocRecSafe(docid, rdata);
}

//liuwei_hit
bool
AosIILBigHit::removeDocPriv(
		//const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	//bool rslt = removeDocRecPriv(value, docid, rdata);
	bool rslt = removeDocRecPriv(docid, rdata);
	aos_assert_r(rslt, false);

	// Ketty 2013/01/15
	// comp not support yet.
	// remove from compiil
	//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//rslt = compiil->removeDocSafe(value, docid, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_r(rslt, false);
	return rslt;
}

//liuwei_hit
bool
AosIILBigHit::removeDocRecPriv(
		//const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	if (isLeafIIL())
	{
		//return removeDocSinglePriv(value, docid, rdata);
		return removeDocSinglePriv(docid, rdata);
	}
	
	aos_assert_r(isParentIIL(), false);
	//AosIILBigHitPtr subiil = getSubiilByCondPriv(value, docid, eAosOpr_eq, false, rdata);
	AosIILBigHitPtr subiil = getSubiilByCondPriv(docid, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
	
	//return subiil->removeDocRecSafe(value, docid, rdata);
	return subiil->removeDocRecSafe(docid, rdata);
}

//liuwei_hit
/*
bool
AosIILBigHit::modifyDocSafe(
		const u64 &oldvalue, 
		const u64 &newvalue, 
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
AosIILBigHit::incrementDocidPriv(
		const u64 &key,
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
	
	AosIILBigHitPtr subiil = getSubiilByCondPriv(key, eAosOpr_eq, false, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, 0);

	return subiil->incrementDocidSafe(key, value, inc_value, init_value, add_flag, rdata);
}
*/

//liuwei_hit
i64
AosIILBigHit::getTotalPriv(
		//const u64 &value, 
		//const AosOpr opr, 
		const AosRundataPtr &rdata)
{	
	if (isLeafIIL())
	{
		//return getTotalSinglePriv(value, opr, rdata);
		return getTotalSinglePriv(rdata);
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

	//AosIILBigHitPtr first_sub = getSubiilByCondPriv(value, opr, false, rdata);
	AosIILBigHitPtr first_sub = getSubiilByIndexPriv(0, rdata);
	if (!first_sub)
	{
		return 0;
	}
	aos_assert_r(first_sub->mLevel >= 0 && first_sub->mLevel == mLevel-1, 0);
	
	//AosIILBigHitPtr last_sub = getSubiilByCondPriv(value, opr, true, rdata);
	AosIILBigHitPtr last_sub = getSubiilByIndexPriv(mNumSubiils-1, rdata);
	aos_assert_r(last_sub, 0);
	aos_assert_r(last_sub->mLevel >= 0 && last_sub->mLevel == mLevel-1, 0);
	
	i64 total = first_sub->getTotalSafe(rdata);
	if (first_sub.getPtr() == last_sub.getPtr())
	{
		return total;
	}

	total += last_sub->getTotalSafe(rdata);
	i64 start_idx = first_sub->getIILIdx();
	i64 end_idx = last_sub->getIILIdx();
	for (i64 i=start_idx+1; i<end_idx; i++)
	{
		total += mNumEntries[i];
	}
	return total;
}

//liuwei_hit
/*
bool
AosIILBigHit::setValueDocUniquePriv(
		const u64 &key, 
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

	AosIILBigHitPtr subiil = getSubiilPriv(key, docid, true, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, false);

	return subiil->setValueDocUniqueSafe(key, docid, must_same, rdata);
}
*/

//liuwei_hit
u64
AosIILBigHit::getMinDocid() const
{
	if (isLeafIIL())
	{
		//aos_assert_r(mNumDocs > 0,0);
		//return mDocids[0];
		
		aos_assert_r(mNumDocs > 0,0);
		u64 docid;
		bool rslt = false;
		mDocBitmap->reset();
		rslt = mDocBitmap->nextDocid(docid);
		aos_assert_r(rslt, 0);
		return docid;
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinDocids[0];
}

//liuwei_hit
u64
AosIILBigHit::getMaxDocid() const
{
	if (isLeafIIL())
	{
		//aos_assert_r(mNumDocs > 0,0);
		//return mDocids[mNumDocs-1];
		
		aos_assert_r(mNumDocs > 0,0);
		u64 docid;
		bool rslt = false;
		mDocBitmap->reset();
		rslt = mDocBitmap->prevDocid(docid);
		aos_assert_r(rslt, 0);
		return docid;
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMaxDocids[mNumSubiils-1];
}

//liuwei_hit
/*
const u64
AosIILBigHit::getMinValue() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mValues[0];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinVals[0];
}


const u64
AosIILBigHit::getMaxValue() const
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
AosIILBigHit::checkDocidUnique(
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
AosIILBigHit::getTotalNEPriv(const u64 &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}
*/

//liuwei_hit
bool
AosIILBigHit::updateIndexData(
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

	AosIILBigHitPtr subiil = mSubiils[idx];
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
		//mMinVals[idx] = subiil->getMinValue();
		mMinDocids[idx] = subiil->getMinDocid();
	}

	if (changeMax)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		//mMaxVals[idx] = subiil->getMaxValue();
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
AosIILBigHit::hasMoreMembers(const AosIILIdx &idx) const
{
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);
	i64 iilidx = idx.getIdx(mLevel);
	return (iilidx >= 0 && iilidx < mNumSubiils);
}

//liuwei_hit
// Chen Ding, 04/22/2012
// New Functions
bool
AosIILBigHit::containDocidPriv(
		const u64 &min_docid, 
		const u64 &max_docid,
		const u64 &docid,
		const bool reverse, 
		bool &keep_search,
		const AosRundataPtr &rdata)
{
	// This function checks whether this iil may contain the 
	// value [value, opr]. 
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);

	return (docid >= min_docid && docid <= max_docid);
/*
	switch (opr)
	{
	//liuwei_hit???
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
*/
}

//liuwei_hit
bool
AosIILBigHit::resetMaxMin(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r((idx >= 0) && (idx < mNumSubiils), false);

	i64 numdoc_new;
	i64 move_num;
	AosIILBigHitPtr subiil;
	for(i64 i=mNumSubiils-1; i>=idx; i--)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil, false);
		
		numdoc_new = subiil->getNumDocs();
		mNumDocs = mNumDocs + numdoc_new - mNumEntries[i];
		if (numdoc_new > 0)
		{
			//mMinVals[i] = subiil->getMinValue();
			mMinDocids[i] = subiil->getMinDocid();

			//mMaxVals[i] = subiil->getMaxValue();
			mMaxDocids[i] = subiil->getMaxDocid();
		
			mNumEntries[i] = numdoc_new;
		}
		else
		{
			//mMinVals[i] = 0;
			mMinDocids[i] = 0;

			//mMaxVals[i] = 0;
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
				//memmove(&mMinVals[i], &mMinVals[i+1], sizeof(u64) * move_num);
				memmove(&mMaxDocids[i], &mMaxDocids[i+1], sizeof(u64) * move_num);
				//memmove(&mMaxVals[i], &mMaxVals[i+1], sizeof(u64) * move_num);
				memmove(&mMinDocids[i], &mMinDocids[i+1], sizeof(u64) * move_num);
				memmove(&mSubiils[i], &mSubiils[i+1], sizeof(AosIILBigHitPtr) * move_num);
				memmove(&mIILIds[i], &mIILIds[i+1], sizeof(u64) * move_num);
				memmove(&mNumEntries[i], &mNumEntries[i+1], sizeof(i64) * move_num);
			}
			mNumSubiils--;
		}
	}
	return true;
}





/*


bool
AosIILBigHit::removeFirstValueDocPriv(
		const u64 &value,
		u64 &docid,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	// This function removes the first entry whose value matches 'value'. 
	if (isLeafIIL()) return removeFirstValueDocSinglePriv(value, docid, reverse, rdata);

	AosIILBigHitPtr subiil = getSubiilByValuePriv(key, reverse, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, false);
	return subiil->removeFirstValueDoc(value, docid, reverse, rdata);
}





*/

