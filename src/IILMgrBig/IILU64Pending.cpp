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
#include "IILMgrBig/IILU64.h"


/*
bool		
AosIILU64::nextDocidPrefixPriv(
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const u64 &prefix,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	// Check whether it is a root iil and it is segmented
	bool rslt = false;
	docid = AOS_INVDID;
	if (isRootIIL())
	{
		// It is segmented and the iil is the root iil.
		// If 'iilidx' is within [0, mNumSubiils-1],
		// use it.
		AosIILU64Ptr subiil;
		if(idx == -10)
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
			if (idx < 0 || iilidx < 0 || (u32)iilidx >= mNumSubiils)
			{
				OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				return false;
			}
		}

		subiil = getSubiilByIndex(iilidx, false, rdata);
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
			if(subiil == this)
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
			if(!keepSearch)
			{
				break;
			}
			// Chen Ding, 12/11/2010
			// !!!!!!!!!!!!!!!! It should use the condition to retrieve the next IIL.
			if(reverse)
			{
				iilidx --;
			}
			else
			{
				iilidx ++;
			}
			if(iilidx < 0 || (u32)iilidx >= mNumSubiils)
			{
				// No more to search
				iilidx = -5;
				idx = -5;
				docid = AOS_INVDID;
				return true;
			}

			subiil = getSubiilByIndex(iilidx, false, rdata);

			// if we found the next subiil, we need to reset 
			// idx to -10 to search from the beginning
			if(subiil)
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
*/


/*
u64     
AosIILU64::nextDocIdSafe2(
		int &idx,
		int &iilidx,
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
	if(!isRootIIL())
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

	if((int)iilidx > (int)mNumSubiils -1)
	{
		idx = -5;
		docid = AOS_INVDID;
		AOSUNLOCK(mLock);
		return true;
	}

	if(isRootIIL())
	{
		AosIILU64Ptr subiil;
		//1. get the subiil
		if(iilidx == -10)
		{
			if(reverse)
			{
				iilidx = mNumSubiils -1;
			}
			else
			{
				iilidx = 0;
			}
		}
		//2. get the docid
		aos_assert_rb(mNumSubiils > 0, mLock, true);
		while(iilidx >= 0 && iilidx <= (int)mNumSubiils - 1)
		{
			subiil = getSubiilByIndex(iilidx, false, rdata);
			aos_assert_rb(subiil, mLock, true);
			if(subiil == this)
			{
				docid = subiil->nextDocIdPriv2(idx,iilidx,reverse);
			}
			else
			{
				docid = subiil->nextDocIdSafe2(idx,iilidx,reverse, rdata);
			}
				
			if(docid != AOS_INVDID)break;
			// not found this time
			if(reverse)
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
		if(docid == AOS_INVDID)
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
*/


/*
bool		
AosIILU64::nextDocidSafe(
		int &idx, 
		int &iilidx,
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
		isunique = false;
		return false;
	}

	// Check whether it is a root iil and it is segmented
	if (isRootIIL())
	{
		// It is segmented and the iil is the root iil. 
		// If 'iilidx' is within [0, mNumSubiils-1], 
		// use it.
		AosIILU64Ptr subiil;
		if(idx == -10)
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
			subiil = getSubiilByIndex(iilidx, false, rdata);
		}

		if (!subiil)
		{
			// This means there are no entries for the query
			AOSUNLOCK(mLock);
			docid = AOS_INVDID;
			isunique = false;
			idx = -5;
			iilidx = -5;
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
			rslt = subiil->nextDocidSubSafe(idx, reverse, opr1, value1, 
					opr2, value2, docid, isunique, rdata);
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
*/


/* Should remove
bool
AosIILU64::modifyDocSafe(
		const u64 &oldvalue, 
		const u64 &newvalue, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const bool iilmgrLocked,
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
	bool rslt1 = removeDocPriv(oldvalue, docid, rdata);
	if (!rslt1)
	{
		OmnAlarm << "Failed to remove: " << docid << ":" << oldvalue << ":"
			<< newvalue << enderr;
	}
	bool rslt2 = addDocPriv(newvalue, docid, value_unique, docid_unique, iilmgrLocked, rdata);
	aos_assert_rb(AosIILU64SanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);

	if (!rslt2)
	{
		OmnAlarm << "Failed to add: " << docid << ":" << oldvalue 
			<< ":" << newvalue << enderr;
	}
	return rslt1 && rslt2;
}
*/


