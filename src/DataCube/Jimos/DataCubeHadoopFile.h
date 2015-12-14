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
#ifndef Aos_DataCube_DataCubeHadoopFile_h
#define Aos_DataCube_DataCubeHadoopFile_h

#if 0
#include "DataCube/DataCube.h"
#include "HDFS/hdfs.h"

class AosDataCubeHadoopFile : virtual public AosDataCube, public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;
	enum                 
	{
		    eMaxSize = 10000000 //10M
	};

private:
	OmnString		mFileName;
	tOffset			mCrtOffset;
	i64				mBlockSize;
	i64				mFileLength;
	hdfsFile		mFile;
	hdfsFS			mFS;
	OmnString		mIP;
	int				mPort;
	OmnString		mCharset;
	AosXmlTagPtr	mMetadata;
	AosDataConnectorCallerObjPtr mCaller;

public:
	AosDataCubeHadoopFile(const u32 version);
	~AosDataCubeHadoopFile();

	//Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// DataCube Interface
	virtual void 	setCaller(const AosDataConnectorCallerObjPtr &caller);
	virtual bool	readData(const u64 reqid, AosRundata *rdata);
	virtual AosDataConnectorObjPtr cloneDataConnector();
	
	// AosDataCubeCallerObj Interface
	virtual void callBack(
			const u64 &reqid,
			const AosBuffDataPtr &buff, 
			bool finished);

private:
	bool config(	const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
};
#endif
#endif


