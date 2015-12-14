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
#ifndef Aos_DataCube_DataCubeInternFile_h
#define Aos_DataCube_DataCubeInternFile_h

#include "DataCube/DataCube.h"

class AosDataCubeInternFile : public AosDataCube
{
	OmnDefineRCObject;
	
private:
	int					mPhysicalId;
	OmnString			mFileName;
	AosDataCubeFilePtr	mFileCube;
	AosDataCubeFile *	mFileCubeRaw;

public:
	AosDataCubeInternFile(const OmnString &version);
	~AosDataCubeInternFile();

	virtual OmnString getFileName() const;
	virtual int64_t getFileLength() const;
	virtual int getPhysicalId() const;
	virtual AosDataCubeObjPtr cloneDataCube();
	virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff);
	virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff);
	virtual bool readBlock(
					AosRundata *rdata, 
					AosBuffDataPtr &buff_data);
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	
	virtual bool appendBlock(
					AosRundata *rdata, 
					AosBuffDataPtr &buff_data); 

	virtual bool writeBlock(
					AosRundata *rdata, 
					const int64_t pos,
					AosBuffDataPtr &buff_data); 

	virtual bool copyData(
					AosRundata *rdata, 
					const OmnString &from_name,
					const OmnString &to_name);

	virtual bool removeData(
					AosRundata *rdata, 
					const OmnString &name);
};
#endif



