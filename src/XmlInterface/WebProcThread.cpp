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
#if 0
#include "XmlInterface/WebProcThread.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataMgr/GroupDataMgr.h"
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
#include "XmlInterface/XmlProc.h"


AosWebProcThread::AosWebProcThread()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "attacker", 0, true, true);
	mThread->start();
}


AosWebProcThread::~AosWebProcThread()
{
}


bool
AosWebProcThread::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	while(mThread->isStopped())
	{
		OmnWait::getSelf()->wait(0,10000);
	}
	return true;
}


bool	
AosWebProcThread::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosWebProcThreadPtr thisPtr(this, false);
	AosWebProcReqPtr req;
    while (state == OmnThrdStatus::eActive)
    {
		req = mMgr->getRequest();
		procRequest(req);
	}
	return true;
}


bool	
AosWebProcThread::signal(const int threadLogicId)
{
	mLock->lock();
	mThreadStatus = false;
	mCondVar->signal();
	mLock->unlock();
	return true;	
}


bool    
AosWebProcThread::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}


void
AosWebProcThread::procRequest(const AosWebProcReqPtr &request)
{
	// This function processes one request. The request contains an 
	// XML doc, which should be in the following format:
	// 	<request>
	// 		<body>
	// 			<element name="dmid">xxx</element>
	// 			<element name="xxx">xxx</element>
	// 			...
	// 		</body>
	// 	</request>
	aos_assert(request);
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;
	OmnString responseContents;

	TiXmlDocument thedoc;
	thedoc.Parse(request->getData());
OmnTrace << "The data: " << request->getData() << endl;

	TiXmlNode *root = thedoc.FirstChild();
	if (!root)
	{
		procError(request, "Invalid Request");
		return;
	}

	TiXmlElement *elem = root->ToElement();
	if (!elem)
	{
		procError(request, "Failed to reconstruct the elemtn (Internal Error)");
		return;
	}

	const char *dmid = root->getChildTextByAttr("name", "dmid", 0);
	if (!dmid)
	{
		procError(request, "Missing Data Manager ID");
		return;
	}

	if (strcmp(dmid, "group_obj") == 0)
	{
		AosGroupDataMgr dm;
		dm.process(elem, errcode, errmsg, responseContents);
	}
	else if (strcmp(dmid, "none") == 0)
	{
		const char *operation = root->getChildTextByAttr(
			"name", "operation", 0);
		if (!operation)
		{
			errmsg = "Missing operation!";
			OmnAlarm << "Missing operation: " << request->getData() 
				<< enderr;
			procError(request, errmsg);
			return;
		}

		if (strcmp(operation, "saveObject") == 0)
		{
			AosXmlProc proc;
			proc.modifyObj(root, errcode, errmsg);
		}
		else if (strcmp(operation, "serverProc") == 0)
		{
			AosXmlProc proc;
			proc.serverProc(root, errcode, errmsg);
		}
		else
		{
			errmsg = "Unrecognized operation: ";
			errmsg << operation;
			OmnAlarm << errmsg << ". Request: " << request->getData()
				<< enderr;
			procError(request, errmsg);
			return;
		}
	}
	else
	{
		errmsg = "Unrecognized Data Manager ID: ";
		errmsg << dmid;
		procError(request, errmsg);
		return;
	}

	OmnString response;
	if (errcode > 200)
	{
		response = "<status error=\"true\" msg=\"";
		response << errmsg << "\">" << errcode << "</status>";

		// It failed processing the request.
		OmnAlarm << "Failed processing the request: \n"
			<< "    Errcode: " << errcode << "\n"
			<< "    Errmsg: " << errmsg<< "\n"
			<< "    Request: " << request->getData() << enderr;
	}
	else
	{
		response = "<status>200</status>";
		response << responseContents;
	}

	request->sendResponse(response);
}


void
AosWebProcThread::procError(const AosWebProcReqPtr &req, const OmnString &errmsg)
{
	OmnString msg = "<status error=\"true\" msg=\"";
	msg << errmsg << "\">455</status>";
	req->sendResponse(msg);
}

#endif
