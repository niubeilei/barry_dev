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
#if 0
#ifndef AOS_DataScanner_DocScanner_h
#define AOS_DataScanner_DocScanner_h

#include "DataScanner/DataScanner.h"
#include "Util/Opr.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include <queue>

class AosDocScanner : public AosDataScanner, public OmnThreadedObj
{

	OmnMutexPtr     mLock;
	OmnCondVarPtr   mCondVar;
	OmnThreadPtr    mThread;
	OmnString 		mScannerId;
	int				mNumPhysicals;
	int				mServerId;
	AosBuffPtr		mPrimaryBuff;
	AosBuffPtr		mSecondaryBuff;
	bool			mNoMoreData;
	AosRundataPtr 	mRundata;


public:
	AosDocScanner(const bool flag);
	AosDocScanner(const bool flag, const AosRundataPtr &rdata);
	AosDocScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDocScanner();

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual bool   split(
						vector<AosDataScannerObjPtr> &scanners,
						const int64_t &record_len,
						const AosRundataPtr &rdata);
	virtual bool   getNextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual int64_t getTotalFileLength();

	void 	setScannerId(const OmnString &scanner_id) {mScannerId = scanner_id;}

private:
	bool	config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	
	bool	start(const AosRundataPtr &rdata);

	bool	createDocScanner(
				vector<AosDataScannerObjPtr> &scanners,
				const OmnString &scanner_id, 
				const AosRundataPtr &rdata);
	void	startThread(const AosRundataPtr &rdata);
};
#endif
#endif

