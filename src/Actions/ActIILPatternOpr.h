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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SdocAction_ActIILPatternOpr_h
#define Aos_SdocAction_ActIILPatternOpr_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "IILUtil/IILUtil.h"
#include "IILEntryMap/IILEntryMapMgr.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "UtilTime/TimeFormat.h"



class AosActIILPatternOpr : virtual public AosSdocAction, public AosActionCaller 
{
private:
	enum
	{
		eMaxBuffSize = 100000000  //100M
		//eMaxBuffSize = 10000000  //10M
	};

	struct SnapInfo
	{
		u64						mSnapId;
		set<OmnString>			mIILNames;
	};

	OmnString					mType;
	OmnString					mIILName;
	OmnString					mMapIILName;
	OmnString					mSpeedIILName;
	OmnString					mMergeIILName;
	OmnString					mKeyOpr;
	AosIILEntryMapPtr			mSpeedMap;
	AosIILEntryMapPtr			mIILMap;
	AosTimeFormat::E    		mOrigTimeFormat;
	AosTimeFormat::E    		mTargetTimeFormat;
	int							mLen;
	AosIILUtil::AosIILIncType 	mIncType;
	u64							mInitdocid;
	AosDataScannerObjPtr		mScanner;
	AosRundataPtr				mRundata;
	u64							mFileId;
	int							mPhysicalId;
	int64_t 					mMaxLength;
	int64_t						mProcLength;
	int64_t						mCrtPos;
	int							mProcgress;

	bool						mIsTest;
	
	bool						mFinished;

	// Chen Ding, 2013/10/21
	u64							mMaxBuffSize;

	//Linda, 2014/03/12
	map<u32, u64>				mSnapMaps;
	u64							mSnapId;
	u64							mMergeSnapId;
	u64							mMergeSnapId2;
	u64							mMergeSnapId3;
	u64							mMergeSnapId4;
	u64							mTaskDocid;
	bool						mBuildBitmap;
public:
	AosActIILPatternOpr(const bool flag);
	AosActIILPatternOpr(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActIILPatternOpr();

	void	setTestFlag(const bool flag) {mIsTest = flag;}
	bool	isFinished() {return mFinished;}

	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
	
	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;

	virtual void callBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished);

	void	createIIL(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished);

	class AosCreateIIL: public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosActIILPatternOprPtr 	mCaller;
		u64 					mReqId;
		int64_t 				mExpectedSize;
		bool 					mFinished;
		
	public:
		AosCreateIIL(
			const AosActIILPatternOprPtr &caller,
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished)
		:
		OmnThrdShellProc("ActCreateIILPatternOprThrd"),
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

	bool	mergeIIL(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);
	
	bool	mergeIIL2(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);
	
	bool	getSnapShotId(
				const OmnString &iilname, 
				u64 &snapid,
				vector<AosTaskDataObjPtr> &snapshots,
				const AosTaskObjPtr &task,
				const AosRundataPtr &rdata);
};
#endif

#endif
