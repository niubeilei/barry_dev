////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/10/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataReader/DataReaderDummy.h"

AosDataReaderDummy::AosDataReaderDummy(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosDataReader(rdata, worker_doc, jimo_doc)
{
}


AosDataReaderDummy::~AosDataReaderDummy()
{
}


bool 
AosDataReaderDummy::readBlock(
		const AosRundataPtr &rdata, 
		const int64_t start, 
		const int64_t len,
		AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataReaderDummy::readNextBlock(
		const AosRundataPtr &rdata, 
		AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}

