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


AosJimoTablePtr
AosJimoTable::getSubiilPriv(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	i64 index = getSubiilIndexPriv(value, rdata);
	aos_assert_r((index >= 0) && (index < mNumSubiils), 0);
	return getSubiilByIndexPriv(index, rdata);
}


i64
AosJimoTable::getSubiilIndexPriv(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mSubiils > 0, -1);

	i64 idx = mMaxData->findPos(0, value.data(), value.length());
	aos_assert_r(idx >= 0, -1);

	if (idx >= mNumSubiils) idx = mNumSubiils - 1;

	return idx;
}


AosJimoTablePtr
AosJimoTable::getSubiilByIndexPriv(
		const i64 &idx, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(!isLeafIIL(), 0);
	
	i64 iilidx = idx;
	if (idx == -10) iilidx = 0;
	aos_assert_r(iilidx >= 0 && iilidx < mNumSubiils, 0);

	if (!mSubiils)
	{
		mSubiils = OmnNew AosJimoTablePtr[AosIIL::mMaxSubIILs + AosIIL::eExtraSubIILsSpace];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[iilidx]) return mSubiils[iilidx];

	AosIILType type = eAosIILType_JimoTable;
	AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
		mIILIds[iilidx], mSiteid, mSnapShotId, type, rdata);
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[iilidx] << ":" << type << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_JimoTable)
	{
		OmnAlarm << "Not a string IIL: " 
			<< iil->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}

	mSubiils[iilidx] = dynamic_cast<AosJimoTable *>(iil.getPtr());
	mSubiils[iilidx]->mParentIIL = this;
	mSubiils[iilidx]->mIILIdx = iilidx;

	return mSubiils[iilidx];
}


AosJimoTablePtr
AosJimoTable::getSubiilByIndexPriv(
		const u64 &iilid,
		const i64 &iil_idx,
		const AosRundataPtr &rdata)
{
	lockUpdate();
	if (!mSubiils)
	{
		mSubiils = OmnNew AosJimoTablePtr[AosIIL::mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mNumSubiils <= iil_idx)
	{
		OmnAlarm << "error" << enderr;
		unlockUpdate();
		return 0;
	}

	i64 idx = iil_idx;
	if (mIILIds[idx] != iilid)
	{
		int i = idx + 1;
		for (; i<mNumSubiils; i++)	
		{
			if (mIILIds[i] == iilid)
			{
				idx = i;
				break;
			}
		}
		if (i == mNumSubiils)
		{
			OmnAlarm << "error" << enderr;
			unlockUpdate();
			return 0;
		}
	}

	AosJimoTablePtr subiil = mSubiils[idx];
	unlockUpdate();

	if (!subiil)
	{
		AosIILType type = eAosIILType_JimoTable;
		AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
			iilid, mSiteid, mSnapShotId, type, rdata);
		aos_assert_r(iil, 0);
		aos_assert_r(iil->getIILType() == eAosIILType_JimoTable, 0);

		lockUpdate();
		bool rslt = setSubiilIndexPriv(iil, idx, rdata);
		unlockUpdate();

		//OmnScreen << "load subiil, iilid:" << iilid << ", idx:"
		//	<< idx << ", real_idx:" << iil->getIILIdx() << endl;

		aos_assert_r(rslt, 0);

		subiil = dynamic_cast<AosJimoTable *>(iil.getPtr());
	}

	return subiil;
}


