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
#ifndef Aos_DataCube_DataCubeReadDocByFilter_h
#define Aos_DataCube_DataCubeReadDocByFilter_h

#include "DataCube/DataCube.h"
#include "DataCube/ReadDocUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/QueryReqObj.h"	

class AosDataCubeReadDocByFilter : public AosDataCube, public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;
	
private:
	u32					mCrtReadIdx;	
	AosDocOprPtr 		mOpr;
	AosQueryReqObjPtr   mQueryReq;
	OmnString		mCondStr;
	vector<AosReadDocBySortUnitPtr>	mDocReaderGrp;
	AosDataConnectorCallerObjPtr		mCaller;

public:
	AosDataCubeReadDocByFilter(const u32 version);
	~AosDataCubeReadDocByFilter();

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;
	
	// DataCube Interface
	virtual bool 	start(const AosRundataPtr &rdata);
	virtual void 	setCaller(const AosDataConnectorCallerObjPtr &caller);
	virtual bool	readData(const u64 reqid, const AosRundataPtr &rdata);
	virtual AosDataConnectorObjPtr cloneDataConnector();
	
	// AosDataCubeCallerObj Interface
	virtual void callBack(
			const u64 &reqid,
			const AosBuffDataPtr &buff, 
			bool finished);

	bool 	setValueBuff(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);

private:
	bool 	splitDocidsBySize(const AosRundataPtr &rdata, vector<u64> &docids);
	bool 	getDocidsByConf(
				const AosXmlTagPtr &conf,
				vector<u64> &docids);
	
	bool 	isReadFinish();
	bool 	addReadUnit(
				const AosRundataPtr &rdata,
				AosDocOprPtr &opr,
				vector<u64> &docids);

};
#endif

