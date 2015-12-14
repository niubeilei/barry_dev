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
// This data blob assumes records are in the following format:
// 		[key, value]
// where both key and value are stored in a fixed length memory record.
// Its value is normally arranged at the end of the memory record. The key
// part is null terminated by this class. 
//
// Modification History:
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataBlob/BlobRecord.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataBlob/Ptrs.h"
#include "DataRecord/DataRecord.h"
#include "DataBlob/DataBlobFullHandler.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/BuffArray.h"
#include "Util/Sort.h"
#include "Util/CompareFun.h"
#include "Util1/MemMgr.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/Ptrs.h"

#include <stdlib.h>

static OmnMutex sgLock;

AosBlobRecord::AosBlobRecord()
:
AosDataBlob(AOSDATABLOB_RECORD, AosDataBlobType::eRecord)
{
}


AosBlobRecord::AosBlobRecord(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
:
AosDataBlob(AOSDATABLOB_RECORD, AosDataBlobType::eRecord)
{
	mDataRecord = record;
	mRecordLen = record->getRecordLen();
	mData = OmnNew AosBuffArray(mRecordLen);
}


AosBlobRecord::AosBlobRecord(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataBlob(AOSDATABLOB_RECORD, AosDataBlobType::eRecord)
{
	init();
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosBlobRecord::AosBlobRecord(const AosBlobRecord &rhs)
:
AosDataBlob(rhs),
mData(rhs.mData->clone()),
mRecordLen(rhs.mRecordLen),
mRewriteLoopIdx(rhs.mRewriteLoopIdx),
mBuffArrayTag(rhs.mBuffArrayTag)
{
}


AosBlobRecord::~AosBlobRecord()
{
}


bool
AosBlobRecord::init()
{
	mRecordLen = 0;
	return true;
}


bool
AosBlobRecord::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_rr(conf, rdata, false);

	AosDataBlob::config(conf, rdata);

	try
	{
		mBuffArrayTag = conf->getFirstChild(AOSTAG_BUFFARRAY);
		if (!mBuffArrayTag)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return false;
		}
		mData = OmnNew AosBuffArray(mBuffArrayTag, rdata.getPtrNoLock());
		if (!mData)
		{
			AosSetErrorU(rdata, "failed to get buff array") << enderr;
			return false;
		}
	
		mRecordLen = mData->getRecordLen();
		mIsStable = mData->isStable();
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return false;
	}
	return true;
}


void
AosBlobRecord::resetRecordLoop()
{
	aos_assert(mData);
	mData->resetRecordLoop();
}


void
AosBlobRecord::resetRangeLoop()
{
	aos_assert(mData);
	mData->resetRangeLoop();
}


bool
AosBlobRecord::sort()
{
	aos_assert_r(mData, false);
	mIsSorted = true;
	return mData->sort();
}


bool
AosBlobRecord::clearData()
{
	aos_assert_r(mData, false);
	return mData->clear();
}



bool    
AosBlobRecord::setData(const char *record, const u64 &len)
{
	aos_assert_r(record, false);
	aos_assert_r(len>0, false);
	AosBuffPtr buff = OmnNew AosBuff(len AosMemoryCheckerArgs);
	aos_assert_r(buff, false);
	memcpy(buff->data(), record, len);
	buff->setDataLen(len);
	aos_assert_r(mData->setBuff(buff), false);
	return true;
}


AosDataBlobObjPtr
AosBlobRecord::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosBlobRecord(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}

	return 0;
}


