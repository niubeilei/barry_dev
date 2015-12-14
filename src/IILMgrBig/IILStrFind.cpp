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


bool		
AosIILStr::nextDocidPrivFind(
		AosIILIdx &idx,
		const OmnString &value,
		const AosOpr opr,
		u64 &docid, 
		bool &isunique,
		const bool reverse, 
		const AosRundataPtr &rdata) 
{
	// It searches the first docid that meets the condition:
	// 		mValues[i] opr value
	//
	// If 'idx' is not a new search, it starts from 'idx' next position. 
	// Otherwise, it searches starting from the beginning. 
	//
	// If an entry is found, 'docid' is set to the corresponding docid
	// and 'idx' points to it.
	//
	// Returns:
	// If any error occurred, it returns false. Otherwise, it returns
	// true, 'docid' is set to the one found, and 'iilidx' is set
	// to the iil in which the doc was found, if it is in a subiil.
	//
	// Performance Analysis:
	// This should be the same as the previous one.
	//
	// If this instance is a root iil, 'rootiil' is null. Otherwise, 
	// it is a subiil and it should be called from its root iil. 
	// 'rootiil' should not be null.
	
	// Check whether it is a root iil and it is segmented
	docid = AOS_INVDID;
	if (isLeafIIL())
	{
		int index = idx.getIdx(mLevel);
		bool rslt = nextDocidSinglePriv(index, reverse, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		
		rslt = idx.setIdx(mLevel, index);
		aos_assert_r(rslt, false);
		
		return rslt;
	}
	
	AosIILStrPtr subiil = getSubiilByCondPriv(idx, value, opr, reverse, rdata);
	if(!subiil)
	{
		// This means there are no entries that satisfies the condition.
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}

	bool rslt;
	while (subiil)
	{
		aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, rdata, false);
		rslt = subiil->nextDocidSafeFind(idx, value, opr, docid, isunique, reverse, rdata);
		aos_assert_rr(rslt, rdata, false);

		if (idx.isValid(mLevel)) return true;
		if (!hasMoreMembers(idx))
		{
			idx.setInvalid(mLevel);
			return true;
		}
		// Find the next subiil.
		subiil = getSubiilByCondPriv(idx, value, opr, reverse, rdata);
	}
	idx.setInvalid(mLevel);
	return true;
}


AosIILStrPtr
AosIILStr::getSubiilByCondPriv(
		AosIILIdx &idx,
		const OmnString &value,    
		const AosOpr opr,
		const bool reverse,        
		const AosRundataPtr &rdata)
{
	if(idx.isStart(mLevel))
	{
		return getFirstSubiilByCondPriv(idx, value, opr, reverse, rdata);
	}
	return nextSubiilByCondPriv(idx, value, opr, reverse, rdata);
}


AosIILStrPtr
AosIILStr::getSubiilByCondPriv(
		AosIILIdx &idx,
		const OmnString &value,    
		const u64 &docid,
		const AosOpr opr,
		const bool reverse,        
		const AosRundataPtr &rdata)
{
	if(idx.isStart(mLevel))
	{
		return getFirstSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
	}
	return nextSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
}


AosIILStrPtr
AosIILStr::getFirstSubiilByCondPriv(
		AosIILIdx &idx, 
		const OmnString &value, 
		const AosOpr opr, 
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the first index (or last index if 'reverse' is true):
	//			[mMinVals[index], mMinDocids[index]] opr [value, docid]
	// If not found, it returns 0.
	idx.setStart(mLevel);
	aos_assert_rr(!isLeafIIL(), rdata, 0);
	aos_assert_rr((int)mNumSubiils > 0, rdata, 0);
	
	int index = reverse ? (int)mNumSubiils-1 : 0;
	int rsltInt;
	if (reverse)
	{
		switch (opr)
		{
		case eAosOpr_gt :	// >
			 rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
			 if(rsltInt <= 0) index = -5;
			 break;

		case eAosOpr_ge:	// >=
			 rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
			 if(rsltInt < 0) index = -5;
			 break;

		case eAosOpr_lt :	// <
			 index = getLastSubiilIndexLT(value, rdata);
			 break;
			 
		case eAosOpr_le :	// <=
			 index = getLastSubiilIndexLE(value, rdata);
			 break;

		case eAosOpr_eq :	// ==
			 index = getLastSubiilIndexLE(value, rdata);
			 break;

		case eAosOpr_ne :	// !=
			 rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
			 if(rsltInt == 0) 
			 {
			 	index = getLastSubiilIndexLT(value, rdata);
			 }
			 break;

		case eAosOpr_an :	// Matching any
			 break;
			 
		case eAosOpr_prefix :	// Prefix
			 index = getLastSubiilIndexPrefix(value, rdata);
			 break;

		case eAosOpr_like : // like
			 break;

		default :	  
			 OmnAlarm << "opr error, " << opr << enderr;
			 return 0;
		}
	}
	else
	{
		switch (opr)
		{
		case eAosOpr_gt :	// >
			 index = getFirstSubiilIndexGT(value, rdata);
			 break;

		case eAosOpr_ge:	// >=
			 index = getFirstSubiilIndexGE(value, rdata);
			 break;

		case eAosOpr_lt :	// <
			 rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
			 if(rsltInt >= 0) index = -5;
			 break;

		case eAosOpr_le :	// <=
			 rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
			 if(rsltInt > 0) index = -5;
			 break;

		case eAosOpr_eq :	// ==
			 index = getFirstSubiilIndexGE(value, rdata);
			 break;

		case eAosOpr_ne :	// !=
			 rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
			 if(rsltInt == 0) 
			 {
			 	index = getFirstSubiilIndexGT(value, rdata);
			 }
			 break;

		case eAosOpr_an :	// Matching any
			 break;

		case eAosOpr_prefix :	// Prefix
			 index = getFirstSubiilIndexGE(value, rdata);
			 break;			 

		case eAosOpr_like : // like
			 break;

		default :
			 OmnAlarm << "opr error, " << opr << enderr;
			 return 0;
		}
	}

	if(index < 0 || index >= (int)mNumSubiils)
	{
		idx.setInvalid(mLevel);	 
		return 0;
	}
	
	bool rslt = idx.setIdx(mLevel, index);
	aos_assert_r(rslt, 0);

	return getSubiilByIndexPriv(index, false, rdata);
}


