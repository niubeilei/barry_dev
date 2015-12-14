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
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_XmlInterface_WebProcThread_h
#define AOS_XmlInterface_WebProcThread_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/Ptrs.h"


class OmnString;

class AosWebProcThread : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	OmnThreadPtr		mThread;
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	AosWebProcMgrPtr	mMgr;

public:
	AosWebProcThread();
	~AosWebProcThread();

	bool 	stop();
	// void 	procReq(const AosWebProcReqPtr &request, const AosWebProcMgrPtr &mgr);
	void 	procRequest(const AosWebProcReqPtr &request);

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:
	void 	procError(const AosWebProcReqPtr &req, const OmnString &errmsg);
};
#endif

