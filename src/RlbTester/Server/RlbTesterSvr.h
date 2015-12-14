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
//
// Modification History:
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_Server_RlbTesterSvr_h
#define Aos_RlbTester_Server_RlbTesterSvr_h 

#include "FmtMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbTesterSvrObj.h"
#include "Thread/ThreadedObj.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include <queue>
using namespace std;


class AosRlbTesterSvr: public AosRlbTesterSvrObj,
					   public OmnThreadedObj 
{
	OmnDefineRCObject;

public:
	enum
	{
		eSendFmtThrdId = 1,
		eMaxCacheSize = 1 * 1000 * 1000,	// 1M. 
	};

private:
	OmnMutexPtr	 			mLock;
	OmnCondVarPtr			mCondVar;
	OmnThreadPtr			mSendFmtThrd;
	
	queue<AosFmtPtr>		mFmts;
	u64						mCacheSize;
	bool					mShowLog;

public:
	AosRlbTesterSvr();
	~AosRlbTesterSvr();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };
	
	virtual bool start();
	virtual bool createFile(
					const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const u32 cube_id,
					const u32 site_id, 
					const u64 requested_size,
					u64 &file_id,
					OmnString &fname);

	virtual bool modifyFile(
					const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const u64 file_id,
					const u64 offset,
					const AosBuffPtr &data);

	virtual bool deleteFile(
					const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const u64 file_id);

	virtual bool readData(
					const AosRundataPtr &rdata,
					const u64 file_id,
					const u64 offset,
					const u32 data_len,
					AosBuffPtr &data);

private:
	OmnString getPureFname(const OmnString &full_fname);
	bool 	sendFmtThrdFunc(
			  OmnThrdStatus::E &state,
			  const OmnThreadPtr &thread);

};
#endif
