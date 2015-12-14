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
// 2015/01/05 created by Andy zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_DataCubeDataCol_h
#define Aos_DataCube_DataCubeDataCol_h

#include "DataCube/Jimos/DataCubeCtnr.h"
#include "DataCube/Jimos/Ptrs.h"


class AosDataCubeDataCol : public AosDataCubeCtnr
{
	OmnDefineRCObject;

private:
	OmnString			mColId; // data collector id
	AosXmlTagPtr				mMetadata;
	int64_t						mReadBlockSize;


public:
	AosDataCubeDataCol(const u32 version);
	~AosDataCubeDataCol();

	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	virtual void setTaskDocid(const u64 task_docid);
private:

	bool		getFileInfo(const AosRundataPtr &rdata);
};
#endif
