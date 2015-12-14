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
#ifndef Aos_DataCube_DataCubeIIL_h
#define Aos_DataCube_DataCubeIIL_h

#include "DataCube/DataCube.h"
#include "SEInterfaces/DataConnectorObj.h"
#include "TransUtil/Ptrs.h"
#include "SEUtil/AsyncRespCaller.h"

class AosDataCubeIIL : public AosDataCube , public AosAsyncRespCaller
{
	OmnDefineRCObject;

	enum
	{
		eDftBlockSize = 1000000
	};

private:
	AosRundataPtr					mRundata;
	//int64_t							mTotalEntries;
	int64_t							mTotalReadEntries;
	u32								mBlockSize;
	OmnString						mIILName;
	AosDataConnectorCallerObjPtr 		mCaller;
	AosQueryContextObjPtr 			mQueryContext;
	u64								mSnapId;
	bool							mWithKey;

public:
	AosDataCubeIIL(const u32 version);
	~AosDataCubeIIL();

	virtual OmnString getFileName() const;
	virtual int64_t getFileLength() const;
	virtual int getPhysicalId() const;
	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff);
	virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff);
	virtual void setCaller(const AosDataConnectorCallerObjPtr &caller);

	virtual bool readData(
			const u64 reqid,
			AosRundata *rdata);
	
	virtual bool readBlock(
					AosRundata *rdata, 
					AosBuffDataPtr &buff_data);
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

	// jimo, interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc,
					const AosXmlTagPtr &jimo_doc) {return true;}

	virtual bool config(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &worker_doc);

	virtual void	 callback(
					const AosTransPtr &trans,
					const AosBuffPtr &resp,
					const bool svr_death);

private:
	AosXmlTagPtr	getXmlFromBuff(
					const AosBuffPtr &buff,
					AosRundata *rdata);

};
#endif


