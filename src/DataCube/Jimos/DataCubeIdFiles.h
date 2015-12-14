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
#ifndef Aos_DataCube_DataCubeIdFiles_h
#define Aos_DataCube_DataCubeIdFiles_h

#include "DataCube/Jimos/DataCubeCtnr.h"
#include "DataCube/Jimos/Ptrs.h"


class AosDataCubeIdFiles : public AosDataCubeCtnr
{
	OmnDefineRCObject;

private:
	int64_t						mReadBlockSize;
	AosXmlTagPtr				mMetadata;

public:
	AosDataCubeIdFiles(const u32 version);
	~AosDataCubeIdFiles();

	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
private:

	bool		getFileInfo(
					AosRundata *rdata,
					const AosXmlTagPtr &files_xml);
};
#endif
