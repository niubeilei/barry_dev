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
#ifndef Aos_DataCube_ReadDocUtil_ReadDocNoSortUnit_h
#define Aos_DataCube_ReadDocUtil_ReadDocNoSortUnit_h

#include "DataCube/DataCube.h"
#include "DataCube/Jimos/Ptrs.h"
#include "DataCube/ReadDocUtil/Ptrs.h"
#include "DataCube/ReadDocUtil/DocOpr.h"
#include "Thread/Mutex.h"
#include "SEUtil/AsyncRespCaller.h"

class AosReadDocNoSortUnit : public AosDataCube, public AosAsyncRespCaller
{
	OmnDefineRCObject;

	enum
	{
		eEachNumPerRead = 1000
	};

private:
	AosDocOprPtr	mDocOpr;
	u32				mDistId;
	vector<u64>		mDocids;
	u32				mReadedNum;
	AosDataConnectorCallerObjPtr		mCaller;

public:
	AosReadDocNoSortUnit();
	AosReadDocNoSortUnit(
			const AosDocOprPtr &doc_opr,
			const u32 dist_id,
			IdVector &docids);
	~AosReadDocNoSortUnit();

	// Jimo	Interface
	virtual bool config(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc);
	virtual AosJimoPtr cloneJimo() const;
	
	// DataConnector Interface
	virtual AosDataConnectorObjPtr cloneDataConnector();
	virtual void setCaller(const AosDataConnectorCallerObjPtr &caller);
	virtual bool readData(const u64 reqid, const AosRundataPtr &rdata);
	
	// AsyncRespCaller Interface
	virtual void callback(
					const AosTransPtr &trans, 
					const AosBuffPtr &resp, 
					const bool svr_death);
	
	static void include();

private:
	bool	 isReadFinish();

};
#endif

