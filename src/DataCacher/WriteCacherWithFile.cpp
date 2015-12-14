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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
Renamed to CacherFile.cpp. Chen Ding, 08/29/2012
#include "DataCacher/WriteCacherWithFile.h"

#include "SEInterfaces/ReadCacherObj.h"
#include "SEInterfaces/DataCacherType.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

AosWriteCacherWithFile::AosWriteCacherWithFile(const bool flag)
:AosWriteCacherObj(AosDataCacherType::eWriteCasherWithFile)
{
	if (flag)
	{
		AosDataCacherObjPtr thisptr(this, false);
		if (!AosDataCacherObj::registerDataCacher(AOSDATACACHER_WRITECASHERWITHFILE, thisptr))
		{
			OmnThrowException("failed_registering write cacher with file");
			return;
		}
		
	}
}

AosWriteCacherWithFile::AosWriteCacherWithFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!config(def, rdata))
	{
	    OmnThrowException(rdata->getErrmsg());
	    return;
	}
}
	
AosWriteCacherWithFile::~AosWriteCacherWithFile()
{
}

bool
AosWriteCacherWithFile::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
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
}

bool
AosWriteCacherWithFile::appendData(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mMerger, false);
	bool rslt = mMerger->addDataSync(buff, rdata);
	return rslt;
}

bool 
AosWriteCacherWithFile::finish(
		const u64 totalentries, 
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mMerger, false);
	return mMerger->finish(totalentries, xml, rdata);
}
bool	
AosWriteCacherWithFile::getNextBlock(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mMerger, false);
	return mMerger->getNextBlock(buff, rdata);
}

bool 
AosWriteCacherWithFile::appendRecord(
		const AosDataRecord &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosWriteCacherWithFile::appendEntry(
		const AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosWriteCacherWithFile::clear()
{
	aos_assert_r(mMerger, false);
	mMerger->clear();
	return false;
}

int64_t
AosWriteCacherWithFile::size()
{
	aos_assert_r(mMerger, -1);
	return mMerger->getTotalEntries();
}

char* 
AosWriteCacherWithFile::getData(int64_t &size)const
{
	// This class does not support this function.
	OmnShouldNeverComeHere;
	return 0;
}

AosDataBlobObjPtr 
AosWriteCacherWithFile::toDataBlob()const
{
	// This class does not support this function.
	OmnShouldNeverComeHere;
	return 0;
}

AosDataCacherObjPtr 
AosWriteCacherWithFile::clone()
{
	return OmnNew AosWriteCacherWithFile(*this);
}

AosDataCacherObjPtr 
AosWriteCacherWithFile::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosWriteCacherWithFile(def, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

bool 
AosWriteCacherWithFile::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mMerger, rdata, false);
	return mMerger->serializeTo(buff, rdata);
}

bool 
AosWriteCacherWithFile::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mMerger, rdata, false);
	return mMerger->serializeFrom(buff, rdata);
}

bool 
AosWriteCacherWithFile::split(
		vector<AosDataCacherObjPtr> &cachers, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosReadCacherObjPtr 
AosWriteCacherWithFile::convertToReadCacher()
{
	OmnNotImplementedYet;
	return 0;
}

bool 
AosWriteCacherWithFile::setFileName(int, const OmnString&)
{
	OmnShouldNeverComeHere;
	return false;
}

char* 
AosWriteCacherWithFile::nextValue(int&, const AosRundataPtr&)
{
	OmnNotImplementedYet;
	return 0;
}


#endif

