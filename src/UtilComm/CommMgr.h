////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommMgr.h
// Description:
//	This is a singleton class that manages the entire application's 
//  sending and receiving over TCP/UDP.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_CommMgr_h
#define Omn_UtilComm_CommMgr_h

#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "NMS/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/CondVar.h"
#include "Thread/Ptrs.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"


class OmnCommMgr : public virtual OmnRCObject 
{
	OmnDefineRCObject;

private:
	enum E
	{
		eContinueToTry,		// Continue trying reconnecting
		eAbortTry,			// Abort trying reconnecting
	};

	enum 
	{
		eReadTimerSec = 2,
		eReadFailIntervalTimerSec = 30,

		eCommRestoreThreadId = 10
	};


	OmnVList<OmnCommPtr>		mComms;	// List of communicators

//	OmnMutexPtr					mMgcpQueueLock;
//	OmnCondVarPtr				mMgcpQueueCondVar;
//	OmnVList<OmnMgcpMsgPtr>	mMgcpQueue;

//	OmnMutexPtr					mNmsQueueLock;
//	OmnCondVarPtr				mNmsQueueCondVar;
//	OmnVList<OmnNmsMsgPtr>	mNmsQueue;

//	OmnMutexPtr					mMiscQueueLock;
//	OmnCondVarPtr				mMiscQueueCondVar;
//	OmnVList<OmnMsgPtr>		mMiscQueue;

	OmnMutexPtr					mCommLock;

	//
	// The member data for Network Interface restoration 
	//
	OmnMutexPtr					mCommToRestoreLock;
	OmnCondVarPtr				mCommToRestoreCondVar;
	OmnVList<OmnCommPtr>		mCommToRestore;

public:
	OmnCommMgr();
	~OmnCommMgr();

	bool		addComm(const OmnCommPtr &comm);
	bool		removeComm(const OmnCommPtr &comm);
	OmnCommPtr	getCommByNIID(const int niid);

	bool		restoreComm(const OmnCommPtr &ni); 

	//virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	//virtual bool signal(const int threadLogicId);
    //virtual void heartbeat();
    //virtual bool checkThread() const; 
    //virtual bool isCriticalThread() const; 


private:
	bool		restoreCommThreadFunc(OmnThrdStatus::E &state, const OmnSPtr<OmnThread> &thread);
//	bool		readingThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

//	void		addMgcpMsg(const OmnMgcpMsgPtr &msg);
//	void		addNmsMsg(const OmnNmsMsgPtr &msg);
//	void		addMiscMsg(const OmnMsgPtr &msg);
};
#endif
