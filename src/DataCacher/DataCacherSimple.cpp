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
// This is a simple data cacher. It uses a datablob as its in-memory cache.
// When adding an entry, it adds the entry to the cacher. When the cacher
// is full, it will automatically send the contents. Since the contents are
// sent through a separate thread, this class does not use two cachers, one
// for sending the contents and the other accepting new contents.
//
// Modification History:
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCacher/DataCacherSimple.h"

#include "Actions/SdocAction.h"
#include "DataBlob/DataBlob.h"
#include "Rundata/Rundata.h"
#include "Debug/Except.h"
#include "Util/Buff.h"
#include "Thread/Mutex.h"


AosDataCacherSimple::AosDataCacherSimple(const bool flag)
:
AosDataCacher(AOSDATACACHER_SIMPLE, AosDataCacherType::eSimple, flag),
mLock(OmnNew OmnMutex())
{
}


AosDataCacherSimple::AosDataCacherSimple(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataCacher(AOSDATACACHER_SIMPLE, AosDataCacherType::eSimple, false), 
mLock(OmnNew OmnMutex())
{
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataCacherSimple::~AosDataCacherSimple()
{
}


bool 
AosDataCacherSimple::config(
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
AosDataCacherSimple::appendEntry(
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mBlob, rdata, false);

	bool rslt = mBlob->appendEntry(value, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosDataCacherPtr 
AosDataCacherSimple::clone(
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataCacherSimple(config, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << rdata->getErrmsg() << enderr;
		return 0;
	}
}


char* 	
AosDataCacherSimple::nextValue(int &len)
{
	aos_assert_r(mBlob, 0);
	return mBlob->nextValue(len);
}


bool 	
AosDataCacherSimple::clear()
{
	aos_assert_r(mBlob, 0);
	return mBlob->clearData();
}


AosDataCacherPtr 
AosDataCacherSimple::clone()
{
	try
	{
		AosDataCacherSimple* cacher = OmnNew AosDataCacherSimple(false);
		AosDataCacher::copyMemberData(cacher);
		cacher->mBlob = mBlob->clone();
		return cacher;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating obj" << enderr;
		return 0;
	}
}
#endif
