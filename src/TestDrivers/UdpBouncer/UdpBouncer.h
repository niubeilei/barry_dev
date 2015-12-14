////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpBouncer.h
// Description:
//   
//
// Modification History:
// 07/12/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Udp_Bouncer_UdpBouncer_h
#define Aos_Udp_Bouncer_UdpBouncer_h

#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/File.h"

class AosUdpBouncer : public virtual OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnUdpPtr		mSock;
	OmnThreadPtr	mRecvThread;

    OmnIpAddr		mLocalAddr;
	int				mLocalPort;
    OmnIpAddr		mRemoteAddr;
	int				mRemotePort;

public:
	
	AosUdpBouncer( const OmnIpAddr &localAddr, 
					const int localPort);

	virtual ~AosUdpBouncer();

	bool start();
	bool recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
};

#endif

