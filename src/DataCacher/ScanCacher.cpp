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
#include "DataCacher/ScanCacher.h"

#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BuffData.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "API/AosApi.h"


AosScanCacher::AosScanCacher(const bool flag)
:
mNoMoreData(false)
{
	mMaxTasks = AosGetNumCpuCores();

	if (flag)
	{
		AosDataCacherObjPtr thisptr(this, false);
		if (!AosDataCacherObj::registerDataCacher(AOSDATACACHER_SCANCACHER, thisptr))
		{
			OmnThrowException("failed_registering scan cacher");
			return;
		}
	}

	initMemberData();
}
	

AosScanCacher::AosScanCacher(const AosScanCacher &rhs)
:
mNoMoreData(false)
{
	mMaxTasks = AosGetNumCpuCores();
}


AosScanCacher::AosScanCacher(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
mNoMoreData(false)
{
	mMaxTasks = AosGetNumCpuCores();
}
	

AosScanCacher::AosScanCacher(
		const AosDataScannerObjPtr &scanner,
		const AosDataBlobObjPtr &blob, 
		const AosRundataPtr &rdata)
:
mScanner(scanner),
mBlob(blob),
mNoMoreData(false)
{
	mMaxTasks = AosGetNumCpuCores();
}


AosScanCacher::~AosScanCacher()
{
	//OmnScreen << "delete scan cacher" << endl;
}


bool
AosScanCacher::initMemberData()
{
	return true;
}


bool
AosScanCacher::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_DATA_SCANNER);
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_scanner") << ": " << def->toString() << enderr;
		return false;
	}

	// Chen Ding, 2013/11/23
	// mScanner = AosDataScannerObj::createDataScanner(tag, rdata);
	mScanner = AosDataScannerObj::createDataScannerStatic(rdata, tag, 0);
	if (!mScanner)
	{
		AosSetErrorU(rdata, "failed_creating_scanner") << ": " << def->toString() << enderr;
		return false;
	}

	tag = def->getFirstChild(AOSTAG_DATA_BLOB);
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_data_blob") << ": " << def->toString() << enderr;
		return false;
	}

	mBlob = AosDataBlobObj::createDataBlobStatic(tag, rdata);
	if (!mBlob)
	{
		AosSetErrorU(rdata, "failed_creating_data_blob") << ": " << def->toString() << enderr;
		return false;
	}
	return true;
}


bool
AosScanCacher::copyMemberData(const AosScanCacher &rhs)
{
	return true;
}


bool
AosScanCacher::nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata)
{
	*data = 0;
	len = 0;
	docid = 0;
	metaData = 0;

	// Chen Ding, 10/30/2012
	if (mNoMoreData) 
	{
		return true;
	}

	aos_assert_r(mBlob, false);
	if (!mBlob->hasMoreData()) 
	{
		aos_assert_r(mScanner, false);

		AosBuffDataPtr info;
		bool rslt = mScanner->getNextBlock(info, rdata);
		aos_assert_r(rslt, false);
		if (mScanner->getDiskError())
		{
			mNoMoreData = true;
			metaData = OmnNew AosBuffData();
			metaData->setDiskError(true);
			return true;
		}
		if (!info || !info->getBuff() || (info->getBuff())->dataLen() <= 0)
		{
			mNoMoreData = true;
			return true;
		}
		rslt = mBlob->setDataBlock(info, rdata);
		aos_assert_r(rslt, false);
	}
	return mBlob->nextValue(data, len, docid, metaData, rdata);
}


bool	
AosScanCacher::nextValue(AosValueRslt &value, const AosRundataPtr &rdata)
{
	// Chen Ding, 10/30/2012
	// This is incorrect
	// aos_assert_rr(mBlob, rdata, 0);
	// if (!mBlob->hasMoreData()) return 0;
	// return mBlob->nextValue(value, false, rdata);
	OmnNotImplementedYet;
	return false;
}


