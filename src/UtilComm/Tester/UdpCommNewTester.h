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
// 2013/06/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilComm_Tester_UdpCommTester_h
#define Aos_UtilComm_Tester_UdpCommTester_h

#include "Debug/Debug.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/Ptrs.h"


class AosUdpCommTester : public OmnTestPkg,
						 public OmnThreadedObj,
					     virtual public OmnCommListener 
{
private:
	enum
	{
		eDefaultTries = 1000000
	};

	AosRundataPtr	mRundata;
	int				mTries;
	int				mId;
	int				mNumRunners;
	AosUdpCommPtr	mServer;
	OmnString		mConnName;
	OmnIpAddr		mAddr;
	int				mPort;

public:
	AosUdpCommTester();
	AosUdpCommTester(const int id);
	~AosUdpCommTester();

	virtual bool		start();

private:
	bool	basicTest();

	// CommListener Interface
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;
	virtual void 		readingFailed();
	bool				config();

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
};
#endif

