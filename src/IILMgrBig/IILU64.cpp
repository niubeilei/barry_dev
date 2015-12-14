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


bool 
AosIILU64::setValueDocUnique(                                                                               
         const u64 &key,
         const u64 &docid,
         const bool must_same,
         const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosIILU64::nextDocidPriv(int &idx,
            int &iilidx,
            const bool reverse, 
            const AosOpr opr,
            u64 &value,
            u64 &docid, 
            bool &isunique,
            const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 01/03/2013
bool 
AosIILU64::retrieveQueryProcBlock(
		AosBuffPtr &buff, 
		const AosQueryContextObjPtr &context, 
		const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 01/03/2013
bool 
AosIILU64::retrieveNodeList(
		AosBuffPtr &buff, 
		const AosQueryContextObjPtr &context, 
		const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2013/01/14
bool 
AosIILU64::retrieveIILBitmap(
		const OmnString &iilname,
		AosBitmapObjPtr &bitmap, 
		const AosBitmapObjPtr &partial_bitmap, 
		const AosBitmapTreeObjPtr &bitmap_tree, 
		AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2013/03/03
u64
AosIILU64::getSubIILID(const int idx) const
{
	aos_assert_r(idx >= 0 && (u32)idx < mNumSubiils, 0);
	return mSubiils[idx];
}

