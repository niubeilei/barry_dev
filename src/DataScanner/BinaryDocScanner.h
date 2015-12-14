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
#ifndef AOS_DataScanner_BinaryDocScanner_h
#define AOS_DataScanner_BinaryDocScanner_h

#include "DataScanner/DataScanner.h"
#include "Util/Opr.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "SEUtil/AsyncRespCaller.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>
#include <queue>

class AosBinaryDocScanner : virtual public AosDataScanner, public AosAsyncRespCaller 
{
	OmnDefineRCObject;

	enum
	{
		eLimitMaxNum = 10 
	};
	OmnMutexPtr 				    mLock;
	OmnCondVarPtr  				 	mCondVar;
	int								mServerId;
	deque<u64>						mDocids;
	queue<AosBuffPtr>				mQueue;
	u32								mReadingNum;
	u32								mLimitMaxNum;
	int64_t							mTotalSize;
	int								mTotalNum;

public:
	AosBinaryDocScanner(const bool flag);
	// AosBinaryDocScanner(const bool flag, const AosRundataPtr &rdata);
	AosBinaryDocScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosBinaryDocScanner();

	virtual bool   getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);

	virtual AosJimoPtr cloneJimo()  const;
	virtual AosDataScannerObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	// virtual int64_t getTotalFileLength();
	virtual int64_t getTotalSize() const;
	virtual	bool initBinaryDocScanner(    
					const vector<u64> &docids,       
					const int physical_id,         
					const AosRundataPtr &rdata);   
	virtual int getPhysicalId() const;

	bool	getNextBlock(AosBuffDataPtr &info, const AosRundataPtr &rdata);

	bool	readData(const int readnum,
				 const AosRundataPtr &rdata);
	bool	signal(const int threadLogicId);

	// Ketty 2013/07/20
	virtual void 	callback(
				const AosTransPtr &trans,
				const AosBuffPtr &resp,
				const bool svr_death);

private:
	bool	config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	
	bool	createBinaryDocScanner(
				vector<AosDataScannerObjPtr> &scanners,
				const AosRundataPtr &rdata);
};
#endif