bool
AosJimoTable::setSubiilIndexPriv(
		const AosIILObjPtr &iil,
		const i64 &iil_idx, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_JimoTable, false);
	aos_assert_r(!isLeafIIL(), false);
	aos_assert_r(iil_idx >= 0 && iil_idx < mNumSubiils, false);

	if (!mSubiils)
	{
		mSubiils = OmnNew AosJimoTablePtr[AosIIL::mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	i64 idx = iil_idx;
	u64 iilid = iil->getIILID();
	if (mIILIds[idx] != iilid)
	{
		int i = idx + 1;
		for (; i<mNumSubiils; i++)	
		{
			if (mIILIds[i] == iilid)
			{
				idx = i;
				break;
			}
		}
		if (i == mNumSubiils)
		{
			OmnAlarm << "error" << enderr;
			return false;
		}
	}

	if (mSubiils[idx] && mSubiils[idx] != iil)
	{
		OmnAlarm << "idx error" << enderr;
		return false;
	}

	mSubiils[idx] = dynamic_cast<AosJimoTable *>(iil.getPtr());
	mSubiils[idx]->mParentIIL = this;
	mSubiils[idx]->mIILIdx = idx;
	return true;
}


/*
bool		
AosIILBigStr::nextDocidPrivFind(
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
		i64 index = idx.getIdx(mLevel);
		bool rslt = nextDocidSinglePriv(index, reverse, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		
		rslt = idx.setIdx(mLevel, index);
		aos_assert_r(rslt, false);
		
		return rslt;
	}
	
	AosIILBigStrPtr subiil = getSubiilByCondPriv(idx, value, opr, reverse, rdata);
	if (!subiil)
	{
		// This means there are no entries that satisfies the condition.
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}

	bool rslt;
	while (subiil)
	{
		aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
		rslt = subiil->nextDocidSafeFind(idx, value, opr, docid, isunique, reverse, rdata);
		aos_assert_r(rslt, false);

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


bool		
AosIILBigStr::findPosPriv(
		const AosOpr opr,
		const OmnString &value,
		const u64 &docid, 
		AosIILIdx &idx,
		const bool reverse, 
		const AosRundataPtr &rdata) 
{
	if (isLeafIIL())
	{
		bool rslt = findPosSinglePriv(opr, value, docid, idx, reverse, rdata);
		aos_assert_r(rslt, false);				
		return true;
	}
	
	AosIILBigStrPtr subiil = getFirstSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
	if (!subiil)
	{
		idx.setInvalid(mLevel);
		return true;
	}
	
	bool rslt = subiil->findPosPriv(opr,value,docid,idx,reverse,rdata);
	aos_assert_r(rslt, false);
	
	if(!idx.isValid(mLevel-1))
	{
		idx.setInvalid(mLevel);
		return true;
	}
	
	return true;
}


AosIILBigStrPtr
AosIILBigStr::getSubiilByCondPriv(
		AosIILIdx &idx,
		const OmnString &value,    
		const AosOpr opr,
		const bool reverse,        
		const AosRundataPtr &rdata)
{
	if (idx.isStart(mLevel))
	{
		return getFirstSubiilByCondPriv(idx, value, opr, reverse, rdata);
	}
	return nextSubiilByCondPriv(idx, value, opr, reverse, rdata);
}


AosIILBigStrPtr
AosIILBigStr::getSubiilByCondPriv(
		AosIILIdx &idx,
		const OmnString &value,    
		const u64 &docid,
		const AosOpr opr,
		const bool reverse,        
		const AosRundataPtr &rdata)
{
	if (idx.isStart(mLevel))
	{
		return getFirstSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
	}
	return nextSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
}


AosIILBigStrPtr
AosIILBigStr::getFirstSubiilByCondPriv(
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
	aos_assert_r(!isLeafIIL(), 0);
	aos_assert_r(mNumSubiils > 0, 0);
	
	i64 index = reverse ? mNumSubiils-1 : 0;
	int rsltInt;
	if (reverse)
	{
		switch (opr)
		{
		case eAosOpr_gt :	// >
			 rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
			 if (rsltInt <= 0) index = -5;
			 break;

		case eAosOpr_ge:	// >=
			 rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
			 if (rsltInt < 0) index = -5;
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
			 if (rsltInt == 0) 
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
			 if (rsltInt >= 0) index = -5;
			 break;

		case eAosOpr_le :	// <=
			 rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
			 if (rsltInt > 0) index = -5;
			 break;

		case eAosOpr_eq :	// ==
			 index = getFirstSubiilIndexGE(value, rdata);
			 break;

		case eAosOpr_ne :	// !=
			 rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
			 if (rsltInt == 0) 
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

	if (index < 0 || index >= mNumSubiils)
	{
		idx.setInvalid(mLevel);	 
		return 0;
	}
	
	bool rslt = idx.setIdx(mLevel, index);
	aos_assert_r(rslt, 0);

	return getSubiilByIndexPriv(index, rdata);
}


AosIILBigStrPtr
AosIILBigStr::getFirstSubiilByCondPriv(
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
	aos_assert_r(!isLeafIIL(), 0);
	aos_assert_r(mNumSubiils > 0, 0);
	
	i64 index = reverse ? mNumSubiils-1 : 0;
	bool rslt;
	if (reverse)
	{
		switch (opr)
		{
		case eAosOpr_gt :	// >
			 rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_gt, value, docid, mIsNumAlpha);
			 if (!rslt) index = -5;
			 break;

		case eAosOpr_ge:	// >=
			 rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_ge, value, docid, mIsNumAlpha);
			 if (!rslt) index = -5;
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
			 if (!rslt) 
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
			 if (!rslt) index = -5;
			 break;

		case eAosOpr_le :	// <=
			 rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_le, value, docid, mIsNumAlpha);
			 if (!rslt) index = -5;
			 break;

		case eAosOpr_eq :	// ==
			 index = getFirstSubiilIndexGE(value, docid, rdata);
			 break;

		case eAosOpr_ne :	// !=
			 rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_ne, value, docid, mIsNumAlpha);
			 if (!rslt) 
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

	if (index < 0 || index >= mNumSubiils)
	{
		idx.setInvalid(mLevel);	 
		return 0;
	}
	
	rslt = idx.setIdx(mLevel, index);
	aos_assert_r(rslt, 0);
	
	return getSubiilByIndexPriv(index, rdata);
}
		
			 
i64
AosIILBigStr::getFirstSubiilIndexGT(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// opr >
	int rsltInt = AosIILUtil::valueMatch(mMaxVals[0], value, mIsNumAlpha);
	if (rsltInt > 0) return 0;

	rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
	if (rsltInt <= 0) return -5;
	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
	while(left <= right)
	{
		if (left == right) return left;
		
		idx = (right + left) >> 1;
		rsltInt = AosIILUtil::valueMatch(mMaxVals[idx], value, mIsNumAlpha);
		if (rsltInt <= 0)
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


i64
AosIILBigStr::getFirstSubiilIndexGT(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// opr >
	bool rslt = AosIILUtil::valueMatch(mMaxVals[0], mMaxDocids[0], eAosOpr_gt, value, docid, mIsNumAlpha);
	if (rslt) return 0;
		
	rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_gt, value, docid, mIsNumAlpha);
	if (!rslt) return -5;
	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
	while(left <= right)
	{
		if (left == right) return left;
		
		idx = (right + left) >> 1;
		rslt = AosIILUtil::valueMatch(mMaxVals[idx], mMaxDocids[idx], eAosOpr_gt, value, docid, mIsNumAlpha);
		if (rslt)
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


i64
AosIILBigStr::getFirstSubiilIndexGE(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	// opr >=
	int rsltInt = AosIILUtil::valueMatch(mMaxVals[0], value, mIsNumAlpha);
	if (rsltInt >= 0) return 0;
	
	rsltInt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], value, mIsNumAlpha);
	if (rsltInt < 0) return -5;
			 	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
	while(left <= right)
	{
		if (left == right) return left;
		
		idx = (right + left) >> 1;
		rsltInt = AosIILUtil::valueMatch(mMaxVals[idx], value, mIsNumAlpha);
		if (rsltInt < 0)
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


i64
AosIILBigStr::getFirstSubiilIndexGE(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// opr >=
	bool rslt = AosIILUtil::valueMatch(mMaxVals[0], mMaxDocids[0], eAosOpr_ge, value, docid, mIsNumAlpha);
	if (rslt) return 0;
		
	rslt = AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], mMaxDocids[mNumSubiils-1], eAosOpr_ge, value, docid, mIsNumAlpha);
	if (!rslt) return -5;
	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
	while(left <= right)
	{
		if (left == right) return left;
		
		idx = (right + left) >> 1;
		rslt = AosIILUtil::valueMatch(mMaxVals[idx], mMaxDocids[idx], eAosOpr_ge, value, docid, mIsNumAlpha);
		if (rslt)
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


i64
AosIILBigStr::getLastSubiilIndexLT(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	//opr <
	aos_assert_r(mNumSubiils > 0, -5);
	int rsltInt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], value, mIsNumAlpha);
	if (rsltInt < 0) return mNumSubiils-1;
	
	rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
	if (rsltInt >= 0) return -5;
	
	// When it comes to this point, index=0 satisifies the condition
	// and index = mNumSubiils-1 does not. 
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
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


i64
AosIILBigStr::getLastSubiilIndexLT(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//opr <
	bool rslt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], mMinDocids[mNumSubiils-1], eAosOpr_lt, value, docid, mIsNumAlpha);
	if (rslt) return mNumSubiils-1;
    
	rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_lt, value, docid, mIsNumAlpha);
	if (!rslt) return -5;
	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
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


i64
AosIILBigStr::getLastSubiilIndexLE(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	//opr <=
	int rsltInt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], value, mIsNumAlpha);
	if (rsltInt <= 0) return mNumSubiils-1;
	
	rsltInt = AosIILUtil::valueMatch(mMinVals[0], value, mIsNumAlpha);
	if (rsltInt > 0) return -5;
	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
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


i64
AosIILBigStr::getLastSubiilIndexLE(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//opr <
	bool rslt = AosIILUtil::valueMatch(mMinVals[mNumSubiils-1], mMinDocids[mNumSubiils-1], eAosOpr_le, value, docid, mIsNumAlpha);
	if (rslt) return mNumSubiils-1;
    
	rslt = AosIILUtil::valueMatch(mMinVals[0], mMinDocids[0], eAosOpr_le, value, docid, mIsNumAlpha);
	if (!rslt) return -5;
	
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
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


i64
AosIILBigStr::getLastSubiilIndexPrefix(
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(!mIsNumAlpha, -5);
	i64 left = 0;
	i64 right = mNumSubiils - 1;
	i64 idx;
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


i64
AosIILBigStr::getLastSubiilIndexPrefix(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return getLastSubiilIndexPrefix(value, rdata);
}


AosIILBigStrPtr
AosIILBigStr::nextSubiilByCondPriv(
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
		i64 iilidx = idx.getIdx(mLevel);
		if (containValuePriv(mMinVals[iilidx], mMaxVals[iilidx], 
					value, opr, reverse, keep_search, rdata))
		{
			AosIILBigStrPtr subiil = getSubiilByIndexPriv(iilidx, rdata);
			aos_assert_r(subiil, 0);
			return subiil;
		}
		if (!keep_search) return 0;
		idx.next(mLevel, reverse);
	}
	return 0;
}


AosIILBigStrPtr
AosIILBigStr::nextSubiilByCondPriv(
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
		i64 iilidx = idx.getIdx(mLevel);
		if (containValuePriv(mMinVals[iilidx], mMaxVals[iilidx], 
					value, opr, reverse, keep_search, rdata))
		{
			AosIILBigStrPtr subiil = getSubiilByIndexPriv(iilidx, rdata);
			aos_assert_r(subiil, 0);
			return subiil;
		}
		if (!keep_search) return 0;
		idx.next(mLevel, reverse);
	}
	return 0;
}


AosIILBigStrPtr
AosIILBigStr::getSubiilPriv(
		const OmnString &value, 
		const u64 &docid,
		const bool value_unique, 
		const bool docid_unique,
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
	
	i64 idx = getSubiilIndex3Priv(value, docid, rdata);
	aos_assert_r(idx >= 0 && idx < mNumSubiils, 0);
				
	int rsltInt;
	if (value_unique)
	{	
		rsltInt = AosIILUtil::valueMatch(mMaxVals[idx], value, mIsNumAlpha);
		if (rsltInt == 0)
		{
			rdata->setError()<< "Value already exist: " << value << ":" << docid << ":" << mDocids[idx];
			OmnAlarm << rdata->getErrmsg()<< enderr;
			return 0;	
		}
		if (rsltInt < 0)
		{
			aos_assert_r(idx == mNumSubiils - 1, 0);
		}

		rsltInt = AosIILUtil::valueMatch(mMinVals[idx], value, mIsNumAlpha);
		if (rsltInt == 0)
		{
			rdata->setError()<< "Value already exist: " << value << ":" << docid << ":" << mDocids[idx];
			OmnAlarm << rdata->getErrmsg()<< enderr;
			return 0;	
		}

		if (rsltInt > 0 && idx > 0)
		{
			rsltInt = AosIILUtil::valueMatch(mMaxVals[idx - 1], value, mIsNumAlpha);
			aos_assert_r(rsltInt <= 0, 0);
			if (rsltInt == 0)
			{
				rdata->setError()<< "Value already exist: " << value << ":" << docid << ":" << mDocids[idx];
				OmnAlarm << rdata->getErrmsg()<< enderr;
				return 0;	
			}
		}
	}
	
	AosIILBigStrPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, 0);
	aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel - 1, 0);
	return subiil;
}


bool
AosIILBigStr::nextUniqueValuePriv(
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
	AosIILBigStrPtr subiil;
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

		i64 iilidx = idx.getIdx(mLevel);
		AosIILBigStrPtr subiil = getSubiilByIndexPriv(iilidx, rdata);
	}
	aos_assert_r(subiil, false);

	while (subiil)
	{
		aos_assert_r(subiil->mLevel >= 0 && subiil->mLevel == mLevel-1, false);
		bool rslt = subiil->nextUniqueValueSafe(idx, reverse, 
					opr, value, unique_value, found, rdata);
		aos_assert_r(rslt, false);
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
		aos_assert_r(rslt, false);
	}

	found = false;
	return true;
}


i64
AosIILBigStr::getSubiilIndex3Priv(
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
    i64 left = 0;
    i64 right = mNumSubiils - 1;
    i64 cur = 0;

	// There has to be at least two subiils
	aos_assert_r(left < right, 0);

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
			// found = true;		// Chen Ding, 05/22/2012, not used.
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


i64
AosIILBigStr::countNumRec(
		const AosIILIdx start_iilidx,
		const AosIILIdx end_iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return countNumSingle(start_iilidx, end_iilidx);
	}

	i64 start = start_iilidx.getIdx(mLevel);
	i64 end = end_iilidx.getIdx(mLevel);

	// Chen Ding, 07/12/2012
	if (end < 0) 
	{
		aos_assert_r(start == -5 && end == -5, 0);
		return 0;
	}

	aos_assert_r(end >= start && start >= 0 && end <= (mNumSubiils-1) , 0);

	AosIILBigStrPtr subiil;
	if (start == end)
	{
		subiil = getSubiilByIndexPriv(start, rdata);
		aos_assert_r(subiil, 0);

		return subiil->countNumRec(start_iilidx, end_iilidx, rdata);
	}
	
	// start != end
	subiil = getSubiilByIndexPriv(start, rdata);
	aos_assert_r(subiil, 0);

	i64 total = subiil->countNumToEndRec(start_iilidx, rdata);

	subiil = getSubiilByIndexPriv(end, rdata);
	aos_assert_r(subiil, 0);

	total += subiil->countNumFromStartRec(end_iilidx, rdata);

	for(i64 i = start+1;i < end;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}


i64
AosIILBigStr::countNumSingle(
		const AosIILIdx start_iilidx,
		const AosIILIdx end_iilidx)
{
	i64 start = start_iilidx.getIdx(0);
	i64 end = end_iilidx.getIdx(0);
	if (end < start) return 0;
	return end - start + 1;
}


i64
AosIILBigStr::countNumToEndSingle(const AosIILIdx start_iilidx)
{
	return mNumDocs - start_iilidx.getIdx(0);
}


i64
AosIILBigStr::countNumFromStartSingle(const AosIILIdx end_iilidx)
{
	return end_iilidx.getIdx(0) + 1;
}


i64
AosIILBigStr::countNumToEndRec(
		const AosIILIdx start_iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return countNumToEndSingle(start_iilidx);
	}

	i64 start = start_iilidx.getIdx(mLevel);
	aos_assert_r(start >= 0 && start <= (mNumSubiils-1) , 0);

	AosIILBigStrPtr subiil = getSubiilByIndexPriv(start, rdata);
	aos_assert_r(subiil, 0);

	i64 total = subiil->countNumToEndRec(start_iilidx, rdata);

	for(i64 i = start+1;i < mNumSubiils; i++)
	{
		total += mNumEntries[i];
	}
	return total;
}


i64
AosIILBigStr::countNumFromStartRec(
		const AosIILIdx end_iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return countNumFromStartSingle(end_iilidx);
	}

	i64 end = end_iilidx.getIdx(mLevel);
	aos_assert_r(end >= 0 && end <= (mNumSubiils-1) , 0);

	AosIILBigStrPtr subiil = getSubiilByIndexPriv(end, rdata);
	aos_assert_r(subiil, 0);

	i64 total = subiil->countNumFromStartRec(end_iilidx, rdata);

	for(i64 i = 0;i < end;i++)
	{
		total += mNumEntries[i];
	}
	return total;
}				   				 


bool 
AosIILBigStr::prevQueryPosSingle(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	i64 idx = iilidx.getIdx(0);
	if (idx > 0)
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
AosIILBigStr::prevQueryPosL1(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	i64 idx0 = iilidx.getIdx(0);
	if (idx0 > 0)
	{
		idx0--;
		iilidx.setIdx(0,idx0);
		return true;
	}
	
	i64 idx1 = iilidx.getIdx(1);
	if (idx1 == 0)
	{
		return false;
	}
	idx1--;
	iilidx.setIdx(1,idx1);
	idx0 = mNumEntries[idx1] - 1;
	iilidx.setIdx(0,idx0);
	return true;
}


bool 
AosIILBigStr::prevQueryPosRec(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return prevQueryPosSingle(iilidx, rdata);
	}
	
	if (mLevel == 1)
	{
		return prevQueryPosL1(iilidx, rdata);
	}

	i64 idx = iilidx.getIdx(mLevel);
	if (idx < 0 || idx >= mNumSubiils) return false;

	AosIILBigStrPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);
	
	bool rslt = subiil->prevQueryPosRec(iilidx, rdata);
	if (rslt) return true;
	
	if (idx == 0)
	{
		return false;
	}

	idx--;
	subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);

	iilidx.setIdx(mLevel, idx);
	rslt = subiil->lastQueryPosRec(iilidx, rdata);
	aos_assert_r(rslt, false);

	return rslt;	
}


bool
AosIILBigStr::lastQueryPosSingle(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumDocs > 0, false);
	iilidx.setIdx(0, mNumDocs-1);
	return true;
}


bool
AosIILBigStr::lastQueryPosL1(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mNumSubiils > 0, false);
	aos_assert_r(mNumEntries[mNumSubiils-1] > 0, false);
	iilidx.setIdx(0,mNumEntries[mNumSubiils-1]-1);
	iilidx.setIdx(1,mNumSubiils-1);
	return true;
}


bool
AosIILBigStr::lastQueryPosRec(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		return lastQueryPosSingle(iilidx, rdata);
	}
	
	if (mLevel == 1)
	{
		return lastQueryPosL1(iilidx, rdata);
	}
	
	i64 idx = mNumSubiils -1;
	iilidx.setIdx(mLevel, idx);

	AosIILBigStrPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);
	
	return subiil->lastQueryPosRec(iilidx, rdata);
}


bool
AosIILBigStr::nextQueryPosSingle(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	i64 idx = iilidx.getIdx(0);
	if (idx >= mNumDocs-1)
	{
		return false;
	}

	idx++;
	iilidx.setIdx(0,idx);
	return true;
}


bool
AosIILBigStr::nextQueryPosL1(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	// This function 'increments' 'iilidx', if possible.
	i64 idx0 = iilidx.getIdx(0);
	i64 idx1 = iilidx.getIdx(1);
	if (idx1 > mNumSubiils-1)
	{
		return false;
	}
	
	if (idx0 < mNumEntries[idx1]-1)
	{
		idx0 ++;
		iilidx.setIdx(0,idx0);
		return true;
	}
	
	if (idx1 >= mNumSubiils-1)
	{
		return false;
	}
	
	idx1 ++;
	iilidx.setIdx(1,idx1);
	iilidx.setIdx(0,0);
	return true;
}


bool
AosIILBigStr::nextQueryPosRec(
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	// This function 'increments' 'iilidx', if possible.
	if (isLeafIIL())
	{
		return nextQueryPosSingle(iilidx, rdata);
	}
	
	if (mLevel == 1)
	{
		return nextQueryPosL1(iilidx, rdata);
	}

	// The IIL is Level > 1. This means it needs to get the current
	// IIL, and recursively ask the subiil to increase. If it cannot, 
	// need to find the next subiil. If it is already the last subiil, 
	// it means there are no more entries.
	i64 idx = iilidx.getIdx(mLevel);
	aos_assert_r(idx >= 0, false);

	if (idx >= mNumSubiils) return false;

	AosIILBigStrPtr subiil = getSubiilByIndexPriv(idx, rdata);
	aos_assert_r(subiil, false);

	bool rslt = subiil->nextQueryPosRec(iilidx, rdata);
	if (rslt) return true;
	
	// This means that the current subiil has no more entries. Check 
	// whether it is the last one.
	if (idx >= mNumSubiils-1)
	{
		// It is the last one.
		return false;
	}

	idx++;
	iilidx.setIdx(mLevel,idx);
	// set first pos(no need to use new function
	for(i64 i = 0;i < mLevel;i++)
	{
		iilidx.setIdx(i,0);
	}
	return true;	
}


AosIILObjPtr 
AosIILBigStr::firstLeaf(AosIILIdx &idx,
					const AosRundataPtr &rdata)
{
	if (mLevel == 0)
	{
		idx.setIdx(0,0);
		AosIILObjPtr thisPtr(this, false);
		return thisPtr;
	}
	
	int cur_level = mLevel;
	AosIILBigStrPtr cur_iil;
	while(cur_level>= 0)
	{
		idx.setIdx(cur_level,0);
		if (cur_level == 0)return cur_iil;		
		// not level 0 node
		cur_iil = getSubiilByIndexPriv(0, rdata);
		aos_assert_r(cur_iil,0);
		cur_level --;
	}
	
	OmnShouldNeverComeHere;
	return 0;	
}

	
AosIILObjPtr 						 
AosIILBigStr::nextLeaf(AosIILIdx &idx,
					const AosRundataPtr &rdata)
{
	bool returned = false;
	returnSubIILsPriv(returned, true, rdata);	

	if (idx.isFinished()) return 0;

	if (mLevel == 0)
	{
		return 0;
	}
	if (mLevel == 1)
	{
		i64 index = idx.getIdx(0);
		if (index < mNumSubiils-1)
		{
			index ++;
			idx.setIdx(0,index);
			return getSubiilByIndexPriv(index, rdata);
		}
		return 0;
	}
	
	// level 2 or more
	
	AosIILBigStrPtr cur_iil;
	AosIILObjPtr next_leaf;
	i64 index = idx.getIdx(mLevel -1);
	while(index < mNumSubiils)
	{
		cur_iil = getSubiilByIndexPriv(index, rdata);
		next_leaf = cur_iil->nextLeaf(idx,rdata);
		if (next_leaf)
		{
			i64 index = idx.setIdx(mLevel -1,index);			
			return next_leaf;
		}
		index ++;
	}
	
	idx.setIdx(mLevel -1,0);
	
	if (isRootIIL())
	{
		// mark that all the leaves are finished
		idx.setFinished();
	}
	return 0;
}


AosIILObjPtr 
AosIILBigStr::firstLeaf(AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree,
						const AosRundataPtr &rdata)
{
	if (mLevel == 0)
	{
		idx.setIdx(0,0);
		AosIILObjPtr thisPtr(this, false);
		return thisPtr;
	}
	
	int cur_level = mLevel;
	AosIILBigStrPtr thisPtr(this, false);
	AosIILBigStrPtr cur_iil = thisPtr;
	while(cur_level>= 0)
	{
		idx.setIdx(cur_level,0);
		if (cur_level == 1)
		{
			tree = cur_iil->getBitmapTree(rdata);
		}
		if (cur_level == 0)return cur_iil;		
		// not level 0 node
		cur_iil = getSubiilByIndexPriv(0, rdata);
		aos_assert_r(cur_iil,0);
		cur_level --;
	}
	
	OmnShouldNeverComeHere;
	return 0;	
}


AosIILObjPtr 						 
AosIILBigStr::nextLeaf(AosIILIdx &idx,
					  AosBitmapTreeObjPtr &tree,
					const AosRundataPtr &rdata)
{
	bool returned = false;
	returnSubIILsPriv(returned, true, rdata);	

	if (idx.isFinished()) return 0;

	if (mLevel == 0)
	{
		return 0;
	}
	if (mLevel == 1)
	{
		tree = getBitmapTree(rdata);
		i64 index = idx.getIdx(0);
		if (index < mNumSubiils-1)
		{
			index ++;
			idx.setIdx(0,index);
			return getSubiilByIndexPriv(index, rdata);
		}
		return 0;
	}
	
	// level 2 or more
	
	AosIILBigStrPtr cur_iil;
	AosIILObjPtr next_leaf;
	i64 index = idx.getIdx(mLevel -1);
	while(index < mNumSubiils)
	{
		cur_iil = getSubiilByIndexPriv(index, rdata);
		next_leaf = cur_iil->nextLeaf(idx,tree,rdata);
		if (next_leaf)
		{
			idx.setIdx(mLevel -1,index);			
			return next_leaf;
		}
		index ++;
	}
	
	idx.setIdx(mLevel -1,0);
	
	if (isRootIIL())
	{
		// mark that all the leaves are finished
		idx.setFinished();
	}
	return 0;
}
	
bool
AosIILBigStr::moveToRec(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						u64 &page_start,
						const bool &reverse,
						bool &has_data,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return moveToSingle(start_iilidx,
						end_iilidx,
						page_start,
						reverse,
						has_data,
						copytype, 
						rdata);
	}

	// parent iil
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);
	bool handle_first = false;
	bool handle_last = false;
	
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}
	else
	{
		start ++;
	}
	
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumSubiils - 1;
	}
	else
	{
		end --;
	}
	
	if (copytype == AosIILUtil::eCopyNormal || 
	   copytype == AosIILUtil::eCopyToHead ||
	   copytype == AosIILUtil::eCopyToTail)
	{
		handle_first = true;
	}

	if (copytype == AosIILUtil::eCopyNormal || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		handle_last = true;
	}
		
	bool rslt = false;

	// hand first 
	if(handle_first)
	{
		if(!reverse)
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(start-1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyToTail, 
								rdata);
			aos_assert_r(rslt,false);
			start_iilidx.setIdx(mLevel,start-1);
		}
		else // reverse
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(end+1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyToHead, 
								rdata);
			aos_assert_r(rslt,false);
			end_iilidx.setIdx(mLevel,end+1);
	
		}
	}
	
	if(page_start == 0 && has_data)
	{
		return true;
	}

	if(start <= end)
	{
		has_data = true;
		if(!reverse)
		{
			for(int i = start;i <= end;i++)
			{
				if((i64)page_start >= mNumEntries[i])
				{
					page_start -= mNumEntries[i];
				}
				else 
				{
					rslt = getSubiilByIndexPriv(i, rdata)->moveToRec(
										start_iilidx,
										end_iilidx,
										page_start,
										reverse,
										has_data,
										AosIILUtil::eCopyAll, 
										rdata);
					aos_assert_r(rslt,false);
					start_iilidx.setIdx(mLevel,i);
					return true;
				}
			}
		}
		else// reverse
		{
			for(int i = end;i >= start;i--)
			{
				if((i64)page_start >= mNumEntries[i])
				{
					page_start -= mNumEntries[i];
				}
				else
				{
					rslt = getSubiilByIndexPriv(i, rdata)->moveToRec(
										start_iilidx,
										end_iilidx,
										page_start,
										reverse,
										has_data,
										AosIILUtil::eCopyAll, 
										rdata);
					aos_assert_r(rslt,false);
					start_iilidx.setIdx(mLevel,i);
					return true;
				}
			}
		}
	}
	
	if(page_start == 0 && has_data)
	{
		return true;
	}

	// hand last
	if(handle_last)
	{
		if(!reverse)
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(end+1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyFromHead, 
								rdata);
			aos_assert_r(rslt,false);
			start_iilidx.setIdx(mLevel,end+1);
		}
		else // reverse
		{
			has_data = true;
			rslt = getSubiilByIndexPriv(start-1, rdata)->moveToRec(
								start_iilidx,
								end_iilidx,
								page_start,
								reverse,
								has_data,
								AosIILUtil::eCopyToHead, 
								rdata);
			aos_assert_r(rslt,false);
			end_iilidx.setIdx(mLevel,start-1);
	
		}
	}
	return true;
}

bool
AosIILBigStr::moveToSingle(
						AosIILIdx &start_iilidx,
						AosIILIdx &end_iilidx,
						u64 &page_start,
						const bool &reverse,
						bool &has_data,
						const AosIILUtil::CopyType copytype, 
						const AosRundataPtr &rdata)
{
	int start = start_iilidx.getIdx(mLevel);
	int end = end_iilidx.getIdx(mLevel);
	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyFromHead ||
	   copytype == AosIILUtil::eCopyToHead)
	{
		start = 0;
	}

	if (copytype == AosIILUtil::eCopyAll || 
	   copytype == AosIILUtil::eCopyToTail ||
	   copytype == AosIILUtil::eCopyFromTail)
	{
		end = mNumDocs - 1;
	}
	
	int num_in_range = end - start +1;
	if(end - start +1 <= (i64)page_start)
	{
		page_start -= num_in_range;
		has_data = false;
		return true;
	} 
	
	if(!reverse)
	{
		start += page_start;
		page_start = 0;
		start_iilidx.setIdx(0,start);
	}
	else
	{
		end -= page_start;
		page_start = 0;
		end_iilidx.setIdx(0,end);
	}
	return true;
}
	
bool	
AosIILBigStr::getValueByIdxRecPriv(
				AosIILIdx &idx, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata)
{
	if(isLeafIIL())
	{
		return getValueByIdxSinglePriv(idx,value,docid,rdata);
	}
	
	i64 index = idx.getIdx(mLevel);
	aos_assert_r(index >= 0 && index < mNumSubiils, false);
	AosIILBigStrPtr subiil = getSubiilByIndexPriv(index, rdata);
	aos_assert_r(subiil,false);	
	return subiil->getValueByIdxRecPriv(idx, value, docid, rdata);
}

bool	
AosIILBigStr::getValueByIdxSinglePriv(
				AosIILIdx &idx, 
				OmnString &value,
				u64 &docid,
				const AosRundataPtr &rdata)
{
	i64 cur_idx = idx.getIdx(0);
	aos_assert_r(cur_idx >= 0 && cur_idx < mNumDocs, false);
	value = mValues[cur_idx];
	docid = mDocids[cur_idx];
	return true;
}
*/