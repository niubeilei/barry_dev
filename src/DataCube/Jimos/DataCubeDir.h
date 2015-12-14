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
// 2013/12/23 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_DataCubeDir_h
#define Aos_DataCube_DataCubeDir_h

#include "DataCube/Jimos/DataCubeCtnr.h"
#include "DataCube/Jimos/DataCubeFile.h"
#include "DataCube/Jimos/Ptrs.h"
#include "SEInterfaces/NetFileCltObj.h"

class AosDataCubeDir : public AosDataCubeCtnr
{
	OmnDefineRCObject;

private:
	int64_t						mReadBlockSize;
	OmnString					mCharset;
	AosXmlTagPtr				mMetadata;

public:
	AosDataCubeDir(const u32 version);
	~AosDataCubeDir();

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;

	virtual AosDataConnectorObjPtr cloneDataConnector();

};
#endif



