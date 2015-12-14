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
#if 0
#ifndef Aos_DataCube_DataCubeBuff_h
#define Aos_DataCube_DataCubeBuff_h

#include "DataCube/DataCube.h"

class AosDataCubeBuff : public AosDataCube, public AosDataConnectorCallerObj 
{
	OmnDefineRCObject;
	
private:
	AosBuffPtr 					mBuff;
	OmnString					mDataId;
	AosDataConnectorCallerObjPtr		mCaller;

public:
	AosDataCubeBuff(const u32 version);
	~AosDataCubeBuff();

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
	
	// AosDataConnectorCallerObj Interface
	virtual void callBack(
			const u64 &reqid,
			const AosBuffDataPtr &buff, 
			bool finished);

	bool 	setValueBuff(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);
};
#endif

#endif
