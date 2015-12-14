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
#include "IILMgrBig/IILStr.h"

#include "IILMgrBig/IILCompStr.h"


bool	
AosIILStr::setValue(
		const int idx, 
		const char *value,
		const int length)
{
	// Only 'AosIILUtil::eMaxStrValueLen' is stored
	//aos_assert_r(length >= 0, false);
	aos_assert_r(length > 0, false);

	// Chen Ding, 07/31/2012
	aos_assert_r(value, false);
	
	int len = (length > AosIILUtil::eMaxStrValueLen) ? AosIILUtil::eMaxStrValueLen : length;
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
AosIILStr::insertBefore(
		const int idx, 
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

	u32 len = value.length() + 1;
	if (len > AosIILUtil::eMaxStrValueLen) 
	{
		len = AosIILUtil::eMaxStrValueLen;
	}
	if (len <= 0)
	{
		OmnAlarm << "Invalid len: " << len << enderr;
		len = eMinStrLen;
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
AosIILStr::insertAfter(
		const int idx, 
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
	aos_assert_r((u32)idx < mNumDocs, false);

	u32 len = value.length() + 1;
	if (len > AosIILUtil::eMaxStrValueLen) 
	{
		len = AosIILUtil::eMaxStrValueLen;
	}
	if (len <= 0)
	{
		OmnAlarm << "Invalid len: " << len << enderr;
		len = eMinStrLen;
	}

	if (idx < (int)mNumDocs - 1)
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
AosIILStr::addDocSinglePriv(
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
	u32 numBefore = mNumDocs;
	int idx = 0;
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
		int left = 0; 
		int right = mNumDocs - 1;
		while (left <= right)
		{
			idx = left + ((right - left) >> 1);
			aos_assert_r(mValues[idx], false);
		
			rsltInt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
			if (rsltInt == 0)
			{
				// A match is found. Check whether value is unique
				if (value_unique)
				{					
					// The value must be unique. it returns an error. 
					if (rdata->needReportError())
					{
						rdata->setError()<< "Value already exist: " << value << ":" << docid
							<< ":" << mDocids[idx];
						OmnAlarm << rdata->getErrmsg()<< enderr;
					}
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
				// if (strcmp(mValues[idx], value.data()) < 0)
				if ((u32)idx == mNumDocs - 1)
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
				// if (strcmp(value.data(), mValues[idx+1]) < 0)
				rsltInt2 = AosIILUtil::valueMatch(mValues[idx + 1], value, mIsNumAlpha);
				if(rsltInt2 > 0)
				{
					// Found the spot. Need to move the elements after
					// 'idx' to make room for the new docid
					insertAfter(idx, docid, value);
					idx++;
					break;
				}
				else if(rsltInt2 == 0)
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
				// if (strcmp(mValues[idx-1], value.data()) < 0)
				rsltInt2 = AosIILUtil::valueMatch(mValues[idx - 1], value, mIsNumAlpha);
				if (rsltInt2 < 0)
				{
					// Found the spot. Need to move the elements after
					// 'idx' to make room for the new docid
					idx--;
					insertAfter(idx, docid, value);
					idx++;
					break;
				}
				else if(rsltInt2 == 0)
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
		bool updateMax = ((u32)idx >= mNumDocs - 1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mParentIIL, false);
		mParentIIL->updateIndexData(mIILIdx, updateMax, updateMin, rdata);
	}

	if (mNumDocs >= mMaxIILSize)
	{
		rslt = splitListPriv(false, rdata);
		aos_assert_r(rslt, false);
	}
	sanityTestForSubiils();
	return true;
}


//change name form insertDocPriv
bool
AosIILStr::insertDocSinglePriv(
		int &idx, 
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
	int left = firstEQ(0, value);
	aos_assert_r(left >= 0 && (u32)left < mNumDocs, false);
	
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

	int right = firstEQRev(mNumDocs - 1, value);
	aos_assert_r(right >= left && (u32)right < mNumDocs, false);

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

		int nn = (left + right) >> 1;
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
AosIILStr::removeDocSinglePriv(
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

	int left = 0;
	int right = mNumDocs - 1;
	int idx = -1;
	int rsltInt;
	bool found = false;
	while(left <= right)
	{
		if(left == right)
		{
			rsltInt = AosIILUtil::valueMatch(mValues[left], value, mIsNumAlpha);
			if(rsltInt == 0 && docid == mDocids[left])
			{
				found = true;
				idx = left;
			}
			break;
		}
		
		idx = (left + right) >> 1;
		rsltInt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
		if(rsltInt == 0)
		{
			if(mDocids[idx] == docid)
			{
				found = true;
				break;
			}
			else if(mDocids[idx] < docid)
			{
				left = idx + 1;
			}
			else
			{
				right = idx - 1;
			}
		}
		else if(rsltInt == -1)
		{
			left = idx + 1;
		}
		else
		{
			right = idx - 1;
		}
	}

	if(!found || idx < 0 || (u32)idx >= mNumDocs)
	{
		return false;
	}
	aos_assert_r(AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) == 0 && mDocids[idx] == docid, false);
	
	// Change mValues
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);
	if ((u32)idx < mNumDocs - 1)
	{
		int ss = sizeof(u64) * (mNumDocs - idx - 1);
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
		bool updateMax = ((u32)idx >= mNumDocs - 1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mParentIIL, false);
		mParentIIL->updateIndexData(mIILIdx, updateMax, updateMin, rdata);
//shawnquleft
		if(mNumDocs < mMinIILSize)
		{
//shawnquleft
			mParentIIL->mergeSubiilPriv(true, mIILIdx, rdata);
		}
	}
	
	return true;
}


bool
AosIILStr::nextDocidLike(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	if (idx >= 0 && (u32)idx < mNumDocs)
	{
		if (!reverse)
		{
			for(; (u32)idx < mNumDocs; idx++)
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
AosIILStr::nextDocidAN(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It matches any entry. It simply retrieves the current one, 
	// if any, and advances 'idx'. 
	if (idx >= 0 && (u32)idx < mNumDocs)
	{
		docid = mDocids[idx];
		return true;
	}

	docid = AOS_INVDID;
	idx = -5;
	return true;
}


bool
AosIILStr::nextDocidPrefix(
		int &idx, 
		const bool reverse, 
		const OmnString &prefix,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	aos_assert_r(mValues[idx], false);
	// if (strcmp(mValues[idx], value.data()) >= 0)
	if(strncmp(mValues[idx], prefix.data(), prefix.length()) == 0)
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
		idx = firstGE(idx, prefix);
		if(strncmp(mValues[idx], prefix.data(), prefix.length()) != 0)
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
AosIILStr::nextDocidGT(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) > 0)
	aos_assert_r(mValues[idx], false);

	// if (strcmp(mValues[idx], value.data()) > 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value, mIsNumAlpha))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstGTRev(idx, value) : firstGT(idx, value);
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
AosIILStr::nextDocidGE(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	aos_assert_r(mValues[idx], false);
	// if (strcmp(mValues[idx], value.data()) >= 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value, mIsNumAlpha))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstGERev(idx, value) : firstGE(idx, value);
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
AosIILStr::nextDocidLT(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] < value
	// if (mValues[idx].compare1(value) < 0)
	aos_assert_r(mValues[idx], false);
	// if (strcmp(mValues[idx], value.data()) < 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value, mIsNumAlpha))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstLTRev(idx, value) : firstLT(idx, value);
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
AosIILStr::nextDocidLE(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) <= 0)
	aos_assert_r(mValues[idx], false);
	// if (strcmp(mValues[idx], value.data()) <= 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value, mIsNumAlpha))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstLERev(idx, value) : firstLE(idx, value);
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
AosIILStr::nextDocidEQ(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	//
	// If success, 'docid' holds the docid found and 'idx' points to
	// the next element (IMPORTANT: It does not point to the current one).
	// if (mValues[idx] == value)
	aos_assert_r(mValues[idx], false);
	// if (strcmp(mValues[idx], value.data()) == 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value, mIsNumAlpha))
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstEQRev(idx, value) : firstEQ(idx, value);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}
	
	docid = mDocids[idx];
	return true;
}


int
AosIILStr::nextDocidNE(
		int &idx, 
		const bool reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the:
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	// int rslt = value.compare1(mValues[idx]);
	aos_assert_r(mValues[idx], -1);
	// int rslt = strcmp(mValues[idx], value.data());
	int rslt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
	if (rslt != 0)
	{
		docid = mDocids[idx];
		return true;
	}

	idx = reverse ? firstNERev(idx, value) : firstNE(idx, value);
	if (idx == -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}

	
int
AosIILStr::firstPrefixRev(const int idx, const OmnString &prefix)
{
	// IMPORTAT!!!!! 
	aos_assert_r(mValues[0], -1);
	// if (strcmp(mValues[0], value.data()) > 0) return -1;
	if (strncmp(mValues[0], prefix.data(), prefix.length()) > 0)
	{
		return -1;
	}

	int left = 0; 
	int right = idx;
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
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);

		// if (strcmp(mValues[nn], value.data()) <= 0) left = nn; else right = nn;
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


int
AosIILStr::firstEQ(const int idx, const OmnString &value)
{
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) > 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return -1;
	aos_assert_r(mValues[idx], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);

	// int rslt = strcmp(mValues[idx], value.data());
	int rslt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);

	// If mValues[idx] == value, return idx;
	if (rslt == 0) return idx;

	// If mValues[idx] > value, return -1
	if (rslt > 0) return -1;

	// If mValues[mNumDocs-1] < value, return -1
	// if (strcmp(mValues[mNumDocs-1], value.data()) < 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], eAosOpr_lt, value, mIsNumAlpha))
	{
		return -1;
	}

	// Now, we are sure mValues[idx] <= value <= mValues[mNumDocs-1].  
	int left = idx; 
	int right = mNumDocs-1;
	if (left == right || left + 1 == right) 
	{
		aos_assert_r(mValues[right], -1);
		// if (strcmp(mValues[right], value.data()) == 0) return right;
		if (AosIILUtil::valueMatch(mValues[right], eAosOpr_eq, value, mIsNumAlpha))
		{
			return right;
		}
		return -1;
	}

	while (left < right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		// int rslt = strcmp(mValues[nn], value.data());
		int rslt = AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha);
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
				// if (strcmp(mValues[left], value.data()) == 0) return left;
				if (AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value, mIsNumAlpha))
				{
					return left;
				}
				return right;
			}
		}
	}

	aos_assert_r(mValues[left], -1);
	// if (left == right && strcmp(mValues[left], value.data()) == 0) return left;
	if (left == right && AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value, mIsNumAlpha))
	{
		return left;
	}
	return -1;
}


int
AosIILStr::firstEQRev(const int idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!!!
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) < 0) return -1;
	// if (mValues[0].compare1(value) > 0) return -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[idx], -1);

	// If mValues[idx] == value, return idx
	// int rslt = strcmp(mValues[idx], value.data());
	int rslt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
	if (rslt == 0) return idx;

	// If mValues[idx] is less than 'value', it is not possible
	if (rslt < 0) return -1;

	// If mValues[0] is greater than value, it is not possible
	// if (strcmp(mValues[0], value.data()) > 0) return -1;
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) > 0) return -1;
		
	// Now, we are sure mValues[0] <= value <= mValues[idx].  
	int left = 0; 
	int right = idx;
	if (left == right) return -1;
	if (left + 1 == right) 
	{
		aos_assert_r(mValues[right], -1);
		// if (strcmp(mValues[right], value.data()) == 0) return right;
		// if (strcmp(mValues[left], value.data()) == 0) return left;
		if (AosIILUtil::valueMatch(mValues[right], value, mIsNumAlpha) == 0)
		{
			return right;
		}
		if (AosIILUtil::valueMatch(mValues[left], value, mIsNumAlpha) == 0)
		{
			return left;
		}
		return -1;
	}

	while (left < right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		// int rslt = strcmp(mValues[nn], value.data());
		int rslt = AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha);
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
				// if (strcmp(mValues[right], value.data()) == 0) return right;
				if (AosIILUtil::valueMatch(mValues[right], value, mIsNumAlpha) == 0)
				{
					return right;
				}
				return left;
			}
		}
	}

	aos_assert_r(mValues[left], -1);
	// if (left == right && strcmp(mValues[left], value.data()) == 0) return left;
	if (left == right && AosIILUtil::valueMatch(mValues[left], value, mIsNumAlpha) == 0)
	{
		return left;
	}
	return -1;
}

	
int
AosIILStr::firstNE(const int idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry whose
	// value is not equal to 'value'. If not found, idx = -1.
	// if (mValues[mNumDocs-1] == value) return -1;
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[mNumDocs-1], value.data()) == 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) == 0)
	{
		return -1;
	}
		
	int left = idx; 
	int right = mNumDocs-1;
	if (left == right-1) return right;
	while (left < right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) != 0) right=nn; else left = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) != 0)
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


