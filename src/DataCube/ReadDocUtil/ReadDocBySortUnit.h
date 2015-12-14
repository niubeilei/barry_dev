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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_ReadDocUtil_ReadDocBySortUnit_h
#define Aos_DataCube_ReadDocUtil_ReadDocBySortUnit_h

#include "DataCube/DataCube.h"
#include "DataCube/Jimos/Ptrs.h"
#include "DataCube/ReadDocUtil/DocOpr.h"
#include "Thread/Mutex.h"
#include "SEUtil/AsyncRespCaller.h"

#include <set>	
using namespace std;

class AosReadDocBySortUnit : public AosDataCube, public AosAsyncRespCaller
{
	OmnDefineRCObject;
	
	//struct DocidGrp
	//{
	
	//};

private:
	u64					mReqId;
	bool				mReaded;
	AosDocOprPtr		mDocOpr;
	IdVector			mTotalDocids;
	map<u32, IdVector>	mSvrIdDocidsMap;
	set<u64>			mRespRecved;
	map<u64, AosBuffPtr> mAllResps;
	AosDataConnectorCallerObjPtr		mCaller;
	
	AosBuffPtr			mMetaResp;
	OmnMutexPtr			mLock;

public:
	AosReadDocBySortUnit();
	AosReadDocBySortUnit(
		const AosRundataPtr &rdata,
		const AosDocOprPtr &doc_opr,
		IdVector &docids);
	~AosReadDocBySortUnit();

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
	bool	isReadFinish();
	bool 	markRespRecved(const u64 seq);
	bool 	cacheDocidsResp(const AosBuffPtr &big_resp);
	bool 	isAllRespRecved();
	AosBuffPtr getAllResp();

};
#endif

