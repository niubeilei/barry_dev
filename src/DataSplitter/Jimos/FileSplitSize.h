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
#ifndef Aos_DataSplitter_Jimos_FileSplitSize_h
#define Aos_DataSplitter_Jimos_FileSplitSize_h

#include "alarm_c/alarm.h"
#include "DataSplitter/Jimos/FileSplit.h"


class AosFileSplitFixSize : public AosFileSplit
{
	OmnDefineRCObject;

	int		mRecordSize;

public:
	AosFileSplitFixSize(const u32 version);
	~AosFileSplitFixSize();

	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	virtual bool split(
					AosRundata *rdata,
					vector<AosXmlTagPtr> &v);

};
#endif



