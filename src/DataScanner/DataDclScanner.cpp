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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/DataDclScanner.h"

#include "DataCollector/DataCollectorMgr.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"

AosDclScanner::AosDclScanner(const bool flag)
:
AosDataScanner(AOSDATASCANNER_DCL, AosDataScannerType::eDcl, flag)
{
}

AosDclScanner::AosDclScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataScanner(AOSDATASCANNER_DCL, AosDataScannerType::eDcl, false)
{
	bool rslt = config(conf, rdata);
	if (!rslt) OmnThrowException("Failed to create data scanner");
}


AosDclScanner::~AosDclScanner()
{
}


AosJimoPtr
AosDclScanner::cloneJimo() const
{
	try
	{
		return OmnNew AosDclScanner(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataScannerObjPtr
AosDclScanner::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDclScanner(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


AosDataRecordObjPtr
AosDclScanner::getDataRecord() const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosDclScanner::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	//<scanner type="dcl" dcl_name="xxx" sequnce="true" auto_delete="true">
	//</scanner>
	
	aos_assert_r(conf, false);
	OmnString datacol_name = conf->getAttrStr("dcl_name", "");
	aos_assert_r(datacol_name != "", false);
	u64 job_docid = conf->getAttrU64(AOSTAG_JOB_DOCID, 0);
	mCacher = NULL;//AosDataCollectorMgr::getSelf()->getDataCacher(job_docid, datacol_name, rdata);
	aos_assert_r(mCacher, false);
	return true;
}


char *
AosDclScanner::nextRecord(int &len, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return NULL;
}


bool   
AosDclScanner::getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	return mCacher->getNextBlock(buff, rdata);
}


int
AosDclScanner::getPhysicalId() const
{
	return -1;
}


int64_t
AosDclScanner::getTotalSize() const
{
	OmnNotImplementedYet;
	return -1;
}

