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
#include "DataSplitter/DataSplitterDummy.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDataSplitterDummy::AosDataSplitterDummy(const u32 version) :
AosDataSplitter(AOSDATASPLITTER_DUMMY, version)
{
}


AosDataSplitterDummy::~AosDataSplitterDummy()
{
}

AosJimoPtr 
AosDataSplitterDummy::cloneJimo()  const
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDataSplitterDummy::splitData(
		AosRundata *rdata, 
		const vector<AosDataSourceObjPtr> &data_sources,
		vector<AosDatasetObjPtr> &datasets)
{
	OmnShouldNeverComeHere;
	return true;
}


bool 
AosDataSplitterDummy::split(
		AosRundata *rdata, 
		vector<AosXmlTagPtr> &data_units)
{
	OmnShouldNeverComeHere;
	return true;
}


bool	
AosDataSplitterDummy::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	OmnShouldNeverComeHere;
	return true;
}
