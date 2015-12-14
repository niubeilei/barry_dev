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
#include "IILMgr/IILStr.h"

#include "AppMgr/App.h"
#include "BatchQuery/BatchQueryCtlr.h"
#include "DfmUtil/DfmDocIIL.h"
#include "IILMgr/IILMgr.h"
#include "IILUtil/IILLog.h"
#include "IILUtil/IILSave.h"
#include "Porting/ThreadDef.h"
#include "SEUtil/Docid.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/StrStrArray.h"
#include "Util1/MemMgr.h"
#include "Util1/MemPoolNew.h"
#include "WordClient/WordClient.h"


static bool sgCheckMemory = false;


AosIILStr::AosIILStr()
:
AosIIL(eAosIILType_Str),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0),
mIsNumAlpha(false)
{
	mTotalIILs[eAosIILType_Str]++;
}


AosIILStr::AosIILStr(
		const u64 &wordid, 
		const u64 &iilid,
		const bool isPersis,
		const OmnString &iilname, 
		const AosRundataPtr &rdata)
:
AosIIL(wordid, iilid, eAosIILType_Str, isPersis, iilname),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0),
mIsNumAlpha(false)
{
	mTotalIILs[eAosIILType_Str]++;
//OmnScreen << "====================create iil, iilid:" << iilid << ",total:" << mTotalIILs[eAosIILType_Str] << endl;
}


AosIILStr::AosIILStr(
		const u64 &iilid, 
		const u32 siteid, 
		const AosDfmDocIILPtr &doc, 	//Ketty 2012/11/15
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_Str),
mValues(0),
mMinVals(0),
mMaxVals(0),
mSubiils(0),
mIsNumAlpha(false)
{
	mTotalIILs[eAosIILType_Str]++;
//OmnScreen << "================create iil, iilid:" << iilid << ",total:" << mTotalIILs[eAosIILType_Str] << endl;
	mIILID = iilid;
	mDfmDoc = doc;		// Ketty 2013/01/15

	//mIsGood = loadFromFilePriv(buff, siteid, rdata);
	//mIsGood = loadFromFilePriv(doc, siteid, rdata);	// Ketty 2012/11/15
	mIsGood = loadFromFilePriv(iilid, rdata);	// Ketty 2012/11/15
}


AosIILStr::~AosIILStr()
{
	//AosRundataPtr rdata = OmnApp::getDeleteObjRundata();
	mTotalIILs[eAosIILType_Str]--;
//OmnScreen << "=================delete iil, iilid:" << mIILID << ",total:" << mTotalIILs[eAosIILType_Str] << endl;

	if (mValues)
	{
		for (i64 i=0; i<mMemCap; i++)
		{
			if (mValues[i]) OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
		}

		// error Ken Lee, 2013/04/08
		//OmnMemRemove(mValues, sizeof(mValues));
	
		OmnDelete [] mValues;
		mValues = 0;
	}

	if (isParentIIL())
	{
		for (i64 i=0; i<mNumSubiils; i++)
		{
			OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);

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
AosIILStr::staticInit(const AosXmlTagPtr &theconfig)
{
	// Its configuration is in the subtag "AOSCONFIG_IILMGR"
	aos_assert_r(theconfig, false);
	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;
	
	OmnMemMgr::getSelf();
	// mMaxStrLen = config->getAttrInt(AOSCONFIG_MAX_STRLEN, mMaxStrLen);
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
AosIILStr::nextDocidPriv(
		i64 &idx, 
		i64 &iilidx,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
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
	
	//ken 2012/05/14
	if (opr == eAosOpr_prefix)
	{
		return nextDocidPrefixPriv(idx, iilidx, reverse, value, docid, rdata);
	}
	
	// Check whether it is a root iil and it is segmented
	bool rslt = false;
	docid = AOS_INVDID;
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		// If 'iilidx' is within [0, mNumSubiils-1],
		// use it.
		AosIILStrPtr subiil;
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
				idx = -5;
				iilidx = -5;
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
			idx = -5;
			iilidx = -5;
			docid = AOS_INVDID;
			isunique = true;
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


bool		
AosIILStr::nextDocidPrefixPriv(
		i64 &idx, 
		i64 &iilidx,
		const bool reverse, 
		const OmnString &prefix,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// Check whether it is a root iil and it is segmented
	bool rslt = false;
	docid = AOS_INVDID;
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil.
		// If 'iilidx' is within [0, mNumSubiils-1],
		// use it.
		AosIILStrPtr subiil;
		if (idx == -10)
		{
			iilidx = getSubIILIndexByPrefix(-10, prefix, !reverse);

			if (iilidx < 0)
			{
				OmnAlarm << "Invalid: " << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				return false;
			}
		}
		else
		{
			if (idx < 0 || iilidx < 0 || iilidx >= mNumSubiils)
			{
				OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
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
			return false;
		}

		bool keepSearch = true;
		while(subiil)
		{
			// if we found the doc, return
			iilidx = subiil->getIILIdx();
			if (subiil == this)
			{
				rslt = subiil->nextDocidPrefixSinglePriv(idx, reverse, prefix, docid,keepSearch);
			}
			else
			{
				rslt = subiil->nextDocidPrefixSubSafe(idx, reverse, prefix, docid,keepSearch);
			}

			if (docid != AOS_INVDID)
			{
				// found it
				return true;
			}
			if (!keepSearch)
			{
				break;
			}
			// Chen Ding, 12/11/2010
			// !!!!!!!!!!!!!!!! It should use the condition to retrieve the next IIL.
			if (reverse)
			{
				iilidx --;
			}
			else
			{
				iilidx ++;
			}
			if (iilidx < 0 || iilidx >= mNumSubiils)
			{
				// No more to search
				iilidx = -5;
				idx = -5;
				docid = AOS_INVDID;
				return true;
			}

			subiil = getSubiilByIndex(iilidx, rdata);

			// if we found the next subiil, we need to reset 
			// idx to -10 to search from the beginning
			if (subiil)
			{
				idx = -10;
			}
		}

		docid = AOS_INVDID;
		idx = -5;
		iilidx = -5;
		return true;
	}
	
	// The IIL is not segmented
	bool keepSearch = true;
	iilidx = 0;
	rslt = nextDocidPrefixSinglePriv(idx, reverse, prefix,docid,keepSearch);
	if (idx == -5)
	{
		iilidx = -5;
	}
	return rslt;
}


bool    
AosIILStr::removeDocByPrefixPriv(const OmnString &prefix, const AosRundataPtr &rdata)
{
	// it may cause iil merge, just for torturer
	mIILMergeFlag = true;
	
	// Check whether it is a root iil and it is segmented
	bool rslt = false;
	u64 docid = AOS_INVDID;
	if (isParentIIL())
	{
		//1. get the first place and the last place 
		//2. remove docs between them
		//3. remove unused sub iils
		//4. merge iils if needed

		//1. get the first place and the last place 
		i64 startidx = -10;
		i64 startiilidx = -10;
		i64 endidx = -10;
		i64 endiilidx = -10;
		u64 docid = AOS_INVDID;

		rslt = nextDocidPrefixPriv(startidx, startiilidx, false, prefix, docid, rdata);
		aos_assert_r(rslt, false);

		if (docid == AOS_INVDID)
		{
			// it means no doc is match
			return true;
		}		
		rslt = nextDocidPrefixPriv(endidx, endiilidx, true, prefix, docid, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(startidx >= 0, false);
		aos_assert_r(startiilidx >= 0, false);
		aos_assert_r(endidx >= 0, false);
		aos_assert_r(endiilidx >= 0, false);
		aos_assert_r(endiilidx >= startiilidx, false);
		if (endiilidx == startiilidx)
		{
			aos_assert_r(endidx >= startidx, false);
		}

		//2. remove docs between them
		if (endiilidx ==startiilidx)
		{
			rslt = removeDocsOnOneSub(startiilidx, startidx, endidx, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			// remove the first iil
			rslt = removeDocsOnOneSub(startiilidx, startidx, -10, rdata);
			aos_assert_r(rslt, false);
			// remove the last iil
			rslt = removeDocsOnOneSub(endiilidx, 0, endidx, rdata);
			// remove all in the middle
			for(i64 i=startiilidx+1; i<endiilidx; i++)
			{
				rslt = removeDocsOnOneSub(i, -10, -10, rdata);
				aos_assert_r(rslt, false);
			}			
		}
		
		//3. remove unused sub iils in the middle
		//   Notice that the first iil may contains 0 docs, but we do not handle this case in this block
		// 3.1 process the iils in the middle first
		i64 iil_to_remove = endiilidx - startiilidx - 1;
		if (iil_to_remove > 0)
		{
			for(i64 i = 0;i < iil_to_remove;i++)
			{
				rslt = removeSubiil(startiilidx+1, rdata);
				aos_assert_r(rslt, false);
			}				
		}
		
		// 4 handle the last iil.
		// 4.1 get the first and the last iil out 
		AosIILStrPtr subiil1 = 0;
		AosIILStrPtr subiil2 = 0;
		i64 numDoc1 = 0;
		i64 numDoc2 = 0;
		
		subiil1 = getSubiilByIndex(startiilidx, rdata);
		aos_assert_r(subiil1,false);
		numDoc1 = subiil1->getNumDocs(); 
		if (startiilidx != endiilidx)
		{
			subiil2 = getSubiilByIndex(startiilidx + 1, rdata);
			aos_assert_r(subiil2,false);
			numDoc2 = subiil2->getNumDocs(); 
		}

		// 4.2 remove the last iil if needed
		if (subiil2 && (numDoc2 == 0))
		{
			rslt = removeSubiil(startiilidx + 1, rdata);
			aos_assert_r(rslt,false);
			subiil2 = 0;
			numDoc2 = 0;
		}

		// 3.3 merge the last iil if needed
		if (subiil2)
		{
			// now subiil2 is not empty
			// update subiil2 information first
			updateIndexData(startiilidx+1, true, true);

			// if num of subiil2 is too small, merge it someway
			if (numDoc2 < mMinIILSize)
			{
				//3.3.1 If iilstart + iilend > mMaxNormIILSize , move some doc to iil1
				if (numDoc1 + numDoc2 >= mMaxNormIILSize)
				{
					i64 numToMove = (numDoc1 + numDoc2) / 2 - numDoc2;
					aos_assert_r(numToMove > 0, false); 
					rslt = mergeSubiilForwardPriv(startiilidx, numToMove, rdata);
					aos_assert_r(rslt, false); 
				}
				else
				{
					rslt = mergeSubiilBackwardPriv(startiilidx+1, numDoc2, rdata);
					aos_assert_r(rslt, false); 
				}
			}
		}
		
		// 5. handle the first iil
		// 5.1 get the num of the first iil again cause it may be changed
		numDoc1 = subiil1->getNumDocs();
		if (!isSingleIIL() && numDoc1 > 0)
		{
			updateIndexData(startiilidx, true, true);
		}

		if (!isSingleIIL() && numDoc1 == 0)
		{
			// check whether the startiilidx == 0 or not
			if (startiilidx == 0)
			{
				// move the 2nd iil to the first.
				AosIILStrPtr subiil1 = getSubiilByIndex(1, rdata);
				i64 numToMove = subiil1->getNumDocs();
				rslt = mergeSubiilBackwardPriv(1, numToMove, rdata);
				aos_assert_r(rslt, false); 
				return rslt;
			}
			rslt = removeSubiil(startiilidx, rdata);
			aos_assert_r(rslt,false);
			return true;
		}
		else if (!isSingleIIL() && numDoc1 < mMinIILSize)
		{
			rslt = mergeSubiilPriv(startiilidx, rdata);
			aos_assert_r(rslt,false);
			return true;
		}
		return true;
	}	
	// The IIL is not segmented
	// 1. get the startidx by the prefix,
	// 2. get the endidx by the prefix
	
	i64 startidx = -10;
	i64 endidx = -10;
	bool keepSearch = false;
	rslt = nextDocidPrefixSinglePriv(startidx, false, prefix,docid,keepSearch);
	aos_assert_r(rslt,false);
	if (docid == AOS_INVDID)
	{
		// it means no doc is match
		return true;
	}		
	rslt = nextDocidPrefixSinglePriv(endidx, true, prefix, docid, keepSearch);
	aos_assert_r(rslt,false);
	aos_assert_r(startidx >=0,false);
	aos_assert_r(endidx >=0,false);
	aos_assert_r(endidx >=startidx,false);

	rslt = removeDocFromSubiil2Priv(startidx, endidx, rdata);
	return rslt;
}


bool
AosIILStr::removeDocsOnOneSub(
		const i64 &iilidx, 
		const i64 &startidx, 
		const i64 &endidx, 
		const AosRundataPtr &rdata)
{
	// This function removes some entries from the iil 'iilidx'. The
	// removed are [startidx, endidx]. If 'startidx' is -10, it means to remove
	// starting from the beginning. If 'endidx' is -10, it removes to the end.
	bool rslt = false;
	aos_assert_r(isParentIIL(),false);

	// XXXX: shall we use assert?
	if (iilidx < 0 || iilidx > mNumSubiils -1)
	{
		return false;
	}
	
	AosIILStrPtr subiil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(subiil, false);	
	if (iilidx == 0)
	{
		rslt = subiil->removeDocFromSubiil2Priv(startidx, endidx, rdata);
		return true;
	}
	else
	{
		rslt = subiil->removeDocFromSubiil2Safe(startidx, endidx, rdata);
		return true;
	}
	aos_assert_r(rslt,false);	

	rslt = updateIndexData(iilidx, true, true);

	return true;
}


u64     
AosIILStr::nextDocIdSafe2(
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
		AosIILStrPtr subiil;
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
AosIILStr::nextDocidSafe(
		i64 &idx, 
		i64 &iilidx,
		const bool reverse, 
		const AosOpr opr1,
		const OmnString &value1,
		const AosOpr opr2,
		const OmnString &value2,
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
		AosIILStrPtr subiil;
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
AosIILStr::getTotalSafe(const OmnString &value, const AosOpr opr)
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
				 if (AosIILUtil::valueMatch(mMinVals[i], opr, value.data(), "", mIsNumAlpha))
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
				 if (AosIILUtil::valueMatch(value, opr, mMaxVals[i], "", mIsNumAlpha))
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
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value.data(), "", mIsNumAlpha))
				 {
					 // add total of subiil i
					 if (i == 0) total += mSubiils[i]->getTotalPriv(value,opr);
					 else total += mSubiils[i]->getTotalSafe(value,opr);

					 for (i64 j=i+1; j<mNumSubiils; j++)
					 {
						 if (AosIILUtil::valueMatch(mMaxVals[j], eAosOpr_eq, value.data(), "", mIsNumAlpha))
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
				 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value, "", mIsNumAlpha) &&
				 	 AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, "", mIsNumAlpha))
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

		case eAosOpr_like:
			 // Ken, 2011/5/25
			 for (i64 i=0; i<mNumSubiils; i++)
			 {
				 // add total of subiil i
				 if (i == 0) total += mSubiils[i]->getTotalPriv(value, opr);
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

	total = getTotalPriv(value, opr);
	AOSUNLOCK(mLock);
	return total;
}


i64 
AosIILStr::getTotalPriv(const OmnString &value, const AosOpr opr)
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
		 aos_assert_r(mValues[0], 0);
		 startidx = (AosIILUtil::valueMatch(mValues[0], eAosOpr_gt, value, "", mIsNumAlpha))?0:firstGT(0, value);
		 if (startidx == -1) return 0;
		 return mNumDocs - startidx;

	case eAosOpr_ge:
		 // startidx = (mValues[0].compare1(value) >= 0)?0:firstGE(0, value);
		 aos_assert_r(mValues[0], 0);
		 startidx = (AosIILUtil::valueMatch(mValues[0], eAosOpr_ge, value, "", mIsNumAlpha))?0:firstGE(0, value);
		 endidx = mNumDocs-1;
		 if (startidx == -1) return 0;
		 return mNumDocs - startidx;

	case eAosOpr_eq:
		 aos_assert_r(mValues[0], 0);
		 startidx = firstEQ(0, value);
		 if (startidx < 0)
		 {
			 return 0;
		 }

		 endidx = lastEQ(startidx, value);
		 aos_assert_r(endidx >= startidx && endidx < mNumDocs, 0);
		 return endidx - startidx + 1;

	case eAosOpr_ne:
		 aos_assert_r(mValues[0], 0);
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

	case eAosOpr_like:
		 // Ken, 2011/5/25
		 for (i64 i = 0; i< mNumDocs; i++)
		 {
		 	if (valueLikeMatch(mValues[i], value))
				num++;
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
AosIILStr::modifyDocSafe(
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const bool override_flag,		// Chen Ding, 07/15/2012
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
	rdata->setOverrideFlag(override_flag);
	bool rslt = removeDocPriv(oldvalue, docid, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed to remove: " << docid << ":" << oldvalue << ":"
			<< newvalue << enderr;
	}
	rslt = addDocPriv(newvalue, docid, value_unique, docid_unique, rdata);
	aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);

	if (!rslt)
	{
		OmnAlarm << "Failed to add: " << docid << ":" << oldvalue 
			<< ":" << newvalue << enderr;
	}
	return rslt;
}


bool		
AosIILStr::checkDocSafe(
		const AosOpr opr,
		const OmnString &value,
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
AosIILStr::checkDocSafe(
		const AosOpr opr,
		const OmnString &value,
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
	// AosIILStrPtr subiil = getSubiilByIndex(false, opr, value);	
	AosIILStrPtr subiil = getSubIIL(-10, value, opr, true, rdata);
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
AosIILStr::checkDocPriv(
		const AosOpr opr,
		const OmnString &value,
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

		// SHAWN_WARN
		// keepSearch = (mValues[mNumDocs-1] == value);
		keepSearch = (strcmp(mValues[mNumDocs-1], value.data()) == 0);
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
AosIILStr::removeDocPriv(
		const OmnString &value,
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
		AosIILStrPtr subiil;
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
				//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
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
	if (rslt)
	{
		// Ketty 2013/01/15
		// comp not support yet.
		// remove from compiil
		//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->removeDocSafe(value, docid, rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	// Chen Ding, 12/07/2010
	// No alarms will be raised. The caller is responsible for raising alarms
	// if (!rslt) OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
	return rslt;
}


bool	
AosIILStr::nextDocidPrefixSinglePriv(
		i64 &idx, 
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

	// Chen Ding, 12/11/2010
	if (!mValues)
	{
	 	OmnAlarm << "mValues null" << enderr;
	 	idx = -5;
	 	docid = AOS_INVDID;
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
			return false;
		}
	}

	// Chen Ding, 12/14/2010
	bool startnew = false;
	if (idx == -10)
	{
		if (reverse)
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
		if (idx < 0) 
		{
			idx = -5;
			docid = AOS_INVDID;
			OmnAlarm << "idx: " << idx << enderr;
			return false;
		}

		if (idx == 0 && reverse)
		{
			idx = -5;
			docid = AOS_INVDID;
			return true;
		}

		if (idx > mNumDocs -1)
		{
			// is the last one, we can't find the next
			idx = -5;
			docid = AOS_INVDID;
			return true;
		}

		if (idx == mNumDocs -1 && !reverse)
		{
			// is the last one, we can't find the next
			idx = -5;
			docid = AOS_INVDID;
			return true;
		}

		// not the last one
		if (reverse)
		{
			idx --;
		}
		else
		{
			idx++;
		}
		keepSearch = false;
	}

	// Chen Ding, 08/31/2012
	if (idx < 0)
	{
		OmnAlarm << "Invalid index: " << idx << enderr;
		idx = -5;
		docid = AOS_INVDID;
		return false;
	}

	if (startnew)
	{
		// from head
		if (reverse)
		{
			rslt = nextDocidPrefix(idx, -1, prefix, docid);
		}
		else
		{
			rslt = nextDocidGE(idx, 1, prefix, docid);
		}
		if (!rslt)
		{
			idx = -5;
			docid = AOS_INVDID;
			return false;
		}

		// Chen Ding, 08/31/2012
		if (idx == -5)
		{
			// Did not find the entry
			docid = AOS_INVDID;
			return false;
		}

		// Chen Ding, 08/31/2012
		if (idx < 0)
		{
			OmnAlarm << "Invalid index: " << idx << enderr;
			docid = AOS_INVDID;
			return false;
		}
	}
	
	if (strncmp(mValues[idx],prefix.data(),prefix.length()) == 0)
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


// ken 2011/06/28
// begin
bool
AosIILStr::nextUniqueValuePriv(
		i64 &idx,
		i64 &iilidx,
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
	bool rslt = false;
	found = false;
	if (idx == -10) unique_value = "";
	if (isParentIIL())
	{
		if (idx == -10)
		{
			iilidx = getSubIILIndex(-10, value, opr, !reverse);
			if (iilidx < 0)
			{
				iilidx = -5;
				idx = -5;
				return true;
			}
		}
		else if (idx < 0 || iilidx < 0)
		{
			OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
			idx = -5;
			iilidx = -5;
			return false;
		}

		AosIILStrPtr subiil = getSubiilByIndex(iilidx, rdata);
		if (!subiil)
		{
			OmnAlarm << "subiil null: " << iilidx << ":" << idx << enderr;
			idx = -5;
			iilidx = -5;
			return false;
		}

		while (subiil)
		{
			iilidx = subiil->getIILIdx();
			if (subiil == this)
			{
				rslt = subiil->nextUniqueValueSinglePriv(idx, reverse, opr, value, unique_value, found);
			}
			else
			{
				rslt = subiil->nextUniqueValueSubSafe(idx, reverse, opr, value, unique_value, found);
			}

			if (!rslt) return rslt;

			if (found)
			{
				aos_assert_r(idx >= 0 && iilidx >= 0, false);
				return true;
			}
			
			// Did not find the value in the subiil 'subiil'. Need to continue searching.
			i64 tmpidx = iilidx;
			if (reverse) tmpidx--;
			else tmpidx++;
			
			aos_assert_r(mNumSubiils > 0,false);
			if (tmpidx < 0 || tmpidx >= mNumSubiils)
			{
				iilidx = -5;
				idx = -5;
				return true;
			}

			iilidx = getSubIILIndex(tmpidx, value, opr, !reverse);
			if (iilidx == -5)
			{
				idx = -5;
				return true;
			}
			subiil = getSubiilByIndex(iilidx, rdata);
			if (subiil) idx = -10;
		}
		idx = -5;
		iilidx = -5;
		return true;
	}
	
	// The IIL is not segmented
	iilidx = 0;
	rslt = nextUniqueValueSinglePriv(idx, reverse, opr, value, unique_value, found);
	if (!found) unique_value = "";
	if (idx == -5) iilidx = -5;
	return rslt;
}


bool
AosIILStr::nextUniqueValueSinglePriv(
		i64 &idx,
		const bool reverse,
		const AosOpr opr,
		const OmnString &value,
		OmnString &unique_value,
		bool &found)
{
	found = false;
	if (mNumDocs == 0)
	{
	 	idx = -5;
	 	return true;
	}

	if (!mValues)
	{
		OmnAlarm << "mValues null" << enderr;
	 	idx = -5;
	 	return false;
	}

	if ((idx < 0 && idx != -10) || (idx >= mNumDocs))
	{
		OmnAlarm << "idx: " << idx << ":" << mNumDocs << enderr;
		idx = -5;
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
		idx = -5;
		return true;
	}

	bool rslt;
	int rr = reverse ? -1 : 1;
	if (startnew)
	{
		u64 docid;
		switch (opr)
		{
		case eAosOpr_gt:
			 rslt = nextDocidGT(idx, rr, value, docid);
			 break;

		case eAosOpr_ge:
			 rslt = nextDocidGE(idx, rr, value, docid);
			 break;

		case eAosOpr_eq:
			 rslt = nextDocidEQ(idx, rr, value, docid);
			 break;

		case eAosOpr_lt:
			 rslt = nextDocidLT(idx, rr, value, docid);
			 break;

		case eAosOpr_le:
			 rslt = nextDocidLE(idx, rr, value, docid);
			 break;

		case eAosOpr_ne:
			 rslt = nextDocidNE(idx, rr, value, docid);
			 break;

		case eAosOpr_an:
			 rslt = nextDocidAN(idx, rr, value, docid);
			 break;

		case eAosOpr_like:
			 rslt = nextDocidLike(idx, rr, value, docid);
			 break;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		  	 idx = -5;
		  	 return false;
		}
		if (!rslt || idx < 0 || idx >= mNumDocs)
		{
			idx = -5;
			return rslt;	
		}

		// Found a matched value
		if (unique_value != "")
		{
			//if (strcmp(unique_value.data(), mValues[idx]) != 0)
			if (AosIILUtil::valueMatch(unique_value.data(), mValues[idx], mIsNumAlpha) != 0)
			{
				unique_value.assign(mValues[idx], strlen(mValues[idx]));
				found = true;
				return true;
			}
			idx+=rr;
		}
		else
		{
			unique_value.assign(mValues[idx], strlen(mValues[idx]));
			found = true;
			return true;
		}
	}

	aos_assert_r(unique_value != "", false);
	while (idx >=0 && idx < mNumDocs)
	{
		//if (strcmp(mValues[idx], unique_value.data()) == 0)
		if (AosIILUtil::valueMatch(mValues[idx], unique_value.data(), mIsNumAlpha) == 0)
		{
			idx += rr;
			continue;	
		}
		unique_value.assign(mValues[idx], strlen(mValues[idx]));

		switch (opr)
		{
		case eAosOpr_gt:
		case eAosOpr_ge:
		case eAosOpr_eq:
		case eAosOpr_lt:
		case eAosOpr_le:
			 if (AosIILUtil::valueMatch(mValues[idx], opr, value, "", mIsNumAlpha))
			 {
				 found = true;
				 return true;
			 }
			 idx = -5;
			 return true;

		case eAosOpr_ne:
			 if (AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) != 0)
			 {
				 found = true;
				 return true;
			 }
			 break;

		case eAosOpr_an:
			 found = true;
			 return true;

		case eAosOpr_like:
			 if (valueLikeMatch(mValues[idx], value))
			 {
				 found = true;
				 return true;
			 }
			 break;

		default:
		  	 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		  	 idx = -5;
		  	 return false;
		}
		idx += rr;
	}

	if (idx < 0 || idx >= mNumDocs) idx = -5;
	return true;
}
//end


bool		
AosIILStr::nextDocidSinglePriv(
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
				idx = -5;
				docid = AOS_INVDID;
				isunique = true;
				OmnAlarm << "idx: " << idx << enderr;
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

		case eAosOpr_like:
			 // Ken, 2011/5/25
			 rslt = nextDocidLike(idx, rr, value, docid);
			 break;
		
		case eAosOpr_an:
		 	 rslt = nextDocidAN(idx, rr, value, docid);
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
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value, "", mIsNumAlpha))
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
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value, "", mIsNumAlpha))
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
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value, "", mIsNumAlpha))
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
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value, "", mIsNumAlpha))
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
			 if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value, "", mIsNumAlpha))
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
			 rslt = nextDocidNE(idx, rr, value, docid);
			 checkDocidUnique(rslt, idx, docid, isunique, rdata);
			 return rslt;

		case eAosOpr_an:
			 docid = mDocids[idx];
			 checkDocidUnique(true, idx, docid, isunique, rdata);
			 return true;

		case eAosOpr_like:
		 	 // Ken, 2011/5/25
			 rslt = nextDocidLike(idx, rr, value, docid);
			 break;

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
	isunique = true;
	return false;
}


