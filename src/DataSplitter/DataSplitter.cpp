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
// 2013/11/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/DataSplitter.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDataSplitter::AosDataSplitter(
		const OmnString &type,
		const int ver)
:
AosDataSplitterObj(ver)
{
}

AosDataSplitter::~AosDataSplitter()
{
}

void
AosDataSplitter::setJobDocid(const u64 &job_docid)
{
	mJobDocid = job_docid;
}
