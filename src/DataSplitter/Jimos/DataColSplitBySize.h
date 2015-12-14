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
// 2015/01/05 Created by Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSplitter_Jimos_DataColSplitBySize_h
#define Aos_DataSplitter_Jimos_DataColSplitBySize_h

#include "alarm_c/alarm.h"
#include "DataSplitter/Jimos/DataColSplitByGroup.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"


class AosDataColSplitBySize : public AosDataColSplitByGroup
{
	OmnDefineRCObject;
	
protected:
	int64_t     		mGroupSize; 
	int     			mRecordSize;
	AosFileInfo			mFileInfo;

public:
	AosDataColSplitBySize(const u32 version);
	AosDataColSplitBySize(const OmnString &type, const u32 version);
	~AosDataColSplitBySize();

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
	virtual AosJimoPtr cloneJimo() const;


	// DataSplitter interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual bool	split(
						AosRundata *rdata,
						vector<AosXmlTagPtr> &data_units);

private:
};
#endif
