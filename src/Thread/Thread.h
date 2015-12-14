////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Thread.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_Thread_h
#define Omn_Thread_Thread_h

#include "Debug/Rslt.h"
#include "Porting/ThreadDef.h"
#include "Rundata/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdStatus.h"
#include "Thread/Ptrs.h"
#include "Tracer/TraceEntry.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/SPtr.h"
#include "Util/String.h"
#include "Util/LogBag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#define AosThreadRegisterAction(timer, action) OmnThreadMgr::getSelf()->registerThreadAction(__FILE__, __LINE__, timer, action)
#define AosThreadUnregisterAction(action) OmnThreadMgr::getSelf()->unregisterThreadAction(action)
#define AosThreadActionLog(thrd, log) if (thrd) {thrd->appendActionLog(__FILE__, __LINE__, log);}
#define AosThreadSetLineno(thrd) if (thrd) {thrd->setLineno(__LINE__);}

class OmnThread : public virtual OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxLogLength = 10000,
		eRemoveLogLength = 3000
	};

private:
	OmnMutexPtr				mMutex;
	OmnCondVarPtr			mCondVar;
	OmnMutexPtr				mMutexWait;
	OmnCondVarPtr			mCondVarWait;
	OmnThreadIdType 		mThreadId;
	OmnString				mName;
	OmnThreadedObjPtr		mThreadObj;
	OmnThrdStatus::E		mThreadStatus;
	int						mLogicId;
	int						mErrSeqno;
	bool					mHighPriority;
	bool					mIsCriticalThread;
	OmnTraceEntry			mTraceEntry;
	bool					mIsManaged;
	void *					mUserData;
	AosBuffPtr				mResponse;		// Ketty 2012/07/27
	//u64					mTransid;
	AosTransId				mTransid;
	bool					mNeedWait;		// Ketty 2012/12/21
	OmnString				mActionFname;		// Chen Ding, 10/15/2011
	int						mActionLineno;		// Chen Ding, 10/15/2011
	int						mActionTimer;		// Chen Ding, 10/15/2011
	int						mActionStart;		// Chen Ding, 10/15/2011
	bool					mErrorReported;		// Chen Ding, 10/15/2011
	OmnString				mAction;			// Chen Ding, 10/15/2011
	OmnString				mActionLog;			// Chen Ding, 10/15/2011
	int						mThrdId;			// Chen Ding, 10/07/2011
	int						mLineno;
	OmnString				mLog;			// Chen Ding, 2013/05/27
	OmnString				mFileName;		// felicia, 2013/06/28
	int						mFileLine;		// felicia, 2013/06/28

public:
	OmnThread(const OmnThreadedObjPtr &threadObj, 
			  const OmnString &name,
			  const int logicId,
			  const bool managedFlag,
			  const bool isCriticalThread,
			  const OmnString &file_name,
			  const int	file_line,
			  const bool highPriority = false);

	OmnThread(
			  const unsigned long tid, 
			  const OmnString &name,
			  const OmnString &file_name,
			  const int file_line);
	~OmnThread();

	OmnString		getName() const {return mName;}
	OmnString		getFileName() const {return mFileName;}
	int				getFileLine() const {return mFileLine;}
	u64				getThreadId() const {return mThreadId;}
	int				getLogicId() const {return mLogicId;}
	bool			isCriticalThread() const {return mIsCriticalThread;}
	void			heartbeat();
	bool			signal();
    bool			checkThread(OmnString &err) const;
	void			setErrSeqno(const int s) {mErrSeqno = s;}
	void			setThreadId(const u64 tid) {mThreadId = tid;}
	OmnThrdStatus::E	getStatus() const {return mThreadStatus;}
	OmnTraceEntry &	getTraceEntry() {return mTraceEntry;}
	bool			isManaged() const {return mIsManaged;}
	OmnRslt			start();
	OmnRslt			stop();
	int 			cancel();
	bool			isStopped();
	bool			isExited();
	OmnRslt			exitThread();

	//felicia, 2014/04/01
	void			setUdata(void* udata){mUserData = udata;}
	void*			getUdata(){return mUserData;}

	// Ketty 2012/04/18
	//void 	setResponse(const AosBuffPtr &response, const u64 &trans_id);
	void 	setResponse(const AosBuffPtr &response, const AosTransId &trans_id);
	AosBuffPtr getResponse(
			const u32 time_sec, 
			const u32 time_usec,
			bool &timedout,
			//const u64 &trans_id);
			const AosTransId &trans_id);

	// Ketty 2012/10/17
	bool	wait(
			const u32 time_sec, 
			const u32 time_usec,
			bool &timedout,
			//const u64 &trans_id);
			const AosTransId &trans_id);
	//bool 	wakeup(const u64 trans_id);
	//void    setWaitTransId(const u64 trans_id);
	bool 	wakeup(const AosTransId &trans_id);
	void    setWaitTransId(const AosTransId &trans_id);

	// Chen Ding, 2013/05/27
	// void appendLog(
	// 		const char *file,
	// 		const int line, 
	// 		const OmnString &str);

	// Chen Ding, 2013/05/27
	OmnString& getLog() {return mLog;}

private:
	static OmnThreadReturnType		msgThreadFunc(OmnThreadParmType obj);
	// static DWORD WINAPI msgThreadFunc(LPVOID obj);
	void		threadFunc();

public:
	// Chen Ding, 10/15/2011
	bool registerAction(
			const char *file, 
			const int line, 
			const int timer,
			const OmnString &action);
	bool unregisterAction(const OmnString &action);
	void appendActionLog(const char *file, const int line, const OmnString &log);
	void resetActionLog();
	void setLineno(const int line) {mLineno = line;}
};

#endif