bool
AosIILStr::checkDocidUnique(
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
		// Chen Ding, 06/26/2011
		hassame = (AosIILUtil::valueMatch(mValues[idx-1], eAosOpr_eq, mValues[idx], "", mIsNumAlpha));
		// hassame = (value == mValues[idx-1]);
	}
	else if (mIILIdx > 0)
	{
		AosIILStrPtr subiil = getNextSubIIL(false, rdata);
		if (subiil.notNull())
		{
			// Chen Ding, 06/26/2011
			hassame = (AosIILUtil::valueMatch(subiil->getLastValue(), eAosOpr_eq, mValues[idx], "", mIsNumAlpha));
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
		hassame = (AosIILUtil::valueMatch(mValues[idx+1], eAosOpr_eq, mValues[idx], "", mIsNumAlpha));
		// hassame = (value == mValues[idx+1]);
	}
	else if (mIILIdx >= 0)
	{
		AosIILStrPtr subiil = getNextSubIIL(true, rdata);
		if (subiil.notNull())
		{
			// Chen Ding, 06/26/2011
			hassame = (AosIILUtil::valueMatch(subiil->getFirstValue(), eAosOpr_eq, mValues[idx], "", mIsNumAlpha));
			// hassame = (value == subiil->getFirstValue());
		}
	}

	if (hassame)
	{
		isunique = false;
	}

	return true;
}


//ken 2011/5/25
bool
AosIILStr::nextDocidLike(
		i64 &idx, 
		const int reverse, 
		const OmnString &value,
		u64 &docid)
{
	if (idx >= 0 && idx < mNumDocs)
	{
		if (reverse == 1)
		{
			for(; idx < mNumDocs; idx++)
			{
				if (valueLikeMatch(mValues[idx], value))
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
				if (valueLikeMatch(mValues[idx], value))
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
		i64 &idx, 
		const int reverse, 
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
AosIILStr::nextDocidGT(
		i64 &idx, 
		const int reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) > 0)
	aos_assert_r(mValues[idx], false);

	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[idx], value.data()) > 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_gt, value, "", mIsNumAlpha))
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
AosIILStr::nextDocidPrefix(
		i64 &idx, 
		const int reverse, 
		const OmnString &prefix,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	aos_assert_r(mValues[idx], false);
	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[idx], value.data()) >= 0)
	if (strncmp(mValues[idx],prefix.data(),prefix.length()) == 0)
	{
		docid = mDocids[idx];
		return true;
	}

	if (reverse == -1)
	{
		idx = firstPrefixRev(idx, prefix);
	}
	else
	{
		idx = firstGE(idx, prefix);
		if (idx > 0 && strncmp(mValues[idx],prefix.data(),prefix.length()) != 0)
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
AosIILStr::nextDocidGE(
		i64 &idx, 
		const int reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] >= value
	// if (mValues[idx].compare1(value) >= 0)
	aos_assert_r(mValues[idx], false);
	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[idx], value.data()) >= 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_ge, value, "", mIsNumAlpha))
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
AosIILStr::nextDocidLT(
		i64 &idx, 
		const int reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		mValues[idx] < value
	// if (mValues[idx].compare1(value) < 0)
	aos_assert_r(mValues[idx], false);
	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[idx], value.data()) < 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_lt, value, "", mIsNumAlpha))
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
AosIILStr::nextDocidLE(
		i64 &idx, 
		const int reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the
	// idx-increasing direction ('reverse' is 1) or the opposite
	// direction for the first one that is:
	// 		value < mValues[idx]
	// if (mValues[idx].compare1(value) <= 0)
	aos_assert_r(mValues[idx], false);
	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[idx], value.data()) <= 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_le, value, "", mIsNumAlpha))
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
AosIILStr::nextDocidEQ(
		i64 &idx, 
		const int reverse, 
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
	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[idx], value.data()) == 0)
	if (AosIILUtil::valueMatch(mValues[idx], eAosOpr_eq, value, "", mIsNumAlpha))
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
AosIILStr::nextDocidNE(
		i64 &idx, 
		const int reverse, 
		const OmnString &value,
		u64 &docid)
{
	// It searches the next doc starting from 'idx' either in the:
	// idx-increasing direction ('reverse' is false) or the opposite
	// direction. Values are ordered (ascending). 
	aos_assert_r(mValues[idx], false);
	int rslt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
	if (rslt != 0)
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
AosIILStr::firstEQ(const i64 &idx, const OmnString &value)
{
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	aos_assert_r(mValues[idx], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);

	int rslt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);

	// If mValues[idx] == value, return idx;
	if (rslt == 0) return idx;

	// If mValues[idx] > value, return -1
	if (rslt > 0) return -1;

	// If mValues[mNumDocs-1] < value, return -1
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], eAosOpr_lt, value, "", mIsNumAlpha)) return -1;

	// Now, we are sure mValues[idx] <= value <= mValues[mNumDocs-1].  
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right || left + 1 == right) 
	{
		aos_assert_r(mValues[right], -1);
		if (AosIILUtil::valueMatch(mValues[right], eAosOpr_eq, value, "", mIsNumAlpha)) return right;
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
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
			if (left == right) return right;
			if (left + 1 == right) 
			{
				if (AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value, "", mIsNumAlpha)) return left;
				return right;
			}
		}
	}

	aos_assert_r(mValues[left], -1);
	if (left == right && AosIILUtil::valueMatch(mValues[left], eAosOpr_eq, value, "", mIsNumAlpha)) return left;
	return -1;
}


i64
AosIILStr::firstEQRev(const i64 &idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!!!
	// Starting from 'idx', it finds the first entry whose
	// value equals to 'value'. If not found, idx = -1.
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[idx], -1);

	// If mValues[idx] == value, return idx
	int rslt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
	if (rslt == 0) return idx;

	// If mValues[idx] is less than 'value', it is not possible
	if (rslt < 0) return -1;

	// If mValues[0] is greater than value, it is not possible
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) > 0) return -1;
		
	// Now, we are sure mValues[0] <= value <= mValues[idx].  
	i64 left = 0; 
	i64 right = idx;
	if (left == right) return -1;
	if (left + 1 == right) 
	{
		aos_assert_r(mValues[right], -1);
		if (AosIILUtil::valueMatch(mValues[right], value, mIsNumAlpha) == 0) return right;
		if (AosIILUtil::valueMatch(mValues[left], value, mIsNumAlpha) == 0) return left;
		return -1;
	}

	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
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
				if (AosIILUtil::valueMatch(mValues[right], value, mIsNumAlpha) == 0) return right;
				return left;
			}
		}
	}

	aos_assert_r(mValues[left], -1);
	if (left == right && AosIILUtil::valueMatch(mValues[left], value, mIsNumAlpha) == 0) return left;
	return -1;
}
	

i64
AosIILStr::firstNERev(const i64 &idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry 
	// where mValues[idx] != value.
	// If not found, idx = -1.
	aos_assert_r(mValues[0], -1);
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) == 0) return -1;
		
	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) != 0) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstNE(const i64 &idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from 'idx', it finds the first entry whose
	// value is not equal to 'value'. If not found, idx = -1.
	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) == 0) return -1;
		
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (right + left) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) != 0) right=nn; else left = nn;
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::lastEQ(const i64 &idx, const OmnString &value) 
{
	// IMPORTANT!!!!!
	// It assumes mValues[idx] == value. 
	// Starting from idx, it searches for the last one that:
	// 		value == mValues[idx]
	// If not found, it returns -1;

	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) == 0) return mNumDocs-1;

	// Now, mValues[idx] <= value < mValues[mNumDocs-1]
	i64 left = idx; 
	i64 right = mNumDocs-1;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) == 0) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::lastLT(const OmnString &value) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] < value
	// If not found, it returns -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) >= 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) < 0) return mNumDocs-1;

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
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) < 0) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::lastLE(const OmnString &value) 
{
	// Starting from 0, it searches for the last one that:
	// 		mValues[idx] <= value
	// If not found, it returns -1;
	aos_assert_r(mValues[0], -1);
	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) > 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) <= 0) return mNumDocs-1;

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
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) <= 0) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstLERev(const i64 &idx, const OmnString &value) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] <= value
	aos_assert_r(mValues[0], -1);
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) > 0) return -1;

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) <= 0) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstLTRev(const i64 &idx, const OmnString &value) 
{
	// IMPORTAT!!!!! 
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	if (AosIILUtil::valueMatch(mValues[0], value, mIsNumAlpha) >= 0) return -1;

	i64 left = 0; 
	i64 right = idx;
	if (left == right-1) return left;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) < 0) left = nn; else right = nn;
		if (left == right-1) return left;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstGTRev(const i64 &idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) <= 0) return -1;
	else return (mNumDocs -1);

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstGERev(const i64 &idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) < 0) return -1;
	else return (mNumDocs -1);

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstPrefixRev(const i64 &idx, const OmnString &prefix)
{
	// IMPORTAT!!!!! 
	aos_assert_r(mValues[0], -1);
	if (strncmp(mValues[0], prefix.data(), prefix.length()) > 0) return -1;

	i64 left = 0; 
	i64 right = idx;
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
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (strncmp(mValues[nn], prefix.data(), prefix.length()) <= 0) left = nn; else right = nn;
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
AosIILStr::firstLT(const i64 &idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	if (AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) >= 0) return -1;
	else return 0;

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstLE(const i64 &idx, const OmnString &value)
{
	// IMPORTAT!!!!!
	// It assumes mValues[idx] > value. Need to move idx backward
	// until it hits:
	// 		mValues[idx] < value
	aos_assert_r(mValues[0], -1);
	if (AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha) > 0) return -1;
	else return 0;

	OmnShouldNeverComeHere;
	return -1;
}

	
i64
AosIILStr::firstGT(const i64 &idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] <= value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] > value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) <= 0) return -1;

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) <= 0) left = nn; else right = nn;
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


