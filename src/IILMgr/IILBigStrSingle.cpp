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
#include "IILMgr/IILBigStr.h"

#include "SEInterfaces/IILExecutorObj.h"


bool	
AosIILBigStr::setValue(
		const i64 &idx, 
		const char *value,
		const i64 &length)
{
	// Only 'AosIILUtil::eMaxStrValueLen' is stored
	//aos_assert_r(length >= 0, false);
	aos_assert_r(length > 0, false);

	// Chen Ding, 07/31/2012
	aos_assert_r(value, false);
	
	i64 len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
	if (len <= 0)
	{
		OmnAlarm << "Length invalid: " << len << enderr;
		len = eMinStrLen;
	}

	char *ptr = mValues[idx];
	if (ptr)
	{
		int *size = (int *)&ptr[-4];
		if (!(len <= *size && (len << 1) > *size))
		{
			OmnMemMgrSelf->release(ptr, __FILE__, __LINE__);
			ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
			aos_assert_r(ptr, false);
			mValues[idx] = ptr;
		}
	}
	else
	{
		ptr = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
		aos_assert_r(ptr, false);
		mValues[idx] = ptr;
	}

	// Chen Ding, 07/31/2012
	if (length == 0)
	{
		ptr[0] = 0;
	}
	else
	{
		aos_assert_r(value, false);
		strncpy(ptr, value, len);
		ptr[len] = 0;
	}
	return true;
}


bool 	
AosIILBigStr::insertBefore(
		const i64 &idx, 
		const u64 &docid, 
		const OmnString &value)
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
	memmove(&mValues[idx + 1], &mValues[idx], sizeof(char *) * (mNumDocs - idx));

	i64 len = value.length();
	if (len <= 0)
	{
		OmnAlarm << "Invalid len: " << len << enderr;
		len = eMinStrLen;
	}
	else if (len > AosIILUtil::eMaxStrValueLen) 
	{
		len = AosIILUtil::eMaxStrValueLen;
	}
	
	char *mem = OmnMemMgrSelf->allocate(len + 1, __FILE__, __LINE__);
	aos_assert_r(mem, false);
	
	strncpy(mem, value.data(), len);
	mem[len] = 0;

	// Change mValues
	mValues[idx] = mem;
	mDocids[idx] = docid;

	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


bool 	
AosIILBigStr::insertAfter(
		const i64 &idx, 
		const u64 &docid, 
		const OmnString &value)
{
	// It inserts an entry after 'idx'. 
	if (mNumDocs >= mMemCap)
	{
		aos_assert_r(expandMemoryPriv(), false);
	}
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < mNumDocs, false);

	i64 len = value.length();
	if (len <= 0)
	{
		OmnAlarm << "Invalid len: " << len << enderr;
		len = eMinStrLen;
	}
	if (len > AosIILUtil::eMaxStrValueLen) 
	{
		len = AosIILUtil::eMaxStrValueLen;
	}

	if (idx < mNumDocs - 1)
	{
		// Change mValues
		memmove(&mDocids[idx + 2], &mDocids[idx + 1], sizeof(u64) * (mNumDocs - idx - 1));
		memmove(&mValues[idx + 2], &mValues[idx + 1], sizeof(char *) * (mNumDocs - idx - 1));
	}
	char *mem = OmnMemMgrSelf->allocate(len + 1, __FILE__, __LINE__);
	aos_assert_r(mem, false);
	
	strncpy(mem, value.data(), len);
	mem[len] = 0;
	
	mDocids[idx+1] = docid;
	mValues[idx+1] = mem;
	
	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


