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
// This cacher will cache the data in multiple local files. When finishing,
// it may do some actions on all the data. Or in other word, this cacher
// can cache data much bigger than the memory.
//
// Modification History:
// 07/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCacher/DataCacherMulti.h"

#include "Actions/SdocAction.h"
#include "DataBlob/DataBlob.h"
#include "Rundata/Rundata.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "Thread/Mutex.h"


AosDataCacherMulti::AosDataCacherMulti(const bool flag)
:
AosDataCacher(AOSDATACACHER_MULTI, AosDataCacherType::eMulti, flag),
mLock(OmnNew OmnMutex())
{
}


AosDataCacherMulti::AosDataCacherMulti(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataCacher(AOSDATACACHER_MULTI, AosDataCacherType::eMulti, false), 
mLock(OmnNew OmnMutex())
{
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataCacherMulti::~AosDataCacherMulti()
{
}


bool 
AosDataCacherMulti::config(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(conf, rdata, false);

	AosXmlTagPtr  blobconf = conf->getFirstChild("zky_blob");
	aos_assert_rr(blobconf, rdata, false);
	mBlob = AosDataBlob::createDataBlob(blobconf, rdata);
	return true;
}


bool 	
AosDataCacherMulti::appendEntry(
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mBlob, rdata, false);

	bool rslt = mBlob->appendEntry(value, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 	
AosDataCacherMulti::appendRecord(
		const AosDataRecord &record,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(record, rdata, false);

	// 1. If there are filters for records, run the filter.
	mLock->lock();
	if (mRecordFilter)
	{
		if (mRecordFilter->filterData(record, rdata))
		{
			// Record is filtered.
			mNumFiltered++;
			if (mFilteredTrashCan)
			{
				mFilteredTrashCan->appendRecord(record, rdata);
				mLock->unlock();
				return true;
			}
		}
	}

	AosValueRslt value;
	if (!mCacherPicker->run(value, record->data(), record->length(), rdata))
	{
		// Should Never Happen
		AosSetErrorU(rdata, "internal_error") << enderr;
		if (mErrorTrashCan)
		{
			mErrorTrashCan->appendRecord(record, rdata);
		}

		mLock->unlock();
		return false;
	}

	CacherItr_t itr = mCacherMap.find(value.getStrValueRef());
	if (itr != mCacherMap.end())
	{
		// Found the cacher.
		if (!itr->second->append(record, rdata))
		{
			if (mNumErrors++ >= mMaxIgnoredErrors)
			{
				mLock->unlock();
				return false;
			}
		}
		mLock->unlock();
		return true;
	}

	// Did not find the member cacher. This means the record is unrecognized.
	if (mUnrecogCacher)
	{
		// There is a data cacher that holds all the unrecognized records.
		mUnrecogCacher->appendRecord(record, rdata);
		mLock->unlock();
		return true;
	}
	
	// No data cacher for unrecognized records. Check whether there
	// is a trashcan for it.
	if (mUnrecogTrashCan)
	{
		// There is a trashcan for all unrecognized records.
		mUnrecogTrashCan->appendRecord(record, rdata);
		mLock->unlock();
		return true;
	}

	if (!mIgnoreUnrecognized)
	{
		mNumErrors++;
		if (mNumErrors >= mMaxIgnoredErrors)
		{
			mLock->unlock();
			return false;
		}
	}

	mLock->unlock();
	return true;
}


AosDataCacherPtr 
AosDataCacherMulti::clone(
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataCacherMulti(config, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << rdata->getErrmsg() << enderr;
		return 0;
	}
}


char* 	
AosDataCacherMulti::nextValue(int&)
{
	OmnNotImplementedYet;
	return 0;
}


bool 	
AosDataCacherMulti::clear()
{
	OmnNotImplementedYet;
	return false;
}


AosDataCacherPtr 
AosDataCacherMulti::clone()
{
	OmnNotImplementedYet;
	return 0;
}

#endif
