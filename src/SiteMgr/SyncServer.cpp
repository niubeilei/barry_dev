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
// Modification History:
// 12/17/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SiteMgr/SyncServer.h"

#include "alarm_c/alarm.h"
#include "IdGen/IdGen.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "XmlInterface/WebRequest.h"
#include "SiteMgr/SiteMgr.h"
#include "SiteMgr/SiteReqProc.h"

OmnSingletonImpl(AosSyncServerSingleton,
                 AosSyncServer,
                 AosSyncServerSelf,
                "AosSyncServer");


static OmnString sgDocidgenName = "docidgen";
static AosIdGenPtr sgDocidGen;


AosSyncServer::AosSyncServer()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	mRequests.setNullValue(0);
	sgDocidGen = OmnNew AosIdGen(sgDocidgenName);
}


AosSyncServer::~AosSyncServer()
{
}


bool
AosSyncServer::start()
{
	// OmnThreadedObjPtr thisPtr(this, false);
    // mThread = OmnNew OmnThread(thisPtr, "SyncServerThrd", 0, true, true, __FILE__, __LINE__);
    // mThread->start();
	return true;
}


bool
AosSyncServer::stop()
{
    if(mThread.isNull())
    {
        return true;
    }
    mThread->stop();
    return true;
}


OmnRslt
AosSyncServer::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool    
AosSyncServer::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSyncServer::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSyncServer::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
	/*
    while (state == OmnThrdStatus::eActive)
    {
		mLock->lock();
		if (mRequests.entries() == 0)
		{
			// There is no more cached entries. Start a timer to 
			// wake this thread up later. 
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		// There are something to be processed. Get the first entry
		// from the list, and then release the lock.
		request = mRequests.pop();
		mLock->unlock();
		proc.procRequest(request);
	}
	*/
	return true;
}


bool		
AosSyncServer::getDataidFromServer(
		const OmnString &idname, 
		const AosSyncServerReqCbPtr &caller)
{
	// This function sends a request to the remote server
	// to retrieve a block of dataids. 
	// The request should be in the form: 
	// 	<request syncopr="a" siteid="xxx"/>
	OmnString req = "<request syncopr=\"a\" siteid=\"";
	req << AosSiteMgrSelf->getSiteId() << "\"/>";
	aos_assert_r(sendRequest(req), false);
	return true;
}


OmnString	
AosSyncServer::getDataidBlock(int &num)
{
	// It retrieves a block of dataids. 
	OmnNotImplementedYet;
	return "";
}


bool
AosSyncServer::sendRequest(const OmnString &req)
{
	OmnNotImplementedYet;
	return false;
}
#endif
