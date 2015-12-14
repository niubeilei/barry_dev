////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/27/2012 Created by Ice Yu
////////////////////////////////////////////////////////////////////////////
#include "Sorter/DataSourceFixBin.h"
#include "Sorter/MergeFileSorter.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"

DataSourceFixBin::DataSourceFixBin(AosNetFileObjPtr &file, int buffSize, AosCompareFun *cmp)
:
DataSource(file, buffSize, cmp)
{
}

DataSourceFixBin::DataSourceFixBin(const AosBuffPtr &buff, AosCompareFun *cmp) 
:
DataSource(buff, cmp)
{
}

DataSourceFixBin::DataSourceFixBin(AosCompareFun *cmp)
:
DataSource(cmp)
{
}


char* 
DataSourceFixBin::getHeadBuff()
{
	return getData();
}

char*
DataSourceFixBin::getData()
{
	if (!mData)
	{
		aos_assert_r(!mBuffRaw, NULL);
		while (!mNextBuffRaw)
		{
			aos_assert_r(!mEOF, NULL);
			mSem->wait();
		}
		mBuff = mNextBuff;
		mBuffRaw = mBuff.getPtr();
		mData = mBuffRaw->data();
		mNextBuff = 0;
		mNextBuffRaw = 0;
		if (!mEOF)
		{
			mFile->readData(mReqId, mBuffSize, OmnApp::getRundata().getPtr());
		}
	}
	aos_assert_r(mData, NULL)
	return mData;
}


void 
DataSourceFixBin::fileReadCallBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished, 
		const AosDiskStat &disk_stat)
{
	mNextBuff = mFile->getBuff();
	bool rslt = AosMergeFileSorter::sanitycheck(mCmpRaw, mNextBuff->data(), mNextBuff->dataLen());
	aos_assert(rslt);

	mNextBuffRaw = mNextBuff.getPtr();
	mReadTotal += expected_size;
	if (mReadTotal == mFileLen)
	{
		mEOF = true;
	}
	mSem->post();
	return;
}

int 
DataSourceFixBin::getEntryLen()
{
	return mCmpRaw->size;
}


void 
DataSourceFixBin::moveNext()
{
	aos_assert(mBuffRaw);
	char *tmp = mData + mCmpRaw->size;
	if (tmp < mBuffRaw->data() + mBuffRaw->dataLen())
	{
		mData = tmp;
		mProcTotal++;
	}
	else
	{
		mData = 0;
		mBuff = 0;
		mBuffRaw = 0;
	}
	if (empty())
	{
		setMaxValue();
		mFile = NULL;
		return;
	}
	return;
}

