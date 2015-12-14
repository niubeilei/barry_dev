////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosThread.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_AosThread_h
#define Omn_Thread_AosThread_h



class OmnThread : public virtual OmnRCObject
{
	OmnDefineRCObject;

public:
	OmnMutexPtr				mMutex;
	OmnCondVarPtr			mCondVar;
	unsigned long			mThreadId;
	OmnString				mName;
	OmnThreadedObjPtr		mThreadObj;
	OmnThrdStatus::E		mThreadStatus;
	int						mLogicId;
	int						mErrSeqno;
	bool					mHighPriority;
	bool					mIsCriticalThread;
	OmnTraceEntry			mTraceEntry;
	bool					mIsManaged;

public:
	OmnThread(const OmnThreadedObjPtr &threadObj, 
			  const OmnString &name,
			  const int logicId,
			  const bool managedFlag,
			  const bool isCriticalThread,
			  const bool highPriority = false);
	~OmnThread();

	OmnString		getName() const {return mName;}
	OmnThreadIdType	getThreadId() const {return mThreadId;}
	int				getLogicId() const {return mLogicId;}
	bool			isCriticalThread() const {return mIsCriticalThread;}
	void			heartbeat();
    bool			checkThread(OmnString &err) const;
	void			setErrSeqno(const int s) {mErrSeqno = s;}
	void			setThreadId(const int tid) {mThreadId = tid;}
	OmnThrdStatus::E	getStatus() const {return mThreadStatus;}
	OmnTraceEntry &	getTraceEntry() {return mTraceEntry;}
	
	bool			isManaged() const {return mIsManaged;}

	OmnRslt			start();
	OmnRslt			stop();
	OmnRslt			exitThread();

private:
	static OmnThreadReturnType		msgThreadFunc(OmnThreadParmType obj);
	// static DWORD WINAPI msgThreadFunc(LPVOID obj);

	void		threadFunc();
};

#endif
