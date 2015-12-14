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
// based on the string value. 
//
// Modification History:
// 11/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Index/IILStr.h"


AosIILStr::AosIILStr(
		const bool isPersis, 
		const AosRundataPtr &rdata)
:
AosIIL(eAosIILType_Str, isPersis)
{
}


AosIILStr::~AosIILStr()
{
}


OmnString 
AosIILStr::getValueSafe(const int idx, const int iilidx, const AosRundataPtr &rdata)
{
	/*
	AOSLOCK(mLock);
	if(isSingleIIL())
	{
		if(!mValues || idx < 0 || idx >= (int)mNumDocs)
		{
			OmnAlarm << "invalid idx: " << idx << ":" << mNumDocs << enderr;
			AOSUNLOCK(mLock);
			return "";
		}
		OmnString value = mValues[idx];
		AOSUNLOCK(mLock);
		return value;
	}
	//mIILIdx = 0;  // Linda, 2010/12/13
	AosIILStrPtr subiil = getSubiilByIndex(iilidx, false, rdata);
	aos_assert_rb(subiil, mLock, "");
	
	if(idx < 0 || (u32)idx >= subiil->mNumDocs)
	{
		OmnAlarm << "invalid idx: " << idx << ":" << subiil->mNumDocs << enderr;
		AOSUNLOCK(mLock);
		return "";
	}
	OmnString value = subiil->mValues[idx];
	AOSUNLOCK(mLock);
	return value;
	*/
	return "";
}


bool 
AosIILStr::nextDocidSafe(
		int &idx, 
		int &iilidx,
		const bool reverse, 
		OmnString &value,
		u64 &docid,
		const AosRundataPtr &rdata) 
{
	/*
	AOSLOCK(mLock);
	bool isunique;
	bool rslt = nextDocidPriv(
					idx, iilidx, reverse, eAosOpr_an, value, docid, isunique, rdata);
	if (rslt && idx >= 0 && iilidx >= 0)
	{
		AosIILStr* iil = 0;
		if (isSingleIIL())
		{
			iil = this;
		}
		else
		{
			AosIILStrPtr the_iil = getSubiilByIndex(iilidx, false, rdata);
			if (!the_iil)
			{
				AOSUNLOCK(mLock);
				OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
				idx = -5;
				iilidx = -5;
				docid = AOS_INVDID;
				return false;
			}
			iil = the_iil.getPtr();
		}

		if ((u32)idx >= iil->mNumDocs)
		{
			AOSUNLOCK(mLock);
			OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
			idx = -5;
			iilidx = -5;
			docid = AOS_INVDID;
			return false;
		}

		if (!iil->mValues)
		{
			AOSUNLOCK(mLock);
			OmnAlarm << "idx: " << idx << ":" << iilidx << enderr;
			idx = -5;
			iilidx = -5;
			docid = AOS_INVDID;
			return false;
		}
		value = iil->mValues[idx];
	}
	AOSUNLOCK(mLock);
	return rslt;
	*/
	return false;
}


OmnString 
AosIILStr::getValueSafeStr(const int idx, u64 &docid)
{
	/*
	aos_assert_r(mNumDocs <= mMemCap, "");
	AOSLOCK(mLock);
	if (idx < 0 || (u32)idx >= mNumDocs) 
	{
		docid = AOS_INVDID;
		AOSUNLOCK(mLock);
		return "";
	}
	docid = mDocids[idx];
	OmnString vv = mValues[idx];
	AOSUNLOCK(mLock);
	return vv;
	*/
	return "";
}


u64		
AosIILStr::getDocIdSafe1(int &idx,int &iilidx,const AosRundataPtr &rdata)
{
	return 0;
}


u64	
AosIILStr::nextDocIdSafe2(
		int &idx,
		int &iilidx,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	return 0;
}


bool 
AosIILStr::nextDocidSafe(
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr1,
		const OmnString &value1,
		const AosOpr opr2,
		const OmnString &value2,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata)
{
	return false;
}


int	
AosIILStr::getTotalSafe(const OmnString &value, const AosOpr opr)
{
	return -1;
}


bool 	
AosIILStr::modifyDocSafe(
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const bool iilmgrLocked,
		const bool override_flag, 		
		const AosRundataPtr &rdata)
{
	return false;
}


bool 
AosIILStr::checkDocSafe(
		const AosOpr opr,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return false;
}


bool 	
AosIILStr::prependDocSafe(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrLocked)
{
	return false;
}


