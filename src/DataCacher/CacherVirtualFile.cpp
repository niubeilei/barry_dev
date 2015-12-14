////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCacher/CacherVirtualFile.h"
#include "SEInterfaces/ReadCacherObj.h"
#include "SEInterfaces/DataCacherType.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

AosCacherVirtualFile::AosCacherVirtualFile(const bool flag)
:
AosWriteCacherObj(AOSDATACACHER_VIRTUAL_FILE, AosDataCacherType::eCasherVirtualFile, flag)
{
}


AosCacherVirtualFile::AosCacherVirtualFile(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosWriteCacherObj(AOSDATACACHER_VIRTUAL_FILE, AosDataCacherType::eCasherVirtualFile, false)
{
	if (!config(def, rdata))
	{
	    OmnThrowException(rdata->getErrmsg());
	    return;
	}
}
	

AosCacherVirtualFile::~AosCacherVirtualFile()
{
}


bool
AosCacherVirtualFile::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	/*
	//compare function
	AosXmlTagPtr tag = def->getFirstChild("CompareFun");
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_compare function") << ": " << def->toString() << enderr;
		return false;
	}
	AosCompareFunPtr comp = AosCompareFun::getCompareFunc(tag);
	if (!comp)
	{
		AosSetErrorU(rdata, "missing_compare function") << ": " << def->toString() << enderr;
		return false;
	}

	// sort stable
	bool stable = def->getAttrBool("stable", false);

	// maxbuckets
	int maxbuckets = def->getAttrInt("maxbuckets", 100);

	//maxMemEntries
	u32 maxMemEntries = def->getAttrU32("max_mementries", 100000);

	//maxEntries
	u32	maxEntries = def->getAttrU32("max_entries", 5000000); 

	//actions
	mActions = def->getFirstChild("actions");

	mMerger = OmnNew AosDataSort(comp, stable, maxbuckets, maxMemEntries, maxEntries, 0, 0);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosCacherVirtualFile::appendData(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(mNetFile, rdata, false);

	// 1. If there are filters for records, run the filter.
	mLock->lock();
	if (!mNetFile->appendRecord(buff, rdata))
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


bool 
AosCacherVirtualFile::finish(const AosRundataPtr &rdata)
{
	// This function will inform all the member data cachers to finish.
	mLock->lock();
	for (u32 i=0; i<mMemberCachers.size(); i++)
	{
		mMemberCachers[i]->finish(rdata);
	}
	mLock->unlock();
	return true;
}


bool 
AosCacherVirtualFile::appendRecord(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(record, rdata, false);
	aos_assert_rr(mNetFile, rdata, false);

	// 1. If there are filters for records, run the filter.
	mLock->lock();
	if (mRecordFilter)
	{
		if (mRecordFilter->filterData(record, rdata))
		{
			// Record is filtered.
			if (mFilteredTrashCan)
			{
				mFilteredTrashCan->appendRecord(record, rdata);
				mLock->unlock();
				return true;
			}
		}
	}

	if (!mNetFile->appendRecord(record, rdata))
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


bool 
AosCacherVirtualFile::appendEntry(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	aos_assert_rr(value, rdata, false);
	aos_assert_rr(mNetFile, rdata, false);

	// 1. If there are filters for value, run the filter.
	mLock->lock();
	if (mRecordFilter)
	{
		if (mRecordFilter->filterData(value, rdata))
		{
			// Record is filtered.
			if (mFilteredTrashCan)
			{
				mFilteredTrashCan->appendRecord(value, rdata);
				mLock->unlock();
				return true;
			}
		}
	}

	if (!mNetFile->appendRecord(value, rdata))
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
	OmnNotImplementedYet;
	return false;
}


bool 
AosCacherVirtualFile::clear()
{
	return true;
}


int64_t
AosCacherVirtualFile::size()
{
	return 0;
}


char* 
AosCacherVirtualFile::getData(int64_t &size)const
{
	// This class does not support this function.
	OmnShouldNeverComeHere;
	return 0;
}


AosDataBlobObjPtr 
AosCacherVirtualFile::toDataBlob()const
{
	// This class does not support this function.
	OmnShouldNeverComeHere;
	return 0;
}


AosDataCacherObjPtr 
AosCacherVirtualFile::clone()
{
	return OmnNew AosCacherVirtualFile(*this);
}


AosDataCacherObjPtr 
AosCacherVirtualFile::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosCacherVirtualFile(def, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}


bool 
AosCacherVirtualFile::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCacherVirtualFile::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCacherVirtualFile::split(
		vector<AosDataCacherObjPtr> &cachers, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosReadCacherObjPtr 
AosCacherVirtualFile::convertToReadCacher()
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosCacherVirtualFile::setFileName(int, const OmnString&)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosCacherVirtualFile::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


char* 
AosCacherVirtualFile::nextValue(int&, const AosRundataPtr&)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosCacherVirtualFile::finish(
	 	const u64 &totalentries, 
	 	const AosXmlTagPtr &xml,
	 	const AosRundataPtr &rdata)
{
	aos_assert_rr(mNetFile, rdata, false);
	return mNetFile->finish(totalentries, xml, rdata);
}

#endif
