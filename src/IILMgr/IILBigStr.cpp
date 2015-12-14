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
// 	Created: 2013/03/14 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/IILBigStr.h"

#include "SEInterfaces/IILExecutorObj.h"
#include "IILUtil/IILSave.h"


// Ice.Yu 2013/1/24
// 1. Call this function it set the leaf iil's mDocids to docids and mNumDoc 
// to num. and it return the next iil's iilidx.
// 2. Call this function should not locked iilmgr.
// 3. First Call this function should set 0. when it has finished. the 
// iilidx will be -10;

bool
AosIILBigStr::getNextDocids(
		char *** values,
		u64 **docids, 
		i64 &num, 
		AosIILIdx &iilidx,
		const AosRundataPtr &rdata)
{
	if (isLeafIIL())
	{
		*docids = mDocids;
		*values = mValues;
		num = mNumDocs;
		iilidx.setIdx(mLevel+1, iilidx.getIdx(mLevel+1)+1);
		if (!mParentIIL)
		{
			iilidx.setEnd();
		}
		return true;
	}

	AosIILBigStrPtr subiil;
	subiil = getSubiilByIndexPriv(iilidx.getIdx(mLevel), rdata);
	aos_assert_r(subiil, false);
	subiil->getNextDocids(values, docids, num, iilidx, rdata);

	AosIILBigStr* iil= this;
	int crtLevel = mLevel;

	while(iilidx.getIdx(crtLevel) == iil->getNumSubiils())
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
AosIILBigStr::incMergeIIL(
		const AosIILBigStrPtr &iil, 
		const i64 &size, 
		const u64 &dftvalue, 
		const AosIILUtil::AosIILIncType incType,
		const AosRundataPtr &rdata)
{
	iil->lockIIL();	
	char **values = NULL;
	u64 *docids = NULL;
	int len = 0;
	char * data = 0;
	i64 num = 0;
	AosIILIdx iilidx;

	i64 crt_len = 0;
	i64 crt_num = 0;
	i64 sgStaticBuffLen = 100000000; // 100M
	AosBuffPtr buff = OmnNew AosBuff(sgStaticBuffLen AosMemoryCheckerArgs);

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

		if (crt_len + num * size > sgStaticBuffLen)
		{
			buff->setDataLen(crt_len);
			buff->expandMemory1(crt_len + num * size + 10);
		}

		data = buff->data();
		data += crt_len;

		for (i64 i=0; i<num; i++)
		{
			len = strlen(values[i]);
			if (len + (int)sizeof(u64) >= size)
			{
				OmnAlarm << "error" << enderr;
				iil->unlockIIL();
				return false;
			}

			memcpy(data, values[i], len);
			data[len] = 0;
			memcpy(data + size - sizeof(u64), &docids[i], sizeof(u64));
			data += size;
		}
		crt_len += num * size;
		crt_num += num;

		iil->returnSubIILsPriv(rdata);

		if (crt_len >= sgStaticBuffLen || iilidx.isFinished())
		{
			data = buff->data();
			batchIncSafe(data, size, crt_num, dftvalue, incType, rdata);
			crt_len = 0;
			crt_num = 0;
		}
	}
	iil->unlockIIL();
	aos_assert_r(crt_len == 0, false);
	return true;
}


bool 
AosIILBigStr::addMergeIIL(
		const AosIILBigStrPtr &iil, 
		const i64 &size, 
		const AosIILExecutorObjPtr &executor,
		const AosRundataPtr &rdata)
{
	iil->lockIIL();	
	char **values = NULL;
	u64 *docids = NULL;
	int len = 0;
	char * data = 0;
	i64 num = 0;
	AosIILIdx iilidx;

	i64 crt_len = 0;
	i64 crt_num = 0;
	i64 sgStaticBuffLen = 100000000; // 100M
	AosBuffPtr buff = OmnNew AosBuff(sgStaticBuffLen AosMemoryCheckerArgs);

	iilidx.setBegin();
	while(!iilidx.isFinished())	
	{
		iil->getNextDocids(&values, &docids, num, iilidx, rdata); 
		if (num <= 0)
		{
			OmnAlarm << "error" << enderr;
			iil->unlockIIL();
			return false;
		}

		if (crt_len + num * size > sgStaticBuffLen)
		{
			buff->setDataLen(crt_len);
			buff->expandMemory1(crt_len + num * size + 10);
		}
		data = buff->data();
		data += crt_len;

		for (i64 i=0; i<num; i++)
		{
			len = strlen(values[i]);
			if (len + (int)sizeof(u64) >= size)
			{
				OmnAlarm << "error" << enderr;
				iil->unlockIIL();
				return false;
			}

			memcpy(data, values[i], len);
			data[len] = 0;
			memcpy(data + size - sizeof(u64), &docids[i], sizeof(u64));
			data += size;
		}

		crt_len += num * size;
		crt_num += num;

		iil->returnSubIILsPriv(rdata);

		if (crt_len >= sgStaticBuffLen || iilidx.isFinished())
		{
			data = buff->data();
			batchAddSafe(data, size, crt_num, executor, rdata);
			crt_len = 0;
			crt_num = 0;
		}
	}
	iil->unlockIIL();
	aos_assert_r(crt_len == 0, false);
	return true;
}


// Chen Ding, 2013/03/01
bool 
AosIILBigStr::computeQueryResults(
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


void
AosIILBigStr::setSnapShotId(const u64 &snap_id)
{
	AOSLOCK(mLock);
	setSnapShotIdPriv(snap_id);
	AOSUNLOCK(mLock);
}


void
AosIILBigStr::setSnapShotIdPriv(const u64 &snap_id)
{
	aos_assert(mSnapShotId == 0 || snap_id == mSnapShotId);
	mSnapShotId = snap_id;

	if (isParentIIL())
	{
		AosIILBigStrPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->setSnapShotIdPriv(snap_id);
		}
	}
}


void
AosIILBigStr::resetSnapShotId()
{
	AOSLOCK(mLock);
	resetSnapShotIdPriv();
	AOSUNLOCK(mLock);
}


void
AosIILBigStr::resetSnapShotIdPriv()
{
	mSnapShotId = 0;	
	if (isParentIIL())
	{
		AosIILBigStrPtr subiil;
		for(i64 i=1; i<mNumSubiils; i++)
		{
			subiil = mSubiils[i];
			if (!subiil) continue;
			subiil->resetSnapShotIdPriv();
		}
	}
}


bool
AosIILBigStr::resetIIL(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}
