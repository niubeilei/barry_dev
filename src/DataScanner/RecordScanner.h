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
#ifndef AOS_DataScanner_RecordScanner_h
#define AOS_DataScanner_RecordScanner_h

#include "DataScanner/DataScanner.h"
#include "Util/Opr.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/Ptrs.h"
#include <vector>
#include <queue>

class AosRecordScanner :public AosDataScanner 
{
	OmnDefineRCObject;

	OmnMutexPtr 				    mLock;
	AosDataScannerObjPtr			mScanner;
	bool							mIsFinished;
	AosBuffPtr						mBuff;
	int								mRcdLen;
	int								mStatus;
	AosXmlTagPtr					mRecordConf;
	int 							mPos;

public:
	AosRecordScanner(const bool flag);
	AosRecordScanner(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosRecordScanner();

	virtual bool   getNextRecord(AosDataRecordObjPtr &record, const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual	bool initRecordScanner(    
					const AosXmlTagPtr &conf,
					const AosRundataPtr &rdata);   
	virtual AosJimoPtr cloneJimo()  const;
	virtual int getPhysicalId() const;
private:
	bool	config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool	getRecord(
				vector<AosXmlTagPtr> &records,
				const AosRundataPtr &rdata);
};
#endif

