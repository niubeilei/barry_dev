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
// 11/17/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataCollector_DataCollectorBuff_h
#define AOS_DataCollector_DataCollectorBuff_h

#include "DataCollector/DataCollector.h"

class AosDataCollectorBuff : virtual public AosDataCollector 
{
public:
	AosDataCollectorBuff(const u64& job_id);
	~AosDataCollectorBuff();
	
	virtual bool addOutput(
				const AosXmlTagPtr &output, 
				const AosRundataPtr &rdata);

	virtual bool finishDataCollector(const AosRundataPtr &rdata);

	virtual bool config(const AosJobObjPtr &job,
					const AosXmlTagPtr &conf, 
					const AosRundataPtr &rdata);
};
#endif