/*
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
	if(!isRootIIL())
	{
		rslt = checkDocPriv(opr,value,docid,keepSearch ,rdata);
		AOSUNLOCK(mLock);
		return rslt;
	}

	// Chen Ding, Bug1217
	// AosIILU64Ptr subiil = getSubiilByIndex(false, opr, value);	
	AosIILU64Ptr subiil = getSubIIL(-10, value, opr, true, rdata);
	if (!subiil)
	{
		AOSUNLOCK(mLock);
		return false;
	}

	keepSearch = true;
	int subIILIdx = subiil->getIILIdx();
	aos_assert_r(subIILIdx >= 0, false);
	while(keepSearch)
	{
		if(subiil == this)
		{
			rslt = subiil->checkDocPriv(opr, value, docid, keepSearch, rdata);
		}
		else
		{
			rslt = subiil->checkDocSafe(opr, value, docid, keepSearch, rdata);
		}
		// already found
		if(rslt)
		{
			AOSUNLOCK(mLock);
			return true;
		}
		// do not need to keep searching
		if(!keepSearch)
		{
			AOSUNLOCK(mLock);
			return false;
		}
		// find to the last subiil
		aos_assert_r(mNumSubiils > 0, false);
		if(subIILIdx >= (int)mNumSubiils-1)
		{
			AOSUNLOCK(mLock);
			return false;
		}
		subIILIdx ++;
		subiil = getSubiilByIndex(subIILIdx, false, rdata);
	}
	AOSUNLOCK(mLock);
	return	false; 
}
*/


/*
bool		
AosIILU64::checkDocPriv(
		const AosOpr opr,
		const u64 &value,
		const u64 &docid,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
//?????????shawn
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


	int startidx = 0; 
	int endidx = mNumDocs-1; 
	u64 did;
	aos_assert_r(firstDoc1(startidx, false, opr, value, did), false);
	aos_assert_r(firstDoc1(endidx, true, opr, value, did), false);

	if (startidx < 0) 
	{
		keepSearch = false;
		return false;
	}

	aos_assert_r(startidx >= 0 && endidx >= startidx, false);

	for (int i=startidx; i<=endidx; i++)
	{
		if (mDocids[i] == docid)
		{
			return true;
		}
	}
	keepSearch = (endidx == (int)(mNumDocs -1));
	return false;
}
*/


/* Should remove
bool
AosIILU64::removeDocPriv(
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata) 
{
	bool keepSearch = true;
	bool rslt = false;
	// Check whether it is a root iil and it is segmented
	if (isRootIIL())
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
			if(rslt) 
			{
				// remove from compiil
				AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
				aos_assert_r(compiil, false);				
				bool rslt = compiil->removeDocSafe(value,docid,rdata);
				returnCompIIL(compiil,rdata);
				aos_assert_r(rslt, false);
				
				return true;
			}
			if(!keepSearch) break;
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
	rslt = removeDocDirectPriv(value, docid,keepSearch, rdata);
	if(rslt)
	{
		// remove from compiil
		AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		aos_assert_r(compiil, false);				
		bool rslt = compiil->removeDocSafe(value,docid,rdata);
		returnCompIIL(compiil,rdata);
		aos_assert_r(rslt, false);
	}
	// Chen Ding, 12/07/2010
	// No alarms will be raised. The caller is responsible for raising alarms
	// if(!rslt) OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
	return rslt;
}
*/


/*
int
AosIILU64::getDocidsSafe(
		const AosOpr opr,
		const u64 &value,
		u64 *docids, 
		const int arraysize,
		const AosRundataPtr &rdata)
{
// shawn if it is useful, rewrite it, otherwise comment it out temperarily
	// This function retrieves all the docs that meet the condition.
	int numdocfound = 0;
	int curPos = 0;
	AOSLOCK(mLock);
	if(!isRootIIL())
	{
		numdocfound = getDocidsPriv(
				opr, value, docids, arraysize, curPos, rdata);
		AOSUNLOCK(mLock);
		return numdocfound;
	}

	// it is root iil
	// 1. get the first sub iil
	// Chen Ding, 12/12/2010
	int firstSubiilIdx = getSubIILIndex(-10, value,opr,true);
	// 2. get the last sub iil
	// Chen Ding, 12/12/2010
	int lastSubiilIdx = getSubIILIndex(-10, value,opr,false);
	// 3. get the docids from the first to the last
	int counter = 0;
	AosIILU64Ptr curSubiil = 0;
	for(int i = firstSubiilIdx;i <= lastSubiilIdx;i++)
	{
		//1. get the subiil
		curSubiil = getSubiilByIndex(i, false, rdata);
		aos_assert_rb(curSubiil,mLock,0);
// do not call the single function, 
		counter = curSubiil->getDocidsPriv(
				opr, value, docids, arraysize, curPos, rdata);
		numdocfound += counter;
	}

	AOSUNLOCK(mLock);
	return numdocfound;
}
*/


