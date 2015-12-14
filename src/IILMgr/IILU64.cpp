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
#include "IILMgr/IILU64.h"

#include "AppMgr/App.h"
#include "DfmUtil/DfmDocIIL.h"
#include "IILMgr/IILMgr.h"
#include "IILUtil/IILLog.h"
#include "IILUtil/IILSave.h"
#include "Porting/ThreadDef.h"
#include "SEUtil/Docid.h"


static bool sgCheckMemory = false;


AosIILU64::AosIILU64()
:
AosIIL(eAosIILType_U64),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0)
{
	mTotalIILs[eAosIILType_U64]++;
}


AosIILU64::AosIILU64(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_U64, isPersis, iilname),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0)
{
	mTotalIILs[eAosIILType_U64]++;
//OmnScreen << "====================create iil, iilid:" << iilid << ",total:" << mTotalIILs[eAosIILType_U64] << endl;
}


AosIILU64::AosIILU64(
		const u64 &iilid, 
		const u32 siteid, 
		const AosDfmDocIILPtr &doc, 	//Ketty 2012/11/15
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_U64),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0)
{
	mTotalIILs[eAosIILType_U64] ++;
//OmnScreen << "====================create iil, iilid:" << iilid << ",total:" << mTotalIILs[eAosIILType_U64] << endl;
	mIILID = iilid;
	mDfmDoc = doc;		// Ketty 2013/01/15

	//mIsGood = loadFromFilePriv(buff, siteid, rdata);
	//mIsGood = loadFromFilePriv(doc, siteid, rdata);	// Ketty 2012/11/15
	mIsGood = loadFromFilePriv(iilid, rdata);	// Ketty 2012/11/15
}


AosIILU64::~AosIILU64()
{
	//AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_U64] --;
//OmnScreen << "====================delete iil, iilid:" << mIILID << ",total:" << mTotalIILs[eAosIILType_U64] << endl;

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
			// if (mSubiils[i]) AosIILMgrSelf->returnIILPublic(mSubiils[i].getPtr(), rdata);
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
AosIILU64::staticInit(const AosXmlTagPtr &theconfig)
{
	// Its configuration is in the subtag "AOSCONFIG_IILMGR"
	aos_assert_r(theconfig, false);
	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;
	return true;
}


//////////////////////////////////////////////////////////////////
//  					Thread-Safe Functions					//
// These functions are thread-safe. Our rules are that these    // 
// functions shall NEVER call other thread-safe functions.      //
// They may call protected or private functions, which shall    // 
// NEVER call thread-safe functions.                            //
//////////////////////////////////////////////////////////////////
bool		
AosIILU64::nextDocidPriv(
		i64 &idx, 
		i64 &iilidx,
		const bool reverse, 
		const AosOpr opr,
		u64 &value,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata) 
{
	// It searches the first docid that meets the condition:
	// 		mValues[i] opr value
	//
	// Or in other word, it checks whether the current value meets
	// the condition. If yes, return the docid.
	// Otherwise, find the first one that meets the condition. If not
	// found, set idx = -1.
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
	bool rslt = false;
	docid = AOS_INVDID;
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		// If 'iilidx' is within [0, mNumSubiils-1],
		// use it.
		AosIILU64Ptr subiil;
		if (idx == -10)
		{
			// This is a new call. Find the sub-iil using either
			// [opr1, value1] for normal order or [opr2, value2]
			// for the reverse order.
			// Chen Ding, 12/12/2010
			iilidx = getSubIILIndex(-10, value, opr, !reverse);
			if (iilidx == -5)
			{
				// Chen Ding, 12/12/2010
				// Did not find the subiil, which means no matching
				// entries. Return.
				idx = -5;
				iilidx = -5;			// Chen Ding, 12/12/2010
				docid = AOS_INVDID;		// Chen Ding, 12/12/2010
				isunique = true;
				return true;			// Chen Ding, 12/12/2010
			}

			// Chen Ding, 12/12/2010
			// aos_assert_r(iilidx >= 0, false);
			if (iilidx < 0)
			{
				OmnAlarm << "Invalid: " << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return false;
			}
		}
		else
		{
			// This is a subsequent call. Use 'iilidx' to locate
			// the sub-iil.
			// Chen Ding, 12/11/2010
			// aos_assert_r(idx >= 0, false);
			// aos_assert_r(iilidx >= 0, false);
			if (idx < 0 || iilidx < 0)
			{
				OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return false;
			}
		}

		subiil = getSubiilByIndex(iilidx, rdata);
		if (!subiil)
		{
			OmnAlarm << "subiil null: " << iilidx << ":" << idx << enderr;
			idx = -5;
			iilidx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return false;
		}

		while(subiil)
		{
			// if we found the doc, return
			iilidx = subiil->getIILIdx();
			if (subiil == this)
			{
				rslt = subiil->nextDocidSinglePriv(idx, reverse, opr, value, docid, isunique, rdata);
			}
			else
			{
				rslt = subiil->nextDocidSubSafe(idx, reverse, opr, value, docid, isunique, rdata);
			}

			if (docid != AOS_INVDID)
			{
				// found it
				return true;
			}
			// Chen Ding, 12/11/2010
			// !!!!!!!!!!!!!!!! It should use the condition to retrieve the next IIL.
			i64 tmpidx = iilidx;
			if (reverse)
			{
				tmpidx--;
			}
			else
			{
				tmpidx++;
			}

			if (tmpidx < 0 || tmpidx >= mNumSubiils)
			{
				// No more to search
				iilidx = -5;
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return true;
			}
			iilidx = getSubIILIndex(tmpidx, value, opr, !reverse);
			if (iilidx == -5)
			{
				// No more to search
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return true;
			}

			// subiil = subiil->getNextSubIIL(!reverse);
			subiil = getSubiilByIndex(iilidx, rdata);

			// if we found the next subiil, we need to reset 
			// idx to -10 to search from the beginning
			if (subiil)
			{
				idx = -10;
			}
		}

		// Chen Ding, 12/11/2010
		// 'subiil' is always null. No need to check it.
		// if (!subiil)
		// {
			// This means there are no entries for the query
			docid = AOS_INVDID;
			isunique = true;
			idx = -5;
			iilidx = -5;
			return true;
		// }

		// Found the sub-iil. Search for it. There are three possibilities:
		// return true;
	}
	
	// The IIL is not segmented
	iilidx = 0;
	rslt = nextDocidSinglePriv(idx, reverse, opr, value, docid, isunique, rdata);
	// Chen Ding, 12/12/2010
	if (idx == -5)
	{
		iilidx = -5;
	}
	return rslt;
}


u64     
AosIILU64::nextDocIdSafe2(
		i64 &idx,
		i64 &iilidx,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	u64 docid = AOS_INVDID;

	if (!(idx >= 0 || idx == -10))
	{
		OmnAlarm << "idx incorrect: " << idx << enderr;
		return AOS_INVDID;
	}

	AOSLOCK(mLock);
	if (!isParentIIL())
	{
		docid = nextDocIdPriv2(idx,iilidx,reverse);
		AOSUNLOCK(mLock);
		return docid;
	}
	
	if (!(iilidx >= 0 || iilidx == -10))
	{
		OmnAlarm << "iilidx incorrect: " << iilidx << enderr;
		AOSUNLOCK(mLock);
		return AOS_INVDID;
	}

	if (iilidx > mNumSubiils -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		AOSUNLOCK(mLock);
		return AOS_INVDID;
	}

	if (isParentIIL())
	{
		aos_assert_rb(mNumSubiils > 0, mLock, AOS_INVDID);
		AosIILU64Ptr subiil;
		//1. get the subiil
		if (iilidx == -10)
		{
			if (reverse)
			{
				iilidx = mNumSubiils -1;
			}
			else
			{
				iilidx = 0;
			}
		}
		//2. get the docid
		while (iilidx >= 0 && iilidx <= mNumSubiils - 1)
		{
			subiil = getSubiilByIndex(iilidx, rdata);
			aos_assert_rb(subiil, mLock, true);
			if (subiil == this)
			{
				docid = subiil->nextDocIdPriv2(idx,iilidx,reverse);
			}
			else
			{
				docid = subiil->nextDocIdSafe2(idx,iilidx,reverse, rdata);
			}
				
			if (docid != AOS_INVDID)break;
			// not found this time
			if (reverse)
			{
				iilidx --;
			}
			else
			{
				iilidx ++;
			}
			// reset idx to -10
			idx = -10;
		}
		if (docid == AOS_INVDID)
		{
			idx = -5;
			iilidx = -5;
		}
	}
	else
	{
		// single IIL
		docid = nextDocIdPriv2(idx,iilidx,reverse);
	}
	AOSUNLOCK(mLock);
	return docid;
}


bool		
AosIILU64::nextDocidSafe(
		i64 &idx, 
		i64 &iilidx,
		const bool reverse, 
		const AosOpr opr1,
		const u64 &value1,
		const AosOpr opr2,
		const u64 &value2,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata) 
{
	// This is the same as the previous function except that the
	// query is expressed as a range: 
	// 		[value1, value2]
	//
	// opr1 should be either '>' or '>=' and opr2 '<' or '<=', which
	// means:
	// 			value1 < mValues[i] < value2
	// On in other word, it checks whether the current value is
	// in the range. If yes, advance the index and return the docid.
	// Otherwise, find the first one that is in the range. If not
	// found, set idx = -1. Otherwise, returns the docid.
	//
	// Performance Analysis:
	// This should be the same as the previous one.
	//
	// Note that it is possible (at least in the current implementations),
	// 'mNumDocs' is larger than 'mMemCap' because at the time of 
	// loading the contents, the IIL is too big (we currently have not
	// implemented splitting IILs yet).
	//
	// If this instance is a root iil, 'rootiil' is null. Otherwise, 
	// it is a subiil and it should be called from its root iil. 
	// 'rootiil' should not be null.
	aos_assert_r(mNumDocs <= mMemCap, false);
	AOSLOCK(mLock);
	if ((opr1 != eAosOpr_gt && opr1 != eAosOpr_ge) ||
		(opr2 != eAosOpr_lt && opr2 != eAosOpr_le))
	{
		AOSUNLOCK(mLock);	
		OmnAlarm << "Operator not allowed: " << opr1 << enderr;
		docid = AOS_INVDID;
		isunique = true;
		return false;
	}

	// Check whether it is a root iil and it is segmented
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		// If 'iilidx' is within [0, mNumSubiils-1], 
		// use it.
		AosIILU64Ptr subiil;
		if (idx == -10)
		{
			// This is a new call. Find the sub-iil using either 
			// [opr1, value1] for normal order or [opr2, value2]
			// for the reverse order. 
			subiil = getSubiil(reverse, opr1, value1, opr2, value2, rdata);
		}
		else
		{
			// This is a subsequent call. Use 'iilidx' to locate
			// the sub-iil.
			subiil = getSubiilByIndex(iilidx, rdata);
		}

		if (!subiil)
		{
			// This means there are no entries for the query
			AOSUNLOCK(mLock);
			idx = -5;
			iilidx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return true;
		}

		// Found the sub-iil. Search for it. There are three possibilities:
		// 1. Errors found
		// 2. The doc does not exist in the IIL (nomore == true)
		// 3. The doc was not found the sub-iil but possible in the
		//    subsequent sub-iil (nomore == false)
		bool rslt;
		if (subiil.getPtr() == this)
		{
			rslt = nextDocidSinglePriv(idx, reverse, opr1, value1, opr2, value2, docid, isunique, rdata);
		}
		else
		{
			rslt = subiil->nextDocidSubSafe(idx, reverse, opr1, value1, opr2, value2, docid, isunique, rdata);
		}

		AOSUNLOCK(mLock);
		return rslt;
	}
	
	// The IIL is not segmented
	iilidx = 0;
	bool rslt = nextDocidSinglePriv(idx, reverse, opr1, value1, opr2, value2, docid, isunique, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


i64
AosIILU64::getTotalSafe(const u64 &value, const AosOpr opr)
{
	// It returns the number of docids that match the condition:
	// 		mValues[i] opr value
	// If opr is '<', for instance, it returns the number of docids
	// that are:
	// 		mValues[i] < value
	//
	// Performance Analysis:
	// Since values are sorted, the search is log(n), which is fast
	// enough.
	//
	aos_assert_r(mNumDocs <= mMemCap, false);
	AOSLOCK(mLock);

	i64 total = 0;
	if (isParentIIL())
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
				 if (valueMatch(mMinVals[i], opr, value))
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
				 if (valueMatch(value, opr, mMaxVals[i]))
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
				 if (mMaxVals[i] >= value)
				 {
					 // add total of subiil i
					 if (i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);

					 for (i64 j=i+1; j<mNumSubiils; j++)
					 {
						 if (mMaxVals[j] == value)
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
				 if ((mMinVals[i] <= value) &&
				 	 (mMaxVals[i] >= value))
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

	total = getTotalPriv(value, opr);
	AOSUNLOCK(mLock);
	return total;
}


i64 
AosIILU64::getTotalPriv(const u64 &value, const AosOpr opr)
{
	if (mNumDocs == 0)
	{
		return 0;
	}

	i64 startidx = 0;
	i64 endidx = 0;
	i64 num = 0;
	switch (opr)
	{
	case eAosOpr_le:
		 // mValues[i] <= value
		 endidx = lastLE(value);
		 if (endidx == -1) return 0;
		 return endidx+1;

	case eAosOpr_lt:
		 // mValues[i] < value
		 endidx = lastLT(value);
		 if (endidx == -1) return 0;
		 return endidx+1;

	case eAosOpr_gt:
		 // value < mValues[i]
		 // startidx = (mValues[0].compare1(value) > 0)?0:firstGT(0, value);
		 startidx = ((mValues[0] > value))?0:firstGT(0, value);
		 if (startidx == -1) return 0;
		 return mNumDocs - startidx;

	case eAosOpr_ge:
		 // startidx = (mValues[0].compare1(value) >= 0)?0:firstGE(0, value);
		 startidx = ((mValues[0] >= value))?0:firstGE(0, value);
		 endidx = mNumDocs-1;
		 if (startidx == -1) return 0;
		 return mNumDocs - startidx;

	case eAosOpr_eq:
		 startidx = firstEQ(0, value);
		 if (startidx < 0)
		 {
			 return 0;
		 }

		 endidx = lastEQ(startidx, value);
		 aos_assert_r(endidx >= startidx && endidx < mNumDocs, 0);
		 return endidx - startidx + 1;

	case eAosOpr_ne:
		 startidx = firstEQ(0, value);
		 if (startidx < 0) 
		 {
			 num = mNumDocs;
			 return num;
		 }

		 endidx = lastEQ(startidx, value);
		 aos_assert_r(endidx >= startidx && endidx < mNumDocs, 0);
		 num = mNumDocs - (endidx - startidx + 1);
		 return num;

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
AosIILU64::modifyDocSafe(
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
	aos_assert_r(mNumDocs <= mMemCap, false);
	AOSLOCK(mLock);
	bool rslt = removeDocPriv(oldvalue, docid, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed to remove: " << docid << ":" << oldvalue << ":"
			<< newvalue << enderr;
	}
	rslt = addDocPriv(newvalue, docid, value_unique, docid_unique, rdata);
	aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);

	if (!rslt)
	{
		OmnAlarm << "Failed to add: " << docid << ":" << oldvalue 
			<< ":" << newvalue << enderr;
	}
	return rslt;
}


bool		
AosIILU64::checkDocSafe(
		const AosOpr opr,
		const u64 &value,
		const u64 &docid,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AOSLOCK(mLock);
	rslt = checkDocPriv(opr, value, docid, keepSearch, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool		
AosIILU64::checkDocSafe(
		const AosOpr opr,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	bool keepSearch = false;
	AOSLOCK(mLock);
	if (!isParentIIL())
	{
		rslt = checkDocPriv(opr, value, docid, keepSearch, rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	// Chen Ding, Bug1217
	// AosIILU64Ptr subiil = getSubiil(false, opr, value);	
	AosIILU64Ptr subiil = getSubIIL(-10, value, opr, true, rdata);
	if (!subiil)
	{
		AOSUNLOCK(mLock);
		return false;
	}

	keepSearch = true;
	i64 subIILIdx = subiil->getIILIdx();
	aos_assert_r(subIILIdx >= 0, false);
	while(keepSearch)
	{
		if (subiil == this)
		{
			rslt = subiil->checkDocPriv(opr, value, docid, keepSearch, rdata);
		}
		else
		{
			rslt = subiil->checkDocSafe(opr, value, docid, keepSearch, rdata);
		}
		// already found
		if (rslt)
		{
			AOSUNLOCK(mLock);
			return true;
		}
		// do not need to keep searching
		if (!keepSearch)
		{
			AOSUNLOCK(mLock);
			return false;
		}
		// find to the last subiil
		aos_assert_r(mNumSubiils > 0, false);
		if (subIILIdx >= mNumSubiils-1)
		{
			AOSUNLOCK(mLock);
			return false;
		}
		subIILIdx ++;
		subiil = getSubiilByIndex(subIILIdx, rdata);
	}
	AOSUNLOCK(mLock);
	return	false; 
}


bool		
AosIILU64::checkDocPriv(
		const AosOpr opr,
		const u64 &value,
		const u64 &docid,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	if (mNumDocs == 0) 
	{
		keepSearch = true;
		return false;
	}

	aos_assert_r(mNumDocs <= mMemCap, false);
	if (opr == eAosOpr_an)
	{
// shawnpro		bool rslt = getCompIIL()->docExistSafe(docid, rdata);
		OmnNotImplementedYet;

		bool rslt = false;
		keepSearch = (mValues[mNumDocs-1] == value);
		return rslt;
	}

	i64 startidx = 0; 
	i64 endidx = mNumDocs-1; 
	u64 did;
	aos_assert_r(firstDoc1(startidx, false, opr, value, did), false);
	aos_assert_r(firstDoc1(endidx, true, opr, value, did), false);

	if (startidx < 0) 
	{
		keepSearch = false;
		return false;
	}

	aos_assert_r(startidx >= 0 && endidx >= startidx, false);

	for (i64 i=startidx; i<=endidx; i++)
	{
		if (mDocids[i] == docid)
		{
			return true;
		}
	}
	keepSearch = (endidx == (mNumDocs -1));
	return false;
}


//////////////////////////////////////////////////////////////////////////
//					Private Functions                                   //
//  IMPORTANT:                                                          //
//  1. All private functions shall NEVER lock the class                 //
//  2. All private functions shall NEVER call thread-safe functions,    // 
//     directly or indirectly.                                          //
//////////////////////////////////////////////////////////////////////////
bool
AosIILU64::removeDocPriv(
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	bool keepSearch = true;
	bool rslt = false;
	// Check whether it is a root iil and it is segmented
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		// If 'iilidx' is within [0, mNumSubiils-1], 
		// use it.
		AosIILU64Ptr subiil;
		subiil = getSubiil(value, rdata);
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
			if (rslt)
			{
				// Ketty 2013/01/15
				// comp not support yet.
				// remove from compiil
				//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
				//aos_assert_r(compiil, false);				
				//bool rslt = compiil->removeDocSafe(value,docid,rdata);
				//returnCompIIL(compiil,rdata);
				//aos_assert_r(rslt, false);

				return true;
			}
			if (!keepSearch) break;
			// get next subiil
			subiil = subiil->getNextSubIIL(true, rdata);
		}
		// found to the last subiil
		// Chen Ding, 12/07/2010
		// No alarms will be raised. The caller is responsible for raising alarms
		// OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
		return false;
	}
	
	// The IIL is not segmented
	rslt = removeDocDirectPriv(value, docid, keepSearch, rdata);
	// Chen Ding, 12/07/2010
	// No alarms will be raised. The caller is responsible for raising alarms
	// if (!rslt) OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
	if (rslt)
	{
		// Ketty 2013/01/15
		// comp not support yet.
		// remove from compiil
		//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->removeDocSafe(value, docid, rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	return rslt;
}


bool		
AosIILU64::nextDocidSinglePriv(
		i64 &idx, 
		const bool reverse,
		const AosOpr opr,
		u64 &value,
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
	//
	// Chen Ding, 12/11/2010
	docid = AOS_INVDID;		// Chen Ding, 06/26/2011
	if (mNumDocs == 0)
	{
	 	// No entries in the list.
	 	idx = -5;
	 	docid = AOS_INVDID;
		isunique = true;
	 	return true;
	}

	// Chen Ding, 12/11/2010
	if (!mValues)
	{
	 	OmnAlarm << "mValues null" << enderr;
	 	idx = -5;
	 	docid = AOS_INVDID;
		isunique = true;
	 	return false;
	}

	// Chen Ding, 12/11/2010
	if (idx < 0)
	{
		if (idx != -10)
		{
			OmnAlarm << "idx: " << idx << enderr;
			idx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return false;
		}
	}
	else
	{
		if (idx >= mNumDocs)
		{
			OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
			idx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return false;
		}
	}

	// Chen Ding, 12/14/2010
	bool startnew = false;
	if (reverse)
	{
		if (idx == -10)
		{
			idx = mNumDocs -1;
			startnew = true;
		}
		else
		{	
			// Chen Ding, 12/11/2010
			// Not possible. 
			if (idx < 0)
			{
				OmnAlarm << "idx: " << idx << enderr;
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return false;
			}

			if (idx == 0)
			{
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return true;
			}
			idx --;
		}
	}
	else // forward
	{
		if (idx == -10)
		{
			idx = 0;
			startnew = true;
		}
		else
		{
			// Chen Ding, 12/11/2010
			// Not possible
			if (idx < 0)
			{
				OmnAlarm << "idx: " << idx << enderr;
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return false;
			}
	
			if (idx >= mNumDocs -1)
			{
				// is the last one, we can't find the next
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				return true;
			}
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

	// Chen Ding, 12/11/2010
	// Moved to the beginning
	// aos_assert_r(mValues, false);
	// if (!mValues)
	// {
	// 	OmnAlarm << "mValues null" << enderr;
	// 	idx = -5;
	// 	docid = AOS_INVDID;
	// 	return false;
	// }

	int rr = (reverse)?-1:1;
	bool rslt;
	if (startnew)
	{
		switch (opr)
		{
		case eAosOpr_gt:
		 	 rslt = nextDocidGT(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_ge:
		 	 rslt = nextDocidGE(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_eq:
		 	 rslt = nextDocidEQ(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_lt:
		 	 rslt = nextDocidLT(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_le:
		 	 rslt = nextDocidLE(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_ne:
		 	 rslt = nextDocidNE(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_an:
		 	 rslt = nextDocidAN2(idx, rr, value, docid);
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
		// Chen Ding, 12/14/2010
		// It is not a new. Just check the current entry.
		switch (opr)
		{
		case eAosOpr_gt:
			 if (mValues[idx] > value)
			 {
				 docid = mDocids[idx];
				 value = mValues[idx];
				 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_ge:
			 if (mValues[idx] >= value)
			 {
				 docid = mDocids[idx];
				 value = mValues[idx];
				 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_eq:
			 if (mValues[idx] == value)
			 {
				 docid = mDocids[idx];
				 value = mValues[idx];
				 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;
	
		case eAosOpr_lt:
			 if (mValues[idx] < value)
			 {
				 docid = mDocids[idx];
				 value = mValues[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_le:
			 if (mValues[idx] <= value)
			 {
				 docid = mDocids[idx];
				 value = mValues[idx];
			 	 checkDocidUnique(true, idx, docid, isunique, rdata);
				 return true;
			 }
			 idx = -5;
			 docid = AOS_INVDID;
			 isunique = true;
			 return true;

		case eAosOpr_ne:
			 rslt = nextDocidNE(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_an:
			 docid = mDocids[idx];
			 value = mValues[idx];
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
	isunique = false;
	return false;
}


bool
AosIILU64::checkDocidUnique(
		const bool rslt, 
		const i64 &idx, 
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	if (!rslt || idx < 0) 
	{
		docid = AOS_INVDID;
		isunique = true;
		return rslt;
	}

	if (idx >= mNumDocs)
	{
		OmnAlarm << "Something is incorrect: " << idx << ":" << mNumDocs << enderr;
		docid = AOS_INVDID;
		isunique = true;
		return true;
	}

	isunique = true;
	bool hassame = false;

	//1. compare previous value
	if (idx > 0)
	{
		hassame = (mValues[idx] == mValues[idx-1]);
	}
	else if (mIILIdx > 0)
	{
		AosIILU64Ptr subiil = getNextSubIIL(false, rdata);
		if (subiil.notNull())
		{
			hassame = (mValues[idx] == subiil->getLastValue());
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
		hassame = (mValues[idx] == mValues[idx+1]);
	}
	else if (mIILIdx >= 0)
	{
		AosIILU64Ptr subiil = getNextSubIIL(true, rdata);
		if (subiil.notNull())
		{
			hassame = (mValues[idx] == subiil->getFirstValue());
		}
	}

	if (hassame)
	{
		isunique = false;
	}

	return true;
}


bool
AosIILU64::nextDocidAN2(
		i64 &idx, 
		const int reverse, 
		u64 &value,
		u64 &docid)
{
	// It matches any entry. It simply retrieves the current one, 
	// if any, and advances 'idx'. 
	if (idx >= 0 && idx < mNumDocs)
	{
		value = mValues[idx];
		docid = mDocids[idx];
		return true;
	}

	docid = AOS_INVDID;
	idx = -5;
	return true;
}


bool
AosIILU64::nextDocidAN(
		i64 &idx, 
		const int reverse, 
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
AosIILU64::nextDocidGT(
		i64 &idx, 
		const int reverse, 
		const u64 &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) > 0)

	if (mValues[idx] > value)
	{
		docid = mDocids[idx];
		return true;
	}

	if (reverse == -1)
	{
		idx = firstGTRev(idx, value);
	}
	else
	{
		idx = firstGT(idx, value);
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
AosIILU64::nextDocidGE(
		i64 &idx, 
		const int reverse, 
		const u64 &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)

	if (mValues[idx] >= value)
	{
		docid = mDocids[idx];
		return true;
	}

	if (reverse == -1)
	{
		idx = firstGERev(idx, value);
	}
	else
	{
		idx = firstGE(idx, value);
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
AosIILU64::nextDocidLT(
		i64 &idx, 
		const int reverse, 
		const u64 &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] < value
	// if (mValues[idx].compare1(value) < 0)
	// Chen Ding, 08/11/2010
	if (mValues[idx] < value)
	{
		docid = mDocids[idx];
		return true;
	}

	idx = (reverse == 1)?firstLT(idx, value):firstLTRev(idx, value);
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
AosIILU64::nextDocidLE(
		i64 &idx, 
		const int reverse, 
		const u64 &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) <= 0)
	// Chen Ding, 08/11/2010
	if (mValues[idx] <= value)
	{
		docid = mDocids[idx];
		return true;
	}

	idx = (reverse == 1)?firstLE(idx, value):firstLERev(idx, value);
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
AosIILU64::nextDocidEQ(
		i64 &idx, 
		const int reverse, 
		const u64 &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	//
	// If success, 'docid' holds the docid found and 'idx' points to
	// the next element (IMPORTANT: It does not point to the current one).
	// if (mValues[idx] == value)
	// Chen Ding, 08/11/2010
	if (mValues[idx] == value)
	{
		docid = mDocids[idx];
		return true;
	}

	idx = (reverse == 1)?firstEQ(idx, value):firstEQRev(idx, value);
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
AosIILU64::nextDocidNE(
		i64 &idx, 
		const int reverse, 
		const u64 &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the:
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	// i64 rslt = value.compare1(mValues[idx]);
	// Chen Ding, 08/11/2010
	if (mValues[idx] != value)
	{
		docid = mDocids[idx];
		return true;
	}

	idx = (reverse == 1)?firstNE(idx, value):firstNERev(idx, value);
	if (idx == -1)
	{
		// No more
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}

	docid = mDocids[idx];
	return true;
}


i64
AosIILU64::firstEQ(const i64 &idx, const u64 &value)
{
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) > 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return -1;

	if (mValues[idx] == value) return idx;
	if (mValues[idx] > value) return -1;
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] < value) return -1;

	// Now, we are sure mValues[idx] <= value <= mValues[mNumDocs-1].  
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right || left + 1 == right) 
	{
		// Chen Ding, 08/11/2010
		if (mValues[right] == value) return right;
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] < value)
		{
			// mValues[nn] < value
			left = nn+1;
		}
		else if (mValues[nn] > value)
		{
			// mValues[nn] > value
			right = nn-1;
		}
		else // mValues[nn] == value
		{
			// mValues[nn] == value. Since it is to find the first match, 
			// it needs to check whether 'left == right'. If not, need 
			// to continue the searching
			right = nn;
			if (left == right) return right;
			if (left + 1 == right) 
			{
				// Chen Ding, 08/11/2010
				if (mValues[left] == value) return left;
				return right;
			}
		}
	}

	// Chen Ding, 08/11/2010
	if (left == right && (mValues[left] == value)) return left;
	return -1;
}


i64
AosIILU64::firstEQRev(const i64 &idx, const u64 &value)
{
	// IMPORTANT!!!!!!!!!!
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	// if (mValues[idx].compare1(value) < 0) return -1;
	// if (mValues[0].compare1(value) > 0) return -1;

	// If mValues[idx] == value, return idx
	if (mValues[idx] == value) return idx;

	// If mValues[idx] is less than 'value', it is not possible
	if (mValues[idx] < value) return -1;

	// If mValues[0] is greater than value, it is not possible
	if (mValues[0] > value) return -1;
		
	// Now, we are sure mValues[0] <= value <= mValues[idx].  
	i64 left = 0; 
	i64 right = idx;
	if (left == right) return -1;
	if (left + 1 == right) 
	{
		// Chen Ding, 08/11/2010
		if (mValues[right] == value) return right;
		if (mValues[left] == value) return left;
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;

		if (mValues[nn] < value)
		{
			// mValues[nn] < value
			left = nn + 1;
		}
		else if (mValues[nn] > value)
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
				// Chen Ding, 08/11/2010
				if (mValues[right] == value) return right;
				return left;
			}
		}
	}

	// Chen Ding, 08/11/2010
	if (left == right && (mValues[left] == value)) return left;
	return -1;
}
	

i64
AosIILU64::firstNERev(const i64 &idx, const u64 &value)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry 
	// where mValues[idx] != value.
	// If not found, idx = -1.
	// if (mValues[0].compare1(value) == 0) return -1;
	// Chen Ding, 08/11/2010
	if (mValues[0] == value) return -1;
		
	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] != value) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstNE(const i64 &idx, const u64 &value)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry whose
	// value is not equal to 'value'. If not found, idx = -1.
	// if (mValues[mNumDocs-1] == value) return -1;
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] == value) return -1;
		
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] != value) right=nn; else left = nn;
		if (left == right-1) return right;
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::lastEQ(const i64 &idx, const u64 &value) 
{
	// IMPORTANT!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from idx, it searches for the last one that:
	// 		value == mValues[idx]
	// If not found, it returns -1;

	// if (mValues[mNumDocs-1] == value) return mNumDocs-1;
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] == value) return mNumDocs-1;

	// Now, mValues[idx] <= value < mValues[mNumDocs-1]
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] == value) left = nn; else right = nn;
		if (left == right-1) return left;
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::lastLT(const u64 &value) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] < value
	// If not found, it returns -1;
	// if (mValues[0].compare1(value) >= 0) return -1;
	// if (mValues[mNumDocs-1].compare1(value) < 0) return mNumDocs-1;
	// Chen Ding, 08/11/2010
	if (mValues[0] >= value) return -1;
	if (mValues[mNumDocs-1] < value) return mNumDocs-1;

	// Now, mValues[0] < value <= mValues[mNumDocs-1]
	i64 left = 0; 
	i64 right = mNumDocs-1;
	// Now, 'left' points to a good one
	// 'right' points to a bad one. 
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] < value) left = nn; else right = nn;
		if (left == right-1) return left;
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::lastLE(const u64 &value) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] <= value
	// If not found, it returns -1;
	// Chen Ding, 08/11/2010
	if (mValues[0]> value) return -1;
	if (mValues[mNumDocs-1] <= value) return mNumDocs-1;

	// Now, mValues[0] <= value < mValues[mNumDocs-1]
	i64 left = 0; 
	i64 right = mNumDocs-1;
	// Now, 'left' points to a good one
	// 'right' points to a bad one. 
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		if (mValues[nn] <= value) left = nn; else right = nn;
		if (left == right-1) return left;
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstLERev(const i64 &idx, const u64 &value) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] <= value
	// Chen Ding, 08/11/2010
	if (mValues[0] > value) return -1;

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] <= value) left = nn; else right = nn;
		if (left == right-1) return left;
	}
	OmnShouldNeverComeHere;
	return -1;
}

i64
AosIILU64::firstLTRev(const i64 &idx, const u64 &value) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	// Chen Ding, 08/11/2010
	if (mValues[0] >= value) return -1;

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] < value) left = nn; else right = nn;
		if (left == right-1) return left;
	}
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstGTRev(const i64 &idx, const u64 &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] <= value) return -1;
	else return (mNumDocs -1);
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstGERev(const i64 &idx, const u64 &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] < value) return -1;
	else return (mNumDocs -1);
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstLT(const i64 &idx, const u64 &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	// Chen Ding, 08/11/2010
	if (mValues[idx] >= value) return -1;
	else return 0;
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstLE(const i64 &idx, const u64 &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	// Chen Ding, 08/11/2010
	if (mValues[idx] > value) return -1;
	else return 0;
	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstGT(const i64 &idx, const u64 &value)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] <= value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] > value
	
	// 1. Check whether the last one is < 'value'
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] <= value) return -1;

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] <= value) left = nn; else right = nn;
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILU64::firstGE(const i64 &idx, const u64 &value)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	// Chen Ding, 08/11/2010
	if (mValues[mNumDocs-1] < value) return -1;

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		// Chen Ding, 08/11/2010
		if (mValues[nn] < value) left = nn; else right = nn;
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool 	
AosIILU64::insertBefore(
		const i64 &nn, 
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
AosIILU64::insertAfter(
		const i64 &nn, 
		const u64 &docid, 
		const u64 &value)
{
	// It inserts an entry after 'nn'. 
	if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(nn >= 0, false);

	// Change mValues
	memmove(&mDocids[nn+2], &mDocids[nn+1], sizeof(u64) * (mNumDocs - nn - 1));
	memmove(&mValues[nn+2], &mValues[nn+1], sizeof(u64) * (mNumDocs - nn - 1));
	mDocids[nn+1] = docid;
	mValues[nn+1] = value;
	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


u64
AosIILU64::removeDocByIdxPriv(const i64 &theidx, const AosRundataPtr &rdata)
{
	// If 'theidx' is -1, it removes the last entry. Otherwise, it 
	// removes 'theidx'-th entry. 
	if (mNumDocs <= 0) return AOS_INVDID;
	i64 idx = (theidx == -1)?mNumDocs-1:theidx;
	aos_assert_r(idx >= 0 && idx < mNumDocs, AOS_INVDID);

	u64 docid = mDocids[idx];

	// Ketty 2013/01/15
	// comp not support yet.
	//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);				
	//bool rslt = compiil->removeDocSafe(mValues[idx],docid,rdata);
	//returnCompIIL(compiil,rdata);
	//aos_assert_r(rslt, false);

	// The docid to be removed is at 'idx'. Need to remove
	// it from mDocids. 

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
	if (isParentIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx, updateMax, updateMin);
	}

	mIsDirty = true;
	return docid;
}


bool 		
AosIILU64::removeDocDirectPriv(
		const u64 &value, 
		const u64 &docid,
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	// It assumes the doc is in this list and the lock has been locked
	// It uses 'value' to locate the start and end idx. It then loops
	// through [startidx, endidx] to check for 'docid'. 
	//
	// Performance Improvements:
	// Entries whose values are the same should be sorted based on 
	// docids. When searching, use the binary search.
	
	aos_assert_r(mNumDocs > 0, false);
	i64 startidx = 0;
	i64 endidx = mNumDocs -1;
	i64 idx = -1;
	u64 did;

	bool rslt = nextDocidEQ(startidx,1,value,did);
	if (!rslt)
	{
		keepSearch = false;
		return false;
	}
	if (startidx < 0 || startidx >= mNumDocs)
	{
		// Chen Ding, 12/07/2010
		// OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
		return false;
	}

	rslt = nextDocidEQ(endidx, -1, value, did);
	// Chen Ding, 12/07/2010
	// aos_assert_r(rslt,false);	
	if (!rslt) 
	{
		return false;
	}

//	aos_assert_r(nextDocidPriv(startidx, startiilidx,
//			false, eAosOpr_eq, value, did, isunique), false);
	
	idx = startidx;
	while(idx <= endidx)
	{
		if (docid == mDocids[idx]) 
		{
			goto removeEntry;
		}
		else if (mDocids[idx] > docid)
		{
			// not found
			break;
		}
		idx ++;
	}

	if (idx > mNumDocs -1)
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
	aos_assert_r(idx >= 0, false);
	// Change mValues
	if (idx < mNumDocs-1)
	{
		memmove(&mDocids[idx], &mDocids[idx+1], sizeof(u64) * (mNumDocs - idx - 1));
		memmove(&mValues[idx], &mValues[idx+1], sizeof(u64) * (mNumDocs - idx - 1));
	}
	decNumDocsNotSafe();

	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	mIsDirty = true;
	
	if (isParentIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);

		if (mNumDocs < mMinIILSize)
		{
			rootiil->mergeSubiilPriv(mIILIdx, rdata);
		}
	}
	
	return true;
}