AosDataBlobObjPtr
AosBlobRecord::clone()
{
	try
	{
		return OmnNew AosBlobRecord(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
	return 0;
}


bool 
AosBlobRecord::resetRewriteLoop()
{
	mRewriteLoopIdx = 0;
	return true;
}


bool    
AosBlobRecord::setRewriteLoopIdx(const u64 idx)
{
	aos_assert_r(idx <= (u64)size(), false);
	mRewriteLoopIdx = idx;
	return true;
}


bool 
AosBlobRecord::rewriteNextRecord(
		const AosDataRecordObjPtr &record, 
		const bool append_if_overflow,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	int record_len;
	char *record_data;
	bool rslt = mData->getEntry(mRewriteLoopIdx, record_data, record_len);
	aos_assert_rl(rslt, mLock, false);

	if (!record_data)
	{
		// This means there are no more records in 'mData'. It needs
		// to append the value.
		if (!append_if_overflow)
		{
			mLock->unlock();
			return false;
		}
		aos_assert_rl(mData->appendEntry(record, rdata.getPtrNoLock()), mLock, false);
		mLock->unlock();
		return true;
	}

	//aos_assert_rl(mData->copyRecordTo(mRewriteLoopIdx, record, rdata.getPtrNoLock()), mLock, false);
	OmnAlarm << enderr;
	mRewriteLoopIdx++;
	mLock->unlock();
	return true;
}


bool 
AosBlobRecord::rewriteRecordByIdx(
		const u64 idx,
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	//return mData->copyRecordTo(idx, record, rdata.getPtrNoLock());
	OmnAlarm << enderr;
}


int 
AosBlobRecord::getRecordLen() const
{
	return mRecordLen;
}


u64     
AosBlobRecord::getRewriteLoopIdx() const
{
	return mRewriteLoopIdx;
}


bool
AosBlobRecord::getRecord(const int64_t &idx, const AosDataRecordObjPtr &record) const
{
	aos_assert_r(mData, false);

	mLock->lock();
	int data_len = 0;
	char *data;
	bool rslt = mData->getEntry(idx, data, data_len);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	// jimodb-1301
	int status;
	return record->setData(data, data_len, 0, status);
}


const char *
AosBlobRecord::getMemory(
		const int64_t &start_idx,
		const int64_t &end_idx,
		int64_t &length,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(mData, 0);
	
	if (start_idx > end_idx) return 0;
	if (start_idx >= size()) return 0;
	mLock->lock();
	int rcdlen;
	char *record;
	bool rslt = mData->getEntry(start_idx, record, rcdlen);
	aos_assert_rl(rslt, mLock, 0);
	length = rcdlen * (end_idx - start_idx);
	mLock->unlock();
	return record;
}


int
AosBlobRecord::getRecordLen(const int idx) const
{
	int len;
	char* data;
	bool rslt = mData->getEntry(idx, data, len);
	aos_assert_r(rslt, -1);
	return len;
}


bool 
AosBlobRecord::start(const AosRundataPtr &rdata)
{
	// This function runs all the 'start actions'. It is called when
	// the data blob is newed. These actions can be run using 
	// the current thread or through thread shells. If it is through
	// thread shells, it may be run in one thread or multiple 
	// threads. 

	// 1. If there are no full actions, do nothing.
	if (mStartActions.size() <= 0) return true;

	mLock->lock();
	vector<AosActionObjPtr> start_actions = mStartActions;
	AosDataBlobObjPtr thisPtr(this, false);
	bool rslt = runStartActionsInCurrentThread(thisPtr, start_actions, rdata);
	mLock->unlock();
	return rslt;
}


bool 
AosBlobRecord::finish(const AosRundataPtr &rdata)
{
	// This function runs all the 'finish actions'. It is called when
	// finish data blob. These actions can be run using 
	// the current thread or through thread shells. If it is through
	// thread shells, it may be run in one thread or multiple 
	// threads. 

	// 1. If there are no full actions, do nothing.
	if (mFinishActionsBeforeSort.size() <= 0 && mFinishActionsAfterSort.size() <= 0) return true;

	mLock->lock();
	vector<AosActionObjPtr> actions_before_sort = mFinishActionsBeforeSort;
	vector<AosActionObjPtr> actions_after_sort = mFinishActionsAfterSort;

	// This means to run all the full actions against the current
	// data: 'mData'.
	if (mRunInThrdShell)
	{
		// This means to run actions through thread shell.
		bool rslt = runActionsInMultiThreads(
				mData, actions_before_sort, actions_after_sort, rdata);
		mLock->unlock();
		return rslt;
	}

	bool rslt = runActionsInCurrentThread(
			mData, actions_before_sort, actions_after_sort, rdata);
	mLock->unlock();
	return rslt;
}


bool 
AosBlobRecord::runFullActions(const AosRundataPtr &rdata)
{
	// This function runs all the 'full actions'. It is called when
	// the data blob is 'full'. These actions can be run using 
	// the current thread or through thread shells. If it is through
	// thread shells, it may be run in one thread or multiple 
	// threads.
	// 1. If there are no full actions, do nothing.
	if (mFullActionsBeforeSort.size() <= 0 && mFullActionsAfterSort.size() <= 0) return true;

	mLock->lock();
	vector<AosActionObjPtr> actions_before_sort = mFullActionsBeforeSort;
	vector<AosActionObjPtr> actions_after_sort = mFullActionsAfterSort;
	AosBuffArrayPtr second_data = mData;
	mData = OmnNew AosBuffArray(mBuffArrayTag, rdata.getPtrNoLock());
	if (mRunFullActionsInBackground)
	{
		// This means the full actions are run in background. 
		// That is, it moves the data to a temporary data and
		// creates a new 'mData'. 
		mLock->unlock();
		return runActionsInBackground(second_data, 
				actions_before_sort, actions_after_sort, rdata);
	}

	// This means to run all the full actions against the current
	// data: 'mData'.
	if (mRunInThrdShell)
	{
		// This means to run actions through thread shell.
		bool rslt = runActionsInMultiThreads(
				second_data, actions_before_sort, actions_after_sort, rdata);
		mLock->unlock();
		return rslt;
	}

	bool rslt = runActionsInCurrentThread(
			second_data, actions_before_sort, actions_after_sort, rdata);
	mLock->unlock();
	return rslt;
}



int64_t
AosBlobRecord::size() const
{
	aos_assert_r(mData, -1);
	return mData->getNumEntries();
}


bool
AosBlobRecord::nextRecord(const AosDataRecordObjPtr &record)
{
	aos_assert_r(mData, false);
	aos_assert_r(record, false);
	return mData->nextValue(record);
}


bool 
AosBlobRecord::nextValue(
		AosValueRslt &value, 
		const bool copy_flag, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mData, false);
	//return mData->nextValue(value, copy_flag);
	OmnAlarm << enderr;
}


bool
AosBlobRecord::nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata)
{
	// JACKIE-HADOOP
	int64_t offset = mMetaData->getOffset();

	aos_assert_rr(mData, rdata, 0);
	AosValueRslt value;
	//if (!mData->nextValue(value, false)) return 0;
	OmnAlarm << enderr;
	OmnString s = value.getStr();
	*data = (char*)s.data();
	len = s.length();
	// JACKIE-HADOOP
	mMetaData->setOffset(offset + len);	

	metaData = mMetaData;
	return true;
}