/*
int
AosIILU64::getDocidsPriv(
		const AosOpr opr,
		const u64 &value,
		u64 *docids,
		const int arraysize,
		int &curPos,
		const AosRundataPtr &rdata)
{
//shawn 1. like ne can not be this way
//2. change to memory copy
	int numfound = 0;
	int startidx = -10;
	int startiilidx = -10;
	int endidx = -10;
	int endiilidx = -10;
	u64 docid;
	bool isunique;
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
	for (int i=startidx; i<=endidx; i++)
	{
		if (curPos >= arraysize -1) break;
		docids[curPos++] = mDocids[i];
		numfound ++;
	}

	return numfound;
}
*/


/*
u64
AosIILU64::getDocIdSafe1(int &idx,int &iilidx, const AosRundataPtr &rdata)
{
// shawn not right, try to remove it
	u64 did = 0;
	AOSLOCK(mLock);
	did = getDocIdPriv1(idx,iilidx, rdata);
	AOSUNLOCK(mLock);
	return did;
}
*/


/*
u64
AosIILU64::getDocIdPriv1(int &idx, int &iilidx, const AosRundataPtr &rdata)
{
// shawn not right, try to remove it
	if(!isRootIIL())
	{
		return getDocIdPriv(idx,rdata);
	}

	// is rootiil
	AosIILU64Ptr subiil = getSubiilByIndex(iilidx, false, rdata);
	aos_assert_r(iilidx >= 0, 0);
	aos_assert_r(mNumSubiils > 0, 0);
	u64 docid = 0;
	while (subiil)
	{
		if(subiil == this)docid = subiil->getDocIdPriv(idx,rdata);
		else docid = subiil->getDocIdSafe(idx,rdata);

		if(docid > 0)
		{
			return docid;
		}
		if(iilidx >= (int)mNumSubiils -1)
		{
			return 0;
		}
		iilidx++;
		idx = 0;
		subiil = getSubiilByIndex(iilidx, false, rdata);
	}
	return 0;
}
*/


/*
bool
AosIILU64::verifyDuplicatedEntriesPriv(const AosRundataPtr &rdata)
{
// shawn not right, try to remove it
	OmnScreen << "\nVerify duplicated entries: Total subiils: " << mNumSubiils << endl;
	for (u32 i=0; i<mNumSubiils; i++)
	{
		AosIILU64Ptr crt_subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(crt_subiil, false);
		u32 crt_numdocs = crt_subiil->mNumDocs;

		u64* crt_values = crt_subiil->mValues;

		for (u32 mm=0; mm<crt_numdocs; mm++)
		{
			const u64 vv = crt_values[mm];
			u64* crt_docids = crt_subiil->mDocids;
			for (u32 kk=mm+1; kk<crt_numdocs; kk++)
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
			for (u32 aa=i+1; aa<mNumSubiils; aa++)
			{
				AosIILU64Ptr next_subiil = getSubiilByIndex(aa, false, rdata);
				u64 *values = next_subiil->mValues;
				u32 entries = next_subiil->mNumDocs;
				for (u32 bb=0; bb<entries; bb++)
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
*/


/*
bool
AosIILU64::incrementCounter(
		const u64 &counter_id, 
		const u64 &value,
		const AosRundataPtr &rdata)
{
// shawn same as incrementDocidSafe1(...)
	// This function increments the counter 'counter_id' by 'value'. 
	// If the counter is not in the list yet, it will add it.
	int idx = -10;
	int iilidx = -10;
	bool isunique;
	u64 ctid = counter_id;
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
		rslt = addDocPriv(counter_id, value, true, false, false, rdata);
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
	rslt = addDocPriv(counter_id, counter, true, false, false, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}
*/


/*
bool 		
AosIILU64::addDocSeqPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		int	&iilidx,
		int &idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
// shawn remove it
	aos_assert_r(value != "", false);
	bool rslt = false;
	bool keepSearch = false;
	// This function inserts the pair (value, docid) into the 
	// list. The list is sorted based on value, ascending.
	if (isRootIIL())
	{
		// It is segmented and the iil is the root iil. 
		AosIILU64Ptr subiil;

		// Chen Ding, Bug1217
		iilidx = getSubiilIndexSeq(value, iilidx);
		if (iilidx == -5) iilidx = 0;
		aos_assert_r(mNumSubiils > 0,false);
		while (iilidx <= (int)mNumSubiils -1)
		{
			subiil = getSubiilByIndex(iilidx, iilmgrLocked, rdata);
			aos_assert_r(subiil, false);
			keepSearch  = false;
			
			if (subiil.getPtr() == this)
			{
				rslt = addDocSeqDirectPriv(
						value, docid, value_unique, docid_unique, idx, iilmgrLocked, keepSearch, rdata);
			}
			else
			{
				rslt = subiil->addDocSeqDirectSafe(
						value, docid, value_unique, docid_unique, idx, iilmgrLocked, keepSearch, rdata);
			}
			
			if(rslt)
			{
				AosIILCompStrPtr compiil = retrieveCompIIL(iilmgrLocked, rdata);
				aos_assert_r(compiil, false);				
				bool rslt = compiil->addDocSafe(value,docid,false,false,iilmgrLocked,rdata);
				returnCompIIL(compiil,rdata);
				aos_assert_r(rslt, false);
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
			value, docid, value_unique, docid_unique, idx, iilmgrLocked, keepSearch, rdata);
	if(rslt)
	{
		AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		aos_assert_r(compiil, false);				
		bool rslt = compiil->addDocSafe(value,docid,false,false,iilmgrLocked,rdata);
		returnCompIIL(compiil,rdata);
		aos_assert_r(rslt, false);
	}
	return rslt;	
}
*/