bool
AosIILU64::firstDoc1(
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

	switch (opr)
	{
	case eAosOpr_gt:
		 return nextDocidGT(idx, reverse_flag, value, docid);

	case eAosOpr_ge:
		 return nextDocidGE(idx, reverse_flag, value, docid);

	case eAosOpr_eq:
		 return nextDocidEQ(idx, reverse_flag, value, docid);

	case eAosOpr_lt:
		 return nextDocidLT(idx, reverse_flag, value, docid);

	case eAosOpr_le:
		 return nextDocidLE(idx, reverse_flag, value, docid);

	case eAosOpr_ne:
		 return nextDocidNE(idx, reverse_flag, value, docid);

	case eAosOpr_an:
		 return nextDocidAN(idx, reverse_flag, value, docid);

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 docid = AOS_INVDID;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
	static AosIILObjPtr lsTestIIL = OmnNew AosIILU64();
	static OmnMutex lsLock;

	lsLock.lock();
	bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
	lsLock.unlock();
	return rslt;
}


bool
AosIILU64::addDocPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{

	bool rslt = false;
	bool keepSearch = false;
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		AosIILU64Ptr subiil;
		i64 iilidx = getSubiilIndex(value);
		if (iilidx == -5) iilidx = 0;
		aos_assert_r(iilidx >= 0, false);
		while (iilidx < mNumSubiils)
		{
			subiil = getSubiilByIndex(iilidx, rdata);
			aos_assert_r(subiil, false);
			keepSearch  = false;
			
			if (subiil.getPtr() == this)
			{
				rslt = addDocDirectPriv(value, docid, value_unique, docid_unique, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->addDocDirectSafe(value, docid, value_unique, docid_unique, keepSearch, rdata);
			}
			
			//if (rslt & !keepSearch)
			if (rslt) //ken 2011/12/06
			{
				// Ketty 2013/01/15
				// comp not support yet.
				// add to compiil
				//AosIILCompU64Ptr compiil = retrieveCompIIL(rdata);
				//aos_assert_r(compiil, false);				
				//bool rslt = compiil->addDocSafe(value,docid,false,false,rdata);
				//returnCompIIL(compiil,rdata);
				//aos_assert_r(rslt, false);
			}
			if (!keepSearch)
			{
				return rslt;
			}
			iilidx++;
		}
		
		OmnAlarm << "fail to add value " << value << enderr;
		return false;
	}

	aos_assert_r(isSingleIIL(),false);
	rslt = addDocDirectPriv(value, docid, value_unique, docid_unique, keepSearch, rdata);
	if (rslt)
	{
		// Ketty 2013/01/15
		// comp not support yet.
		// add to compiil
		//AosIILCompU64Ptr compiil = retrieveCompIIL(rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->addDocSafe(value, docid, false, false, rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	return rslt;
}


bool
AosIILU64::addDocDirectPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	keepSearch = false;
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// In this case, the doc may be inserted in the next subiil.
	
	// Check whether we need to split
	bool rslt = false;
	i64 numBefore = mNumDocs;

	i64 idx = 0;
	i64 left = 0; 
	i64 right = mNumDocs-1;
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
		if (mValues[idx] == value)
		{
			// Chen Ding, 10/24/2010
			if (value_unique)
			{
				/*
				// The value must be unique. This fails the operation. 
				// if 'value_unique' is AOSIIL_FORCE_UNIQUE, it will 
				// override the entry. Otherwise, it returns an error. 
				if (value_unique == AOSIIL_FORCE_UNIQUE)
				{
					// Force unique
					mDocids[idx] = docid;
					return true;
				}
				*/
				OmnAlarm << "Value already exist: " << value << ":" << docid
					<< ":" << mDocids[idx] << enderr;
				return false;
			}

			// Need to insert the doc based on docid
			rslt = insertDocPriv(
						idx, value, docid, value_unique, docid_unique, keepSearch);
			aos_assert_r(rslt, false);
			if (!keepSearch)
			{
				goto finished;
			}
			return true;
		}

		// Chen Ding, 08/11/2010
		if (mValues[idx] < value) 
		{
			if (idx == mNumDocs-1)
			{
				// Append the docid
				if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
				mDocids[mNumDocs] = docid;
				// Change mValues
				mValues[mNumDocs] = value;
				incNumDocsNotSafe();
				idx = mNumDocs-1;
				goto finished;
			}

			// Chen Ding, 08/11/2010
			// Check whether mValues[idx+1] >= value
			if (mValues[idx+1] > value)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (mValues[idx+1] == value)
			{
				if (value_unique)
				{
					OmnAlarm << "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx] << enderr;
					return false;
				}
				idx++;
				rslt = insertDocPriv(
							idx, value, docid, value_unique, docid_unique, keepSearch);
				aos_assert_r(rslt, false);
				if (!keepSearch) goto finished;
				return true;
			}
			left = idx+1;
	 	}
		else
		{
			// value == mValues[idx]
			if (idx == 0)
			{
				// Insert into the front
				insertBefore(idx, docid, value);
				goto finished;
			}

			// Chen Ding, 08/11/2010
			if (mValues[idx-1] < value)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				idx--;
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (mValues[idx-1] == value)
			{
				if (value_unique)
				{
					OmnAlarm << "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx] << enderr;
					return false;
				}
				idx --;
				rslt = insertDocPriv(idx, value, docid, value_unique, docid_unique, keepSearch);
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
	// Ketty RlbTest tmp.	for line:2832 IILTmp
	aos_assert_r(numBefore + 1 == mNumDocs, false);
	if (!isSingleIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);

		AosIILU64 *rootiil = dynamic_cast<AosIILU64 *>(mRootIIL.getPtr());
		rootiil->updateIndexData(mIILIdx, updateMax, updateMin);
	}

	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILObjPtr subiil;
		rslt = splitListPriv(subiil, rdata);
	}
	sanityTestForSubiils();
	return true;
}


bool
AosIILU64::sanityTestForSubiils()
{
//	for(i64 i = 0; i < mNumDocs - 1; i++)
//	{
//if (mValues[i] > mValues[i+1])
//{
//	i64 a = 0;
//	a++;
//}
//		aos_assert_r(mValues[i] <= mValues[i+1],false);
//	}
	return true;
}



bool
AosIILU64::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 

	i64 newsize = mNumDocs + mExtraDocids;
	u64 *mem = OmnNew u64[newsize];
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
AosIILU64::prepareMemoryForReloading()
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
	
	// Grow 'mValues' now
	if (mNumDocs < mMemCap)
	{
		return true;
	}

	i64 newsize = mNumDocs + mExtraDocids;
	u64 *mem = OmnNew u64[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(u64) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(u64 *) * mMemCap);
		OmnDelete [] mValues;
		mValues = 0;
	}

	mValues = mem;
	bool rslt = expandDocidMem();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILU64::splitListPriv(AosIILObjPtr &subiil, const AosRundataPtr &rdata)
{
	if (mRootIIL && mRootIIL->getNumSubiils() > mMaxSubIILs)
	{
		OmnAlarm << "too many subiils : " << mRootIIL->getNumSubiils()<< enderr;
	}

	// It cuts the current list in two. The first half stays 
	// with the current list and the second half is in a new
	// IIL list.

	// 1. Check whether it can create more subiils
	aos_assert_r(mNumSubiils < mMaxSubIILs-1, false);

	// 2. Create the subiil
	subiil = AosIILMgrSelf->createSubIILSafe(mIILID, mSiteid, mSnapShotId, eAosIILType_U64, mIsPersis, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->getIILType() == eAosIILType_U64, false);
	AosIILU64Ptr newsubiil =  dynamic_cast<AosIILU64 *>(subiil.getPtr());

	newsubiil->setSnapShotId(mSnapShotId);
	
	// 3. Set mRootIIL
	if (mRootIIL.isNull())
	{
		mRootIIL = this;
		mIILIdx = 0;
	}
	subiil->setRootIIL(mRootIIL);
	subiil->setIILIdx(mIILIdx + 1);

	// 4. Initialize the new subiil
	i64 startidx = mNumDocs / 2;
	const i64 len = mNumDocs - startidx;
	aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);
	AosIILU64 *rootiil = dynamic_cast<AosIILU64 *>(mRootIIL.getPtr());
	aos_assert_r(mIILIdx >= 0, false);

	bool rslt = newsubiil->initSubiil(&mDocids[startidx], &mValues[startidx], len, mIILIdx+1, rootiil);
	aos_assert_r(rslt, false);

	// 5. Shrink this IIL
	memset(&mValues[startidx], 0, sizeof(u64) * len);
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	mIsDirty = true;
	aos_assert_r(mNumDocs < mMaxNormIILSize, false);

	// 6. Add the subiil
	AosIILU64Ptr thisptr(this, false);
	aos_assert_r(rootiil->addSubiil(thisptr, newsubiil), false);
	newsubiil->setDirty(true);

	AosIILU64Ptr r = dynamic_cast<AosIILU64 *>(mRootIIL.getPtr());
//	r->splitSanityCheck();
	return true;
}


bool
AosIILU64::splitSanityCheck()
{
	for(i64 i = 0;i < mNumSubiils;i++)
	{
		if (mSubiils[i])
		{
			aos_assert_r(mMaxVals[i] == mSubiils[i]->getMaxValue(),false);
			aos_assert_r(mMinVals[i] == mSubiils[i]->getMinValue(),false);
		}
	}
	return true;
}

