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
// This class simulates a local cache file. Initially, all data are kept 
// in memory. When the size of the in-memory cacher reaches mSamplingSize,
// it splits the data into 'mNumBuckets', which is the maximum number of
// buckets this cache may create. 
//
// After buckets being created, when new data are appended (by calling 
// appendData(...)), it will split the received data based on the distribution
// map and saves the contents into the corresponding buckets. This is done
// by a thread shell. 
//
// Note that processing received data is thread-safe, which means that when
// a new call to appendData(...) is made bfore the current one finishes, 
// it is safe to process the newly received data. 
//
// There may be a background thread that is responsible for rebalancing the
// distribution map. This thread wakes up when a bucket becomes 'mBucketTooBig'. 
// When this thread wakes up, it checks whether it needs to rebalance. 
//
// 'mRebalancePolicy' determines whether and how to rebalance. Allowed values
// are:
// 	1. NoRelancing
//  2. MultiLevel
//	   This means that when a bucket becomes 'mBucketTooBig', the bucket
//	   splits its contents into a second level LocalCacheFile. This is done
//	   by renaming the current bucket file name and creating a new empty
//	   bucket file for the overflown bucket. Splitting and creating the 
//	   second level cacher is done in a separate thread. 
//	   A second leveled bucket works exactly the same as normal buckets. 
//	   When new contents are received, it dumps the data into its bucket 
//	   file until it is 'mBucketTooBig' again. When that happens, it renames
//	   the current bucket file to another one and empties its. It then 
//	   kicks off another thread shell to split the contents into its 
//	   second-level buckets. 
//
//	   Note that this rebalancing algorith does not change the distribution 
//	   map, nor does it merge small buckets. This makes the algorithm very
//	   simple.
//
// Modification History:
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSource/DataSourceLocalCacheFile.h"

#include "ThreadShellRunner/AosDistrBlobToBucketsRunner.h"


AosDsLocalCacheFile::AosDsLocalCacheFile(const bool regflag)
:
AosDataStore(AOSDATASOURCE_LOCAL_CACHE_FILE, AosDataSourceType::eLocalCacheFile, regflag)
{
	initMemberData();
}


AosDsLocalCacheFile::AosDsLocalCacheFile(const AosDsLocalCacheFile &rhs)
:
AosDataStore(rhs)
{
	copyMemberData(rhs);
}


AosDsLocalCacheFile::AosDsLocalCacheFile(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosDataStore(AOSDATASOURCE_LOCAL_CACHE_FILE, AosDataSourceType::eLocalCacheFile, false)
{
	initMemberData();
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDsLocalCacheFile::~AosDsLocalCacheFile()
{
}


bool 
AosDsLocalCacheFile::copyMemberData(const AosDsLocalCacheFile &rhs)
{
	if (rhs.mDataCacher)
	{
		mDataCacher = rhs.mDataCacher->clone();
	}
	return true;
}


bool 
AosDsLocalCacheFile::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_rr(conf, rdata, false);
	
	// Create the cacher
	AosXmlTagPtr tag = conf->getFirstChild(AOSTAG_DATA_CACHER);
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_data_cacher") << ": " << conf->toString() << enderr;
		return false;
	}
	mCacher = AosDataCacherObj::createDataCacher(tag, rdata);
	aos_assert_r(!mDataCacher, false);

	// This class assumes that mDataCacher always use a secondary data blob when
	// the cacher is full.
	mDataCacher->alwaysUseSecondaryBlob();

	return true;
}