bool
AosBlobRecord::firstRecordInRange(
		const int64_t &rcd_idx, 
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mData, rdata, false);
	aos_assert_rr(record, rdata, false);
	return mData->firstRecordInRange(rcd_idx, record);
}


bool
AosBlobRecord::nextRecordInRange(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mData, false);
	aos_assert_r(record, false);
	return mData->nextRecordInRange(record);
}


bool
AosBlobRecord::appendEntry(
		const AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	/*
	// This function appends [key, value] to the data. 
	aos_assert_rr(mData, rdata, false);

	// The first, do actions for the entry.
	mLock->lock();
	for (u32 i=0; i<mAppendActions.size(); i++)
	{
		// Since mAppendActions may be used to filter data, we need
		// to check whether it should continue the processing.
		rdata->setContinueFlag(true);
		if (!mAppendActions[i]->run(value, rdata)) 
		{
			// The entry is invalid. 
			if (mInvalidActions.size() > 0)
			{
				for (u32 k=0; k<mInvalidActions.size(); k++)
				{
					mInvalidActions[k]->run(value, rdata);
				}
			}
		}

		if (!rdata->continueProc())
		{
			// It should stop the processing, or in other word, 
			// the action functions as a filter.
			rdata->setContinueFlag(true);
			mLock->unlock();
			return true;
		}
	}

	aos_assert_rl(mData->addValue(value, rdata), mLock, false);
	mLock->unlock();

	if (checkFull(rdata)) return handleFull(rdata);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosBlobRecord::appendRecord(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	/*
	// This function appends [key, value] to the data. 
	aos_assert_rr(mData, rdata, false);

	// The first, do actions for the entry.
	mLock->lock();
	for (u32 i=0; i<mAppendActions.size(); i++)
	{
		// Since mAppendActions may be used to filter data, we need
		// to check whether it should continue the processing.
		rdata->setContinueFlag(true);
		if (!mAppendActions[i]->run(record, rdata)) 
		{
			// The entry is invalid. 
			if (mInvalidActions.size() > 0)
			{
				for (u32 k=0; k<mInvalidActions.size(); k++)
				{
					mInvalidActions[k]->run(record, rdata);
				}
			}
		}

		if (!rdata->continueProc())
		{
			// It should stop the processing, or in other word, 
			// the action functions as a filter.
			rdata->setContinueFlag(true);
			mLock->unlock();
			return true;
		}
	}

	aos_assert_rl(mData->addValue(record, rdata), mLock, false);
	mLock->unlock();

	if (checkFull(rdata)) return handleFull(rdata);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


u64
AosBlobRecord::getMemSize() const
{
	return mData->dataLength();
}


bool
AosBlobRecord::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// This function put the information of datablob into buff.
	// The format:
	// 		mIsSorted									(u32) 0|1
	// 		mRewriteLoopIdx								(u64)
	// 		mConfigLength								(int)
	// 		mConfig										(variable)
	// 		mDataLen									(u64)
	// 		mData										(variable)
	aos_assert_rr(buff, rdata, false);
	buff->setU32(mIsSorted ? 1 : 0);
	buff->setU64(mRewriteLoopIdx);
	buff->setInt(mConfig->getDataLength());
	buff->setOmnStr(mConfig->toString());
	buff->setU64(mData->dataLength());
	buff->setBuff(mData->getBuff());
	return true;
}
	

bool 
AosBlobRecord::serializeFrom(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata)
{
	aos_assert_r(buff, 0);

	// The format is: 
	// mIsSorted                                   (u32) 0|1
	// mRewriteLoopIdx                             (u64)
	// mConfigLength                               (int)
	// mConfig                                     (variable)
	// mDataLen                                    (u64)
	// mData                                       (variable)
	bool isSorted = buff->getU32(0) == 1 ? true : false;
	u64 rwLpIdx = buff->getU64(0);
	
	int length = buff->getInt(0);
	aos_assert_r(length > 0, 0);

	OmnString confstr = buff->getOmnStr("");
	aos_assert_r(confstr != "", 0);
	
	u64 datalen = buff->getU64(0);
	char * contents = &((buff->data())[buff->getCrtIdx()]);
	aos_assert_r(contents, 0);

	AosXmlTagPtr conf = AosXmlParser::parse(confstr AosMemoryCheckerArgs);
	aos_assert_r(conf, 0);
	aos_assert_r(config(conf, rdata), false);

	mIsSorted = isSorted;
	mRewriteLoopIdx = rwLpIdx;
	aos_assert_r(setData(contents, datalen), false);
	
	return true;
}


bool
AosBlobRecord::firstValueInRange(
		const int64_t &rcd_idx, 
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


const char *
AosBlobRecord::getMemory(
		const u64 &start_idx,
		const u64 &end_idx,
		int64_t &mem_size,
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}
	

bool
AosBlobRecord::hasMoreData() const
{
	if (!mData)
	{
		OmnAlarm << "No BuffArray" << enderr;
		return false;
	}
	return mData->hasMoreData();
}


bool 
AosBlobRecord::setDataBlock(
		const AosBuffDataPtr &info,
		const AosRundataPtr &rdata)
{
	aos_assert_r(info, false);
	AosBuffPtr buff = info->getBuff();
	aos_assert_r(buff, false);
	aos_assert_r(mData->setBuff(buff), false);
	mMetaData = info;
	return true;
}
	

bool
AosBlobRecord::nextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
	

int64_t
AosBlobRecord::findFirstEntry(
		const int64_t &start_pos,
		const OmnString &key,
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return -1;
}
	

bool
AosBlobRecord::nextValueInRange(
		AosValueRslt &value, 
		const bool need_copy,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