AosIILStrPtr
AosIILStr::getFirstSubiilByCondPriv(
		AosIILIdx &idx, 
		const OmnString &value, 
		const u64 &docid,
		const AosOpr opr, 
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the first index (or last index if 'reverse' is true):
	//			[mMinVals[index], mMinDocids[index]] opr [value, docid]
	// If not found, it returns 0.
	idx.setStart(mLevel);
	aos_assert_rr(!isLeafIIL(), rdata, 0);
	aos_assert_rr((int)mNumSubiils > 0, rdata, 0);
	
	int index = reverse ? mNumSubiils-1 : 0;
	bool rslt;
	if (reverse)
	{
		switch (opr)
		{
		case eAosOpr_gt :	// >
			 rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_gt, value, docid, mIsNumAlpha);
			 if(!rslt) index = -5;
			 break;

		case eAosOpr_ge:	// >=
			 rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_ge, value, docid, mIsNumAlpha);
			 if(!rslt) index = -5;
			 break;

		case eAosOpr_lt :	// <
			 index = getLastSubiilIndexLT(value, docid, rdata);
			 break;
			 
		case eAosOpr_le :	// <=
			 index = getLastSubiilIndexLE(value, docid, rdata);
			 break;

		case eAosOpr_eq :	// ==
			 index = getLastSubiilIndexLE(value, docid, rdata);
			 break;

		case eAosOpr_ne :	// !=
			 rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_ne, value, docid, mIsNumAlpha);
			 if(!rslt) 
			 {
			 	index = getLastSubiilIndexLT(value, docid, rdata);
			 }
			 break;

		case eAosOpr_an :	// Matching any
			 break;
			 
		case eAosOpr_prefix :	// Prefix
			 index = getLastSubiilIndexPrefix(value, docid, rdata);
			 break;

		case eAosOpr_like : // like
			 break;

		default :	  
			 OmnAlarm << "opr error, " << opr << enderr;
			 return 0;
		}
	}
	else
	{
		switch (opr)
		{
		case eAosOpr_gt :	// >
			 index = getFirstSubiilIndexGT(value, docid, rdata);
			 break;

		case eAosOpr_ge:	// >=
			 index = getFirstSubiilIndexGE(value, docid, rdata);
			 break;

		case eAosOpr_lt :	// <
			 rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_lt, value, docid, mIsNumAlpha);
			 if(!rslt) index = -5;
			 break;

		case eAosOpr_le :	// <=
			 rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_le, value, docid, mIsNumAlpha);
			 if(!rslt) index = -5;
			 break;

		case eAosOpr_eq :	// ==
			 index = getFirstSubiilIndexGE(value, docid, rdata);
			 break;

		case eAosOpr_ne :	// !=
			 rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_ne, value, docid, mIsNumAlpha);
			 if(!rslt) 
			 {
			 	index = getFirstSubiilIndexGT(value, docid, rdata);
			 }
			 break;

		case eAosOpr_an :	// Matching any
			 break;

		case eAosOpr_prefix :	// Prefix
			 index = getFirstSubiilIndexGE(value, docid, rdata);
			 break;			 

		case eAosOpr_like : // like
			 break;

		default :
			 OmnAlarm << "opr error, " << opr << enderr;
			 return 0;
		}
	}

	if(index < 0 || index >= (int)mNumSubiils)
	{
		idx.setInvalid(mLevel);	 
		return 0;
	}
	
	rslt = idx.setIdx(mLevel, index);
	aos_assert_r(rslt, 0);
	
	return getSubiilByIndexPriv(index, false, rdata);
}
		
			 
int
AosIILStr::getFirstSubiilIndexGT(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// opr >
	int rsltInt = AosIILUtil::valueMatch(mMaxVals[0], value, mIsNumAlpha);
	if(rsltInt > 0) return 0;

	rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
	if(rsltInt <= 0) return -5;
	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while(left <= right)
	{
		if(left == right) return left;
		
		idx = (right + left) >> 1;
		rsltInt = AosIILUtil::valueMatch(mMaxVals[idx], value, mIsNumAlpha);
		if(rsltInt <= 0)
		{
			left = idx + 1;			
		}
		else
		{
			right = idx;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getFirstSubiilIndexGT(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// opr >
	bool rslt = AosIILUtil::valueMatch(mMaxVals[0], mMaxDocids[0], eAosOpr_gt, value, docid, mIsNumAlpha);
	if(rslt) return 0;
		
	rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_gt, value, docid, mIsNumAlpha);
	if(!rslt) return -5;
	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while(left <= right)
	{
		if(left == right) return left;
		
		idx = (right + left) >> 1;
		rslt = AosIILUtil::valueMatch(mMaxVals[idx], mMaxDocids[idx], eAosOpr_gt, value, docid, mIsNumAlpha);
		if(rslt)
		{
			right = idx;
		}
		else
		{
			left = idx + 1;			
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getFirstSubiilIndexGE(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// opr >=
	int rsltInt = AosIILUtil::valueMatch(mMaxVals[0], value, mIsNumAlpha);
	if(rsltInt >= 0) return 0;
	
	rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
	if(rsltInt < 0) return -5;
			 	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while(left <= right)
	{
		if(left == right) return left;
		
		idx = (right + left) >> 1;
		rsltInt = AosIILUtil::valueMatch(mMaxVals[idx], value, mIsNumAlpha);
		if(rsltInt < 0)
		{
			left = idx + 1;			
		}
		else
		{
			right = idx;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getFirstSubiilIndexGE(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// opr >=
	bool rslt = AosIILUtil::valueMatch(mMaxVals[0], mMaxDocids[0], eAosOpr_ge, value, docid, mIsNumAlpha);
	if(rslt) return 0;
		
	rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_ge, value, docid, mIsNumAlpha);
	if(!rslt) return -5;
	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while(left <= right)
	{
		if(left == right) return left;
		
		idx = (right + left) >> 1;
		rslt = AosIILUtil::valueMatch(mMaxVals[idx], mMaxDocids[idx], eAosOpr_ge, value, docid, mIsNumAlpha);
		if(rslt)
		{
			right = idx;
		}
		else
		{
			left = idx + 1;			
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getLastSubiilIndexLT(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	//opr <
	aos_assert_rr(mNumSubiils > 0, rdata, -5);
	int rsltInt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], value, mIsNumAlpha);
	if(rsltInt < 0) return mNumSubiils-1;
	
	rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
	if(rsltInt >= 0) return -5;
	
	// When it comes to this point, index=0 satisifies the condition
	// and index = mNumSubiils-1 does not. 
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while (left <= right)
	{
		if (left == right) return right;
		
		idx = (right + left + 1) >> 1;
    	rsltInt = AosIILUtil::valueMatch(mMinVals[idx], value, mIsNumAlpha);
		if (rsltInt < 0)
		{
			left = idx;
		}
		else
		{
			right = idx - 1;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getLastSubiilIndexLT(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//opr <
	bool rslt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], mMinDocids[mNumSubiils-1], eAosOpr_lt, value, docid, mIsNumAlpha);
	if (rslt) return mNumSubiils-1;
    
	rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_lt, value, docid, mIsNumAlpha);
	if(!rslt) return -5;
	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while (left <= right)
	{
		if (left == right) return right;
		
		idx = (right + left + 1) >> 1;
    	rslt = AosIILUtil::valueMatch(mMinVals[idx], mMinDocids[idx], eAosOpr_lt, value, docid, mIsNumAlpha);
		if (rslt)
		{
			left = idx;
		}
		else
		{
			right = idx - 1;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getLastSubiilIndexLE(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	//opr <=
	int rsltInt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], value, mIsNumAlpha);
	if(rsltInt <= 0) return mNumSubiils-1;
	
	rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
	if(rsltInt > 0) return -5;
	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while (left <= right)
	{
		if (left == right) return right;
		
		idx = (right + left + 1) >> 1;
    	rsltInt = AosIILUtil::valueMatch(mMinVals[idx], value, mIsNumAlpha);
		if (rsltInt <= 0)
		{
			left = idx;
		}
		else
		{
			right = idx - 1;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getLastSubiilIndexLE(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//opr <
	bool rslt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], mMinDocids[mNumSubiils-1], eAosOpr_le, value, docid, mIsNumAlpha);
	if (rslt) return mNumSubiils-1;
    
	rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_le, value, docid, mIsNumAlpha);
	if(!rslt) return -5;
	
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while (left <= right)
	{
		if (left == right) return right;
		
		idx = (right + left + 1) >> 1;
    	rslt = AosIILUtil::valueMatch(mMinVals[idx], mMinDocids[idx], eAosOpr_le, value, docid, mIsNumAlpha);
		if (rslt)
		{
			left = idx;
		}
		else
		{
			right = idx - 1;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getLastSubiilIndexPrefix(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(!mIsNumAlpha, rdata, -5);
	int left = 0;
	int right = mNumSubiils - 1;
	int idx;
	while (left <= right)
	{
		if (left == right)
		{
			int rslt1 = strncmp(mMinVals[left], value.data(), value.length());
			int rslt2 = strncmp(mMaxVals[left], value.data(), value.length());
			if (rslt1 <= 0 && rslt2 >= 0) return left;
			return -5;
		}
		
		idx = (right + left + 1) >> 1;
		int rslt = strncmp(mMinVals[idx], value.data(), value.length());
		if (rslt <= 0)
		{
			left = idx;
		}
		else
		{
			right = idx-1;
		}
	}
	OmnShouldNeverComeHere;
	return -5;
}


int
AosIILStr::getLastSubiilIndexPrefix(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return getLastSubiilIndexPrefix(value, rdata);
}


AosIILStrPtr
AosIILStr::nextSubiilByCondPriv(
		AosIILIdx &idx, 
		const OmnString &value, 
		const AosOpr opr, 
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function searches the next subiil starting from 'idx'
	// that satisifies the condition [value, opr]. For instance, 
	// if 'opr' is eq, it searches the next subiil that may contain
	// the values that are the same as 'value'.
	idx.next(mLevel, reverse);
	while (hasMoreMembers(idx))
	{
		bool keep_search;
		int iilidx = idx.getIdx(mLevel);
		if (containValuePriv(mMinVals[iilidx], mMaxVals[iilidx], 
					value, opr, reverse, keep_search, rdata))
		{
			AosIILStrPtr subiil = getSubiilByIndexPriv(iilidx, false, rdata);
			aos_assert_rr(subiil, rdata, 0);
			return subiil;
		}
		if (!keep_search) return 0;
		idx.next(mLevel, reverse);
	}
	return 0;
}


AosIILStrPtr
AosIILStr::nextSubiilByCondPriv(
		AosIILIdx &idx, 
		const OmnString &value, 
		const u64 &docid,
		const AosOpr opr, 
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// This function searches the next subiil starting from 'idx'
	// that satisifies the condition [value, opr]. For instance, 
	// if 'opr' is eq, it searches the next subiil that may contain
	// the values that are the same as 'value'.
	idx.next(mLevel, reverse);
	while (hasMoreMembers(idx))
	{
		bool keep_search;
		int iilidx = idx.getIdx(mLevel);
		if (containValuePriv(mMinVals[iilidx], mMaxVals[iilidx], 
					value, opr, reverse, keep_search, rdata))
		{
			AosIILStrPtr subiil = getSubiilByIndexPriv(iilidx, false, rdata);
			aos_assert_rr(subiil, rdata, 0);
			return subiil;
		}
		if (!keep_search) return 0;
		idx.next(mLevel, reverse);
	}
	return 0;
}


AosIILStrPtr
AosIILStr::getSubiilByIndexPriv(
		const int idx, 
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	// It retrieves the iilidx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil. 
	//
	// The function assumes 'mLock' should have been locked.
	// Chen Ding, 12/14/2010
	// aos_assert_r(mIILIdx == 0, 0);
	aos_assert_rr(!isLeafIIL(), rdata, 0);
	
	int iilidx = idx;
	if(idx == -10) iilidx = 0;

	aos_assert_rr(iilidx >= 0 && (u32)iilidx < mNumSubiils, rdata, 0);
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILStrPtr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[iilidx]) return mSubiils[iilidx];

	AosIILType type= eAosIILType_Str;
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
		mIILIds[iilidx], mSiteid, type, false, iilmgrlock, rdata);
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[iilidx] << ":" << type << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		OmnAlarm << "Not a string IIL: " 
			<< iil->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, iilmgrlock, rdata);
		return 0;
	}
	mSubiils[iilidx] = (AosIILStr*)iil.getPtr();
	mSubiils[iilidx]->mParentIIL = this;
	mSubiils[iilidx]->mIILIdx = iilidx;
	return mSubiils[iilidx];
}


AosIILStrPtr
AosIILStr::getSubiilPriv(
		const OmnString &value, 
		const u64 &docid,
		const bool value_unique, 
		const bool docid_unique,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
{
	// This function finds the subiil in which the entry [value, docid]
	// may be inserted. This is normally used by adding an entry. 
	// This function shall be called by non-leaf IILs.
	//
	// 1. If 'value' is not in boundary, it returns the iil in which the
	//    entry [value, docid] may reside. 
	// 2. If 'value' is at boundary:
	// 	  a. If docid == 0, return.
	// 	  b. If docid != 0, if 
	// 2. If 'value_unique' is true, the function checks whether the value
	//    already exists. If yes, it returns 0. 
	// 2. If 'docid_unique' is true, the function checks whether [value, docid] 
	//    exists. If yes, it returns 0. 
	// 3. Otherwise, it returns the subiil in which the entry [value, docid]
	//    may resides. If it is to add, this is the subiil to add.
	aos_assert_r(!isLeafIIL(), 0);
	aos_assert_r(value != "", 0);
	
	int idx = getSubiilIndex3Priv(value, docid, rdata);
	aos_assert_r(idx >= 0 && (u32)idx < mNumSubiils, 0);
				
	int rsltInt;
	if(value_unique)
	{	
		rsltInt = AosIILUtil::valueMatch(mMaxVals[idx], value, mIsNumAlpha);
		if(rsltInt == 0)
		{
			rdata->setError()<< "Value already exist: " << value << ":" << docid << ":" << mDocids[idx];
			OmnAlarm << rdata->getErrmsg()<< enderr;
			return 0;	
		}
		if(rsltInt < 0)
		{
			aos_assert_r((u32)idx == mNumSubiils - 1, 0);
		}

		rsltInt = AosIILUtil::valueMatch(mMinVals[idx], value, mIsNumAlpha);
		if(rsltInt == 0)
		{
			rdata->setError()<< "Value already exist: " << value << ":" << docid << ":" << mDocids[idx];
			OmnAlarm << rdata->getErrmsg()<< enderr;
			return 0;	
		}
		if(rsltInt > 0 && idx > 0)
		{
			rsltInt = AosIILUtil::valueMatch(mMaxVals[idx - 1], value, mIsNumAlpha);
			aos_assert_r(rsltInt <= 0, 0);
			if(rsltInt == 0)
			{
				rdata->setError()<< "Value already exist: " << value << ":" << docid << ":" << mDocids[idx];
				OmnAlarm << rdata->getErrmsg()<< enderr;
				return 0;	
			}
		}
	}
	
	AosIILStrPtr subiil = getSubiilByIndexPriv(idx, false, rdata);
	aos_assert_rr(subiil, rdata, 0);
	aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, rdata, 0);
	return subiil;
}


bool
AosIILStr::nextUniqueValuePriv(
		AosIILIdx &idx,
		const bool reverse,
		const AosOpr opr,
		const OmnString &value,
		OmnString &unique_value,
		bool &found,
		const AosRundataPtr &rdata)
{
	// This function retrieves the next doc that meets the following conditions:
	// 	1. [opr, value]
	// 	2. The value is different from the previous value
	// The search can be either a new search or the next search. If it is a 
	// new search, it should use binary search. If it is the next search,
	// it searches the next.
	//
	// New Search
	// It uses the binary search to find the first occurrence that satisifies
	// the condition.
	//
	// Next Search
	// It retrieves the current subiil and calls the corresponding member function
	// on the subiil. If found, it returns. Otherwise, it advances its subiil
	// index. If there is no more subiil, it returns 0. Otherwise, it resets 
	// its member index to 'start', and then calls the corresponding member function.
	found = false;
	if (isLeafIIL())
	{
		return nextUniqueValueSinglePriv(idx, reverse, opr, value, unique_value, found, rdata);
	}

	// It is non-leaf. It searches the first subiil that satisfies the
	// condition. 
	AosIILStrPtr subiil;
	if (idx.isStart(mLevel))
	{
		subiil = getFirstSubiilByCondPriv(idx, value, 0, opr, reverse, rdata);
		if (!subiil)
		{
			found = false;
			return true;
		}
	}
	else
	{
		// This is to search the next. 'next' means it starts the current
		// subiil and find its next member. If found, it returns (i.e., 
		// the subiil is not changed, only its member). Otherwise, it 
		// checks the next subiil.
		if (!isIILIdxValid(idx))
		{
			// This means that its subiil index is out of bound,
			// which means that there are no more subiils.
			found = false;
			return true;
		}

		int iilidx = idx.getIdx(mLevel);
		AosIILStrPtr subiil = getSubiilByIndexPriv(iilidx, false, rdata);
	}
	aos_assert_rr(subiil, rdata, false);

	while (subiil)
	{
		aos_assert_rr(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, rdata, false);
		bool rslt = subiil->nextUniqueValueSafe(idx, reverse, 
					opr, value, unique_value, found, rdata);
		aos_assert_rr(rslt, rdata, false);
		if (found) return true;
			
		// There is no more members for 'subiil'. Need to check the next
		// subiil.
		if (!hasMoreMembers(idx))
		{
			// There are no more subiils. 
			found = false;
			return true;
		}

		// Find the next subiil.
		subiil = nextSubiilByCondPriv(idx, value, opr, reverse, rdata);
		aos_assert_rr(rslt, rdata, false);
	}

	found = false;
	return true;
}


AosIILStrPtr
AosIILStr::getSubiil3Priv(
		const OmnString &value,
		const u64 &docid,
		const bool iilmgrlock,
		const AosRundataPtr &rdata)
{
	int index = getSubiilIndex3Priv(value,docid,rdata);
	aos_assert_r((index >= 0) && (index <= (int)mNumSubiils-1), 0);
	return getSubiilByIndexPriv(index,iilmgrlock,rdata);
}


int
AosIILStr::getSubiilIndex3Priv(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	// Or it is to find the first index that:
	// 		[value, docid] <= [mMaxVals[index], mMaxDocids[index]]
	//
	aos_assert_r(mSubiils > 0, 0);
    int left = 0;
    int right = mNumSubiils - 1;
    int cur = 0;
    if (right == left) return right;
	while(1)
    {
		if (right == left)
		{
			return left;
		}

        if (right == left +1)
        {
			// If the left satisfies the condition, return left.
			// Otherwise, return right.
            if (AosIILUtil::valueMatch(mMaxVals[left], mMaxDocids[left], 
						eAosOpr_ge, value, docid, mIsNumAlpha))
            {
                return left;
            }
            else
            {
                return right;
            }
        }

        cur = (right - left)/2 + left;

        if (AosIILUtil::valueMatch(mMaxVals[cur], mMaxDocids[cur], 
					eAosOpr_ge, value, docid, mIsNumAlpha))
        {
			// Found a match: 
			// 		[value, docid] <= [mMaxVals[cur], mMaxDocids[cur]]
			// Move left. 
			right = cur;
		}
		else
		{
			// Not a match:
			// 		[value, docid] > [mMaxVals[cur], mMaxDocids[cur]]
			// Move right. 
			left = cur;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}

int
AosIILStr::getSubiilIndex3Priv(
		const OmnString &value,
		const u64 &docid,
		const AosOpr opr,
		const AosRundataPtr &rdata)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	// Or it is to find the first index that:
	// 		[value, docid] <= [mMaxVals[index], mMaxDocids[index]]
	//
	aos_assert_r(mSubiils > 0, 0);
    int left = 0;
    int right = mNumSubiils - 1;
    int cur = 0;

	// There has to be at least two subiils
	aos_assert_rr(left < right, rdata, 0);

	// bool found = false;		// Chen Ding, 05/22/2012, Not used.
	while(1)
    {
		if (right == left)
		{
			return left;
		}

        if (right == left +1)
        {
			// If the left satisfies the condition, return left.
			// Otherwise, return right.
            if (AosIILUtil::valueMatch(mMaxVals[left], mMaxDocids[left], 
						eAosOpr_ge, value, docid, mIsNumAlpha))
            {
                return left;
            }
            else
            {
                return right;
            }
        }

        cur = (right - left)/2 + left;

        if (AosIILUtil::valueMatch(mMaxVals[cur], mMaxDocids[cur], 
					opr, value, docid, mIsNumAlpha))
        {
			// Found a match: 
			// 		[value, docid] opr [mMaxVals[cur], mMaxDocids[cur]]
			// Move left. 
			right = cur;
			// found = true;		// Chen Ding, 05/22/2012, not used
		}
		else
		{
			// Not a match:
			// 		[value, docid] > [mMaxVals[cur], mMaxDocids[cur]]
			// Move right. 
			left = cur;
		}
	}

	OmnShouldNeverComeHere;
	return -1;
}


/* Chen Ding, 04/24/2012
 * Backup of Shawn's function
int
AosIILStr::getSubiilIndex3(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	// Or it is to find the first index that:
	// 		[value, docid] <= [mMaxVals[index], mMaxDocids[index]]
	//
	aos_assert_r(mSubiils > 0, 0);
    int left = 0;
    int right = mNumSubiils - 1;
    int cur = 0;
    if (right == left) return right;
    bool leftnotfit = false;
	while(1)
    {
		// BUG: If there is only one subiil, it is a dead loop.
        if (right == left +1)
        {
			// BUG: This means that if there are two subiils, it always return
			// the second one. 
			if (leftnotfit) return right;
            if (AosIILUtil::valueMatch(mMaxVals[left], mMaxDocids[left], 
						eAosOpr_ge, value, docid, mIsNumAlpha))
            {
                return left;
            }
            else
            {
                return right;
            }
        }

        cur = (right - left)/2 + left;

        if (AosIILUtil::valueMatch(mMaxVals[cur], mMaxDocids[cur], 
					eAosOpr_ge, value, docid, mIsNumAlpha))
        {
			// The entry [mMaxVals[cur], mMaxDocids[cur]] is >= [value, docid]
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
*/


/* Chen Ding Modifications, 04/22/2012
 * Shall not use this function anymore.
AosIILStrPtr
AosIILStr::getSubiil(
		const bool reverse,
		const AosOpr opr1,
		const OmnString &value1, 
		const AosOpr opr2,
		const OmnString &value2,
		const AosRundataPtr &rdata) 
{
//shawn comment out
	// It searches the index to determine the first subiil
	// that makes [opr, value] true. If not found, return 0.
	// Otherwise, it retrieves the IIL and 'iilidx' is set
	// to the sub-iil index.
	//
	// As an example, if we want to search:
	// 	['value1' <= mValues <= 'value2']
	// in the reverse order, start from the last sub-iil,
	// find the one that holds:
	// 		mMinVals[i] < value2
	//
	// In the normal order, starting from the first sub-iil, 
	// find the first one that holds:
	// 		value1 <= mMaxVals[i]
	if (reverse)
	{
		// Search the index in the reversed order
		for (int i=mNumSubiils-1; i>=0; i--)
		{
			if (mNumEntries[i]>0 && AosIILUtil::valueMatch(mMinVals[i], opr2, value2, mIsNumAlpha))
			{
				// Found the sub-iil
				return getSubiilByIndex(i, false, rdata);
			}
		}
		
		// Not found
		return 0;
	}

	// It is normal order
	for (u32 i=0; i<mNumSubiils; i++)
	{
		if ( mNumEntries[i]>0 && AosIILUtil::valueMatch(value1, opr1, mMaxVals[i], mIsNumAlpha))
		{
			// Found the sub-iil
			return getSubiilByIndex(i, false, rdata);
		}
	}
	return 0;
}


// Chen Ding, 12/12/2010
// This function searches for the first subiil that 
// passes the condition [value, opr]. If 'startidx'
// is -10, it starts from the beginning. Otherwise, 
// it is the subiil from which the search starts. 
//
// Function name changed from 'getSubIILIndex(startidx, value, opr, fromHead)'
int
AosIILStr::getSubiilByCondSinglePriv(
		AosIILIdx &idx, 
		const OmnString &value,
		const AosOpr opr,
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	// shawn linear search -> binary search, remove mMinVals
	if (fromHead)
	{
		// Chen Ding, 12/12/2010
		int idx = startidx;
		if (startidx < 0)
		{
			if (startidx == -10)
			{
				idx = 0;
			}
			else
			{
				// This is incorrect
				OmnAlarm << "startidx incorrect: " << startidx << enderr;
				return -5;
			}
		}

		if (idx < 0)
		{
			// Should never happen
			OmnAlarm << "idx: " << idx << enderr;
			return -5;
		}

		aos_assert_r(mNumSubiils > 0,0);
		if (idx >= (int)mNumSubiils)
		{
			return -5;
		}
		// End of Chen Ding, 12/12/2010
		// Normal order searching
		switch (opr)
		{
		case eAosOpr_gt:
			 // Find the first one whose value < max. If it runs out of the
			 // loop, there is no subiil for it.
			 // for(int i=0; i<mNumSubiils; i++)// Chen Ding, 12/12/2010
			 for(u32 i=idx; i<mNumSubiils; i++)	// Chen Ding, 12/12/2010
			 {
			  	 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_gt, value, mIsNumAlpha)) return i;
			 }
			 return -5;

		case eAosOpr_ge:
			 // Find the first one whose value <= max. If it runs out of 
			 // the loop, there is no subiil for it.
			 for(u32 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, mIsNumAlpha)) return i;
			 }
			 return -5;
				 
		case eAosOpr_eq:
			 // Find the first one that max >= value. If it runs out of
			 // the loop, there is no subiil for it.
			 // for(int i=0; i<mNumSubiils; i++)	// Chen Ding, 12/12/2010
			 for(u32 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, mIsNumAlpha))
				 {
					 // If value < min, not possible
					 if (AosIILUtil::valueMatch(value, eAosOpr_lt, mMinVals[i], mIsNumAlpha)) return -5;
					 return i;
				 }
			 }
			 return -5;

		case eAosOpr_lt: 
			 // If the first min < value, return 0. Otherwise, return -5.
			 if (AosIILUtil::valueMatch(mMinVals[idx], eAosOpr_lt, value, mIsNumAlpha)) return idx;
			 return -5;
			
		case eAosOpr_le:
			 // If the first min <= value, return 0. Otherwise, return -5.
			 if (AosIILUtil::valueMatch(mMinVals[idx], eAosOpr_le, value, mIsNumAlpha)) return idx;
			 return -5;
	
		case eAosOpr_ne:
			 // Chen Ding, 12/12/2010
			 // if (AosIILUtil::valueMatch(mMinVals[0], eAosOpr_ne, value, mIsNumAlpha)) return 0;
			 for(u32 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_ne, value, mIsNumAlpha)) return i;
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ne, value, mIsNumAlpha)) return i;
			 }
			 // for (int i=0; i < mNumSubiils; i++)
			 // {
			 // 	if (AosIILUtil::valueMatch(mMaxVals[i],eAosOpr_ne,mMinVals[i], mIsNumAlpha)) return 0;
			 // 	if (AosIILUtil::valueMatch(mMaxVals[i],eAosOpr_ne,value), mIsNumAlpha) return 0;
			 // }
			 return -5;

		case eAosOpr_an:
			 // return 0;
			 return idx;

		case eAosOpr_like:
			 // Ken, 2011/5/25
			 //for(int i=idx; i<mNumSubiils; i++)
			 //{
			//	 AosIILStrPtr subiil = getSubiil(i);
			//	 aos_assert_r(subiil, -5);
			//	 
			//	 if (subiil->valueLikeMatch(value, false))
			//		 return i;
			// }
			// return -5;
			 return idx;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 return -5;
		}

		OmnShouldNeverComeHere;
		return -5;
	}

	// Chen Ding, 12/12/2010
	int idx = startidx;
	if (startidx < 0)
	{
		if (startidx == -10)
		{
			idx = mNumSubiils-1;
		}
		else
		{
			// This is incorrect
			OmnAlarm << "startidx incorrect: " << startidx << enderr;
			return -5;
		}
	}

	if (idx < 0)
	{
		// It could be the case that the caller keeps on looping
		// on the subiil (in the decreasing order) so that 
		// 'idx' becomes -1, which means that there are no
		// more subiils to search
		if (idx == -1)
		{
			return -5;
		}

		// Should never come here
		OmnAlarm << "idx: " << idx << enderr;
		return -5;
	}

	aos_assert_r(mNumSubiils > 0,false);
	if (idx >= (int)mNumSubiils)
	{
		// Should never happen
		OmnAlarm << "idx: " << idx << enderr;
		return -5;
	}
	// When it comes to this point, idx is in [0, mNumSubiils-1]
	// End of Chen Ding, 12/12/2010

	// Search in the reversed order	
	switch (opr)
	{
	case eAosOpr_gt:
		 // eAosOpr_gt: as long as the value is no less than the
		 // last max, which means that there are no values that are
		 // greater than 'value', return -5. Otherwise, return the
		 // last index.
		 // Chen Ding, 12/12/2010
		 // if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_le, value, mIsNumAlpha)) return -5;
		 // return mNumSubiils-1;
		 if (AosIILUtil::valueMatch(mMaxVals[idx], eAosOpr_le, value, mIsNumAlpha)) return -5;
		 return idx;

	case eAosOpr_ge:
		 // Chen Ding, 12/12/2010
		 // if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_lt, value, mIsNumAlpha)) return -5;
		 // return mNumSubiils-1;
		 // Chen Ding, Bug1217
		 if (AosIILUtil::valueMatch(mMaxVals[idx], eAosOpr_lt, value, mIsNumAlpha)) return -5;
		 return idx;

	case eAosOpr_eq:
		 // Chen Ding, 12/12/2010
		 // for (int i=mNumSubiils-1; i>=0; i--)
		 for (int i=idx; i>=0; i--)
		 {
			 // Find the first one whose min <= value. It then 
			 // checks whether max >= value, return that one. 
			 if(AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value, mIsNumAlpha))
			 {
				 if(AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, mIsNumAlpha)) return i;
				 return -5;
			 }
		 }
		 return -5;

	case eAosOpr_lt:
		 // Chen Ding, 12/12/2010
		 // for (int i=mNumSubiils-1; i>=0; i--)
		 for (int i=idx; i>=0; i--)
		 {
			// Find the first one whose min < 'value'. 
			if(AosIILUtil::valueMatch(mMinVals[i], eAosOpr_lt, value, mIsNumAlpha)) return i;
		 }
		 return -5;

	case eAosOpr_le:
		 // Chen Ding, 12/12/2010
		 // for (int i=mNumSubiils-1; i>=0; i--)
		 for (int i=idx; i>=0; i--)
		 {
		  	 if(AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value, mIsNumAlpha)) return i;
		 }
		 return -5;

	case eAosOpr_ne:
		 // Chen Ding, 12/12/2010
		 // if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_ne, value, mIsNumAlpha)) return mNumSubiils-1;
		 for(int i=idx; i>= 0; i--)		// Chen Ding, 12/12/2010
		 {
			 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_ne, value, mIsNumAlpha)) return i;
			 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ne, value, mIsNumAlpha)) return i;
		 }
		 //if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_ne, value, mIsNumAlpha)) return 0;
		 // for (int i= mNumSubiils-1; i >=0 ;i--)
		 // {
		// 	 if (AosIILUtil::valueMatch(mMaxVals[i],eAosOpr_ne,mMinVals[i], mIsNumAlpha)) return 0;
		// 	 if (AosIILUtil::valueMatch(mMaxVals[i],eAosOpr_ne,value, mIsNumAlpha)) return 0;
		 // }
		 return -5;

	case eAosOpr_an:
		 // Chen Ding, 12/12/2010
		 // return mNumSubiils-1;
		 return idx;

	case eAosOpr_like:
		 //ken 2011/5/25
		 //for(int i=idx; i>=0; i--)
		 //{
		//	 AosIILStrPtr subiil = getSubiil(i);
		//	 aos_assert_r(subiil, -5);
		//		 
		//	 if (subiil->valueLikeMatch(value, true))
		//	 	return i;
		// }/
		// return -5;
		 return idx;

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 return -5;
	}

	OmnShouldNeverComeHere;
	return -5;
}


bool
AosIILStr::locateIdx(
		int &cur_idx,
		int &cur_iilidx,
		const OmnString &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)	
{
	if (reverse)
	{
		prevQueryPos(cur_idx, cur_iilidx);
	}
	else
	{
		nextQueryPos(cur_idx, cur_iilidx);
	}

	if (reverse)
	{
		if (cur_iilidx < 0)
		{
			has_data = false;
			return true;
		}

		if (isRootIIL() && cur_iilidx > (int)mNumSubiils -1)
		{
			cur_iilidx = mNumSubiils -1;
		}

		//1. get the first doc which value == cur_value && docid < cur_docid
		//2. if not found, get the first doc which value < cur_value
		//3. if not found, set has_data == false and return
		if (cur_idx < 0) cur_idx = 0;		
	}
	else // Normal order
	{
		if(isRootIIL() && cur_iilidx > (int)mNumSubiils -1)
		{
			has_data = false;
			return true;			
		}
		
		if(cur_iilidx < 0)
		{
			cur_iilidx = 0;
		}
		
		if(isRootIIL() && cur_idx > mNumEntries[cur_iilidx]-1)
		{
			cur_idx = mNumEntries[cur_iilidx]-1;
		}
		
		if(isSingleIIL() && cur_idx > (int)mNumDocs -1)
		{
			cur_idx = mNumDocs -1;
		}
		//1. get the first doc which value == cur_value && docid > cur_docid
		//2. if not found, get the first doc which value > cur_value
		//3. if not found, set has_data == false and return		
	}
	
	
//	if(reverse)
//	{
//		// get the first doc which less than cur_docid backward
//		if(cur_docid < mDocids[0])
//		{
//			cur_iilidx = 0;
//			cur_idx = 0;
//			has_data = false;
///			return true;
//		}
//	}
//	else // normal order
//	{
//		// get the first doc which greater than cur_docid
//		u64 max_docid = mDocids[mNumDocs-1];
//		if(isRootIIL()) max_value = mMaxVals[mNumSubiils-1];
//		if(cur_docid > max_docid)
//		{
//			cur_iilidx = 0;
//			cur_idx = 0;
//			has_data = false;
//			return true;
//		}
//	}
	
//	if(isSingleIIL())
//	{
//		cur_iilidx = 0;
//		if(reverse)
//		{
//			cur_idx = firstLTRev(cur_docid);
//			aos_assert_r(cur_idx >= 0,false);
//		}
//		else
//		{
//			cur_idx = firstGT(cur_docid);
//			aos_assert_r(cur_idx <= (int)mNumDocs -1,false);
//		}
//		return true;
//	}
	// root iil
//	AosIILHitPtr subiil;
//	if(reverse)
//	{
//		cur_iilidx = firstSubLTRev(cur_docid);
//		aos_assert_r(cur_iilidx >= 0,false);
//		subiil = getIILByIndex(cur_iilidx,rdata);
//		if(!subiil)
//		{
//			// should not happen
//			has_data = false;
//			return true;
//		}
//		cur_idx = subiil->firstLTRev(cur_docid);
//	}
//	else
//	{
//		cur_iilidx = firstSubGT(cur_docid);
//		aos_assert_r(cur_iilidx <= (int)mNumSubiils-1,false);
//		subiil = getIILByIndex(cur_iilidx,rdata);
//		if(!subiil)
//		{
//			// should not happen
//			has_data = false;
//			return true;
//		}
//		cur_idx = subiil->firstGT(cur_docid);
//	}
	
	return true;
}			
*/



int
AosIILStr::countNumRec(const AosIILIdx start_iilidx,
					   const AosIILIdx end_iilidx,
					   const bool iilmgrlock, 
					   const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return countNumSingle(start_iilidx, end_iilidx);
	}

	AosIILStrPtr subiil;
	
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);

	// Chen Ding, 07/12/2012
	if (end < 0) 
	{
		aos_assert_r(start == -5 && end == -5, 0);
		return 0;
	}

	aos_assert_r(end >= start && start >= 0 && end <= ((int)mNumSubiils-1) , 0);

	if(start == end)
	{
		subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
		aos_assert_r(subiil,0);
		return subiil->countNumRec(
							start_iilidx,
				   			end_iilidx,
				   			iilmgrlock, 
				   			rdata);
	}
	
	// start != end
	int total = 0;
	subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
	aos_assert_r(subiil,0);
	total += subiil->countNumToEndRec(
						start_iilidx,
			   			iilmgrlock, 
			   			rdata);
	subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);
	total += subiil->countNumFromStartRec(
						end_iilidx,
			   			iilmgrlock, 
			   			rdata);
	for(int i = start+1;i < end;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}


int
AosIILStr::countNumSingle(const AosIILIdx start_iilidx, const AosIILIdx end_iilidx)
{
	int start = start_iilidx.getIdx(0);
	int end = end_iilidx.getIdx(0);
	if (end < start) return 0;
	return end - start + 1;
}


int
AosIILStr::countNumToEndSingle(const AosIILIdx start_iilidx)
{
	return mNumDocs - start_iilidx.getIdx(0);
}


int
AosIILStr::countNumFromStartSingle(const AosIILIdx end_iilidx)
{
	return end_iilidx.getIdx(0) + 1;
}


int				
AosIILStr::countNumToEndRec(const AosIILIdx start_iilidx,
								 const bool iilmgrlock, 
				   				 const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return countNumToEndSingle(start_iilidx);
	}

	AosIILStrPtr subiil;

	int start = start_iilidx.getIdx(mLevel);

	aos_assert_r(start >= 0 && start <= ((int)mNumSubiils-1) , 0);

	int total = 0;
	subiil = getSubiilByIndexPriv(start, iilmgrlock, rdata);
	aos_assert_r(subiil,0);
	total += subiil->countNumToEndRec(
						start_iilidx,
			   			iilmgrlock, 
			   			rdata);
	for(int i = start+1;i < (int)mNumSubiils;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}

int				
AosIILStr::countNumFromStartRec(const AosIILIdx end_iilidx,
								 const bool iilmgrlock, 
				   				 const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return countNumFromStartSingle(end_iilidx);
	}

	AosIILStrPtr subiil;
	int end = end_iilidx.getIdx(mLevel);

	aos_assert_r(end >= 0 && end <= ((int)mNumSubiils-1) , 0);

	int total = 0;
	subiil = getSubiilByIndexPriv(end, iilmgrlock, rdata);
	total += subiil->countNumFromStartRec(
						end_iilidx,
			   			iilmgrlock, 
			   			rdata);
	for(int i = 0;i < end;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}				   				 

bool 
AosIILStr::prevQueryPosSingle(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	int idx = iilidx.getIdx(0);
	if(idx > 0)
	{
		idx --;
		iilidx.setIdx(0,idx);
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool 
AosIILStr::prevQueryPosL1(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	int idx0 = iilidx.getIdx(0);
	if(idx0 > 0)
	{
		idx0 --;
		iilidx.setIdx(0,idx0);
		return true;
	}
	
	int idx1 = iilidx.getIdx(1);
	if(idx1 == 0)
	{
		return false;
	}
	idx1 --;
	iilidx.setIdx(1,idx1);
	idx0 = mNumEntries[idx1]-1;
	iilidx.setIdx(0,idx0);
	return true;
}

bool 
AosIILStr::prevQueryPosRec(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return prevQueryPosSingle(iilidx,
								  iilmgrlock, 
								  rdata);
	}
	
	if(mLevel == 1)
	{
		return prevQueryPosL1(iilidx,
							  iilmgrlock, 
							  rdata);
	}

	int idx = iilidx.getIdx(mLevel);
	if(idx < 0 || idx >= (int)mNumSubiils)return false;
	AosIILStrPtr subiil = getSubiilByIndexPriv(idx, iilmgrlock, rdata);
	aos_assert_r(subiil,false);
	
	if(subiil->prevQueryPosRec(iilidx,iilmgrlock,rdata))
	{
		return true;
	}
	
	if(idx == 0)
	{
		return false;
	}
	idx --;
	subiil = getSubiilByIndexPriv(idx, iilmgrlock, rdata);
	aos_assert_r(subiil,false);
	iilidx.setIdx(mLevel,idx);
	bool rslt = subiil->lastQueryPosRec(iilidx,iilmgrlock,rdata);
	aos_assert_r(rslt,false);
	return rslt;	
}

bool
AosIILStr::lastQueryPosSingle(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	aos_assert_rr(mNumDocs > 0, rdata, false);
	iilidx.setIdx(0,mNumDocs-1);
	return true;
}

bool
AosIILStr::lastQueryPosL1(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	aos_assert_rr((int)mNumSubiils > 0, rdata, false);
	aos_assert_rr(mNumEntries[mNumSubiils-1] > 0, rdata, false);
	iilidx.setIdx(0,mNumEntries[mNumSubiils-1]-1);
	iilidx.setIdx(1,mNumSubiils-1);
	return true;
}

bool
AosIILStr::lastQueryPosRec(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return lastQueryPosSingle(iilidx,
								  iilmgrlock, 
								  rdata);
	}
	
	if(mLevel == 1)
	{
		return lastQueryPosL1(iilidx,
							  iilmgrlock, 
							  rdata);
	}
	
	int idx = mNumSubiils -1;
	iilidx.setIdx(mLevel,idx);
	AosIILStrPtr subiil = getSubiilByIndexPriv(idx, iilmgrlock, rdata);
	aos_assert_r(subiil,false);
	
	return subiil->lastQueryPosRec(iilidx,
								   iilmgrlock,
								   rdata);
}						

bool
AosIILStr::nextQueryPosSingle(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	int idx = iilidx.getIdx(0);
	if(idx >= (int)mNumDocs-1)
	{
		return false;
	}
	idx++;
	iilidx.setIdx(0,idx);
	return true;
}

bool
AosIILStr::nextQueryPosL1(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	// This function 'increments' 'iilidx', if possible.
	int idx0 = iilidx.getIdx(0);
	int idx1 = iilidx.getIdx(1);
	if(idx1 > (int)mNumSubiils-1)
	{
		return false;
	}
	
	if(idx0 < mNumEntries[idx1]-1)
	{
		idx0 ++;
		iilidx.setIdx(0,idx0);
		return true;
	}
	
	if(idx1 >= (int)mNumSubiils-1)
	{
		return false;
	}
	
	idx1 ++;
	iilidx.setIdx(1,idx1);
	iilidx.setIdx(0,0);
	return true;
}

bool
AosIILStr::nextQueryPosRec(AosIILIdx &iilidx,
						const bool iilmgrlock, 
						const AosRundataPtr &rdata)
{
	// This function 'increments' 'iilidx', if possible.
	if (isLeafIIL())
	{
		return nextQueryPosSingle(iilidx,
								  iilmgrlock, 
								  rdata);
	}
	
	if(mLevel == 1)
	{
		return nextQueryPosL1(iilidx,
							  iilmgrlock, 
							  rdata);
	}

	// The IIL is Level > 1. This means it needs to get the current
	// IIL, and recursively ask the subiil to increase. If it cannot, 
	// need to find the next subiil. If it is already the last subiil, 
	// it means there are no more entries.
	int idx = iilidx.getIdx(mLevel);
	aos_assert_rr(idx >= 0, rdata, false);

	if(idx >= (int)mNumSubiils)return false;

	AosIILStrPtr subiil = getSubiilByIndexPriv(idx, iilmgrlock, rdata);
	aos_assert_r(subiil,false);
	
	if(subiil->nextQueryPosRec(iilidx,iilmgrlock,rdata))
	{
		return true;
	}
	
	// This means that the current subiil has no more entries. Check 
	// whether it is the last one.
	if(idx >= (int)mNumSubiils-1)
	{
		// It is the last one.
		return false;
	}

	idx++;
	iilidx.setIdx(mLevel,idx);
	// set first pos(no need to use new function
	for(int i = 0;i < mLevel;i++)
	{
		iilidx.setIdx(i,0);
	}
	return true;	
}


