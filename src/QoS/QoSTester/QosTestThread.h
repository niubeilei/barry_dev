////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: QoSTestThread.h
// Description:
//   
//
// Modification History:
// 	created: 01/05/2007
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_QoSTester_QoSTestThread_h
#define Omn_QoSTester_QoSTestThread_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"

#define SRC_IP_OFFSET 12
#define DST_IP_OFFSET 16
#define SRC_PORT_OFFSET 20
#define DST_PORT_OFFSET 22
#define PROTO_OFFSET 9


class AosQoSTestThread : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	int times;
public:
	//AosQoSTestThread() {}
	//~AosQoSTestThread() {}
	bool		start();
	AosQoSTestThread();
	virtual ~AosQoSTestThread();

	OmnThreadPtr 	mThread;	

	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
   	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
};
#endif

