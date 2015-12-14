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
// 2013/12/23 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_DataCubeFiles_h
#define Aos_DataCube_DataCubeFiles_h

#include "DataCube/Jimos/DataCubeCtnr.h"
#include "DataCube/Jimos/Ptrs.h"


class AosDataCubeFiles : public AosDataCubeCtnr
{
	OmnDefineRCObject;

private:
	int64_t						mReadBlockSize;
	OmnString					mCharset;
	AosXmlTagPtr				mMetadata;

public:
	AosDataCubeFiles(const u32 version);
	~AosDataCubeFiles();

	// Jimo	Interface
	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;
	
	virtual AosDataConnectorObjPtr cloneDataConnector();

private:

	bool		getFileInfo(
					AosRundata *rdata,
					const AosXmlTagPtr &defs);
};
#endif