//change name form addDocDirectPriv
bool
AosIILBigStr::addDocSinglePriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata) 
{
	// This function adds a new entry [value, docid] into the list. 
	// It is responsible for all adding.
	
	aos_assert_r(value != "", false);
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// Check whether we need to split

	bool rslt = false;
	i64 numBefore = mNumDocs;
	i64 idx = 0;
	u64 cmp_len = rdata->getCmpLen();
	if (mNumDocs == 0)
	{
		// Change mValues
		if (mNumDocs >= mMemCap) 
		{
			aos_assert_r(expandMemoryPriv(), false);
		}
		mDocids[0] = docid;
		rslt = setValue(0, value.data(), value.length());
		aos_assert_r(rslt, false);
		
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
			aos_assert_r(mValues[idx], false);
		
			rsltInt = AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len);
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
					rslt = setValue(mNumDocs, value.data(), value.length());
					aos_assert_r(rslt, false);
					
					incNumDocsNotSafe();
					idx = mNumDocs - 1;
					break;
				}

				aos_assert_r(mValues[idx + 1], false);
				// Check whether mValues[idx+1] >= value
				rsltInt2 = AosIILUtil::valueNMatch(mValues[idx + 1], value, mIsNumAlpha,cmp_len);
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

				aos_assert_r(mValues[idx - 1], false);
				// Chen Ding, 08/11/2010
				rsltInt2 = AosIILUtil::valueNMatch(mValues[idx - 1], value, mIsNumAlpha,cmp_len);
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
AosIILBigStr::insertDocSinglePriv(
		i64 &idx, 
		const OmnString &value, 
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
	aos_assert_r(mValues[0], false);
	aos_assert_r(mValues[mNumDocs-1], false);
	
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
AosIILBigStr::removeDocSinglePriv(
		const OmnString &value, 
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
	u64 cmp_len = rdata->getCmpLen();
	while(left <= right)
	{
		if (left == right)
		{
			rsltInt = AosIILUtil::valueNMatch(mValues[left], value, mIsNumAlpha,cmp_len);
			if (rsltInt == 0 && docid == mDocids[left])
			{
				found = true;
				idx = left;
			}
			break;
		}
		
		idx = (left + right) >> 1;
		rsltInt = AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len);
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
	aos_assert_r(AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len) == 0 && mDocids[idx] == docid, false);
	
	// Change mValues
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);
	if (idx < mNumDocs - 1)
	{
		i64 ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx + 1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx + 1], ss);

		ss = sizeof(char *) * (mNumDocs - idx - 1);
		OmnMemCheck (&mValues[idx + 1], ss);	
		OmnMemCheck(&mValues[idx], ss);	
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
AosIILBigStr::nextDocidLike(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	if (idx >= 0 && idx < mNumDocs)
	{
		if (!reverse)
		{
			for(; idx < mNumDocs; idx++)
			{
				if (AosIILUtil::valueLikeMatch(mValues[idx], value))
				{
					docid = mDocids[idx];
					return true;
				}
			}
		}
		else
		{
			for(;idx > 0; idx--)
			{
				if (AosIILUtil::valueLikeMatch(mValues[idx], value))
				{
					docid = mDocids[idx];
					return true;
				}
			}
		}
	}

	docid = AOS_INVDID;
	idx = -5;
	return true;
}


bool
AosIILBigStr::nextDocidAN(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
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
AosIILBigStr::nextDocidPrefix(
		i64 &idx, 
		const bool reverse, 
		const OmnString &prefix,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	aos_assert_r(mValues[idx], false);
	if (strncmp(mValues[idx], prefix.data(), prefix.length()) == 0)
	{
		docid = mDocids[idx];
		return true;
	}

	if (reverse)
	{
		idx = firstPrefixRev(idx, prefix);
	}
	else
	{
		idx = firstGE(idx, prefix, rdata);
		if (idx > 0 && strncmp(mValues[idx], prefix.data(), prefix.length()) != 0)
		{
			idx = -1;
		}
	}
	
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
AosIILBigStr::nextDocidGT(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) > 0)
	aos_assert_r(mValues[idx], false);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_gt, value, "", mIsNumAlpha,cmp_len))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstGTRev(idx, value, rdata) : firstGT(idx, value, rdata);
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
AosIILBigStr::nextDocidGE(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	aos_assert_r(mValues[idx], false);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_ge, value, "", mIsNumAlpha,cmp_len))
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
AosIILBigStr::nextDocidLT(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] < value
	// if (mValues[idx].compare1(value) < 0)
	aos_assert_r(mValues[idx], false);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_lt, value, "", mIsNumAlpha,cmp_len))
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
AosIILBigStr::nextDocidLE(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) <= 0)
	aos_assert_r(mValues[idx], false);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_le, value, "", mIsNumAlpha,cmp_len))
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
AosIILBigStr::nextDocidEQ(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
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
	aos_assert_r(mValues[idx], false);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_eq, value, "", mIsNumAlpha,cmp_len))
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
AosIILBigStr::nextDocidNE(
		i64 &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the next doc starting from 'idx' either in the:
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	aos_assert_r(mValues[idx], false);
	u64 cmp_len = rdata->getCmpLen();
	int rslt = AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len);
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
AosIILBigStr::firstPrefixRev(const i64 &idx, const OmnString &prefix)
{
	// IMPORTAT!!!!! 
	aos_assert_r(mValues[0], -1);
	if (strncmp(mValues[0], prefix.data(), prefix.length()) > 0)
	{
		return -1;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right - 1) 
	{
		if (strncmp(mValues[left], prefix.data(), prefix.length()) == 0)
		{
			return left;
		}
		else
		{
			return -1;
		}
	}
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);

		if (strncmp(mValues[nn], prefix.data(), prefix.length()) <= 0)
		{
			left = nn;
		}
		else
		{
			right = nn;
		}
		if (left == right-1)
		{
			if (strncmp(mValues[left], prefix.data(), prefix.length()) == 0)
			{
				return left;
			}
			else
			{
				return -1;
			}
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILBigStr::firstEQ(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) > 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return -1;
	aos_assert_r(mValues[idx], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	int rslt = AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len);

	// If mValues[idx] == value, return idx;
	if (rslt == 0) return idx;

	// If mValues[idx] > value, return -1
	if (rslt > 0) return -1;

	// If mValues[mNumDocs-1] < value, return -1
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], eAosOpr_lt, value, "", mIsNumAlpha,cmp_len))
	{
		return -1;
	}

	// Now, we are sure mValues[idx] <= value <= mValues[mNumDocs-1].  
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right || left + 1 == right) 
	{
		aos_assert_r(mValues[right], -1);
		if (AosIILUtil::valueNMatch(mValues[right], eAosOpr_eq, value, "", mIsNumAlpha,cmp_len))
		{
			return right;
		}
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		rslt = AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len);
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
				if (AosIILUtil::valueNMatch(mValues[left], eAosOpr_eq, value, "", mIsNumAlpha,cmp_len))
				{
					return left;
				}
				return right;
			}
		}
	}

	aos_assert_r(mValues[left], -1);
	if (left == right && AosIILUtil::valueNMatch(mValues[left], eAosOpr_eq, value, "", mIsNumAlpha,cmp_len))
	{
		return left;
	}
	return -1;
}