int
AosIILStr::firstNERev(const int idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry 
	// where mValues[idx] != value.
	// If not found, idx = -1.
	// if (mValues[0].compare1(value) == 0) return -1;
	aos_assert_r(mValues[0], -1);
	// if (strcmp(mValues[0], value.data()) == 0) return -1;
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) == 0)
	{
		return -1;
	}
		
	int left = 0; 
	int right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		int nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) != 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) != 0)
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


int
AosIILStr::lastEQ(const int idx, const OmnString &value) 
{
	// IMPORTANT!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from idx, it searches for the last one that:
	// 		value == mValues[idx]
	// If not found, it returns -1;

	// if (mValues[mNumDocs-1] == value) return mNumDocs-1;
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[mNumDocs-1], value.data()) == 0) return mNumDocs-1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) == 0)
	{
		return mNumDocs-1;
	}

	// Now, mValues[idx] <= value < mValues[mNumDocs-1]
	int left = idx; 
	int right = mNumDocs-1;
	if (left == right-1) return left;
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) == 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) == 0)
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


int
AosIILStr::lastLT(const OmnString &value) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] < value
	// If not found, it returns -1;
	// if (mValues[0].compare1(value) >= 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return mNumDocs-1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	// if (strcmp(mValues[mNumDocs-1], value.data()) < 0) return mNumDocs-1;
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) >= 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) < 0)
	{
		return mNumDocs-1;
	}

	// Now, mValues[0] < value <= mValues[mNumDocs-1]
	int left = 0; 
	int right = mNumDocs-1;
	// Now, 'left' points to a good one
	// 'right' points to a bad one. 
	if (left == right-1) return left;
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) < 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) < 0)
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


