////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/26/2010	Create by Tank
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_BrowserSimu_BrowserThrd_h
#define AOS_BrowserSimu_BrowserThrd_h

#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "WebServerTorturer/Ptrs.h"
#include "BrowserSimu/Ptrs.h"


class AosBrowserThrd : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnThreadPtr		mThread;
	AosBrowserSimuPtr	mTest;
	int					mThreadId;

public:
	AosBrowserThrd(const int tid);
	~AosBrowserThrd();

	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	bool		    doProcess();

private:

};
#endif

