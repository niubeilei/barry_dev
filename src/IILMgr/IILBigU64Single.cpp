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


bool 	
AosIILBigU64::insertBefore(
		const i64 &idx, 
		const u64 &docid, 
		const u64 &value)
{
	// It inserts an entry in front of 'idx'. 
	if (mNumDocs >= mMemCap) 
	{
		aos_assert_r(expandMemoryPriv(), false);
	}
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(idx >= 0, false);

	// Need to move pointers starting at 'nn'
	memmove(&mDocids[idx + 1], &mDocids[idx], sizeof(u64) * (mNumDocs - idx));
	memmove(&mValues[idx + 1], &mValues[idx], sizeof(u64) * (mNumDocs - idx));

	// Change mValues
	mValues[idx] = value;
	mDocids[idx] = docid;

	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


bool 	
AosIILBigU64::insertAfter(
		const i64 &idx, 
		const u64 &docid, 
		const u64 &value)
{
	// It inserts an entry after 'idx'. 
	if (mNumDocs >= mMemCap)
	{
		aos_assert_r(expandMemoryPriv(), false);
	}
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < mNumDocs, false);

	if (idx < mNumDocs - 1)
	{
		// Change mValues
		memmove(&mDocids[idx + 2], &mDocids[idx + 1], sizeof(u64) * (mNumDocs - idx - 1));
		memmove(&mValues[idx + 2], &mValues[idx + 1], sizeof(u64) * (mNumDocs - idx - 1));
	}
	
	
	mDocids[idx+1] = docid;
	mValues[idx+1] = value;
	
	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


//change name form addDocDirectPriv
bool
AosIILBigU64::addDocSinglePriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata) 
{
	// This function adds a new entry [value, docid] into the list. 
	// It is responsible for all adding.
	
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// Check whether we need to split

	bool rslt = false;
	i64 numBefore = mNumDocs;
	i64 idx = 0;
	if (mNumDocs == 0)
	{
		// Change mValues
		if (mNumDocs >= mMemCap) 
		{
			aos_assert_r(expandMemoryPriv(), false);
		}
		mDocids[0] = docid;
		mValues[0] = value;
		
		mNumDocs = 1;
		idx = 0;
	}
	else
	{
		aos_assert_r(mNumDocs <= mMemCap, false);
		
		int rsltInt = 0;
		int rsltInt2 = 0;
		i64 left = 0; 
		i64 right = mNumDocs - 1;
		while (left <= right)
		{
			idx = left + ((right - left) >> 1);
		
			rsltInt = AosIILUtil::valueMatch(mValues[idx], value);
			if (rsltInt == 0)
			{
				// A match is found. Check whether value is unique
				if (value_unique)
				{					
					// Chen Ding, 2013/06/02
					// The value must be unique. it returns an error. 
					// if (rdata->needReportError())
					// {
					//	rdata->setError()<< "Value already exist: " << value << ":" << docid
					//		<< ":" << mDocids[idx];
					//	OmnAlarm << rdata->getErrmsg()<< enderr;
					AosSetErrorU(rdata, "value_already_exist") << value
						<< ":" << docid << ":" << mDocids[idx] << ":" << idx << enderr;
					// }
					return false;
				}

				// Need to insert the doc based on docid
				rslt = insertDocSinglePriv(idx, value, docid, 
						value_unique, docid_unique, rdata);
				aos_assert_r(rslt, false);
				break;
			}
			else if (rsltInt < 0) 
			{
				if (idx == mNumDocs - 1)
				{
					// Append the docid
					if (mNumDocs >= mMemCap)
					{
						aos_assert_r(expandMemoryPriv(), false);
					}
					mDocids[mNumDocs] = docid;
					mValues[mNumDocs] = value;
					aos_assert_r(rslt, false);
					
					incNumDocsNotSafe();
					idx = mNumDocs - 1;
					break;
				}

				// Check whether mValues[idx+1] >= value
				rsltInt2 = AosIILUtil::valueMatch(mValues[idx + 1], value);
				if (rsltInt2 > 0)
				{
					// Found the spot. Need to move the elements after
					// 'idx' to make room for the new docid
					insertAfter(idx, docid, value);
					idx++;
					break;
				}
				else if (rsltInt2 == 0)
				{
					if (value_unique)
					{
						// Chen Ding, 2013/06/02
						// if (rdata->needReportError())
						// {
						//	rdata->setError() << "Value already exist: " << value << ":" << docid
						//		<< ":" << mDocids[idx];
						//	OmnAlarm << rdata->getErrmsg() << enderr;
						AosSetErrorU(rdata, "value_already_exist") << value
							<< ":" << docid << ":" << mDocids[idx] << ":" << idx << enderr;
						// }
						return false;
					}
					idx++;
					rslt = insertDocSinglePriv(idx, value, docid, value_unique, docid_unique, rdata);
					aos_assert_r(rslt, false);
					break;
				}
				left = idx + 1;
			}
			else
			{
				// value < mValues[idx]
				if (idx == 0)
				{
					// Insert into the front
					insertBefore(idx, docid, value);
					break;
				}

				// Chen Ding, 08/11/2010
				rsltInt2 = AosIILUtil::valueMatch(mValues[idx - 1], value);
				if (rsltInt2 < 0)
				{
					// Found the spot. Need to move the elements after
					// 'idx' to make room for the new docid
					idx--;
					insertAfter(idx, docid, value);
					idx++;
					break;
				}
				else if (rsltInt2 == 0)
				{
					if (value_unique)
					{
						// Chen Ding, 2013/06/02
						// if (rdata->needReportError())
						// {
						//	rdata->setError()<< "Value already exist: " << value << ":" << docid
						//		<< ":" << mDocids[idx];
						//	OmnAlarm << rdata->getErrmsg() << enderr;
						AosSetErrorU(rdata, "value_already_exist") << value
							<< ":" << docid << ":" << mDocids[idx] << ":" << idx << enderr;
						// }
						return false;
					}
					idx--;
					rslt = insertDocSinglePriv(idx, value, docid, value_unique, docid_unique, rdata);
					aos_assert_r(rslt, false);
					break;
				}
				right = idx - 1;
			}
		}
	}

	// if we have subiil, set root iil max/min value and mNumEntries. 
	// Note that we may need change them both
	aos_assert_r(numBefore + 1 == mNumDocs, false);
	mIsDirty = true;
	if (isChildIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs - 1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mParentIIL, false);
		mParentIIL->updateIndexData(mIILIdx, updateMax, updateMin, rdata);
	}

	if (mNumDocs >= mMaxBigIILSize)
	{
		rslt = splitListPriv(0, rdata);
		aos_assert_r(rslt, false);
	}
	sanityTestForSubiils();
	return true;
}