int
AosIILStr::lastLE(const OmnString &value) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] <= value
	// If not found, it returns -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[0], value.data()) > 0) return -1;
	// if (strcmp(mValues[mNumDocs-1], value.data()) <= 0) return mNumDocs-1;
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) > 0)
	{
		return -1;
	}
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) <= 0)
	{
		return mNumDocs-1;
	}

	// Now, mValues[0] <= value < mValues[mNumDocs-1]
	int left = 0; 
	int right = mNumDocs-1;
	// Now, 'left' points to a good one
	// 'right' points to a bad one. 
	if (left == right-1) return left;
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) <= 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) <= 0)
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


int
AosIILStr::firstLE(const int idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) > 0)
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


int
AosIILStr::firstLERev(const int idx, const OmnString &value) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] <= value
	aos_assert_r(mValues[0], -1);
	// if (strcmp(mValues[0], value.data()) > 0) return -1;
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) > 0)
	{
		return -1;
	}

	int left = 0; 
	int right = idx;
	if (left == right - 1)
	{
		return left;
	}
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) <= 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) <= 0)
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


int
AosIILStr::firstLT(const int idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) >= 0)
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


int
AosIILStr::firstLTRev(const int idx, const OmnString &value) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) >= 0)
	{
		return -1;
	}

	int left = 0; 
	int right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) < 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) < 0)
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


