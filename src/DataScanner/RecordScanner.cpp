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
#include "DataScanner/RecordScanner.h"

#include "API/AosApi.h"
#include "DataScanner/Ptrs.h"
#include "DbQuery/Query.h"
#include "SEUtil/IILName.h"
#include "Util/ValueRslt.h"
#include "ValueSel/ValueSel.h"
#include "Thread/Sem.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/TaskDataObj.h"


AosRecordScanner::AosRecordScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_RECORD, AosDataScannerType::eRecord, flag),
mLock(OmnNew OmnMutex()),
mIsFinished(false),
mBuff(0),
mRcdLen(0),
mStatus(-1),
mPos(0)
{
}


AosRecordScanner::AosRecordScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_RECORD, AosDataScannerType::eRecord, false),
mLock(OmnNew OmnMutex()),
mIsFinished(false),
mBuff(0),
mRcdLen(0),
mStatus(-1),
mPos(0)
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


AosRecordScanner::~AosRecordScanner()
{
}


bool
AosRecordScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return true;
}

bool 
AosRecordScanner::getNextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	aos_assert_r(mScanner, false);

	if (!record)
	{
		record = AosDataRecordObj::createDataRecordStatic(
				mRecordConf, 0, rdata AosMemoryCheckerArgs);	
		aos_assert_r(record, false);
	}

	mLock->lock();
	if (mIsFinished)
	{
		mLock->unlock();
		record = 0;
		return true;
	}

	if (!mBuff)
	{
		bool rslt = mScanner->getNextBlock(mBuff, rdata);
		mPos = 0;
		if (!rslt || !mBuff)
		{
			mIsFinished = true;
			record = 0;
			mLock->unlock();
			return true;
		}
OmnScreen << "  11111111111  " << endl;
	}

	int bufflen = mBuff->dataLen() - mPos;
	
	bool isnotgood = record->setData(mBuff, bufflen, rdata);
	mPos += record->getDataLen();
	while (!isnotgood)
	{
		if (mPos != 0)
		{
			AosBuffPtr buff = mBuff->copyContents(mPos, mBuff->dataLen()-mPos);
			mBuff = 0;
			mPos = 0;
			mBuff = buff;
OmnScreen << "  2222222222  " << endl;
		}

		AosBuffPtr buff = 0;
		bool rslt = mScanner->getNextBlock(buff, rdata);
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(buff, mLock, false);
		aos_assert_rl(buff->dataLen() > 0, mLock, false);
		mBuff->appendBuff(buff);
		mPos += record->getDataLen();
		bufflen = mBuff->dataLen() - mPos;
	
		rslt = record->setData(mBuff, bufflen, rdata);
		aos_assert_rl(rslt, mLock, false);
OmnScreen << "  3333333333  " << endl;
	}

	if (mBuff->dataLen() - mPos == 0)
	{
		mBuff = 0;
		mPos = 0;
OmnScreen << "  4444444444  " << endl;
	}

	mLock->unlock();
	return true;
}

bool                                  
AosRecordScanner::initRecordScanner(      
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)   
{
	AosXmlTagPtr task_data_tag = conf->getFirstChild(AOSTAG_TASKDATA_TAGNAME);
	aos_assert_r(task_data_tag, false);

	AosTaskDataObjPtr task_data = AosTaskDataObj::createTaskDataStatic(task_data_tag, rdata);
	aos_assert_r(task_data, false);

	mScanner = task_data->createDataScanner(rdata);
	aos_assert_r(mScanner, false);

	mRecordConf = conf->getFirstChild("record");
	aos_assert_r(mRecordConf, false);

	return true;	
}

bool
AosRecordScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosRecordScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosDataScannerObjPtr 
AosRecordScanner::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}
#endif