i64
AosIILStr::firstGE(const i64 &idx, const OmnString &value)
{
	// IMPORTANT!!!!!!!!
	// It assumes mValues[idx] < value. Need to move idx forward 
	// until it hits:
	// 		mValues[idx] >= value
	
	// 1. Check whether the last one is < 'value'
	aos_assert_r(mValues[mNumDocs-1], -1);
	// Chen Ding, 08/11/2010
	// if (strcmp(mValues[mNumDocs-1], value.data()) < 0) return -1;
	if (AosIILUtil::valueMatch(mValues[mNumDocs-1], value, mIsNumAlpha) < 0) return -1;

	i64 left = idx; 
	i64 right = mNumDocs-1;

	// Now, 'left' points to a bad one and (<value)
	// 'right' points to a good one (>=value). 
	if (left == right-1) return right;
	while (left < right)
	{
		i64 nn = (left + right) >> 1;
		aos_assert_r(mValues[nn], -1);
		if (AosIILUtil::valueMatch(mValues[nn], value, mIsNumAlpha) < 0) left = nn; else right = nn;
		if (left == right-1) return right;
	}

	OmnShouldNeverComeHere;
	return -1;
}


bool 	
AosIILStr::insertBefore(
		const i64 &nn, 
		const u64 &docid, 
		const OmnString &value)
{
	// It inserts an entry in front of 'nn'. 
	if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(nn >= 0, false);

	// Need to move pointers starting at 'nn'
	memmove(&mDocids[nn+1], &mDocids[nn], sizeof(u64) * (mNumDocs - nn));
	memmove(&mValues[nn+1], &mValues[nn], sizeof(char *) * (mNumDocs - nn));

	i64 len = value.length() + 1;
	if (len > AosIILUtil::eMaxStrValueLen) len = AosIILUtil::eMaxStrValueLen;

	char *mem = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
	aos_assert_r(mem, false);
	strncpy(mem, value.data(), len);
	mem[len] = 0;

	// Change mValues
	mValues[nn] = mem;
	mDocids[nn] = docid;

	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


bool 	
AosIILStr::insertAfter(
		const i64 &nn, 
		const u64 &docid, 
		const OmnString &value)
{
	// It inserts an entry after 'nn'. 
	if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
	aos_assert_r(mNumDocs > 0, false);
	aos_assert_r(nn >= 0, false);
	aos_assert_r(nn < mNumDocs, false);

	i64 len = value.length() + 1;
	if (len > AosIILUtil::eMaxStrValueLen) len = AosIILUtil::eMaxStrValueLen;
	if (len <= 0)
	{
		OmnAlarm << "Invalid len: " << len << enderr;
		len = eMinStrLen;
	}

	if (nn < mNumDocs-1)
	{
		// Change mValues
		memmove(&mDocids[nn+2], &mDocids[nn+1], sizeof(u64) * (mNumDocs - nn -1));
		memmove(&mValues[nn+2], &mValues[nn+1], sizeof(char *) * (mNumDocs - nn - 1));
	}
	char *mem = OmnMemMgrSelf->allocate(len+1, __FILE__, __LINE__);
	aos_assert_r(mem, false);
	strncpy(mem, value.data(), len);
	mem[len] = 0;
	mDocids[nn+1] = docid;
	mValues[nn+1] = mem;
	aos_assert_r(checkMemory(), false);
	incNumDocsNotSafe();
	mIsDirty = true;
	return true;
}


u64
AosIILStr::removeDocByIdxPriv(const i64 &theidx, const AosRundataPtr &rdata)
{
	// If 'theidx' is -1, it removes the last entry. Otherwise, it 
	// removes 'theidx'-th entry. 
	if (mNumDocs <= 0) return AOS_INVDID;
	i64 idx = (theidx == -1)?mNumDocs-1:theidx;
	aos_assert_r(idx >= 0 && idx < mNumDocs, AOS_INVDID);

	u64 docid = mDocids[idx];

	// Ketty 2013/01/15
	// comp not support yet.
	//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);				
	//bool rslt = compiil->removeDocSafe(mValues[idx],docid,rdata);
	//returnCompIIL(compiil,rdata);
	//aos_assert_r(rslt, false);

	// The docid to be removed is at 'idx'. Need to remove
	// it from mDocids. 
	aos_assert_r(mValues[idx], false);
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);

	// Change mValues
	if (idx < mNumDocs-1)
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
	if (isParentIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
		AosIILStr *rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
		rootiil->updateIndexData(mIILIdx, updateMax, updateMin);
	}

	mIsDirty = true;
	return docid;
}


bool 		
AosIILStr::removeDocDirectPriv(
		const OmnString &value, 
		const u64 &docid,
		bool &keepSearch,
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
	
	// Chen Ding, 12/07/2010
	//aos_assert_r(mNumDocs > 0, false);
	if (mNumDocs <= 0) return false;

	i64 startidx = 0;
	i64 endidx = mNumDocs -1;
	i64 idx = -1;
	u64 did;
	keepSearch = false;

	// Find the first entry that matches the value
	bool rslt = nextDocidEQ(startidx, 1, value, did);
	if (!rslt)
	{
		// Did not find it.
		return false;
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
	aos_assert_r(idx >= 0 && idx < mNumDocs, false);

	// Change mValues
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);
	if (idx < mNumDocs-1)
	{
		i64 ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx+1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx+1], ss);

		ss = sizeof(char *) * (mNumDocs - idx - 1);
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
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
		AosIILStr *rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);

		if (mNumDocs < mMinIILSize)
		{
			rootiil->mergeSubiilPriv(mIILIdx, rdata);
		}
	}
	
	return true;
}


