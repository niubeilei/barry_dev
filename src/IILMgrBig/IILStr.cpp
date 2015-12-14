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


// Chen Ding, 01/03/2013
bool 
AosIILStr::retrieveQueryProcBlock(
		AosBuffPtr &buff, 
		const AosQueryContextObjPtr &context, 
		const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 01/03/2013
bool 
AosIILStr::retrieveNodeList(
		AosBuffPtr &buff, 
		const AosQueryContextObjPtr &context, 
		const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2013/01/14
bool 
AosIILStr::retrieveIILBitmap(
		const OmnString &iilname,
		AosBitmapObjPtr &bitmap, 
		const AosBitmapObjPtr &partial_bitmap, 
		const AosBitmapTreeObjPtr &bitmap_tree, 
		AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}



// Ice.Yu 2013/1/24
// 1. Call this function it set the leaf iil's mDocids to docids and mNumDoc 
// to num. and it return the next iil's iilidx.
// 2. Call this function should not locked iilmgr.
// 3. First Call this function should set 0. when it has finished. the 
// iilidx will be -10;
bool
AosIILStr::getNextDocids(
		char *** values,
		u64 **docids, 
		int &num, 
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)

{
	if(isLeafIIL())
	{
		*docids = mDocids;
		*values = mValues;
		num = mNumDocs;
		iilidx.setIdx(mLevel+1, iilidx.getIdx(mLevel+1)+1);
		return true;
	}
	AosIILStrPtr subiil;
	subiil = getSubiilByIndexPriv(iilidx.getIdx(mLevel), false, rdata);
	aos_assert_r(subiil, false);
	subiil->getNextDocids(values, docids, num, iilidx, rdata);

	AosIILStr* iil= this;
	int crtLevel = mLevel;

	while(iilidx.getIdx(crtLevel) == (int)iil->getNumSubiils())
	{
		iilidx.setIdx(crtLevel, 0);
		iilidx.setIdx(crtLevel+1, iilidx.getIdx(crtLevel+1)+1);
		if (!iil->mParentIIL)
		{
			iilidx.setEnd();
			break;
		}
		iil = iil->mParentIIL.getPtr();
		crtLevel++;
	}
	return true;
}

bool 
AosIILStr::incMergeIIL(
		AosIILStrPtr &iil, 
		int size, 
		const u64 dftvalue, 
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	iil->lockIIL();	
	char **values = NULL;
	u64 *docids = NULL;
	int num = 0;
	AosIILIdx iilidx;
	iilidx.setBegin();
	while(!iilidx.isFinished())	
	{
		iil->getNextDocids(&values, &docids, num, iilidx, rdata); 
		if (num < 0)
		{
			OmnAlarm << enderr;
			iil->unlockIIL();
			return false;
		}

		AosBuffPtr buff = OmnNew AosBuff(num*size, 0 AosMemoryCheckerArgs);
		char *data = buff->data();
		for (int i=0; i<num; i++)
		{
			strcpy(data, values[i]);
			memcpy(data+size-sizeof(u64), &docids[i], sizeof(u64));
			data = data + size;
		}
		iil->unlockIIL();

		data = buff->data();
		incBlockSafe(data, size, num, dftvalue, incType, false, rdata);
		iil->lockIIL();
	}
	iil->unlockIIL();
	return true;
}

bool 
AosIILStr::addMergeIIL(
		AosIILStrPtr &iil, 
		int size, 
		const AosRundataPtr &rdata)
{
	iil->lockIIL();	
	char **values = NULL;
	u64 *docids = NULL;
	int num = 0;
	AosIILIdx iilidx;
	iilidx.setBegin();
	while(!iilidx.isFinished())	
	{
		iil->getNextDocids(&values, &docids, num, iilidx, rdata); 
		if (num < 0)
		{
			OmnAlarm << enderr;
			iil->unlockIIL();
			return false;
		}

		AosBuffPtr buff = OmnNew AosBuff(num*size, 0 AosMemoryCheckerArgs);
		char *data = buff->data();
		for (int i=0; i<num; i++)
		{
			strcpy(data, values[i]);
			memcpy(data+size-sizeof(u64), &docids[i], sizeof(u64));
			data = data + size;
		}
		iil->unlockIIL();

		data = buff->data();
		addBlockSafe(data, size, num, false, rdata);
		iil->lockIIL();
	}
	iil->unlockIIL();
	return true;
}


// Chen Ding, 2013/03/03
u64
AosIILStr::getSubIILID(const int idx) const
{
	aos_assert_r(idx >= 0 && (u32)idx < mNumSubiils, 0);
	return mSubiils[idx];
}


int64_t 
AosIILStr::getNumDocsInSubiil(const int idx) const
{
	OmnNotImplementedYet;
	return -1;
}


bool 
AosIILStr::retrieveQueryBlock(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &query)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2013/03/01
bool 
AosIILStr::computeQueryResults(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		const AosQueryContextObjPtr &context, 
		const AosBitmapObjPtr &bitmap, 
		const u64 &query_id, 
		const int physical_id)
{
	OmnNotImplementedYet;
	return false;
}