/*
bool 		
AosIILU64::addDocSeqDirectPriv(
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		int		   &curidx,
		const bool iilmgrLocked,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
// shawn remove it
	keepSearch = false;
	aos_assert_r(value != "", false);
//	AosIILU64Ptr r1 = (AosIILU64*)mRootIIL.getPtr();
//	if(r1)r1->splitSanityCheck();
	// The caller has determined that 'value' is in this iil.
	// It will add the doc into this iil unless:
	// 	1. mValues[mNumDocs-1] == value
	// 	2. mDocids[mNumDocs-1] < docid
	// In this case, the doc may be inserted in the next subiil.
	
	// Check whether we need to split

	int rsltInt = 0;
	int rsltInt2 = 0;
	bool rslt = false;
	u32 numBefore = mNumDocs;

	// OmnScreen << "IILU64 add: " << this << endl;
	u32 idx = 0;
	int left = 0; 
	int right = mNumDocs-1;
	if(curidx > 0)
	{
		left = curidx;
	}
	if(left > right)
	{
		left = right;
	}
	if(AosIILUtil::valueMatch(mValues[left], eAosOpr_ge, value.data(), mIsNumAlpha))
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
				// The value must be unique. This fails the operation. 
				// if 'value_unique' is AOSIIL_FORCE_UNIQUE, it will 
				// override the entry. Otherwise, it returns an error. 
				// if (value_unique == AOSIIL_FORCE_UNIQUE)
				// {
				// 	// Force unique
				// 	mDocids[idx] = docid;
				// 	return true;
				// }

				OmnAlarm << "Value already exist: " << value << ":" << docid
					<< ":" << mDocids[idx] << enderr;
				return false;
			}

			// Need to insert the doc based on docid
			rslt = insertDocPriv(
					idx, value, docid, value_unique, docid_unique, iilmgrLocked, keepSearch);
			if (!rslt) return rslt;
			if(!keepSearch)
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
			if(rsltInt2 > 0)
			{
				// Found the spot. Need to move the elements after
				// 'idx' to make room for the new docid
				insertAfter(idx, docid, value);
				idx++;
				goto finished;
			}
			else if(rsltInt2 == 0)
			{
				if(value_unique)
				{
					OmnAlarm << "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx] << enderr;
					return false;
				}
				idx++;
				rslt = insertDocPriv(idx, value, docid, value_unique, docid_unique,
								iilmgrLocked,keepSearch);
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
			else if(rsltInt2 == 0)
			{
				if(value_unique)
				{
					OmnAlarm << "Value already exist: " << value << ":" << docid
						<< ":" << mDocids[idx] << enderr;
					return false;
				}
				idx --;
				rslt = insertDocPriv(idx, value, docid, value_unique, docid_unique,
							iilmgrLocked,keepSearch);
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
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);
	}

	if (mNumDocs >= mMaxIILSize)
	{
		AosIILPtr subiil;
		rslt = splitListPriv(iilmgrLocked, subiil, rdata);

	}
	sanityTestForSubiils();
//	AosIILU64Ptr r = (AosIILU64*)mRootIIL.getPtr();
//	if(r)r->splitSanityCheck();
	return true;
}
*/


/*
bool		
AosIILU64::removeDocSeqPriv(
		const u64 &value, 
		const u64 &docid,
		int	&iilidx,
		int	&idx,
		const AosRundataPtr &rdata)
{
// shawn remove it
	bool keepSearch = true;
	bool rslt = false;
	// Check whether it is a root iil and it is segmented
	if (isRootIIL())
	{
		// It is segmented and the iil is the root iil. 
		// If 'iilidx' is within [0, mNumSubiils-1], 
		// use it.
		AosIILU64Ptr subiil;
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
			if(rslt)
			{
				AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
				aos_assert_r(compiil, false);				
				bool rslt = compiil->removeDocSafe(value,docid,rdata);
				returnCompIIL(compiil,rdata);
				aos_assert_r(rslt, false);
				return true;
			}
			if(!keepSearch) break;
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
	// if(!rslt) OmnAlarm << "Failed to remove doc: " << value << ":" << docid << enderr;
	if(rslt)
	{
		AosIILCompStrPtr compiil = retrieveCompIIL(false, rdata);
		aos_assert_r(compiil, false);				
		bool rslt = compiil->removeDocSafe(value,docid,rdata);
		returnCompIIL(compiil,rdata);
		aos_assert_r(rslt, false);
	}
	return rslt;
}
*/
		