bool
AosIILStr::firstDoc1(
		i64 &idx, 
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

	case eAosOpr_like:
		 // Ken, 2011/5/25
		 return nextDocidLike(idx, reverse_flag, value, docid);

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 docid = AOS_INVDID;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILStr::saveSanityCheckProtected(const AosRundataPtr &rdata)
{
	static AosIILObjPtr lsTestIIL = OmnNew AosIILStr();
	static OmnMutex lsLock;

	lsLock.lock();
	bool rslt = AosIIL::saveSanityCheck(lsTestIIL, rdata);
	lsLock.unlock();
	return rslt;
}


bool
AosIILStr::addDocPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{

//OmnScreen << "-------------addDocPriv."
//		<< "; iilid:" << mIILID 
//		<< "; iilname:" << mIILName
//		<< "; value:" << value
//		<< "; docid: " << docid
//		<< "; value_unique:" << value_unique
//		<< "; docid_unique:" << docid_unique
//		<< endl;

	aos_assert_r(value != "", false);
	bool rslt = false;
	bool keepSearch = false;
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		AosIILStrPtr subiil;

		// Chen Ding, Bug1217
		i64 iilidx = getSubiilIndex(value);
		// i64 iilidx = getSubIILIndex(-10, value, eAosOpr_ge, false);
		if (iilidx == -5) iilidx = 0;
		// End of Bug1217
		
		aos_assert_r(iilidx >= 0, false);
		aos_assert_r(mNumSubiils > 0, false);
		while (iilidx <= mNumSubiils -1)
		{
			subiil = getSubiilByIndex(iilidx, rdata);
			aos_assert_r(subiil, false);
			keepSearch  = false;
			
			if (subiil.getPtr() == this)
			{
				rslt = addDocDirectPriv(
						value, docid, value_unique, docid_unique, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->addDocDirectSafe(
						value, docid, value_unique, docid_unique, keepSearch, rdata);
			}
			
			if (!keepSearch)
			{
				if (rslt)
				{
					// Ketty 2013/01/15
					// comp not support yet.
					// add to compiil
					//AosIILCompStrPtr compiil = retrieveCompIIL(rdata);
					//aos_assert_r(compiil, false);				
					//bool rslt = compiil->addDocSafe(value,docid,false,false,rdata);
					//returnCompIIL(compiil,rdata);
					//aos_assert_r(rslt, false);
				}
				return rslt;
			}
			iilidx++;
		}
		// raise alarm  lxx
		return false;
	}

	aos_assert_r(isSingleIIL(),false);
	// James, 01/04/2011
	rslt = addDocDirectPriv(
		value, docid, value_unique, docid_unique, keepSearch, rdata);

	if (rslt)
	{
		// Ketty 2013/01/15
		// comp not support yet.
		// add to compiil
		//AosIILCompStrPtr compiil = retrieveCompIIL(rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->addDocSafe(value,docid,false,false,rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}

	return rslt;
}


bool
AosIILStr::addDocDirectPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	keepSearch = false;
	aos_assert_r(value != "", false);
//	AosIILStrPtr r1 = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
//	if (r1)r1->splitSanityCheck();
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// In this case, the doc may be inserted in the next subiil.
	
	// Check whether we need to split

	int rsltInt = 0;
	int rsltInt2 = 0;
	bool rslt = false;
	i64 numBefore = mNumDocs;

	// OmnScreen << "IILStr add: " << this << endl;
	i64 idx = 0;
	i64 left = 0; 
	i64 right = mNumDocs-1;
	if (mNumDocs == 0)
	{
		// Change mValues
		if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
		mDocids[0] = docid;
		aos_assert_r(setValue(0, value.data(), value.length()), false);
		mNumDocs = 1;
		idx = 0;
		goto finished;
	}

	aos_assert_r(mNumDocs <= mMemCap, false);
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
		aos_assert_r(mValues[idx], false);
		rsltInt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
		if (rsltInt == 0)
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

				// Chen Ding, 07/15/2012
				if (rdata->needOverride())
				{
					mDocids[idx] = docid;
					numBefore--;
					goto finished;
				}

				// Chen Ding, 2013/06/02
				// if (rdata->needReportError())
				// {
				// 	rdata->setError()<< "Value already exist: " << value << ":" << docid
				// 		<< ":" << mDocids[idx];
				// 	OmnAlarm << rdata->getErrmsg()<< enderr;
				// }
				
				// Ketty RlbTest tmp.
				//return true;	
				//AosSetErrorU(rdata, "value_already_exist") << value
				//	<< ":" << docid << ":" << mDocids[idx] << ":" << idx << enderr;
				AosSetErrorUser(rdata, __func__) << "value_already_exist" << enderr;
				OmnAlarm << __func__ << enderr;
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
		// if (strcmp(mValues[idx], value.data()) < 0) 
		if (rsltInt < 0) 
		{
			if (idx == mNumDocs-1)
			{
				// Append the docid
				if (mNumDocs >= mMemCap)
					aos_assert_r(expandMemoryPriv(), false);
				mDocids[mNumDocs] = docid;
				// Change mValues
				aos_assert_r(setValue(mNumDocs, value.data(), value.length()), false);
				incNumDocsNotSafe();
				idx = mNumDocs-1;
				goto finished;
			}

			aos_assert_r(mValues[idx+1], false);
			// Chen Ding, 08/11/2010
			// Check whether mValues[idx+1] >= value
			// if (strcmp(value.data(), mValues[idx+1]) < 0)
			rsltInt2 = AosIILUtil::valueMatch(mValues[idx+1], value, mIsNumAlpha);
			if (rsltInt2 > 0)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (rsltInt2 == 0)
			{
				if (value_unique)
				{
					// Chen Ding, 07/15/2012
					if (rdata->needOverride())
					{
						mDocids[idx+1] = docid;
						numBefore--;
						goto finished;
					}

					// Chen Ding, 2013/06/02
					// if (rdata->needReportError())
					// {
					// 	rdata->setError() << "Value already exist: " << value << ":" << docid
					// 		<< ":" << mDocids[idx];
					// 	OmnAlarm << rdata->getErrmsg() << enderr;
					// }
					
					// Ketty RlbTest tmp.
					//return true;
					AosSetErrorU(rdata, "value_already_exist") << value
						<< ":" << docid << ":" << mDocids[idx] << ":" << idx << enderr;
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
		else
		{
			// value < mValues[idx]
			if (idx == 0)
			{
				// Insert into the front
				insertBefore(idx, docid, value);
				goto finished;
			}

			aos_assert_r(mValues[idx-1], false);
			// Chen Ding, 08/11/2010
			// if (strcmp(mValues[idx-1], value.data()) < 0)
			rsltInt2 = AosIILUtil::valueMatch(mValues[idx-1], value, mIsNumAlpha);
			if (rsltInt2 < 0)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				idx--;
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (rsltInt2 == 0)
			{
				if (value_unique)
				{
					// Chen Ding, 07/15/2012
					if (rdata->needOverride())
					{
						mDocids[idx-1] = docid;
						numBefore--;
						goto finished;
					}

					// Chen Ding, 2013/06/02
					// if (rdata->needReportError())
					// {
					// 	rdata->setError()<< "Value already exist: " << value << ":" << docid
					// 		<< ":" << mDocids[idx];
					// 	OmnAlarm << rdata->getErrmsg() << enderr;
					// }
				
					// Ketty RlbTest tmp.
					//return true;
					AosSetErrorU(rdata, "value_already_exist") << value
						<< ":" << docid << ":" << mDocids[idx] << ":" << idx << enderr;
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
	// Ketty temp 2012/11/28
	//aos_assert_r(numBefore + 1 == mNumDocs, false);
	mIsDirty = true;
	if (!isSingleIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);

		AosIILStr *rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
		rootiil->updateIndexData(mIILIdx, updateMax, updateMin);
	}

	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILObjPtr subiil;
		rslt = splitListPriv(subiil, rdata);
	}
	sanityTestForSubiils();
//	AosIILStrPtr r = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
//	if (r)r->splitSanityCheck();
	return true;
}


bool
AosIILStr::sanityTestForSubiils()
{
//	for(i64 i = 0; i < mNumDocs - 1; i++)
//	{
//if (AosIILUtil::valueMatch(mValues[i] ,eAosOpr_gt,mValues[i+1], mIsNumAlpha))
//{
//	i64 a = 0;
//	a++;
//}
//		aos_assert_r(AosIILUtil::valueMatch(mValues[i] ,eAosOpr_le,mValues[i+1], mIsNumAlpha),false);
//	}
	return true;
}



bool
AosIILStr::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 

	i64 newsize = mNumDocs + mExtraDocids;
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char *) * newsize);

	if (mValues)
	{
		// Copy the contents from the old buffer to 'mem'
		// and release the old memory. Note that this function
		// may be called by loadFromFilePriv(buff). This function
		// will set mNumDocs based on the contents read from
		// the file. In this case, 'mNumDocs' does not really mean
		// that 'mValues[]' has that many values. For this reason,
		// we should copy only min(mNumDocs, mMemCap).
		memcpy(mem, mValues, sizeof(char *) * mMemCap);
		OmnDelete [] mValues;
	}

	mValues = mem;
	aos_assert_r(expandDocidMem(), false);
	aos_assert_r(checkMemory(), false);
	return true;
}


bool
AosIILStr::prepareMemoryForReloading()
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

	if (mNumDocs < mMemCap)
	{
		// It has enough memory. If it has more than what we need, 
		// thrink it. We only need 'mNumDocs + mExtraDocids'. Release
		// the extra ones.
		for (i64 i=mNumDocs + mExtraDocids; i<mMemCap; i++)
		{
			if (!mValues[i]) break;
			
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			mValues[i] = 0;
		}

		// IMPORTANT: the memory held in 'mValues[i]' are for 
		// the previous IIL, not this IIL's!!!
		return true;
	}

	// Grow 'mValues' now
	i64 newsize = mNumDocs + mExtraDocids;
	char **mem = OmnNew char *[newsize];
	aos_assert_r(mem, false);
	memset(mem, 0, sizeof(char *) * newsize);

	if (mValues)
	{
		memcpy(mem, mValues, sizeof(char *) * mMemCap);
		OmnDelete [] mValues;
		mValues = 0;
	}

	mValues = mem;
	bool rslt = expandDocidMem();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILStr::splitListPriv(AosIILObjPtr &subiil, const AosRundataPtr &rdata)
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
	subiil = AosIILMgrSelf->createSubIILSafe(mIILID, mSiteid, mSnapShotId, eAosIILType_Str, mIsPersis, rdata);
	aos_assert_r(subiil, false);
	aos_assert_r(subiil->getIILType() == eAosIILType_Str, false);
	AosIILStrPtr newsubiil = dynamic_cast<AosIILStr *>(subiil.getPtr());
	//ken 2011/07/25
	if (mIsNumAlpha) newsubiil->setNumAlpha();

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
	aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
	AosIILStr *rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
	aos_assert_r(mIILIdx >= 0, false);

	bool rslt = newsubiil->initSubiil(&mDocids[startidx], &mValues[startidx], len, mIILIdx+1, rootiil);
	aos_assert_r(rslt, false);

	// 5. Shrink this IIL
	memset(&mValues[startidx], 0, sizeof(char *) * len);
	memset(&mDocids[startidx], 0, sizeof(u64) * len);

	// ModifyNumDocs
	mNumDocs = startidx;
	mIsDirty = true;
	aos_assert_r(mNumDocs < mMaxNormIILSize, false);

	// 6. Add the subiil
	AosIILStrPtr thisptr(this, false);
	aos_assert_r(rootiil->addSubiil(thisptr, newsubiil), false);
	newsubiil->setDirty(true);

	AosIILStrPtr r = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
//	r->splitSanityCheck();
	return true;
}


bool
AosIILStr::splitSanityCheck()
{
	for(i64 i = 0;i < mNumSubiils;i++)
	{
		if (mSubiils[i])
		{
			aos_assert_r(OmnString(mMaxVals[i]) == mSubiils[i]->getMaxValue(),false);
			aos_assert_r(OmnString(mMinVals[i]) == mSubiils[i]->getMinValue(),false);
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
AosIILStr::insertDocPriv(
		i64 &idx, 
		const OmnString &value, 
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
	aos_assert_r(mValues[0], false);
	aos_assert_r(mValues[mNumDocs-1], false);
	i64 left = firstEQ(0, value);
	i64 right = firstEQRev(mNumDocs-1, value);

	aos_assert_r(left >= 0 && left < mNumDocs, false);
	aos_assert_r(right >= left && right < mNumDocs, false);

	if (docid == mDocids[left])
	{
		if (docid_unique)
		{
			// Ketty temp 2013/01/09
			//OmnAlarm << "Not unique: " << docid << ":" << value << ":" << mIILID << enderr;
			//return false;
			return true;
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
		if (right == mNumDocs-1 && mIILIdx >= 0 && mRootIIL &&
			mRootIIL->getNumSubiils() > 0 &&
			mIILIdx != mRootIIL->getNumSubiils() -1)
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
				// Ketty temp 2012/11/28
				if (mIILID == 15086 && value == "50003")         return true;
		
				// Ketty temp 2013/01/09
				//OmnAlarm << "Not unique: " << docid 
				//		<< ":" << value << ":" << mIILID << enderr;
				//return false;
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
AosIILStr::listSanityCheckPriv()
{
	// The class should have been locked. It checks all the values are 
	// sorted based on the value. For entries whose values are the
	// same, they are sorted based on docids.
	if (mNumDocs == 0) 
	{
		return true;
	}

	aos_assert_r(mValues[0], false);
	for (i64 i=1; i<mNumDocs; i++)
	{
		aos_assert_r(mValues[i], false);
		int rslt = AosIILUtil::valueMatch(mValues[i-1], mValues[i], mIsNumAlpha);
		aos_assert_r(rslt <= 0, false);

		if (rslt < 0) continue;

		if (rslt == 0)
		{
			aos_assert_r(mDocids[i-1] <= mDocids[i], false);
		}
	}

	return true;
}


AosBuffPtr 
AosIILStr::getBodyBuffProtected() const
{
	// This function is called when we need to save the IIL.
	aos_assert_r(AosIILStrSanityCheck(this), 0);

	i64 expect_size = mNumDocs * 120; 
	if (expect_size <= 5000) expect_size = 5000;
	AosBuffPtr buff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

// OmnString docid_str, value_str;
	// Save mDocids
	//for (i64 i=0; i<mNumDocs; i++)
	//{
	//	buff->setU64(mDocids[i]);
//docid_str << mDocids[i] << "; ";
//if (i % 10 == 0) docid_str << " \n";
	//}
	
	bool rslt = buff->setU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, 0);

	// Save Subiil Index Data, if needed
	buff->setI64(mNumSubiils);

	// save it but not use it.
	//buff->setI64(mIILIdx);
	// now this place is be used by isNumAlpha flag. 
	buff->setU8(mIsNumAlpha);

	if (isParentIIL())
	{
		// Save the subiil index data
		/*
		for (i64 i=0; i<mNumSubiils; i++)
		{
			buff->setCharStr(mMinVals[i], strlen(mMinVals[i]));
			buff->setCharStr(mMaxVals[i], strlen(mMaxVals[i]));
			//buff->setU64(mIILIds[i]);
			//buff->setI64(mNumEntries[i]);
			aos_assert_r(mIILIds[i]>0, 0);
		}*/

		for (i64 i=0; i<mNumSubiils; i++)
		{
			buff->setCharStr(mMinVals[i], strlen(mMinVals[i]));
			buff->setCharStr(mMaxVals[i], strlen(mMaxVals[i]));
		}
		
		rslt = buff->setU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, 0);

		rslt = buff->setI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, 0);
	}

	// Save Values
	aos_assert_r(mNumDocs <= mMaxNormIILSize, 0);
	for (i64 i=0; i<mNumDocs; i++)
	{
		if (!mValues[i])
		{
			OmnAlarm << "This is a serious problem: " << mNumDocs << ":" << i 
				<< ":" << mWordId << enderr;
		}
		else
		{
			buff->setCharStr(mValues[i], strlen(mValues[i]));
		}

//value_str << mValues[i] << "; ";
//if (i % 10 == 0) value_str << " \n";
	}

	// now we do not mix the CompIIL content into strIIL buff
	// save compIIL ID ( instead of saving it's content)

//	buff->setU64(mCompIIL->getIILID());
	return buff;
}


bool
AosIILStr::setContentsProtected(
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
	mNumSubiils = buff->getI64(0);
	//aos_assert_r(mNumSubiils >= 0, false);
	if (mNumSubiils < 0 || mNumSubiils > mMaxSubIILs)
	{
		OmnAlarm << "Failed to retrieve mNumSubiils:" << mNumSubiils << enderr;
		mNumSubiils = 0;
		return false;
	}

	// This place used to save IILIdx, now we get it out but not use it
	if (mNumSubiils > 1)
	{
		mIILIdx = 0;
	}
	else
	{
		mIILIdx = -1;
	}

	//now this place is used to be isNumAlpha flag;
	mIsNumAlpha = buff->getU8(0);
	
	if (mNumSubiils > 1)
	{
		mRootIIL = this;
	}

	int len;
	char *data;
	if (mNumSubiils > 0)
	{
		if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
		mSubiils[0] = this;

		/*for (i64 i=0; i<mNumSubiils; i++)
		{
            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMinVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMinVals[i][len] = 0;

            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMaxVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMaxVals[i][len] = 0;

            //mIILIds[i] = buff->getU64(0);
			aos_assert_r(mIILIds[i], false);

			//mNumEntries[i] = buff->getI64(0);
			aos_assert_r(mNumEntries[i] >= 0, false);
		}*/

		for (i64 i=0; i<mNumSubiils; i++)
		{
            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
			aos_assert_r(AosCopyMemStr(&mMinVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMinVals[i][len] = 0;

            data = buff->getCharStr(len);
            aos_assert_r(data && len>0 && len <= AosIILUtil::eMaxStrValueLen, false);
            aos_assert_r(AosCopyMemStr(&mMaxVals[i], data, len + 1, __FILE__, __LINE__), false);
			mMaxVals[i][len] = 0;
		}

		rslt = buff->getU64s(mIILIds, mNumSubiils);
		aos_assert_r(rslt, false);

		rslt = buff->getI64s(mNumEntries, mNumSubiils);
		aos_assert_r(rslt, false);
	}

	for (i64 i=0; i<mNumDocs; i++)
	{
		data = buff->getCharStr(len);
		// Chen Ding, 09/04/2010
		// if (len <= 0 || !data)
		if (len < 0 || !data)
		{
			// u32 len;
			// char *ww = AosWordClient::getSelf()->getWord(mWordId, len);
			// OmnString word(ww, len);
			OmnAlarm << "Failed to retrieve the data. Data possibly corrupted: " << enderr;
			
			// This means it failed parsing the retrieved data. 
			// Need to set 'mNumDocs' to whatever has been parsed.
			setNumDocsNotSafe(0);
			return false;
		}

		if (!setValue(i, data, len))
		{
			// u32 len;
			// char *ww = AosWordClient::getSelf()->getWord(mWordId, len);
			// OmnString word(ww, len);
			OmnAlarm << "Failed to set dta " << i << ":" << enderr;
			setNumDocsNotSafe(0);
			return false;
		}
	}

	// load compIIL from file
//	u64 compIILID = buff->getU64(AOS_INVIILID);

	rslt =	AosIILStrSanityCheck(this);
	aos_assert_r(rslt, false);
	return true;
}


bool		
AosIILStr::nextDocidSinglePriv(
		i64 &idx, 
		const bool reverse, 
		const AosOpr opr1,
		const OmnString &value1,
		const AosOpr opr2,
		const OmnString &value2,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata) 
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

	aos_assert_r(mValues[idx], false);
	if (reverse)
	{
		// In the reverse order. Moving from 'idx' (including)
		// until it hits the right edge (i.e., value2 <= mValues[idx]). 
		// If it moves out of the left edge (i.e., mValues[idx] <= value1)
		// it means there is no more.
		if (AosIILUtil::valueMatch(mValues[idx], opr2, value2, "", mIsNumAlpha))
		{
			// It enters the right edge. Check whether it is out of
			// the left edge.
			if (AosIILUtil::valueMatch(mValues[idx], opr1, value1, "", mIsNumAlpha))
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

		if (AosIILUtil::valueMatch(mValues[idx], opr1, value1, "", mIsNumAlpha))
		{
			// Found one
			docid = mDocids[idx];
			goto found_wrap;
		}

		// No more
		idx = -5;
		docid = AOS_INVDID;
		isunique = true;
		return true;
	}
	
	// Normal order. Moving from 'idx' (including)
	// until it hits the left edge (i.e., value1 >= mValues[idx]). 
	// If it moves out of the right edge (i.e., value2 >= mValues[idx])
	// it means there is no more.
	if (AosIILUtil::valueMatch(mValues[idx], opr1, value1, "", mIsNumAlpha))
	{
		// It enters the left edge. Check whether it is out of
		// the right edge.
		if (AosIILUtil::valueMatch(mValues[idx], opr2, value2, "", mIsNumAlpha))
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
	if (AosIILUtil::valueMatch(mValues[idx], opr2, value2, "", mIsNumAlpha))
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
	// Chen Ding, 06/26/2011
	checkDocidUnique(true, idx, docid, isunique, rdata);
	return true;
}


AosIILStrPtr
AosIILStr::getSubiil(
		const bool reverse,
		const AosOpr opr1,
		const OmnString &value1, 
		const AosOpr opr2,
		const OmnString &value2,
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
			if (mNumEntries[i]>0 && AosIILUtil::valueMatch(mMinVals[i], opr2, value2, "", mIsNumAlpha))
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
		if ( mNumEntries[i]>0 && AosIILUtil::valueMatch(value1, opr1, mMaxVals[i], "", mIsNumAlpha))
		{
			// Found the sub-iil
			return getSubiilByIndex(i, rdata);
		}
	}
	return 0;
}


AosIILStrPtr
AosIILStr::getSubiilByIndex(
		const i64 &idx,
		const AosRundataPtr &rdata)
{
	// It retrieves the idx-th sub-iil. Note that 0th subiil
	// is the root iil. This function should be called by
	// the root iil. 
	//
	// The function assumes 'mLock' should have been locked.
	// Chen Ding, 12/14/2010
	// aos_assert_r(mIILIdx == 0, 0);
	if (mRootIIL)
	{
		aos_assert_r(mRootIIL.getPtr() == this, 0);
	}

	if (idx == 0) 
	{
		aos_assert_r(mRootIIL, 0);
//		if (!mRootIIL) mRootIIL = this;
		return this;
	}

	aos_assert_r(idx > 0 && idx < mNumSubiils, 0);
	if (!mSubiils)
	{
		mSubiils = OmnNew AosIILStrPtr[mMaxSubIILs];
		aos_assert_r(mSubiils, 0);
	}

	if (mSubiils[idx]) return mSubiils[idx];

	// Retrieve an IIL from IILMgr
	aos_assert_r(mIILIds[idx], 0);

	// Chen Ding, 05/05/2011
	// bool status;
	// AosIILObjPtr iil = AosIILMgrSelf->loadIILByIDSafe(mIILIds[idx], 
	// 		AOS_INVWID, eAosIILType_Str, 0, 0, status);
	AosIILType type= eAosIILType_Str;
	AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(
		mIILIds[idx], mSiteid, mSnapShotId, type, rdata);
	if (!iil)
	{
		OmnAlarm << "Can not get the iil: " 
			<< mIILIds[idx] << ":" << type << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		OmnAlarm << "Not a string IIL: " 
			<< iil->getIILType() << enderr;
		AosIILMgrSelf->returnIILPublic(iil, rdata);
		return 0;
	}
	mSubiils[idx] = dynamic_cast<AosIILStr *>(iil.getPtr());
	mSubiils[idx]->mRootIIL = this;
	mSubiils[idx]->mIILIdx = idx;

	aos_assert_r(mSubiils[0], 0);
	return mSubiils[idx];
}


bool 
AosIILStr::removeDocFromSubiil(
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
		i64 ss = sizeof(char *) * (newsize);
		OmnMemCheck(mValues, ss);
		memmove(mValues, &(mValues[numRemove]), ss);
		ss = sizeof(char *)*numRemove;
		OmnMemCheck(&mValues[newsize], ss);
		memset(&(mValues[newsize]), 0, ss);

		// copy docid
		ss = sizeof(u64) * (newsize);
		OmnMemCheck(mDocids, ss); 
		memmove(mDocids, &(mDocids[numRemove]), ss);
		ss = sizeof(u64)*numRemove;
		OmnMemCheck(&mDocids[newsize], ss);
		memset(&(mDocids[newsize]), 0, ss);
	}
	else // del from tail
	{
		OmnMemCheck(&mValues[newsize], sizeof(char *) * numRemove);
		memset(&(mValues[newsize]), 0, sizeof(char *) * numRemove);
		OmnMemCheck(&mDocids[newsize], sizeof(u64) * numRemove);
		memset(&(mDocids[newsize]), 0, sizeof(u64) * numRemove);		
	}
	
	mNumDocs = newsize;
	mIsDirty = true;
	return true;
}


bool 
AosIILStr::removeDocFromSubiil2Priv(
		const i64 &start_idx,
		const i64 &end_idx,
		const AosRundataPtr &rdata)
{
	if (mNumDocs == 0)
	{
		return false;
	}
	
	i64 startidx = start_idx;
	i64 endidx = end_idx;
	if (startidx == -10) startidx = 0;
	if (endidx == -10) endidx = mNumDocs -1;

	aos_assert_r(0 <= startidx, false);
	aos_assert_r(startidx <= endidx, false);
	aos_assert_r(endidx < mNumDocs, false);

	// Ketty 2013/01/15
	// comp not support yet.
	//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);				
	//bool rslt = false;
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

	i64 numremove = endidx - startidx + 1;
	i64 newsize = mNumDocs - numremove;

	i64 ss = sizeof(char *) * (mNumDocs - endidx - 1);
	OmnMemCheck(&mValues[endidx+1], ss);
	OmnMemCheck(&mValues[startidx], ss);

	memmove(&(mValues[startidx]), &(mValues[endidx+1]), ss);

	ss = sizeof(char *)*numremove;
	OmnMemCheck(&mValues[mNumDocs - numremove], ss);
	memset(&(mValues[mNumDocs - numremove]), 0, ss);

	// copy docid
	ss = sizeof(u64) * (mNumDocs - endidx - 1);
	OmnMemCheck(&mDocids[endidx+1], ss);
	OmnMemCheck(&mDocids[endidx], ss);
	memcpy(&(mDocids[startidx]), &(mDocids[endidx+1]), ss);

	ss = sizeof(char *) * numremove;
	OmnMemCheck(&mValues[mNumDocs - numremove], ss);
	memset(&(mValues[mNumDocs - numremove]), 0, ss);
	
	mNumDocs = newsize;
	mIsDirty = true;
	return true;
}


bool 
AosIILStr::appendDocToSubiil(
		char **values, 
		u64 *docids, 
		const i64 &numDocs,
		const bool addToHead) 
{
	// Notice that we use memcpy to copy string point list to the new subiil, 
	// So we can NOT use delete in the original subiil, instead, we use 
	// memset to clear that memory.
	aos_assert_r(mIILType == eAosIILType_Str, false);

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
			memmove(&(mValues[numDocs]), mValues, sizeof(char *) * mNumDocs);
			memcpy(mValues, values, sizeof(char *) * numDocs);

			// copy docid
			memmove(&(mDocids[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mDocids, docids, sizeof(u64) * numDocs);
		}	
		else
		{
			// copy value
			memcpy(&mValues[mNumDocs], values, sizeof(char *) * numDocs);			

			// copy docid
			memcpy(&mDocids[mNumDocs], docids, sizeof(u64) * numDocs);			
		}
	}
	else
	{
		// for values
		OmnAlarm << "mem expand: should not happen " << enderr;
		char **mem = OmnNew char *[newNumDocs + mExtraDocids];
		aos_assert_r(mem, false);
		memset(mem, 0, sizeof(char *) * (newNumDocs + mExtraDocids));
		// for docids
		u64 *mem2 = OmnNew u64[newNumDocs + mExtraDocids];
		aos_assert_r(mem2, false);
		memset(mem2 , 0, sizeof(u64) * (newNumDocs + mExtraDocids));
	
		mMemCap = newNumDocs;

		if (addToHead)
		{
			// copy value
			memmove(&(mem[numDocs]), mValues, sizeof(char *) * mNumDocs);
			memcpy(mem, values, sizeof(char *) * numDocs);
			// copy docid
			memmove(&(mem2[numDocs]), mDocids, sizeof(u64) * mNumDocs);
			memcpy(mem2, docids, sizeof(u64) * numDocs);
			
		}
		else
		{
			// copy value
			memmove(mem, mValues, sizeof(char *) * mNumDocs);			
			memcpy(&(mem[mNumDocs]), values, sizeof(char *) * numDocs);			
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
AosIILStr::initSubiil(
		u64 *docids,
		char **values,
		const i64 &numDocs,
		const i64 &subiilidx,
		const AosIILStrPtr &rootiil)
{
	aos_assert_r(mIILType == eAosIILType_Str, false);
	aos_assert_r(rootiil, false);

	mRootIIL = rootiil.getPtr();
    mWordId = AOS_INVWID;
    mNumDocs = numDocs; 
	// ModifyNumDocs
	aos_assert_r(mNumDocs < mMaxNormIILSize, false);

	if (mMinVals)
	{
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mMinVals[i])
			{
				OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			}
		}
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if (mMaxVals)
	{
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mMaxVals[i])
			{
				OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
			}
		}
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


//	mCompIIL->resetData();
	// Prepare the memory
	bool rslt = prepareMemoryForReloading();
	aos_assert_r(rslt, false);
	memcpy(mValues, values, sizeof(char *) * mNumDocs);
	memcpy(mDocids, docids, sizeof(u64) * mNumDocs);
	mIsDirty = true;
	return true;
}


bool
AosIILStr::addSubiil(
		const AosIILStrPtr &crtsubiil,
		const AosIILStrPtr &newsubiil)
{
	// It inserts the new subiil 'newsubiil' after 'crtsubiil'. This function
	// must be called by the root IIL.
	aos_assert_r(mIILIdx == 0, false);
	aos_assert_r(mNumSubiils < mMaxSubIILs-1, false);
	if (!mSubiils) aos_assert_r(createSubiilIndex(), false);
	i64 iilidx = crtsubiil->getIILIdx();
	aos_assert_r(iilidx >= 0, false);

	i64 num_to_move = mNumSubiils - iilidx - 1;
	// 1. Insert the new subiil in mSubiils[]
	if (iilidx < mNumSubiils-1)
	{
		//ken 2012/11/12
		// this place must use for loop
		//memmove(&mSubiils[iilidx+2], &mSubiils[iilidx+1], sizeof(AosIILStrPtr) * (mNumSubiils-iilidx-1));
		//memset(&mSubiils[iilidx+1], 0, sizeof(AosIILStrPtr));
        for (int i=num_to_move; i>0; i--)
		{
			mSubiils[iilidx+i+1] = mSubiils[iilidx+i];
		}
		mSubiils[iilidx+1] = 0;

        memmove(&mMinVals[iilidx+2], &mMinVals[iilidx+1], sizeof(char *) * (mNumSubiils-iilidx-1));
		mMinVals[iilidx+1] = 0;

        memmove(&mMaxVals[iilidx+2], &mMaxVals[iilidx+1], sizeof(char *) * (mNumSubiils-iilidx-1));
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


// get sub iil functions for add and remove 
i64
AosIILStr::getSubiilIndex(const OmnString &value)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
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
            if (AosIILUtil::valueMatch(mMaxVals[left], eAosOpr_ge, value, "", mIsNumAlpha))
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

        if (AosIILUtil::valueMatch(mMaxVals[cur], eAosOpr_ge, value, "", mIsNumAlpha))
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


i64
AosIILStr::getSubiilIndexSeq(
		const OmnString &value,
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
	// It searches the index to determine the subiil
	// that 'value' falls in the value range defined by the subiil.
	aos_assert_r(mSubiils, 0);
    i64 left = iilidx>0? iilidx:0;
    i64 right = mNumSubiils - 1;
    if (right <= left)
	{
    	return right;
	}
	
	if (AosIILUtil::valueMatch(mMaxVals[left], eAosOpr_ge, value, "", mIsNumAlpha))
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
            if (AosIILUtil::valueMatch(mMaxVals[left], eAosOpr_ge, value, "", mIsNumAlpha))
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

        if (AosIILUtil::valueMatch(mMaxVals[cur], eAosOpr_ge, value, "", mIsNumAlpha))
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


// get sub iil functions for add and remove 
AosIILStrPtr
AosIILStr::getSubiil(const OmnString &value, const AosRundataPtr &rdata)
{
	i64 index = getSubiilIndex(value);
	aos_assert_r(mNumSubiils > 0, 0);
	aos_assert_r(index >= 0 && index <= mNumSubiils - 1, 0);
	return getSubiilByIndex(index, rdata);
}


AosIILStrPtr
AosIILStr::getSubiilSeq(const OmnString &value, i64 &iilidx, const AosRundataPtr &rdata)
{
	iilidx = getSubiilIndexSeq(value, iilidx, rdata);
	aos_assert_r(mNumSubiils > 0, 0);
	aos_assert_r(iilidx >= 0 && iilidx <= mNumSubiils -1, 0);
	return getSubiilByIndex(iilidx, rdata);
}


u64
AosIILStr::getMinDocid() const
{
	aos_assert_r(false, AOS_INVDID);
	aos_assert_r(mNumDocs > 0, AOS_INVDID);
// shawnpro
	OmnNotImplementedYet;
	return 0;
}


u64
AosIILStr::getMaxDocid() const
{
	aos_assert_r(false, AOS_INVDID);
	aos_assert_r(mNumDocs > 0, AOS_INVDID);
// shawnpro
	OmnNotImplementedYet;
	return 0;
}


const char *
AosIILStr::getMinValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mValues[0];
}


const char *
AosIILStr::getMaxValue() const
{
	aos_assert_r(mNumDocs > 0, 0);
	return mValues[mNumDocs-1];
}


bool
AosIILStr::checkMemory() const
{
	// It assumes the memory was allocated by memory pool, which 
	// has some extra space. In the first round, it sets the index
	// in the extra space for each entry. It then goes to the second
	// round to see whether any element's index is altered by 
	// someone else. If yes, it means the entry was shared by 
	// one elements.
	if (!sgCheckMemory) return true;
	if (!mValues) return true;
	for (i64 i=0; i<mNumDocs; i++)
	{
		aos_assert_r(mValues[i], false);
		char *ptr = &mValues[i][-4];
		int *size = (int*)ptr;
		u32 *idx = (u32*)&ptr[*size+AosMemPool::eUserDataOffset];
		*idx = i;
	}

	for (i64 i=0; i<mNumDocs; i++)
	{
		char *ptr = &mValues[i][-4];
		int *size = (int*)ptr;
		u32 *idx = (u32*)&ptr[*size+AosMemPool::eUserDataOffset];
		aos_assert_r(*idx == i, false);
	}
	return true;
}


bool
AosIILStr::updateIndexData(const i64 &idx,const bool changeMax,const bool changeMin)
{
	// This function updates the index data based on the
	// entry mValues[idx]. Index data are stored in root iil.
	setDirty(true);
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < mNumSubiils, false);
	aos_assert_r(mSubiils[idx], false);

	AosIILStrPtr subiil = mSubiils[idx];
	mNumEntries[idx] = subiil->getNumDocs();
	if (changeMax)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		AosCopyMemStr(&mMaxVals[idx], subiil->getMaxValue(), __FILE__, __LINE__);
	}

	if (changeMin)
	{
		aos_assert_r(mNumEntries[idx] > 0, false);
		AosCopyMemStr(&mMinVals[idx], subiil->getMinValue(), __FILE__, __LINE__);
	}

	return true;
}


bool
AosIILStr::createSubiilIndex()
{
	aos_assert_r(!mSubiils, false);

	mSubiils = OmnNew AosIILStrPtr[mMaxSubIILs];
	aos_assert_r(mSubiils, false);

	mMinVals = OmnNew char*[mMaxSubIILs];
	aos_assert_r(mMinVals, false);
	memset(mMinVals, 0, sizeof(char *) * mMaxSubIILs);

	mMaxVals = OmnNew char*[mMaxSubIILs];
	aos_assert_r(mMaxVals, false);
	memset(mMaxVals, 0, sizeof(char *) * mMaxSubIILs);

	mIILIds = OmnNew u64[mMaxSubIILs];
	aos_assert_r(mIILIds, false);
	memset(mIILIds, 0, sizeof(u64) * mMaxSubIILs);

	mNumEntries = OmnNew i64[mMaxSubIILs];
	aos_assert_r(mNumEntries, false);
	memset(mNumEntries, 0, sizeof(i64) * mMaxSubIILs);

	return true;
}


i64
AosIILStr::getDocidsSafe(
		const AosOpr opr,
		const OmnString &value,
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
	// Chen Ding, 12/12/2010
	i64 firstSubiilIdx = getSubIILIndex(-10, value, opr, true);
	// 2. get the last sub iil
	// Chen Ding, 12/12/2010
	i64 lastSubiilIdx = getSubIILIndex(-10, value, opr, false);
	// 3. get the docids from the first to the last
	i64 counter = 0;
	AosIILStrPtr curSubiil = 0;
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


i64
AosIILStr::getSubIILIndexByPrefix(
		const i64 &startidx, 
		const OmnString &prefix,
		const bool fromHead)
{
	if (fromHead)
	{
		if (prefix.length() == 0)return 0;
		return getSubIILIndex(startidx, prefix, eAosOpr_ge, true);
	}
	
	// from tail
	if (prefix.length() == 0)return (mNumSubiils -1);
	for (i64 i= mNumSubiils -1; i>=0; i--)
	{
		if (strncmp(mMinVals[i],prefix.data(),prefix.length()) <= 0)
		{
			return i;
		}
	}
	return 0;
}


// Chen Ding, 12/12/2010
// This function searches for the first subiil that 
// passes the condition [value, opr]. If 'startidx'
// is -10, it starts from the beginning. Otherwise, 
// it is the subiil from which the search starts. 
i64
AosIILStr::getSubIILIndex(
		const i64 &startidx, 
		const OmnString &value,
		const AosOpr opr,
		const bool fromHead)
{
	//ken 2012/05/14
	if (opr == eAosOpr_prefix)
	{
		return getSubIILIndexByPrefix(startidx, value, fromHead);
	}
	
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

		aos_assert_r(mNumSubiils > 0,0);
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
			  	 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_gt, value, "", mIsNumAlpha)) return i;
			 }
			 return -5;

		case eAosOpr_ge:
			 // Find the first one whose value <= max. If it runs out of 
			 // the loop, there is no subiil for it.
			 for(i64 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, "", mIsNumAlpha)) return i;
			 }
			 return -5;
				 
		case eAosOpr_eq:
			 // Find the first one that max >= value. If it runs out of
			 // the loop, there is no subiil for it.
			 for (i64 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, "", mIsNumAlpha))
				 {
					 // If value < min, not possible
					 if (AosIILUtil::valueMatch(value, eAosOpr_lt, mMinVals[i], "", mIsNumAlpha)) return -5;
					 return i;
				 }
			 }
			 return -5;

		case eAosOpr_lt: 
			 // If the first min < value, return 0. Otherwise, return -5.
			 if (AosIILUtil::valueMatch(mMinVals[idx], eAosOpr_lt, value, "", mIsNumAlpha)) return idx;
			 return -5;
			
		case eAosOpr_le:
			 // If the first min <= value, return 0. Otherwise, return -5.
			 if (AosIILUtil::valueMatch(mMinVals[idx], eAosOpr_le, value, "", mIsNumAlpha)) return idx;
			 return -5;
	
		case eAosOpr_ne:
			 // Chen Ding, 12/12/2010
			 // if (AosIILUtil::valueMatch(mMinVals[0], eAosOpr_ne, value, mIsNumAlpha)) return 0;
			 for(i64 i=idx; i<mNumSubiils; i++)		// Chen Ding, 12/12/2010
			 {
				 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_ne, value, "", mIsNumAlpha)) return i;
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ne, value, "", mIsNumAlpha)) return i;
			 }
			 // for (i64 i=0; i < mNumSubiils; i++)
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
			 /*for(i64 i=idx; i<mNumSubiils; i++)
			 {
				 AosIILStrPtr subiil = getSubiil(i);
				 aos_assert_r(subiil, -5);
				 
				 if (subiil->valueLikeMatch(value, false))
					 return i;
			 }
			 return -5;*/
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

	aos_assert_r(mNumSubiils > 0, false);
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
		 // Chen Ding, 12/12/2010
		 // if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_le, value, mIsNumAlpha)) return -5;
		 // return mNumSubiils-1;
		 if (AosIILUtil::valueMatch(mMaxVals[idx], eAosOpr_le, value, "", mIsNumAlpha)) return -5;
		 return idx;

	case eAosOpr_ge:
		 // Chen Ding, 12/12/2010
		 // if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_lt, value, mIsNumAlpha)) return -5;
		 // return mNumSubiils-1;
		 // Chen Ding, Bug1217
		 if (AosIILUtil::valueMatch(mMaxVals[idx], eAosOpr_lt, value, "", mIsNumAlpha)) return -5;
		 return idx;

	case eAosOpr_eq:
		 // Chen Ding, 12/12/2010
		 // for (i64 i=mNumSubiils-1; i>=0; i--)
		 for (i64 i=idx; i>=0; i--)
		 {
			 // Find the first one whose min <= value. It then 
			 // checks whether max >= value, return that one. 
			 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value, "", mIsNumAlpha))
			 {
				 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ge, value, "", mIsNumAlpha)) return i;
				 return -5;
			 }
		 }
		 return -5;

	case eAosOpr_lt:
		 // Chen Ding, 12/12/2010
		 // for (i64 i=mNumSubiils-1; i>=0; i--)
		 for (i64 i=idx; i>=0; i--)
		 {
			// Find the first one whose min < 'value'. 
			if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_lt, value, "", mIsNumAlpha)) return i;
		 }
		 return -5;

	case eAosOpr_le:
		 // Chen Ding, 12/12/2010
		 // for (i64 i=mNumSubiils-1; i>=0; i--)
		 for (i64 i=idx; i>=0; i--)
		 {
		  	 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_le, value, "", mIsNumAlpha)) return i;
		 }
		 return -5;

	case eAosOpr_ne:
		 // Chen Ding, 12/12/2010
		 // if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_ne, value, mIsNumAlpha)) return mNumSubiils-1;
		 for(i64 i=idx; i>= 0; i--)		// Chen Ding, 12/12/2010
		 {
			 if (AosIILUtil::valueMatch(mMinVals[i], eAosOpr_ne, value, "", mIsNumAlpha)) return i;
			 if (AosIILUtil::valueMatch(mMaxVals[i], eAosOpr_ne, value, "", mIsNumAlpha)) return i;
		 }
		 //if (AosIILUtil::valueMatch(mMaxVals[mNumSubiils-1], eAosOpr_ne, value, mIsNumAlpha)) return 0;
		 // for (i64 i= mNumSubiils-1; i >=0 ;i--)
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
		 /*for(i64 i=idx; i>=0; i--)
		 {
			 AosIILStrPtr subiil = getSubiil(i);
			 aos_assert_r(subiil, -5);
				 
			 if (subiil->valueLikeMatch(value, true))
			 	return i;
		 }
		 return -5;*/
		 return idx;

	default:
		 OmnAlarm << "Unrecognized operator: " << opr << enderr;
		 return -5;
	}

	OmnShouldNeverComeHere;
	return -5;
}

