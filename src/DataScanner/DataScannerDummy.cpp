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
// This data scanner has only one file. Normally the file resides on the 
// local machine, but it is also possible that the file is remote.
//
// Modification History:
// 2013/11/21: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataScanner/DataScannerDummy.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include "Util/DiskStat.h"

AosDataScannerDummy::AosDataScannerDummy(const u32 version)
:
AosDataScanner(AOSDATASCANNER_DUMMY, version)
{
}


AosDataScannerDummy::~AosDataScannerDummy()
{
}

int 	
AosDataScannerDummy::getPhysicalId() const
{
	OmnShouldNeverComeHere;
	return -1;	
}


int64_t
AosDataScannerDummy::getTotalSize() const
{
	OmnShouldNeverComeHere;
	return -1;	
}


AosDataScannerObjPtr
AosDataScannerDummy::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	return OmnNew AosDataScannerDummy(*this);
}


bool
AosDataScannerDummy::getNextBlock(
		AosBuffDataPtr &buff_data,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;	
}


bool 
AosDataScannerDummy::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;	
}


bool 
AosDataScannerDummy::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;	
}


AosJimoPtr 
AosDataScannerDummy::cloneJimo()  const
{
	OmnShouldNeverComeHere;
	return NULL;
}