/*
bool		
AosIILU64::removeDocSeqDirectPriv(
		const u64 &value, 
		const u64 &docid,
		int	&curiilidx,
		int &curidx,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
// shawn remove it
	if (mNumDocs <= 0) return false;

	//int startidx = -10;
	int startidx = 0;

	if(curidx > 0)startidx = curidx;
	int endidx = mNumDocs -1;
	if(endidx < startidx)
	{
		return false;
	}
	
	int idx = -1;
	u64 did;
	keepSearch = false;
	bool rslt = false;
	
	if (AosIILUtil::valueMatch(mValues[startidx], eAosOpr_eq, value.data(), mIsNumAlpha))
	{
		// found the startidx
	}
	else
	{
		// Find the first entry that matches the value
		rslt = nextDocidEQ(startidx, 1, value, did);
		if(!rslt)
		{
			// Did not find it.
			return false;
		}
	}
	
	if (startidx < 0 || (u32)startidx >= mNumDocs)
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
		else if(mDocids[idx] > docid)
		{
			// not found
			break;
		}
		idx ++;
	}

	if(idx > (int)mNumDocs -1)
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
	aos_assert_r(idx >= 0 && (u32)idx < mNumDocs, false);

	// Change mValues
	OmnMemMgrSelf->release(mValues[idx], __FILE__, __LINE__);
	if ((u32)idx < mNumDocs-1)
	{
		int ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mDocids[idx+1], ss);
		OmnMemCheck(&mDocids[idx], ss);
		memmove(&mDocids[idx], &mDocids[idx+1], ss);

		ss = sizeof(u64) * (mNumDocs - idx - 1);
		OmnMemCheck(&mValues[idx], ss);	
		OmnMemCheck (&mValues[idx+1], ss);	
		memmove(&mValues[idx], &mValues[idx+1], ss);
	}
	decNumDocsNotSafe();

	mValues[mNumDocs] = 0;
	mDocids[mNumDocs] = 0;
	aos_assert_r(checkMemory(), false);
	mIsDirty = true;
	
	if (isRootIIL() || isLeafIIL())
	{
		bool updateMin = (idx == 0);
		bool updateMax = ((u32)idx >= mNumDocs-1);
		aos_assert_r(mIILIdx >= 0, false);
		aos_assert_r(mRootIIL, false);
		aos_assert_r(mRootIIL->getIILType() == eAosIILType_Str, false);
		AosIILU64 *rootiil = (AosIILU64*)mRootIIL.getPtr();
		rootiil->updateIndexData(mIILIdx,updateMax,updateMin);

		if(mNumDocs < mMinSubIILSize)
		{
			rootiil->mergeSubiilPriv(true, mIILIdx, rdata);
			curiilidx = -1;
			curidx = -1;
		}
	}	
	return true;
}
*/