/*
bool
AosIILStr::valueLikeMatch(
		const OmnString &value,
		const bool reverse)
{
	if (reverse)
	{
		for(i64 i=mNumDocs; i>=0; i--)
		{
			if (valueLikeMatch(mValues[i], value))
			{
				return true;
			}
		}
	}
	else
	{
		for(i64 i=0; i<mNumDocs; i++)
		{
			if (valueLikeMatch(mValues[i], value))
			{
				return true;
			}
		}
	}
	return false;
}
*/

bool
AosIILStr::valueLikeMatch(
		const char *v,
		const OmnString &value)
{
	const char * r = strstr(v, value.data());
	return r != NULL;
}


i64
AosIILStr::getDocidsPriv(
		const AosOpr opr,
		const OmnString &value,
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

	// Chen Ding, 10/24/2010
	// endidx = mNumDocs-1;
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


u64
AosIILStr::incrementDocidSafe(
		const OmnString &key, 
		const u64 &dft, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the value identified by 'key'. 
	// 'key' should uniquely identifies an entry. Otherwise, it is
	// an error. It then increments the docid by one and returns
	// the current value of the docid.
	AOSLOCK(mLock);
	i64 startidx = -1;
	i64 iilidx = 0;
	u64 docid;
	bool isunique = true;
	bool rslt = nextDocidPriv(
			startidx, iilidx, false, eAosOpr_eq, key, docid, isunique, rdata);
	aos_assert_rb(rslt, mLock, -1);

	if (mNumSubiils == 0)
	{
		aos_assert_rb(startidx >= 0 && startidx < mNumDocs, mLock, dft);
		docid = mDocids[startidx];
		aos_assert_rb(isunique, mLock, dft);
		mDocids[startidx]++;
		mIsDirty = true;

		// Ketty 2013/01/15
		// comp not support yet.
		//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);
		//bool rslt = compiil->removeDocSafe(key,docid,rdata);
		//if (!rslt)
		//{
		//	returnCompIIL(compiil,rdata);
		//	aos_assert_r(rslt, false);
		//}
		//rslt = compiil->addDocSafe(key,docid+1,false,false,true,rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);

		AOSUNLOCK(mLock);
		return docid;
	}

	AOSUNLOCK(mLock);
	OmnNotImplementedYet;
	return 0;
}


bool
AosIILStr::incrementDocidSafe(
		const OmnString &key,
		u64 &value,
		const u64 &incvalue,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	// This function retrieves the value identified by 'key'. 
	// If the entry does not exist, it checks 'add_flag'. If it
	// is true, it will add it. 
	// It increments the docid by 'incvalue' and returns
	// the old value (i.e., the value before incrementing) of the docid.
	AOSLOCK(mLock);
	i64 startidx = -10;
	i64 iilidx = -10;
	u64 docid;
	bool isunique = true;
	bool rslt = nextDocidPriv(
			startidx, iilidx, false, eAosOpr_eq, key, docid, isunique, rdata);
	aos_assert_rb(rslt, mLock, false);
	
	// Chen Ding, 01/27/2012
	if (iilidx == 0)
	{
		aos_assert_r(startidx >= 0 && startidx < mNumDocs, false);
	}
	
	if (iilidx == -5)
	{
		// Does not exist. 
		if (add_flag)
		{
			// Need to add it.
			value = init_value + incvalue;
			bool rslt = addDocPriv(key, value, true, true, rdata);
			aos_assert_rb(rslt, mLock, false);	
			value = init_value;
		}
		else
		{
		    AosSetError(rdata, AosErrmsgId::eValueNotFound);
		    OmnAlarm << rdata->getErrmsg() << ". Key: " << key 
			     << ". IILID: " << mIILID << enderr;
			AOSUNLOCK(mLock);
		    return false;
		}
	}
	else
	{
		value = docid+incvalue;
		bool rslt = removeDocPriv(key, docid, rdata);
		aos_assert_rb(rslt, mLock, false);
		rslt = addDocPriv(key, value, true, true, rdata);
		aos_assert_rb(rslt, mLock, false);	
		value = docid;
	}

	/*if (mNumSubiils == 0)
	{
		aos_assert_rb(startidx >= 0 && startidx < mNumDocs, mLock, false);
		docid = mDocids[startidx];
		aos_assert_rb(isunique, mLock, false);
		mDocids[startidx] += incvalue;
		AOSUNLOCK(mLock);
		return true;
	}*/

	AOSUNLOCK(mLock);
	return true;
}


// Ketty 2012/11/19
/*
bool
AosIILStr::saveSubIILToTransFileSafe(
		const AosDocTransPtr &docTrans, 
		const bool forcesafe, 
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (isParentIIL())
	{
		// this iil is the root iil of the list
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			if (mSubiils[i].notNull())
			{
				rslt = mSubiils[i]->saveToTransFileSafe(docTrans, forcesafe, rdata);
				aos_assert_r(rslt,false);
			}
		}
	}
	return true;
}
*/

bool
AosIILStr::saveSubIILToLocalFileSafe(const AosRundataPtr &rdata)
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
AosIILStr::returnSubIILsPriv(
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
	returned = true;
	if (!isParentIIL())
	{
		return true;
	}

	AosIILStrPtr subiil = 0;
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
			rslt = AosIILMgrSelf->returnIILPublic(subiil, subIILReturned, returnHeader, rdata);
			aos_assert_r(rslt, false);
			aos_assert_r(subIILReturned, false);
			mSubiils[i] = 0;
		}
	}
	return true;
}


