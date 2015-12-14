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
// 02/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SoapServer/SoapServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MsgProc/MsgProc.h"
#include "MsgProc/GareaMsgProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/ThreadMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Soap/soapH.h"

#include <iostream>
using namespace std;


OmnSingletonImpl(AosSoapServerSingleton,
                 AosSoapServer,
                 AosSoapServerSelf,
                "AosSoapServer");

static struct soap sgSoap;
static AosGareaMsgProcPtr sgProc = OmnNew AosGareaMsgProc(false);
static bool	sgInitProc = false;

AosSoapServer::AosSoapServer()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mStarted(false),
mConfigured(false),
mMaxQueueSize(eDftMaxQueueSize),
mNumThrds(0)
{
	soap_init(&sgSoap);
	 soap_set_mode(&sgSoap,SOAP_C_UTFSTRING);
}


AosSoapServer::~AosSoapServer()
{
	soap_done(&sgSoap);//close master socket and detach environment
}


bool
AosSoapServer::start()
{
	aos_assert_r(mNumThrds, false);
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<mNumThrds; i++)
	{
	    OmnString name = "soapserver_";
	    name << i;
	    OmnThreadPtr thread = OmnNew OmnThread(thisPtr, name, i, true, true, __FILE__, __LINE__);
    	mThreads.push_back(thread);
    	mRundata.push_back(OmnNew AosRundata(AosMemoryCheckerArgsBegin));
		thread->start();
	}
	return true;
}


bool
AosSoapServer::config(const AosXmlTagPtr &config)
{
	sgProc->init();
	mLock->lock();
	aos_assert_rl(!mConfigured, mLock, false);
	if (config)
	{
		AosXmlTagPtr tag = config->getFirstChild("soapserver");
		if (tag)
		{
			mNumThrds = tag->getAttrInt("num_threads", eDftNumThreads);
			if (mNumThrds <= 0 || mNumThrds > eMaxThreads)
			{
				OmnAlarm << "Invalid number of threads: " << tag->toString() << enderr;
				mNumThrds = eDftNumThreads;
			}

			mMaxQueueSize = tag->getAttrInt("max_queue_size", eDftMaxQueueSize);
			if (mMaxQueueSize <= 0)
			{
				OmnAlarm << "Invalid max queue size: " << tag->toString() << enderr;
			}
			else if (mMaxQueueSize > eMaxQueueSizeLimit)
			{
				OmnAlarm << "max queue size too big: " << tag->toString() 
					<< ". Max allowed: " << eMaxQueueSizeLimit << enderr;
			}
			OmnString ip = tag->getAttrStr("local_addr");
			aos_assert_rl(ip != "", mLock, false);
			u64 port = tag->getAttrInt("local_port", 0);
			aos_assert_rl(port>0, mLock, false);
			if (!soap_bind(&sgSoap, ip.data(), port, 100))
			{
				soap_print_fault(&sgSoap, stderr);
				OmnAlarm << "failed bind to addr: " << ip << " port:" << port << enderr;
				mLock->unlock();
				return false;
			}
		}
		else
		{
			OmnAlarm << "Failed to start soapserver" << enderr;
			mLock->unlock();
			return false;
		}
	}

	mConfigured = true;
	mLock->unlock();
	return true;
}


bool
AosSoapServer::stop()
{
    return true;
}


bool	
AosSoapServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		if (!soap_accept(&sgSoap))
		{
			OmnAlarm << "Soap accept request failed" << enderr;
			soap_print_fault(&sgSoap, stderr);
			continue;
		}

		if (soap_serve(&sgSoap) != SOAP_OK)
		{
			OmnAlarm << "Soap get request failed" << enderr;
			soap_print_fault(&sgSoap, stderr);
			continue;
		}		   
	}

	OmnScreen << "Existing soad server thread!" << endl;
	return true;
}


bool
AosSoapServer::procMsg(
		const OmnString &id, 
		const OmnString &input, 
		OmnString &output)
{
	cout << __FILE__ << ":" << __LINE__ << " Hello : " << input.data() << endl;

	/*
	ProcMapItr_t itr = mProcMap.find(id);
	if (itr == mProcMap.end())
	{
		OmnAlarm << "Failed to retrieve msgproc" << enderr;
		return false;
	}
	*/
	if (!sgInitProc) 
	{
		//sgProc->init();
		sgInitProc = true;
	}
	AosXmlParser parser;
	AosXmlTagPtr inputxml = parser.parse(input, "" AosMemoryCheckerArgs);
	aos_assert_r(inputxml, false);
	if (inputxml->isRootTag())
	{
		inputxml = inputxml->getFirstChild();
	}
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	aos_assert_r(sgProc, false);
	bool rslt = sgProc->msgReceived(id, inputxml, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed to received" << enderr;
		return false;
	}
	
	if ((output = rdata->getContents()) == "")
	{
		OmnAlarm << "Failed to get respose " << enderr;
		return false;
	}
	return true;
}

/*
bool
AosSoapServer::msgReceived(
		const OmnString &appid, 
		const AosSoapConnPtr &conn,
		const AosXmlTagPtr &msg) 
{
	// A message was received from a soap connection. This function adds the
	// message to the queue.
	aos_assert_r(appid != "", false);
	aos_assert_r(msg, false);
	mLock->lock();

	if (mMsgQueue.size() > mMaxQueueSize)
	{
		OmnAlarm << "Too many messages in the queue. Maximum allowed: " 
			<< mMaxQueueSize << enderr;
		mLock->unlock();
		conn->sendError(AOSSOAPERROR_TOO_BUSY, msg);
		return false;
	}

	mMsgQueue.push(Request(appid, conn, msg));
	mCondVar->signal();
	mLock->unlock();
	return true;
}
*/

bool	
AosSoapServer::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSoapServer::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool 
AosSoapServer::registerMsgProc(const OmnString &appid, const AosMsgProcPtr &proc)
{
	mLock->lock();
/*
	ProcMapItr_t itr = mProcMap.find(appid);
	if (itr != mProcMap.end())
	{
		OmnScreen << "Proc already registered: " << appid << endl;
		mLock->unlock();
		return false;
	}

	mProcMap[appid] = proc;
	mLock->unlock();
	*/
	return true;
}