/*
bool    
AosIILU64::removeDocByPrefixPriv(
		const u64 &prefix, 
		const AosRundataPtr &rdata)
{
	// it may cause iil merge, just for torturer
	mIILMergeFlag = true;
	
	// Check whether it is a root iil and it is segmented
	bool rslt = false;
	u64 docid = AOS_INVDID;
	if (isRootIIL())
	{
		//1. get the first place and the last place 
		//2. remove docs between them
		//3. remove unused sub iils
		//4. merge iils if needed

		//1. get the first place and the last place 
		int startidx = -10;
		int startiilidx = -10;
		int endidx = -10;
		int endiilidx = -10;
		u64 docid = AOS_INVDID;

		rslt = nextDocidPrefixPriv(
					startidx, startiilidx, false, prefix, docid, rdata);
		aos_assert_r(rslt,false);
		if(docid == AOS_INVDID)
		{
			// it means no doc is match
			return true;
		}		
		rslt = nextDocidPrefixPriv(
					endidx, endiilidx, true, prefix, docid, rdata);
		aos_assert_r(rslt, false);
		aos_assert_r(startidx >= 0, false);
		aos_assert_r(startiilidx >= 0, false);
		aos_assert_r(endidx >= 0, false);
		aos_assert_r(endiilidx >= 0, false);
		aos_assert_r(endiilidx >= startiilidx, false);
		if(endiilidx == startiilidx)
		{
			aos_assert_r(endidx >= startidx, false);
		}

		//2. remove docs between them
		if(endiilidx ==startiilidx)
		{
			rslt = removeDocsOnOneSub(startiilidx, startidx, endidx, rdata);
			aos_assert_r(rslt, false);
		}else
		{
			// remove the first iil
			rslt = removeDocsOnOneSub(startiilidx, startidx, -10, rdata);
			aos_assert_r(rslt, false);
			// remove the last iil
			rslt = removeDocsOnOneSub(endiilidx, 0, endidx, rdata);
			// remove all in the middle
			for(int i=startiilidx+1; i<endiilidx; i++)
			{
				rslt = removeDocsOnOneSub(i, -10, -10, rdata);
				aos_assert_r(rslt, false);
			}			
		}
		
		//3. remove unused sub iils in the middle
		//   Notice that the first iil may contains 0 docs, but we do not handle this case in this block
		// 3.1 process the iils in the middle first
		int iil_to_remove = endiilidx - startiilidx - 1;
		if(iil_to_remove > 0)
		{
			for(int i = 0;i < iil_to_remove;i++)
			{
				rslt = removeSubiil(startiilidx+1, true, rdata);
				aos_assert_r(rslt, false);
			}				
		}
		
		// 4 handle the last iil.
		// 4.1 get the first and the last iil out 
		AosIILU64Ptr subiil1 = 0;
		AosIILU64Ptr subiil2 = 0;
		int numDoc1 = 0;
		int numDoc2 = 0;
		
		subiil1 = getSubiilByIndex(startiilidx, false, rdata);
		aos_assert_r(subiil1,false);
		numDoc1 = subiil1->getNumDocs(); 
		if(startiilidx != endiilidx)
		{
			subiil2 = getSubiilByIndex(startiilidx +1, false, rdata);
			aos_assert_r(subiil2,false);
			numDoc2 = subiil2->getNumDocs(); 
		}

		// 4.2 remove the last iil if needed
		if(subiil2 && (numDoc2 == 0))
		{
			rslt = removeSubiil(startiilidx +1,true, rdata);
			aos_assert_r(rslt,false);
			subiil2 = 0;
			numDoc2 = 0;
		}

		// 3.3 merge the last iil if needed
		if(subiil2)
		{
			// now subiil2 is not empty
			// update subiil2 information first
			updateIndexData(startiilidx+1, true, true);

			// if num of subiil2 is too small, merge it someway
			if(numDoc2 < (int)mMinSubIILSize)
			{
				//3.3.1 If iilstart + iilend > (int)mMaxIILSize , move some doc to iil1
				if(numDoc1 + numDoc2 >= (int)mMaxIILSize)
				{
					int numToMove = (numDoc1 + numDoc2)/2 - numDoc2;
					aos_assert_r(numToMove > 0, false); 
					rslt = mergeSubiilForwardPriv(true, startiilidx, numToMove, rdata);
					aos_assert_r(rslt, false); 
				}
				else
				{
					rslt = mergeSubiilBackwardPriv(true, startiilidx+1, numDoc2, rdata);
					aos_assert_r(rslt, false); 
				}
			}
		}
		
		// 5. handle the first iil
		// 5.1 get the num of the first iil again cause it may be changed
		numDoc1 = subiil1->getNumDocs();
		if(!isSingleIIL() && numDoc1 > 0)
		{
			updateIndexData(startiilidx,true,true);
		}

		if(!isSingleIIL() && numDoc1 == 0)
		{
			// check whether the startiilidx == 0 or not
			if(startiilidx == 0)
			{
				// move the 2nd iil to the first.
				AosIILU64Ptr subiil1 = getSubiilByIndex(1, false, rdata);
				int numToMove = subiil1->getNumDocs();
				rslt = mergeSubiilBackwardPriv(true,1,numToMove, rdata);
				aos_assert_r(rslt, false); 
				return rslt;
			}
			rslt = removeSubiil(startiilidx,true, rdata);
			aos_assert_r(rslt,false);
			return true;
		}
		else if(!isSingleIIL() && numDoc1 < (int)mMinSubIILSize)
		{
			rslt = mergeSubiilPriv(true,startiilidx, rdata);
			aos_assert_r(rslt,false);
			return true;
		}
		return true;
	}	
	// The IIL is not segmented
	// 1. get the startidx by the prefix,
	// 2. get the endidx by the prefix
	
	int startidx = -10;
	int endidx = -10;
	bool keepSearch = false;
	rslt = nextDocidPrefixSinglePriv(startidx, false, prefix,docid,keepSearch);
	aos_assert_r(rslt,false);
	if(docid == AOS_INVDID)
	{
		// it means no doc is match
		return true;
	}		
	rslt = nextDocidPrefixSinglePriv(endidx, true, prefix,docid,keepSearch);
	aos_assert_r(rslt,false);
	aos_assert_r(startidx >=0,false);
	aos_assert_r(endidx >=0,false);
	aos_assert_r(endidx >=startidx,false);

	rslt = removeDocFromSubiil2Priv(startidx, endidx, rdata);
	return rslt;
}
*/