AosIILStrPtr
AosIILStr::getNextSubIIL(const bool forward, const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mRootIIL.isNull())
	{
		return 0;
	}

	aos_assert_r(mRootIIL->getNumSubiils() > 0,0);
	aos_assert_r(mIILIdx >= 0,0);
	if (forward)
	{
		rslt = mIILIdx < (mRootIIL->getNumSubiils() - 1);
	}
	else
	{
		rslt = mIILIdx > 0;
	}

	if (!rslt)
	{
		return 0;
	}

	aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str,0);
	AosIILStrPtr rootIIL = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());

	if (forward)
	{
		return rootIIL->getSubiilByIndex(mIILIdx + 1, rdata);
	}
	else
	{
		return rootIIL->getSubiilByIndex(mIILIdx - 1, rdata);
	}
}


bool
AosIILStr::resetSubIILInfo(const AosRundataPtr &rdata)
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
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mMinVals[i])
			{
				OmnMemMgrSelf->release(mMinVals[i], __FILE__, __LINE__);
			}
		}
		OmnDelete [] mMinVals;
		mMinVals = 0;
	}

	if (mMaxVals)
	{
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			if (mMaxVals[i])
			{
				OmnMemMgrSelf->release(mMaxVals[i], __FILE__, __LINE__);
			}
		}
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
	mIILIdx = -1;
	return true;
}


bool
AosIILStr::resetSpec()
{
	// Ken Lee, 2013/01/11
	mIsNumAlpha = false;
	if (mNumDocs != 0)
	{
		aos_assert_r(mValues, false);
		for(i64 i = 0; i < mNumDocs;i++)
		{
			aos_assert_r(mValues[i],false);
			OmnMemMgrSelf->release(mValues[i], __FILE__, __LINE__);
			mValues[i] = 0;
		}
	}

	OmnDelete [] mValues;
	mValues = 0;
	return true;
}


u64
AosIILStr::getDocIdSafe1(i64 &idx,i64 &iilidx, const AosRundataPtr &rdata)
{
	u64 did = 0;
	AOSLOCK(mLock);
	did = getDocIdPriv1(idx,iilidx, rdata);
	AOSUNLOCK(mLock);
	return did;
}


u64
AosIILStr::getDocIdPriv1(i64 &idx, i64 &iilidx, const AosRundataPtr &rdata)
{
	if (!isParentIIL())
	{
		u64 docid = getDocIdPriv(idx,rdata);
		return docid;
	}

	// is rootiil
	AosIILStrPtr subiil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iilidx >= 0, 0);
	aos_assert_r(mNumSubiils > 0, 0);
	u64 docid = 0;
	while (subiil)
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
		iilidx++;
		idx = 0;
		subiil = getSubiilByIndex(iilidx, rdata);
	}
	return 0;
}


