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
// 10/26/2010 by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEServerCGI/FcgiThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "sys/stat.h"
#include "UtilComm/TcpClient.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
extern char ** environ;
#endif

#include "fcgio.h"
#include "fcgi_config.h"  // HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
#include "fcgiapp.h"
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static OmnMutex sgLock;

int	AosFcgiThrd::mNumThreads = 1;
AosFcgiThrdPtr	AosFcgiThrd::mThreads[AosFcgiThrd::eNumThreads];


static int AosFcgiGetRequest(FCGX_Request *request)
{
	sgLock.lock();
    int rc = FCGX_Accept_r(request);
	sgLock.unlock();
	return rc;
}


using namespace std;

AosFcgiThrd::AosFcgiThrd(const AosFcgiReqProcPtr &req)
:mProc(req)
{
	start();
}


AosFcgiThrd::~AosFcgiThrd()
{
}


bool      	
AosFcgiThrd::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "FcgiThrd", 0, true, true, __FILE__, __LINE__);
    mThread->start();
	return true;
}


bool        
AosFcgiThrd::stop()
{
	return true;
}


bool
AosFcgiThrd::startThreads(const AosFcgiReqProcPtr &req)
{
    FCGX_Init();
	for (int i=0; i<mNumThreads; i++)
	{
		mThreads[i] = OmnNew AosFcgiThrd(req);
	}
	return true;
}


OmnRslt     
AosFcgiThrd::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool    
AosFcgiThrd::signal(const int threadLogicId)
{
	return true;
}


bool    
AosFcgiThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosFcgiThrd::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
    FCGX_Request request;
    FCGX_InitRequest(&request, 0, 0);

	while (state == OmnThrdStatus::eActive)
	{
		int rc = AosFcgiGetRequest(&request);
		if (rc >= 0)
		{
			mProc->procRequest(request);
		}
		else
		{
			OmnAlarm << "Failed to get request: " << rc << enderr;
			OmnSleep(2);
		}
	}

	return true;
}

