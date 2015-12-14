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
#ifndef Aos_DataSplitter_Jimos_FileSplit_h
#define Aos_DataSplitter_Jimos_FileSplit_h

#include "alarm_c/alarm.h"
#include "DataSplitter/DataSplitter.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"


class AosFileSplit : public AosDataSplitter
{
	OmnDefineRCObject;

protected:
	int64_t 	mGroupSize;
	AosFileInfo mFileInfo;

public:
	AosFileSplit(const u32 version);
	AosFileSplit(const OmnString &type, const u32 version);
	~AosFileSplit();

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
	void setFileInfo(const AosFileInfo &fileinfo) { mFileInfo = fileinfo;}
	virtual bool	split(
						AosRundata *rdata,
						vector<AosXmlTagPtr> &data_units);

private:
};
#endif