// Description:
// This function assumes mValues[idx] == value. For entries whose
// values are the same, they are sorted based on docid. 
// 1. If mDocids[idx] == docid, the doc is inserted @idx.
// 2. If mDocids[idx] < docid, the doc is inserted after idx
// 3. Otherwise, the doc is inserted before idx.
//
// IMPORTANT!!!!!!!!!!
// It assumes mValues[idx] == value
bool
AosIILU64::insertDocPriv(
		i64 &idx, 
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		bool &keepSearch) 
{
	// It inserts the doc at position 'idx'. If:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// the doc may need to be inserted in the next subiil.
	aos_assert_r(mNumDocs > 0, false);
	i64 left = firstEQ(0, value);
	i64 right = firstEQRev(mNumDocs-1, value);

	aos_assert_r(left >= 0 && left < mNumDocs, false);
	aos_assert_r(right >= left && right < mNumDocs, false);

	if (docid == mDocids[left])
	{
		if (docid_unique)
		{
			// Ketty RlbTest tmp.
			//return true;
			OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			return false;
		}

		aos_assert_r(insertBefore(left, docid, value), false);
		idx = left;
		return true;
	}

	if (docid < mDocids[left])
	{
		aos_assert_r(insertBefore(left, docid, value), false);
		idx = left;
		return true;
	}

	if (docid == mDocids[right])
	{
		if (docid_unique)
		{
			OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			return false;
		}
		aos_assert_r(insertAfter(right, docid, value), false);
		idx = right + 1;
		return true;
	}

	if (docid > mDocids[right])
	{
		// It is possible that the entry should be inserted
		// in the next subiil. Need to verify:
		// 1. Whether there is a subiil
		// 2. Whether the first value in the subiil equals 'value'
		// 3. Whether the first docid is bigger than 'docid'
		// aos_assert_r(mRootIIL->getNumSubiils() > 0,false );
		// aos_assert_r(mIILIdx >= 0,false );
		if (right == mNumDocs-1 && mIILIdx >= 0 && mRootIIL &&
			mRootIIL->getNumSubiils() > 0 && mIILIdx != mRootIIL->getNumSubiils() -1)
		{
			keepSearch = true;
			return true;
		}

		aos_assert_r(insertAfter(right, docid, value), false);
		idx = right + 1;
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
			idx = left + 1;
			return true;
		}

		i64 nn = (left + right) >> 1;
		if (mDocids[nn] == docid)
		{
			if (docid_unique)
			{
//				OmnAlarm << "Not unique: " << docid 
//					<< ":" << value << ":" << mIILID << enderr;
				return false;
			}
			aos_assert_r(insertBefore(nn, docid, value), false);
			idx = nn;
			return true;
		}
		else if (mDocids[nn] < docid) left = nn;
		else right = nn;
	}

	OmnShouldNeverComeHere;
	return false;
}

	
bool
AosIILU64::listSanityCheckPriv() 
{
	// The class should have been locked. It checks all the values are 
	// sorted based on the value. For entries whose values are the
	// same, they are sorted based on docids.
	for (i64 i=1; i<mNumDocs; i++)
	{
		aos_assert_r((mValues[i-1] <= mValues[i]), false);
		if ((mValues[i-1] == mValues[i]))
		{
			aos_assert_r(mDocids[i-1] <= mDocids[i], false);
		}
	}
	return true;
}


AosBuffPtr 
AosIILU64::getBodyBuffProtected() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILU64SanityCheck(this), 0);
	
	i64 expect_size = mNumDocs * 60; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	// Save mDocids
	// Ken Lee, 2013/04/18
	//for (i64 i=0; i<mNumDocs; i++)
	//{
	//	//*buff << mDocids[i];
	//	buff->setU64(mDocids[i]);
	//}
	bool rslt = buff->setU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, 0);

	// Save Subiil Index Data, if needed
	//*buff << mNumSubiils;
	buff->setI64(mNumSubiils);

	// save it but not use it.
	//*buff << mIILIdx;
	buff->setI64(mIILIdx);
	
	if (isParentIIL())
	{
		/*
		// Save the subiil index data
		for (i64 i=0; i<mNumSubiils; i++)
		{
			// *buff << mMinVals[i]
			//	<< mMaxVals[i]
			//	<< mIILIds[i]
			//	<< mNumEntries[i];
			//buff->setU64(mMinVals[i]);
			//buff->setU64(mMaxVals[i]);
			//buff->setU64(mIILIds[i]);
			//buff->setI64(mNumEntries[i]);
		}
		*/

		rslt = buff->setU64s(mMinVals, mNumSubiils);
		aos_assert_r(rslt, 0);

		rslt = buff->setU64s(mMaxVals, mNumSubiils);
		aos_assert_r(rslt, 0);
		
		rslt = buff->setU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, 0);

		rslt = buff->setI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, 0);
	}

	// Save Values
	aos_assert_r(mNumDocs <= mMaxNormIILSize, 0);

	/*
	for (i64 i=0; i<mNumDocs; i++)
	{
		// *buff << mValues[i];
		//buff->setU64(mValues[i]);
	}
	*/

	rslt = buff->setU64s(mValues, mNumDocs);
	aos_assert_r(rslt, 0);
	
	// now we do not mix the CompIIL content into strIIL buff
	// save compIIL ID ( instead of saving it's content)
	return buff;
}


bool
AosIILU64::setContentsProtected(
		AosBuffPtr &buff, 
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
	
	aos_assert_r(mNumDocs <= mMaxNormIILSize, false);
	
	bool rslt = prepareMemoryForReloading();
	aos_assert_r(rslt, false);

	// 1. Retrieve mDocids
	aos_assert_r(setDocidContents(buff), false);

	// 2. Retrieve Subiil Index Data, if needed
	i64 num_subiils = buff->getI64(0);
	if (num_subiils < 0 || num_subiils > mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << num_subiils << enderr;
		mNumSubiils = 0;
		return false;
	}
	mNumSubiils = num_subiils;

	// This place used to save IILIdx, now we get it out but not use it
	if (mNumSubiils > 1)
	{
		mIILIdx = 0;
	}
	else
	{
		mIILIdx = -1;
	}
	buff->getI64(0);
	
	if (mNumSubiils > 1)
	{
		mRootIIL = this;
	}

//	mIILIdx = buff->getI64(0);
//	if (mIILIdx == 0 && mNumSubiils > 1)
//	{
//		mRootIIL = this;
//	}

	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
		mSubiils[mIILIdx] = this;

		/*
		for (i64 i=0; i<mNumSubiils; i++)
		{
			//mMinVals[i] = buff->getU64(0);

			//mMaxVals[i] = buff->getU64(0);
			
            //mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			mNumEntries[i] = buff->getI64(0);
			aos_assert_r(mNumEntries[i] >= 0, false);
		}*/
		
		rslt = buff->getU64s(mMinVals, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mMaxVals, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, false);
	}

	/*
	for (i64 i=0; i<mNumDocs; i++)
	{
		//mValues[i] = buff->getU64(0);
	}*/

	rslt = buff->getU64s(mValues, mNumDocs);
	aos_assert_r(rslt, false);

	rslt =	AosIILU64SanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool		
AosIILU64::nextDocidPriv(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr1,
		const u64 &value1,
		const AosOpr opr2,
		const u64 &value2,
		u64 &docid, 
		bool &isunique) 
{
	// It searches the next entry starting at 'idx' (including)
	// that meets:
	// 		value1 <= value <= value2
	// If found, 'idx' is the index of the entry found; 
	// 'docid' holds the docid. 
	// 'nomore' is set to false if there is no more. Otherwise,
	// it should set to true.
	if (idx < 0 || idx >= mNumDocs || mNumDocs == 0) 
	{
		// It may happen.
		idx = -5;
		docid = AOS_INVDID;
		isunique = true;
		return true;
	}

	if (reverse)
	{
		// In the reverse order. Moving from 'idx' (including)
		// until it hits the right edge (i.e., value2 <= mValues[idx]). 
		// If it moves out of the left edge (i.e., mValues[idx] <= value1)
		// it means there is no more.
		if (valueMatch(mValues[idx], opr2, value2))
		{
			// It enters the right edge. Check whether it is out of
			// the left edge.
			if (valueMatch(mValues[idx], opr1, value1))
			{
				docid = mDocids[idx];
				goto found_wrap;
			}

			// It is out of the left edge, which means there 
			// is no more.
			idx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return true;
		}
		
		// Need to find the first mValues[...] that is <= value2
		aos_assert_r(findFirstRev(idx, opr2, value2), false);
		if (idx < -4)
		{
			// Since the caller did subiil selection first, if
			// a subiil fails finding the doc, we are sure the doc
			// is not in the entire iil. 
			idx = -5;
			docid = AOS_INVDID;
			isunique = true;
			return true;
		}

		if (valueMatch(mValues[idx], opr1, value1))
		{
			// Found one
			docid = mDocids[idx];
			goto found_wrap;
		}

		// No more
		idx = -5;
		docid = AOS_INVDID;
		return true;
	}
	
	// Normal order. Moving from 'idx' (including)
	// until it hits the left edge (i.e., value1 >= mValues[idx]). 
	// If it moves out of the right edge (i.e., value2 >= mValues[idx])
	// it means there is no more.
	if (valueMatch(mValues[idx], opr1, value1))
	{
		// It enters the left edge. Check whether it is out of
		// the right edge.
		if (valueMatch(mValues[idx], opr2, value2))
		{
			// Found it
			docid = mDocids[idx];
			goto found_wrap;
		}

		// It is out of the right edge, which means there 
		// is no more.
		idx = -5;
		docid = AOS_INVDID;
		isunique = true;
		return true;
	}
	
	// Find the first mValues[...] that is >= value1
	aos_assert_r(findFirst(idx, opr1, value1), false);
	if (idx < -4)
	{
		// No more
		idx = -5;
		docid = AOS_INVDID;
		isunique = true;
		return true;
	}
	OmnNotImplementedYet;
// seems not right lxx
	if (valueMatch(mValues[idx], opr2, value2))
	{
		// Found one
		docid = mDocids[idx];
		goto found_wrap;
	}

	// NO more
	idx = -5;
	isunique = true;
	return true;

found_wrap:
	// Not implemented yet
	// if ((idx > 0 && value == mValues[idx-1]) ||
	// 	(idx < mNumDocs-1 && value == mValues[idx+1]))
	// {
	// 	isunique = false;
	// 	return true;
	// }
	isunique = true;
	return true;
}


AosIILU64Ptr
AosIILU64::getSubiil(
		const bool reverse,
		const AosOpr opr1,
		const u64 &value1, 
		const AosOpr opr2,
		const u64 &value2,
		const AosRundataPtr &rdata) 
{
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
		for (i64 i=mNumSubiils-1; i>=0; i--)
		{
			if (mNumEntries[i]>0 && valueMatch(mMinVals[i], opr2, value2))
			{
				// Found the sub-iil
				return getSubiilByIndex(i, rdata);
			}
		}
		
		// Not found
		return 0;
	}

	// It is normal order
	for (i64 i=0; i<mNumSubiils; i++)
	{
		if ( mNumEntries[i]>0 && valueMatch(value1, opr1, mMaxVals[i]))
		{
			// Found the sub-iil
			return getSubiilByIndex(i, rdata);
		}
	}
	return 0;
}


AosIILU64Ptr
AosIILU64::getSubiilByIndex(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	// It retrieves the idx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil. 
	//
	// The function assumes 'mLock' should have been locked.
	aos_assert_r(isParentIIL() || isSingleIIL(), 0);
	if (mRootIIL)
	{
		aos_assert_r(mRootIIL.getPtr() == this, 0);
	}

	if (idx == 0) 
	{
		// Chen Ding, 06/04/2011
		// aos_assert_r(mRootIIL,0);
		// if (!mRootIIL) mRootIIL = this;
		return this;
	}

	aos_assert_r(idx > 0 && idx < mNumSubiils, 0);
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILU64Ptr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[idx]) return mSubiils[idx];

	// Retrieve an IIL from IILMgr
	aos_assert_r(mIILIds[idx], 0);

	// Chen Ding, 05/05/2011
	// bool status;
	// AosIILObjPtr iil = AosIILMgrSelf->loadIILByIDSafe(mIILIds[idx],
	// 		AOS_INVWID, eAosIILType_U64, 0, 0, status);
	AosIILType type= eAosIILType_U64;
	AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
		mIILIds[idx], mSiteid, mSnapShotId, type, rdata);
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[idx] << ":" << type << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_U64)
	{
		OmnAlarm << "Not a string IIL: " 
			<< iil->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}
	mSubiils[idx] = dynamic_cast<AosIILU64 *>(iil.getPtr());
	mSubiils[idx]->mRootIIL = this;
	mSubiils[idx]->mIILIdx = idx;

	aos_assert_r(mSubiils[0], 0);
	return mSubiils[idx];
}


bool 
AosIILU64::initSubiil(
		u64 *docids,
		u64 *values,
		const i64 &numDocs,
		const i64 &subiilidx,
		const AosIILU64Ptr &rootiil)
{
	aos_assert_r(mIILType == eAosIILType_U64, false);
	aos_assert_r(rootiil, false);

	mRootIIL = rootiil.getPtr();
    mWordId = AOS_INVWID;
    mNumDocs = numDocs; 
	// ModifyNumDocs
	aos_assert_r(mNumDocs < mMaxNormIILSize, false);

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


    mFlag = 0;
    mHitcount = 0;
	//mTotalDocs = 0;
    //mIILDiskSize = 0;	// Ketty 2012/11/15
    //mOffset = 0;		// Ketty 2012/11/15
    //mSeqno = 0;		// Ketty 2012/11/15
	mCompressedSize = 0;

    mNumSubiils = 0;
    mIILIdx = 0;


	// Prepare the memory
	bool rslt = prepareMemoryForReloading();
	aos_assert_r(rslt, false);
	memcpy(mValues, values, sizeof(u64) * mNumDocs);
	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool
AosIILU64::addSubiil(
		const AosIILU64Ptr &crtsubiil,
		const AosIILU64Ptr &newsubiil)
{
	// It inserts the new subiil 'newsubiil' after 'crtsubiil'. This function
	// must be called by the root IIL.
	aos_assert_r(mIILIdx == 0, false);
	aos_assert_r(mNumSubiils < mMaxSubIILs, false);
//	aos_assert_r(mNumSubiils >= 1, false);
	if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
	i64 iilidx = crtsubiil->getIILIdx();
	aos_assert_r(iilidx >= 0, false);

	i64 num_to_move = mNumSubiils - iilidx - 1;
	// 1. Insert the new subiil in mSubiils[]
	if (iilidx < mNumSubiils-1)
	{
		//ken 2012/11/12
		// this place must use for loop
		//memmove(&mSubiils[iilidx+2], &mSubiils[iilidx+1], sizeof(AosIILU64Ptr) * (mNumSubiils-iilidx-1));
		//memset(&mSubiils[iilidx+1], 0, sizeof(AosIILU64Ptr));
        for (int i=num_to_move; i>0; i--)
		{
			mSubiils[iilidx+i+1] = mSubiils[iilidx+i];
		}
		mSubiils[iilidx+1] = 0;

        memmove(&mMinVals[iilidx+2], &mMinVals[iilidx+1], sizeof(u64) * (mNumSubiils-iilidx-1));
		mMinVals[iilidx+1] = 0;

        memmove(&mMaxVals[iilidx+2], &mMaxVals[iilidx+1], sizeof(u64) * (mNumSubiils-iilidx-1));
		mMaxVals[iilidx+1] = 0;

        memmove(&mIILIds[iilidx+2], &mIILIds[iilidx+1], sizeof(u64) * (mNumSubiils-iilidx-1));
        mIILIds[iilidx+1] = 0;

        memmove(&mNumEntries[iilidx+2], &mNumEntries[iilidx+1], sizeof(i64) * (mNumSubiils-iilidx-1));
        mNumEntries[iilidx+1] = 0;

		for (i64 i=iilidx+2; i<mNumSubiils; i++)
		{
			if (mSubiils[i].notNull())
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
	for (i64 i = iilidx; i < mNumSubiils; i++)
	{
		if (mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}

	updateIndexData(iilidx, true, true);
	updateIndexData(iilidx+1, true, true);

	mIsDirty = true;
	crtsubiil->setDirty(true);
	newsubiil->setDirty(true);

	aos_assert_r(mSubiils[0], false);
	return true;
}


i64
AosIILU64::getSubiilIndex(const u64 &value)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, -1);
	aos_assert_r(isParentIIL(), -1);
	aos_assert_r(mNumSubiils >= 2, -1);
    i64 left = 0;
    i64 right = mNumSubiils - 1;
    i64 cur = 0;
    if (right == left)return right;
    bool leftnotfit = false;
	while(1)
    {
        if (right == left +1)
        {
			if (leftnotfit) return right;
            // if left fit, return left
            if (mMaxVals[left] >= value)
            {
                return left;
            }
            else
            {
                return right;
            }
            // else return right
        }

        cur = (right - left)/2 + left;

        if (mMaxVals[cur] >= value)
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


AosIILU64Ptr
AosIILU64::getSubiil(const u64 &value, const AosRundataPtr &rdata)
{
	i64 index = getSubiilIndex(value);
	aos_assert_r(index >= 0 && index <= mNumSubiils - 1, 0);
	return getSubiilByIndex(index, rdata);
}


u64
AosIILU64::getMinDocid() const
{
	aos_assert_r(false, AOS_INVDID);
	aos_assert_r(mNumDocs > 0, AOS_INVDID);
	OmnNotImplementedYet;
	return 0;
}


u64
AosIILU64::getMaxDocid() const
{
	aos_assert_r(false, AOS_INVDID);
	aos_assert_r(mNumDocs > 0, AOS_INVDID);
	OmnNotImplementedYet;
	return 0;
}


u64
AosIILU64::getMinValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mValues[0];
}


u64
AosIILU64::getMaxValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mValues[mNumDocs-1];
}


bool
AosIILU64::checkMemory() const
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
AosIILU64::updateIndexData(const i64 &idx,const bool changeMax,const bool changeMin)
{
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	setDirty(true);
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < mNumSubiils, false);
	aos_assert_r(mSubiils[idx], false);

	AosIILU64Ptr subiil = mSubiils[idx];
	mNumEntries[idx] = subiil->getNumDocs();
	if (changeMax)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		mMaxVals[idx] = subiil->getMaxValue();
	}

	if (changeMin)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		mMinVals[idx] = subiil->getMinValue();
	}

	return true;
}


bool
AosIILU64::createSubiilIndex()
{
	aos_assert_r(!mSubiils, false);

	mSubiils = OmnNew AosIILU64Ptr[mMaxSubIILs];
	aos_assert_r(mSubiils, false);

	mMinVals = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mMinVals, false);
	memset(mMinVals, 0, sizeof(u64) * mMaxSubIILs);

	mMaxVals = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mMaxVals, false);
	memset(mMaxVals, 0, sizeof(u64) * mMaxSubIILs);

	mIILIds = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * mMaxSubIILs);

	mNumEntries = OmnNew i64[mMaxSubIILs];
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(i64) * mMaxSubIILs);

	return true;
}


i64
AosIILU64::getDocidsSafe(
		const AosOpr opr,
		u64 &value,
		u64 *docids, 
		const i64 &arraysize,
		const AosRundataPtr &rdata)
{
	// This function retrieves all the docs that meet the condition.
	i64 numdocfound = 0;
	i64 curPos = 0;
	AOSLOCK(mLock);
	if (!isParentIIL())
	{
		numdocfound = getDocidsPriv(opr, value, docids, arraysize, curPos, rdata);
		AOSUNLOCK(mLock);
		return numdocfound;
	}

	// it is root iil
	// 1. get the first sub iil
	i64 firstSubiilIdx = getSubIILIndex(-10, value, opr, true);
	// 2. get the last sub iil
	i64 lastSubiilIdx = getSubIILIndex(-10, value, opr, false);
	// 3. get the docids from the first to the last
	i64 counter = 0;
	AosIILU64Ptr curSubiil = 0;
	for(i64 i = firstSubiilIdx;i <= lastSubiilIdx;i++)
	{
		//1. get the subiil
		curSubiil = getSubiilByIndex(i, rdata);
		aos_assert_rb(curSubiil,mLock,0);
		counter = curSubiil->getDocidsPriv(opr, value, docids, arraysize, curPos, rdata);
		numdocfound += counter;
	}

	AOSUNLOCK(mLock);
	return numdocfound;
}


// Chen Ding, 12/12/2010
// This function searches for the first subiil that 
// passes the condition [value, opr]. If 'startidx'
// is -10, it starts from the beginning. Otherwise, 
// it is the subiil from which the search starts. 
i64
AosIILU64::getSubIILIndex(
		const i64 &startidx, 
		const u64 &value,
		const AosOpr opr,
		const bool fromHead)
{
	aos_assert_r(isParentIIL(), -1);
	aos_assert_r(mNumSubiils >= 2, -1);
	if (fromHead)
	{
		// Chen Ding, 12/12/2010
		i64 idx = startidx;
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

		if (idx >= mNumSubiils)
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
			 for(i64 i=idx; i<mNumSubiils; i++)	// Chen Ding, 12/12/2010
			 {
			  	 if (mMaxVals[i] > value) return i;
			 }
			 return -5;

		case eAosOpr_ge:
			 // Find the first one whose value <= max. If it runs out of 
			 // the loop, there is no subiil for it.
			 for(i64 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (mMaxVals[i] >= value) return i;
			 }
			 return -5;
				 
		case eAosOpr_eq:
			 // Find the first one that max >= value. If it runs out of
			 // the loop, there is no subiil for it.
			 for (i64 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (mMaxVals[i] >= value)
				 {
					 // If value < min, not possible
					 if (value < mMinVals[i]) return -5;
					 return i;
				 }
			 }
			 return -5;

		case eAosOpr_lt: 
			 // If the first min < value, return 0. Otherwise, return -5.
			 if (mMinVals[idx] < value) return idx;
			 return -5;
			
		case eAosOpr_le:
			 // If the first min <= value, return 0. Otherwise, return -5.
			 if (mMinVals[idx] <= value) return idx;
			 return -5;
	
		case eAosOpr_ne:
			 // Chen Ding, 12/12/2010
			 // if (valueMatch(mMinVals[0], eAosOpr_ne, value)) return 0;
			 for (i64 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (mMinVals[i] != value) return i;
				 if (mMaxVals[i] != value) return i;
			 }
			 return -5;

		case eAosOpr_an:
			 return idx;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 return -5;
		}

		OmnShouldNeverComeHere;
		return -5;
	}

	// Chen Ding, 12/12/2010
	i64 idx = startidx;
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

	if (idx >= mNumSubiils)
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
		 if (mMaxVals[idx] <= value) return -5;
		 return idx;

	case eAosOpr_ge:
		 if (mMaxVals[idx] < value) return -5;
		 return idx;

	case eAosOpr_eq:
		 for (i64 i=idx; i>=0; i--)
		 {
			 // Find the first one whose min <= value. It then 
			 // checks whether max >= value, return that one. 
			 if (mMinVals[i] <= value)
			 {
				 if (mMaxVals[i] >= value) return i;
				 return -5;
			 }
		 }
		 return -5;

	case eAosOpr_lt:
		 for (i64 i=idx; i>=0; i--)
		 {
			// Find the first one whose min < 'value'. 
			if (mMinVals[i] < value) return i;
		 }
		 return -5;

	case eAosOpr_le:
		 for (i64 i=idx; i>=0; i--)
		 {
		  	 if (mMinVals[i] <= value) return i;
		 }
		 return -5;

	case eAosOpr_ne:
		 for(i64 i=idx; i>= 0; i--)		// Chen Ding, 12/12/2010
		 {
			 if (mMinVals[i] != value) return i;
			 if (mMaxVals[i] != value) return i;
		 }
		 return -5;

	case eAosOpr_an:
		 return idx;

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 return -5;
	}

	OmnShouldNeverComeHere;
	return -5;
}


