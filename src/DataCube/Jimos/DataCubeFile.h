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
#ifndef Aos_DataCube_DataCubeFile_h
#define Aos_DataCube_DataCubeFile_h

#include "DataCube/DataCube.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SEInterfaces/DataScannerObj.h"

class AosDataCubeFile : public AosDataCube, public AosFileReadListener
{
	OmnDefineRCObject;
	
private:
	AosFileInfo				mFileInfo;
	AosXmlTagPtr			mMetadata;
	int64_t					mReadBlockSize;
	bool					mIsSkipFirstLine;
	AosNetFileObjPtr		mFile;
	AosDataConnectorCallerObjPtr mCaller;
	bool 					mFlag;    //when first to call the function nextConnector,mFlag is true;otherwise is false 
	OmnMutexPtr				mLock;

public:
	AosDataCubeFile(const u32 version);
	AosDataCubeFile(const AosFileInfo &file_info,
					const int64_t readBlockSize,
					const AosXmlTagPtr &metadata);
	~AosDataCubeFile();

/*
	virtual int getPhysicalId() const;
	virtual AosDataCubeObjPtr cloneDataCube();
	virtual bool getNextBlock(
						AosRundata *rdata, 
						AosBuffDataPtr &buff_data);
*/	
	// AosFileReadListener Interface
	virtual void fileReadCallBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished,
			const AosDiskStat &disk_stat);

	virtual OmnString getFileName() const;
	virtual int64_t getFileLength() const;
	virtual int getPhysicalId() const;
	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff);
	virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff);
	
	virtual void setCaller(const AosDataConnectorCallerObjPtr &caller);

	virtual bool	readData(const u64 reqid, AosRundata *rdata);
	
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
	
    virtual AosDataConnectorObjPtr nextDataConnector();

	// jimo, interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc) {return true;}

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc);
};
#endif