i64
AosIILBigStr::firstEQRev(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!!!
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) < 0) return -1;
	// if (mValues[0].compare1(value) > 0) return -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[idx], -1);
	u64 cmp_len = rdata->getCmpLen();
	// If mValues[idx] == value, return idx
	int rslt = AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len);
	if (rslt == 0) return idx;

	// If mValues[idx] is less than 'value', it is not possible
	if (rslt < 0) return -1;

	// If mValues[0] is greater than value, it is not possible
	if (AosIILUtil::valueNMatch(mValues[0], value, mIsNumAlpha,cmp_len) > 0) return -1;
		
	// Now, we are sure mValues[0] <= value <= mValues[idx].  
	i64 left = 0; 
	i64 right = idx;
	if (left == right) return -1;
	if (left + 1 == right) 
	{
		aos_assert_r(mValues[right], -1);
		if (AosIILUtil::valueNMatch(mValues[right], value, mIsNumAlpha,cmp_len) == 0)
		{
			return right;
		}
		if (AosIILUtil::valueNMatch(mValues[left], value, mIsNumAlpha,cmp_len) == 0)
		{
			return left;
		}
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		rslt = AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len);
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
				aos_assert_r(mValues[right], -1);
				if (AosIILUtil::valueNMatch(mValues[right], value, mIsNumAlpha,cmp_len) == 0)
				{
					return right;
				}
				return left;
			}
		}
	}

	aos_assert_r(mValues[left], -1);
	if (left == right && AosIILUtil::valueNMatch(mValues[left], value, mIsNumAlpha,cmp_len) == 0)
	{
		return left;
	}
	return -1;
}

	
i64
AosIILBigStr::firstNE(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry whose
	// value is not equal to 'value'. If not found, idx = -1.
	// if (mValues[mNumDocs-1] == value) return -1;
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) == 0)
	{
		return -1;
	}
		
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) != 0)
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
AosIILBigStr::firstNERev(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry 
	// where mValues[idx] != value.
	// If not found, idx = -1.
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], value, mIsNumAlpha,cmp_len) == 0)
	{
		return -1;
	}
		
	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) != 0)
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
AosIILBigStr::lastEQ(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata) 
{
	// IMPORTANT!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from idx, it searches for the last one that:
	// 		value == mValues[idx]
	// If not found, it returns -1;

	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) == 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) == 0)
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
AosIILBigStr::lastLT(const OmnString &value, const AosRundataPtr &rdata) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] < value
	// If not found, it returns -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], value, mIsNumAlpha,cmp_len) >= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) < 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) < 0)
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
AosIILBigStr::lastLE(const OmnString &value, const AosRundataPtr &rdata) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] <= value
	// If not found, it returns -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], value, mIsNumAlpha,cmp_len) > 0)
	{
		return -1;
	}
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) <= 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstLE(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len) > 0)
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
AosIILBigStr::firstLERev(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] <= value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], value, mIsNumAlpha,cmp_len) > 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstLT(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len) >= 0)
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
AosIILBigStr::firstLTRev(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], value, mIsNumAlpha,cmp_len) >= 0)
	{
		return -1;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) < 0)
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
AosIILBigStr::firstGT(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] <= value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] > value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) <= 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstGTRev(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstGE(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) < 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) < 0)
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
AosIILBigStr::firstGERev(const i64 &idx, const OmnString &value, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) < 0)
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
AosIILBigStr::nextDocidSinglePriv(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
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

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;
		
		case eAosOpr_prefix:
			 rslt = nextDocidPrefix(idx, reverse, value, docid, rdata);
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
		u64 cmp_len = rdata->getCmpLen();
		switch (opr)
		{
		case eAosOpr_gt:
			 if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_gt, value, "", mIsNumAlpha,cmp_len))
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
			 if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_ge, value, "", mIsNumAlpha,cmp_len))
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
			 if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_eq, value, "", mIsNumAlpha,cmp_len))
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
			 if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_lt, value, "", mIsNumAlpha,cmp_len))
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
			 if (AosIILUtil::valueNMatch(mValues[idx], eAosOpr_le, value, "", mIsNumAlpha,cmp_len))
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

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, reverse, value, docid, rdata);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;
		
		case eAosOpr_prefix:
			 rslt = nextDocidPrefix(idx, reverse, value, docid, rdata);
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

	OmnShouldNeverComeHere;
	idx = -5;
	docid = AOS_INVDID;
	return false;
}


