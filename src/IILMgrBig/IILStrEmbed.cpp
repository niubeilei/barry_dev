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
// 2013/02/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/IILStr.h"

/*
bool	
AosIILStr::addDocSafe(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const bool iilmgrlocked,
		const AosRundataPtr &rdata)
{
	return addDocSafe(value, docid, value_unique, docid_unique, rdata);
}


bool 	
AosIILStr::addDocSafe(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = addDocPriv(value, docid, value_unique, docid_unique, rdata);
	aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);
	return rslt;
}

	
bool	
AosIILStr::deleteIILSafe(
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = deleteIILPriv(iilmgrLocked, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool 	
AosIILStr::addDocRecSafe(
		const OmnString &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = addDocRecPriv(value, docid, value_unique, docid_unique, rdata);
	aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);
	return rslt;
}
	

bool	
AosIILStr::removeDocSafe(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = removeDocPriv(value, docid, rdata);
	aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);
	return rslt;
}
	

bool	
AosIILStr::removeDocRecSafe(
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = removeDocRecPriv(value, docid, rdata);
	aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);
	return rslt;
}
	

bool 	
AosIILStr::modifyDocSafe(
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const bool iilmgrlocked,
		const bool override_flag,
		const AosRundataPtr &rdata)
{
	return modifyDocSafe(oldvalue, newvalue, docid, value_unique, docid_unique, rdata);
}


bool 	
AosIILStr::modifyDocSafe(
		const OmnString &oldvalue, 
		const OmnString &newvalue, 
		const u64 &docid, 
		const bool value_unique,
		const bool docid_unique,
		const bool iilmgrlocked,
		const AosRundataPtr &rdata)
{
	return modifyDocSafe(oldvalue, newvalue, docid, value_unique, docid_unique, rdata);
}


bool	
AosIILStr::nextDocidSafe(
		AosIILIdx the_idx,
		int &idx, 
		int &iilidx,
		const bool reverse, 
		const AosOpr opr,
		const OmnString &value,
		u64 &docid, 
		bool &isunique,
		const AosRundataPtr &rdata)
{
	return nextDocidSafeFind( the_idx, value, opr, docid,
				isunique, reverse, rdata); 
}


bool	
AosIILStr::nextDocidSafeFind(
		AosIILIdx &idx, 
		const OmnString &value,
		const AosOpr opr,
		u64 &docid, 
		bool &isunique,
		const bool reverse, 
		const AosRundataPtr &rdata) 
{
	AOSLOCK(mLock);
	bool rslt = nextDocidPrivFind(idx, value, opr, docid, isunique, reverse, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


u64 
AosIILStr::incrementDocidSafe(
		const OmnString &key,
		const u64 &dft,
		const AosRundataPtr &rdata)
{
	u64 docid;
	bool rslt = incrementDocidSafe(key, docid, 1, dft, true, rdata);
	if (!rslt) return dft;
	return docid;
}


bool 
AosIILStr::incrementDocidSafe(
		const OmnString &key,
		u64 &value,
		const u64 &inc_value,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = incrementDocidPriv(key, value, inc_value, init_value, add_flag, rdata);
	aos_assert_rb(AosIILStrSanityCheck(this), mLock, false);
	AOSUNLOCK(mLock);
	return rslt;
}
	

u64 
AosIILStr::getTotalSafe(
		const OmnString &value, 
		const AosOpr opr, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	u64 total = getTotalPriv(value, opr, rdata);
	AOSUNLOCK(mLock);
	return total;
}
	

bool	
AosIILStr::returnSubIILsSafe(
		const bool iilmgrLocked,
		bool &returned,
		const AosRundataPtr &rdata,
		const bool returnHeader)
{
	AOSLOCK(mLock);
    bool rslt = returnSubIILsPriv(iilmgrLocked, returned, rdata, returnHeader);
    AOSUNLOCK(mLock);
	return rslt;		
}
	

bool	
AosIILStr::querySafe(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = queryPriv(query_rslt, query_bitmap, query_context, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}
	

bool	
AosIILStr::queryRangeSafe(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = queryRangePriv(query_rslt, query_bitmap, query_context, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}
	

bool	
AosIILStr::preQuerySafe(
		const AosQueryContextObjPtr &query_context,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = preQueryPriv(query_context,iilmgrlock, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool	
AosIILStr::queryNewSafe(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = queryNewPriv(query_rslt,query_bitmap,query_context,true,rdata);
	AOSUNLOCK(mLock);
	return rslt; 
}


bool	
AosIILStr::queryNewSafe(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = queryNewPriv(query_rslt,query_bitmap,rdata);
	AOSUNLOCK(mLock);
	return rslt; 
}


bool	
AosIILStr::removeFirstValueDocSafe(
		const OmnString &value,
		u64 &docid,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	AosIILIdx idx;
	bool isunique;
	bool rslt = nextDocidPrivFind(idx, value, eAosOpr_eq, docid, isunique, reverse, rdata);
	if(rslt)
	{
		rslt = removeDocPriv(value, docid, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	AOSUNLOCK(mLock);
	return rslt;
}

	
bool	
AosIILStr::setValueDocUniqueSafe(
		const OmnString &key, 
		const u64 &docid, 
		const bool must_same, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = setValueDocUniquePriv(key, docid, must_same, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}

	
bool	
AosIILStr::incrementDocidSingleSafe(
		const OmnString &key,
		u64 &value,
		const u64 &inc_value,
		const u64 &init_value,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = incrementDocidSinglePriv(key, value, 
			inc_value, init_value, add_flag, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


AosIILStrPtr 	
AosIILStr::getSubiilByCondPriv(
		const OmnString &value, 
		const u64 &docid,
		const AosOpr opr,
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	AosIILIdx idx;
	return getSubiilByCondPriv(idx, value, docid, opr, reverse, rdata);
}
	

AosIILStrPtr 	
AosIILStr::getSubiilByCondPriv(
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse, 
		const AosRundataPtr &rdata)
{
	AosIILIdx idx;
	return getSubiilByCondPriv(idx, value, opr, reverse, rdata);
}


bool			
AosIILStr::nextUniqueValueSafe(
		AosIILIdx &idx,
		const bool reverse,
		const AosOpr opr,
		const OmnString &value,
		OmnString &unique_value,
		bool &found,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = nextUniqueValueSafe(idx, reverse, opr, value, unique_value, found, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool			
AosIILStr::getSplitValueSafe(
		const int &num_blocks,
		const AosQueryRsltObjPtr &query_rslt,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = getSplitValuePriv(num_blocks, query_rslt,iilmgrlock,rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool			
AosIILStr::getSplitValueRecSafe(
		int &num_splitters, 
		int &cur_split_num, 
		int &num_each_split, 
		int &cur_doc_num,
		OmnString &cur_value,
		const AosQueryRsltObjPtr &query_rslt,
		const bool iilmgrlock, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = getSplitValueRecSafe( num_splitters, cur_split_num, 
			num_each_split, cur_doc_num, cur_value, query_rslt,
			iilmgrlock, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool	
AosIILStr::deleteIILRecSafe(
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = deleteIILRecPriv(iilmgrLocked, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool	
AosIILStr::getSplitValueSafe(
		const AosQueryContextObjPtr &context,
		const int size,
		vector<AosQueryContextObjPtr> &contexts,
		const bool iilmgrlock,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = getSplitValuePriv(context,size,contexts,iilmgrlock,rdata);
	AOSUNLOCK(mLock);
	return rslt;
}
*/