//change name form insertDocPriv
bool
AosIILBigU64::insertDocSinglePriv(
		i64 &idx, 
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata) 
{
	// It inserts the doc at position 'idx'. If:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// the doc may need to be inserted in the next subiil.
	aos_assert_r(mNumDocs > 0, false);
	
	// Find the position for 'value'.
	// NEWIIL_TASK
	// Chen Ding, 04/18/2012, 'firstEQ(...)' needs to be replaced with 
	// a new function that finds the position with both 'value' and 'docid'.
	i64 left = firstEQ(0, value, rdata);
	aos_assert_r(left >= 0 && left < mNumDocs, false);
	
	bool rslt;
	if (docid == mDocids[left])
	{
		// NEWIIL_TASK
		// Chen Ding, 04/18/2012
		// This checking: 'docid == mDocids[left]' is incorrect. It is possible
		// that docid may be included in the IIL but the above condition is not true,
		// which may cause incorrect insertion. Need to fix!!!!!!!!!!!!!!!!!
		if (docid_unique)
		{
			OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			return false;
		}
		rslt = insertBefore(left, docid, value);
		aos_assert_r(rslt, false);
		
		idx = left;
		return true;
	}

	if (docid < mDocids[left])
	{
		rslt = insertBefore(left, docid, value);
		aos_assert_r(rslt, false);
		
		idx = left;
		return true;
	}

	i64 right = firstEQRev(mNumDocs - 1, value, rdata);
	aos_assert_r(right >= left && right < mNumDocs, false);

	if (docid == mDocids[right])
	{
		if (docid_unique)
		{
			OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			return false;
		}
		rslt = insertAfter(right, docid, value);
		aos_assert_r(rslt, false);
		
		idx = right + 1;
		return true;
	}

	if (docid > mDocids[right])
	{
		rslt = insertAfter(right, docid, value);
		aos_assert_r(rslt, false);
		
		idx = right + 1;
		return true;
	}

	// mDocids[left] < value < mDocids[right], this means that 
	// the new doc must be inserted in between (left, right)
	while (left < right)
	{
		if (left + 1 == right)
		{
			rslt = insertAfter(left, docid, value);
			aos_assert_r(rslt, false);
			
			idx = left + 1;
			return true;
		}

		i64 nn = (left + right) >> 1;
		if (mDocids[nn] == docid)
		{
			if (docid_unique)
			{
				OmnAlarm << "Not unique: " << docid 
					<< ":" << value << ":" << mIILID << enderr;
				return false;
			}
			
			rslt = insertBefore(nn, docid, value);
			aos_assert_r(rslt, false);
			
			idx = nn;
			return true;
		}
		else if (mDocids[nn] < docid)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


//change name form removeDocDirectPriv
bool 		
AosIILBigU64::removeDocSinglePriv(
		const u64 &value, 
		const u64 &docid,
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
	
	if (mNumDocs <= 0) 
	{
		return false;
	}

	i64 left = 0;
	i64 right = mNumDocs - 1;
	i64 idx = -1;
	int rsltInt;
	bool found = false;
	while(left <= right)
	{
		if (left == right)
		{
			rsltInt = AosIILUtil::valueMatch(mValues[left], value);
			if (rsltInt == 0 && docid == mDocids[left])
			{
				found = true;
				idx = left;
			}
			break;
		}
		
		idx = (left + right) >> 1;
		rsltInt = AosIILUtil::valueMatch(mValues[idx], value);
		if (rsltInt == 0)
		{
			if (mDocids[idx] == docid)
			{
				found = true;
				break;
			}
			else if (mDocids[idx] < docid)
			{
				left = idx + 1;
			}
			else
			{
				right = idx - 1;
			}
		}
		else if (rsltInt < 0)
		{
			left = idx + 1;
		}
		else
		{
			right = idx - 1;
		}
	}

	if (!found || idx < 0 || idx >= mNumDocs)
	{
		return false;
	}
	aos_assert_r(AosIILUtil::valueMatch(mValues[idx], value) == 0 && mDocids[idx] == docid, false);
	
	// Change mValues
	if (idx < mNumDocs - 1)
	{
		i64 ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx + 1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx + 1], ss);

		ss = sizeof(u64) * (mNumDocs - idx - 1);
		memmove(&mValues[idx], &mValues[idx + 1], ss);
	}
	decNumDocsNotSafe();

	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	mIsDirty = true;
	
	if (isChildIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs - 1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mParentIIL, false);
		mParentIIL->updateIndexData(mIILIdx, updateMax, updateMin, rdata);
//shawnquleft
		if (mNumDocs < mMinIILSize)
		{
//shawnquleft
			mParentIIL->mergeSubiilPriv(mIILIdx, rdata);
		}
	}
	
	return true;
}


