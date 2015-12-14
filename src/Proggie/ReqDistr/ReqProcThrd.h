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
// 03/23/2009 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Proggie_ReqDistr_ReqProcThrd_h
#define AOS_Proggie_ReqDistr_ReqProcThrd_h

#include "Proggie/ReqDistr/Ptrs.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/Ptrs.h"


class OmnString;

class AosReqProcThrd : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	OmnThreadPtr		mThread;
	AosReqDistrPtr		mMgr;
	AosNetReqProcPtr	mProcessor;
	AosNetReqProc*		mProcessorRaw;
	int					mIndex;
//	bool				mThreadStatus;

public:
	AosReqProcThrd(const AosReqDistrPtr &mgr, const AosNetReqProcPtr &proc, const int index);
	~AosReqProcThrd();

	bool 	stop();

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	void 			setThreadStatus()
	{
		mThreadStatus = true;
	}
private:
	void 	procError(const OmnConnBuffPtr &req, const OmnString &errmsg);
	// bool 	procRequest(const OmnConnBuffPtr &req);
};
#endif

