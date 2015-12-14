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
// 1. This class maintaines the fixed numbers of buckets to cache iiltrans 
// that is a sorted  array of AosIILEntry. 
// 2. Every bucket has boundary of low and hight, the entries will append
// to the matched bucket by the range. 
// 3. The bucket can automaticly ajust range.  The mechanism of mergering 
// and spliting bucket by the aging and total entries  of the bucket. 
// If the bucket is too old. it will be merged to it's left bucket or the
// right bucket, and the bucket will free. If the bucket has full, 
// the entries will be proccesed, and the bucket will free. when the entrie 
// has not in all the buckets range .it will get the  free buckets 
// to split the range.
// 
// Modification History:
// 11/18/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "DataReceiver/DataReceiverMgr.h"

#include "DataReceiver/DataReceiver.h"
#include "SingletonClass/SingletonImpl.cpp"

#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosDataReceiverMgrSingleton,
				 AosDataReceiverMgr,
				 AosDataReceiverMgrSelf,
				 "AosDataReceiverMgr");


AosDataReceiverMgr::AosDataReceiverMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosDataReceiverMgr::~AosDataReceiverMgr()
{
}


bool
AosDataReceiverMgr::createDataReceiver(
		const OmnString &data_col_name,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mDataReceivers.find(data_col_name);
	if (itr != mDataReceivers.end()) 
	{
		mLock->unlock();
		return true;
	}
	AosDataReceiverObjPtr dataRec = AosDataReceiver::createDataReceiver(conf, rdata);
	aos_assert_r(dataRec, false);

	mDataReceivers[data_col_name] = dataRec;
	mLock->unlock();
	return true;
}


bool 
AosDataReceiverMgr::dataReceiver(
		const OmnString &collectorid, 
		const AosBuffPtr &buff)
{
	mLock->lock();
	mItr_t itr = mDataReceivers.find(collectorid);
	aos_assert_rl(itr != mDataReceivers.end(), mLock, false);
	mLock->unlock();
	itr->second->dataReceiver(buff);
	return true;
}


bool
AosDataReceiverMgr::finishDataReceiver(
		const OmnString &collectorid,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mDataReceivers.find(collectorid);
	aos_assert_rl(itr != mDataReceivers.end(), mLock, false);
	mLock->unlock();
	itr->second->finishDataReceiver(rdata);
	return true;
}


bool
AosDataReceiverMgr::start()
{
	return true;
}


bool
AosDataReceiverMgr::stop()
{
	return true;
}


bool
AosDataReceiverMgr::config(const AosXmlTagPtr &config)
{
	return true;
}