int		
AosIILStr::getDocidsSafe(
		const AosOpr opr,
		const OmnString &value,
		u64 *docids, 
		const int arraysize,
		const AosRundataPtr &rdata)
{
	return -1;
}


u64		
AosIILStr::incrementDocidSafe(
		const OmnString &key,
		const u64 &dft,
		const AosRundataPtr &rdata)
{
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
	return false;
}


bool	
AosIILStr::addBlockSafe(
		char * entries,
		const int size,
		const int num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::increaseBlockSafe(
		char * &entries,
		const int size,
		const int num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	return false;
}


bool 
AosIILStr::saveSubIILToTransFileSafe(
		const AosDocTransPtr &docTrans,
		const bool forcesave, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool 
AosIILStr::saveSubIILToLocalFileSafe(
		const bool forcesave, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::setValueDocUniqueSafe(
		const OmnString &key, 
		const u64 &docid, 
		const bool must_same, 
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::getSplitValueSafe(
		const AosQueryContextPtr &context,
		const int size,
		vector<AosQueryContextPtr> &contexts,
		const bool iilmgrlock,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::queryValuePriv(
		vector<OmnString> &values,
		const AosOpr opr,
		const OmnString &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::querySingleLikePriv(
		const AosQueryRsltPtr &query_rslt,
		const AosBitmapPtr &query_bitmap,
		const OmnString &value,
		const bool setBlockSize,
		const AosRundataPtr &rdata)
{
	return false;
}

	
bool	
AosIILStr::queryRangePriv(
		const AosQueryRsltPtr &query_rslt,
		const AosBitmapPtr &query_bitmap,
		const AosQueryContextPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::preQueryPriv(
		const AosQueryContextPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::addDocSeqDirectPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		int	&idx,
		const bool iilmgrLocked,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	return false;
}


bool 	
AosIILStr::addDocPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	return false;
}


bool			
AosIILStr::queryNewPriv(
		const AosQueryRsltPtr &query_rslt,
		const AosBitmapPtr &query_bitmap, 
		const AosQueryContextPtr &query_context,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::addDocSeqPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		int	&iilidx,
		int	&idx,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	return false;
}


bool 	
AosIILStr::addDocDirectPriv(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrLocked,
		bool &keepSearch,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::queryPriv(
		const AosQueryRsltPtr &query_rslt,
		const AosBitmapPtr &query_bitmap,
		const AosOpr opr,
		const OmnString &value,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::nextDocidPriv(
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::removeDocDirectPriv(
		const OmnString &value, 
		const u64 &docid,
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	return false;
}


bool	
AosIILStr::removeDocSeqDirectPriv(
		const OmnString &value, 
		const u64 &docid,
		int	&iilidx,
		int	&idx,
		bool &keepSearch,
		const AosRundataPtr &rdata) 
{
	return false;
}


AosIILStrPtr 	
AosIILStr::getSubiilByIndex(
		const int idx,
		const bool iilmgrlocked,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::nextDocidPrefixPriv(
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const OmnString &prefix,
		u64 &docid,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::removeDocSeqPriv(
		const OmnString &value, 
		const u64 &docid,
		int	&iilidx,
		int	&idx,
		const AosRundataPtr &rdata) 
{
	return false;
}


bool	
AosIILStr::removeDocPriv(
		const OmnString &value, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	return false;
}


bool 	
AosIILStr::nextDocidSinglePriv(
		int &idx,
		const bool reverse,
		const AosOpr opr1,
		const OmnString &value1,
		const AosOpr opr2,
		const OmnString &value2,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata) 
{
	return false;
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
	return false;
}


bool	
AosIILStr::nextUniqueValuePriv(
		int &idx,
		int &iilidx,
		const bool reverse,
		const AosOpr opr,
		const OmnString &value,
		OmnString &unique_value,
		bool &found,
		const AosRundataPtr &rdata)
{
	return false;
}

	
bool	
AosIILStr::nextUniqueValueSinglePriv(
		int &idx,
		const bool reverse,
		const AosOpr opr,
		const OmnString &value,
		OmnString &unique_value,
		bool &found)
{
	return false;
}


bool	
AosIILStr::nextDocidPrefixSinglePriv(
		int &idx, 
		const bool reverse, 
		const OmnString &prefix,
		u64 &docid,
		bool &keepSearch) 
{
	return false;
}


bool	
AosIILStr::removeDocByPrefixPriv(
		const OmnString &prefix,
		const AosRundataPtr &rdata)
{
	return false;
}


bool	
AosIILStr::deleteIILPriv(
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	return false;
}