bool	
AosScanCacher::nextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	// Chen Ding, 10/30/2012
	// This is incorrect
	// aos_assert_rr(mBlob, rdata, 0);
	// if (!mBlob->hasMoreData()) return 0;
	// return mBlob->nextRecord(record, rdata);
	OmnNotImplementedYet;
	return false;
}


bool	
AosScanCacher::firstValueInRange(
		const int64_t &idx, 
		AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosScanCacher::nextValueInRange(AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosScanCacher::firstRecordInRange(
		const int64_t &idx, 
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool	
AosScanCacher::nextRecordInRange(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosDataCacherObjPtr 
AosScanCacher::clone()
{
	return OmnNew AosScanCacher(*this);
}


AosDataCacherObjPtr 
AosScanCacher::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosScanCacher(def, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}


bool
AosScanCacher::split(
		vector<AosDataCacherObjPtr> &cachers, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mScanner, rdata, false);
	aos_assert_rr(mBlob, rdata, false);
	aos_assert_r(mMaxTasks >= 0, false);
	//Jozhi 2013/04/07
	AosDataBlobObjPtr blob;
	AosDataCacherObjPtr cacher;
	for (u32 i=0; i<mMaxTasks; i++)
	{
		try
		{
			blob = mBlob->clone();
			cacher = OmnNew AosScanCacher(mScanner, blob, rdata);
			cachers.push_back(cacher);
		}
		catch(...)
		{
			OmnAlarm << "split error" << enderr;
			return false;
		}

	}
	//Jozhi 2013/04/07
	/*
	vector<AosDataScannerObjPtr> scanners;
	bool rslt = mScanner->split(scanners, mBlob->getRecordLen(), rdata);
	aos_assert_rr(rslt, rdata, false);

	AosDataBlobObjPtr blob;
	AosDataCacherObjPtr cacher;
	for(u32 i=0; i<scanners.size(); i++)
	{
		try
		{
			blob = mBlob->clone();
			cacher = OmnNew AosScanCacher(scanners[i], blob, rdata);
			cachers.push_back(cacher);
		}
		catch(...)
		{
			OmnAlarm << "split error" << enderr;
			return false;
		}
	}
	*/
	return true;
}


bool 
AosScanCacher::serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	buff->setU32(AosDataCacherType::eScanCacher);
	bool rslt = mScanner->serializeTo(buff, rdata);
	aos_assert_r(rslt, false);
	mBlob->serializeTo(buff, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosScanCacher::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	// Chen Ding, 2013/11/23
	// mScanner = AosDataScannerObj::serializeFromStatic(buff, rdata);
	mScanner = AosDataScannerObj::createDataScannerStatic(rdata, buff);
	aos_assert_r(mScanner, false);
	mBlob = AosDataBlobObj::serializeFromStatic(buff, rdata);
	aos_assert_r(mBlob, false);
	return true;
}


int64_t
AosScanCacher::size()
{
	OmnNotImplementedYet;
	return -1;
}

	
bool
AosScanCacher::clear()
{
	OmnNotImplementedYet;
	return false;
}


char *	
AosScanCacher::getData(int64_t &len) const
{
	OmnNotImplementedYet;
	return 0;
}


AosDataBlobObjPtr
AosScanCacher::toDataBlob() const
{
	OmnNotImplementedYet;
	return 0;
}


AosDataCacherType::E
AosScanCacher::getType() const
{
	OmnNotImplementedYet;
	return AosDataCacherType::eScanCacher;
}


bool
AosScanCacher::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


int64_t
AosScanCacher::getTotalFileLength()
{
	return mScanner->getTotalSize(); 
}


bool 
AosScanCacher::appendData(const AosBuffArrayPtr &buff_array, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosScanCacher::appendRecord(const AosDataRecordObjPtr&, const AosRundataPtr&)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosScanCacher::appendEntry(const AosValueRslt&, const AosRundataPtr&)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosScanCacher::finish(
	 	const u64 &totalentries, 
	 	const AosXmlTagPtr &xml,
	 	const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