bool
AosIILBigU64::nextDocidAN(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid)
{
	// It matches any entry. It simply retrieves the current one, 
	// if any, and advances 'idx'. 
	if (idx >= 0 && idx < mNumDocs)
	{
		docid = mDocids[idx];
		return true;
	}

	docid = AOS_INVDID;
	idx = -5;
	return true;
}


bool
AosIILBigU64::nextDocidGT(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) > 0)

	// Chen Ding, 2014/02/26
	// if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value))
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value, 0))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstGTRev(idx, value, rdata) : firstGT(idx, value,	rdata);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}


bool
AosIILBigU64::nextDocidGE(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	// Chen Ding, 2014/02/26
	// if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value))
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value, 0))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstGERev(idx, value, rdata) : firstGE(idx, value, rdata);
	if (idx == -1)
	{	
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}


bool
AosIILBigU64::nextDocidLT(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] < value
	// if (mValues[idx].compare1(value) < 0)
	// Chen Ding, 2014/02/26
	// if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value))
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value, 0))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstLTRev(idx, value, rdata) : firstLT(idx, value, rdata);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}


bool
AosIILBigU64::nextDocidLE(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) <= 0)
	// Chen Ding, 2014/02/26
	// if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value))
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value, 0))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstLERev(idx, value, rdata) : firstLE(idx, value, rdata);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}


bool
AosIILBigU64::nextDocidEQ(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	//
	// If success, 'docid' holds the docid found and 'idx' points to
	// the next element (IMPORTANT: It does not point to the current one).
	// if (mValues[idx] == value)

	// Chen Ding, 2014/02/26
	// if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value))
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value, 0))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstEQRev(idx, value, rdata) : firstEQ(idx, value, rdata);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}
	
	docid = mDocids[idx];
	return true;
}


bool
AosIILBigU64::nextDocidNE(
		i64 &idx, 
		const bool reverse, 
		const u64 &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the:
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	int rslt = AosIILUtil::valueMatch(mValues[idx], value);
	if (rslt != 0)
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstNERev(idx, value, rdata) : firstNE(idx, value, rdata);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}


i64
AosIILBigU64::firstEQ(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) > 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return -1;

	int rslt = AosIILUtil::valueMatch(mValues[idx], value);

	// If mValues[idx] == value, return idx;
	if (rslt == 0) return idx;

	// If mValues[idx] > value, return -1
	if (rslt > 0) return -1;

	// If mValues[mNumDocs-1] < value, return -1
	// Chen Ding, 2014/02/26
	// if (AosIILUtil::valueMatch(mValues[mNumDocs-1], eAosOpr_lt, value))
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], eAosOpr_lt, value, 0))
	{
		return -1;
	}

	// Now, we are sure mValues[idx] <= value <= mValues[mNumDocs-1].  
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right || left + 1 == right) 
	{

		// Chen Ding, 2014/02/26
		// if (AosIILUtil::valueMatch(mValues[right], eAosOpr_eq, value))
		if (AosIILUtil::valueMatch(mValues[right], eAosOpr_eq, value, 0))
		{
			return right;
		}
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		int rslt = AosIILUtil::valueMatch(mValues[nn], value);
		if (rslt < 0)
		{
			// mValues[nn] < value
			left = nn+1;
		}
		else if (rslt > 0)
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
			if (left == right)
			{
				return right;
			}
			if (left + 1 == right) 
			{
				// Chen Ding, 2014/02/26
				// if (AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value))
				if (AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value, 0))
				{
					return left;
				}
				return right;
			}
		}
	}


	// Chen Ding, 2014/02/26
	// if (left == right && AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value))
	if (left == right && AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value, 0))
	{
		return left;
	}
	return -1;
}


