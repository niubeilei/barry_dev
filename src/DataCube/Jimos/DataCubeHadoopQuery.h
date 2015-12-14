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
// 2015/03/27 Created by Crystal Cao
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_DataCubeHadoopQuery_h
#define Aos_DataCube_DataCubeHadoopQuery_h

#include "DataCube/DataCube.h"
#include "HDFS/hdfs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/Ptrs.h"

#if 0

class AosDataCubeHadoopQuery : public AosDataCube, public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;
	enum
	{
		eMaxSize = 10000000 //10M
	};

private:
	AosBuffPtr                  mBuff;
	OmnString       			mIP;                      
	int             			mPort;                    
	hdfsFS 						mFS;
	AosDataConnectorCallerObjPtr mCaller;


public:
	AosDataCubeHadoopQuery(const u32 version);
	~AosDataCubeHadoopQuery();

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
	OmnString getRecordByDocid(u64& docid, AosRundata *rdata);
};
#endif

#endif
