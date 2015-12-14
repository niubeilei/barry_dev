////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_IILDataScanner_h
#define AOS_DataScanner_IILDataScanner_h

#include "DataScanner/DataScanner.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>
#include <queue>

class AosIILDataScanner :public AosDataScanner , public AosAsyncRespCaller 
{
	OmnDefineRCObject;

	enum
	{
		eLimitMaxNum = 1,
		eDftBlockSize = 1000000
	};
	
	OmnMutexPtr 				    mLock;
	OmnCondVarPtr  				 	mCondVar;
	int								mServerId;
	deque<u64>						mDocids;
	queue<AosBuffPtr>				mQueue;
	u32								mReadingNum;
	u32								mLimitMaxNum;
	u32								mBlockSize;
	int64_t							mTotalEntries;
	int64_t							mTotalReadEntries;
	OmnString						mIILName;
	AosRundataPtr					mRundata;

public:
	AosIILDataScanner(const bool flag);
	AosIILDataScanner(const bool flag, const AosRundataPtr &rdata);
	AosIILDataScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosIILDataScanner();

	virtual bool   getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	// Chen Ding, 2013/11/25
	// virtual int64_t getTotalFileLength();
	virtual int64_t getTotalSize() const;
	virtual	bool initIILDataScanner(    
					const AosXmlTagPtr &conf,
					const AosRundataPtr &rdata);   
	virtual AosJimoPtr cloneJimo()  const;
	virtual int getPhysicalId() const;

//	bool	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);

	bool	readData(
				const AosQueryContextObjPtr &query_context,
				const AosQueryRsltObjPtr &query_rslt,
				const AosRundataPtr &rdata);
	
	bool	signal(const int threadLogicId);

	virtual void 	callback(
				const AosTransPtr &trans,
				const AosBuffPtr &resp,
				const bool svr_death);
	//void	callback(AosAsyncReqTransPtr &trans);

private:
	bool	config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	
	bool	createIILDataScanner(
				vector<AosDataScannerObjPtr> &scanners,
				const AosRundataPtr &rdata);
	
	AosXmlTagPtr	getXmlFromBuff(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
};
#endif