//change name from getTotalPriv
i64 
AosIILBigStr::getTotalSinglePriv(
		const OmnString &value, 
		const AosOpr opr, 
		const AosRundataPtr &rdata)
{
	if (mNumDocs == 0)
	{
		return 0;
	}

	i64 startidx = 0;
	i64 endidx = 0;
	i64 num = 0;
	u64 cmp_len = rdata->getCmpLen();
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
		 aos_assert_r(mValues[0], 0);
		 startidx = (AosIILUtil::valueNMatch(mValues[0], 
					 eAosOpr_gt, value, "", mIsNumAlpha,cmp_len)) ? 0 : firstGT(0, value, rdata);
		 if (startidx == -1) 
		 {
			return 0;
		 }
		 return mNumDocs - startidx;

	case eAosOpr_ge:
		 // mValues[i] >= value
		 // startidx = (mValues[0].compare1(value) >= 0) ? 0 : firstGE(0, value);
		 aos_assert_r(mValues[0], 0);
		 startidx = (AosIILUtil::valueNMatch(mValues[0], 
					 eAosOpr_ge, value, "", mIsNumAlpha,cmp_len)) ? 0 : firstGE(0, value, rdata);
		 endidx = mNumDocs-1;
		 if (startidx == -1)
		 {
			return 0;
		 }
		 return mNumDocs - startidx;

	case eAosOpr_eq:
		 // mValues[i] == value
		 aos_assert_r(mValues[0], 0);
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
		 aos_assert_r(mValues[0], 0);
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

	case eAosOpr_like:
		 num = 0;
		 for (i64 i = 0; i< mNumDocs; i++)
		 {
		 	if (AosIILUtil::valueLikeMatch(mValues[i], value))
			{
				num++;
			}
		 }
		 return num;

	default:
		 OmnAlarm << "Incorrect operator: " << opr << enderr;
		 return -1;
	}

	OmnShouldNeverComeHere;
	return -1; 
}


