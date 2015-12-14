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
// 03/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/ReqDistr/ReqProcThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataMgr/GroupDataMgr.h"
#include "Debug/Except.h"
#include "Proggie/ProggieUtil/NetRequest.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "TinyXml/TinyXml.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebProcMgr.h"
#include "XmlInterface/WebProcReq.h"
#include "XmlInterface/XmlRc.h"


static int sgTotalThreads = 0;


AosReqProcThrd::AosReqProcThrd(const AosReqDistrPtr &mgr, 
		const AosNetReqProcPtr &proc,
		const int index)
:
mMgr(mgr),
mProcessor(proc),
mProcessorRaw(mProcessor.getPtr()),		// Chen Ding, 2014/12/12
mIndex(index)
{
	// Chen Ding, 2014/12/12
	if (!mProcessorRaw)
	{
		OmnThrowException("Processor is null");
		return;
	}
	sgTotalThreads++;
	OmnScreen << "Start ReqDistr Thread: " << sgTotalThreads << " : " << index << endl;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "ReqProcThrd", index, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosReqProcThrd::~AosReqProcThrd()
{
}


bool
AosReqProcThrd::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosReqProcThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosReqProcThrdPtr thisPtr(this, false);
	OmnConnBuffPtr req;
    while (state == OmnThrdStatus::eActive)
    {
		mThreadStatus = true;
		req = mMgr->getRequest(mIndex);
		// if (mProcessor) rslt = mProcessor->procRequest(req);
		// if (!rslt) procRequest(req);
		try 
		{
			// Chen Ding, 2014/12/12
			// if (mProcessor) mProcessor->procRequest(req);
			mProcessorRaw->procRequest(req);
		}
		catch(std::exception &e)
		{
			OmnAlarm << e.what() << enderr;	
		}
		catch(...)
		{
			OmnAlarm << enderr;
		}
	}
	return true;
}


/* Chen Ding, 2014/12/12
bool
AosReqProcThrd::procRequest(const OmnConnBuffPtr &req)
{
	OmnTrace << "To process request: " << req->getData() << endl;
	// aos_assert_r(mProcessor, false);
	mProcessor->procRequest(req);
	return true;
}
*/

bool
AosReqProcThrd::signal(const int threadLogicId)
{
	mMgr->signal(threadLogicId);
	return true;	
}

/*
bool    
AosReqProcThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}
*/
