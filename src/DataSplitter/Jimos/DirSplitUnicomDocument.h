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
#ifndef Aos_DataSplitter_Jimos_DirSplitUnicomDocument_h
#define Aos_DataSplitter_Jimos_DirSplitUnicomDocument_h

#include "alarm_c/alarm.h"
#include "DataSplitter/Jimos/DirSplit.h"

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosDirSplitUnicomDocument : public AosDirSplit
{
	OmnDefineRCObject;

	enum
	{
		eDftMaxGroupNumFiles = 1000,
		eDftGroupSize = 1000000000	// 1GB
	};

	int								mPhysicalId;
	int64_t 						mGroupNumFiles;	// Num of files
	int64_t							mGroupSize;		// Group max size
	vector<AosFileInfo>				mXmlList;
	map<OmnString, AosFileInfo>		mZipList;

public:
	AosDirSplitUnicomDocument(const u32 version);
	~AosDirSplitUnicomDocument();

	// Jimo interface
	virtual AosJimoPtr cloneJimo() const;
	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc);

	// DataSplitterObj interface
	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	virtual bool split(
					AosRundata *rdata,
					vector<AosXmlTagPtr> &v);

	bool	listcmp(const AosFileInfo &lhs, const AosFileInfo &rhs);

private:
	AosXmlTagPtr	getCubeFilesConfig(
						AosRundata *rdata,
						vector<AosFileInfo> &v);
};
#endif