bool
AosIILStr::verifyDuplicatedEntriesPriv(const AosRundataPtr &rdata)
{
	OmnScreen << "\nVerify duplicated entries: Total subiils: " << mNumSubiils << endl;
	for (i64 i=0; i<mNumSubiils; i++)
	{
		AosIILStrPtr crt_subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(crt_subiil, false);
		i64 crt_numdocs = crt_subiil->mNumDocs;

		char** crt_values = crt_subiil->mValues;

		for (i64 mm=0; mm<crt_numdocs; mm++)
		{
			const char *vv = crt_values[mm];
			u64* crt_docids = crt_subiil->mDocids;
			for (i64 kk=mm+1; kk<crt_numdocs; kk++)
			{
				//if (strcmp(vv, crt_values[kk]) == 0)
				if (AosIILUtil::valueMatch(vv, crt_values[kk], mIsNumAlpha) == 0)
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
				AosIILStrPtr next_subiil = getSubiilByIndex(aa, rdata);
				char **values = next_subiil->mValues;
				i64 entries = next_subiil->mNumDocs;
				for (i64 bb=0; bb<entries; bb++)
				{
					//if (strcmp(vv, values[bb]) == 0)
					if (AosIILUtil::valueMatch(vv, values[bb], mIsNumAlpha) == 0)
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


OmnString
AosIILStr::getRandomValuePriv(u64 &docid, const AosRundataPtr &rdata)
{
	docid = 0;
	char **values;
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
		AosIILStrPtr subiil = getSubiilByIndex(iilidx, rdata);
		aos_assert_r(subiil, "");
		values = subiil->mValues;
		docids = subiil->mDocids;
		num_docs = subiil->mNumDocs;
	}

	if (num_docs == 0) return "";
	i64 nn = rand() % num_docs;
	docid = docids[nn];
	return values[nn];
}


// Chen Ding New
bool
AosIILStr::copyData(
        char** values,
        u64 *docids,
        const i64 &buff_len,
        i64 &num_docs, // num_docs is in/out param
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
        AosIILStrPtr subiil = getSubiilByIndex(ss, rdata);
//        aos_assert_r(idx + subiil->mNumDocs < total, false);

//		doc_copied = doc_limit;
        rslt = subiil->copyDataSingle(&(values[idx]),&(docids[idx]),doc_limit,doc_copied);
		aos_assert_r(rslt, false);
		idx += doc_copied;
		doc_limit -= doc_copied;
	
/*	
		for (i64 mm=0; mm<subiil->mNumDocs; mm++)
        {
            strcpy(values[idx], mValues[mm]);
            docids[idx] = mDocids[mm];
            idx++;
        }
*/
    }
    num_docs = idx;
    return true;
}

bool
AosIILStr::copyDataSingle(
        char** values,
        u64 *docids,
        const i64 &buff_len,
        i64 &num_docs)
{
    i64 idx = 0;
    for (i64 mm=0; mm<mNumDocs; mm++)
    {
		aos_assert_r(idx < buff_len, false);
		aos_assert_r(strlen(mValues[mm])< AosIILUtil::eMaxStrValueLen, false);
        strcpy(values[idx], mValues[mm]);
        docids[idx] = mDocids[mm];
        idx++;
    }
    num_docs = idx;
    return true;
}


bool
AosIILStr::queryValueSinglePriv(
		vector<OmnString> &values,
		const AosOpr opr,
		const OmnString &value,
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
		rslt = nextDocidSinglePriv(idx_start, false, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		if (idx_start < 0) return true;
		
		rslt = nextDocidSinglePriv(idx_end, true, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
	}

	for(i64 i = idx_start; i <= idx_end; i++)
	{
		if (unique_value && i == 0 && (!values.empty()) && values.back() == (OmnString)(mValues[0]))
		{
			continue;
		}
		//if (unique_value && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if (unique_value && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
		values.push_back(mValues[i]);
	}
	return true;
}


bool
AosIILStr::queryValueLikeSinglePriv(
		vector<OmnString> &values,
		const OmnString &value,
		const bool unique_value)
{
	for(i64 i=0; i<=mNumDocs-1; i++)
	{
		if (unique_value && i == 0 && (!values.empty()) && values.back() == OmnString(mValues[0]))
		{
			continue;
		}
		//if (unique_value && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if (unique_value && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
	 	if (!valueLikeMatch(mValues[i], value))
		{
			continue;
		}
		values.push_back(mValues[i]);
	}
	return true;
}


bool
AosIILStr::querySinglePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	if (opr == eAosOpr_ne)
	{
		return querySingleNEPriv(query_rslt,query_bitmap,value, rdata);
	}
	else if (opr == eAosOpr_like)
	{
		return querySingleLikePriv(query_rslt,query_bitmap,value,true, rdata);
	}
	
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;
	
	if (opr == eAosOpr_an)
	{
		idx_start = 0;
	}
	else
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value, docid, isunique, rdata);
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
		rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt,false);
	}


	// count num of docs
	if (query_rslt.notNull())
	{
		// i64 num_docs = idx_end - idx_start + 1;
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
AosIILStr::queryValueNEPriv(
		vector<OmnString> &values,
		const OmnString &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return queryValueNESinglePriv(values, value, unique_value, false, rdata);
	}

	bool rslt = false;
	AosIILStrPtr subiil;
	
	i64 iilidx_start = getSubIILIndex(-10, value, eAosOpr_eq, true);
	if (iilidx_start < 0)
	{
		for(i64 i = 0;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, rdata);
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
		
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		rslt = subiil->queryValueNESinglePriv(values, value, unique_value, getall, rdata);
		aos_assert_r(rslt, false);
	}
	if (iilidx_end == iilidx_start)
	{
		iilidx_end++;
	}
	for(i64 i = iilidx_end;i < mNumSubiils;i++)
	{
		if (i != iilidx_end)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}
		
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		rslt = subiil->queryValueNESinglePriv(values, value, unique_value, getall, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosIILStr::queryValueLikePriv(
		vector<OmnString> &values,
		const OmnString &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return queryValueLikeSinglePriv(values, value, unique_value);
	}

	for(i64 i = 0;i < mNumSubiils;i++)
	{
		AosIILStrPtr subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		subiil->queryValueLikeSinglePriv(values, value, unique_value);
	}
	return true;
}


bool
AosIILStr::queryNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	
	if (idx_start >= 0)
	{	
		rslt = nextDocidPriv(idx_end, iilidx_end, true, eAosOpr_eq, value, docid, isunique, rdata);
		aos_assert_r(rslt,false);
		aos_assert_r(idx_end >= 0 && iilidx_end >= 0,false);
	}

	// count num of docs
	i64 num_docs = 0;

	aos_assert_r(mNumSubiils > 0,false);
	if (idx_start < 0)
	{
		// Copy all the list out
		for(i64 i = 0;i < mNumSubiils -1;i++)
		{
			num_docs += mNumEntries[i];
		}
	}
	else // idx_start >= 0
	{
		aos_assert_r(idx_end >= 0,false);
		for(i64 i = 0;i < iilidx_start;i++)
		{
			num_docs += mNumEntries[i];
		}
		num_docs += idx_start;
		num_docs += mNumEntries[iilidx_end] - idx_end - 1;
		for(i64 i = iilidx_end +1; i < mNumSubiils; i++)
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
	AosIILStrPtr subiil;
	if (idx_start < 0)
	{
		// copy all data
		for(i64 i = 0;i < mNumSubiils;i++)
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
	else// there's some data, which's value == value
	{
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
	
		for(i64 i = iilidx_end;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			aos_assert_r(subiil.notNull(),false);


			start = (i == iilidx_end)?idx_end+1:-10;
			end = -10;

			if (i == iilidx_end && start > mNumEntries[iilidx_end] - 1)continue;

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
	
	// if (query_rslt.notNull())
	// {
	// 		query_rslt->setOrdered(false);
	// }
	return true;
}


bool
AosIILStr::queryLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(isParentIIL(),false);
	aos_assert_r(mNumSubiils > 0, false);
	bool rslt = false;
	
	i64 num_docs = 0;
	for(i64 i = 0;i < mNumSubiils -1;i++)
	{
		num_docs += mNumEntries[i];
	}
	
	if (query_rslt.notNull())
	{
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}

	// copy data
	AosIILStrPtr subiil;
	for(i64 i = 0;i < mNumSubiils;i++)
	{
		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(),false);
		if (i == 0)
		{
			rslt = subiil->querySingleLikePriv(query_rslt, query_bitmap, value, false, rdata);
			aos_assert_r(rslt,false);
		}
		else
		{
			rslt = subiil->querySingleLikeSafe(query_rslt, query_bitmap, value, false, rdata);
			aos_assert_r(rslt,false);
		}
	}
	return true;
}


bool
AosIILStr::querySingleNEPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;

	if (mNumDocs <= 0)return true;
	
	rslt = nextDocidPriv(idx_start, iilidx_start, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	if (idx_start >= 0)
	{
		rslt = nextDocidPriv(idx_end, iilidx_end, true, eAosOpr_eq, value, docid, isunique, rdata);
		aos_assert_r(rslt,false);
	}
	
	// count num of docs
	// i64 num_docs = 0;
	// if (idx_start < 0)
	// {
	// 	num_docs = mNumDocs;
	// }
	// else
	// {
	// 	num_docs = mNumDocs - (idx_end - idx_start + 1);
	// }
	// 	
	// if (query_rslt)
	// {
	// 	// Chen Ding, 08/02/2011
	// 	// query_rslt->setBlockSize(num_docs);
	// }
	
	// copy data
	if (idx_start < 0)
	{
		rslt = copyDocidsPriv(query_rslt,query_bitmap,0,mNumDocs -1);
		aos_assert_r(rslt,false);
		// num_docs = mNumDocs;
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
	
	// if (query_rslt.notNull())
	// {
	// 		query_rslt->setOrdered(false);
	// }
	return true;
}


bool
AosIILStr::queryValueNESinglePriv(
		vector<OmnString> &values,
		const OmnString &value,
		const bool value_unique,
		const bool getall,
		const AosRundataPtr &rdata)
{
	if (mNumDocs <= 0) return true;
	
	i64 idx_start = -10;
	i64 idx_end = -10;
	u64 docid;
	bool isunique = true;

	bool rslt = nextDocidSinglePriv(idx_start, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	if (idx_start < 0 || getall)
	{
		for(i64 i=0; i<=mNumDocs-1;i++)
		{
			if (value_unique && i == 0 && (!values.empty()) && values.back() == (OmnString)(mValues[0]))
			{
				continue;
			}
			//if (value_unique && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
			if (value_unique && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
			{
				continue;
			}
			values.push_back(mValues[i]);
		} 
		return true;
	}

	rslt = nextDocidSinglePriv(idx_end, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt,false);
	
	for(i64 i= 0;i <= (idx_start - 1);i++)
	{
		if (value_unique && i == 0 && (!values.empty()) && values.back() == (OmnString)(mValues[0]))
		{
			continue;
		}
		//if (value_unique && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if (value_unique && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
		values.push_back(mValues[i]);
	} 
	
	for(i64 i= (idx_end + 1);i <= (mNumDocs - 1);i++)
	{
		if (value_unique && i == 0 && (!values.empty()) && values.back() == (OmnString)(mValues[0]))
		{
			continue;
		}
		//if (value_unique && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if (value_unique && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
		values.push_back(mValues[i]);
	} 
	return true;
}


bool
AosIILStr::queryValuePriv(
		vector<OmnString> &values,
		const AosOpr opr,
		const OmnString &value,
		const bool value_unique,
		const AosRundataPtr &rdata)
{
	if (opr == eAosOpr_ne)
	{
		return queryValueNEPriv(values, value, value_unique, rdata);
	}

	if (opr == eAosOpr_like)
	{
		return queryValueLikePriv(values, value, value_unique, rdata);
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

	AosIILStrPtr subiil;
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

		subiil = getSubiilByIndex(i, rdata);
		aos_assert_r(subiil.notNull(),false);

		rslt = subiil->queryValueSinglePriv(values, opr, value, value_unique, getall, rdata);
		aos_assert_r(rslt,false);
	}
	return true;
}


bool
AosIILStr::querySingleLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const OmnString &value,
		const bool setBlockSize,
		const AosRundataPtr &rdata)
{
	bool rslt = false;

	if (mNumDocs <= 0)return true;
	
	if (setBlockSize && query_rslt)
	{
		// i64 num_docs = mNumDocs;
		// Chen Ding, 08/02/2011
		// query_rslt->setBlockSize(num_docs);
	}
	
	for(i64 i = 0;i < mNumDocs ;i++)
	{
	 	if (valueLikeMatch(mValues[i], value))
		{
			rslt = copyDocidsPriv(query_rslt,query_bitmap,i,i);
			aos_assert_r(rslt,false);
		}
	}
	return true;
}


bool
AosIILStr::queryPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosOpr opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return querySinglePriv(query_rslt,query_bitmap,opr,value, rdata);
	}

	if (opr == eAosOpr_ne)
	{
		return queryNEPriv(query_rslt,query_bitmap,value, rdata);
	}
	else if (opr == eAosOpr_like)
	{
		return queryLikePriv(query_rslt,query_bitmap,value, rdata);
	}
	
	bool rslt = false;
	i64 idx_start = -10;
	i64 iilidx_start = -10;
	i64 idx_end = -10;
	i64 iilidx_end = -10;
	u64 docid;
	bool isunique = true;
	
	if (opr == eAosOpr_an)
	{
		idx_start = 0;
		iilidx_start = 0;
	}
	else
	{
		rslt = nextDocidPriv(idx_start, iilidx_start, false, opr, value, docid, isunique, rdata);
		if (!rslt || idx_start < 0 || iilidx_start < 0)
		{
			// This means that it did not find anything.
			if (query_rslt.notNull())
			{
				// Chen Ding, 08/02/2011
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
		rslt = nextDocidPriv(idx_end, iilidx_end, true, opr, value, docid, isunique, rdata);
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
	AosIILStrPtr subiil;
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
AosIILStr::queryRangePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

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
	OmnString startValue = query_context->getStrValue();
	OmnString endValue = query_context->getStrValue2();
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
		aos_assert_r(rslt, false);

		// copy data
		rslt = copyDocidsPriv(query_rslt, query_bitmap, idx_start, idx_end);
		aos_assert_r(rslt, false);

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
	AosIILStrPtr subiil;
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
AosIILStr::docidSortedSanityCheck(const i64 &crtidx)
{
	OmnString vv = mValues[crtidx];
	
	// 1. Find the first entry with the same value.
	i64 start_idx = crtidx;
	for (i64 i=crtidx-1; i>=0; i--)
	{
		// SHAWN_WARN
		// if (mValues[i] != vv) break;
		if (vv != mValues[i]) break;
		start_idx = i;
	}

	i64 end_idx = crtidx;
	for (i64 i=crtidx+1; i<mNumDocs; i++)
	{
		// SHAWN_WARN
		// if (mValues[i] != vv) break;
		if (vv != mValues[i]) break;
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


/*
}
bool
AosIILStr::query(AosQueryRsltObjPtr *query_rslt,
				 const AosOpr opr,
				 const OmnString &value)
{
	aos_assert_r((*query_rslt).notNull(),false);

	i64 startidx = -10;
	i64 endidx = -10;
	aos_assert_r(firstDoc1(startidx, false, opr, value, did), false);
	aos_assert_r(firstDoc1(endidx, true, opr, value, did), false);
	aos_assert_r(startidx >= 0 && startidx <= mNumDocs - 1,false);
	aos_assert_r(endidx >= startidx && endidx <= mNumDocs - 1,false);
	i64 num_doc = endidx - startidx + 1;
	(*query_rslt)->setBlock(&(mDocids[startidx]),num_doc);
	return true;
}
*/


bool
AosIILStr::incrementCounter(
		const OmnString &counter_id, 
		const u64 &value,
		const AosRundataPtr &rdata)
{
	// This function increments the counter 'counter_id' by 'value'. 
	// If the counter is not in the list yet, it will add it.
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	OmnString ctid = counter_id;
	u64 did = 0;
	AOSLOCK(mLock);
	bool rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, ctid, did, isunique, rdata);
	if (ctid != counter_id)
	{
		OmnAlarm << "Counter ID changed: " << ctid << ":" << counter_id << enderr;
		AOSUNLOCK(mLock);
		return false;
	}

	if (!rslt)
	{
		// Did not find it. 
		rslt = addDocPriv(counter_id, value, true, false, rdata);
		AOSUNLOCK(mLock);
		return rslt; 
	}

	// Found it. Need to modify the value
	bool rslt1 = removeDocPriv(counter_id, did, rdata);
	if (!rslt1)
	{
		OmnAlarm << "Failed to remove: " << counter_id<< ":" << did << ":"
			<< value<< enderr;
	}
	u64 counter = did + value;
	rslt = addDocPriv(counter_id, counter, true, false, rdata);
	/*
	AosIILStrPtr subiil = getSubiil(iilidx);
	if (!subiil)
	{
		OmnAlarm << "Internal error!" << enderr;
		AOSUNLOCK(mLock);
		return false;
	}

	if (idx < 0 || idx >= subiil->mNumDocs)
	{
		OmnAlarm << "Internal error: " << idx << ":" << subiil->mNumDocs << enderr;
		AOSUNLOCK(mLock);
		return false;
	}

	subiil->mDocids[idx] += value;
	*/
	AOSUNLOCK(mLock);
	return rslt;
}


bool 		
AosIILStr::mergeSubiilPriv(const i64 &iilidx, const AosRundataPtr &rdata)
{
	// This function should be called on mRootIIL. The iil 'iilidx' is too 
	// small. The function will re-arrange the contents as follows:
	// 1. If 'iilidx-1' and 'iilidx' together < mMaxNormIILSize, average them
	// 2. Otherwise, if 'iilidx' and 'iilidx+1' < mMaxNormIILSize, average them
	// 3. Otherwise, 
	// for torturer
	mIILMergeFlag = true;

	aos_assert_r(isParentIIL(),false);	
	aos_assert_r(mNumSubiils > 1,false);
	bool rslt = false;

	//1. If there is a iil around, two iils contains more than 	
	//   mMaxNormIILSize docs, merge some docs from that iil to this one.
	AosIILStrPtr iil = getSubiilByIndex(iilidx, rdata);
	aos_assert_r(iil,false);
	i64 numdoc1 = iil->getNumDocs();
	i64 numdoc2 = 0; // num doc prev
	i64 numdoc3 = 0; // num doc next
	i64 merge_size = mMaxNormIILSize / 2 + mMinIILSize;
	if (iilidx > 0)
	{
		AosIILStrPtr iil2 = getSubiilByIndex(iilidx - 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc2 = iil2->getNumDocs();

		if (numdoc1 + numdoc2 >= merge_size)
		{
			// iilidx-1 and iilidx are too 
			// move some docs from iil2 to iil1
			i64 nummove = (numdoc1 + numdoc2)/2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilForwardPriv(iilidx - 1, nummove, rdata);
			aos_assert_r(rslt, false);
			return rslt;
		}
	}
	
	aos_assert_r(mNumSubiils > 1, false);
	if (iilidx < mNumSubiils - 1)
	{
		AosIILStrPtr iil2 = getSubiilByIndex(iilidx + 1, rdata);
		aos_assert_r(iil2,false);		
		numdoc3 = iil2->getNumDocs();

		if (numdoc1 + numdoc3 >= merge_size)
		{
			i64 nummove = (numdoc1 + numdoc3) / 2 - numdoc1;
			aos_assert_r(nummove > 0, false); 
			rslt = mergeSubiilBackwardPriv(iilidx + 1, nummove, rdata);
			aos_assert_r(rslt, false);
			return true;
		}
	}
	
	aos_assert_r(mNumSubiils > 1, false);
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
AosIILStr::mergeSubiilForwardPriv(
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
	aos_assert_r(isParentIIL(),false);
	aos_assert_r(mNumSubiils > 0 ,false);
	aos_assert_r(iilidx < mNumSubiils -1,false);
	aos_assert_r(mNumEntries[iilidx] >= numDocToMove,false);		
	if (iilidx == 0)
	{
		aos_assert_r(mNumEntries[iilidx] >  numDocToMove,false);		
	}

	// 2. move entries from a to b
	AosIILStrPtr crtiil = AosIILStr::getSubiilByIndex(iilidx, rdata);
	AosIILStrPtr nextiil = AosIILStr::getSubiilByIndex(iilidx + 1, rdata);
	aos_assert_r(crtiil, false);
	aos_assert_r(nextiil, false);
		
	bool rslt = false;
	char ** valuePtr = 0;
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
		updateIndexData(iilidx, true, true);
	}
	return true;	
}


bool 		
AosIILStr::mergeSubiilBackwardPriv(
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
	aos_assert_r(mNumSubiils > 0, false);
	aos_assert_r(iilidx <= mNumSubiils -1,false);
	aos_assert_r(iilidx > 0,false);
	
	// 2. move entries from a to b
	AosIILStrPtr previil = AosIILStr::getSubiilByIndex(iilidx - 1, rdata);
	AosIILStrPtr crtiil = AosIILStr::getSubiilByIndex(iilidx, rdata);
	aos_assert_r(previil, false);
	aos_assert_r(crtiil, false);
		
	bool rslt = false;
	char ** valuePtr = 0;
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
AosIILStr::removeSubiil(
		const i64 &iilidx,
		const AosRundataPtr &rdata)
{
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
	OmnMemMgrSelf->release(mMinVals[iilidx], __FILE__, __LINE__);
	OmnMemMgrSelf->release(mMaxVals[iilidx], __FILE__, __LINE__);
	if (iilidx < mNumSubiils -1)
	{
		memmove(&(mMinVals[iilidx]), &(mMinVals[iilidx+1]), (sizeof(char *)) * (mNumSubiils - iilidx - 1));
		memmove(&(mMaxVals[iilidx]), &(mMaxVals[iilidx+1]), (sizeof(char *)) * (mNumSubiils - iilidx - 1));
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
	for(i64 i = iilidx ;i < mNumSubiils - 1;i++)
	{
		mSubiils[i] = mSubiils[i+1];// no matter it is null or not
		
		if (mSubiils[i].notNull())
		{
			mSubiils[i]->setIILIdx(i);
		}
	}
	mSubiils[mNumSubiils - 1] = 0;
	mNumSubiils --;

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
			OmnMemMgrSelf->release(mMinVals[0], __FILE__, __LINE__);
			OmnDelete [] mMinVals;
			mMinVals = 0;
		}
			
		if (mMaxVals)
		{
			OmnMemMgrSelf->release(mMaxVals[0], __FILE__, __LINE__);
			OmnDelete [] mMaxVals;
			mMaxVals = 0;
		}
	}
	
	//6. set dirty flag
	mIsDirty = true;
	return true;	
}


bool		
AosIILStr::getValueDocidPtr(
		char ** &valuePtr,
		u64*    &docidPtr,
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
AosIILStr::addDocSeqPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		i64	&iilidx,
		i64 &idx,
		const AosRundataPtr &rdata)
{
	aos_assert_r(value != "", false);
	bool rslt = false;
	bool keepSearch = false;
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	if (isParentIIL())
	{
		// It is segmented and the iil is the root iil. 
		AosIILStrPtr subiil;

		// Chen Ding, Bug1217
		iilidx = getSubiilIndexSeq(value, iilidx, rdata);
		if (iilidx == -5) iilidx = 0;
		aos_assert_r(mNumSubiils > 0,false);
		while (iilidx <= mNumSubiils -1)
		{
			subiil = getSubiilByIndex(iilidx, rdata);
			aos_assert_r(subiil, false);
			keepSearch  = false;
			
			if (subiil.getPtr() == this)
			{
				rslt = addDocSeqDirectPriv(
					value, docid, value_unique, docid_unique, idx, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->addDocSeqDirectSafe(
					value, docid, value_unique, docid_unique, idx, keepSearch, rdata);
			}
			
			if (rslt)
			{
				// Ketty 2013/01/15
				// comp not support yet.
				//AosIILCompStrPtr compiil = retrieveCompIIL(rdata);
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
		//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->addDocSafe(value,docid,false,false,rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	return rslt;	
}


bool 		
AosIILStr::addDocSeqDirectPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		i64		   &curidx,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	keepSearch = false;
	aos_assert_r(value != "", false);
//	AosIILStrPtr r1 = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
//	if (r1)r1->splitSanityCheck();
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// In this case, the doc may be inserted in the next subiil.
	
	// Check whether we need to split

	int rsltInt = 0;
	int rsltInt2 = 0;
	bool rslt = false;
	i64 numBefore = mNumDocs;

	// OmnScreen << "IILStr add: " << this << endl;
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
	if (AosIILUtil::valueMatch(mValues[left], eAosOpr_ge, value.data(), "", mIsNumAlpha))
	{
		right = left;
	}	
	
	if (mNumDocs == 0)
	{
		// Change mValues
		if (mNumDocs >= mMemCap) aos_assert_r(expandMemoryPriv(), false);
		mDocids[0] = docid;
		aos_assert_r(setValue(0, value.data(), value.length()), false);
		mNumDocs = 1;
		idx = 0;
		goto finished;
	}

	aos_assert_r(mNumDocs <= mMemCap, false);
	while (left <= right)
	{
		idx = left + ((right - left) >> 1);
		aos_assert_r(mValues[idx], false);
		rsltInt = AosIILUtil::valueMatch(mValues[idx], value, mIsNumAlpha);
		if (rsltInt == 0)
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
		if (rsltInt < 0) 
		{
			if (idx == mNumDocs-1)
			{
				// Append the docid
				if (mNumDocs >= mMemCap)
					aos_assert_r(expandMemoryPriv(), false);
				mDocids[mNumDocs] = docid;
				// Change mValues
				aos_assert_r(setValue(mNumDocs, value.data(), value.length()), false);
				incNumDocsNotSafe();
				idx = mNumDocs-1;
				goto finished;
			}

			aos_assert_r(mValues[idx+1], false);
			// Chen Ding, 08/11/2010
			// Check whether mValues[idx+1] >= value
			// if (strcmp(value.data(), mValues[idx+1]) < 0)
			rsltInt2 = AosIILUtil::valueMatch(mValues[idx+1], value, mIsNumAlpha);
			if (rsltInt2 > 0)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (rsltInt2 == 0)
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
		else
		{
			// value < mValues[idx]
			if (idx == 0)
			{
				// Insert into the front
				insertBefore(idx, docid, value);
				goto finished;
			}

			aos_assert_r(mValues[idx-1], false);
			// Chen Ding, 08/11/2010
			// if (strcmp(mValues[idx-1], value.data()) < 0)
			rsltInt2 = AosIILUtil::valueMatch(mValues[idx-1], value, mIsNumAlpha);
			if (rsltInt2 < 0)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				idx--;
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if (rsltInt2 == 0)
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
	
	// Ketty temp 2012/11/28
	//aos_assert_r(numBefore + 1 == mNumDocs, false);
	if (!isSingleIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = (idx >= mNumDocs-1);

		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
		AosIILStr *rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);
	}

	if (mNumDocs >= mMaxNormIILSize)
	{
		AosIILObjPtr subiil;
		rslt = splitListPriv(subiil, rdata);

	}
	sanityTestForSubiils();
//	AosIILStrPtr r = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
//	if (r)r->splitSanityCheck();
	return true;
}


bool		
AosIILStr::removeDocSeqPriv(
		const OmnString &value, 
		const u64 &docid,
		i64	&iilidx,
		i64	&idx,
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
		AosIILStrPtr subiil;
		subiil = getSubiilSeq(value, iilidx, rdata);
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
				//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
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
		//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		//aos_assert_r(compiil, false);				
		//bool rslt = compiil->removeDocSafe(value,docid,rdata);
		//returnCompIIL(compiil,rdata);
		//aos_assert_r(rslt, false);
	}
	return rslt;
}
		

bool		
AosIILStr::removeDocSeqDirectPriv(
		const OmnString &value, 
		const u64 &docid,
		i64	&curiilidx,
		i64 &curidx,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	if (mNumDocs <= 0) return false;

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
	
	if (AosIILUtil::valueMatch(mValues[startidx], eAosOpr_eq, value.data(), "", mIsNumAlpha))
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
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);
	if (idx < mNumDocs-1)
	{
		i64 ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx+1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx+1], ss);

		ss = sizeof(char *) * (mNumDocs - idx - 1);
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
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
		AosIILStr *rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
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
AosIILStr::saveIILsToFileSafe(const OmnFilePtr &file, i64 &crt_pos, const AosRundataPtr &rdata)
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

// Chen Ding, 06/26/2011
bool
AosIILStr::setValueDocUniqueSafe(
		const OmnString &key, 
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
	OmnString vv = key;
	bool rslt = nextDocidPriv(idx, iilidx, false, eAosOpr_eq, vv, did, isunique, rdata);
	if (!rslt || idx < 0)
	{
		// Did not find it. Create it.
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

	// The docid is different. Need to modify it.
	if (isParentIIL())
	{
		AosIILStrPtr subiil = getSubiilByIndex(iilidx, rdata);
		if (!subiil)
		{
			rdata->setError() << "Failed retrieving the subiil: " << iilidx;	
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSUNLOCK(mLock);
			return false;
		}

		if (!(subiil->mValues && idx < subiil->mNumDocs &&
			  subiil->mDocids && AosIILUtil::valueMatch(subiil->mValues[idx], vv, mIsNumAlpha) == 0))
		{
			AOSUNLOCK(mLock);
			rdata->setError() << "Internal error: " << idx << ":"
				<< subiil->mNumDocs;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		subiil->mDocids[idx] = docid;
		subiil->setDirty(true);
	}
	else
	{
		// single iil
		if (!(mValues && idx < mNumDocs &&
			  mDocids && AosIILUtil::valueMatch(mValues[idx], vv, mIsNumAlpha) == 0))
		{
			AOSUNLOCK(mLock);
			rdata->setError() << "Internal error: " << idx << ":"
				<< mNumDocs;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		mDocids[idx] = docid;
		mIsDirty = true;
	}

	// modify in comp iil
	// Ketty 2013/01/15
	// comp not support yet.
	//AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
	//aos_assert_r(compiil, false);
	//rslt = compiil->removeDocSafe(key, did, rdata);
	//if (!rslt)
	//{
	//	returnCompIIL(compiil, rdata);
	//	aos_assert_r(rslt, false);
	//}
	//rslt = compiil->addDocSafe(key, docid, false, false, false, rdata);
	//returnCompIIL(compiil, rdata);
	//aos_assert_r(rslt, false);
	
	AOSUNLOCK(mLock);
	return true; 
}


void    
AosIILStr::exportIILPriv(
		vector<OmnString> 	&values,
		vector<u64>			&docids,
		const AosRundataPtr &rdata)
{
	values.clear();
	docids.clear();
	if (isParentIIL())
	{
		AosIILStrPtr subiil;
		exportIILSinglePriv(values,docids,rdata);
		for(i64 i = 1;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, rdata);
			subiil->exportIILSingleSafe(values,docids,rdata);
			return;
		}
	}
	return exportIILSinglePriv(values,docids,rdata);
}


void    
AosIILStr::exportIILSinglePriv(
		vector<OmnString> 	&values,
	   	vector<u64>			&docids,
		const AosRundataPtr &rdata)
{
	for(i64 i=0; i<mNumDocs; i++)
	{
		values.push_back(mValues[i]);
		docids.push_back(mDocids[i]);
	}
}


bool
AosIILStr::copyDocidsPriv(
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
				query_rslt->appendStrValue(mValues[i]);
			}
		}
	}

	return rslt;
}


// Ketty 2013/01/15
//AosIILObjPtr	
//AosIILStr::createCompIIL(const AosRundataPtr &rdata)
//{
//	return AosIILMgr::getSelf()->createIILPublic1(mIILID, mSiteid, eAosIILType_CompStr, false, false, rdata);
//}


// Ketty 2013/01/15
// comp not support yet.
/*
AosIILCompStrPtr 	
AosIILStr::retrieveCompIIL(
		const bool iilmgr_locked,
		const AosRundataPtr &rdata) 
{
	if (isLeafIIL())
	{
		aos_assert_r(mRootIIL,0);
		AosIILStrPtr rootiil = dynamic_cast<AosIILStr *>(mRootIIL.getPtr());
		return rootiil->retrieveCompIIL(iilmgr_locked, rdata);
	}
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILObjPtr compiil1 = AosIILMgr::getSelf()->loadIILByIDPublic(
								mIILID, 
								mSiteid,
								iiltype, 
								true, 
								iilmgr_locked, 
								rdata);
	aos_assert_r(compiil1, 0);
	AosIILCompStrPtr compiil = (AosIILCompStr*)compiil1.getPtr();
	return compiil;
}


bool	
AosIILStr::returnCompIIL(const AosIILObjPtr &iil,const AosRundataPtr &rdata)
{
	return AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
}	
*/

bool
AosIILStr::removeFirstValueDocSafe(
		const OmnString &value,
		u64 &docid,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	
	i64 idx = -10;
	i64 iilidx = -10;
	bool isunique = true;
	bool rslt = nextDocidPriv(idx, iilidx, reverse, eAosOpr_eq, value, docid, isunique, rdata);
	if (idx < 0 || iilidx < 0 || !rslt)
	{
		AOSUNLOCK(mLock);
		return false;
	}
	rslt = removeDocPriv(value, docid, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed to remove: " << docid << ":" << value << enderr;
	}
	
	AOSUNLOCK(mLock);
	return rslt;
}

bool
AosIILStr::queryNewLikePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	
	i64 cur_idx = query_context->getIndex();
	bool reverse = query_context->isReverse();
	i64 cur_iilidx = query_context->getIILIndex();

	if (cur_idx >= 0)
	{
		bool has_data = true;
		OmnString cur_value = query_context->getCrtValue();
		u64 cur_docid = query_context->getCrtDocid();
		aos_assert_r(cur_iilidx >= 0,false);
		nextIndexFixErrorIdx(cur_idx,cur_iilidx,cur_value,cur_docid,reverse,has_data,rdata);
	}else
	{
		// cur_idx < 0
		if (reverse)
		{
			if (isSingleIIL())
			{
				cur_idx = mNumDocs -1;
				cur_iilidx = 0;
			}
			else
			{
				cur_idx = mNumDocs -1;
				cur_iilidx = mNumSubiils-1;
			}
		}
		else
		{
			cur_idx = 0;
			cur_iilidx = 0;
		}
	}
	if (isSingleIIL())
	{
		queryNewLikeSinglePriv(query_rslt,query_bitmap,query_context,cur_idx,reverse,rdata);
		if (!query_context->finished())
		{
			aos_assert_r(cur_idx >= 0 && cur_idx <= mNumDocs-1, false);
			query_context->setIndex(cur_idx);
			query_context->setIILIndex(0);
			query_context->setCrtDocid(mDocids[cur_idx]);
			query_context->setCrtValue(mValues[cur_idx]);
		}
		return true;
	}
	
	if (reverse)
	{
		
		while(cur_iilidx >= 0)
		{
			AosIILStrPtr iil = getIILByIndex(cur_iilidx, rdata);
			aos_assert_r(iil, false);
			iil->queryNewLikeSinglePriv(query_rslt,query_bitmap,query_context,cur_idx,reverse,rdata);
			if (cur_iilidx != 0)
			{
				query_context->setFinished(false);
			}
			if (query_rslt->getNumDocs() >= query_context->getBlockSize())
			{
				break;
			}
			cur_iilidx --;
			if (cur_iilidx >= 0)cur_idx = mNumEntries[cur_iilidx]-1;
		}
		
		
	}else
	{
		while(cur_iilidx < mNumSubiils)
		{
			AosIILStrPtr iil = getIILByIndex(cur_iilidx, rdata);
			aos_assert_r(iil, false);
			iil->queryNewLikeSinglePriv(query_rslt,query_bitmap,query_context,cur_idx,reverse,rdata);
			if (cur_iilidx != mNumSubiils -1)
			{
				query_context->setFinished(false);
			}
			if (query_rslt->getNumDocs() >= query_context->getBlockSize())
			{
				break;
			}
			cur_iilidx ++;
			cur_idx = 0;
		}
	}

	if (!query_context->finished())
	{
		aos_assert_r(cur_iilidx >= 0 && cur_iilidx <= mNumSubiils-1, false);
		aos_assert_r(cur_idx >= 0 && cur_idx <= mNumEntries[cur_iilidx]-1, false);
		query_context->setIndex(cur_idx);
		query_context->setIILIndex(cur_iilidx);
		AosIILStrPtr iil = getIILByIndex(cur_iilidx, rdata);
		aos_assert_r(iil, false);
		query_context->setCrtDocid(iil->getDocid(cur_idx));
		query_context->setCrtValue(iil->getValue(cur_idx));
	}
	return true;
}


bool
AosIILStr::queryNewLikeSinglePriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const i64 &start_idx,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	i64 cur_idx = start_idx;
	bool search_over = false;
	OmnString value = query_context->getStrValue();
	while(!search_over)
	{
		if (valueLikeMatch(mValues[cur_idx],value))
		{
			// append docid
			query_rslt->appendDocid(mDocids[cur_idx]);
			if (query_rslt->getNumDocs() >= query_context->getBlockSize())
			{
				search_over = true;
				break;
			}
		}
		//check if it is the last one
		if (reverse)
		{
			if (cur_idx == 0)
			{
				query_context->setFinished(true);
				search_over = true;
			}
			else
			{
				cur_idx --;
			}
		}
		else// normal order
		{
			if (cur_idx == mNumDocs-1)
			{
				query_context->setFinished(true);
				search_over = true;
			}
			else
			{
				cur_idx ++;
			}
		}
		if (query_rslt->getNumDocs() >= query_context->getBlockSize())
		{
			search_over = true;
		}
	}
	return false;
}


bool
AosIILStr::queryNewNEPriv(
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
	
	OmnString value = query_context->getStrValue();

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
	OmnString cur_value = query_context->getCrtValue();
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

						AosIILStrPtr iil = getIILByIndex(block_start_iilidx_2, rdata);
						aos_assert_r(iil, false);
						
						query_context->setCrtDocid(iil->getDocid(block_start_idx_2));
						query_context->setCrtValue(iil->getValue(block_start_idx_2));
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
						AosIILStrPtr iil = getIILByIndex(block_start_iilidx_1, rdata);
						aos_assert_r(iil, false);
						
						query_context->setCrtDocid(iil->getDocid(block_start_idx_1));
						query_context->setCrtValue(iil->getValue(block_start_idx_1));
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
						AosIILStrPtr iil = getIILByIndex(block_end_iilidx_1, rdata);
						aos_assert_r(iil, false);

						query_context->setCrtDocid(iil->getDocid(block_end_idx_1));
						query_context->setCrtValue(iil->getValue(block_end_idx_1));
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
						AosIILStrPtr iil = getIILByIndex(block_end_iilidx_2, rdata);
						aos_assert_r(iil, false);
						
						query_context->setCrtDocid(iil->getDocid(block_end_idx_2));
						query_context->setCrtValue(iil->getValue(block_end_idx_2));
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


i64
AosIILStr::compPos(
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
	
/*bool
AosIILStr::copyDataBlockForNE(
		const i64 &start_idx,
		const i64 &start_iilidx,
		const i64 &end_idx,
		const i64 &end_iilidx,
		const i64 &cur_idx,
		const i64 &cur_iilidx,
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	bool reverse = query_rslt->isReverse();
}
*/

bool
AosIILStr::queryNewPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	query_context->setAlphaNum(mIsNumAlpha);
	
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
	else if (opr == eAosOpr_like)
	{
		return queryNewLikePriv(query_rslt, query_bitmap, query_context, rdata);
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
AosIILStr::queryPagingProc(
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
	OmnString str_value = query_context->getStrValue();
	OmnString str_value2 = str_value; 

	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt <= 0, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

	//1. get the start idx/iilidx
	start_idx = -10;
	start_iilidx = -10;
	u64 cur_docid_0 = 0;
	bool isunique = true;
	bool rslt = nextDocidPriv(
		start_idx, start_iilidx, false, // normal order 
		opr1, str_value, cur_docid_0, isunique, rdata);
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
		opr2, str_value2, cur_docid_0, isunique, rdata);
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
		OmnString cur_value = query_context->getCrtValue();
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
			query_context->setCrtValue(mValues[new_cur_idx]);
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

			AosIILStrPtr iil = getIILByIndex(new_cur_iilidx, rdata);
			aos_assert_r(iil, false);

			query_context->setCrtDocid(iil->getDocid(new_cur_idx));
			query_context->setCrtValue(iil->getValue(new_cur_idx));
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
			query_context->setCrtValue(mValues[new_cur_idx]);
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
			AosIILStrPtr iil = getIILByIndex(new_cur_iilidx, rdata);
			aos_assert_r(iil, false);

			query_context->setCrtDocid(iil->getDocid(new_cur_idx));
			query_context->setCrtValue(iil->getValue(new_cur_idx));
		}
		
		end_idx = new_cur_idx;
		end_iilidx = new_cur_iilidx;
	}

	return true;
}


void
AosIILStr::nextIndexFixErrorIdx(
		i64 &cur_idx,
		i64 &cur_iilidx,
		const OmnString &cur_value,
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
		// SHAWN_WARN
		// if (mValues[cur_idx] != cur_value && mDocids[cur_idx] != cur_docid)
		if (cur_value != mValues[cur_idx] && mDocids[cur_idx] != cur_docid)
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
	
	AosIILStrPtr subiil = getIILByIndex(cur_iilidx, rdata);
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
AosIILStr::verifyIdx(
		const i64 &cur_idx,
		const OmnString &cur_value,
		const u64 &cur_docid)
{
	// SHAWN_WARN
	// return (cur_idx < mNumDocs && 
	// 		mDocids[cur_idx] == cur_docid && 
	// 		mValues[cur_idx] == cur_value);
	return (cur_idx < mNumDocs && 
			mDocids[cur_idx] == cur_docid && 
			cur_value == mValues[cur_idx]);
}


bool	
AosIILStr::copyData(
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
	AosIILStrPtr subiil;

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
AosIILStr::copyDataWithCheck(
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
	AosIILStrPtr subiil;
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
AosIILStr::copyDataSingleWithCheck(
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

	if (query_context->isFieldFilterValid())
	{
		aos_assert_r(query_rslt, false);

		bool addValue = query_rslt->isWithValues();

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

				if (!query_context->filterFieldCond(mValues[i]))
				{
					continue;
				}
					
				query_rslt->appendDocid(mDocids[i]);
				if (addValue)
				{
					query_rslt->appendStrValue(mValues[i]);
				}

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);

				if (query_context->isFull(num_docs))
				{
					query_context->setIndex(i);
					query_context->setCrtDocid(mDocids[i]);
					query_context->setCrtValue(mValues[i]);
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

				if (!query_context->filterFieldCond(mValues[i]))
				{
					continue;
				}
					
				query_rslt->appendDocid(mDocids[i]);
				if (addValue)
				{
					query_rslt->appendStrValue(mValues[i]);
				}

				num_docs = getRsltNumDocs(query_rslt, query_bitmap);
				aos_assert_r(num_docs >= 0, false);
					
				if (query_context->isFull(num_docs))
				{
					query_context->setIndex(i);
					query_context->setCrtDocid(mDocids[i]);
					query_context->setCrtValue(mValues[i]);
					return true;
				}
			}
		}
		return true;
	}

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
					query_rslt->appendStrValue(mValues[i]);
				}
			}

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				query_context->setIndex(i);
				query_context->setCrtDocid(mDocids[i]);
				query_context->setCrtValue(mValues[i]);
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
					query_rslt->appendStrValue(mValues[i]);
				}
			}

			num_docs = getRsltNumDocs(query_rslt, query_bitmap);
			aos_assert_r(num_docs >= 0, false);

			if (query_context->isFull(num_docs))
			{
				query_context->setIndex(i);
				query_context->setCrtDocid(mDocids[i]);
				query_context->setCrtValue(mValues[i]);
				return true;
			}
		}
	}	

	return true;
}			


bool	
AosIILStr::copyDataSingle(
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

	//Linda, 2013/02/22
	if (query_context->isFieldFilterValid())
	{
		aos_assert_r(query_rslt, false);

		bool addValue = query_rslt->isWithValues();

		if (reverse)
		{
			for (i64 i=end_idx; i>=start_idx; i--)
			{
				if (!query_context->filterFieldCond(mValues[i]))
				{
					continue;
				}
				
				query_rslt->appendDocid(mDocids[i]);
				if (addValue)
				{
					query_rslt->appendStrValue(mValues[i]);
				}
			}
		}
		else
		{
			for (i64 i=start_idx; i<=end_idx; i++)
			{
				if (!query_context->filterFieldCond(mValues[i]))
				{
					continue;
				}
				
				query_rslt->appendDocid(mDocids[i]);
				if (addValue)
				{
					query_rslt->appendStrValue(mValues[i]);
				}
			}
		}
		return true;
	}
	
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
					query_rslt->appendStrValue(mValues[i]);
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
					query_rslt->appendStrValue(mValues[i]);
				}
			}
		}
	}
	
	return true;
}