i64
AosIILBigU64::firstEQRev(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!!!
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) < 0) return -1;
	// if (mValues[0].compare1(value) > 0) return -1;

	// If mValues[idx] == value, return idx
	int rslt = AosIILUtil::valueMatch(mValues[idx], value);
	if (rslt == 0) return idx;

	// If mValues[idx] is less than 'value', it is not possible
	if (rslt < 0) return -1;

	// If mValues[0] is greater than value, it is not possible
	if (AosIILUtil::valueMatch(mValues[0], value) > 0) return -1;
		
	// Now, we are sure mValues[0] <= value <= mValues[idx].  
	i64 left = 0; 
	i64 right = idx;
	if (left == right) return -1;
	if (left + 1 == right) 
	{
		if (AosIILUtil::valueMatch(mValues[right], value) == 0)
		{
			return right;
		}
		if (AosIILUtil::valueMatch(mValues[left], value) == 0)
		{
			return left;
		}
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		int rslt = AosIILUtil::valueMatch(mValues[nn], value);
		if (rslt < 0)
		{
			// mValues[nn] < value
			left = nn + 1;
		}
		else if (rslt > 0)
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
				if (AosIILUtil::valueMatch(mValues[right], value) == 0)
				{
					return right;
				}
				return left;
			}
		}
	}

	if (left == right && AosIILUtil::valueMatch(mValues[left], value) == 0)
	{
		return left;
	}
	return -1;
}

	
i64
AosIILBigU64::firstNE(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry whose
	// value is not equal to 'value'. If not found, idx = -1.
	// if (mValues[mNumDocs-1] == value) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) == 0)
	{
		return -1;
	}
		
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) != 0)
		{
			right = nn;
		}
		else
		{
			left = nn;
		}
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstNERev(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry 
	// where mValues[idx] != value.
	// If not found, idx = -1.
	// if (mValues[0].compare1(value) == 0) return -1;
	if (AosIILUtil::valueMatch(mValues[0], value) == 0)
	{
		return -1;
	}
		
	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) != 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right - 1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::lastEQ(const i64 &idx, const u64 &value, const AosRundataPtr &rdata) 
{
	// IMPORTANT!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from idx, it searches for the last one that:
	// 		value == mValues[idx]
	// If not found, it returns -1;

	// if (mValues[mNumDocs-1] == value) return mNumDocs-1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) == 0)
	{
		return mNumDocs-1;
	}

	// Now, mValues[idx] <= value < mValues[mNumDocs-1]
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) == 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right - 1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::lastLT(const u64 &value, const AosRundataPtr &rdata) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] < value
	// If not found, it returns -1;
	// if (mValues[0].compare1(value) >= 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return mNumDocs-1;
	if (AosIILUtil::valueMatch(mValues[0], value) >= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) < 0)
	{
		return mNumDocs-1;
	}

	// Now, mValues[0] < value <= mValues[mNumDocs-1]
	i64 left = 0; 
	i64 right = mNumDocs-1;
	// Now, 'left' points to a good one
	// 'right' points to a bad one. 
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) < 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::lastLE(const u64 &value, const AosRundataPtr &rdata) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] <= value
	// If not found, it returns -1;
	if (AosIILUtil::valueMatch(mValues[0], value) > 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) <= 0)
	{
		return mNumDocs-1;
	}

	// Now, mValues[0] <= value < mValues[mNumDocs-1]
	i64 left = 0; 
	i64 right = mNumDocs-1;
	// Now, 'left' points to a good one
	// 'right' points to a bad one. 
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) <= 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLE(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[idx], value) > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLERev(const i64 &idx, const u64 &value, const AosRundataPtr &rdata) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] <= value
	if (AosIILUtil::valueMatch(mValues[0], value) > 0)
	{
		return -1;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right - 1)
	{
		return left;
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) <= 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLT(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[idx], value) >= 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLTRev(const i64 &idx, const u64 &value, const AosRundataPtr &rdata) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[0], value) >= 0)
	{
		return -1;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) < 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGT(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] <= value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] > value
	
	// 1. Check whether the last one is < 'value'
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) <= 0)
	{
		return -1;
	}

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1)
	{
		return right;
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) <= 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return right;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGTRev(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) <= 0)
	{
		return -1;
	}
	else
	{
		return (mNumDocs -1);
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGE(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) < 0)
	{
		return -1;
	}

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1)
	{
		return right;
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) < 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return right;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGERev(const i64 &idx, const u64 &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) < 0)
	{
		return -1;
	}
	else
	{
		return (mNumDocs -1);
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool		
AosIILBigU64::nextDocidSinglePriv(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr,
		const u64 &value,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata) 
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is false) or the opposite
	// (reverse is true). The value must meet:
	// 		<value> <opr> 'value'
	//
	// It first advances 'idx' to the next element based on 'reverse'. 
	// If 'idx' is -10, it means starting from beginning. 
	//
	// If the next element matches the query, it returns the 
	// one. 
	//
	// If a doc is not found and this subiil is sure the searched doc
	// is not in the entire IIL, 'nomore' is set to true.
	//
	// Performance Analysis: 
	// Since the value list is sorted, it uses the binary search algorithm
	// to locate the first matching element in the array. The complexity
	// is log(n), which is fast enough. Since this is a string IIL, 
	// each compair is a string compair, which can be a little bit 
	// slow. 
	//
	// Possible Improvements:
	// Need to check whether memcmp() is faster than strcmp(). If yes,
	// we should probably use memcmp().
	
	docid = AOS_INVDID;
	if (mNumDocs == 0)
	{
	 	// No entries in the list.
	 	idx = -5;
	 	docid = AOS_INVDID;
		isunique = true;
	 	return true;
	}

	if (!mValues)
	{
	 	OmnAlarm << "mValues null" << enderr;
	 	idx = -5;
	 	docid = AOS_INVDID;
		isunique = true;
	 	return false;
	}

	if ((idx < 0 && idx != -10) || (idx > 0 && idx >= mNumDocs))
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		idx = -5;
		docid = AOS_INVDID;
		isunique = true;
		return false;
	}

	bool startnew = false;
	if (reverse)
	{
		if (idx == 0)
		{
			idx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return true;
		}
		else if (idx == -10)
		{
			idx = mNumDocs -1;
			startnew = true;
		}
		else
		{	
			idx--;
		}
	}
	else // forward
	{
		if (idx >= mNumDocs -1)
		{
			// is the last one, we can't find the next
			idx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return true;
		}
		else if (idx == -10)
		{
			idx = 0;
			startnew = true;
		}
		else
		{
			// not the last one
			idx++;
		}
	}

	// Chen Ding, 12/11/2010
	// When it comes to this point, 'idx' must be in [0, mNumDocs-1]
	if (idx < 0 || idx >= mNumDocs)
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		idx = -5;
		docid = AOS_INVDID;
		isunique = true;
		return false;
	}

	bool rslt;
	if (startnew)
	{
		switch (opr)
		{
		case eAosOpr_gt:
		 	 rslt = nextDocidGT(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_ge:
		 	 rslt = nextDocidGE(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_eq:
		 	 rslt = nextDocidEQ(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_lt:
		 	 rslt = nextDocidLT(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_le:
		 	 rslt = nextDocidLE(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_ne:
		 	 rslt = nextDocidNE(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;

		case eAosOpr_an:
		 	 rslt = nextDocidAN(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;

		default:
		 	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 	 idx = -5;
		 	 docid = AOS_INVDID;
		 	 isunique = true;
		 	 return false;
		}
	}
	else
	{
		// It is not a new. Just check the current entry.
		switch (opr)
		{
		case eAosOpr_gt:
			 // Chen Ding, 2014/02/26
			 // if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value))
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value, 0))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_ge:
			 // Chen Ding, 2014/02/26
			 // if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value))
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value, 0))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_eq:
			 // Chen Ding, 2014/02/26
			 // if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value))
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value, 0))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;
	
		case eAosOpr_lt:
			 // Chen Ding, 2014/02/26
			 // if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value))
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value, 0))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_le:
			 // Chen Ding, 2014/02/26
			 // if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value))
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value, 0))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_ne:
			 rslt = nextDocidNE(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_an:
			 docid = mDocids[idx];
			 checkDocidUnique(true, idx, docid, isunique, rdata);
			 return true;

		default:
		  	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		  	 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
		  	 return false;
		}
	}

	OmnShouldNeverComeHere;
	idx = -5;
	docid = AOS_INVDID;
	return false;
}


