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
#ifndef Aos_DataSplitter_Jimos_FilesSplitByCube_h
#define Aos_DataSplitter_Jimos_FilesSplitByCube_h

#include "alarm_c/alarm.h"
#include "DataSplitter/DataSplitter.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"


class AosFilesSplitByCube : public AosDataSplitter
{
	OmnDefineRCObject;
	
	struct FileDef
	{
		AosFileInfo		mFileInfo;
		u32				mShuffleId;
	};

protected:
	//vector<AosFileInfo>  	mFileList;
	vector<FileDef>  	mFileList;

public:
	AosFilesSplitByCube(const u32 version);
	AosFilesSplitByCube(const OmnString &type, const u32 version);
	~AosFilesSplitByCube();

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



