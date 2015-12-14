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
// This data cacher is used to interface with files. Files are obtained
// from AosStorageMgr so that this function does not have to worry about
// flooding the system. 
//
// Files have maximum size. When a file is full, it will create another 
// file. 
//
// All files are grouped into a container. 
//
// Modification History:
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCacher/CacherFile.h"

#include "SEInterfaces/ReadCacherObj.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataCacherType.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

AosCacherFile::AosCacherFile(const bool flag)
:
AosWriteCacherObj(AOSDATACACHER_FILE, AosDataCacherType::eCasherFile, flag)
{
}


AosCacherFile::AosCacherFile(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosWriteCacherObj(AOSDATACACHER_FILE, AosDataCacherType::eCasherFile, false)
{
	if (!config(def, rdata))
	{
	    OmnThrowException(rdata->getErrmsg());
	    return;
	}
}
	

AosCacherFile::~AosCacherFile()
{
}


bool
AosCacherFile::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
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
//	bool stable = def->getAttrBool("stable", false);

	// maxbuckets
	//int maxbuckets = def->getAttrInt("maxbuckets", 50);

	//operator
	OmnString opr = def->getAttrStr(AOSTAG_OPR, "index");
	aos_assert_r(opr != "", false);
	AosDataColOpr::E oper = AosDataColOpr::toEnum(opr);
	bool isvalid = AosDataColOpr::isValid(oper);
	aos_assert_r(isvalid, false);

	//maxCacheSize
	//int64_t maxCacheSize = def->getAttrU32("max_cachesize", 500000000); // 500M
	int64_t maxCacheSize = def->getAttrU32("max_cachesize", 30000000); // 30M

	//maxFileSize
	int64_t	maxFileSize = def->getAttrU32("max_filesize", 250000000); // 250M

	//minSize
	int64_t minSize = def->getAttrInt("min_size", 2000000); //2M

	//maxSize 
	int64_t maxSize = def->getAttrInt("max_size", 200000000); // 200M

	//buffSize
	int64_t buffSize = def->getAttrInt("buff_size", 1500000); //1.5M

	//actions
	mActions = def->getFirstChild("actions");

	//mMerger = OmnNew AosDataSort(comp, stable, 0, 0, 0, 0, 0);
	//aos_assert_r(mMerger, false);
//	mMerger = OmnNew AosDataSort(comp, stable, oper, maxCacheSize, maxFileSize, minSize, maxSize, buffSize, rdata);
//	aos_assert_r(mMerger, false);

	OmnString tableid = def->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");
	aos_assert_r(tableid != "", false);
/*	mMerger->setTableid(tableid);
	
	int job_serverid = def->getAttrInt(AOSTAG_JOBSERVERID, -1);
	aos_assert_r(job_serverid >= 0, false);
	mMerger->setJobServerId(job_serverid);

	u64 job_docid = def->getAttrU64(AOSTAG_JOB_DOCID, 0);
	aos_assert_r(job_docid > 0, false);
	mMerger->setJobDocId(job_docid);
*/
	return true;
}


bool
AosCacherFile::appendData(
		const AosBuffArrayPtr &buff_array,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mMergerRaw, false);
	bool rslt = mMergerRaw->addDataSync(buff_array, rdata.getPtr());
	return rslt;
}


bool 
AosCacherFile::finish(const AosRundataPtr &rdata)
{
	// aos_assert_r(mMergerRaw, false);
	// return mMergerRaw->finish(totalentries, xml, rdata);
	OmnNotImplementedYet;
	return false;
}


bool	
AosCacherFile::getNextBlock(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return true;
	//aos_assert_r(mMergerRaw, false);
	//return mMergerRaw->getNextBlock(buff, rdata);
}


bool 
AosCacherFile::appendRecord(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCacherFile::appendEntry(
		const AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCacherFile::clear()
{
	aos_assert_r(mMergerRaw, false);
	mMergerRaw->clear();
	return false;
}


int64_t
AosCacherFile::size()
{
	aos_assert_r(mMergerRaw, -1);
	return mMergerRaw->getTotalEntries();
}


char* 
AosCacherFile::getData(int64_t &size)const
{
	// This class does not support this function.
	OmnShouldNeverComeHere;
	return 0;
}


AosDataBlobObjPtr 
AosCacherFile::toDataBlob()const
{
	// This class does not support this function.
	OmnShouldNeverComeHere;
	return 0;
}


AosDataCacherObjPtr 
AosCacherFile::clone()
{
	return OmnNew AosCacherFile(*this);
}


AosDataCacherObjPtr 
AosCacherFile::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosCacherFile(def, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}


bool 
AosCacherFile::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mMergerRaw, rdata, false);
	return true;
	//return mMergerRaw->serializeTo(buff, rdata);
}


bool 
AosCacherFile::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	return true;
	aos_assert_rr(mMergerRaw, rdata, false);
	//return mMergerRaw->serializeFrom(buff, rdata);
}


bool 
AosCacherFile::split(
		vector<AosDataCacherObjPtr> &cachers, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosReadCacherObjPtr 
AosCacherFile::convertToReadCacher()
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosCacherFile::nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


int64_t
AosCacherFile::getTotalFileLength()
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosCacherFile::finish(
	 	const u64 &totalentries, 
	 	const AosXmlTagPtr &xml,
	 	const AosRundataPtr &rdata)
{
	aos_assert_r(mMergerRaw, false);
	return true;
//	return mMergerRaw->finish(totalentries, xml, rdata);
}

void
AosCacherFile::setMemory(const u64 maxmem)
{
	aos_assert(mMergerRaw);
	return;
//	mMergerRaw->setMemory(maxmem);
}

int
AosCacherFile::getMaxBucket()
{
	aos_assert_r(mMergerRaw, false);
	return 0;
	//return mMergerRaw->getMaxBucket();
}
