////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMerger/IILMergerBkt.h"

#include "IILMerger/IILMerger.h"
#include "IILMerger/IILMergerHandler.h"
#include "Rundata/Rundata.h"
#include "Util/BuffArray.h"
#include "Util/File.h"


AosIILMergerBkt::AosIILMergerBkt(
		AosIILMerger *merger, 
		const u32 maxmementries,
		const u32 maxentries)
:
mMaxMemEntries(maxmementries),
mMaxEntries(maxentries),
mMerger(merger),
mLow(NULL),
mHigh(NULL),
mTotalEntries(0),
mTotalMemEntries(0),
mComp(mMerger->getComp()),
mHandler(mMerger->getHandler()),
mLock(OmnNew OmnMutex()),
mRegionLock(OmnNew OmnMutex())
{
	mTimestamp = OmnGetSecond();
	mData = (char*)OmnNew char[(*mComp).size * mMaxMemEntries];
	mLow = OmnNew char[1000];
	mHigh = OmnNew char[1000];
	OmnString fname = mMerger->createFileName();
	fname << (u64)(unsigned long)this << ".txt";
	mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs); 
	aos_assert(mFile->isGood());
}


AosIILMergerBkt::~AosIILMergerBkt()
{
	OmnDelete [] mData;
	OmnDelete [] mLow;
	OmnDelete [] mHigh;
	mMerger = 0;
	mData = 0;
	mLow = 0;
	mHigh = 0;
	if (mFile)
	{
		mFile->deleteFile();
	}
}

int 
AosIILMergerBkt::compare(const char* lhs,const char* rhs)
{
	return mMerger->compare(lhs, rhs);
}

char* 
AosIILMergerBkt::checkSort(
		char *begin, 
		const int length)
{
	char* last = 0;
	for (int i = 0; i< length-1; i++)
	{
		aos_assert_r(compare(begin+i*(*mComp).size, begin+(i+1)*(*mComp).size) <= 0, 0);
		if (!last && (compare(begin+i*(*mComp).size, mHigh) > 0))
			last = begin + i*(*mComp).size;
	}
	if (!last && (compare(begin+(length-1)*(*mComp).size, mHigh) > 0))
	{
		last = begin + (length-1)*(*mComp).size;
	}
if (!last)
{
	OmnAlarm << "error" << enderr;
}
	return last;
}

bool 
AosIILMergerBkt::append(
		char *begin, 
		const int num,
		const AosRundataPtr &rdata)
{
	if (mTotalMemEntries)
	{
		sanitycheck(mData, mTotalMemEntries);
	}
	aos_assert_r(begin && compare(begin, mLow) >= 0, false);
	aos_assert_r(compare(begin + (num-1)*((*mComp).size), mHigh) <= 0, false);
	if (mTotalEntries < mMaxEntries)
	{
		if (num+mTotalMemEntries < mMaxMemEntries)
		{
			memcpy(&mData[mTotalMemEntries*(*mComp).size], begin, num*(*mComp).size);
			mTotalMemEntries += num;
			mTotalEntries += num; 
			sanitycheck(mData, mTotalMemEntries);
			mMerger->updateAppendTotal(num);
		}
		else if (num + mTotalEntries >= mMaxEntries)
		{
			bool rslt = overFlow(num, begin, rdata);
			aos_assert_r(rslt, false);
		}
		else
		{
			//felicia,2012/06/25
			//doaction
			// !==============================================================
			u32 fileEntries = mTotalEntries - mTotalMemEntries;
			AosBuffPtr buff = OmnNew AosBuff((*mComp).size*(mTotalMemEntries + num), 0 AosMemoryCheckerArgs);
			int pos = 0;
			if (mTotalMemEntries > 0)
			{
				memcpy(&buff->data()[pos], mData, mTotalMemEntries * (*mComp).size);
				pos = mTotalMemEntries * (*mComp).size;
			}
			memcpy(&buff->data()[pos], begin, num * (*mComp).size);
			buff->setDataLen((*mComp).size*(mTotalMemEntries + num));
			AosBuffArray array(buff, mComp, mHandler->getStable());
			sanitycheck((char*)buff->data(), num + mTotalMemEntries);
			//mFile->lock();
			mFile->append(buff->data(), buff->dataLen(), true);   
			//mFile->unlock();
			mTotalMemEntries = 0;
			mTotalEntries = buff->dataLen() / (*mComp).size + fileEntries; 
			mMerger->updateAppendTotal(num);
		}
	}
	else
	{
		//!==================================================================
		bool rslt = overFlow(num, begin, rdata);
		aos_assert_r(rslt, false);
	}
	sanitycheck(mData, mTotalMemEntries);
	return true;
}