/*
bool
AosIILU64::queryValuePriv(
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool value_unique,
		const AosRundataPtr &rdata)
{
	// shawn do we need to query value??????
	if(opr == eAosOpr_ne)
	{
		return queryValueNEPriv(values, value, value_unique, rdata);
	}

	if(opr == eAosOpr_like)
	{
		return queryValueLikePriv(values, value, value_unique, rdata);
	}

	if(isSingleIIL())
	{
		return queryValueSinglePriv(values, opr, value, value_unique, false, rdata);
	}

	bool rslt = false;

	// copy data
	int iilidx_start = getSubIILIndex(-10, value, opr, true);
	if(iilidx_start < 0) return true;

	int iilidx_end = getSubIILIndex(-10, value, opr, false);
	if(iilidx_start < 0) return true;

	AosIILU64Ptr subiil;
	bool getall = false;
	for(int i = iilidx_start; i <= iilidx_end; i++)
	{
		if(i != iilidx_start && i != iilidx_end)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}

		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(),false);

		rslt = subiil->queryValueSinglePriv(values, opr, value, value_unique, getall, rdata);
		aos_assert_r(rslt,false);
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
	// shawn do we need to query value??????
	if (isSingleIIL())
	{
		return queryValueNESinglePriv(values, value, unique_value, false, rdata);
	}

	bool rslt = false;
	AosIILU64Ptr subiil;
	
	int iilidx_start = getSubIILIndex(-10, value, eAosOpr_eq, true);
	if (iilidx_start < 0)
	{
		for(u32 i = 0;i < mNumSubiils;i++)
		{
			subiil = getSubiilByIndex(i, false, rdata);
			aos_assert_r(subiil.notNull(),false);
			rslt = subiil->queryValueNESinglePriv(values, value, unique_value, true, rdata);
			aos_assert_r(rslt, false);
		}
		return true;
	}
	
	int iilidx_end = getSubIILIndex(-10, value, eAosOpr_eq, false);
	aos_assert_r(iilidx_end >= 0,false);

	bool getall = false;
	for(int i = 0; i <= iilidx_start; i++)
	{
		if(i != iilidx_start)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}
		
		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(),false);
		rslt = subiil->queryValueNESinglePriv(values, value, unique_value, getall, rdata);
		aos_assert_r(rslt, false);
	}

	if(iilidx_end == iilidx_start)
	{
		iilidx_end++;
	}

	for(u32 i = (u32)iilidx_end;i < mNumSubiils;i++)
	{
		if(i != (u32)iilidx_end)
		{
			getall = true;
		}
		else
		{
			getall = false;
		}
		
		subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(),false);
		rslt = subiil->queryValueNESinglePriv(values, value, unique_value, getall, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}
*/


/*
bool
AosIILU64::queryValueSinglePriv(
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool unique_value,
		const bool getall,
		const AosRundataPtr &rdata)
{
	// shawn  need to ask pengzhihu
	bool rslt = false;
	int idx_start = -10;
	int idx_end = -10;
	u64 docid;
	bool isunique = false;
	if (getall || opr == eAosOpr_an)
	{
		idx_start = 0;
		idx_end = mNumDocs - 1;
	}
	else
	{
		rslt = nextDocidSinglePriv(idx_start, false, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
		if(idx_start < 0) return true;
		
		rslt = nextDocidSinglePriv(idx_end, true, opr, value, docid, isunique, rdata);
		aos_assert_r(rslt, false);
	}

	for(int i = idx_start; i <= idx_end; i++)
	{
		if(unique_value && i == 0 && (!values.empty()) && values.back() == (u64)(mValues[0]))
		{
			continue;
		}
		//if(unique_value && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if(unique_value && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
		values.push_back(mValues[i]);
	}
	return true;
}


bool
AosIILU64::queryValueLikeSinglePriv(
		vector<u64> &values,
		const u64 &value,
		const bool unique_value)
{
// shawn  need to ask pengzhihu
	for(int i= 0;i <= (int)mNumDocs -1;i++)
	{
		if(unique_value && i == 0 && (!values.empty()) && values.back() == u64(mValues[0]))
		{
			continue;
		}
		//if(unique_value && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if(unique_value && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
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
*/

/*
bool
AosIILU64::queryValueLikePriv(
		vector<u64> &values,
		const u64 &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
// shawn do we need to query value??????
	if(isSingleIIL())
	{
		return queryValueLikeSinglePriv(values, value, unique_value);
	}

	for(u32 i = 0;i < mNumSubiils;i++)
	{
		AosIILU64Ptr subiil = getSubiilByIndex(i, false, rdata);
		aos_assert_r(subiil.notNull(),false);
		subiil->queryValueLikeSinglePriv(values, value, unique_value);
	}
	return true;
}
*/


