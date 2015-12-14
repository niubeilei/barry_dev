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
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/DocReadRunner.h"

#include "API/AosApiC.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"


AosDocReadRunner::AosDocReadRunner(const int &num_thrds)
:
mBitmaps(0),
mCacher(0),
mRundata(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mOmnSem(OmnNew OmnSem(0));
mNumThreads(num_thrds),
mNumReqs(0)
{
	if (mNumThreads >= eMaxThreads || mNumThreads < 1) mNumThreads = eDftNumThreads;
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i = 0; i< mNumThreads; i++)
	{
		mThreads[i] = OmnNew OmnThread(thisPtr, "DocReaderThrd", i, true, true, __FILE__, __LINE__);
		mThreads[i]->start();
	}
}


bool
AosDocReadRunner::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mNumReqs == 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			OmnSem->post();
			continue;
		}
		int index = mNumReqs -1;
		mNumReqs --;
		if (mNumReqs > 0)
		{
			mCondVar->signal();
		}
		mLock->unlock();
		aos_assert_rl(readData(index, mRundata), mLock, false);
		return true;
	}
	return true;
}

bool
AosDocReadRunner::signal(const int threadLogicId)
{
	return true;
}

bool    
AosDocReadRunner::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosDocReadRunner::proc(
		const vector<AosBitmapPtr> &bitmaps,
		const AosDataCacherObjPtr &cacher,
		const int &num,
		const vector<bool> &finished,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mBitmaps = bitmaps;
	mCacher = cacher;
	mRundata = rdata;
	mFinished = finished;
	aos_assert_rl(!mNumReqs, mLock, false);
	mNumReqs = num;

	mCondVar->signal();
	mLock->unlock();

	mOmnSem->wait();
	return true;
}


//bool
//AosDocReadRunner::read(const int &index)
//{
//	AosXmlTagPtr docs;
//	aos_assert_r(readData(mBitmaps[index], docs, mRundata), false);
//	AosXmlTagPtr doc;
//	for (u32 i = 0; i< docs.size(); i++)
//	{
//		aos_assert_r(docs[i], false);
//		doc = docs[i]->getFirstChild();
//		while (doc)
//		{
//			AosValueRslt valueRslt;
//			valueRslt.setValue(doc->toString());
//			valueRslt.setDocid(doc->getAttrU64(AOSTAG_DOCID, 0));
//			//mCacher->appendEntry(valueRslt, mRundata);
//			doc = docs[i]->getNextChild();
//			//mSem->post();
//		}
//	}
//	return true;
//}


bool
AosDocReadRunner::readData(
		const int index,
		const AosRundataPtr &rdata)
{
	AosBitmapPtr bitmap = mBitmaps[index];
	AosBitmapPtr result_bitmap = AosBitmapMgrObj::getBitmapStatic();
	aos_assert_r(result_bitmap, 0);
	AosBuffPtr dd;
	while (!bitmap->isEmpty())
	{
		if (!result_bitmap->isEmpty())
		{
			aos_assert_r(diffBitmap(bitmap, result_bitmap, rdata), false);
		}

		result_bitmap->clear();
		if (!bitmap->isEmpty())
		{
			if ((dd = getDocs(bitmap, result_bitmap, rdata))) 
			{
				//mSem->Wait();
				//docs.push_back(dd);
				mCacher->setDataBlock(dd);
			}
		}
	}
	mFinished[index] = true;
	return true;
}


AosBuffPtr
AosDocReadRunner::getDocs(
		const AosBitmapPtr &bitmap,
		AosBitmapPtr &result_bitmap,
		const AosRundataPtr &rdata)
{
	AosBuffPtr docs = AosDocClientObj::getDocClient()->getDocsByDocid(bitmap, result_bitmap, rdata);
	return docs;
}


bool
AosDocReadRunner::diffBitmap(
		const AosBitmapPtr &a,
		const AosBitmapPtr &b,
		const AosRundataPtr &rdata)
{
	u64 docid;
	while((docid = b->nextDocid()))
	{
		if (a->checkDoc(docid))
		{
			a->removeDocid(docid);
		}
	}
	return true;
}

#endif 
