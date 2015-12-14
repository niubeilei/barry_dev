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
// 2014/08/01 Created by Ice 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataSplitter_Jimos_IILSplitByTask_h
#define Aos_DataSplitter_Jimos_IILSplitByTask_h

#include "alarm_c/alarm.h"
#include "DataSplitter/DataSplitter.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"


class AosIILSplitByTask : public AosDataSplitter
{
	OmnDefineRCObject;

	enum
	{
		eEntries = 100000000
	};

	OmnString 		mIILName;
	AosXmlTagPtr 	mConfig;
	u64				mEntries;
public:
	AosIILSplitByTask(const u32 version);
	AosIILSplitByTask(const OmnString &type, const u32 version);
	~AosIILSplitByTask();

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



