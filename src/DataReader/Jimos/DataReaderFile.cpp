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
// This data reader reads a local file. The file does not have 
// backup. It is a local file. 
//
// 2013/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataReader/Jimos/DataReaderFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"


AosDataReaderFile::AosDataReaderFile(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
:
AosDataReader(rdata, worker_doc, jimo_doc)
{
	if (!config(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataReaderFile::~AosDataReaderFile()
{
}


bool
AosDataReaderFile::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


bool 
AosDataReaderFile::readBlock(
		const AosRundataPtr &rdata, 
		const int64_t start, 
		const int64_t len,
		AosBuffPtr &buff)
{
	if (!mFile)
	{
		if (!createFile(rdata)) return false;
	}
	aos_assert_rr(mFile, rdata, false);


}


bool 
AosDataReaderFile::readNextBlock(
		const AosRundataPtr &rdata, 
		AosBuffPtr &buff)
{
}

