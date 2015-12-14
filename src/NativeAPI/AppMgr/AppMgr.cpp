////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgr.cpp
// Description:
//	This is the Application Manager client. All application management 
//  functions are accessed through this class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "AppMgr/AppMgr.h"

#include "Alarm/Alarm.h"
#include "AppMgrUtil/SmGetAppInfoReq.h"
#include "AppMgrUtil/SmGetAppInfoResp.h"
#include "AppMgrUtil/SmRegisterAppNameReq.h"
#include "AppMgrUtil/SmRegisterAppNameResp.h"
#include "AppMgrUtil/SmRegisterAppReq.h"
#include "AppMgrUtil/SmRegisterAppResp.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Message/Msg.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(OmnAppMgrSingleton, 
				 OmnAppMgr, 
				 OmnAppMgrSelf,
				"OmnAppMgr");


OmnAppMgr::OmnAppMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


OmnAppMgr::~OmnAppMgr()
{
}


bool
OmnAppMgr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "AppMgr", 0, true, true, __FILE__, __LINE__);
	mThread->start();

	if (!loadAppInfo())
	{
		OmnAlarm << "Failed to load app info" << enderr;
		return false;
	}

	return true;
}


bool
OmnAppMgr::stop()
{
	OmnTrace << "Singleton class OmnAppMgr stopped!" << endl;
	if (mThread)
	{
		mThread->stop();
		return true;
	}

	OmnAlarm << OmnErrId::eSingletonError 
		<< "Thread not started!" << enderr;
	return false;
}

/*
bool
OmnAppMgr::loadAppInfo()
{
	// 
	// This function retrieves all the images from the database. 
	//
	OmnAppInfo appinfo;
	OmnString stmt = "select * from ";
	stmt << appinfo.getTableName();

	OmnDbTablePtr table;
	OmnRslt rslt = appinfo.retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to load AppInfo: " << rslt.getErrmsg() << enderr;
		return false;
	}

	OmnDbRecordPtr record;
	table->reset();
	int num = 0;
	while (num < eMaxAppInfo && (record = table->next()))
	{
		OmnAppInfoPtr r = OmnNew OmnAppInfo();
		r->serializeFromRecord(record);
		mAppInfo[num++] = r;
	}

	return true;
}


OmnAppInfoPtr
OmnAppMgr::getAppInfo(const OmnString &appname)
{
	int num = 0;
	while (num < eMaxAppInfo && mAppInfo[num])
	{
		if (mAppInfo[num]->getAppName() == appname)
		{
			return mAppInfo[num];
		}
		num++;
	}

	return 0;
}
*/


bool
OmnAppMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool 
OmnAppMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter OmnAppMgr::threadFunc." << endl;

	OmnMsgPtr msg;
    while (state == OmnThrdStatus::eActive)
    {
        mLock->lock();
        if (mMsgQueue.entries() <= 0)
        {
            //
            // There is no more messages.
            //
            mCondVar->wait(mLock);
            mLock->unlock();
            continue;
        }

        msg = mMsgQueue.popFront();
        mLock->unlock();

        procMsg(msg);
    }

    OmnTraceThread << "Leaving OmnAppMgr::threadFunc" << endl;
    return true;
}



bool
OmnAppMgr::signal(const int threadLogicId)
{
	//
	// Wake up the thread in case it is sleeping.
	//
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
    return true;
}


void
OmnAppMgr::heartbeat(const int tid)
{
	mThreadStatus = false;
}


bool
OmnAppMgr::checkThread(OmnString &errmsg, const int tid) const
{
	return mThreadStatus;
}


bool
OmnAppMgr::procMsg(const OmnMsgPtr &msg)
{
	switch (msg->getMsgId())
	{
	case OmnMsgId::eSmGetAppInfoReq:
		 return procGetAppInfo((OmnSmGetAppInfoReq*)msg.getPtr());

	default:
		 OmnAlarm << OmnErrId::eProgError 
			<< "Message unrecognized: " << msg->getSeqno() << enderr;
		 return false;
	}

	OmnShouldNeverComeToThisPoint;
	return false;
}


/*
OmnRslt
OmnAppMgr::registerAppName(const OmnString &name)
{
	// 
	// This function registers the application name 'name' with the AM. If 
	// 'requester' is null, it is a blocking call. Otherwise, it is a non-blocking
	// call. 
	// 
	// It sends a message to the server and waits for the response. 
	// 
	OmnSmRegisterAppNameReqPtr req = OmnNew OmnSmRegisterAppNameReq(name);
	req->setRecvDn(OmnDomainNames::eAppMgrServer);

	OmnRespPtr resp;
	OmnRslt rslt = OmnInfobusCltSelf->sendAndRecv(req.getPtr(), resp);
	if (rslt)
	{
		OmnAssert(resp, "");
		if (resp->isPositive())
		{
			// Registration successful
			return true;			
		}

		// Registration failed
		rslt.setErrId(resp->getRespCode());
		rslt.setErrmsg(resp->getErrmsg());
		return rslt;
	}

	// 
	// Failed to send the message. 
	//
	return rslt;
}
*/


OmnRslt
OmnAppMgr::registerApp(const OmnString &appName)
{
	// 
	// It registers this application. This is done by sending a registration message
	// and receives the response. 
	//
	OmnSmRegisterAppReqPtr req = OmnNew OmnSmRegisterAppReq(appName);

	// 
	// Not implemented yet;
	//
	return false;
}


bool
OmnAppMgr::procGetAppInfo(const OmnSmGetAppInfoReqPtr &msg)
{
	// 
	// An entity wants to retrieve the app information. 
	//
	switch (msg->getInfoType())
	{
	case OmnSmGetAppInfoReq::eAppInfo:
		 return getAppInfo(msg);

	case OmnSmGetAppInfoReq::eThreadInfo:
		 return getThreadInfo(msg);

	default:
		 OmnAlarm << OmnErrId::eProgError 
			 << "Unrecognized enum value: " << msg->getInfoType() << enderr;
		{
			OmnSmGetAppInfoRespPtr resp = (OmnSmGetAppInfoResp*)(msg->getResp()).getPtr();
			resp->setRespCode(OmnErrId::eUnrecognizedEnumValue);
			resp->setErrmsg("Unrecognized Infotype");
			resp->send();
			return true;
		}
		 return false;
	}

	OmnShouldNeverComeToThisPoint;
	return false;
}


bool
OmnAppMgr::getAppInfo(const OmnSmGetAppInfoReqPtr &msg)
{
	return true;
}


bool
OmnAppMgr::getThreadInfo(const OmnSmGetAppInfoReqPtr &msg)
{
	return true;
}