i64
AosIILU64::getDocidsPriv(
		const AosOpr opr,
		u64 &value,
		u64 *docids,
		const i64 &arraysize,
		i64 &curPos,
		const AosRundataPtr &rdata)
{
	i64 numfound = 0;
	i64 startidx = -10;
	i64 startiilidx = -10;
	i64 endidx = -10;
	i64 endiilidx = -10;
	u64 docid;
	bool isunique = true;
	bool rslt = nextDocidPriv(startidx, startiilidx, false, opr, value, docid, isunique, rdata);

	aos_assert_r(rslt, -1);

	if (startidx < 0)
	{
		return 0;
	}

	endidx = -10;
	rslt = nextDocidPriv(endidx, endiilidx,true, opr, value, docid, isunique, rdata);
	aos_assert_r(rslt, 0);
	aos_assert_r((endiilidx > startiilidx)||(endidx >= startidx), 0);
	for (i64 i=startidx; i<=endidx; i++)
	{
		if (curPos >= arraysize -1) break;
		docids[curPos++] = mDocids[i];
		numfound ++;
	}

	return numfound;
}


bool
AosIILU64::saveSubIILToLocalFileSafe(const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (isParentIIL())
	{
		// this iil is the root iil of the list
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			if (mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToLocalFileSafe(rdata);
				aos_assert_r(rslt,false);
			}
		}
	}
	return true;
}


bool
AosIILU64::returnSubIILsPriv(
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
	returned = true;
	if (!isParentIIL())
	{
		return true;
	}

	AosIILU64Ptr subiil = 0;
	bool subIILReturned = false;
	bool rslt = false;

	// return all sub IIL except itself
	for (i64 i = 1; i < mNumSubiils; i++)
	{
		subiil = mSubiils[i];
		aos_assert_r(subiil != this, false);

		//ken 2011/9/2
		if (subiil.notNull() && !subiil->isDirty())
		{
			rslt = AosIILMgrSelf->returnIILPublic(
				subiil, subIILReturned, returnHeader, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}


AosIILU64Ptr
AosIILU64::getNextSubIIL(const bool forward, const AosRundataPtr &rdata)
{
	bool valid = false;
	if (mRootIIL.isNull())
	{
		return 0;
	}

	if (forward)
	{
		aos_assert_r(mIILIdx >= 0,0);
		aos_assert_r(mRootIIL->getNumSubiils() > 0,0);
		valid = mIILIdx < (mRootIIL->getNumSubiils() - 1);
	}
	else
	{
		valid = mIILIdx > 0;
	}

	if (!valid)
	{
		return 0;
	}

	aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64,0);
	AosIILU64Ptr rootIIL = (AosIILU64*)mRootIIL.getPtr();

	if (forward)
	{
		return rootIIL->getSubiilByIndex(mIILIdx+1, rdata);
	}
	else
	{
		return rootIIL->getSubiilByIndex(mIILIdx-1, rdata);
	}
}


bool
AosIILU64::resetSubIILInfo(const AosRundataPtr &rdata)
{
	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
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

	if (mSubiils)
	{
		OmnDelete [] mSubiils;
		mSubiils = 0;
	}
	mRootIIL = 0;
	mNumSubiils = 0;
	mIILIdx = 0;
	return true;
}


bool
AosIILU64::resetSpec()
{
	return true;
}


u64
AosIILU64::getDocIdSafe1(i64 &idx,i64 &iilidx, const AosRundataPtr &rdata)
{
	u64 did = 0;
	AOSLOCK(mLock);
	did = getDocIdPriv1(idx,iilidx, rdata);
	AOSUNLOCK(mLock);
	return did;
}


u64
AosIILU64::getDocIdPriv1(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata)
{
	if (!isParentIIL())
	{
		u64 docid = getDocIdPriv(idx,rdata);
		return docid;
	}

	// is rootiil
	AosIILU64Ptr subiil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iilidx >= 0,0);
	u64 docid = 0;
	aos_assert_r(mNumSubiils >= 2, 0);
	while(subiil)
	{
		if (subiil == this)docid = subiil->getDocIdPriv(idx,rdata);
		else docid = subiil->getDocIdSafe(idx,rdata);

		if (docid > 0)
		{
			return docid;
		}
		if (iilidx >= mNumSubiils -1)
		{
			return 0;
		}
		iilidx ++;
		idx = 0;
		subiil = getSubiilByIndex(iilidx, rdata);
	}
	return 0;
}


bool
AosIILU64::verifyDuplicatedEntriesPriv(const AosRundataPtr &rdata)
{
	OmnScreen << "\nVerify Duplicated Entries: Total subiils: " << mNumSubiils << endl;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		AosIILU64Ptr crt_subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(crt_subiil, false);
		i64 crt_numdocs = crt_subiil->mNumDocs;

		u64* crt_values = crt_subiil->mValues;

		for (i64 mm=0; mm<crt_numdocs; mm++)
		{
			u64 vv = crt_values[mm];

			u64* crt_docids = crt_subiil->mDocids;

			for (i64 kk=mm+1; kk<crt_numdocs; kk++)
			{
				if (vv == crt_values[kk])
				//if (strcmp(vv, crt_values[kk]) == 0)
				{
					OmnAlarm << "Duplicated entries are found, value: "
						<< vv << ", idx1: " << mm << ", idx2: " << kk 
						<< ", docid1: " << crt_docids[mm] << ", docid2: " << crt_docids[kk] 
						<< ",num_docs: " << crt_numdocs 
						<< ", Subiil: " << i << enderr;
				}
			}

			// Did not find duplicated entries in the current subiil. 
			// Check all the subsequent subiils
			for (i64 aa=i+1; aa<mNumSubiils; aa++)
			{
				AosIILU64Ptr next_subiil = getSubiilByIndex(aa, rdata);
				u64 *values = next_subiil->mValues;
				i64 entries = next_subiil->mNumDocs;
				for (i64 bb=0; bb<entries; bb++)
				{
					if (vv == values[bb])
					{
						OmnAlarm << "Duplicated entries are found: " 
							<< vv << ":" << aa << ":" << bb << ":" << entries << enderr;
					}
				}
			}
		}
	}
	OmnScreen << "\nFinished checking: " << mNumSubiils << endl;
	return true;
}


u64
AosIILU64::getRandomValuePriv(u64 &docid, const AosRundataPtr &rdata)
{
	docid = 0;
	u64 *values;
	u64 *docids;
	i64 num_docs;
	if (mNumSubiils == 0)
	{
		values = mValues;
		docids = mDocids;
		num_docs = mNumDocs;
	}
	else
	{
		i64 iilidx = rand() % mNumSubiils;
		AosIILU64Ptr subiil = getSubiilByIndex(iilidx, rdata);
		aos_assert_r(subiil, 0);
		values = subiil->mValues;
		docids = subiil->mDocids;
		num_docs = subiil->mNumDocs;
	}

	if (num_docs == 0) return 0;
	i64 nn = rand() % num_docs;
	docid = docids[nn];
	return values[nn];
}


// Chen Ding New
bool
AosIILU64::copyData(
        u64 values[AOSSENG_TORTURER_MAX_DOCS],
        u64 *docids,
        const i64 &buff_len,
        i64 &num_docs,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (!isParentIIL())
	{
		aos_assert_r(isSingleIIL(),false);
		return copyDataSingle(values,docids,buff_len,num_docs);
	}
	// rootiil
	i64 doc_limit = buff_len;
	i64 doc_copied;
    i64 idx = 0;
    for (i64 ss=0; ss<mNumSubiils; ss++)
    {
        AosIILU64Ptr subiil = getSubiilByIndex(ss, rdata);

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
AosIILU64::copyDataSingle(
        u64 values[AOSSENG_TORTURER_MAX_DOCS],
        u64 *docids,
        const i64 &buff_len,
        i64 &num_docs)
{
    i64 idx = 0;
    for (i64 mm=0; mm<mNumDocs; mm++)
    {
		aos_assert_r(idx < buff_len, false);
        values[idx] = mValues[mm];
        docids[idx] = mDocids[mm];
        idx++;
    }
    num_docs = idx;
    return true;
}


bool
AosIILU64::querySinglePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	if (opr == eAosOpr_ne)
	{
		return querySingleNEPriv(query_rslt,query_bitmap,value, rdata);
	}
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	u64 value2 = value;
	bool isunique = true;
	
	if (opr == eAosOpr_an)
	{
		idx_start = 0;
	}
	else
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value2, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		if (idx_start < 0)
		{
			return true;
		}
	}
	
	if (opr == eAosOpr_an)
	{
		idx_end = mNumDocs - 1;
	}
	else
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value2, docid, isunique, rdata);
		aos_assert_r(rslt,false);
	}
	
	// count num of docs
	// i64 num_docs = idx_end - idx_start + 1;
	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	// copy data
	rslt = copyDocidsPriv(query_rslt,query_bitmap,idx_start,idx_end);
	aos_assert_r(rslt,false);

//	if (query_rslt.notNull())
//	{
//		if (opr == eAosOpr_eq)
//		{
//			query_rslt->setOrdered(true);
//		}
//		else
//		{
//			query_rslt->setOrdered(false);
//		}
//	}
	return true;
}


bool
AosIILU64::queryNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;
	u64 value2 = value;
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, eAosOpr_eq, value2, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	
	if (idx_start >= 0)
	{	
		rslt = nextDocidPriv(idx_end, iilidx_end, true, eAosOpr_eq, value2, docid, isunique, rdata);
		aos_assert_r(rslt,false);
		aos_assert_r(idx_end >= 0 && iilidx_end >= 0,false);
	}

	// count num of docs
	i64 num_docs = 0;
	if (idx_start < 0)
	{
		// Copy all the list out
		for(i64 i = 0;i < mNumSubiils -1;i++)
		{
			num_docs += mNumEntries[i];
		}
	}
	else
	{
		for(i64 i = 0;i < iilidx_start;i++)
		{
			num_docs += mNumEntries[i];
		}
		num_docs += idx_start;
		num_docs += mNumEntries[iilidx_end] - idx_end - 1;
		for (i64 i = iilidx_end +1; i < mNumSubiils; i++)
		{
			num_docs += mNumEntries[i];	
		}		
	}
	
	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	// copy data
	i64 start = -10;
	i64 end = -10;
	AosIILU64Ptr subiil;
	if (idx_start < 0)
	{
		// copy all data
		for (i64 i = 0;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil.notNull(),false);
			if (i == 0)
			{
				rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
			else
			{
				rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
		}
		
	}
	else// idx_start >= 0, which means there's some data, which's value == value
	{
		aos_assert_r(idx_end >= 0,false);
		i64 start = -10;
		i64 end = -10;
		for(i64 i = 0;i <= iilidx_start;i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil.notNull(),false);
			start = 0;
			end = (i == iilidx_start)?idx_start - 1:-10;

			if (i == iilidx_start && end < start)continue;
				
			if (i == 0)
			{
				rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
			else
			{
				rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
		}			
	
		for (i64 i = iilidx_end;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil.notNull(),false);


			start = (i == iilidx_end)?idx_end+1:-10;
			end = -10;

			if (i == iilidx_end)
			{
				aos_assert_r(mNumEntries[iilidx_end] > 0,false);
			}
			if (i == iilidx_end && start > mNumEntries[iilidx_end] - 1) continue;

			if (i == 0)
			{
				rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
			else
			{
				rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
				aos_assert_r(rslt,false);
			}
		}			
	}
//	if (query_rslt.notNull())
//	{
//		query_rslt->setOrdered(false);
//	}
	return true;
}


bool
AosIILU64::querySingleNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;
	u64 value2 = value;

	if (mNumDocs <= 0)return true;
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, eAosOpr_eq, value2, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	if (idx_start >= 0)
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, eAosOpr_eq, value2, docid, isunique, rdata);
		aos_assert_r(rslt,false);
	}
	
	// count num of docs
	i64 num_docs = 0;
	if (idx_start < 0)
	{
		num_docs = mNumDocs;
	}
	else
	{
		num_docs = mNumDocs - (idx_end - idx_start + 1);
	}

	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	// copy data
	if (idx_start < 0)
	{
		rslt = copyDocidsPriv(query_rslt,query_bitmap,0,mNumDocs -1);
		aos_assert_r(rslt,false);
		num_docs = mNumDocs;
	}
	else
	{
		// first part
		if (idx_start > 0)
		{
			rslt = copyDocidsPriv(query_rslt,query_bitmap,0,idx_start - 1);
			aos_assert_r(rslt,false);
			
		}
		// second part
		if (idx_end < mNumDocs -1)
		{
			rslt = copyDocidsPriv(query_rslt,query_bitmap,idx_end + 1, mNumDocs - 1);
			aos_assert_r(rslt,false);
		}
	}
	//if (query_rslt.notNull())
	//{
	//	query_rslt->setOrdered(false);
	//}
	return true;
}


bool
AosIILU64::queryPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return querySinglePriv(query_rslt,query_bitmap,opr,value, rdata);
	}

	aos_assert_r(isParentIIL(), false);
	aos_assert_r(mNumSubiils >= 2, false);
	if (opr == eAosOpr_ne)
	{
		return queryNEPriv(query_rslt,query_bitmap,value, rdata);
	}

	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;
	u64 value2 = value;
	
	if (opr == eAosOpr_an)
	{
		idx_start = 0;
		iilidx_start = 0;
	}
	else
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value2, docid, isunique, rdata);
		if (!rslt || idx_start < 0 || iilidx_start < 0)
		{
			// This means that it did not find anything.
			if (query_rslt.notNull())
			{
				// Chen Ding, 08/02/0211
				// query_rslt->setBlockSize(0);
			}
			return true;
		}
	}
	
	aos_assert_r(iilidx_start >= 0, false);
	if (opr == eAosOpr_an)
	{
		iilidx_end = mNumSubiils -1;
		idx_end = mNumEntries[iilidx_end] -1;
	}
	else
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value2, docid, isunique, rdata);
		if (!rslt || idx_end < 0 || iilidx_end < 0)
		{
			OmnAlarm << "Internal error: " << rslt << ":"
				 << idx_end << ":" << iilidx_end << enderr;
			if (query_rslt.notNull())
			{
				// Chen Ding, 08/02/2011
				// query_rslt->setBlockSize(0);
			}
			return false;
		}
	}

	// count num of docs
	i64 num_docs = 0;
	for(i64 i = iilidx_start;i < iilidx_end;i++)
	{
		num_docs += mNumEntries[i];
	}
	num_docs += idx_end + 1;
	num_docs -= idx_start;

	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	// copy data
	AosIILU64Ptr subiil;
	i64 start = -10;
	i64 end = -10;
	for(i64 i = iilidx_start;i <= iilidx_end;i++)
	{
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		if (i == iilidx_start)
		{
			start = idx_start;
		}
		else
		{
			start = 0;
		}

		if (i == iilidx_end)
		{
			end = idx_end;
		}
		else
		{
			end = -10;
		}

		if (i == 0)
		{
			rslt = subiil->copyDocidsPriv(query_rslt,query_bitmap,start,end);
			aos_assert_r(rslt,false);
		}
		else
		{
			rslt = subiil->copyDocidsSafe(query_rslt,query_bitmap,start,end);
			aos_assert_r(rslt,false);
		}
	}
//	if (query_rslt.notNull())
//	{
//		if (opr == eAosOpr_eq)
//		{
//			query_rslt->setOrdered(true);
//		}
//		else
//		{
//			query_rslt->setOrdered(false);
//		}
//	}
	return true;
}


bool
AosIILU64::queryRangePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context,false);
	AosOpr startOpr = query_context->getOpr();
	AosOpr endOpr;
	
	if (startOpr == eAosOpr_range_ge_le)
	{
		startOpr = eAosOpr_ge;
		endOpr = eAosOpr_le;
	}
	else if (startOpr == eAosOpr_range_ge_lt)
	{
		startOpr = eAosOpr_ge;
		endOpr = eAosOpr_lt;
	}
	else if (startOpr == eAosOpr_range_gt_le)
	{
		startOpr = eAosOpr_gt;
		endOpr = eAosOpr_le;
	}
	else if (startOpr == eAosOpr_range_gt_lt)
	{
		startOpr = eAosOpr_gt;
		endOpr = eAosOpr_lt;
	}
	else
	{
		OmnAlarm << "Operation Error, not range opr:" << AosOpr_toStr(startOpr) << enderr;
	}

	aos_assert_r(startOpr == eAosOpr_gt || startOpr == eAosOpr_ge, false);
	aos_assert_r(endOpr == eAosOpr_lt || endOpr == eAosOpr_le, false);
	u64 startValue = query_context->getStrValue().toU64(0);
	u64 endValue = query_context->getStrValue2().toU64(0);

	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;

	if (isSingleIIL())
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, startOpr, startValue, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		if (idx_start < 0)
		{
			return true;
		}
	
		rslt = nextDocidPriv(idx_end, iilidx_end, true, endOpr, endValue, docid, isunique, rdata);
		aos_assert_r(rslt,false);

		// copy data
		rslt = copyDocidsPriv(query_rslt, query_bitmap, idx_start, idx_end);
		aos_assert_r(rslt,false);

		return true;
	}

	rslt = nextDocidPriv(idx_start, iilidx_start, false, startOpr, startValue, docid, isunique, rdata);
	if (!rslt || idx_start < 0 || iilidx_start < 0)
	{
		// This means that it did not find anything.
		return true;
	}
	
	aos_assert_r(iilidx_start >= 0, false);
	rslt = nextDocidPriv(idx_end, iilidx_end, true, endOpr, endValue, docid, isunique, rdata);
	if (!rslt || idx_end < 0 || iilidx_end < 0)
	{
		OmnAlarm << "Internal error: " << rslt << ":"
			 << idx_end << ":" << iilidx_end << enderr;
		return false;
	}

	// count num of docs
	i64 num_docs = 0;
	for(i64 i=iilidx_start; i<iilidx_end; i++)
	{
		num_docs += mNumEntries[i];
	}
	num_docs += idx_end + 1;
	num_docs -= idx_start;

	// copy data
	AosIILU64Ptr subiil;
	i64 startpos = -10;
	i64 endpos = -10;
	for(i64 i=iilidx_start; i<=iilidx_end; i++)
	{
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(), false);
		if (i == iilidx_start)
		{
			startpos = idx_start;
		}
		else
		{
			startpos = 0;
		}

		if (i == iilidx_end)
		{
			endpos = idx_end;
		}
		else
		{
			endpos = -10;
		}

		if (i == 0)
		{
			rslt = subiil->copyDocidsPriv(query_rslt, query_bitmap, startpos, endpos);
			aos_assert_r(rslt, false);
		}
		else
		{
			rslt = subiil->copyDocidsSafe(query_rslt, query_bitmap, startpos, endpos);
			aos_assert_r(rslt, false);
		}
	}
	
	return true;
}


bool
AosIILU64::docidSortedSanityCheck(const i64 &crtidx)
{
	u64 vv = mValues[crtidx];
	
	// 1. Find the first entry with the same value.
	i64 start_idx = crtidx;
	for (i64 i=crtidx-1; i>=0; i--)
	{
		if (mValues[i] != vv) break;
		start_idx = i;
	}

	i64 end_idx = crtidx;
	for (i64 i=crtidx+1; i<mNumDocs; i++)
	{
		if (mValues[i] != vv) break;
		end_idx = i;
	}

	u64 did = mDocids[start_idx];
	for (i64 i=start_idx+1; i<=end_idx; i++)
	{
		aos_assert_r(did <= mDocids[i], false);
		did = mDocids[i];
	}
	return true;
}


bool
AosIILU64::updateKeyedValue(
		const u64 &key,
		const bool inc_flag,
		const u64 &delta, 
		u64 &new_value,
		const AosRundataPtr &rdata)
{
	// This function updates the keyed value. If not there, the entry:
	// 			[key, delta]
	// is added. Otherwise, the value is either incremented or decremented
	// by 'delta'. 
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	u64 crt_value = 0;
	u64 vv = key;
	AOSLOCK(mLock);
	bool rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, vv, crt_value, isunique, rdata);
	if (!rslt || idx < 0)
	{
		// Did not find it. 
		rslt = addDocPriv(key, delta, true, false, rdata);
		AOSUNLOCK(mLock);
		new_value = delta;
		return rslt; 
	}

	aos_assert_rl(delta > 0, mLock, false);
	
	// Found it
	if (inc_flag) 
	{
		new_value = crt_value + delta;
	}
	else
	{
		if (crt_value < delta) 
		{
			new_value = 0;
		}
		else
		{
			new_value = crt_value - delta;
		}
	}
	
	removeDocPriv(key, crt_value, rdata);
	rslt = addDocPriv(key, new_value, true, false, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIILU64::updateKeyedValue(const u64 &key, const u64 &value, const AosRundataPtr &rdata) 
{
	// This is a hack on IILU64. 'value' is not really a value but a docid,
	// and 'docid' is not a docid but a value. It is used by counters. 
	aos_assert_r(isParentIIL()||isSingleIIL(),false);
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	u64 did = 0;
	bool rslt;
	u64 thekey = key;
	AOSLOCK(mLock);
	rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, thekey, did, isunique, rdata);
	if (!rslt || idx < 0)
	{
		// Did not find it. 
		rslt = addDocPriv(key, value, false, false, rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}

	// Found it. If its value is not the same, update it. Otherwise, do nothing.
	if (did != value)
	{
		removeDocPriv(key, did, rdata);
		addDocPriv(key, value, false, false, rdata);
	}
	
	AOSUNLOCK(mLock);
	return false; 
}


#if 0
// This is changed to setValueDocUnique(...). If this is needed in the future, 
//
bool
AosIILU64::setValueDoc(
		const u64 &key, 
		const u64 &docid) 
{
	// This function used to be updateValue2(key, docid).
	// It sets the entry [key, docid]. If the entry is 
	// already there, do nothing. Otherwise, it adds the entry. 
	//
	aos_assert_r(isParentIIL()||isSingleIIL(),false);
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique;
	u64 did = 0;
	u64 vv = key;
	bool rslt;
	AOSLOCK(mLock);
	while(1)
	{
		// Chen Ding, 06/04/2011
		// The problem is that every time this function is called, it may add
		// a new entry. It can potentially lead to entry leaks. Do not use
		// this function. If this is really needed, please re-write this 
		// function. 
		// End of Chen Ding, 06/04/2011
		//
		// This is done by looping over 'key'. If no entry is found, 
		// it adds the entry. Otherwise, it compares the docid. If
		// the docid is the same, the entry is already in the IIL. 
		// Do nothing. Otherwise, it searches for the next one. 
		rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, vv, did, isunique);
		if (!rslt || idx < 0)
		{
			// Did not find it. 
			rslt = addDocPriv(key, docid, true, false);
			AOSUNLOCK(mLock);
			return rslt; 
		}

		if (docid == did)
		{
			AOSUNLOCK(mLock);
			return true;
		}
	}

	AOSUNLOCK(mLock);
	OmnShouldNeverComeHere;
	return rslt; 
}
#endif


