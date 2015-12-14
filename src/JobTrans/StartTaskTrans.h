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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_StartTaskTrans_h
#define Aos_JobTrans_StartTaskTrans_h

#include "TransUtil/TaskTrans.h"
#include "SEInterfaces/TaskObj.h"

class AosStartTaskTrans : virtual public AosTaskTrans
{
private:
	u64			mTaskDocid;

	class StartTaskThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		u64 				mTaskDocid;
		AosRundataPtr       mRundata;
		public:
		StartTaskThrd(
				const u64 task_docid,
				const AosRundataPtr &rdata)
			:
				OmnThrdShellProc("StartTaskThrd"),
				mTaskDocid(task_docid),
				mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		~ StartTaskThrd() 
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};

	class RunTaskThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		u64 				mTaskDocid;
		OmnSemPtr			mSem;
		AosRundataPtr       mRundata;
		public:
		RunTaskThrd(
				const u64 task_docid,
				const OmnSemPtr &sem,
				const AosRundataPtr &rdata)
			:
				OmnThrdShellProc("RunTaskThrd"),
				mTaskDocid(task_docid),
				mSem(sem),
				mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		~ RunTaskThrd()
		{
		}
		virtual bool run();
		virtual bool procFinished();
		
	};

	class FinishTaskThrd: public OmnThrdShellProc
	{
		OmnDefineRCObject;
		enum
		{
			eReportWaitTime = 1000*30		//30s
		};

		u64					mTaskDocid;
		OmnSemPtr			mSem;
		u32					mThrdNum;
		AosRundataPtr       mRundata;

		public:
		FinishTaskThrd(
				const u64 task_docid,
				const OmnSemPtr &sem,
				const u32 thrd_num,
				const AosRundataPtr &rdata)
			:
				OmnThrdShellProc("FinishTaskThrd"),
				mTaskDocid(task_docid),
				mSem(sem),
				mThrdNum(thrd_num),
				mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		~ FinishTaskThrd()
		{
		}
		virtual bool run();
		virtual bool procFinished();
	};

public:
	AosStartTaskTrans(const bool regflag);
	AosStartTaskTrans(
			const u64 &task_docid,
			const int svr_id,
			const u32 to_proc_id);
	~AosStartTaskTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual OmnString toString();
private:
	bool startTask(const AosRundataPtr &rdata);

};
#endif

