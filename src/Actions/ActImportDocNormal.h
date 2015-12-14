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
#ifndef Aos_SdocAction_ActImportDocNormal_h
#define Aos_SdocAction_ActImportDocNormal_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/DataAssemblerType.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/SengineImportDocObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Sem.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <map>


class AosActImportDocNormal : virtual public AosSdocAction,
							 virtual public AosSengineImportDocObj,
							 public OmnThreadedObj,
							 public AosActionCaller
{
	enum
	{
		eMaxBlockSize = 200000000 //200M
		//eMaxBlockSize = 20000000 //20M
	};

	struct Info
	{
		bool finished;
		u64 fileid;
		int serverid;

		Info()
		:
		finished(false),
		fileid(0),
		serverid(-1)
		{
		}

		Info(const u64 &f,
			const int i)
		:
		finished(false),
		fileid(f),
		serverid(i)
		{
		}

		~Info()
		{
		}
	};

	struct Req
	{
		u64 reqId;
		int64_t blocksize;
	};


	struct FinishAllReq : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		const AosSengineImportDocObjPtr mCaller;

		FinishAllReq(const AosSengineImportDocObjPtr &caller)
		:
		OmnThrdShellProc("FinishAllReq"),
		mCaller(caller)
		{
		}
		
		bool run()                
		{
			mCaller->allFinished();
			return true;
		}
		bool procFinished()
		{
			return true;
		}
	};

	AosDataScannerObjPtr			mScanner;
	AosRundataPtr					mRundata;
	map<u64, Info>					mInfo;
	queue<Req>						mQueue;

	OmnThreadPtr					mThread;
	OmnMutexPtr     				mLock;
	OmnCondVarPtr   				mCondVar;
	OmnSemPtr               		mSem;
	int								mTotalReqs;
	bool							mExitThrd;
	map<int, u64>					mSnapMaps;
	u64								mTaskDocid;

public:
	AosActImportDocNormal(const bool flag);
	~AosActImportDocNormal();

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

	virtual void allFinished();
	virtual void semPost();
	virtual AosBuffPtr	getNextBlock(
					const u64 &reqId,
					const int64_t &expected_size);

	virtual bool createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata);
	//virtual bool actionFailed(const AosRundataPtr &rdata);
	virtual bool storageEngineError(const AosRundataPtr &rdata);
private:
	bool  readFileFinishedLocked();

	void  deleteFile();
};
#endif

