/////////////////////////////////////////////////////////////// 
// Created by:	Open Source Society 
// Created:		10/3/2000 
// Comments: 
// 
// Change History: 
//	10/03/2000 - File created 
// 
/////////////////////////////////////////////////////////////// 
#ifndef Omn_Thread_ThreadShell_h 
#define Omn_Thread_ThreadShell_h 
 
#include "Event/EventId.h" 
#include "Event/EventListener.h" 
#include "Event/Ptrs.h" 
#include "Message/Ptrs.h" 
#include "Porting/ThreadDef.h" 
#include "Thread/Ptrs.h" 
#include "Thread/ThreadedObj.h" 
#include "Util/ValList.h" 
#include "Util/RCObjImp.h" 
#include "Util/BasicTypes.h" 
#include "UtilComm/Ptrs.h" 
#include "UtilComm/ConnBuff.h" 
#include "Util/a_string.h"

#include <string>

class OmnCntlVar;

class OmnThreadShell : public OmnThreadedObj
//class OmnThreadShell : public OmnThreadedObj,
					   //public AosEventListener
{
	OmnDefineRCObject;

public:
	enum State
	{
		eStateIdle,
		eStateStopped,
		eStateActive
	};

	/*
	struct Entry
	{
		u32 			mTransId;
		OmnConnBuffPtr	mBuff;

		Entry(const OmnConnBuffPtr &buff, const u32 transId)
		:
		mTransId(transId),
		mBuff(buff)
		{
		}

		Entry()
		:
		mTransId(0)
		{
		}
	};
	*/

private:
	//OmnMutexPtr 		mInLock;
	//OmnCondVarPtr		mInCondVar;
	OmnMutexPtr			mCntlLock;
	OmnCondVarPtr		mCntlCondVar;
	OmnThrdShellProcPtr	mProcessor;
	//OmnVList<Entry>		mInQueue;
	OmnString			mName;
	State 				mState;
	void *				mUserData;	// set and used by caller
	OmnThreadPtr		mThread;
	//int					mEventType;

	//OmnMutexPtr			mEventLock;
	//OmnCondVarPtr		mEventCondVar;
	//OmnVList<AosEventPtr>	mEventQueue;
	//u32					mTransIdWaitOn;
	u32					mStartTime;

	// Ken Lee, 2013/04/12
	OmnThreadShellCallerPtr	mCaller;

	OmnString			mFileName; //felicia, 2013/06/28
	int					mFileLine; //felicia, 2013/06/28

	// Do not use the two:
	OmnThreadShell(const OmnThreadShell &rhs);
	OmnThreadShell & operator = (const OmnThreadShell &rhs);

public:
	OmnThreadShell(
			const OmnString &name, 
			const OmnString &file_name,
			const int file_line,
			const OmnThreadShellCallerPtr &caller); 
	~OmnThreadShell(); 
 
	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, 
							   const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// EventListener Interface
	//virtual bool		procEvent(const AosEventPtr &event, bool &);
	//virtual OmnString	getListenerName() const {return mName;}

	bool		isActive() const {return mState==eStateActive;}
	bool		startProcess(const OmnThrdShellProcPtr &processor);
	//bool		getTrans(const OmnThrdTransPtr &trans, const u32 timer);
	//bool		peekTrans(OmnThrdTransPtr &trans);
	//void		clear();
	void		setUserData(void *v) {mUserData = v;}
	void *		getUserData() const {return mUserData;}
	//bool		waitOn(const AosEventId::E eventId, 
	//					   AosEventPtr &event, 
	//					   const int ttlSec, 
	//					   bool &isTimedout);
	//void		signalEvent(const AosEventPtr &event);
	//void		resetEventType() {mEventType = -1;}
	//bool		waitForMsg(const OmnThrdTransPtr &trans, const u32 timer);
	//bool		msgRcved(const OmnConnBuffPtr &buff, const u32 transId);
	//bool		isWaitingFor(const u32 transId) const 
	//			{return mTransIdWaitOn == transId;}
	//void		setWaitForTransId(const u32 transId);
	//u32			getProcLength() const;
	//OmnString	getProcName() const;
	//bool		isFinished() const;

private:
	static OmnThreadReturnType staticThreadFunc(OmnThreadParmType);
};
#endif

