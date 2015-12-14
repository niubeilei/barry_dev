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
#ifndef Aos_SdocAction_ActImportDocCSV_h
#define Aos_SdocAction_ActImportDocCSV_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "DataAssembler/DataAssemblerType.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/SengineImportDocObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Sem.h"
#include "Rundata/Ptrs.h"
#include <queue>
#include <map>
#include "Debug/Debug.h"


class AosActImportDocCSV : virtual public AosSdocAction,
						   virtual public AosSengineImportDocObj,
						   public AosActionCaller
{
	enum
	{
		eMaxBlockSize = 100000000 //100M
		//eMaxBlockSize = 10000000 //10M
	};

	struct Info
	{
		u32			sizeid;
		int			record_size;
		OmnString	key;
		bool		finished;
		u64			fileid;
		int			serverid;
		AosBuffPtr	buff;

		Info()
		:
		sizeid(0),
		record_size(-1),
		key(""),
		finished(false),
		fileid(0),
		serverid(-1),
		buff(0)
		{
		}

		Info(const u32 s, 
			 const int r, 
			 const OmnString &k,
			 const u64 &f,
			 const int i)
		:
		sizeid(s),
		record_size(r),
		key(k),
		finished(false),
		fileid(f),
		serverid(i),
		buff(0)
		{
		}

		~Info()
		{
		}
	};

	struct Req
	{
		u64 reqId;
		AosBuffPtr buff;
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
			OmnTagFuncInfo << endl;
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

	OmnMutexPtr     				mLock;
	OmnCondVarPtr   				mCondVar;
	OmnSemPtr               		mSem;
	int								mTotalReqs;
	int								mSaveDocReqs;
	int								mBlockSize;
	map<int, u64>					mSnapMaps;
	u64								mTaskDocid;

public:
	AosActImportDocCSV(const bool flag);
	~AosActImportDocCSV();

	// ActionCaller Interface
	virtual void callBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished); 

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual bool runInMem(
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
	bool	sanityCheck();

	bool  readFileFinishedLocked();

	void  deleteFile();
};
#endif

#endif
