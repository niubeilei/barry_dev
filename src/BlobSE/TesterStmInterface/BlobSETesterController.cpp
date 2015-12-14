////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2015-11-20 Created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/TesterStmInterface/BlobSETesterController.h"
#include "Util/OmnNew.h"
#include "alarm_c/alarm.h"
#include "BlobSE/BlobSEAPI.h"
#include "Random/RandomUtil.h"
#include "BlobSE/TesterStmInterface/BlobSETesterThrd.h"

AosBlobSETesterController::AosBlobSETesterController()
:
mLock(OmnNew OmnMutex()),
mOprIDLastAppended(0),
mOprIDLastApplied(0)
{
	AosXmlTagPtr pConf = OmnApp::getAppConfig();
	aos_assert(pConf.notNull());
	AosXmlTagPtr blobse_conf = OmnApp::getAppConfig()->getFirstChild("BlobSE");
	mBlobSE = new AosBlobSEAPI(mRundata, 0, 10, blobse_conf->getAttrU32("header_custom_data_size", 0));
	aos_assert(mBlobSE.notNull());
	pConf = OmnApp::getAppConfig()->getFirstChild("BlobSETester");
	aos_assert(pConf.notNull());
	mRundata = OmnApp::getRundata();
	aos_assert(mRundata.notNull());

	mNumFinished 	= 0;
	mDelTestCount 	= 0;
	mDelFailedCount = 0;
	mReadTestCount  = 0;
	mReadFailedCount 	= 0;
	mCreateTestCount 	= 0;
	mCreateFailedCount	= 0;
	mModifyTestCount 	= 0;
	mModifyFailedCount  = 0;

}


AosBlobSETesterController::~AosBlobSETesterController()
{
	OmnDelete mLock;
}


bool
AosBlobSETesterController::start()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig()->getFirstChild("BlobSETester");
	aos_assert_r(conf.notNull(),false);
	mNumOfThreads = conf->getAttrU32("number_of_threads",0);
	aos_assert_r(mNumOfThreads > 0, false);
	for (u32 i = 1; i <= mNumOfThreads; i++)
	{
		OmnThreadedObjPtr thread(OmnNew AosBlobSETesterThrd(i, mBlobSE, this), false);

		OmnThreadPtr p = OmnNew OmnThread(thread, "AosBlobSETesterThrd", i, false, false, __FILE__, __LINE__);
		p->start();
		mThreads.push_back(p);
	}
	while (1)
	{
		if (mNumFinished < mNumOfThreads)
		{
			sleep(1);
		}
		else
		{
			break;
		}
	}

	OmnScreen << "\nTest Result:\nRead Test Count:" << mReadTestCount << " Failed Count:" << mReadFailedCount << "\n"
			<< "Create Test Count:" << mCreateTestCount << " Failed Count:" << mCreateFailedCount << "\n"
			<< "Modify Test Count:" << mModifyTestCount << " Failed Count:" << mModifyFailedCount << "\n"
			<< "Delete Test Count:" << mDelTestCount << " Failed Count:" << mDelFailedCount << "\n"
			<< "Total Test Count:" << mReadTestCount + mCreateTestCount + mModifyTestCount + mDelTestCount
			<< " Total Failed Count:" << mReadFailedCount + mCreateFailedCount + mModifyFailedCount + mDelFailedCount << endl;
	return true;
}


bool
AosBlobSETesterController::appendEntry(
		const AosBuffPtr	&pBuff,
		u64					&ullOprID)
{
	mLock->lock();
	mOprIDLastAppended++;
	bool rslt = mBlobSE->appendEntry(mRundata, 0, mOprIDLastAppended, pBuff);
	ullOprID = mOprIDLastAppended;
	mLock->unlock();
	return rslt;
}


bool
AosBlobSETesterController::applyChanges()
{
	mLock->lock();
	if (mOprIDLastApplied == mOprIDLastAppended)
	{
		mLock->unlock();
		OmnScreen << "nothing to apply" << endl;
		return true;
	}
	aos_assert_rl(mOprIDLastAppended > mOprIDLastApplied, mLock, false);
	u32 ulCount = OmnRandom::nextU32(1, (u32)(mOprIDLastAppended - mOprIDLastApplied));	//they should not differ too much
	bool rslt = false;
	for (u32 i = 0; i < ulCount; i++)
	{
		rslt = mBlobSE->apply(mRundata, ++mOprIDLastApplied);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return rslt;
}


u64
AosBlobSETesterController::getOprIdLastApplied() const
{
	mLock->lock();
	u64 ullOprID = mOprIDLastApplied;
	mLock->unlock();
	return ullOprID;
}
