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
//
// Modification History:
// 08/31/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/LogBag.h"

#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "Util/File.h"


AosLogBag gAosLogBag("", true);

AosLogBag::AosLogBag(const OmnString &fname, const bool)
:
mCount(0),
mCrtIdx(0),
mLock(OmnNew OmnMutex()),
mSeqno(0),
mFileIdx(0),
mNumRecords(0)
{
	mFname = fname;
	if (mFname == "") mFname = "log";

	// OmnString ff = mFname;
	// ff << "_" << mFileIdx++;
	// mFile = OmnNew OmnFile(ff, OmnFile::eCreate);
	// aos_assert(mFile && mFile->isGood());
}


AosLogBag::~AosLogBag()
{
}


OmnString & 
AosLogBag::getEntry(const char *file, const int line)
{
	mLock->lock();
	mCrtIdx++;
	if (mCrtIdx >= eSize) mCrtIdx = 0;
	int theidx = mCrtIdx;
	mCount++;
	if (mFile && mFile->isGood() && mCount >= 20)
	{
		// Need to save the contents
		int last_idx = mCrtIdx - mCount;
		if (last_idx < 0) last_idx += eSize;
		for (int c=0; c<mCount-10; c++)
		{
			int ddd = last_idx + c;
			if (ddd >= eSize) ddd -= eSize;
			OmnString ss = mContents[ddd];
			ss << "\n";
			mFile->append(ss, false);
			mNumRecords++;
		}
		mCount = 10;
		mFile->flushFileContents();
	}

	if (mFile && mNumRecords >= 100000)
	{
		OmnString fname = mFname;
		fname << "_" << mFileIdx;
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);

		mFileIdx++;
		if (mFileIdx >= eMaxFiles)
		{
			mFileIdx = 0;
		}
		mNumRecords = 0;
	}

	mContents[theidx] = "";
	mContents[theidx] << "<" << mSeqno++ << ":" << file << ":" << line << ">";
	mLock->unlock();
	return mContents[theidx];
}

