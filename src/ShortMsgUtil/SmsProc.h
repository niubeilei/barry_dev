////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgUtil_SmsProc_h
#define AOS_ShortMsgUtil_SmsProc_h

#include "Rundata/Ptrs.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/ShortMsgSvr.h"
#include "ShortMsgUtil/ShortMsg.h"
#include "ShortMsgUtil/SmsReq.h"
#include "ShortMsgUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/CondVar.h"
#include "Util/RCObject.h"
#include <queue>

using namespace std;
class AosSmsProc : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eProcThrdId,
		eHandlerThrdId
	};

private:
	
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mProcThread;
	OmnThreadPtr		mHandlerThread;
	int					mNumReqs;

	AosSmsReqPtr		mHead;
	AosSmsReqPtr		mTail;

	u32					mTotalAccepted;
	u32					mTotalProcessed;
	
	static bool			smShowLog;

	queue<AosShortMsg>	mHandlerQueue;
	AosShortMsgSvrPtr	mListener;
	AosRundataPtr		mRundata;
	
	int 				mTryToRead;
	int					mSerialPort;
	OmnString			mPhone;

public:
	AosSmsProc(
			const int &port, 
			const int &try_read,
			const OmnString &phone);
	~AosSmsProc();
    // OmnThreadedObj Interface
    virtual bool    			threadFunc(
								OmnThrdStatus::E &state, 
								const OmnThreadPtr &thread);

    virtual bool    			signal(const int threadLogicId);

    virtual bool    			checkThread(
								OmnString &err,
								const int thrdLogicId) const;


	bool 						addProcSmsRequest(
								const AosSmsReqPtr &req,
								const AosRundataPtr &rdata);

	void						writeToPort(const char * const buff);

	bool						readOk();
	
	void 						startThread();
private:

	bool						init();

	bool						procSendShortMsg(
								OmnThrdStatus::E &state,
								const OmnThreadPtr &thread);

	bool						handlerShortMsg(
								OmnThrdStatus::E &state,
								const OmnThreadPtr &thread);
	bool						procSmsPriv(const AosSmsReqPtr &request);
	
	bool						checkBuffInput(const char * const buff);
	
	bool						checkBuffOk(const char * const buff);
	
	bool 						checkBuffOut(const char * const buff);
};
#endif