bool	
AosIILStr::nextQueryPos(i64 &idx,i64 &iilidx)
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
AosIILStr::prevQueryPos(i64 &idx,i64 &iilidx)
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
AosIILStr::locateIdx(
		i64 &cur_idx,
		i64 &cur_iilidx,
		const OmnString &cur_value,
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


// Chen Ding, 05/05/2012
bool 
AosIILStr::queryColumn(
		const OmnString &value1, 
		const AosOpr opr1, 
		const OmnString &value2, 
		const AosOpr opr2, 
		const AosStrStrArrayPtr data,
		const AosRundataPtr &rdata)
{
	// This function should be moved to AosIILStrStr but we do not have that
	// type of IIL yet, we use AosIILStr to simulate AosIILStrStr. 
	// AosIILStrStr is also called "Column", or "Name-Value" pairs. 
	// It is a two-column table.
	// To use AosIILStr to simulate AosIILStrStr, the 'value' is constructed
	// as:
	// 		'value1|$$|valuw2'
	// and the docid is not used for now.
	OmnNotImplementedYet;
	return false;
}

bool
AosIILStr::deleteIILPriv(
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	if (isSingleIIL())
	{
		return deleteIILSinglePriv(rdata);
	}
	
	// remove from IILMgr IDHash
	AosIILMgr::getSelf()->removeIILFromHashPublic(mIILID, mSiteid, rdata);

	AosIILStrPtr subiil;
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
AosIILStr::deleteIILSinglePriv(const AosRundataPtr &rdata)
{
	//1. remove from IILMgr IDHash
	AosIILMgr::getSelf()->removeIILFromHashPublic(mIILID, mSiteid, rdata);
	//2. remove from disk
	deleteFromLocalFilePriv(rdata);
	
	// Ketty 2013/01/15
	//3. remove trans
	//finishTransSafe(rdata);	
	return true;
}


bool
AosIILStr::getSplitValueSafe(
		const AosQueryContextObjPtr &context,
		const i64 &size,
		vector<AosQueryContextObjPtr> &contexts,
		const AosRundataPtr &rdata)
{
	context->setIILType(getIILType());
	OmnNotImplementedYet;
	return false;
}

bool	
AosIILStr::preQueryNEPriv(
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}

bool	
AosIILStr::preQueryPriv(
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
	OmnString str_value = query_context->getStrValue();
	OmnString str_value2 = str_value; 
	if (opr1 == eAosOpr_range_ge_le)
	{
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt <= 0, false);
	}

    if (opr1 == eAosOpr_range_ge_lt)
    {
		opr1 = eAosOpr_ge;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_le)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_le;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
    }

    if (opr1 == eAosOpr_range_gt_lt)
    {
		opr1 = eAosOpr_gt;
		opr2 = eAosOpr_lt;
		str_value2 = query_context->getStrValue2();
		int rsltInt = AosIILUtil::valueMatch(str_value, str_value2, mIsNumAlpha);
		aos_assert_r(rsltInt < 0, false);
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
						 str_value,
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
						 str_value2,
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
AosIILStr::batchAddSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = false;
	u64 value = 0;
	for(int i=0; i<num; i++)
	{
		OmnString key(entries, strlen(entries));
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


bool
AosIILStr::batchDelSafe(
		char * &entries,
		const i64 &size,
		const i64 &num,
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	i64 start_total = mNumDocs;
	u64 start_time = OmnGetTimestamp();
	bool rslt = false;
	u64 value = 0;
	for(int i=0; i<num; i++)
	{
		OmnString key(entries, strlen(entries));
		value = *(u64 *)&entries[size - sizeof(u64)];
		rslt = removeDocSafe(key, value, rdata);
	
		// Ken Lee, 2015/06/25
		// May be the removed data is not exist, it wll return false.
		// aos_assert_r(rslt, false);
		if (!rslt) {
		}
		
		entries += size;
	}
	
	rslt = saveToFileSafe(rdata);
	aos_assert_r(rslt, false);

	u64 cost = OmnGetTimestamp() - start_time;
	i64 end_total = mNumDocs;

	OmnScreen << "batch delete finish, IILID:" << mIILID
		<< ", start_total:" << start_total
		<< ", end_total:" << end_total
		<< ", time_cost:" << AosTimestampToHumanRead(cost) 
		<< endl;

	return true;

}




void
AosIILStr::setSnapShotId(const u64 &snap_id)
{
	AOSLOCK(mLock);
	setSnapShotIdPriv(snap_id);
	AOSUNLOCK(mLock);
}


void
AosIILStr::setSnapShotIdPriv(const u64 &snap_id)
{
	aos_assert(mSnapShotId == 0 || snap_id == mSnapShotId);
	mSnapShotId = snap_id;

	if (isParentIIL())
	{
		AosIILStrPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->setSnapShotIdPriv(snap_id);
		}
	}
}


void
AosIILStr::resetSnapShotId()
{
	AOSLOCK(mLock);
	resetSnapShotIdPriv();
	AOSUNLOCK(mLock);
}


void
AosIILStr::resetSnapShotIdPriv()
{
	mSnapShotId = 0;	
	if (isParentIIL())
	{
		AosIILStrPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->resetSnapShotIdPriv();
		}
	}
}


bool
AosIILStr::resetIIL(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	if (isParentIIL())
	{
		AosIILStrPtr subiil;
		AosIILType type= eAosIILType_Str;
		for (i64 i = 1;i < mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			aos_assert_rl(subiil != this, mLock, false);
			if (!subiil)
			{
				AosIILObjPtr iil = AosIILMgr::getSelf()->getIILFromHashPublic(
					mIILIds[i], mSiteid, type, rdata);
				subiil = dynamic_cast<AosIILStr *>(iil.getPtr());
			}
			if (!subiil)
			{
				continue;
			}
			aos_assert_rl(subiil->getRefCountByIILMgr() == 1, mLock, false);

			AosIILMgr::getSelf()->removeIILFromHashPublic(
				mIILIds[i], mSiteid, rdata);

			mSubiils[i] = 0;
			subiil = 0;
		}
	}
	
	//mSnapShotId = AosIILSave::getSelf()->getSnapIdByIILID(mIILID);
	resetSelf(rdata);
	bool rslt = loadFromFilePriv(mIILID, rdata);
	aos_assert_rl(rslt, mLock, false);
	
	AOSUNLOCK(mLock);
	return true;
}