int
AosIILStr::firstGT(const int idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] <= value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] > value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[mNumDocs-1], value.data()) <= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) <= 0)
	{
		return -1;
	}

	int left = idx; 
	int right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1)
	{
		return right;
	}
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) <= 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) <= 0)
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


int
AosIILStr::firstGTRev(const int idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) <= 0)
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


int
AosIILStr::firstGE(const int idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[mNumDocs-1], value.data()) < 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) < 0)
	{
		return -1;
	}

	int left = idx; 
	int right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1)
	{
		return right;
	}
	while (left < right)
	{
		int nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		// if (strcmp(mValues[nn], value.data()) < 0) left = nn; else right = nn;
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) < 0)
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


int
AosIILStr::firstGERev(const int idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	// if (strcmp(mValues[0], value.data()) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) < 0)
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
AosIILStr::nextDocidSinglePriv(
		int &idx, 
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
		isunique = false;
	 	return true;
	}

	if (!mValues)
	{
	 	OmnAlarm << "mValues null" << enderr;
	 	idx = -5;
	 	docid = AOS_INVDID;
		isunique = false;
	 	return false;
	}

	if ((idx < 0 && idx != -10) || (idx > 0 && (u32)idx >= mNumDocs))
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		idx = -5;
		docid = AOS_INVDID;
		isunique = false;
		return false;
	}

	bool startnew = false;
	if(reverse)
	{
		if(idx == 0)
		{
			idx = -5;
			docid = AOS_INVDID;
			isunique = false;
			return true;
		}
		else if(idx == -10)
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
		if(idx >= (int)mNumDocs -1)
		{
			// is the last one, we can't find the next
			idx = -5;
			docid = AOS_INVDID;
			isunique = false;
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
	if (idx < 0 || (u32)idx >= mNumDocs)
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		idx = -5;
		docid = AOS_INVDID;
		isunique = false;
		return false;
	}

	bool rslt;
	if (startnew)
	{
		switch (opr)
		{
		case eAosOpr_gt:
		 	 rslt = nextDocidGT(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_ge:
		 	 rslt = nextDocidGE(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_eq:
		 	 rslt = nextDocidEQ(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_lt:
		 	 rslt = nextDocidLT(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_le:
		 	 rslt = nextDocidLE(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;
		
		case eAosOpr_ne:
		 	 rslt = nextDocidNE(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;
		
		case eAosOpr_an:
		 	 rslt = nextDocidAN(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
		 	 return rslt;

		case eAosOpr_prefix:
			 rslt = nextDocidPrefix(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		default:
		 	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 	 idx = -5;
		 	 docid = AOS_INVDID;
		 	 isunique = false;
		 	 return false;
		}
	}
	else
	{
		// It is not a new. Just check the current entry.
		switch (opr)
		{
		case eAosOpr_gt:
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value, mIsNumAlpha))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = false;
			 return true;

		case eAosOpr_ge:
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value, mIsNumAlpha))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = false;
			 return true;

		case eAosOpr_eq:
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value, mIsNumAlpha))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = false;
			 return true;
	
		case eAosOpr_lt:
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value, mIsNumAlpha))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = false;
			 return true;

		case eAosOpr_le:
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value, mIsNumAlpha))
			 {
				 docid = mDocids[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = false;
			 return true;

		case eAosOpr_ne:
			 rslt = nextDocidNE(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_an:
			 docid = mDocids[idx];
			 checkDocidUnique(true, idx, docid, isunique, rdata);
			 return true;

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;
		
		case eAosOpr_prefix:
			 rslt = nextDocidPrefix(idx, reverse, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		default:
		  	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		  	 idx = -5;
			 docid = AOS_INVDID;
			 isunique = false;
		  	 return false;
		}
	}

	OmnShouldNeverComeHere;
	idx = -5;
	docid = AOS_INVDID;
	return false;
}


//change name from getTotalPriv
u64 
AosIILStr::getTotalSinglePriv(
		const OmnString &value, 
		const AosOpr opr, 
		const AosRundataPtr &rdata)
{
	if (mNumDocs == 0)
	{
		return 0;
	}

	int startidx = 0;
	int endidx = 0;
	int num = 0;
	switch (opr)
	{
	case eAosOpr_le:
		 // mValues[i] <= value
		 endidx = lastLE(value);
		 if (endidx == -1)
		 {
			return 0;
		 }
		 return endidx + 1;

	case eAosOpr_lt:
		 // mValues[i] < value
		 endidx = lastLT(value);
		 if (endidx == -1)
		 {
			return 0;
		 }
		 return endidx + 1;

	case eAosOpr_gt:
		 // mValues[i] > value
		 // startidx = (mValues[0].compare1(value) > 0) ? 0 : firstGT(0, value);
		 aos_assert_r(mValues[0], 0);
		 startidx = (AosIILUtil::valueMatch(mValues[0], 
					 eAosOpr_gt, value, mIsNumAlpha)) ? 0 : firstGT(0, value);
		 if (startidx == -1) 
		 {
			return 0;
		 }
		 return mNumDocs - startidx;

	case eAosOpr_ge:
		 // mValues[i] >= value
		 // startidx = (mValues[0].compare1(value) >= 0) ? 0 : firstGE(0, value);
		 aos_assert_r(mValues[0], 0);
		 startidx = (AosIILUtil::valueMatch(mValues[0], 
					 eAosOpr_ge, value, mIsNumAlpha)) ? 0 : firstGE(0, value);
		 endidx = mNumDocs-1;
		 if (startidx == -1)
		 {
			return 0;
		 }
		 return mNumDocs - startidx;

	case eAosOpr_eq:
		 // mValues[i] == value
		 aos_assert_r(mValues[0], 0);
		 startidx = firstEQ(0, value);
		 if (startidx < 0) 
		 {
			 return 0;
		 }
		 endidx = lastEQ(startidx, value);
		 aos_assert_r(endidx >= startidx && (u32)endidx < mNumDocs, 0);
		 return endidx - startidx + 1;

	case eAosOpr_ne:
		 // mValues[i] != value
		 aos_assert_r(mValues[0], 0);
		 startidx = firstEQ(0, value);
		 if (startidx < 0) 
		 {
			 return mNumDocs;
		 }
		 endidx = lastEQ(startidx, value);
		 aos_assert_r(endidx >= startidx && (u32)endidx < mNumDocs, 0);
		 return mNumDocs - (endidx - startidx + 1);

	case eAosOpr_an:
		 return mNumDocs;

	case eAosOpr_like:
		 num = 0;
		 for (u32 i = 0; i< mNumDocs; i++)
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
AosIILStr::setValueDocUniqueSinglePriv(
		const OmnString &key, 
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	// If the list contains [key, docid], override 'docid'. Otherwise, 
	// it adds an entry [key, docid].
	aos_assert_rr(isLeafIIL(), rdata, false);

	int idx = -10;
	bool isunique;
	u64 did = 0;
	OmnString vv = key;
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
	if (!(mValues && (u32)idx < mNumDocs &&
		  mDocids && AosIILUtil::valueMatch(mValues[idx], vv, mIsNumAlpha) == 0))
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
	//aos_assert_rr(compiil, rdata, false);
	//rslt = compiil->removeDocSafe(key, did, rdata);
	//if (!rslt)
	//{
	//	returnCompIIL(compiil, rdata);
	//	aos_assert_rr(rslt, rdata, false);
	//}
	//rslt = compiil->addDocSafe(key, docid, false, false, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_rr(rslt, rdata, false);
	return true; 
}


bool
AosIILStr::incrementDocidSinglePriv(
		const OmnString &key,
		u64 &value,
		const u64 &incvalue,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(isLeafIIL(), rdata, false);

	// This function retrieves the value identified by 'key'. 
	// If the entry does not exist, it checks 'add_flag'. If it
	// is true, it will add it. 
	// It increments the docid by 'incvalue' and returns
	// the old value (i.e., the value before incrementing) of the docid.
	
	u64 docid;
	bool isunique;
	int idx = -10;
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
		//if(!rslt)
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
AosIILStr::nextUniqueValueSinglePriv(
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

	int idx = theidx.getIdx(mLevel);
	if ((idx < 0 && idx != -10) || (idx >= (int)mNumDocs))
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		theidx.setInvalid(mLevel);
		return false;
	}
	
	bool startnew = false;
	if(idx == -10)
	{
		idx = reverse ? mNumDocs - 1 : 0;
		startnew = true;
	}
	else
	{
		idx = reverse ? idx - 1 : idx + 1;
	}
	if(idx < 0 || idx >= (int)mNumDocs)
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
			 rslt = nextDocidGT(idx, reverse, value, docid);
			 break;

		case eAosOpr_ge:
			 rslt = nextDocidGE(idx, reverse, value, docid);
			 break;

		case eAosOpr_eq:
			 rslt = nextDocidEQ(idx, reverse, value, docid);
			 break;

		case eAosOpr_lt:
			 rslt = nextDocidLT(idx, reverse, value, docid);
			 break;

		case eAosOpr_le:
			 rslt = nextDocidLE(idx, reverse, value, docid);
			 break;

		case eAosOpr_ne:
			 rslt = nextDocidNE(idx, reverse, value, docid);
			 break;

		case eAosOpr_an:
			 rslt = nextDocidAN(idx, reverse, value, docid);
			 break;

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, reverse, value, docid);
			 break;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 theidx.setInvalid(mLevel);
		  	 return false;
		}

		if(!rslt || idx < 0 || idx >= (int)mNumDocs)
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
			//if (strcmp(unique_value.data(), mValues[idx]) != 0)
			if (AosIILUtil::valueMatch(unique_value.data(), mValues[idx], mIsNumAlpha) != 0)
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
	while (idx >=0 && idx < (int)mNumDocs)
	{
		//if (strcmp(mValues[idx], unique_value.data()) == 0)
		if (AosIILUtil::valueMatch(mValues[idx], unique_value.data(), mIsNumAlpha) == 0)
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
			 if (AosIILUtil::valueMatch(mValues[idx], opr, value, mIsNumAlpha))
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
			 if(AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) != 0)
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

	if (idx < 0 || idx >= (int)mNumDocs) 
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

































/*

//change name form removeDocByIdxPriv
u64
AosIILStr::removeDocByIdxSinglePriv(
		const int theidx, 
		const AosRundataPtr &rdata)
{
	// If 'theidx' is -1, it removes the last entry. Otherwise, it 
	// removes 'theidx'-th entry. 
	if (mNumDocs <= 0) return AOS_INVDID;
	int idx = (theidx == -1)?mNumDocs-1:theidx;
	aos_assert_r(idx >= 0 && (u32)idx < mNumDocs, AOS_INVDID);

	u64 docid = mDocids[idx];
// shawn should not be in the single functions
	AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
	aos_assert_r(compiil, false);				
	bool rslt = compiil->removeDocSafe(mValues[idx],docid,rdata);
	returnCompIIL(compiil,rdata);
	aos_assert_r(rslt, false);

	// The docid to be removed is at 'idx'. Need to remove
	// it from mDocids. 
	aos_assert_r(mValues[idx], false);
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);

	// Change mValues
	if ((u32)idx < mNumDocs-1)
	{
		// Need to move the memory
		memmove(&mDocids[idx], &mDocids[idx+1], sizeof(u64) * (mNumDocs - idx - 1));
		memmove(&mValues[idx], &mValues[idx+1], sizeof(char *) * (mNumDocs - idx - 1));
	}
	decNumDocsNotSafe();
	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	// if we have subiil, set root iil max/min value and mNumEntries. Note that we may need change them both
	if (isRootIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = ((u32)idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);
	}
	mIsDirty = true;
	return docid;
}




bool	
AosIILStr::nextDocidPrefixSinglePriv(
		int &idx, 
		const bool reverse,
		const OmnString &prefix,
		u64 &docid,
		bool &keepSearch)
{
	bool rslt = false;
	keepSearch = true;
	if (mNumDocs == 0)
	{
	 	// No entries in the list.
	 	idx = -5;
	 	docid = AOS_INVDID;
	 	return true;
	}

	if (!mValues)
	{
	 	OmnAlarm << "mValues null" << enderr;
	 	idx = -5;
	 	docid = AOS_INVDID;
	 	return false;
	}

	if (idx < 0)
	{
		if (idx != -10)
		{
			OmnAlarm << "idx: " << idx << enderr;
			idx = -5;
			docid = AOS_INVDID;
			return false;
		}
	}
	else
	{
		if ((u32)idx >= mNumDocs)
		{
			OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
			idx = -5;
			docid = AOS_INVDID;
			return false;
		}
	}

	bool startnew = false;
	if(idx == -10)
	{
		if(reverse)
		{
			idx = mNumDocs - 1;
		}
		else
		{
			idx = 0;
		}
		startnew = true;
	}
	else
	{
		if(idx < 0) 
		{
			idx = -5;
			docid = AOS_INVDID;
			OmnAlarm << "idx: " << idx << enderr;
			return false;
		}

		if(idx == 0 && reverse)
		{
			idx = -5;
			docid = AOS_INVDID;
			return true;
		}

		if(idx > (int)mNumDocs -1)
		{
			// is the last one, we can't find the next
			idx = -5;
			docid = AOS_INVDID;
			return true;
		}

		if(idx == (int)mNumDocs -1 && !reverse)
		{
			// is the last one, we can't find the next
			idx = -5;
			docid = AOS_INVDID;
			return true;
		}

		// not the last one
		if(reverse)
		{
			idx --;
		}
		else
		{
			idx++;
		}
		keepSearch = false;
	}


	if (startnew)
	{
		// from head
		if(!reverse)
		{
			rslt = nextDocidGE(idx, reverse, prefix, docid);
		}
		else
		{
			rslt = nextDocidPrefix(idx, reverse, prefix, docid);
		}
		if(!rslt)
		{
			idx = -5;
			docid = AOS_INVDID;
			return false;
		}
	}
	
	if(strncmp(mValues[idx],prefix.data(),prefix.length()) == 0)
	{
		docid = mDocids[idx];
		return true;
	}
	else
	{
		keepSearch = false;
		docid = AOS_INVDID;
		idx = -5;
		return true;
	}
	return false;
}


//change name from checkDocidUnique
bool
AosIILStr::checkDocidUniqueSinglePriv(
		const bool rslt, 
		const int idx, 
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

	if ((u32)idx >= mNumDocs)
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
		hassame = (AosIILUtil::valueMatch(mValues[idx-1], eAosOpr_eq, mValues[idx], mIsNumAlpha));
		// hassame = (value == mValues[idx-1]);
	}
	else if(mIILIdx > 0)
	{
		AosIILStrPtr subiil = getNextSubIIL(false, rdata);
		if(subiil.notNull())
		{
			// Chen Ding, 06/26/2011
			hassame = (AosIILUtil::valueMatch(subiil->getLastValue(), eAosOpr_eq, mValues[idx], mIsNumAlpha));
			// hassame = (value == subiil->getLastValue());
		}
	}

	if (hassame)
	{
		isunique = false;
		return true;
	}

	//1. compare next value
	if (idx < (int)(mNumDocs -1))
	{
		// Chen Ding, 06/26/2011
		hassame = (AosIILUtil::valueMatch(mValues[idx+1], eAosOpr_eq, mValues[idx], mIsNumAlpha));
		// hassame = (value == mValues[idx+1]);
	}
	else if(mIILIdx >= 0)
	{
		AosIILStrPtr subiil = getNextSubIIL(true, rdata);
		if(subiil.notNull())
		{
			// Chen Ding, 06/26/2011
			hassame = (AosIILUtil::valueMatch(subiil->getFirstValue(), eAosOpr_eq, mValues[idx], mIsNumAlpha));
			// hassame = (value == subiil->getFirstValue());
		}
	}
	if(hassame)
	{
		isunique = false;
	}

	return true;
}


//change name form firstDoc1
bool
AosIILStr::getFirstDocSinglePriv(
		int &idx, 
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
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

	case eAosOpr_like:
		 // Ken, 2011/5/25
		 return nextDocidLike(idx, reverse, value, docid);

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 docid = AOS_INVDID;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}

*/
