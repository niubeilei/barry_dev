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
// 	Created By Ken Lee, 2014/09/11
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/JimoTable.h"

#include "IILMgr/IILMgr.h"


AosJimoTable::AosJimoTable()
:
mRefcount(0),
mIILType(eAosIILType_JimoTable),
mIILID(0),
mSiteid(AOS_INVIILID),
mWordId(0),
mFlag(0),
mHitcount(0),
mVersion(0),
mIsPersis(false),
mCompressedSize(0),
mLock(OmnNew OmnMutex()),
mIsDirty(true),
mIsNew(false),
mIsGood(true),
mNumDocs(0),
mMemSize(0),
mLevel(0),
mIILIdx(-1),
mNumSubiils(0),
mSubiils(0),
mIILIds(0),	
mNumEntries(0),
mSubMemSize(0),
mSnapShotId(0),
mUpdateLock(OmnNew OmnMutex())
{
	AosIIL::sgLock->lock();
	AosIIL::sgNumIILCreated++;
	AosIIL::mTotalIILs[eAosIILType_JimoTable]++;
	//OmnScreen << "---jimo table created:" << mIILID << "," << sgNumIILCreated << "," << this << endl;
	AosIIL::sgLock->unlock();

	mTrans.clear();
}


AosJimoTable::~AosJimoTable()
{
	AosIIL::sgLock->lock();
	AosIIL::sgNumIILCreated--;
	AosIIL::mTotalIILs[eAosIILType_JimoTable]--;
	// OmnScreen << "---jimo table deleted:" << mIILID << "," << sgNumIILCreated << "," << this << endl;
	AosIIL::sgLock->unlock();

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

	if (mSubMemSize)
	{
		OmnDelete [] mSubMemSize;
		mSubMemSize = 0;
	}

	mTrans.clear();
}


u32
AosJimoTable::addRefCountByIILMgr()
{
	mRefcount++;
	AosIIL::sgNumIILRefs++;
	return mRefcount;
}


u32
AosJimoTable::removeRefCountByIILMgr()
{
	aos_assert_r(mRefcount > 0, 0);
	mRefcount--;
	AosIIL::sgNumIILRefs--;
	return mRefcount;
}


u64
AosJimoTable::getIILIDForHash()
{
	return AosSiteid::combineSiteid(mIILID, mSiteid);
}


bool
AosJimoTable::setCmpTag(
		const AosXmlTagPtr &cmp_tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(cmp_tag, false);

	AosBuffArrayVarPtr array = AosBuffArrayVar::create(cmp_tag, true, rdata);
	aos_assert_r(array, false);

	AOSLOCK(mLock);
	mData = array; 
	mCmpTag = cmp_tag;
	AOSUNLOCK(mLock);
	
	return true;
}


