////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SdocAction_ActImportStatDoc_h
#define Aos_SdocAction_ActImportStatDoc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataStructs/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <map>

class AosActImportStatDoc : virtual public AosSdocAction, public OmnThreadedObj, public AosActionCaller
{
	enum
	{
		eMaxBlockSize = 200000000 //200M
	};

	struct Req
	{
		u64 	reqId;
		int64_t expectedSize;
		bool	finished;
	};

	AosDataScannerObjPtr			mScanner;
	bool							mFinished;
	AosRundataPtr					mRundata;

	OmnMutexPtr     				mMainLock;
	OmnCondVarPtr   				mMainCondVar;
	queue<u64>						mReqIds;

	bool							mExitThrd;
	AosVector2DPtr					mVector2D;
	OmnThreadPtr					mThread;

	OmnMutexPtr     				mLock;
	OmnCondVarPtr   				mCondVar;
	queue<Req>						mReq;

	AosDataAssemblerObjPtr			mNewIILAsm;
	AosDataAssemblerObjPtr			mOldIILAsm;
	int								mRecordLen;

public:
	AosActImportStatDoc(const bool flag);
	~AosActImportStatDoc();

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// ActionCaller Interface
	virtual void callBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished); 

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

	bool allFinished();

private:
	bool modifyLocal(const Req &req);

	bool configAsm(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata);

	bool configFileInfo(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata);

	bool configVector2D(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata);

	bool configRecordLen(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata);
};
#endif
#endif