bool 	
AosDsLocalCacheFile::create(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDsLocalCacheFile::destroy(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDsLocalCacheFile::open(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDsLocalCacheFile::close(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 	
AosDsLocalCacheFile::appendValue(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataCacher, rdata, false);
	mLock->lock();
	bool rslt = mDataCacher->appendValue(value, rdata);
	mLock->unlock();
	return rslt;
}


bool 	
AosDsLocalCacheFile::appendRecord(const AosDataRecord &record, const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataCacher, rdata, false);
	mLock->lock();
	bool rslt = mDataCacher->appendRecord(value, rdata);
	mLock->unlock();
	return rslt;
}


bool 	
AosDsLocalCacheFile::appendData(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataCacher, rdata, false);
	mLock->lock();
	if (!mBucketsCreated)
	{
		return mDataCacher->appendData(buff, rdata);
		mLock->unlock();
	}

	// The buckets have already created. It needs to split the contents and 
	// distribute them into the buckets. 
	AosDataBlobObjPtr blob = mCacher->toDataBlob();
	aos_assert_rr(blob, rdata, false);
	return distributeData(blob, rdata);
}


char *	
AosDsLocalCacheFile::nextValue(int &len)
{
	aos_assert_rr(mDataCacher, rdata, false);
	mLock->lock();
	char * data = mDataCacher->nextValue(len);
	mmLock->unlock();
	return data;
}


bool	
AosDsLocalCacheFile::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataCacher, rdata, false);
	mLock->lock();
	bool rslt = mDataCacher->nextValue(value);
	mLock->unlock();
	return rslt;
}


bool	
AosDsLocalCacheFile::nextRecord(const AosDataRecordPtr &rec, const AosRundataPtr &rdata)
{
	aos_assert_rr(mDataCacher, rdata, false);
	mLock->lock();
	bool rslt = mDataCacher->nextRecord(record);
	mLock->unlock();
	return rslt;
}


AosDsLocalCacheFilePtr 
AosDsLocalCacheFile::clone()
{
	try
	{
		return OmnNew AosDsLocalCacheFile(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDsLocalCacheFilePtr 
AosDsLocalCacheFile::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDsLocalCacheFile(def, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


bool
AosDsLocalCacheFile::cacherFull(
		const AosDataBlobObjPtr &data_blob, 
		const AosRundataPtr &rdata)
{
	// The data cacher is full. This is normally caused by adding too many
	// contents to the data cacher. 'data_blob' contains all the contents. 
	//
	// When data cacher is full, this class will save the contents into local
	// cache files. A local cache file consists of a number of sub-files, each
	// serves as a bucket. 
	//
	// If no buckets have been created yet, it accumulates the data until to 
	// 'mMinSamplingSize' size. When this size is reached, it sorts the 
	// data and then evenly distribute the data into 'mNumBuckets'. 
	//
	// After that, whenever this function is called again, if the blob is
	// not sorted yet, it sorts the blob, and then distributes the contents
	// based on the distribution map.
	//
	// Note that it is possible that as more and more data are collected, the
	// distribution may not be well balanced. When the balance is not good
	// enough, it may re-balance the distribution map.
	//
	// IMPORTANT: when this function is called, it assumes the class was locked.
	if (mFailedCreatingBuckets)
	{
		// Something serious wrong. It failed creating the buckets. 
		AosSetErrorU(rdata, "failed_creating_buckets") << enderr;
		return false;
	}

	mDataCollected += data_blob->size();
	if (!mBucketsCreated)
	{
		if (mAccumulatedDataSize > 0)
		{
			// It has accumulated some data. 
			mAccumulatedDataSize += data_blob->size();
			mAccumulatedBlobs.push_back(data_blob);
		}

		if (mAccumulatedDataSize < mSamplingSize) return true;

		// It has collected enough data. It is the time to create the buckets. 
		bool rslt = createBuckets(rdata);
		mAccumulatedBlobs.clear();
		aos_assert_r(rslt, false);

		// Buckets were successfully created. 
		mBucketsCreated = true;
		return true;
	}

	// Buckets were created. Need to distribute the contents into these buckets. 
	// In the current implementations, it always does this in a thread shell.
	return distributeData(data_blob, rdata);
}

		
bool
AosDsLocalCacheFile::distributeData(
		const AosDataBlobObjPtr &data_blob, 
		const AosRundataPtr &rdata)
{
	// This function creates a runner and asks the thread shell to run it.
	AosDataSourceObjPtr thisptr(this, false);
	return AosDistrBlobToBucketsRunner::run(data_blob, thisptr, rdata);
}

