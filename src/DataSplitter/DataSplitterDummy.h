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
#ifndef Aos_DataSplitter_DataSplitterDummy_h
#define Aos_DataSplitter_DataSplitterDummy_h

#include "DataSplitter/DataSplitter.h"


class AosDataSplitterDummy : public AosDataSplitter
{
public:
	AosDataSplitterDummy(const u32 version);
	~AosDataSplitterDummy();

	virtual AosJimoPtr cloneJimo() const;
	virtual bool splitData(
					AosRundata *rdata, 
					const vector<AosDataSourceObjPtr> &data_sources,
					vector<AosDatasetObjPtr> &datasets);

	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc);
	virtual bool	split(
						AosRundata *rdata,
						vector<AosXmlTagPtr> &data_units);
};
#endif



