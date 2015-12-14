////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_StreamData_h
#define AOS_StreamEngine_StreamData_h

#include "StreamEngine/StreamData.h"
#include "StreamEngine/RDD.h"
#include "SEInterfaces/DataProcObj.h"
#include "Dataset/Dataset.h"
#include "JSON/JSON.h"
#include "JSON/value.h"
#include <time.h>
#include "StreamEngine/Ptrs.h"
#include "StreamEngine/StreamPolicy.h"

class AosStreamDataProc;
class AosRecordsetObj;

class AosStreamData : virtual public OmnRCObject,
						public OmnThreadedObj
{
	OmnDefineRCObject;
	
	enum
	{
		eDefaultInterval = 0, 			//0s
		eMaxCacheSize = 2000000000, 		//2G
		eMaxRecordset = 10000
		//eMaxCacheSize = 5000000 		//testing
	};

private:
	u64								mStartTime;
	int								mTotalProcs;
	u64								mServiceDocid;
	u64								mCurRDDId;
	u64								mInterval;
	i64								mMaxCacheSize;
	i64								mRecvDataLen;
	OmnString						mDataId;
	OmnString						mName;
	OmnString						mDataProcName;
	OmnString						mServiceId;
	AosStreamDataProc*				mDataProc;
	AosStreamPolicyPtr				mPolicy;
	AosRundataPtr 					mRundata;
	OmnMutexPtr						mLock;
	OmnCondVarPtr					mCondVar;
	OmnThreadPtr					mThread;
	deque<AosRDDPtr>				mWaitRDDs;	
	map<u64, AosRDDPtr>				mStartRDDs;
	map<OmnString, OmnString>		mFieldMap;
	AosRecordsetObjPtr				mRecord_set;

public:
	AosStreamData(
			const AosRundataPtr &rdata,
			const OmnString &dataset_name,
			const OmnString &service_name);

	~AosStreamData();

	// ThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	//void setDataId(const OmnString &dataId) { mDataId = dataId; }
	bool feedStreamData(const AosRundataPtr &rdata, const AosRecordsetObjPtr &rs);
	bool feedStreamData(AosRundata *rdata,AosDataRecordObjPtr &rcd);
	bool updateRDDStatus(const AosTaskStatus::E status, const OmnString &dp_name, const u64 &rddid, const int remain, const AosRundataPtr &rdata);
	static AosStreamDataPtr getStreamData(AosRundata *rdata, const OmnString &dataset_name, const OmnString &service_name);
	bool setInterval(const u64 &val);
	bool setMaxCacheSize(const i64 &max_cache_size);
	bool finishFeedStreamData(AosRundata *rdata);

private:
	u64	getCrtRDDId();
	bool sendRDD(const AosRDDPtr &rdd);
	bool findDataFlow(const JSONValue &dataflow, const OmnString &fname);
};
#endif
