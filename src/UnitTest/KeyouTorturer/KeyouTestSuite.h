////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KeyouTestSuite.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_KeyouTorturer_KeyouTestSuite_h
#define Omn_UnitTest_KeyouTorturer_KeyouTestSuite_h

#include "Tester/Ptrs.h"
#include "Tester/TestSuite.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"

class OmnKeyouTestSuite	:	public OmnThreadedObj, OmnTestSuite 
{
private:
	OmnTestMgrPtr 		mTestMgr;
	OmnThreadPtr		mThread;
	OmnMutexPtr			mLock;
	
	bool				mFinished;
	
public:
	OmnKeyouTestSuite(const OmnString &suiteId, 
				 	  const OmnString &desc);
	virtual ~OmnKeyouTestSuite();

	static OmnTestSuitePtr	getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &serverIp,
							const int		&ftpPort,
							const int		&ftpDataPort,
							const int		&telnetPort);

	virtual int		start(const OmnTestMgrPtr &testMgr);


	//
	// ThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
    virtual void heartbeat();
    virtual bool checkThread(OmnString &err, const int thrdLogicId) const;

private:
	int				run();

};

#endif

