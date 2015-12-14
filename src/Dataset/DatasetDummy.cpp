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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Dataset/DatasetDummy.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosDatasetDummy::AosDatasetDummy(const u32 version)
:
AosDataset(AOSDATASET_DUMMY, version)
{
}


AosDatasetDummy::~AosDatasetDummy()
{
}

/*
bool 
AosDatasetDummy::reset(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


int64_t 
AosDatasetDummy::getNumEntries()
{
	OmnShouldNeverComeHere;
	return -1;
}


bool 
AosDatasetDummy::nextRecordset(
		const AosRundataPtr &rdata, 
		AosRecordsetObjPtr &recordset)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDatasetDummy::addDataConnector(
	const AosRundataPtr &rdata, 
	const AosDataConnectorObjPtr &data_cube)
{
	OmnNotImplementedYet;
	return false;
}
	
bool 
AosDatasetDummy::addUnrecogContents(
	const AosRundataPtr &rdata,
	const AosBuffDataPtr &buff_data,
	const char *data,
	const int64_t &start_pos,
	const int64_t &length)
{
	OmnNotImplementedYet;
	return false;
}

AosJimoPtr 
AosDatasetDummy::cloneJimo()  const
{
	OmnNotImplementedYet;
	return NULL;
}

*/