bool
AosIILU64::setValueDocUniqueSafe(
		const u64 &key,
		const u64 &docid,
		const bool must_same,
		const AosRundataPtr &rdata)
{
	// If the list contains [key, docid], override 'docid'. Otherwise, 
	// it adds an entry [key, docid].
	aos_assert_r(isParentIIL() || isSingleIIL(), false);
	
	AOSLOCK(mLock);
	
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	u64 did = 0;
	u64 vv = key;
	bool rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, vv, did, isunique, rdata);
	if (!rslt || idx < 0)
	{
		// Did not find it. 
		rslt = addDocPriv(key, docid, true, false, rdata);
		AOSUNLOCK(mLock);
		if (!rslt)
		{
			rdata->setError() << "Failed adding the entry: " 
				<< key << ":" << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
		return rslt; 
	}
	
	if (!isunique)
	{
		rdata->setError() << "Value not unique: " << key;
		// OmnAlarm << rdata->getErrmsg() << enderr;
		AOSUNLOCK(mLock);
		return true;
		//return false;
	}

	if (docid == did)
	{
		AOSUNLOCK(mLock);
		return true;
	}
	
	if (must_same)
	{
		AOSUNLOCK(mLock);
		aos_assert_r(docid == did, false);
		return true;
	}

	AosIILU64Ptr subiil = getSubiilByIndex(iilidx, rdata);
	aos_assert_rb(subiil, mLock, false);
	aos_assert_rb(subiil->mValues && idx < subiil->mNumDocs, mLock, false);
	aos_assert_rb(subiil->mValues[idx] == vv, mLock, false);
	aos_assert_rb(subiil->mDocids, mLock, false);
	subiil->mDocids[idx] = docid;
	subiil->setDirty(true);
	
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
	//rslt = compiil->addDocSafe(key, docid, false, false, false, rdata);
	//returnCompIIL(compiil, rdata);
	
	AOSUNLOCK(mLock);
	return true; 
}


// Chen Ding, 03/23/2012
// This function should be identical to AosIILStr::incrementDocidSafe(...). 
// bool
// AosIILU64::incrementDocid(
// 		const u64 &key, 
// 		const u64 &incValue, 
// 		const u64 &initValue,
// 		u64 &new_value,
// 		const AosRundataPtr &rdata)
// {
// 	// This is a hack on IILU64. 'key' is the value part, which 
// 	// should be unique.  It retrieves the entry identified by 'key'. 
// 	// If found, it increments it by 'incValue'. If the entry is not there, it adds:
// 	// 		[key, incValue]
// 	i64 idx = -10;
// 	i64 iilidx = -10;
// 	bool isunique;
// 	u64 did = 0;
// 	u64 eid = key;
// 	AOSLOCK(mLock);
// 	bool rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, eid, did, isunique, rdata);
// 
// 	// Chen Ding, 12/15/2010
// 	// if (!rslt || did == AOS_INVDID || idx < 0)
// 	if (!rslt || idx < 0)
// 	{
// 		// Did not find it. 
// 		new_value = initValue;
// 	}
// 	else
// 	{
// 		// Found it
// 		new_value = did + incValue;
// 		removeDocPriv(key, did, rdata);
// 	}
// 
// 	rslt = addDocPriv(key, new_value, true, false, false, rdata);
// 	AOSUNLOCK(mLock);
// 	return rslt; 
// }


bool
AosIILU64::incrementDocidSafe(
		const u64 &key, 
		u64 &value,
		const u64 &incValue, 
		const u64 &initValue,
		const bool add_flag,
		const u64 &dft_value,
		const AosRundataPtr &rdata)
{
	// This function retrieves the value identified by 'key'.
	// If the entry does not exist, it checks 'add_flag'. If it
	// is true, it will add it.
	// It increments the docid by 'incvalue' and returns
	// the old value (i.e., the value before incrementing) of the docid.
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	u64 did = 0;
	u64 eid = key;
	AOSLOCK(mLock);
	bool rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, eid, did, isunique, rdata);

	// Chen Ding, 12/15/2010
	// if (!rslt || did == AOS_INVDID || idx < 0)
	if (!rslt || idx < 0)
	{
		if (add_flag)
		{
			// Need to add it.
			value = initValue + incValue;
			bool rslt = addDocPriv(key, value, true, false, rdata);
			AOSUNLOCK(mLock);
			value = initValue;
			return rslt;
		}

		AOSUNLOCK(mLock);
		AosSetError(rdata, "entry_not_exist_001");
		value = dft_value;
		return false;
	}

	// Found it
	value = did + incValue;
	removeDocPriv(key, did, rdata);
	rslt = addDocPriv(key, value, true, false, rdata);
	AOSUNLOCK(mLock);
	value = did;
	return rslt; 
}


/*
u64         
AosIILU64::getEntrySafe(const i64 idx, OmnString &value)
{
	aos_assert_r(mNumDocs <= mMemCap, false);
	AOSLOCK(mLock);
	if (idx < 0 || idx >= mNumDocs) 
	{
		AOSUNLOCK(mLock);
		return AOS_INVDID;
	}

	value = "";
	value << mValues[idx];
	u64 did = mDocids[idx];
	AOSUNLOCK(mLock);
	return did;
}
*/


// Tank, 12/23/2010, Zky2064
bool
AosIILU64::appendManualOrder(const u64 &docid, u64 &value, const AosRundataPtr &rdata)
{
	// This function appends an entry to the list. The entry's 
	// docid is 'docid'. This is used to maintain a manual order.
	// For manual orders, an 'artificial' number called 'seqno'
	// is used. When appending an entry, its seqno is the last
	// one + 0x00000001 00000000 (u64). In other word, values
	// are assigned every 4G apart. When inserting an entry between
	// two entries (changing the order), its value is changed
	// to (prev_seqno + next_seqno)/2. Since the number is big
	// enough, normally it does not need to re-assign seqnos.
	u64 seqn = 0;
	i64 idx = -10;
	i64 iilidx = -10;
	bool rslt;

	AOSLOCK(mLock);
	u64 did = nextDocidPriv(idx, iilidx, true, rdata);
	if (did == AOS_INVDID)
	{
		// The list is empty, add it.
		rslt = addDocPriv(seqn, docid, true, true, rdata);
		aos_assert_rb(rslt, mLock, false);
		value = seqn;
		AOSUNLOCK(mLock);
		return true;
	}

	// 'iilidx' points to the last subiil.
	// if iilidx == -1 means a signal iil
	AosIILU64Ptr subiil;
	if (iilidx > 0) subiil = getSubiilByIndex(iilidx, rdata);
	else subiil = this;
	aos_assert_rb(subiil, mLock, false);
	aos_assert_rb(subiil->mNumDocs > 0, mLock, false);
	seqn = subiil->mValues[subiil->mNumDocs-1];
	seqn += (((u64)1) << 32);
	if (subiil.getPtr() == this)
	{
		rslt = addDocPriv(seqn, docid, true, true, rdata);
		aos_assert_rb(rslt, mLock, false);
		value = seqn;
	}
	else
	{
		bool keepSearch;
		rslt = subiil->addDocDirectSafe(seqn, docid, true, true, keepSearch, rdata);
		aos_assert_rb(rslt, mLock, false);
		value = seqn;
	}
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILU64::moveManualOrder(
		u64 &v1,
		const u64 &d1, 
		u64 &v2,
		const u64 &d2,
		const OmnString flag,
		const AosRundataPtr &rdata)
{
	// This function moves the entry [value2, docid2] to the flag 
	// of the entry [value1, docid1]. 
	aos_assert_r(v1 != v2, false);
	u64 value1, value2, docid1, docid2;
		
	value1 = v1;
	docid1 = d1;
	value2 = v2;
	docid2 = d2;

	// It moves Entry [value2, docid2] to the front of Entry [value1, docid1].
	// This is done by:
	// 	1. Changing [value1, docid1] to [value1, docid2].
	// 	2. Find a new value for 'value1', called 'new_value1'.
	// 	3. Inserting [new_value1, docid1] after [value1, docid2]. 
	i64 idx1 = -10;
	i64 iilidx1 = -10;
	u64 did = 0;
	bool isunique = true;
	AOSLOCK(mLock);
	// 1. Find the position of [value1, docid1].
	bool rslt = nextDocidPriv(idx1, iilidx1, false, eAosOpr_eq, value1, did, isunique, rdata);
	if (!rslt || idx1 < 0 || did == AOS_INVDID)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry not found: " << value1 << ":" << docid1 << enderr;
		return false;
	}

	if (did != docid1)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry mismatch: " << value1 << ":" << did << ":" << docid1 << enderr;
		return false;
	}


	// 2. Find the position of [entry2, docid2]
	i64 idx2 = -10;
	i64 iilidx2 = -10;
	rslt = nextDocidPriv(idx2, iilidx2, false, eAosOpr_eq, value2, did, isunique, rdata);
	if (!rslt || idx2 < 0 || did == AOS_INVDID)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry not found: " << value2 << ":" << docid2 << enderr;
		return false;
	}

	if (did != docid2)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry mismatch: " << value2 << ":" << did << ":" << docid2 << enderr;
		return false;
	}

	// 3. Retrieve all the subiils
	AosIILU64Ptr subiil1;
	if (iilidx1 > 0) subiil1 = getSubiilByIndex(iilidx1, rdata);
	else subiil1 = this;
	aos_assert_rb(subiil1, mLock, false);
	aos_assert_rb(subiil1->mNumDocs > 0 && idx1 < subiil1->mNumDocs, mLock, false);
	
	AosIILU64Ptr subiil2;
	if (iilidx2 > 0) subiil2 = getSubiilByIndex(iilidx2, rdata);
	else subiil2 = this;
	aos_assert_rb(subiil2, mLock, false);
	aos_assert_rb(subiil2->mNumDocs > 0 && idx2 < subiil2->mNumDocs, mLock, false);
	
	i64 idx3 = idx1 + 1;
	AosIILU64Ptr subiil3 = subiil1;
	if (idx1 == (subiil1->mMaxNormIILSize-1))
	{
		subiil3 = getSubiilByIndex(iilidx1+1, rdata);
		aos_assert_rb(subiil3->mNumDocs > 0, mLock, false);
		idx3 = 0;
	}
	
	// 3. Find the next entry flag Entry 1, called Entry 3
	u64 value3 = subiil3->mValues[idx3];

	// 4. Make sure Seqno1 + 1 < Seqno3, by calling resortSeqno()
	if ((value1 + 1) == value3) 
	{
		bool rslt = reassignValuesPriv(rdata);
		aos_assert_rb(rslt, mLock, false);
		value1 = subiil1->mValues[idx1];
		value2 = subiil2->mValues[idx2];
		value3 = subiil3->mValues[idx3];
	}
	
	bool keepSearch;

	if (flag == "before")
	{
		subiil1->mDocids[idx1] = docid2;
		subiil1->setDirty(true);

		u64 new_value;
		if (value3 == 0)
		{
			aos_assert_rb(subiil3->mDocids[idx3]==0, mLock, false);
			value3 = value1 + (((u64)1) << 32); 
			new_value = value3;
		}
		else
		{
			new_value = (value1 + value3)/2;
		}


		if (this == subiil1.getPtr())
		{
			addDocDirectPriv(new_value, docid1, true, true, keepSearch, rdata);
		}
		else
		{
			subiil1->addDocDirectSafe(new_value, docid1, true, true, keepSearch, rdata);
		}

		v1 = new_value;
		v2 = value1;
	}

	if (flag == "after")
	{
		u64 new_value;
		if (value3 == 0)
		{
			aos_assert_rb(subiil3->mDocids[idx3]==0, mLock, false);
			value3 = value1 + (((u64)1) << 32); 
			new_value = value3;
		}
		else
		{
			new_value = (value1 + value3)/2;
		}

		if (this == subiil3.getPtr())
		{
			addDocDirectPriv(new_value, docid2, true, true, keepSearch, rdata);
		}
		else
		{
			subiil3->addDocDirectSafe(new_value, docid2, true, true, keepSearch, rdata);
		}

		v1 = value1;
		v2 = new_value;
	}
	
	// 5. Delete [value2, docid2]
	if (this == subiil2.getPtr())
	{
		removeDocDirectPriv(value2, docid2, keepSearch ,rdata);
	}
	else
	{
		subiil2->removeDocDirectSafe(value2, docid2, keepSearch, rdata);
	}

	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILU64::reassignValuesPriv(const AosRundataPtr &rdata)
{
	// This function is locked. It goes over all its values, 
	// reassign the values so that the higher four bytes
	// form a sequence (i.e., 0, 1, 2, ...)
	AOSLOCK(mLock);
	if (isSingleIIL())
	{
		for (i64 i=0; i<mNumDocs; i++)
		{
			mValues[i] = (i << 32) + (i64)mValues[i];
		}
		AOSUNLOCK(mLock);
		return true;
	}

	aos_assert_rb(isParentIIL(), mLock, false);
	aos_assert_rb(mNumSubiils > 0, mLock, false);
	for (i64 iilidx=0; iilidx<mNumSubiils; iilidx++)
	{
		AosIILU64Ptr iil = getSubiilByIndex(iilidx, rdata);
		aos_assert_rb(iil, mLock, false);
		aos_assert_rb(iil->reassignValuesDirectSafe(), mLock, false);
	}
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILU64::reassignValuesDirectSafe()
{
	AOSLOCK(mLock);
	for (i64 i=0; i<mNumDocs; i++)
	{
		mValues[i] = (i << 32) + (i64)mValues[i];
	}
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILU64::swapManualOrder(
		const u64 &value1,
		const u64 &docid1,
		const u64 &value2,
		const u64 &docid2,
		const AosRundataPtr &rdata)
{
	// This function swap docid1, docid2. 
	u64 vv;
	u64 key1 = value1;
	u64 key2 = value2;
	i64 idx1 = -10;
	i64 iilidx1 = -10;
	bool isunique = true;
	bool rslt;

	AOSLOCK(mLock);
	rslt = nextDocidPriv(idx1, iilidx1, false, eAosOpr_eq, key1, vv, isunique, rdata);
	if (!rslt || idx1 < 0 || vv == AOS_INVDID)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry not found: " << value1 << ":" << docid1 << enderr;
		return false;
	}
	
	if (vv != docid1)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry mismatch: " << value1 << ":" << vv << ":" << docid1 << enderr;
		return false;
	}

	i64 idx2 = -10;
	i64 iilidx2 = -10;
	rslt = nextDocidPriv(idx2, iilidx2, false, eAosOpr_eq, key2, vv, isunique, rdata);
	if (!rslt || idx2 < 0 || vv == AOS_INVDID)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry not found: " << value2 << ":" << docid2 << enderr;
		return false;
	}
	
	if (vv != docid2)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry mismatch: " << value2 << ":" << vv << ":" << docid2 << enderr;
		return false;
	}

	u64 temp;
	AosIILU64Ptr subiil1;
	if (iilidx1 > 0) subiil1 = getSubiilByIndex(iilidx1, rdata);
	else subiil1 = this;
	aos_assert_rb(subiil1, mLock, false);
	aos_assert_rb(subiil1->mNumDocs > 0 && idx1 < subiil1->mNumDocs, mLock, false);
	
	AosIILU64Ptr subiil2;
	if (iilidx2 > 0) subiil2 = getSubiilByIndex(iilidx2, rdata);
	else subiil2 = this;
	aos_assert_rb(subiil2, mLock, false);
	aos_assert_rb(subiil2->mNumDocs > 0 && idx2 < subiil2->mNumDocs, mLock, false);

	temp = subiil1->mDocids[idx1];
	subiil1->mDocids[idx1] = subiil2->mDocids[idx2];
	subiil2->mDocids[idx2] = temp;
	subiil1->setDirty(true);
	subiil2->setDirty(true);

	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILU64::removeManualOrder(const u64 &value, const u64 &docid, const AosRundataPtr &rdata)
{
	u64 vv;
	u64 key = value;
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	bool rslt;

	AOSLOCK(mLock);
	rslt = nextDocidPriv(idx, iilidx, true, eAosOpr_eq, key, vv, isunique, rdata);
	if (!rslt || idx < 0 || vv == AOS_INVDID)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry not found: " << value << ":" << vv << enderr;
		return false;
	}
	
	if (vv != docid)
	{
		AOSUNLOCK(mLock);
		OmnAlarm << "Entry mismatch: " << value << ":" << vv << ":" << docid << enderr;
		return false;
	}

	AosIILU64Ptr subiil;
	if (iilidx > 0) subiil = getSubiilByIndex(iilidx, rdata);
	else subiil = this;
	aos_assert_rb(subiil, mLock, false);
	aos_assert_rb(subiil->mNumDocs > 0 && idx < subiil->mNumDocs, mLock, false);

	bool keepSearch;
	if (this == subiil.getPtr())
	{
		removeDocDirectPriv(value, docid, keepSearch, rdata);
	}
	else
	{
		subiil->removeDocDirectSafe(value, docid, keepSearch, rdata);
	}
	AOSUNLOCK(mLock);
	return true;
}


// Chen Ding, 12/28/2010     
u64
AosIILU64::nextDocidPriv(
		i64 &idx,
		i64 &iilidx,
		const bool reverse, 
		const AosRundataPtr &rdata)
{	
	u64 docid = AOS_INVDID;	
	if (!(idx >= 0 || idx == -10))	
	{		
		OmnAlarm << "idx incorrect: " << idx << enderr;		
		return AOS_INVDID;	
	}	
	
	if (!(iilidx >= 0 || iilidx == -10))	
	{		
		OmnAlarm << "iilidx incorrect: " << iilidx << enderr;		
		return AOS_INVDID;	
	}	

	if (iilidx > mNumSubiils -1)	
	{		
		idx = -5;		
		docid = AOS_INVDID;		
		return true;	
	}	
	
	if (isParentIIL())	
	{		
		AosIILU64Ptr subiil;		
		//1. get the subiil		
		if (iilidx == -10)		
		{			
			if (reverse)			
			{				
				iilidx = mNumSubiils -1;			
			}			
			else			
			{				
				iilidx = 0;			
			}		
		}	

		aos_assert_r(mNumSubiils > 0, false);
		//2. get the docid		
		while(iilidx >= 0 && iilidx <= mNumSubiils - 1)		
		{			
			subiil = getSubiilByIndex(iilidx, rdata);			
			aos_assert_r(subiil, true);			
			if (subiil == this)			
			{				
				docid = subiil->nextDocIdPriv2(idx,iilidx,reverse);			
			}			
			else			
			{				
				docid = subiil->nextDocIdSafe2(idx,iilidx,reverse, rdata);			
			}							
			if (docid != AOS_INVDID)
				break;			
			// not found this time			
			if (reverse)			
			{				
				iilidx --;			
			}			
			else			
			{				
				iilidx ++;			
			}			
			// reset idx to -10			
			idx = -10;		
		}		
		if (docid == AOS_INVDID)		
		{			
			idx = -5;			
			iilidx = -5;		
		}	
	}	
	else	
	{		
		// single IIL		
		docid = nextDocIdPriv2(idx,iilidx,reverse);	
	}	
	return docid;
}