/*
bool
AosIILU64::queryValueNESinglePriv(
		vector<u64> &values,
		const u64 &value,
		const bool value_unique,
		const bool getall,
		const AosRundataPtr &rdata)
{
// shawn do we need to query value??????
	if(mNumDocs <= 0) return true;
	
	int idx_start = -10;
	int idx_end = -10;
	u64 docid;
	bool isunique = false;

	bool rslt = nextDocidSinglePriv(idx_start, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt, false);
	if(idx_start < 0 || getall)
	{
		for(int i= 0;i <= (int)mNumDocs-1;i++)
		{
			if(value_unique && i == 0 && (!values.empty()) && values.back() == (u64)(mValues[0]))
			{
				continue;
			}
			//if(value_unique && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
			if(value_unique && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
			{
				continue;
			}
			values.push_back(mValues[i]);
		} 
		return true;
	}

	rslt = nextDocidSinglePriv(idx_end, false, eAosOpr_eq, value, docid, isunique, rdata);
	aos_assert_r(rslt,false);
	
	for(int i= 0;i <= (idx_start - 1);i++)
	{
		if(value_unique && i == 0 && (!values.empty()) && values.back() == (u64)(mValues[0]))
		{
			continue;
		}
		//if(value_unique && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if(value_unique && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
		values.push_back(mValues[i]);
	} 
	
	for(int i= (idx_end + 1);i <= ((int)mNumDocs - 1);i++)
	{
		if(value_unique && i == 0 && (!values.empty()) && values.back() == (u64)(mValues[0]))
		{
			continue;
		}
		//if(value_unique && i > 0 && (strcmp(mValues[i], mValues[i-1]) == 0))
		if(value_unique && i > 0 && (AosIILUtil::valueMatch(mValues[i], mValues[i-1], mIsNumAlpha) == 0))
		{
			continue;
		}
		values.push_back(mValues[i]);
	} 
	return true;
}
*/

/*
bool		
AosIILU64::nextDocidSinglePriv(
		int &idx, 
		const bool reverse, 
		const AosOpr opr1,
		const u64 &value1,
		const AosOpr opr2,
		const u64 &value2,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata) 
{
//shawn comment out
	// It searches the next entry starting at 'idx' (including)
	// that meets:
	// 		value1 <= value <= value2
	// If found, 'idx' is the index of the entry found; 
	// 'docid' holds the docid. 
	// 'nomore' is set to false if there is no more. Otherwise,
	// it should set to true.
	if (idx < 0 || (u32)idx >= mNumDocs || mNumDocs == 0) 
	{
		// It may happen.
		idx = -5;
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}

	aos_assert_r(mValues[idx], false);
	if (reverse)
	{
		// In the reverse order. Moving from 'idx' (including)
		// until it hits the right edge (i.e., value2 <= mValues[idx]). 
		// If it moves out of the left edge (i.e., mValues[idx] <= value1)
		// it means there is no more.
		if (AosIILUtil::valueMatch(mValues[idx], opr2, value2, mIsNumAlpha))
		{
			// It enters the right edge. Check whether it is out of
			// the left edge.
			if (AosIILUtil::valueMatch(mValues[idx], opr1, value1, mIsNumAlpha))
			{
				docid = mDocids[idx];
				goto found_wrap;
			}

			// It is out of the left edge, which means there 
			// is no more.
			idx = -5;
			docid = AOS_INVDID;
			isunique = false;
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
			isunique = false;
			return true;
		}

		if (AosIILUtil::valueMatch(mValues[idx], opr1, value1, mIsNumAlpha))
		{
			// Found one
			docid = mDocids[idx];
			goto found_wrap;
		}

		// No more
		idx = -5;
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}
	
	// Normal order. Moving from 'idx' (including)
	// until it hits the left edge (i.e., value1 >= mValues[idx]). 
	// If it moves out of the right edge (i.e., value2 >= mValues[idx])
	// it means there is no more.
	if (AosIILUtil::valueMatch(mValues[idx], opr1, value1, mIsNumAlpha))
	{
		// It enters the left edge. Check whether it is out of
		// the right edge.
		if (AosIILUtil::valueMatch(mValues[idx], opr2, value2, mIsNumAlpha))
		{
			// Found it
			docid = mDocids[idx];
			goto found_wrap;
		}

		// It is out of the right edge, which means there 
		// is no more.
		idx = -5;
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}
	
	// Find the first mValues[...] that is >= value1
	aos_assert_r(findFirst(idx, opr1, value1), false);
	if (idx < -4)
	{
		// No more
		idx = -5;
		docid = AOS_INVDID;
		isunique = false;
		return true;
	}
	OmnNotImplementedYet;
// seems not right lxx
	if (AosIILUtil::valueMatch(mValues[idx], opr2, value2, mIsNumAlpha))
	{
		// Found one
		docid = mDocids[idx];
		goto found_wrap;
	}

	// NO more
	idx = -5;
	isunique = false;
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
*/