//change name from getTotalPriv
i64 
AosIILBigU64::getTotalSinglePriv(
		const u64 &value, 
		const AosOpr opr, 
		const AosRundataPtr &rdata)
{
	if (mNumDocs == 0)
	{
		return 0;
	}

	i64 startidx = 0;
	i64 endidx = 0;
	switch (opr)
	{
	case eAosOpr_le:
		 // mValues[i] <= value
		 endidx = lastLE(value, rdata);
		 if (endidx == -1)
		 {
			return 0;
		 }
		 return endidx + 1;

	case eAosOpr_lt:
		 // mValues[i] < value
		 endidx = lastLT(value, rdata);
		 if (endidx == -1)
		 {
			return 0;
		 }
		 return endidx + 1;

	case eAosOpr_gt:
		 // mValues[i] > value
		 // startidx = (mValues[0].compare1(value) > 0) ? 0 : firstGT(0, value);

		 // Chen Ding, 2014/02/26
		 // startidx = (AosIILUtil::valueMatch(mValues[0], 
		 // 			 eAosOpr_gt, value)) ? 0 : firstGT(0, value);
		 startidx = (AosIILUtil::valueMatch(mValues[0], 
					 eAosOpr_gt, value, 0)) ? 0 : firstGT(0, value, rdata);
		 if (startidx == -1) 
		 {
			return 0;
		 }
		 return mNumDocs - startidx;

	case eAosOpr_ge:
		 // mValues[i] >= value
		 // startidx = (mValues[0].compare1(value) >= 0) ? 0 : firstGE(0, value);

		 // Chen Ding, 2014/02/26
		 // startidx = (AosIILUtil::valueMatch(mValues[0], 
		// 			 eAosOpr_ge, value)) ? 0 : firstGE(0, value);
		 startidx = (AosIILUtil::valueMatch(mValues[0], 
					 eAosOpr_ge, value, 0)) ? 0 : firstGE(0, value, rdata);
		 endidx = mNumDocs-1;
		 if (startidx == -1)
		 {
			return 0;
		 }
		 return mNumDocs - startidx;

	case eAosOpr_eq:
		 // mValues[i] == value
		 startidx = firstEQ(0, value, rdata);
		 if (startidx < 0) 
		 {
			 return 0;
		 }
		 endidx = lastEQ(startidx, value, rdata);
		 aos_assert_r(endidx >= startidx && endidx < mNumDocs, 0);
		 return endidx - startidx + 1;

	case eAosOpr_ne:
		 // mValues[i] != value
		 startidx = firstEQ(0, value, rdata);
		 if (startidx < 0) 
		 {
			 return mNumDocs;
		 }
		 endidx = lastEQ(startidx, value, rdata);
		 aos_assert_r(endidx >= startidx && endidx < mNumDocs, 0);
		 return mNumDocs - (endidx - startidx + 1);

	case eAosOpr_an:
		 return mNumDocs;

	default:
		 OmnAlarm << "Incorrect operator: " << opr << enderr;
		 return -1;
	}

	OmnShouldNeverComeHere;
	return -1; 
}


