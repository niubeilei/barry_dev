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
#ifndef Aos_DataCube_DataCubeReadDocNoSort_h
#define Aos_DataCube_DataCubeReadDocNoSort_h

#include "DataCube/Jimos/DataCubeCtnr.h"
#include "DataCube/ReadDocUtil/Ptrs.h"
#include "Thread/Mutex.h"

class AosDataCubeReadDocNoSort : public AosDataCubeCtnr
{
	OmnDefineRCObject;
	
public:
	AosDataCubeReadDocNoSort(const u32 version);
	AosDataCubeReadDocNoSort(
			const OmnString &type,
			const u32 version);

	~AosDataCubeReadDocNoSort();

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;

	// DataCube Interface
	virtual AosDataConnectorObjPtr cloneDataConnector();

private:
	bool 	splitDocids(
				AosRundata *rdata,
				const AosXmlTagPtr &conf);
	bool 	getDocidsByConf(
				const AosXmlTagPtr &conf,
				AosBuffPtr &docids_buff,
				u32 &total);

};
#endif

