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
// 2015/01/05	Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_NotifyTaskMgrGetDataColOutput_h
#define Aos_JobTrans_NotifyTaskMgrGetDataColOutput_h

#include "TransUtil/TaskTrans.h"
#include "TaskUtil/Ptrs.h"


class AosNotifyTaskMgrGetDataColOutput : virtual public AosTaskTrans
{
private:
	OmnString 		mDataColId;
	u64				mJobDocid;
	u64				mTaskDocid;

private:
	class GetFileListThrd: public OmnThrdShellProc
	{
		OmnDefineRCObject;
	private:
		OmnString							mDataColId;
		u64									mJobDocid;
		u64									mTaskDocid;
		AosTransPtr							mTrans;
		AosRundataPtr 						mRdata;
	public:
		GetFileListThrd(
				const OmnString &datacol_id,
				const u64 &job_docid,
				const u64 &task_docid,
				const AosTransPtr &trans,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("GetFileListThread"),             
		mDataColId(datacol_id),
		mJobDocid(job_docid),
		mTaskDocid(task_docid),
		mTrans(trans),
		mRdata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(){return true;} 
	};


public:
	AosNotifyTaskMgrGetDataColOutput(const bool regflag);
	AosNotifyTaskMgrGetDataColOutput(
			const OmnString &datacol_id,
			const u64 &job_id,
			const u64 &task_docid,
			const int svr_id);
	~AosNotifyTaskMgrGetDataColOutput();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

