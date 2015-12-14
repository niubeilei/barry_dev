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
#ifndef Aos_DataCube_DataCubeHbase_h
#define Aos_DataCube_DataCubeHbase_h

#include "DataCube/DataCube.h"
#include "DataCube/ReadDocUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/QueryReqObj.h"	

#if 0
#include "HBase/HBaselib/Hbase.h"
using namespace apache::hadoop::hbase::thrift;

class AosDataCubeHbase : public AosDataCube, public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;
	
private:
	AosBuffPtr 					mBuff;
	AosDataConnectorCallerObjPtr		mCaller;
	boost::shared_ptr<HbaseClient>	mClient;
	OmnString					mTname;

public:
	AosDataCubeHbase(const u32 version);
	~AosDataCubeHbase();

	// Jimo	Interface
	virtual bool	config(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;
	
	// DataCube Interface
	virtual bool 	start(AosRundata *rdata);
	virtual void 	setCaller(const AosDataConnectorCallerObjPtr &caller);
	virtual bool	readData(const u64 reqid, AosRundata *rdata);
	virtual AosDataConnectorObjPtr cloneDataConnector();
	
	// AosDataCubeCallerObj Interface
	virtual void callBack(
			const u64 &reqid,
			const AosBuffDataPtr &buff, 
			bool finished);

	bool 	setValueBuff(
				const AosBuffPtr &buff,
				AosRundata *rdata);
private:
	OmnString getRecordByRowid(OmnString rowid);
};
#endif
#endif