bool
AosJimoTable::updateIndexData(
		const i64 &idx,
		const bool changeMax,
		const bool changeMin,
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	AosJimoTablePtr subiil = mSubiils[idx];
	aos_assert_r(subiil, false);

	mIsDirty = true;

	i64 numdoc_new = subiil->getNumDocs();
	mNumDocs = mNumDocs + numdoc_new - mNumEntries[idx];
	mNumEntries[idx] = numdoc_new;

	i64 m = 0;	
	for (i64 i=0; i<mNumSubiils; i++)
	{
		m += mNumEntries[i];
	}

	if (mNumDocs != m)
	{
		OmnAlarm << "mNumDocs not match:" << mNumDocs << "," << m << enderr;
		mNumDocs = m;
	}

	i64 memsize_new = subiil->getMemSize();
	mMemSize = mMemSize + memsize_new - mSubMemSize[idx];
	mSubMemSize[idx] = memsize_new;

	i64 n = 0;	
	for (i64 i=0; i<mNumSubiils; i++)
	{
		n += mSubMemSize[i];
	}

	if (mMemSize != n)
	{
		OmnAlarm << "mMemSize not match:" << mMemSize << "," << n << enderr;
		mMemSize = n;
	}

	bool rslt = true;
	if (changeMin)
	{
		int len = 0;
		const char * data = subiil->getMinValue(len);
		aos_assert_r(data, false);

		mMinData->modifyEntry(idx, data, len, rdata.getPtr());
		mMinData->setSorted(true);
	}
	
	if (changeMax)
	{
		int len = 0;
		const char * data = subiil->getMaxValue(len);
		aos_assert_r(data, false);

		mMaxData->modifyEntry(idx, data, len, rdata.getPtr());	
		mMaxData->setSorted(true);
	}

	if (mParentIIL)
	{
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mParentIIL, false);

		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumSubiils - 1);
		rslt = mParentIIL->updateIndexData(
			mIILIdx, updateMax, updateMin, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosJimoTable::addDocRecPriv(
		const OmnString &value, 
		const bool value_unique, 
		const AosRundataPtr &rdata)
{
	if (isParentIIL())
	{
		AosJimoTablePtr subiil = getSubiilPriv(value, rdata);
		aos_assert_r(subiil, false);
		aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, false);
	
		return subiil->addDocRecSafe(value, value_unique, rdata);
	}
	
	aos_assert_r(isLeafIIL(), false);

	i64 numBefore = mNumDocs;
	i64 insert_idx = 0;
	bool rslt = addDocSinglePriv(value, value_unique, insert_idx, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(numBefore + 1 == mNumDocs, false);

	if (isChildIIL())
	{
		bool updateMin = (insert_idx == 0);
		bool updateMax = (insert_idx >= mNumDocs - 1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mParentIIL, false);
		mParentIIL->updateIndexData(mIILIdx, updateMax, updateMin, rdata);
	}

	if (mNumDocs >= AosIIL::mMaxBigIILSize)
	{
		rslt = splitListPriv(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosJimoTable::addDocSinglePriv(
		const OmnString &value,
		const bool value_unique,
		i64 &insert_idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mData, false);

	bool unique = true;
	insert_idx = mData->findPos(0, value.data(), value.length(), unique);
	aos_assert_r(insert_idx >= 0 && insert_idx <= mNumDocs, false);

	if (!unique && value_unique)
	{
		AosSetErrorU(rdata, "value_already_exist")
			<< value << ":" << insert_idx << enderr;
		return false;
	}

	bool rslt = mData->insertEntry(insert_idx, value.data(), value.length(), rdata.getPtr());
	aos_assert_r(rslt, false);

	mData->setSorted(true);

	mNumDocs++;
	mMemSize = mData->getMemSize();
	mIsDirty = true;
	return true;
}


bool
AosJimoTable::getValue(
		const i64 &idx,
		char * &data,
		int &len)
{
	aos_assert_r(idx >= 0 && idx < mNumDocs, false);

	return mData->getEntry(idx, data, len);
}


bool
AosJimoTable::getMinValue(
		const i64 &idx,
		char * &data,
		int &len)
{
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);

	return mMinData->getEntry(idx, data, len);
}


bool
AosJimoTable::getMaxValue(
		const i64 &idx,
		char * &data,
		int &len)
{
	aos_assert_r(idx >= 0 && idx < mNumSubiils, false);
	
	return mMaxData->getEntry(idx, data, len);
}


const char *
AosJimoTable::getMinValue(int &len) const
{
	len = 0;
	char * data;
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0, 0);
		bool rslt = mData->getEntry(0, data, len);
		aos_assert_r(rslt, false);
	}
	else
	{
		aos_assert_r(mNumSubiils > 0, 0);
		bool rslt = mMinData->getEntry(0, data, len);
		aos_assert_r(rslt, false);
	}
	return data;
}


