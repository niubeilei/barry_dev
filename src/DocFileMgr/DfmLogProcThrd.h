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
// 06/18/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmLogProcThrd_h
#define AOS_DocFileMgr_DfmLogProcThrd_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocFileMgr/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Thread/ThreadedObj.h"
#include <queue>


class AosDfmLogProcThrd: public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxThrds = 6,
		eMaxFileSize = 1000000000 // 100M
	};

	struct DfmLogReq
	{
		AosRundataPtr  	mRdata;
		AosDfmLogPtr 	mDfmLog;
		u64				mFileId;

		DfmLogReq(const AosDfmLogPtr &dl, const u64 &ff, const AosRundataPtr &r)
		:
		mRdata(r->clone(AosMemoryCheckerArgsBegin)),
		mDfmLog(dl),
		mFileId(ff)
		{
		}

		~DfmLogReq()
		{
		}
	};

	struct StopCache
	{
		bool stop_proc_log;
		bool finish_crt_log;

		void reset(const bool s, const bool f)
		{
			stop_proc_log = s;
			finish_crt_log = f;
		}
	};



private:
	OmnMutexPtr			mLock[eMaxThrds];
	OmnCondVarPtr		mCondVar[eMaxThrds];
	OmnThreadPtr    	mThread[eMaxThrds];
	queue<DfmLogReq> 	mQueue[eMaxThrds]; 
	int64_t				mMaxFileSize;
	u32					mNumThrds;
	StopCache			mStopCache[eMaxThrds];
	bool				mShowLog;

public:
	AosDfmLogProcThrd();
	~AosDfmLogProcThrd();

    bool		config(const AosXmlTagPtr &def);

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool 	stop();

	u32 	getMaxFileSize(){return mMaxFileSize;}

	bool	addRequest(
				const AosRundataPtr &rdata,
				const AosDfmLogPtr &dfmlog,
				const u64 &fileid);

	bool stopProcCache();

	bool continueProcCache();

	bool flushContentsByVirtualId(const u32 virtual_id);
};

#endif
