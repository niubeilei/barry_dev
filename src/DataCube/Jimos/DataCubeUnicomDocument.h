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
#ifndef Aos_DataCube_DataCubeUnicomDocument_h
#define Aos_DataCube_DataCubeUnicomDocument_h

#include "DataCube/DataCube.h"
#include "DataCube/Jimos/Ptrs.h"
#include "DataCube/Jimos/DataCubeFile.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/NetFileCltObj.h"


class AosDataCubeUnicomDocument : public AosDataCube, public AosFileReadListener , public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;
private:

protected:
	AosFileInfo					mFileInfo;
	OmnMutexPtr             	mLock;
	int64_t						mReadBlockSize;
	OmnString					mCharset;
	AosXmlTagPtr				mMetadata;

	u32							mCrtFileIdx;
	u32							mReadTotalLen;
	vector<AosFileInfo>			mFileList;
	AosFileInfo					mCrtFileInfo;
	AosDataConnectorObjPtr		mDataCubeFileObj;
	AosDataConnectorCallerObjPtr		mCaller;

public:
	AosDataCubeUnicomDocument(const u32 version);

	AosDataCubeUnicomDocument(
			const AosFileInfo &file_info,
			const AosXmlTagPtr &metadata);

	~AosDataCubeUnicomDocument();

	// AosDataCubeCallerObj Interface
	virtual void callBack(
			const u64 &reqid,
			const AosBuffDataPtr &buff, 
			bool finished);

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;


	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual void 		setCaller(const AosDataConnectorCallerObjPtr &caller);
	virtual bool		readData(const u64 reqid, AosRundata *rdata);

	virtual AosDataConnectorObjPtr	nextDataConnector();

private:
	bool	getFileInfo(
				AosRundata *rdata,
				const AosXmlTagPtr &worker_doc);

};
#endif