const char *
AosJimoTable::getMaxValue(int &len) const
{
	len = 0;
	char * data;
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0, 0);
		bool rslt = mData->getEntry(mNumDocs - 1, data, len);
		aos_assert_r(rslt, false);
	}
	else
	{
		aos_assert_r(mNumSubiils > 0, 0);
		bool rslt = mMaxData->getEntry(mNumSubiils - 1, data, len);
		aos_assert_r(rslt, false);
	}
	return data;
}


void 
AosJimoTable::addTrans(const AosTransId &id)
{
	AOSLOCK(mLock);
	mTrans.push_back(id);
	AOSUNLOCK(mLock);
}


void
AosJimoTable::finishTrans()
{
	AOSLOCK(mLock);
	vector<AosTransId> trans = mTrans;
	mTrans.clear();
	AOSUNLOCK(mLock);

	if (trans.size() > 0)
	{
		AosFinishTrans(trans);
	}
}


/*
bool
AosIILBigStr::removeDocPriv(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(value != "", false);

	bool rslt = removeDocRecPriv(value, docid, rdata);
	aos_assert_r(rslt, false);

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
AosIILBigStr::removeDocRecPriv(
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
	AosIILBigStrPtr subiil = getSubiilByCondPriv(value, docid, eAosOpr_eq, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
	
	return subiil->removeDocRecSafe(value, docid, rdata);
}


bool
AosIILBigStr::modifyDocSafe(
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
AosIILBigStr::incrementDocidPriv(
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
	
	AosIILBigStrPtr subiil = getSubiilByCondPriv(key, eAosOpr_eq, false, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, 0);

	return subiil->incrementDocidSafe(key, value, inc_value, init_value, add_flag, rdata);
}


i64
AosIILBigStr::getTotalPriv(
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

	case eAosOpr_prefix:
		 return getTotalPrefixPriv(value, rdata);

	case eAosOpr_like:
		 return getTotalLikePriv(value, rdata);

	default:
		 OmnAlarm << "unrecognized operator: " << opr << enderr;
		 return 0;
	}

	AosIILBigStrPtr first_sub = getSubiilByCondPriv(value, opr, false, rdata);
	if (!first_sub)
	{
		return 0;
	}
	aos_assert_r(first_sub->mLevel >= 0 && first_sub->mLevel == mLevel-1, 0);
	
	AosIILBigStrPtr last_sub = getSubiilByCondPriv(value, opr, true, rdata);
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
}


bool
AosIILBigStr::setValueDocUniquePriv(
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

	AosIILBigStrPtr subiil = getSubiilPriv(key, docid, true, false, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, false);

	return subiil->setValueDocUniqueSafe(key, docid, must_same, rdata);
}


u64
AosIILBigStr::getMinDocid() const
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
AosIILBigStr::getMaxDocid() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mDocids[mNumDocs-1];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMaxDocids[mNumSubiils-1];
}


const char *
AosIILBigStr::getMinValue() const
{
	if (isLeafIIL())
	{
		aos_assert_r(mNumDocs > 0,0);
		return mValues[0];
	}

	aos_assert_r(mNumSubiils > 0,0);
	return mMinVals[0];
}


const char *
AosIILBigStr::getMaxValue() const
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
AosIILBigStr::checkDocidUnique(
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
AosIILBigStr::getTotalNEPriv(const OmnString &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


i64
AosIILBigStr::getTotalLikePriv(const OmnString &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


i64
AosIILBigStr::getTotalPrefixPriv(const OmnString &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


// Chen Ding, 04/22/2012
bool
AosIILBigStr::hasMoreMembers(const AosIILIdx &idx) const
{
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);
	i64 iilidx = idx.getIdx(mLevel);
	return (iilidx >= 0 && iilidx < mNumSubiils);
}


// Chen Ding, 04/22/2012
// New Functions
bool
AosIILBigStr::containValuePriv(
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
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(mNumSubiils > 0, false);

	bool rslt;
	int cmp_rslt;
	switch (opr)
	{
	case eAosOpr_gt:
	case eAosOpr_ge:
		 // This means that it wants to find the values that are greater
		 // than 'value'. For instance, for Age IIL, we want to find all 
		 // the docs whoseage is greater than 'value'. It returns true
		 // if its max value is greater than 'value'.
		 rslt = AosIILUtil::valueMatch(max_value, opr, value.data(), "", mIsNumAlpha);
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
		 rslt = AosIILUtil::valueMatch(min_value, opr, value.data(), "", mIsNumAlpha);
		 if (rslt) return true;
		 
		 // If it is normal search, there is no need to continue searching.
		 // Otherwise, it should.
		 keep_search = reverse;
		 return false;

	case eAosOpr_eq:
		 if (AosIILUtil::valueMatch(min_value, eAosOpr_le, value.data(), "", mIsNumAlpha))
		 {
			 if (AosIILUtil::valueMatch(value.data(), eAosOpr_le, max_value, "", mIsNumAlpha))
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
		 return AosIILUtil::valueMatch(min_value, eAosOpr_ne, value.data(), "", mIsNumAlpha) ||
		 		AosIILUtil::valueMatch(max_value, eAosOpr_ne, value.data(), "", mIsNumAlpha);

	case eAosOpr_an:
		 return true;

	case eAosOpr_Objid:
	case eAosOpr_date:
	case eAosOpr_epoch:
		 OmnShouldNeverComeHere;
		 return false;

	// Chen Ding, CHENDING20130920
	// case eAosOpr_range:
	
	case eAosOpr_prefix:
		 // Assume 'aaa' is the prefix, if the min_value is bigger than 'aaa'
		 // or the max_value is less than 'aaa', it returns false. 
		 // Otherwise, it returns true. Note that returning true does not
		 // mean the subiil [min_value, max_value] definitely contains 
		 // the value, but only possible.
		 cmp_rslt = strncmp(min_value, value.data(), value.length());
		 if (cmp_rslt > 0) 
		 {
			 // min_value > value, not possible.
			 // if it is normal order, no need to continue searching.
			 keep_search = reverse;
			 return false;
		 }

		 // This means min_value <= value, it is possible, but need to 
		 // check the max_value.
		 cmp_rslt = strncmp(max_value, value.data(), value.length());
		 if (cmp_rslt < 0) 
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
AosIILBigStr::resetMaxMin(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r((idx >= 0) && (idx < mNumSubiils), false);

	i64 numdoc_new;
	i64 move_num;
	AosIILBigStrPtr subiil;
	for(i64 i=mNumSubiils-1; i>=idx; i--)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil, false);
		
		numdoc_new = subiil->getNumDocs();
		mNumDocs = mNumDocs + numdoc_new - mNumEntries[i];
		if (numdoc_new > 0)
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
			
			subiil->deleteIILSafe(true, rdata);
			//subiil->deleteIILRecSafe(rdata);
			AosIILMgr::getSelf()->returnIILPublic(subiil, rdata);
			
			mSubiils[i] = 0;
			mIILIds[i] = 0;
			mNumEntries[i] = 0;

			move_num = mNumSubiils - 1 - i;
			if (move_num != 0)
			{
				memmove(&mMinVals[i], &mMinVals[i+1], sizeof(char *) * move_num);
				memmove(&mMaxDocids[i], &mMaxDocids[i+1], sizeof(u64) * move_num);
				memmove(&mMaxVals[i], &mMaxVals[i+1], sizeof(char *) * move_num);
				memmove(&mMinDocids[i], &mMinDocids[i+1], sizeof(u64) * move_num);
				memmove(&mSubiils[i], &mSubiils[i+1], sizeof(AosIILBigStrPtr) * move_num);
				memmove(&mIILIds[i], &mIILIds[i+1], sizeof(u64) * move_num);
				memmove(&mNumEntries[i], &mNumEntries[i+1], sizeof(i64) * move_num);
			}
			mNumSubiils--;
		}
	}
	return true;
}

*/

