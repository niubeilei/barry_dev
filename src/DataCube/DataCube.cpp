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
//
// Modification History:
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/DataCube.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"


AosDataCube::AosDataCube()
{
}


AosDataCube::AosDataCube(
		const OmnString &type,
		const u32 version)
:
AosDataConnectorObj(version),
mJimoSubType(type)
{
}


AosDataCube::~AosDataCube()
{
}

OmnString 
AosDataCube::getFileName() const
{
	OmnNotImplementedYet;
	return "";
}

int64_t 
AosDataCube::getFileLength() const
{
	OmnShouldNeverComeHere;
	return -1;	
}

int 
AosDataCube::getPhysicalId() const
{
	return -1;
}


AosDataConnectorObjPtr 
AosDataCube::cloneDataConnector()
{
	return OmnNew AosDataCube(*this);
}

	
bool 
AosDataCube::appendBlock(
		AosRundata *rdata, 
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCube::writeBlock(
		AosRundata *rdata, 
		const int64_t pos,
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCube::copyData(
		AosRundata *rdata, 
		const OmnString &from_name,
		const OmnString &to_name)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCube::removeData(
		AosRundata *rdata, 
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCube::serializeFrom(AosRundata *rdata, const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataCube::serializeTo(AosRundata *rdata, const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr 
AosDataCube::cloneJimo()  const
{
	OmnShouldNeverComeHere;
	return 0;
}

void 
AosDataCube::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	OmnShouldNeverComeHere;
}

bool 
AosDataCube::readData(
		const u64 reqid,
		AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	return false;
}
	
AosDataConnectorObjPtr	
AosDataCube::nextDataConnector()
{
	return 0;
}

bool 
AosDataCube::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	return true;
}