bool
AosIILBigStr::setValueDocUniqueSinglePriv(
		const OmnString &key, 
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
	OmnString vv = key;
	u64 cmp_len = rdata->getCmpLen();
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
		  mDocids && AosIILUtil::valueNMatch(mValues[idx], vv, mIsNumAlpha,cmp_len) == 0))
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
	//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
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
AosIILBigStr::incrementDocidSinglePriv(
		const OmnString &key,
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
			//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
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
		//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
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
AosIILBigStr::nextUniqueValueSinglePriv(
		AosIILIdx &theidx,
		const bool reverse,
		const AosOpr opr,
		const OmnString &value,
		OmnString &unique_value,
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
	u64 cmp_len = rdata->getCmpLen();
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

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, reverse, value, docid, rdata);
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
		if (unique_value != "")
		{
			// It is to find the next unique value. Since this is to start from
			// new, 'unique_value' should (normally) be empty. 
			if (AosIILUtil::valueNMatch(unique_value.data(), mValues[idx], mIsNumAlpha,cmp_len) != 0)
			{
				// Found a unique value (i.e., mValues[idx] is different from 'unique_value'.
				unique_value.assign(mValues[idx], strlen(mValues[idx]));
				found = true;
				return true;
			}
			idx += reverse ? -1 : 1;
		}
		else
		{
			// 'unique_value' is empty.
			unique_value.assign(mValues[idx], strlen(mValues[idx]));
			found = true;
			return true;
		}
	}

	// It is not starting from new. 'unique_value' should not be empty and it
	// should hold the last value being found.
	aos_assert_r(unique_value != "", false);
	while (idx >=0 && idx < mNumDocs)
	{
		if (AosIILUtil::valueNMatch(mValues[idx], unique_value.data(), mIsNumAlpha,cmp_len) == 0)
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
			 if (AosIILUtil::valueNMatch(mValues[idx], opr, value, "", mIsNumAlpha,cmp_len))
			 {
				 found = true;
				 unique_value.assign(mValues[idx], strlen(mValues[idx]));
				 theidx.setIdx(mLevel, idx);
				 return true;
			 }
			 unique_value = "";
			 theidx.setInvalid(mLevel);
			 theidx.setIdx(mLevel, idx);
			 return true;

		case eAosOpr_ne:
			 if (AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len) != 0)
			 {
				 found = true;
				 unique_value.assign(mValues[idx], strlen(mValues[idx]));
			 	 theidx.setIdx(mLevel, idx);
				 return true;
			 }
			 break;

		case eAosOpr_an:
			 found = true;
			 unique_value.assign(mValues[idx], strlen(mValues[idx]));
			 return true;

		case eAosOpr_like:
			 if (AosIILUtil::valueLikeMatch(mValues[idx], value))
			 {
				 found = true;
			 	 unique_value.assign(mValues[idx], strlen(mValues[idx]));
			 	 theidx.setIdx(mLevel, idx);
				 return true;
			 }
			 break;

		default:
		  	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 theidx.setInvalid(mLevel);
		  	 return false;
		}
		idx += reverse ? -1 : 1;
	}

	if (idx < 0 || idx >= mNumDocs) 
	{
		unique_value = "";
		theidx.setInvalid(mLevel);
	}
	else
	{
	 	unique_value.assign(mValues[idx], strlen(mValues[idx]));
		theidx.setIdx(mLevel, idx);
	}
	return true;
}



bool
AosIILBigStr::buildBitmap(
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


bool		
AosIILBigStr::findPosSinglePriv(
		const AosOpr opr,
		const OmnString &value,
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
AosIILBigStr::firstLE(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], mDocids[idx], value, docid, mIsNumAlpha,cmp_len) > 0)
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
AosIILBigStr::firstLERev(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata) 
{
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], mDocids[0], value, docid, mIsNumAlpha,cmp_len) > 0)
	{
		return -1;
	}
	if (AosIILUtil::valueNMatch(mValues[idx], mDocids[idx], value, docid, mIsNumAlpha,cmp_len) <= 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], mDocids[nn], value, docid, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstLT(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[idx], mDocids[idx], value, docid, mIsNumAlpha,cmp_len) >= 0)
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
AosIILBigStr::firstLTRev(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[0], mDocids[0], value, docid, mIsNumAlpha,cmp_len) >= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueNMatch(mValues[idx], mDocids[idx], value, docid, mIsNumAlpha,cmp_len) < 0)
	{
		return idx;
	}

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], mDocids[nn], value, docid, mIsNumAlpha,cmp_len) < 0)
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
AosIILBigStr::firstGT(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1],mDocids[mNumDocs-1], value, docid, mIsNumAlpha,cmp_len) <= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueNMatch(mValues[idx],mDocids[idx], value, docid, mIsNumAlpha,cmp_len) > 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], mDocids[nn], value, docid, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstGTRev(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], mDocids[mNumDocs-1], value, docid, mIsNumAlpha,cmp_len) <= 0)
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
AosIILBigStr::firstGE(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], value, mIsNumAlpha,cmp_len) < 0)
	{
		return -1;
	}
	if (AosIILUtil::valueNMatch(mValues[idx], value, mIsNumAlpha,cmp_len) >= 0)
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
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueNMatch(mValues[nn], value, mIsNumAlpha,cmp_len) < 0)
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
AosIILBigStr::firstGERev(const i64 &idx, const OmnString &value, const u64 &docid, const AosRundataPtr &rdata)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	u64 cmp_len = rdata->getCmpLen();
	if (AosIILUtil::valueNMatch(mValues[mNumDocs-1], mDocids[mNumDocs-1], value, docid, mIsNumAlpha,cmp_len) < 0)
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
