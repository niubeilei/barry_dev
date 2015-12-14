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
// 2014/09/24 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SdocAction_ActJimoTableBatchOpr_h
#define Aos_SdocAction_ActJimoTableBatchOpr_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "IILUtil/IILUtil.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"


class AosActJimoTableBatchOpr : virtual public AosSdocAction,
								public AosActionCaller 
{
private:
	enum
	{
		eMaxBuffSize = 100000000  //100M
		//eMaxBuffSize = 10000000  //10M
	};

	OmnString					mType;
	OmnString					mIILName;
	AosIILUtil::AosIILIncType 	mIncType;
	u64							mInitdocid;
	AosDataScannerObjPtr		mScanner;
	AosRundataPtr				mRundata;
	u64							mFileId;
	int							mPhysicalId;
	i64 						mMaxLength;
	i64							mProcLength;
	i64							mCrtPos;
	int							mProcgress;

	bool						mIsTest;
	
	bool						mFinished;

	// Chen Ding, 2013/10/21
	u64							mMaxBuffSize;

	//Linda, 2014/03/12
	u64							mSnapId;
	u64							mTaskDocid;
	bool						mBuildBitmap;

	AosXmlTagPtr				mCmpTag;

public:
	AosActJimoTableBatchOpr(const bool flag);
	AosActJimoTableBatchOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActJimoTableBatchOpr();

	void	setTestFlag(const bool flag) {mIsTest = flag;}
	bool	isFinished() {return mFinished;}

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

	virtual void callBack(
			const u64 &reqId, 
			const i64 &expected_size, 
			const bool &finished);

	void	createIIL(
			const u64 &reqId, 
			const i64 &expected_size, 
			const bool &finished);

	class AosCreateIIL: public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosActJimoTableBatchOprPtr 	mCaller;
		u64 					mReqId;
		i64 					mExpectedSize;
		bool 					mFinished;
		
	public:
		AosCreateIIL(
			const AosActJimoTableBatchOprPtr &caller,
			const u64 &reqId, 
			const i64 &expected_size, 
			const bool &finished)
		:
		OmnThrdShellProc("ActCreateIILBatchOprThrd"),
		mCaller(caller),
		mReqId(reqId),
		mExpectedSize(expected_size),
		mFinished(finished)
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};

	virtual bool createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata);

private:
	bool 	config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool	rollBackSnapShot(
				const u32 virtualid,
				const AosTaskDataObjPtr &task_data,
				const AosRundataPtr &rdata);
};
#endif

#endif