bool 		
AosIILU64::mergeSubiilPriv(
		const i64 &iilidx, 
		const AosRundataPtr &rdata)
{
	// This function should be called on mRootIIL. The iil 'iilidx' is too 
	// small. The function will re-arrange the contents as follows:
	// 1. If 'iilidx-1' and 'iilidx' together < mMaxNormIILSize, average them
	// 2. Otherwise, if 'iilidx' and 'iilidx+1' < mMaxNormIILSize, average them
	// 3. Otherwise, 
	// for torturer
	mIILMergeFlag = true;

	aos_assert_r(isParentIIL(), false);	
	aos_assert_r(mNumSubiils > 1, false);
	aos_assert_r(iilidx >= 0, false);
	bool rslt = false;

	//1. If there is a iil around, two iils contains more than 	
	//   mMaxNormIILSize docs, merge some docs from that iil to this one.
	AosIILU64Ptr iil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iil,false);
	i64 numdoc1 = iil->getNumDocs();
	i64 numdoc2 = 0; // num doc prev
	i64 numdoc3 = 0; // num doc next
	
	//i64 merge_size = (mMaxNormIILSize - mMinIILSize * 2)/2 + mMinIILSize * 2;
	i64 merge_size = mMaxNormIILSize/2 + mMinIILSize;
	if (iilidx > 0)
	{
		AosIILU64Ptr iil2 = getSubiilByIndex(iilidx - 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc2 = iil2->getNumDocs();

		if (numdoc1 + numdoc2 >= merge_size)
		{
			// iilidx-1 and iilidx are too 
			// move some docs from iil2 to iil1
			i64 nummove = (numdoc1 + numdoc2)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilForwardPriv(iilidx -1, nummove, rdata);
			aos_assert_r(rslt, false);
			return rslt;
		}
	}
	
	if (iilidx < mNumSubiils - 1)
	{
		AosIILU64Ptr iil2 = getSubiilByIndex(iilidx + 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc3 = iil2->getNumDocs();

		if (numdoc1 + numdoc3 >= merge_size)
		{
			i64 nummove = (numdoc1 + numdoc3)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilBackwardPriv(iilidx + 1, nummove, rdata);
			aos_assert_r(rslt, false);
			return true;
		}
	}
	
	// if it is the first docid, merge the 2nd one to the first one.		
	if (iilidx == 0)
	{
		// merge iil[1] to iil[0]
		rslt = mergeSubiilBackwardPriv(1, numdoc3, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	

	// if it is the last docid, merge this iil to the one before.		
	if (iilidx == mNumSubiils -1)
	{
		rslt = mergeSubiilBackwardPriv(iilidx, numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;
	}	
	
	// The iil is in the middle
	if (numdoc2 < numdoc3)
	{
		// merge to the one before
		rslt = mergeSubiilBackwardPriv(iilidx, numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;		
	}
	else
	{
		//merge to the next one.
		rslt = mergeSubiilForwardPriv(iilidx, numdoc1, rdata);
		aos_assert_r(rslt, false); 
		return rslt;		
	}

	OmnShouldNeverComeHere;
	return true;
}


bool 		
AosIILU64::mergeSubiilForwardPriv(
		const i64 &iilidx,
		const i64 &numDocToMove, 
		const AosRundataPtr &rdata)
{
	// This function is used by root iil, it moves [numDocToMove] docs from 
	// subiil[iilidx] to subiil[iilidx+1]. If the original subiil mNumDocs finally 
	// turns to 0, we'll call another function to remove the subiil 
	// from the subiil list. To remove all the entries from the root 
	// iil(the first subiil) is forbidden. We'll check it before.
	
	// 1. validation(params checking)
	aos_assert_r(isParentIIL(), false);
	aos_assert_r(mNumSubiils > 1, false);
	aos_assert_r(iilidx >= 0, false);
	aos_assert_r(iilidx < mNumSubiils -1,false);
	aos_assert_r(mNumEntries[iilidx] >= numDocToMove,false);		
	if (iilidx == 0)
	{
		aos_assert_r(mNumEntries[iilidx] > numDocToMove,false);		
	}

	// 2. move entries from a to b
	AosIILU64Ptr crtiil = getSubiilByIndex(iilidx, rdata);
	AosIILU64Ptr nextiil = getSubiilByIndex(iilidx+1, rdata);
	aos_assert_r(crtiil, false);
	aos_assert_r(nextiil, false);
		
	bool rslt = false;
	u64* 	valuePtr = 0;
	u64*    docidPtr = 0;
	// get pointers
	i64 offset = crtiil->getNumDocs() - numDocToMove;
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
	if (crtiil->getNumDocs() == 0)
	{
		removeSubiil(iilidx, rdata);
	}
	else
	{
		updateIndexData(iilidx,true,true);
	}
	return true;	
}


bool 		
AosIILU64::mergeSubiilBackwardPriv(
		const i64 &iilidx,
		const i64 &numDocToMove,
		const AosRundataPtr &rdata)
{
	// This function is used by root iil, it moves [numDocToMove] docs 
	// from subiil[iilidx] to subiil[iilidx-1]. If the original subiil 
	// mNumDocs finally turns to 0, we'll call another function to remove 
	// the subiil from the subiil list.
	// To remove all the entries from the root iil(the first subiil) 
	// is forbidden. We'll check it before.
	
	// 1. validation(params checking)
	aos_assert_r(isParentIIL(), false);
	aos_assert_r(mNumSubiils > 1, false);
	aos_assert_r(iilidx > 0,false);
	aos_assert_r(iilidx <= mNumSubiils -1, false);
	
	// 2. move entries from a to b
	AosIILU64Ptr previil = getSubiilByIndex(iilidx-1, rdata);
	AosIILU64Ptr crtiil = getSubiilByIndex(iilidx, rdata);
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

	if (crtiil->getNumDocs() == 0)
	{
		removeSubiil(iilidx, rdata);
	}
	else
	{
		updateIndexData(iilidx, true, true);
	}
	return true;	
}


bool		
AosIILU64::getValueDocidPtr(
		u64* &valuePtr,
		u64* &docidPtr,
		const i64 &offset)
{
	aos_assert_r(mValues, false);
	aos_assert_r(mDocids, false);
	aos_assert_r(offset >= 0, false);
	aos_assert_r(offset < mNumDocs, false);
	valuePtr = &(mValues[offset]);
	docidPtr = &(mDocids[offset]);
	return true;
}


bool 
AosIILU64::appendDocToSubiil(
		u64 *values, 
		u64 *docids, 
		const i64 &numDocs,
		const bool addToHead) 
{
	// Notice that we use memcpy to copy string point list to the new subiil, 
	// So we can NOT use delete in the original subiil, instead, we use 
	// memset to clear that memory.
	aos_assert_r(mIILType == eAosIILType_U64, false);

	// ModifyNumDocs
	aos_assert_r(mNumDocs + numDocs < mMaxNormIILSize, false);
	if (numDocs == 0)
	{
		return true;
	}
	i64 newNumDocs = mNumDocs + numDocs; 
	aos_assert_r(newNumDocs > 0,false);	
	//1. enlarge mValues/mDocids space to hold all the values and docids, in the same time, 
	// move the original data and new data to the new place.	
	// if the number of the new list is less than mMemCap, do nothing to 		
	
	if (newNumDocs < mMemCap)
	{
		if (addToHead)
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
		u64* mem = OmnNew u64[newNumDocs + mExtraDocids];
		aos_assert_r(mem, false);
		memset(mem, 0, sizeof(u64) * (newNumDocs + mExtraDocids));
		// for docids
		u64 *mem2 = OmnNew u64[newNumDocs + mExtraDocids];
		aos_assert_r(mem2, false);
		memset(mem2 , 0, sizeof(u64) * (newNumDocs + mExtraDocids));
	
		mMemCap = newNumDocs;

		if (addToHead)
		{
			// copy value
			memmove(&(mem[numDocs]), mValues, sizeof(u64) * mNumDocs);
			memcpy(mem, values, sizeof(u64) * numDocs);
			// copy docid
			memmove(&(mem2[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mem2, docids, sizeof(u64) * numDocs);
			
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
AosIILU64::removeDocFromSubiil(
		const i64 &numRemove,
		const bool delFromHead)
{
	// Notice:
	// 1. When we move docs to another subiil, we use memcpy to copy string point list. 
	// 	  So we can NOT use delete in the original subiil, instead, we use 
	// 	  memset to clear that memory.
	// 2. We do not shrink memory when deleting elements.
	aos_assert_r(numRemove <= mNumDocs, false);
	i64 newsize = mNumDocs - numRemove;
	if (delFromHead)
	{
		// copy value
		i64 ss = sizeof(u64) * (newsize);
		OmnMemCheck(mValues, ss);
		memmove(mValues, &(mValues[numRemove]), ss);
		ss = sizeof(u64)*numRemove;
		OmnMemCheck(&mValues[newsize], ss);
		memset(&(mValues[newsize]), 0, ss);

		// copy docid
		ss = sizeof(u64) * (newsize);
		OmnMemCheck(mDocids, ss); 
		memmove(mDocids, &(mDocids[numRemove]), ss);
		ss = sizeof(u64) * numRemove;
		OmnMemCheck(&mDocids[newsize], ss);
		memset(&(mDocids[newsize]), 0, ss);
	}
	else // del from tail
	{
		OmnMemCheck(&mValues[newsize], sizeof(u64) * numRemove);
		memset(&(mValues[newsize]), 0, sizeof(u64) * numRemove);
		OmnMemCheck(&mDocids[newsize], sizeof(u64) * numRemove);
		memset(&(mDocids[newsize]), 0, sizeof(u64) * numRemove);		
	}
	
	mNumDocs = newsize;
	mIsDirty = true;
	return true;
}


bool 		
AosIILU64::removeSubiil(
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(isParentIIL(), false);
	aos_assert_r(iilidx > 0 && iilidx < mNumSubiils,false);
	aos_assert_r(mNumSubiils > 1,false);
	bool rslt = false;
	//1. return the iil
	if (mSubiils[iilidx])
	{
		aos_assert_r(mSubiils[iilidx]->getNumDocs() == 0,false);
		rslt = AosIILMgrSelf->returnIILPublic(mSubiils[iilidx], rdata);
		aos_assert_r(rslt, false);
		mSubiils[iilidx] = 0;
	}

	//2. proc mMinVals and mMaxVals;
	if (iilidx < mNumSubiils -1)
	{
		memmove(&(mMinVals[iilidx]), &(mMinVals[iilidx+1]), (sizeof(u64)) * (mNumSubiils - iilidx - 1));
		memmove(&(mMaxVals[iilidx]), &(mMaxVals[iilidx+1]), (sizeof(u64)) * (mNumSubiils - iilidx - 1));
	}
	mMinVals[mNumSubiils-1] = 0;
	mMaxVals[mNumSubiils-1] = 0;

	//3. proc mNumEntries mIILIds
	if (iilidx < mNumSubiils -1)
	{
		memmove(&(mNumEntries[iilidx]), &(mNumEntries[iilidx +1]), (sizeof(i64) * (mNumSubiils - 1)));
		memmove(&(mIILIds[iilidx]), &(mIILIds[iilidx +1]), (sizeof(u64) * (mNumSubiils - 1)));
	}
	mNumEntries[mNumSubiils-1] = 0;
	mIILIds[mNumSubiils-1] = 0;
	
	//4. proc mSubiils
	for (i64 i = iilidx ;i < mNumSubiils - 1;i++)
	{
		mSubiils[i] = mSubiils[i+1];// no matter it is null or not
		
		if (mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}
	mSubiils[mNumSubiils - 1] = 0;
	mNumSubiils--;

	//5. do some processing if the iil turns to be a single one 
	if (mNumSubiils == 1)
	{
		
		mRootIIL = 0;
		mNumSubiils = 0;
		mIILIdx = -1;
	    if (mNumEntries)
		{
			OmnDelete [] mNumEntries;
			mNumEntries = 0;
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
	}
	
	//6. set dirty flag
	mIsDirty = true;
	return true;	
}


i64
AosIILU64::getSubiilIndexSeq(
		const u64 &value,
		const i64 &iilidx, 
		const AosRundataPtr &rdata)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, -1);
	aos_assert_r(isParentIIL(), -1);
	aos_assert_r(mNumSubiils > 1, -1);
    i64 left = iilidx>0? iilidx:0;
    i64 right = mNumSubiils - 1;
    if (right <= left)
	{
    	return right;
	}
	
	if (mMaxVals[left] >= value)
	{
		return left;
	}
	
    i64 cur = 0;
    if (right == left)return right;
    bool leftnotfit = false;
	while(1)
    {
        if (right == left +1)
        {
			if (leftnotfit) return right;
            // if left fit, return left
            if (mMaxVals[left] >= value)
            {
                return left;
            }
            else
            {
                return right;
            }
            // else return right
        }

        cur = (right - left)/2 + left;

        if (mMaxVals[cur] >= value)
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


AosIILU64Ptr
AosIILU64::getSubiilSeq(const u64 &value,i64 &iilidx, const AosRundataPtr &rdata)
{
	iilidx = getSubiilIndexSeq(value,iilidx, rdata);
	aos_assert_r(iilidx >= 0 && iilidx <= mNumSubiils -1,0);
	return getSubiilByIndex(iilidx, rdata);
}


bool 		
AosIILU64::addDocSeqPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		i64	&iilidx,
		i64	&idx,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	bool keepSearch = false;
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		AosIILU64Ptr subiil;

		// Chen Ding, Bug1217
		iilidx = getSubiilIndexSeq(value, iilidx, rdata);
		if (iilidx == -5) iilidx = 0;
		aos_assert_r(iilidx >= 0, false);
		
		while (iilidx <= mNumSubiils -1)
		{
			subiil = getSubiil(iilidx, rdata);
			aos_assert_r(subiil, false);
			keepSearch  = false;
			
			if (subiil.getPtr() == this)
			{
				rslt = addDocSeqDirectPriv(value, docid, value_unique, docid_unique,
							idx, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->addDocSeqDirectSafe(value, docid, value_unique, docid_unique, 
							idx, keepSearch, rdata);
			}
			
			if (rslt)
			{
				// Ketty 2013/01/15
				// comp not support yet.
				// add to compiil
				//AosIILCompU64Ptr compiil = retrieveCompIIL(rdata);
				//aos_assert_r(compiil, false);				
				//bool rslt = compiil->addDocSafe(value,docid,false,false,rdata);
				//returnCompIIL(compiil,rdata);
				//aos_assert_r(rslt, false);
			}
			if (!keepSearch)
			{
				return rslt;
			}
			iilidx++;
			idx = -1;
		}
		// raise alarm  lxx
		return false;
	}

	aos_assert_r(isSingleIIL(),false);
	// James, 01/04/2011
	rslt = addDocSeqDirectPriv(
			value, docid, value_unique, docid_unique, idx, keepSearch, rdata);
	if (rslt)
	{
		// Ketty 2013/01/15
		// comp not support yet.
		// add to compiil
		//AosIILCompU64Ptr compiil = retrieveCompIIL(rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->addDocSafe(value, docid, false, false, rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	return rslt;	
}


bool 		
AosIILU64::addDocSeqDirectPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		i64	&curidx,
		bool &keepSearch, 
		const AosRundataPtr &rdata)
{
	keepSearch = false;
//	AosIILU64Ptr r1 = (AosIILU64*)mRootIIL.getPtr();
//	if (r1)r1->splitSanityCheck();
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// In this case, the doc may be inserted in the next subiil.
	
	// Check whether we need to split
	bool rslt = false;
	i64 numBefore = mNumDocs;

	// OmnScreen << "IILU64 add: " << this << endl;
	i64 idx = 0;
	i64 left = 0; 
	i64 right = mNumDocs-1;

	if (curidx > 0)
	{
		left = curidx;
	}
	if (left > right)
	{
		left = right;
	}
	if (mValues[left]>= value)
	{
		right = left;
	}	
	
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
		if (mValues[idx] == value)
		{
			// Chen Ding, 10/24/2010
			if (value_unique)
			{
				/*
				// The value must be unique. This fails the operation. 
				// if 'value_unique' is AOSIIL_FORCE_UNIQUE, it will 
				// override the entry. Otherwise, it returns an error. 
				if (value_unique == AOSIIL_FORCE_UNIQUE)
				{
					// Force unique
					mDocids[idx] = docid;
					return true;
				}
				*/
				OmnAlarm << "Value already exist: " << value << ":" << docid
					<< ":" << mDocids[idx] << enderr;
				return false;
			}

			// Need to insert the doc based on docid
			rslt = insertDocPriv(
					idx, value, docid, value_unique, docid_unique, keepSearch);
			if (!rslt) return rslt;
			if (!keepSearch)
			{
				goto finished;
			}
			return true;
		}

		// Chen Ding, 08/11/2010
		// if (strcmp(mValues[idx], value.data()) < 0) 
		if (mValues[idx] < value) 
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

			// Chen Ding, 08/11/2010
			// Check whether mValues[idx+1] >= value
			// if (strcmp(value.data(), mValues[idx+1]) < 0)
			if (mValues[idx+1] > value)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (mValues[idx+1] ==  value)
			{
				if (value_unique)
				{
					OmnAlarm << "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx] << enderr;
					return false;
				}
				idx++;
				rslt = insertDocPriv(idx, value, docid, value_unique, docid_unique, keepSearch);
				aos_assert_r(rslt, false);
				if (!keepSearch) goto finished;
				return true;
			}

			left = idx+1;
	 	}
		else //mValues[idx] > value
		{
			// value < mValues[idx]
			if (idx == 0)
			{
				// Insert into the front
				insertBefore(idx, docid, value);
				goto finished;
			}

			// Chen Ding, 08/11/2010
			// if (strcmp(mValues[idx-1], value.data()) < 0)
			if (mValues[idx-1] < value)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				idx--;
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (mValues[idx-1] == value)
			{
				if (value_unique)
				{
					OmnAlarm << "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx] << enderr;
					return false;
				}
				idx --;
				rslt = insertDocPriv(idx, value, docid, value_unique, docid_unique, keepSearch);
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
	curidx = idx;
	aos_assert_r(numBefore + 1 == mNumDocs, false);
	if (!isSingleIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);
	}

	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILObjPtr subiil;
		rslt = splitListPriv(subiil, rdata);
	}
	sanityTestForSubiils();
//	AosIILU64Ptr r = (AosIILU64*)mRootIIL.getPtr();
//	if (r)r->splitSanityCheck();
	return true;
}


bool		
AosIILU64::removeDocSeqPriv(
		const u64 &value, 
		const u64 &docid,
		i64	&iilidx,
		i64 &idx, 
		const AosRundataPtr &rdata)
{
	bool keepSearch = true;
	bool rslt = false;
	// Check whether it is a root iil and it is segmented
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		// If 'iilidx' is within [0, mNumSubiils-1], 
		// use it.
		AosIILU64Ptr subiil;
		subiil = getSubiilSeq(value,iilidx,rdata);
		aos_assert_r(subiil, false);
		while(subiil)
		{
			if (subiil.getPtr() == this)
			{
				rslt = removeDocSeqDirectPriv(value, docid, iilidx,idx,keepSearch, rdata);
			}
			else
			{
				rslt = subiil->removeDocSeqDirectSafe(value, docid, iilidx,idx,keepSearch, rdata);		
			}
			if (rslt)
			{
				// Ketty 2013/01/15
				// comp not support yet.
				// remove from compiil
				//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
				//aos_assert_r(compiil, false);				
				//bool rslt = compiil->removeDocSafe(value,docid,rdata);
				//returnCompIIL(compiil,rdata);
				//aos_assert_r(rslt, false);
				return true;
			}
			if (!keepSearch) break;
			// get next subiil
			subiil = subiil->getNextSubIIL(true, rdata);
			iilidx ++;
			idx = -1;
		}
		// found to the last subiil
		// Chen Ding, 12/07/2010
		// No alarms will be raised. The caller is responsible for raising alarms
		// OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
		return false;
	}
	
	// The IIL is not segmented
	rslt = removeDocSeqDirectPriv(value, docid,iilidx,idx,keepSearch, rdata);
	// Chen Ding, 12/07/2010
	// No alarms will be raised. The caller is responsible for raising alarms
	// if (!rslt) OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
	if (rslt)
	{
		// Ketty 2013/01/15
		// comp not support yet.
		// remove from compiil
		//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->removeDocSafe(value,docid,rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	return rslt;
}
		
bool		
AosIILU64::removeDocSeqDirectPriv(
		const u64 &value, 
		const u64 &docid,
		i64	&curiilidx,
		i64	&curidx,
		bool &keepSearch, 
		const AosRundataPtr &rdata)
{
	if (mNumDocs <= 0) return false;

	//i64 startidx = -10;
	i64 startidx = 0;

	if (curidx > 0)startidx = curidx;
	i64 endidx = mNumDocs -1;
	if (endidx < startidx)
	{
		return false;
	}
	
	i64 idx = -1;
	u64 did;
	keepSearch = false;
	bool rslt = false;
	
	if (mValues[startidx] ==  value)
	{
		// found the startidx
	}
	else
	{
		// Find the first entry that matches the value
		rslt = nextDocidEQ(startidx, 1, value, did);
		if (!rslt)
		{
			// Did not find it.
			return false;
		}
	}
	
	if (startidx < 0 || startidx >= mNumDocs)
	{
		// Chen Ding, 12/07/2010
		// OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
		return false;
	}

	// Find the last one
	rslt = nextDocidEQ(endidx, -1, value, did);
	// Chen Ding, 12/07/2010
	// aos_assert_r(rslt,false);	
	if (!rslt) 
	{
		// Did not find it.
		return false;
	}

//	aos_assert_r(nextDocidPriv(startidx, startiilidx,
//			false, eAosOpr_eq, value, did, isunique), false);
	
	idx = startidx;
	while(idx <= endidx)
	{
		if (docid == mDocids[idx]) 
		{
			goto removeEntry;
		}
		else if (mDocids[idx] > docid)
		{
			// not found
			break;
		}
		idx ++;
	}

	if (idx > mNumDocs -1)
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
	curidx = idx;
	aos_assert_r(idx >= 0 && idx < mNumDocs, false);

	// Change mValues
	if (idx < mNumDocs-1)
	{
		i64 ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx+1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx+1], ss);

		OmnMemCheck(&mValues[idx], ss);	
		OmnMemCheck (&mValues[idx+1], ss);	
		memmove(&mValues[idx], &mValues[idx+1], ss);
	}
	decNumDocsNotSafe();

	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	mIsDirty = true;
	
	if (isParentIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);

		if (mNumDocs < mMinIILSize)
		{
			rootiil->mergeSubiilPriv(mIILIdx, rdata);
			curiilidx = -1;
			curidx = -1;
		}
	}
	
	return true;
}


/*
bool
AosIILU64::saveIILsToFileSafe(const OmnFilePtr &file, i64 &crt_pos, const AosRundataPtr &rdata)
{
	// This function saves all the dirty subiils to 'file', starting
	// from 'crt_pos'. The entry format is:
	// 		length		(4 bytes)
	// 		contents	(variable)
	// 		length		
	// 		contents
	// 		...
	AOSLOCKTRACE(mLock);
	aos_assert_rl(isParentIIL(), mLock, false);
	
	bool rslt;
	for(i64 i=1; i<mNumSubiils; i++)
	{
		if (mSubiils[i].notNull())
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


bool
AosIILU64::queryValueSinglePriv(
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool unique_value,
		const bool getall,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	i64 idx_start = -10;
	i64 idx_end = -10;
	u64 docid;
	bool isunique = true;
	if (getall || opr == eAosOpr_an)
	{
		idx_start = 0;
		idx_end = mNumDocs - 1;
	}
	else
	{
		u64 value2 = value;
		rslt = nextDocidSinglePriv(idx_start, false, opr, value2, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		if (idx_start < 0) return true;
		
		value2 = value;
		rslt = nextDocidSinglePriv(idx_end, true, opr, value2, docid, isunique, rdata);
		aos_assert_r(rslt, false);
	}

	for(i64 i = idx_start; i <= idx_end; i++)
	{
		if (unique_value && i == 0 && (!values.empty()) && values.back() == mValues[0])
		{
			continue;
		}
		if (unique_value && i > 0 && (mValues[i] == mValues[i-1]))
		{
			continue;
		}
		values.push_back(mValues[i]);
	}
	return true;
}

bool
AosIILU64::queryValueNEPriv(
		vector<u64> &values,
		const u64 &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return queryValueNESinglePriv(values, value, unique_value, false, rdata);
	}

	bool rslt = false;
	AosIILU64Ptr subiil;
	
	i64 iilidx_start = getSubIILIndex(-10, value, eAosOpr_eq, true);
	if (iilidx_start < 0)
	{
		for (i64 i = 0;i < mNumSubiils;i++)
		{
			subiil = getSubiil(i, rdata);
			aos_assert_r(subiil.notNull(),false);
			rslt = subiil->queryValueNESinglePriv(values, value, unique_value, true, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}
	
	i64 iilidx_end = getSubIILIndex(-10, value, eAosOpr_eq, false);
	aos_assert_r(iilidx_end >= 0,false);

	bool getall = false;
	for(i64 i = 0; i <= iilidx_start; i++)
	{
		if (i != iilidx_start)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}
		
		subiil = getSubiil(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		rslt = subiil->queryValueNESinglePriv(values, value, unique_value, getall, rdata);
		aos_assert_r(rslt, false);
	}

	if (iilidx_end == iilidx_start)
	{
		iilidx_end++;
	}

	for (i64 i = iilidx_end; i < mNumSubiils;i++)
	{
		if (i != iilidx_end)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}
		
		subiil = getSubiil(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		rslt = subiil->queryValueNESinglePriv(values, value, unique_value, getall, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosIILU64::queryValueNESinglePriv(
		vector<u64> &values,
		const u64 &value,
		const bool value_unique,
		const bool getall,
		const AosRundataPtr &rdata)
{
	if (mNumDocs <= 0) return true;
	
	i64 idx_start = -10;
	i64 idx_end = -10;
	u64 docid;
	bool isunique = true;

	u64 value2 = value;
	bool rslt = nextDocidSinglePriv(idx_start, false, eAosOpr_eq, value2, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	if (idx_start < 0 || getall)
	{
		for(i64 i= 0;i <= mNumDocs-1;i++)
		{
			if (value_unique && i == 0 && (!values.empty()) && values.back() == mValues[0])
			{
				continue;
			}
			if (value_unique && i > 0 && (mValues[i] == mValues[i-1]))
			{
				continue;
			}
			values.push_back(mValues[i]);
		} 
		return true;
	}

	value2 = value;
	rslt = nextDocidSinglePriv(idx_end, false, eAosOpr_eq, value2, docid, isunique, rdata);
	aos_assert_r(rslt,false);
	
	for(i64 i= 0;i <= (idx_start - 1);i++)
	{
		if (value_unique && i == 0 && (!values.empty()) && (values.back() == mValues[0]))
		{
			continue;
		}
		if (value_unique && i > 0 && (mValues[i] == mValues[i-1]))
		{
			continue;
		}
		values.push_back(mValues[i]);
	} 
	
	for(i64 i= (idx_end + 1);i <= (mNumDocs - 1);i++)
	{
		if (value_unique && i == 0 && (!values.empty()) && values.back() == mValues[0])
		{
			continue;
		}
		if (value_unique && i > 0 && (mValues[i] == mValues[i-1]))
		{
			continue;
		}
		values.push_back(mValues[i]);
	} 
	return true;
}


bool
AosIILU64::queryValuePriv(
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool value_unique,
		const AosRundataPtr &rdata)
{
	if (opr == eAosOpr_ne)
	{
		return queryValueNEPriv(values, value, value_unique, rdata);
	}

	if (opr == eAosOpr_like)
	{
		return false;
	}

	if (isSingleIIL())
	{
		return queryValueSinglePriv(values, opr, value, value_unique, false, rdata);
	}

	bool rslt = false;

	// copy data
	i64 iilidx_start = getSubIILIndex(-10, value, opr, true);
	if (iilidx_start < 0) return true;

	i64 iilidx_end = getSubIILIndex(-10, value, opr, false);
	if (iilidx_start < 0) return true;

	AosIILU64Ptr subiil;
	bool getall = false;
	for(i64 i = iilidx_start; i <= iilidx_end; i++)
	{
		if (i != iilidx_start && i != iilidx_end)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}

		subiil = getSubiil(i, rdata);
		aos_assert_r(subiil.notNull(),false);

		rslt = subiil->queryValueSinglePriv(values, opr, value, value_unique, getall, rdata);
		aos_assert_r(rslt,false);
	}
	return true;
}

bool    
AosIILU64::removeDocByValuePriv(
        const u64 &value,
        i64 &entriesRemoved,
        const AosRundataPtr &rdata)
{
	bool keepSearch = true;
	bool rslt = false;
	bool shrinked = false;
	AosIILU64Ptr subiil = 0;
	entriesRemoved = 0;
	// Check whether it is a root iil and it is segmented
	while(keepSearch)
	{
		keepSearch = false;
		if (isParentIIL())
		{
			// It is segmented and the iil is the root iil. 
			// If 'iilidx' is within [0, mNumSubiils-1], 
			// use it.
			if (!subiil)
			{
				subiil = getSubiil(value, rdata);
			}
			aos_assert_r(subiil, false);
			
			if (subiil.getPtr() == this)
			{
				rslt = removeDocByValueDirectPriv(value, entriesRemoved, keepSearch, shrinked, rdata);
			}
			else
			{
				rslt = subiil->removeDocByValueDirectSafe(value, entriesRemoved, keepSearch, shrinked, rdata);		
			}
			if (keepSearch)
			{
				if (shrinked)
				{
					subiil = 0;
				}
				else
				{
					subiil = subiil->getNextSubIIL(true, rdata);
				}
			}
		}
		else
		{
			// The IIL is not segmented
			rslt = removeDocByValueDirectPriv(value, entriesRemoved, keepSearch, shrinked, rdata);
			aos_assert_r(rslt, false);
		}
	}	
	return rslt;
}                


bool	
AosIILU64::removeDocByValueDirectPriv(
		const u64 &value, 
        i64 &entriesRemoved,
		bool &keepSearch,
		bool &shrinked, 
		const AosRundataPtr  &rdata)
{
	// It assumes the doc is in this list and the lock has been locked
	// It uses 'value' to locate the start and end idx. It then loops
	// through [startidx, endidx] to check for 'docid'. 
	//
	// Performance Improvements:
	// Entries whose values are the same should be sorted based on 
	// docids. When searching, use the binary search.
	
	keepSearch = false;
	shrinked = false;

	aos_assert_r(mNumDocs > 0, false);
	//i64 startidx = -10;
	i64 startidx = 0;
	i64 endidx = mNumDocs -1;
	i64 idx = -1;
	u64 did;

	bool rslt = nextDocidEQ(startidx,1,value,did);
	if (!rslt)
	{
		keepSearch = false;
		return false;
	}
	if (startidx < 0 || startidx >= mNumDocs)
	{
		// Chen Ding, 12/07/2010
		// OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
		return false;
	}

	rslt = nextDocidEQ(endidx, -1, value, did);
	// Chen Ding, 12/07/2010
	// aos_assert_r(rslt,false);	
	if (!rslt) 
	{
		return false;
	}

//	aos_assert_r(nextDocidPriv(startidx, startiilidx,
//			false, eAosOpr_eq, value, did, isunique), false);
	idx = startidx;
	i64 num_to_remove = (endidx - startidx +1);
	entriesRemoved += num_to_remove;

	// Ketty 2013/01/15
	// comp not support yet.
	// remove from compiil
	//AosIILCompU64Ptr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//for(i64 i = startidx;i <= endidx;i++)
	//{
	//	rslt = compiil->removeDocSafe(mValues[i],mDocids[i],rdata);
	//	if (!rslt)
	//	{
	//		returnCompIIL(compiil,rdata);
	//		aos_assert_r(rslt, false);
	//	}
	//}
	//returnCompIIL(compiil,rdata);
	//aos_assert_r(rslt, false);

	if (mValues[mNumDocs -1] == value)	
	{
		keepSearch = true;
	}
	else
	{
		keepSearch = false;
	}
	
	// Remove the entry 'idx'. Need to move all the entries.
	aos_assert_r(idx >= 0, false);

	// Change mValues
	if (endidx < mNumDocs -1)
	{
		memmove(&mDocids[idx], &mDocids[endidx + 1], sizeof(u64) * (mNumDocs - endidx - 1));
		memmove(&mValues[idx], &mValues[endidx + 1], sizeof(u64) * (mNumDocs - endidx - 1));
	}
	decNumDocsNotSafe(num_to_remove);

	memset(&mValues[mNumDocs], 0, sizeof(u64) * num_to_remove);
	memset(&mDocids[mNumDocs], 0, sizeof(u64) * num_to_remove);

	aos_assert_r(checkMemory(), false);
	mIsDirty = true;
	if (isParentIIL() || isLeafIIL())
	{
		bool updateMin = (startidx == 0);
		bool updateMax = (endidx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_U64, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);

		if (mNumDocs < mMinIILSize)
		{
			shrinked = true;
			rootiil->mergeSubiilPriv(mIILIdx, rdata);
		}
	}
	return true;
}


void    
AosIILU64::exportIILPriv(
		vector<u64> &values,
	   	vector<u64>	&docids,
		const AosRundataPtr &rdata)
{
	values.clear();
	docids.clear();
	if (isParentIIL())
	{
		AosIILU64Ptr subiil;
		exportIILSinglePriv(values,docids,rdata);
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i,rdata);
			subiil->exportIILSingleSafe(values,docids,rdata);
		}
		return;
	}
	return exportIILSinglePriv(values,docids,rdata);
}


void    
AosIILU64::exportIILSinglePriv(
		vector<u64>	&values,
	   	vector<u64> &docids,
		const AosRundataPtr &rdata)
{
	for(i64 i = 0;i < mNumDocs;i++)
	{
		values.push_back(mValues[i]);
		docids.push_back(mDocids[i]);
	}
}


bool
AosIILU64::copyDocidsPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const i64 &start,
		const i64 &end)
{
	// Chen Ding, 12/15/2010
	aos_assert_r(mDocids, false);
	aos_assert_r(end < 0 || end < mNumDocs, false);

	i64 s = start;
	if (s<0)s = 0;
	i64 e = end;
	if (e<0)e = mNumDocs -1;
	i64 size = e - s + 1;
	aos_assert_r(size>=0,false);
	
	if (!query_rslt && query_bitmap)
	{
		query_bitmap->appendDocids(&(mDocids[s]),size);
		return true;
	}
	
	bool rslt = true;	
	u64 docid = 0;
	if (query_rslt)
	{
		bool addValue = query_rslt->isWithValues();
		for(i64 i = s;i <= e;i++)
		{
			docid = mDocids[i];
			// filter by bitmap
			if (query_bitmap && (!query_bitmap->checkDoc(docid))) continue;
			query_rslt->appendDocid(docid);
			if (addValue)
			{
				query_rslt->appendU64Value(mValues[i]);
			}
		}
	}

	return rslt;
}


// Ketty 2013/01/15
//AosIILObjPtr	
//AosIILU64::createCompIIL(const AosRundataPtr &rdata)
//{
//	return AosIILMgr::getSelf()->createIILPublic1(
//						mIILID,
//						mSiteid,
//						eAosIILType_CompU64,
//						false,
//						false,
//						rdata);
//}


// Ketty 2013/01/15
// comp not support yet.
/*
AosIILCompU64Ptr 	
AosIILU64::retrieveCompIIL(
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		aos_assert_r(mRootIIL,0);
		AosIILU64Ptr rootiil = (AosIILU64*)mRootIIL.getPtr();
		return rootiil->retrieveCompIIL(iilmgrlock, rdata);
	}
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILObjPtr compiil1 = AosIILMgr::getSelf()->loadIILByIDPublic(
								mIILID, 
								mSiteid,
								iiltype, 
								true, 
								iilmgrlock, 
								rdata);
	aos_assert_r(compiil1, 0);
	AosIILCompU64Ptr compiil = (AosIILCompU64*)compiil1.getPtr();
	return compiil;
}

bool	
AosIILU64::returnCompIIL(const AosIILObjPtr &iil,const AosRundataPtr &rdata)
{
	return AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
}	
*/

bool
AosIILU64::queryNewNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	// 1. We get 2 pieces of data which contain the data we need.
	bool block_exist_1 = true;
	bool block_exist_2 = true;
	i64 block_start_idx_1 = 0;
	i64 block_start_iilidx_1 = 0;
	i64 block_end_idx_1 = mNumDocs -1;
	i64 block_end_iilidx_1 = 0;

	i64 block_start_idx_2 = 0;
	i64 block_start_iilidx_2 = 0;
	i64 block_end_idx_2 = mNumDocs -1;
	i64 block_end_iilidx_2 = 0;
	u64 docid = 0;
	if (isParentIIL())
	{
		block_end_idx_1 = mNumEntries[mNumSubiils -1]-1;
		block_end_iilidx_1 = mNumSubiils -1;
		block_end_idx_2 = mNumEntries[mNumSubiils -1]-1;
		block_end_iilidx_2 = mNumSubiils -1;
	}	
	
	u64 value = query_context->getStrValue().toU64(0);

	i64 cur_idx = -10;
	i64 cur_iilidx = -10;
	bool is_unique = false;
	bool rslt = nextDocidPriv(cur_idx, cur_iilidx, false, eAosOpr_eq,value, docid, is_unique, rdata);
	aos_assert_r(rslt,false);
	if (cur_idx >= 0)
	{
		//1. set block 1 infomation
		if (cur_idx == 0)
		{
			if (cur_iilidx == 0)
			{
				block_exist_1 = false;
			}else
			{
				block_end_iilidx_1 = cur_iilidx -1;
				block_end_idx_1 = mNumEntries[block_end_iilidx_1]-1;
			}
		}
		else
		{
			block_end_idx_1 = cur_idx -1;
			block_end_iilidx_1 = cur_iilidx;
		}
		
		//2. set block 2 infomation
		cur_idx = -10;
		cur_iilidx = -10;
		rslt = nextDocidPriv(cur_idx, cur_iilidx, true,eAosOpr_eq, value, docid, is_unique, rdata);
		aos_assert_r(rslt && cur_idx >= 0,false);
		if (isSingleIIL())
		{
			if (cur_idx >= mNumDocs -1)
			{
				block_exist_2 = false;				
			}
			else
			{
				block_start_idx_2 = cur_idx +1;
				block_start_iilidx_2 = 0;
			}
		}
		else// root iil
		{
			if (cur_idx >= mNumEntries[cur_iilidx] -1)
			{
				block_start_idx_2 = 0;
				block_start_iilidx_2 = cur_iilidx +1;
				if (block_start_iilidx_2 >= mNumSubiils)
				{
					block_exist_2 = false;								
				}
			}
			else
			{
				block_start_idx_2 = cur_idx +1;
				block_start_iilidx_2 = cur_iilidx;
			}
		}
	}
	else
	{
		block_exist_2 = false;
	}
	
	// 2. Get the current index from query rslt and fix it if it's wrong
	cur_idx = query_context->getIndex();
	cur_iilidx	= query_context->getIILIndex();
	u64 cur_value = query_context->getCrtValue().toU64(0);
	u64 cur_docid = query_context->getCrtDocid();
	bool has_data = true;
	bool reverse = query_context->isReverse();
		
	if (cur_idx == -10 || cur_iilidx == -10)
	{
		//no need to change current position
	}
	else
	{
		aos_assert_r(cur_idx >= 0 && cur_iilidx >= 0,false);
		nextIndexFixErrorIdx(
			cur_idx, cur_iilidx, cur_value,
			cur_docid, reverse, has_data, rdata);
		if (!has_data)
		{
			query_context->setFinished(true);
			return true;
		}
	}

	// Now we count the real start/end we need in the 2 pieces of data		
	i64 start_pos = 0;
	i64 end_pos = 0;
	bool finished = true;	

	i64 doc_needed = query_context->getBlockSize();
	if (reverse)
	{
		// start from block 2
		if (block_exist_2)
		{
			if (compPos(cur_idx,cur_iilidx,block_end_idx_2,block_end_iilidx_2) < 0)
			{
				block_end_idx_2		= cur_idx;
				block_end_iilidx_2	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_start_idx_2,block_start_iilidx_2) < 0)
			{
				block_exist_2 = false;
			}
			else
			{
				for(i64 i = block_end_iilidx_2;i >= block_start_iilidx_2;i--)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_2)start_pos = block_start_idx_2;
					if (i == block_end_iilidx_2)end_pos = block_end_idx_2;

					if (end_pos - start_pos +1 > doc_needed)
					{
						// we found the end
						block_exist_1 = false;
						block_start_iilidx_2 = i;
						block_start_idx_2 = end_pos - doc_needed +1;
						finished = false;	
						query_context->setIndex(block_start_idx_2);
						query_context->setIILIndex(block_start_iilidx_2);

						AosIILU64Ptr iil = getIILByIndex(block_start_iilidx_2, rdata);
						aos_assert_r(iil, false);
						
						query_context->setCrtDocid(iil->getDocid(block_start_idx_2));
						query_context->setCrtValue(OmnStrUtil::ulltoa(iil->getValue(block_start_idx_2)));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
		if (!doc_needed && block_exist_1)
		{
			finished = false;
		}
		
		if (doc_needed && block_exist_1)
		{
			if (compPos(cur_idx,cur_iilidx,block_end_idx_1,block_end_iilidx_1) < 0)
			{
				block_end_idx_1		= cur_idx;
				block_end_iilidx_1	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_start_idx_1,block_start_iilidx_1) < 0)
			{
				block_exist_1 = false;
			}
			else
			{
				for(i64 i = block_end_iilidx_1;i >= block_start_iilidx_1;i--)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_1)start_pos = block_start_idx_1;
					if (i == block_end_iilidx_1)end_pos = block_end_idx_1;

					if (end_pos - start_pos +1 >= doc_needed)
					{
						// we found the end
						block_start_iilidx_1 = i;
						block_start_idx_1 = end_pos - doc_needed +1;
						finished = false;
						query_context->setIndex(block_start_idx_1);
						query_context->setIILIndex(block_start_iilidx_1);
						AosIILU64Ptr iil = getIILByIndex(block_start_iilidx_1, rdata);
						aos_assert_r(iil, false);
						
						query_context->setCrtDocid(iil->getDocid(block_start_idx_1));
						query_context->setCrtValue(OmnStrUtil::ulltoa(iil->getValue(block_start_idx_1)));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
	}
	else // normal order
	{
		// start from block 1
		if (block_exist_1)
		{
			if (compPos(cur_idx,cur_iilidx,block_start_idx_1,block_start_iilidx_1) > 0)
			{
				block_start_idx_1		= cur_idx;
				block_start_iilidx_1	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_end_idx_1,block_end_iilidx_1) > 0)
			{
				block_exist_1 = false;
			}
			else
			{
				for(i64 i = block_start_iilidx_1;i <= block_end_iilidx_1;i++)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_1)start_pos = block_start_idx_1;
					if (i == block_end_iilidx_1)end_pos = block_end_idx_1;

					if (end_pos - start_pos +1 >= doc_needed)
					{
						// we found the end
						block_exist_2 = false;
						block_end_iilidx_1 = i;
						block_end_idx_1 = start_pos + doc_needed -1;
						finished = false;	
						query_context->setIndex(block_end_idx_1);
						query_context->setIILIndex(block_end_iilidx_1);
						AosIILU64Ptr iil = getIILByIndex(block_end_iilidx_1, rdata);
						aos_assert_r(iil, false);

						query_context->setCrtDocid(iil->getDocid(block_end_idx_1));
						query_context->setCrtValue(OmnStrUtil::ulltoa(iil->getValue(block_end_idx_1)));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
		if (!doc_needed && block_exist_2)
		{
			finished = false;
		}
		
		if (doc_needed && block_exist_2)
		{
			if (compPos(cur_idx,cur_iilidx,block_start_idx_2,block_start_iilidx_2) > 0)
			{
				block_start_idx_2		= cur_idx;
				block_start_iilidx_2	= cur_iilidx;			
			}
			
			if (compPos(cur_idx,cur_iilidx,block_end_idx_2,block_end_iilidx_2) > 0)
			{
				block_exist_2 = false;
			}
			else
			{
				for(i64 i = block_end_iilidx_2;i <= block_start_iilidx_2;i++)
				{
					start_pos = 0;
					// get the end_pos of the iil
					if (i == 0)
					{
						end_pos = mNumDocs-1;
					}else
					{
						end_pos = mNumEntries[i]-1;
					}
					if (i == block_start_iilidx_2)start_pos = block_start_idx_2;
					if (i == block_end_iilidx_2)end_pos = block_end_idx_2;

					if (end_pos - start_pos +1 >= doc_needed)
					{
						// we found the end
						block_start_iilidx_2 = i;
						block_start_idx_2 = start_pos + doc_needed -1;
						finished = false;
						query_context->setIndex(block_end_idx_2);
						query_context->setIILIndex(block_end_iilidx_2);
						AosIILU64Ptr iil = getIILByIndex(block_end_iilidx_2, rdata);
						aos_assert_r(iil, false);
						
						query_context->setCrtDocid(iil->getDocid(block_end_idx_2));
						query_context->setCrtValue(OmnStrUtil::ulltoa(iil->getValue(block_end_idx_2)));
						break;
					}
					doc_needed -= (end_pos - start_pos +1);
				}
			}
			
		}
		
	}

	// copy data
	query_context->setFinished(finished);
	if (reverse)
	{
		query_rslt->setDataReverse(true);
	}
	if (block_exist_1)
	{
		copyData(query_rslt,
				 query_bitmap,
				 query_context,
				 block_start_idx_1,
				 block_start_iilidx_1,
				 block_end_idx_1,
				 block_end_iilidx_1,
				 rdata);
	}
	
	if (block_exist_2)
	{
		copyData(query_rslt,
				 query_bitmap,
				 query_context,
				 block_start_idx_2,
				 block_start_iilidx_2,
				 block_end_idx_2,
				 block_end_iilidx_2,
				 rdata);
	}

	return true;
}


bool
AosIILU64::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	if (isSingleIIL() && mNumDocs == 0)
	{
		query_context->setFinished(true);
		return true;
	}
	
	if (query_rslt)
	{
		query_rslt->setDataReverse(false);
	}

	aos_assert_r(!query_context->isCheckRslt(), false);

	//Now we have two special cases to handle
	//1. opr == like
	//2. opr == not equal
	AosOpr opr = query_context->getOpr();
	if (opr == eAosOpr_ne)
	{
		return queryNewNEPriv(query_rslt, query_bitmap, query_context, rdata);
	}
	// Normal query
	//1. get the block start/end idx/iilidx

	//2. copy the data

	//1. get the block start/end idx/iilidx
	i64 block_start_idx 		= 0;
	i64 block_start_iilidx		= 0;
	i64 block_end_idx			= 0;
	i64 block_end_iilidx		= 0;

	bool has_data = true;
	bool rslt = queryPagingProc(
		query_rslt, query_bitmap, query_context,
		block_start_idx, block_start_iilidx, block_end_idx,
		block_end_iilidx, has_data, rdata);
	aos_assert_r(rslt, false);

	if (!has_data)
	{
		if (query_rslt) query_rslt->clean();
		if (!query_rslt && query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	//2. copy the data
	rslt = copyData(
		query_rslt, query_bitmap, query_context,
		block_start_idx, block_start_iilidx,
		block_end_idx, block_end_iilidx, rdata);	
	aos_assert_r(rslt, false);

	if (isSingleIIL())
	{
		query_context->setTotalDocInIIL(mNumDocs);
	}
	else
	{
		i64 total_in_iil = 0;
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			total_in_iil += mNumEntries[i];
		}
		query_context->setTotalDocInIIL(total_in_iil);
	}

	return true;
}


bool
AosIILU64::queryPagingProc(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		i64 &start_idx,
		i64 &start_iilidx,
		i64 &end_idx,
		i64 &end_iilidx,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	//1. get the start idx/iilidx
	//2. get the end idx/iilidx
	//3. get the cur idx/iilidx
	//4. get the block start/end  idx/iilidx
	bool need_paging = true;
	if (query_rslt && query_bitmap)
	{
		aos_assert_r(!query_bitmap->isEmpty(), false);
		need_paging  = false;
	}
	if (!query_rslt && query_bitmap)
	{
		aos_assert_r(query_bitmap->isEmpty(), false);
	}
	i64 bsize = query_context->getBlockSize();
	if (!bsize || query_context->hasFilter())
	{
		need_paging = false;
	}
		
	has_data = true;
	if (query_context->finished())
	{
		OmnAlarm << "Querycontext incorrect" << enderr;
		has_data = false;
		return true;
	}

	bool reverse = query_context->isReverse();
	AosOpr opr1 = query_context->getOpr();
	AosOpr opr2 = opr1;
	u64 u64_value = query_context->getStrValue().toU64(0);
	u64 u64_value2 = u64_value; 

	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value <= u64_value2, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value < u64_value2, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value < u64_value2, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value < u64_value2, false);
    }

	//1. get the start idx/iilidx
	start_idx = -10;
	start_iilidx = -10;
	u64 cur_docid_0 = 0;
	bool isunique = true;
	bool rslt = nextDocidPriv(
		start_idx, start_iilidx, false, // normal order 
		opr1, u64_value, cur_docid_0, isunique, rdata);
	aos_assert_r(rslt, false);

	if (start_idx < 0)
	{
		has_data = false;
		return true;
	}
	
	//2. get the end idx/iilidx	
	end_idx = -10;
	end_iilidx = -10;
	rslt = nextDocidPriv(
		end_idx, end_iilidx, true, // reverse order
		opr2, u64_value2, cur_docid_0, isunique, rdata);
	aos_assert_r(rslt, false);

	if (end_idx < 0)
	{
		aos_assert_r(opr1 != opr2, false);
		has_data = false;
		return true;
	}

	if (isSingleIIL())
	{
		start_iilidx = 0;
		end_iilidx = 0;
	}

	if (query_context->getTotalDocInRslt() == 0)
	{
		i64 total_in_rslt = 0;
		if (start_iilidx == end_iilidx)
		{
			total_in_rslt = (end_idx - start_idx + 1);
		}
		else
		{
			for (i64 i=start_iilidx+1; i<end_iilidx; i++)
			{
				total_in_rslt += mNumEntries[i];
			}
			total_in_rslt += (mNumEntries[start_iilidx] - start_idx);
			total_in_rslt += (end_idx + 1);
		}
		query_context->setTotalDocInRslt(total_in_rslt);
	}

	//3. get the cur idx/iilidx, to fix start_idx/iilidx & end_idx/iilidx
	i64 cur_idx = query_context->getIndex();
	i64 cur_iilidx	= query_context->getIILIndex();
	if (cur_idx == -10 || cur_iilidx == -10)
	{
		//no need to change current position
	}
	else
	{
		u64 cur_value = query_context->getCrtValue().toU64(0);
		u64 cur_docid = query_context->getCrtDocid();
		aos_assert_r(cur_idx >= 0 && cur_iilidx >= 0, false);

		nextIndexFixErrorIdx(
			cur_idx, cur_iilidx, cur_value, cur_docid,
			reverse, has_data, rdata);
		if (!has_data)
		{
			return true;
		}
		
		if (reverse)
		{
			end_idx = cur_idx;
			end_iilidx = cur_iilidx;
		}
		else
		{
			start_idx = cur_idx;
			start_iilidx = cur_iilidx;
		}
	}
	
	// here is a check, if there are no more docs there, set has_data to false and return.
	if (end_iilidx < start_iilidx || ((end_iilidx == start_iilidx) && (end_idx < start_idx)))
	{
		has_data = false;
		return true;
	}
	
	if (!need_paging) return true;
	
	//4. get the block start/end  idx/iilidx
	i64 new_cur_idx = 0;
	i64 new_cur_iilidx = 0;
	if (reverse)
	{
		if (isSingleIIL())
		{
			new_cur_idx = end_idx - bsize + 1;
			if (new_cur_idx <= start_idx)
			{
				query_context->setFinished(true);
				query_context->setIndex(0);
				query_context->setIILIndex(0);
				return true;
			}
			
			query_context->setIndex(new_cur_idx);
			query_context->setIILIndex(0);
			query_context->setCrtDocid(mDocids[new_cur_idx]);
			query_context->setCrtValue(OmnStrUtil::ulltoa(mValues[new_cur_idx]));
			start_idx = new_cur_idx;
			return true;
		}
		
		// root iil
		new_cur_idx = end_idx;
		new_cur_iilidx = end_iilidx;
		i64 start_1 = 0;
		i64 end_1 = 0;
		while (bsize > 0 && new_cur_iilidx >= start_iilidx)
		{
			start_1 = (new_cur_iilidx == start_iilidx)?start_idx:0;
			end_1 = (new_cur_iilidx == end_iilidx)?end_idx:(mNumEntries[new_cur_iilidx]-1);
			
			if (end_1 - start_1 + 1 >= bsize)
			{
				// enough doc to get
				new_cur_idx = end_1 - bsize +1;
				break;
			}
			
			// not enough to get
			if (new_cur_iilidx == start_iilidx)
			{
				// no more iils left
				new_cur_idx = start_idx;
				break;
			}

			new_cur_iilidx--;
			new_cur_idx = mNumEntries[new_cur_iilidx];
		}
		
		if (new_cur_iilidx == start_iilidx && new_cur_idx == start_idx)
		{
			query_context->setFinished(true);
			query_context->setIndex(0);
			query_context->setIILIndex(0);
		}
		else
		{
			query_context->setIndex(new_cur_idx);
			query_context->setIILIndex(new_cur_iilidx);

			AosIILU64Ptr iil = getIILByIndex(new_cur_iilidx, rdata);
			aos_assert_r(iil, false);

			query_context->setCrtDocid(iil->getDocid(new_cur_idx));
			query_context->setCrtValue(OmnStrUtil::ulltoa(iil->getValue(new_cur_idx)));
		}
		
		start_idx = new_cur_idx;
		start_iilidx = new_cur_iilidx;
		return true;
	}
	else// Normal order, not reverse
	{
		if (isSingleIIL())
		{
			new_cur_idx = start_idx + bsize - 1;

			if (new_cur_idx >= end_idx)
			{
				query_context->setFinished(true);
				query_context->setIndex(0);
				query_context->setIILIndex(0);
				return true;
			}
			
			query_context->setIndex(new_cur_idx);
			query_context->setIILIndex(0);
			query_context->setCrtDocid(mDocids[new_cur_idx]);
			query_context->setCrtValue(OmnStrUtil::ulltoa(mValues[new_cur_idx]));
			end_idx = new_cur_idx;
			return true;
		}
		
		// root iil
		new_cur_idx = start_idx;
		new_cur_iilidx = start_iilidx;
		i64 start_1 = 0;
		i64 end_1 = 0;
		while (bsize > 0 && new_cur_iilidx <= end_iilidx)
		{
			start_1 = (new_cur_iilidx == start_iilidx)?start_idx:0;
			end_1 = (new_cur_iilidx == end_iilidx)?end_idx:(mNumEntries[new_cur_iilidx]-1);
			
			if (end_1 - start_1 + 1 >= bsize)
			{
				// enough doc to get
				new_cur_idx = start_1 + bsize -1;
				break;
			}
			
			// not enough to get
			if (new_cur_iilidx == end_iilidx)
			{
				// no more iils left
				new_cur_idx = end_idx;
				break;
			}
			bsize -= (end_1 - start_1 + 1);
			new_cur_iilidx++;
			new_cur_idx = 0;
		}
		
		if (new_cur_iilidx == end_iilidx && new_cur_idx == end_idx)
		{
			query_context->setFinished(true);
			query_context->setIndex(0);
			query_context->setIILIndex(0);
		}
		else
		{
			query_context->setIndex(new_cur_idx);
			query_context->setIILIndex(new_cur_iilidx);
			AosIILU64Ptr iil = getIILByIndex(new_cur_iilidx, rdata);
			aos_assert_r(iil, false);

			query_context->setCrtDocid(iil->getDocid(new_cur_idx));
			query_context->setCrtValue(OmnStrUtil::ulltoa(iil->getValue(new_cur_idx)));
		}
		
		end_idx = new_cur_idx;
		end_iilidx = new_cur_iilidx;
	}

	return true;
}


void
AosIILU64::nextIndexFixErrorIdx(
		i64 &cur_idx,
		i64 &cur_iilidx,
		const u64 &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)
{
	if (!(cur_idx >= 0 && cur_iilidx >=0))
	{
		OmnMark;
	}
	aos_assert(cur_idx >= 0 && cur_iilidx >=0);
	has_data = true;
	// single IIL
	if (isSingleIIL())
	{
		//1. index out of bound
		if (cur_iilidx > 0 || cur_idx > mNumDocs -1)
		{
			locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
			return;
		}
		//2. value changed
		if (mValues[cur_idx] != cur_value && mDocids[cur_idx] != cur_docid)
		{
			locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
			return;
		}
		//3. nothing changed		
		if (reverse)
		{
			prevQueryPos(cur_idx,cur_iilidx);
		}
		else
		{
			nextQueryPos(cur_idx,cur_iilidx);
		}
		return;
	}
	
	// root IIL
	if (cur_iilidx > mNumSubiils -1 ||
	   ((cur_iilidx == (mNumSubiils -1 )) && (cur_idx > (mNumEntries[mNumSubiils -1]-1))))
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}

	if (cur_idx > (mNumEntries[cur_iilidx]-1))
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}
	
	AosIILU64Ptr subiil = getIILByIndex(cur_iilidx,rdata);
	if (!subiil)
	{
		// should not happen
		has_data = false;	
		return;
	}
	if (!subiil->verifyIdx(cur_idx,cur_value,cur_docid))
	{
		locateIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
		return;
	}

	if (reverse)
	{
		prevQueryPos(cur_idx,cur_iilidx);
	}
	else
	{
		nextQueryPos(cur_idx,cur_iilidx);
	}
	
	return;
}


bool
AosIILU64::verifyIdx(
		const i64 &cur_idx,
		const u64 &cur_value,
		const u64 &cur_docid)
{
	return (cur_idx < mNumDocs && mDocids[cur_idx] == cur_docid && mValues[cur_idx] == cur_value);
}


bool	
AosIILU64::copyData(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start_idx,
		const i64 &start_iilidx,
		const i64 &end_idx,
		const i64 &end_iilidx,
		const AosRundataPtr &rdata)
{
	bool reverse = query_context->isReverse();
	if (query_rslt && reverse)
	{
		query_rslt->setDataReverse(true);
	}

	bool withCheck = (query_rslt && query_bitmap);
	bool hasFilter = query_context->hasFilter();

	if (withCheck || hasFilter)
	{
		return copyDataWithCheck(query_rslt, query_bitmap, query_context,
			start_idx, start_iilidx, end_idx, end_iilidx, rdata);
	}

	if (isSingleIIL())
	{
		return copyDataSingle(query_rslt, query_bitmap,
			query_context, start_idx, end_idx, rdata);
	}
	
	// rootIIL
	bool rslt = true;
	i64 start = 0;
	i64 end = 0;
	AosIILU64Ptr subiil;

	if (reverse)
	{
		for (i64 i=end_iilidx; i>=start_iilidx; i--)
		{
			start = 0;
			end = mNumEntries[i] - 1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;
		
			subiil = getIILByIndex(i, rdata);
			aos_assert_r(subiil, false);

			rslt = subiil->copyDataSingle(query_rslt, query_bitmap,
				query_context, start, end, rdata);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		for (i64 i=start_iilidx; i<=end_iilidx; i++)
		{
			start = 0;
			end = mNumEntries[i] - 1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;
		
			subiil = getIILByIndex(i, rdata);
			aos_assert_r(subiil, false);

			rslt = subiil->copyDataSingle(query_rslt, query_bitmap,
				query_context, start, end, rdata);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


bool	
AosIILU64::copyDataWithCheck(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start_idx,
		const i64 &start_iilidx,
		const i64 &end_idx,
		const i64 &end_iilidx,
		const AosRundataPtr &rdata)
{
	bool rslt = true;
	bool keep_search = true;
	bool reverse = query_context->isReverse();

	if (isSingleIIL())
	{
		rslt = copyDataSingleWithCheck(
			query_rslt, query_bitmap, query_context,
			start_idx, end_idx, reverse, keep_search, rdata);
		aos_assert_r(rslt, false);

		if (keep_search)
		{
			query_context->setIndex(0);
			query_context->setIILIndex(0);
			query_context->setFinished(true);
		}
		return true;
	}

	i64 start = 0;
	i64 end = 0;
	AosIILU64Ptr subiil;
	i64 num_docs = 0;

	if (reverse)
	{
		for (i64 i=end_iilidx; i>=start_iilidx; i--)
		{
			start = 0;
			end = mNumEntries[i] -1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;

			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil,false);

			rslt = subiil->copyDataSingleWithCheck(
				query_rslt, query_bitmap, query_context,
				start, end, reverse, keep_search, rdata);
			aos_assert_r(rslt, false);

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				return true;
			}
		}
	}
	else //normal order
	{
		for (i64 i=start_iilidx; i<=end_iilidx; i++)
		{
			start = 0;
			end = mNumEntries[i] -1;
			if (i == start_iilidx) start = start_idx;
			if (i == end_iilidx) end = end_idx;

			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil, false);

			rslt = subiil->copyDataSingleWithCheck(
				query_rslt, query_bitmap, query_context,
				start, end, reverse, keep_search, rdata);
			aos_assert_r(rslt, false);

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				return true;
			}
		}
	}

	if (keep_search)
	{
		query_context->setIndex(0);
		query_context->setIILIndex(0);
		query_context->setFinished(true);
	}

	return true;
}


bool	
AosIILU64::copyDataSingleWithCheck(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start_idx,
		const i64 &end_idx,
		const bool reverse, 
		bool  keep_search,
		const AosRundataPtr &rdata)
{
	keep_search = false;
	i64 num_docs = 0;
	bool onlyBitmap = (!query_rslt && query_bitmap);
	bool withCheck = (query_rslt && query_bitmap);
	bool hasFilter = query_context->hasFilter();

	if (reverse)
	{
		for (i64 i=end_idx; i>=start_idx; i--)
		{
			if (i == start_idx)
			{
				keep_search = true;
			}

			if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
			{
				continue;
			}

			if (hasFilter && !query_context->evalFilter(
				mValues[i], mDocids[i], rdata))
			{
				continue;	
			}

			if (onlyBitmap)
			{
				query_bitmap->appendDocid(mDocids[i]);
			}
			else
			{
				query_rslt->appendDocid(mDocids[i]);
				if (query_rslt->isWithValues())
				{
					query_rslt->appendU64Value(mValues[i]);
				}
			}

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				query_context->setIndex(i);
				query_context->setCrtDocid(mDocids[i]);
				query_context->setCrtValue(OmnStrUtil::ulltoa(mValues[i]));
				return true;
			}
		}
	}		
	else
	{
		for (i64 i=start_idx; i<=end_idx; i++)
		{
			if (i == end_idx)
			{
				keep_search = true;
			}

			if (withCheck && !query_bitmap->checkDoc(mDocids[i]))
			{
				continue;
			}

			if (hasFilter && !query_context->evalFilter(
				mValues[i], mDocids[i], rdata))
			{
				continue;	
			}

			if (onlyBitmap)
			{
				query_bitmap->appendDocid(mDocids[i]);
			}
			else
			{
				query_rslt->appendDocid(mDocids[i]);
				if (query_rslt->isWithValues())
				{
					query_rslt->appendU64Value(mValues[i]);
				}
			}

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				query_context->setIndex(i);
				query_context->setCrtDocid(mDocids[i]);
				query_context->setCrtValue(OmnStrUtil::ulltoa(mValues[i]));
				return true;
			}
		}
	}	

	return true;
}			


bool	
AosIILU64::copyDataSingle(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const i64 &start_idx,
		const i64 &end_idx,
		const AosRundataPtr &rdata)
{
	i64 size = end_idx - start_idx + 1;
	aos_assert_r(size >= 0, false);

	if (size == 0) return true;
	bool reverse = query_context->isReverse();

	if (query_bitmap)
	{
		query_bitmap->appendDocids(&(mDocids[start_idx]),size);
	}

	if (query_rslt)
	{
		bool addValue = query_rslt->isWithValues();
		if (reverse)
		{
			query_rslt->appendBlockRev(&(mDocids[start_idx]), size);
			if (addValue)
			{
				for (i64 i=end_idx; i>=start_idx; i--)
				{
					query_rslt->appendU64Value(mValues[i]);
				}
			}
		}
		else
		{
			query_rslt->appendBlock(&(mDocids[start_idx]), size);
			if (addValue)
			{
				for (i64 i=start_idx; i<=end_idx; i++)
				{
					query_rslt->appendU64Value(mValues[i]);
				}
			}
		}
	}
	
	return true;
}


bool	
AosIILU64::nextQueryPos(i64 &idx,i64 &iilidx)
{
	if (iilidx < 0)
	{
		iilidx = 0;
		idx = 0;
		return true;
	}
	idx ++;
	if (isParentIIL() && idx >= mNumEntries[iilidx])
	{
		iilidx ++;
		idx = 0;
	}
	
	return true;
}


bool	
AosIILU64::prevQueryPos(i64 &idx,i64 &iilidx)
{
	if (isSingleIIL() && iilidx > 0)
	{
		iilidx = 0;
		idx = mNumDocs -1;
		return true;
	}

	idx --;

	if (isParentIIL() && idx < 0)
	{
		iilidx --;
		aos_assert_r(iilidx >= 0,false);
		idx = mNumEntries[iilidx] -1;
	}
	
	return true;
}


bool
AosIILU64::locateIdx(
		i64 &cur_idx,
		i64 &cur_iilidx,
		const u64 &cur_value,
		const u64 &cur_docid,
		const bool reverse,
		bool &has_data,
		const AosRundataPtr &rdata)	
{
	if (reverse)
	{
		prevQueryPos(cur_idx,cur_iilidx);
	}
	else
	{
		nextQueryPos(cur_idx,cur_iilidx);
	}

	if (reverse)
	{
		if (cur_iilidx < 0)
		{
			has_data = false;
			return true;
		}
		if (isParentIIL() && cur_iilidx > mNumSubiils -1)
		{
			cur_iilidx = mNumSubiils -1;
		}
		//1. get the first doc which value == cur_value && docid < cur_docid
		//2. if not found, get the first doc which value < cur_value
		//3. if not found, set has_data == false and return
		if (cur_idx < 0)cur_idx = 0;		
	}
	else // Normal order
	{
		if (isParentIIL() && cur_iilidx > mNumSubiils -1)
		{
			has_data = false;
			return true;			
		}
		
		if (cur_iilidx < 0)
		{
			cur_iilidx = 0;
		}
		
		if (isParentIIL() && cur_idx > mNumEntries[cur_iilidx]-1)
		{
			cur_idx = mNumEntries[cur_iilidx]-1;
		}
		
		if (isSingleIIL() && cur_idx > mNumDocs -1)
		{
			cur_idx = mNumDocs -1;
		}
		//1. get the first doc which value == cur_value && docid > cur_docid
		//2. if not found, get the first doc which value > cur_value
		//3. if not found, set has_data == false and return		
	}
	
	
/*	if (reverse)
	{
		// get the first doc which less than cur_docid backward
		if (cur_docid < mDocids[0])
		{
			cur_iilidx = 0;
			cur_idx = 0;
			has_data = false;
			return true;
		}
	}
	else // normal order
	{
		// get the first doc which greater than cur_docid
		u64 max_docid = mDocids[mNumDocs-1];
		if (isParentIIL()) max_value = mMaxVals[mNumSubiils-1];
		if (cur_docid > max_docid)
		{
			cur_iilidx = 0;
			cur_idx = 0;
			has_data = false;
			return true;
		}
	}
	
	if (isSingleIIL())
	{
		cur_iilidx = 0;
		if (reverse)
		{
			cur_idx = firstLTRev(cur_docid);
			aos_assert_r(cur_idx >= 0,false);
		}
		else
		{
			cur_idx = firstGT(cur_docid);
			aos_assert_r(cur_idx <= mNumDocs -1,false);
		}
		return true;
	}
	// root iil
	AosIILHitPtr subiil;
	if (reverse)
	{
		cur_iilidx = firstSubLTRev(cur_docid);
		aos_assert_r(cur_iilidx >= 0,false);
		subiil = getIILByIndex(cur_iilidx,rdata);
		if (!subiil)
		{
			// should not happen
			has_data = false;
			return true;
		}
		cur_idx = subiil->firstLTRev(cur_docid);
	}
	else
	{
		cur_iilidx = firstSubGT(cur_docid);
		aos_assert_r(cur_iilidx <= mNumSubiils-1,false);
		subiil = getIILByIndex(cur_iilidx,rdata);
		if (!subiil)
		{
			// should not happen
			has_data = false;
			return true;
		}
		cur_idx = subiil->firstGT(cur_docid);
	}
*/	
	return true;
}			


i64
AosIILU64::compPos(	
		const i64 &idx1,	
		const i64 &iilidx1,	
		const i64 &idx2,	
		const i64 &iilidx2)
{	
	if (iilidx1 != iilidx2)	
	{		
		return iilidx1 - iilidx2;	
	}	
	return idx1-idx2;
}

bool
AosIILU64::deleteIILPriv(
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return deleteIILSinglePriv(rdata);
	}
	
	// remove from IILMgr IDHash
	AosIILMgr::getSelf()->removeIILFromHashPublic(mIILID, mSiteid, rdata);

	AosIILU64Ptr subiil;
	for(i64 i = 1;i < mNumSubiils;i++)
	{
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil, false);

		subiil->deleteIILSafe(true_delete, rdata);
		mSubiils[i] = 0;
	}

	return true;
}


bool	
AosIILU64::deleteIILSinglePriv(const AosRundataPtr &rdata)
{
	//1. remove from IILMgr IDHash
	AosIILMgr::getSelf()->removeIILFromHashPublic(mIILID, mSiteid, rdata);
	//2. remove from disk
	deleteFromLocalFilePriv(rdata);
	return true;
}


bool	
AosIILU64::preQueryNEPriv(
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}

bool	
AosIILU64::preQueryPriv(
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	if (isSingleIIL() && mNumDocs == 0)
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}
	
	AosOpr opr1 = query_context->getOpr();
	if (opr1 == eAosOpr_ne)
	{
		return preQueryNEPriv(query_context,rdata);
	}

	//1. get the block start/end idx/iilidx
	AosOpr opr2 = opr1;
	u64 u64_value = query_context->getStrValue().toU64(0);
	u64 u64_value2 = u64_value; 

	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value <= u64_value2, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value < u64_value2, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value < u64_value2, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		u64_value2 = query_context->getStrValue2().toU64(0);
		aos_assert_r(u64_value < u64_value2, false);
    }

	//1. get the start idx/iilidx
	i64 start_idx = -10;
	i64 start_iilidx = -10;
	u64 cur_docid_0 = 0;
	bool isunique = true;
	bool rslt = nextDocidPriv(start_idx, 
						 start_iilidx,
						 false, // normal order 
						 opr1,
						 u64_value,
						 cur_docid_0, 
						 isunique,
						 rdata);
	aos_assert_r(rslt,false);
	if (start_idx < 0)
	{
		return true;
	}
	
	//2. get the end idx/iilidx	
	i64 end_idx = -10;
	i64 end_iilidx = -10;
						 
	rslt = nextDocidPriv(end_idx, 
						 end_iilidx,
						 true, // normal order 
						 opr2,
						 u64_value2,
						 cur_docid_0, 
						 isunique,
						 rdata);
	aos_assert_r(rslt,false);
	if (end_idx < 0)
	{
		aos_assert_r(opr1 != opr2, false);
		return true;
	}
	
	if (isSingleIIL())
	{
		start_iilidx = 0;
		end_iilidx = 0;
	}

	i64 total_in_rslt = 0;
	if (start_iilidx == end_iilidx)
	{
		total_in_rslt = (end_idx - start_idx +1);
	}
	else
	{
		for(i64 i = start_iilidx +1;i < end_iilidx;i++)
		{
			total_in_rslt += mNumEntries[i];
		}
		total_in_rslt += (mNumEntries[start_iilidx] - start_idx);
		total_in_rslt += (end_idx +1);
	}

	query_context->setTotalDocInRslt(total_in_rslt);

	return rslt;
}


