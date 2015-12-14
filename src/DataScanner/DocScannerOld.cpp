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
#include "DataScanner/DocScanner.h"

#include "API/AosApi.h"
#include "DataScanner/Ptrs.h"
#include "DataScanner/DocReadRunner.h"
#include "SEInterfaces/DocClientObj.h"
#include "DbQuery/Query.h"
#include "Rundata/Rundata.h"
#include "SEUtil/IILName.h"
#include "Util/ValueRslt.h"
#include "ValueSel/ValueSel.h"
#include "Thread/Sem.h"
#include "SEInterfaces/DataRecordObj.h"


AosDocScanner::AosDocScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_DOC, AosDataScannerType::eDoc, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mThread(0),
mScannerId(""),
mNumPhysicals(AosGetNumPhysicals()),
mServerId(0),
mNoMoreData(false),
mRundata(0)
{
	if (mNumPhysicals <= 0)
	{
		OmnThrowException("No physical servers");
	}

	if (mNumPhysicals == 1)
	{
		mServerId = 0;
	}
	else
	{
		mServerId = AosGetSelfServerId();
		if (mServerId == 0)
		{
			mServerId = mNumPhysicals;
		}
	}
}


AosDocScanner::AosDocScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_DOC, AosDataScannerType::eDoc, false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mThread(0),
mScannerId(""),
mNumPhysicals(AosGetNumPhysicals()),
mServerId(0),
mNoMoreData(false),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}

	rslt = start(rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}

	if (mNumPhysicals <= 0)
	{
		OmnThrowException("No physical servers");
	}

	if (mNumPhysicals == 1)
	{
		mServerId = 0;
	}
	else
	{
		mServerId = AosGetSelfServerId();
		if (mServerId == 0)
		{
			mServerId = mNumPhysicals;
		}
	}
}


AosDocScanner::~AosDocScanner()
{
}

void
AosDocScanner::startThread(const AosRundataPtr &rdata)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "docbatchreader", 0, false, true, __FILE__, __LINE__);
	mThread->start();
}


bool
AosDocScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return true;
}


bool
AosDocScanner::start(const AosRundataPtr &rdata)
{
	// This function starts querying the docs. 
	OmnNotImplementedYet;
	return false;
}

bool
AosDocScanner::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	// This function is called when someone wants to read some more 
	// docs from this class. It sends a request to the physical server
	// identified by 'mServerId'. If there are no data, the function
	// will wait until some data are available. If there are no more
	// data, it should return immediately and 'buff' should contain
	// no data. 
	mLock->lock();
	if (!mThread) startThread(rdata);

	if (mNoMoreData)
	{
		//aos_assert_rl(!mPrimaryBuff && !mSecondaryBuff, mLock, false); 
		aos_assert_rl(!mSecondaryBuff, mLock, false); 
		mLock->unlock();
		if (mPrimaryBuff)
		{
			buff = mPrimaryBuff;
			mPrimaryBuff = 0;
		}
		else
		{
			buff = 0;
		}
		return true;
	}

	while (!mPrimaryBuff && !mNoMoreData)
	{
		mCondVar->wait(mLock);
	}

	if (mNoMoreData)
	{
		//aos_assert_rl(!mPrimaryBuff && !mSecondaryBuff, mLock, false); 
		aos_assert_rl(!mSecondaryBuff, mLock, false); 
		mLock->unlock();
		if (mPrimaryBuff)
		{
			buff = mPrimaryBuff;
			mPrimaryBuff = 0;
		}
		else
		{
			buff = 0;
		}
		return true;
	}

	buff = mPrimaryBuff;
OmnScreen << "========================getNextBlock : " << &buff << endl;
	mPrimaryBuff = 0;
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosDocScanner::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mNoMoreData) 
		{
			state = OmnThrdStatus::eStop;
			mLock->unlock();
			return true;
		}

		if (mPrimaryBuff && mSecondaryBuff)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		if (!mPrimaryBuff && mSecondaryBuff)
		{
			mPrimaryBuff = mSecondaryBuff;
			mSecondaryBuff = 0;
			mCondVar->signal();
		}

		mLock->unlock();
		aos_assert_r(mRundata, false);
		AosBuffPtr buff = AosDocClientObj::getDocClient()->batchGetDocs(mScannerId, mServerId, mRundata);
OmnScreen << "=========================batchGetDocs: " << &buff << " : " << buff->dataLen() << endl;
		mLock->lock();

		if (!buff || buff->dataLen() <= 0)
		{
			if (!mPrimaryBuff) 
			{
				mNoMoreData = true;
				state = OmnThrdStatus::eStop;
			}
			if (mPrimaryBuff && !mSecondaryBuff)
			{
				mNoMoreData = true;
				state = OmnThrdStatus::eStop;
			}
			mCondVar->signal();
			mLock->unlock();
			break;
		}

		if (!mPrimaryBuff)
		{
			mPrimaryBuff = buff;
		}
		else if(!mSecondaryBuff)
		{
			mSecondaryBuff = buff;
		}
		else
		{
			mLock->unlock();
			OmnAlarm << "Internal error" << enderr;
			return false;
		}

		mCondVar->signal();
		mLock->unlock();
	}
	return true;
}


bool
AosDocScanner::signal(const int threadLogicId)
{
	return true;
}


bool
AosDocScanner::checkThread(OmnString &err, const int thrdLogicId) const 
{
	return true;
}


bool
AosDocScanner::createDocScanner(
		vector<AosDataScannerObjPtr> &scanners,
		const OmnString &scanner_id,
		const AosRundataPtr &rdata)
{
	try
	{
		AosDocScanner* scanner = OmnNew AosDocScanner(false);
		scanner->setScannerId(scanner_id);  
		scanners.push_back(scanner);
	}
	catch(...)
	{
		OmnAlarm << "Faild to create FileScanner" << enderr;
		return false;
	}
	return true;
}


bool
AosDocScanner::split(
		vector<AosDataScannerObjPtr> &scanners,
		const int64_t &record_len,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mScannerId != "", false);
	int num = AosGetNumCpuCores();
	for (int i=0; i<num; i++)
	{
		bool rslt = createDocScanner(scanners, mScannerId, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosDocScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosDataScannerObjPtr 
AosDocScanner::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


int64_t 
AosDocScanner::getTotalFileLength()
{
	return 100;
}

#endif 
