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
// 07/11/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataScanner/VirtualFileScanner.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "API/AosApi.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlDoc.h"


AosVirtualFileScanner::AosVirtualFileScanner(
		const OmnString &objid,
		const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_VIRTUALFILE, AosDataScannerType::eVirtualFile, false)
{
	try
	{
		mNetFile = OmnNew AosNetFile(objid, rdata);
	}
	catch(...)
	{
		OmnAlarm << "failed to create virtual file" << enderr;
	}
}

AosVirtualFileScanner::AosVirtualFileScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_VIRTUALFILE, AosDataScannerType::eVirtualFile, flag)
{
}


AosVirtualFileScanner::AosVirtualFileScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_VIRTUALFILE, AosDataScannerType::eVirtualFile, false)
{
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosVirtualFileScanner::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


AosVirtualFileScanner::~AosVirtualFileScanner()
{
}


AosDataScannerObjPtr
AosVirtualFileScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosVirtualFileScanner(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}

bool
AosVirtualFileScanner::getNextBlock(
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	if (mCrtSegments.size())
	{
		bool rslt = getAllCrtSegments(rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	aos_assert_rr(mCrtSegments.size() > 0, rdata,false);
	if (mCrtIdx >= mCrtSegments.size())
	{
		return true;
	}
	u64 segid = mCrtSegments[mCrtIdx++];
	return mNetFile->readSegment(segid, buff, rdata);
}



bool 
AosVirtualFileScanner::appendBlock(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosVirtualFileScanner::modifyBlock(
		const int64_t &seekPos, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosVirtualFileScanner::split(
		vector<AosDataScannerObjPtr> &scanners,
		const int64_t &record_len,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosVirtualFileScanner::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	return false;
}


bool 
AosVirtualFileScanner::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(buff, rdata, false);
	return false;
}


bool
AosVirtualFileScanner::getAllCrtSegments(const AosRundataPtr &rdata)
{
	vector<u64> segids = mNetFile->getAllSegment(rdata);
	for (u32 i=0; i<segids.size(); i++)
	{
		int physicalid = AosGetPhysicalId(AosXmlDoc::getOwnDocid(segids[i]));
		if (AosIsPhysicalIdLocal(physicalid))
		{
			mCrtSegments.push_back(segids[i]);
		}
	}
	return true;
}

#endif