bool
AosIILMergerBkt::overFlow(
		const int num,
		char *begin,
		const AosRundataPtr &rdata)
{
	AosBuffPtr buff = OmnNew AosBuff((*mComp).size*(mTotalEntries + num), 0 AosMemoryCheckerArgs);
	int pos = 0;
	if (mTotalEntries - mTotalMemEntries > 0)
	{
		//mFile->lock();
		mFile->readToBuff(0, (*mComp).size* (mTotalEntries-mTotalMemEntries), buff->data());
		//mFile->unlock();
		pos += (mTotalEntries-mTotalMemEntries) * (*mComp).size;
	}
	memcpy(&buff->data()[pos], begin, num*(*mComp).size);
	pos += num*(*mComp).size;
	if (mTotalMemEntries > 0)
	{
		memcpy(&buff->data()[pos], mData, mTotalMemEntries*(*mComp).size);
	}
	buff->setDataLen((*mComp).size*(num + mTotalEntries));
	sanitycheck((char*)buff->data(), num + mTotalEntries);

	AosBuffArray array(buff, mComp, mHandler->getStable());
	array.sort();
	//felicia,2012/06/25
	// 1. doaction
	// 2. after doaction,if size<mMaxEntries, go on append
	//

	mTotalMemEntries = 0;
	mTotalEntries = 0;
	//mFile->lock();
	mFile->resetFile();
	//mFile->unlock();

	//AosBuffArray newarray(buff, mComp, mHandler->getStable()); 
	//newarray.sort();
	bool rslt;
	int updatenum = (buff->dataLen()/mComp->size) - num;
	aos_assert_r(updatenum >= 0, false);
	mMerger->updateAppendTotal(-updatenum);
	if (buff->dataLen() / (*mComp).size < mMaxEntries * 0.8)
	{
		//array's size < mMaxEntries
		rslt = append(buff->data(), buff->dataLen()/(*mComp).size, rdata);
		return rslt;
	}
	else //if(isFirstAppend || mChildMerger)
	{
		rslt = splitBranch(buff, rdata);
	}
	return rslt;
}

bool
AosIILMergerBkt::splitBranch(
			const AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	if (!mChildMerger)
	{
		u32 layer = mMerger->getLayer();
		OmnString tableid = mMerger->getTableid();
		int maxbkt = mMerger->getMaxBucket();
		aos_assert_r(maxbkt > 0, false);
		mChildMerger = OmnNew AosIILMerger(mHandler, maxbkt, 0, mMaxEntries, ++layer, mMerger);
		mChildMerger->setTableid(tableid);
	}
	bool rslt = mChildMerger->addDataAsync(buff, rdata);
	return rslt;
}

bool
AosIILMergerBkt::mergeAllData(const AosRundataPtr &rdata)
{
	if(mChildMerger)
	{
		AosBuffPtr buff = load();
		//mFile->lock();
		mFile->deleteFile();
		mFile = 0;
		//mFile->unlock();
		mChildMerger->addDataAsync(buff, rdata);
		return mChildMerger->mergeAllData(rdata);
	}
	if (mTotalMemEntries > 0)
	{
		//mFile->lock();
		mFile->append(mData, mTotalMemEntries * mComp->size, true);
		//mFile->unlock();
	    mTotalMemEntries = 0;
	}
	AosBuffPtr buff = load();
	return mMerger->mergeData(buff, rdata);
}


u64
AosIILMergerBkt::getTotalSize()
{
	u64 total = 0;
	if(mChildMerger)
	{
		total += mChildMerger->getNumEntries();
	}
	return total += mTotalEntries;
}

AosBuffPtr
AosIILMergerBkt::load()
{
	AosBuffPtr buff = OmnNew AosBuff((*mComp).size*mTotalEntries, 0 AosMemoryCheckerArgs);
	int pos = 0;
	if (mTotalEntries != mTotalMemEntries)
	{
		//mFile->lock();
		mFile->readToBuff(0, (*mComp).size* (mTotalEntries-mTotalMemEntries), buff->data());
		mFile->seek(0);
		//mFile->unlock();
		pos += (*mComp).size* (mTotalEntries-mTotalMemEntries);
	}

	sanitycheck(mData, mTotalMemEntries);
	if (mTotalMemEntries >0)
	{

		memcpy(&buff->data()[pos], mData, mTotalMemEntries*(*mComp).size);
	}
	buff->setDataLen((*mComp).size*(mTotalEntries));
	AosBuffArray array(buff, mComp, mHandler->getStable()); 
	array.sort();
	sanitycheck(buff->data(), buff->dataLen()/(*mComp).size);

	mTotalMemEntries = 0;
	mTotalEntries = 0;
	return buff;
}


bool 
AosIILMergerBkt::sanitycheck(char *begin, int length)
{
	return true;
	if (mTotalEntries)
	{
		aos_assert_r(strcmp(mLow, "a") >=0, false);
		aos_assert_r(strcmp(mLow, "{") <=0, false);
		aos_assert_r(strcmp(mHigh, "a") >=0, false);
		aos_assert_r(strcmp(mHigh, "{") <=0, false);
	}
	//return true;
	for (int i=0; i<length; i++)
	{
		aos_assert_r(begin+i*(*mComp).size != 0, false);
		aos_assert_r(strcmp(begin+i*(*mComp).size, "a") >= 0, false);
		aos_assert_r(strcmp(begin+i*(*mComp).size, "{") <= 0, false);
		aos_assert_r((compare(begin+i*(*mComp).size, mHigh) <= 0) && (compare(begin+i*(*mComp).size, mLow) >= 0), false);
	}
	return true;
}