bool	
AosIILU64::batchAddSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = false;
	u64 key = 0;
	u64 value = 0;
	for(int i=0; i<num; i++)
	{
		key = *(u64 *)entries;
		value = *(u64 *)&entries[size - sizeof(u64)];
		rslt = addDocSafe(key, value, false, false, rdata);
		aos_assert_r(rslt, false);
		
		entries += size;
	}
	
	rslt = saveToFileSafe(rdata);
	aos_assert_r(rslt, false);

	u64 cost = OmnGetTimestamp() - start_time;
	i64 end_total = mNumDocs;

	OmnScreen << "batch add finish, IILID:" << mIILID
		<< ", start_total:" << start_total
		<< ", end_total:" << end_total
		<< ", time_cost:" << AosTimestampToHumanRead(cost) 
		<< endl;

	return true;
}


void
AosIILU64::setSnapShotId(const u64 &snap_id)
{
	AOSLOCK(mLock);
	setSnapShotIdPriv(snap_id);
	AOSUNLOCK(mLock);
}


void
AosIILU64::setSnapShotIdPriv(const u64 &snap_id)
{
	aos_assert(mSnapShotId == 0 || snap_id == mSnapShotId);
	mSnapShotId = snap_id;

	if (isParentIIL())
	{
		AosIILU64Ptr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->setSnapShotIdPriv(snap_id);
		}
	}
}


void
AosIILU64::resetSnapShotId()
{
	AOSLOCK(mLock);
	resetSnapShotIdPriv();
	AOSUNLOCK(mLock);
}


void
AosIILU64::resetSnapShotIdPriv()
{
	mSnapShotId = 0;	
	if (isParentIIL())
	{
		AosIILU64Ptr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->resetSnapShotIdPriv();
		}
	}
}


bool
AosIILU64::resetIIL(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}