bool
AosIILBigU64::setValueDocUniqueSinglePriv(
		const u64 &key, 
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	// If the list contains [key, docid], override 'docid'. Otherwise, 
	// it adds an entry [key, docid].
	aos_assert_r(isLeafIIL(), false);

	i64 idx = -10;
	bool isunique = true;
	u64 did = 0;
	u64 vv = key;
	bool rslt = nextDocidSinglePriv(idx, false, eAosOpr_eq, vv, did, isunique, rdata);
	if (!rslt || idx < 0)
	{
		// Did not find it. Create it.
		rslt = addDocPriv(key, docid, true, false, rdata);
		if (!rslt)
		{
			AosSetError(rdata, "failed_adding") << ": " << key << ":" << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
		return rslt; 
	}

	if (!isunique)
	{
		AosSetError(rdata, "value_not_unique") << ": " << key << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (docid == did)
	{
		return true;
	}

	if (must_same)
	{
		AosSetError(rdata, "value_not_same") << ": " << key << ":" << docid << ":" << did;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// The docid is different. Need to modify it.
	if (!(mValues && idx < mNumDocs &&
		  mDocids && AosIILUtil::valueMatch(mValues[idx], vv) == 0))
	{
		AosSetError(rdata, "internal_error") << ": " << idx << ":" << mNumDocs
			<< ":" << key << ":" << mValues[idx];
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mDocids[idx] = docid;
	mIsDirty = true;

	// Ketty 2013/01/15
	// comp not support yet.
	// modify in comp iil
	//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//rslt = compiil->removeDocSafe(key, did, rdata);
	//if (!rslt)
	//{
	//	returnCompIIL(compiil, rdata);
	//	aos_assert_r(rslt, false);
	//}
	//rslt = compiil->addDocSafe(key, docid, false, false, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_r(rslt, false);
	return true; 
}


bool
AosIILBigU64::incrementDocidSinglePriv(
		const u64 &key,
		u64 &value,
		const u64 &incvalue,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(isLeafIIL(), false);

	// This function retrieves the value identified by 'key'. 
	// If the entry does not exist, it checks 'add_flag'. If it
	// is true, it will add it. 
	// It increments the docid by 'incvalue' and returns
	// the old value (i.e., the value before incrementing) of the docid.
	
	u64 docid;
	bool isunique = true;
	i64 idx = -10;
	bool rslt = nextDocidSinglePriv(idx, false, eAosOpr_eq, key, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	
	if (idx == -5)
	{
		// Does not exist. 
		if (add_flag)
		{
			// Need to add it.
			value = init_value + incvalue;
			rslt = addDocSinglePriv(key, value, true, false, rdata);
			aos_assert_r(rslt, false);
			
			// Ketty 2013/01/15
			// comp not support yet.
			//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
			//aos_assert_r(compiil, false);
			//rslt = compiil->addDocSafe(key, value, false, true, rdata);
			//returnCompIIL(compiil, rdata);
			//aos_assert_r(rslt, false)
			
			value = init_value;
		}
		else
		{
		    AosSetError(rdata, AosErrmsgId::eValueNotFound);
		    OmnAlarm << rdata->getErrmsg() << ". Key: " << key 
			     << ". IILID: " << mIILID << enderr;
		    return false;
		}
	}
	else
	{
		rslt = removeDocSinglePriv(key, docid, rdata);
		aos_assert_r(rslt, false);
		
		value = docid + incvalue;
		rslt = addDocSinglePriv(key, value, true, false, rdata);
		aos_assert_r(rslt, false);

		// Ketty 2013/01/15
		// comp not support yet.
		//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);
		//bool rslt = compiil->removeDocSafe(key, docid, rdata);
		//if (!rslt)
		//{
		//	returnCompIIL(compiil, rdata);
		//	aos_assert_r(rslt, false);
		//}
		//rslt = compiil->addDocSafe(key, value, false, true, rdata);
		//returnCompIIL(compiil, rdata);
		//aos_assert_r(rslt, false);

		value = docid;
	}

	return true;
}


bool
AosIILBigU64::nextUniqueValueSinglePriv(
		AosIILIdx &theidx,
		const bool reverse,
		const AosOpr opr,
		const u64 &value,
		u64 &unique_value,
		bool &found, 
		const AosRundataPtr &rdata)
{
	// This is to find the next unique value. If it is to start from the 
	// beginning, 'unique_value' is set to the first value found. If
	// it is not start from the beginning, 'unique_value' holds the 
	// previous value. It should search until it finds a value that is
	// different from 'unique_value'. 'unique_value' is then set to 
	// the new value.
	found = false;
	if (mNumDocs == 0)
	{
		theidx.setInvalid(mLevel);
	 	return true;
	}

	if (!mValues)
	{
		OmnAlarm << "mValues null" << enderr;
		theidx.setInvalid(mLevel);
	 	return false;
	}

	i64 idx = theidx.getIdx(mLevel);
	if ((idx < 0 && idx != -10) || (idx >= mNumDocs))
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		theidx.setInvalid(mLevel);
		return false;
	}
	
	bool startnew = false;
	if (idx == -10)
	{
		idx = reverse ? mNumDocs - 1 : 0;
		startnew = true;
	}
	else
	{
		idx = reverse ? idx - 1 : idx + 1;
	}
	if (idx < 0 || idx >= mNumDocs)
	{
		theidx.setInvalid(mLevel);
		return true;
	}

	bool rslt;
	if (startnew)
	{
		u64 docid;
		switch (opr)
		{
		case eAosOpr_gt:
			 rslt = nextDocidGT(idx, reverse, value, docid, rdata);
			 break;

		case eAosOpr_ge:
			 rslt = nextDocidGE(idx, reverse, value, docid, rdata);
			 break;

		case eAosOpr_eq:
			 rslt = nextDocidEQ(idx, reverse, value, docid, rdata);
			 break;

		case eAosOpr_lt:
			 rslt = nextDocidLT(idx, reverse, value, docid, rdata);
			 break;

		case eAosOpr_le:
			 rslt = nextDocidLE(idx, reverse, value, docid, rdata);
			 break;

		case eAosOpr_ne:
			 rslt = nextDocidNE(idx, reverse, value, docid, rdata);
			 break;

		case eAosOpr_an:
			 rslt = nextDocidAN(idx, reverse, value, docid);
			 break;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 theidx.setInvalid(mLevel);
		  	 return false;
		}

		if (!rslt || idx < 0 || idx >= mNumDocs)
		{
			theidx.setInvalid(mLevel);
			return rslt;	
		}

		// Found a matched value
		theidx.setIdx(mLevel, idx);
		if (unique_value != 0)
		{
			// It is to find the next unique value. Since this is to start from
			// new, 'unique_value' should (normally) be empty. 
			if (unique_value != mValues[idx])
			{
				// Found a unique value (i.e., mValues[idx] is different from 'unique_value'.
				unique_value = mValues[idx];
				found = true;
				return true;
			}
			idx += reverse ? -1 : 1;
		}
		else
		{
			// 'unique_value' is empty.
			unique_value = mValues[idx];
			found = true;
			return true;
		}
	}

	// It is not starting from new. 'unique_value' should not be empty and it
	// should hold the last value being found.
	while (idx >=0 && idx < mNumDocs)
	{
		if (mValues[idx] ==  unique_value)
		{
			// The value is the same
			idx += reverse ? -1 : 1;
			continue;	
		}

		switch (opr)
		{
		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
		case eAosOpr_lt:
		case eAosOpr_le:
			 // Chen Ding, 2014/02/26
			 // if (AosIILUtil::valueMatch(mValues[idx], opr, value))
			 if (AosIILUtil::valueMatch(mValues[idx], opr, value, 0))
			 {
				 found = true;
				 unique_value = mValues[idx];
				 theidx.setIdx(mLevel, idx);
				 return true;
			 }
			 unique_value = 0;
			 theidx.setInvalid(mLevel);
			 theidx.setIdx(mLevel, idx);
			 return true;

		case eAosOpr_ne:
			 if (AosIILUtil::valueMatch(mValues[idx], value) != 0)
			 {
				 found = true;
				 unique_value = mValues[idx];
			 	 theidx.setIdx(mLevel, idx);
				 return true;
			 }
			 break;

		case eAosOpr_an:
			 found = true;
			 unique_value = mValues[idx];
			 return true;

		default:
		  	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 theidx.setInvalid(mLevel);
		  	 return false;
		}
		idx += reverse ? -1 : 1;
	}

	if (idx < 0 || idx >= mNumDocs) 
	{
		unique_value = 0;
		theidx.setInvalid(mLevel);
	}
	else
	{
	 	unique_value = mValues[idx];
		theidx.setIdx(mLevel, idx);
	}
	return true;
}



bool
AosIILBigU64::buildBitmap(
		const AosBitmapObjPtr &bitmap,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLevel == 0,false);
	bitmap->setBitmapId(mIILID);
	bitmap->setNodeLevel(0);
	bitmap->setIILLevel(0);
	bitmap->appendDocids(mDocids, mNumDocs);
	return true;
}





















/*

//change name form removeDocByIdxPriv
u64
AosIILBigU64::removeDocByIdxSinglePriv(
		const i64 theidx, 
		const AosRundataPtr &rdata)
{
	// If 'theidx' is -1, it removes the last entry. Otherwise, it 
	// removes 'theidx'-th entry. 
	if (mNumDocs <= 0) return AOS_INVDID;
	i64 idx = (theidx == -1)?mNumDocs-1:theidx;
	aos_assert_r(idx >= 0 && idx < mNumDocs, AOS_INVDID);

	u64 docid = mDocids[idx];
// shawn should not be in the single functions
	AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
	aos_assert_r(compiil, false);				
	bool rslt = compiil->removeDocSafe(mValues[idx],docid,rdata);
	returnCompIIL(compiil,rdata);
	aos_assert_r(rslt, false);

	// The docid to be removed is at 'idx'. Need to remove
	// it from mDocids. 
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);

	// Change mValues
	if (idx < mNumDocs-1)
	{
		// Need to move the memory
		memmove(&mDocids[idx], &mDocids[idx+1], sizeof(u64) * (mNumDocs - idx - 1));
		memmove(&mValues[idx], &mValues[idx+1], sizeof(u64) * (mNumDocs - idx - 1));
	}
	decNumDocsNotSafe();
	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	// if we have subiil, set root iil max/min value and mNumEntries. Note that we may need change them both
	if (isRootIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);
	}
	mIsDirty = true;
	return docid;
}




//change name from checkDocidUnique
bool
AosIILBigU64::checkDocidUniqueSinglePriv(
		const bool rslt, 
		const i64 idx, 
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
// shawn change the way impliment, leaf can not get its next leaf
	if (!rslt || idx < 0) 
	{
		docid = AOS_INVDID;
		isunique = false;
		return rslt;
	}

	if (idx >= mNumDocs)
	{
		OmnAlarm << "Something is incorrect: " << idx << ":" << mNumDocs << enderr;
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}

	isunique = true;
	bool hassame = false;

	//1. compare previous value
	if (idx > 0)
	{
		// Chen Ding, 06/26/2011
		hassame = (AosIILUtil::valueMatch(mValues[idx-1], eAosOpr_eq, mValues[idx]));
		// hassame = (value == mValues[idx-1]);
	}
	else if (mIILIdx > 0)
	{
		AosIILBigU64Ptr subiil = getNextSubIIL(false, rdata);
		if (subiil.notNull())
		{
			// Chen Ding, 06/26/2011
			hassame = (AosIILUtil::valueMatch(subiil->getLastValue(), eAosOpr_eq, mValues[idx]));
			// hassame = (value == subiil->getLastValue());
		}
	}

	if (hassame)
	{
		isunique = false;
		return true;
	}

	//1. compare next value
	if (idx < (mNumDocs -1))
	{
		// Chen Ding, 06/26/2011
		hassame = (AosIILUtil::valueMatch(mValues[idx+1], eAosOpr_eq, mValues[idx]));
		// hassame = (value == mValues[idx+1]);
	}
	else if (mIILIdx >= 0)
	{
		AosIILBigU64Ptr subiil = getNextSubIIL(true, rdata);
		if (subiil.notNull())
		{
			// Chen Ding, 06/26/2011
			hassame = (AosIILUtil::valueMatch(subiil->getFirstValue(), eAosOpr_eq, mValues[idx]));
			// hassame = (value == subiil->getFirstValue());
		}
	}
	if (hassame)
	{
		isunique = false;
	}

	return true;
}


//change name form firstDoc1
bool
AosIILBigU64::getFirstDocSinglePriv(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr,
		const u64 &value,
		u64 &docid)
{
	// Starting from the first entry, it searches for the firsst
	// doc that matches the condition (opr, value). If found, 
	// the docid is set to 'docid' and 'idx' is set to the 
	// position. Otherwise, 'docid' is set to AOS_INVDID and
	// 'idx' is set to -1. 
	if (mNumDocs == 0) 
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	
	idx = reverse ? mNumDocs - 1 : 0;
	switch (opr)
	{
	case eAosOpr_gt:
		 return nextDocidGT(idx, reverse, value, docid);

	case eAosOpr_ge:
		 return nextDocidGE(idx, reverse, value, docid);

	case eAosOpr_eq:
		 return nextDocidEQ(idx, reverse, value, docid);

	case eAosOpr_lt:
		 return nextDocidLT(idx, reverse, value, docid);

	case eAosOpr_le:
		 return nextDocidLE(idx, reverse, value, docid);

	case eAosOpr_ne:
		 return nextDocidNE(idx, reverse, value, docid);

	case eAosOpr_an:
		 return nextDocidAN(idx, reverse, value, docid);

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 docid = AOS_INVDID;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}

*/

bool		
AosIILBigU64::findPosSinglePriv(
		const AosOpr opr,
		const u64 &value,
		const u64 &docid, 
		AosIILIdx &idx,
		const bool reverse, 
		const AosRundataPtr &rdata) 
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is false) or the opposite
	// (reverse is true). The value must meet:
	// 		<value> <opr> 'value'
	//
	// It first advances 'idx' to the next element based on 'reverse'. 
	// If 'idx' is -10, it means starting from beginning. 
	//
	// If the next element matches the query, it returns the 
	// one. 
	//
	// If a doc is not found and this subiil is sure the searched doc
	// is not in the entire IIL, 'nomore' is set to true.
	//
	// Performance Analysis: 
	// Since the value list is sorted, it uses the binary search algorithm
	// to locate the first matching element in the array. The complexity
	// is log(n), which is fast enough. Since this is a string IIL, 
	// each compair is a string compair, which can be a little bit 
	// slow. 
	//
	// Possible Improvements:
	// Need to check whether memcmp() is faster than strcmp(). If yes,
	// we should probably use memcmp().
	
	if (mNumDocs == 0)
	{
	 	// No entries in the list.
	 	idx.setInvalid(mLevel);
	 	return true;
	}

	if (!mValues)
	{
	 	OmnAlarm << "mValues null" << enderr;
	 	idx.setInvalid(mLevel);
	 	return false;
	}

	int index = 0;
	if(reverse)
	{
		index = mNumDocs-1;
	}
	
	switch (opr)
	{
	case eAosOpr_gt:
		 index = reverse ? firstGTRev(index, value, docid, rdata) : firstGT(index, value, docid, rdata);
		 break;
	
	case eAosOpr_ge:
		 index = reverse ? firstGERev(index, value, docid, rdata) : firstGE(index, value, docid, rdata);
		 break;
	
	case eAosOpr_lt:
		 index = reverse ? firstLTRev(index, value, docid, rdata) : firstLT(index, value, docid, rdata);
		 break;
	
	case eAosOpr_le:
		 index = reverse ? firstLERev(index, value, docid, rdata) : firstLE(index, value, docid, rdata);
		 break;
	
	default:
	 	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
	 	 idx.setInvalid(mLevel);
	 	 return false;
	}
	if(index < 0 || index >= mNumDocs)
	{
	 	idx.setInvalid(mLevel);
	}
	else
	{
	 	idx.setIdx(mLevel, index);
	}
 	return true;
}


i64
AosIILBigU64::firstLE(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[idx], mDocids[idx], value, docid) > 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLERev(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata) 
{
	if (AosIILUtil::valueMatch(mValues[0], mDocids[0], value, docid) > 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[idx], mDocids[idx], value, docid) <= 0)
	{
		return idx;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right - 1)
	{
		return left;
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], mDocids[nn], value, docid) <= 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLT(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[idx], mDocids[idx], value, docid) >= 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstLTRev(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[0], mDocids[0], value, docid) >= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[idx], mDocids[idx], value, docid) < 0)
	{
		return idx;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// if (strcmp(mValues[nn], value.data()) < 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], mDocids[nn], value, docid) < 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return left;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGT(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// 1. Check whether the last one is < 'value'
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1],mDocids[mNumDocs-1], value, docid) <= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[idx],mDocids[idx], value, docid) > 0)
	{
		return idx;
	}

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1)
	{
		return right;
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], mDocids[nn], value, docid) <= 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return right;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGTRev(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], mDocids[mNumDocs-1], value, docid) <= 0)
	{
		return -1;
	}
	else
	{
		return (mNumDocs -1);
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGE(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value) < 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[idx], value) >= 0)
	{
		return idx;
	}

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1)
	{
		return right;
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (AosIILUtil::valueMatch(mValues[nn], value) < 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			return right;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigU64::firstGERev(const i64 &idx, const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], mDocids[mNumDocs-1], value, docid) < 0)
	{
		return -1;
	}
	else
	{
		return (mNumDocs -1);
	}

	OmnShouldNeverComeHere;
	return -1;
}

