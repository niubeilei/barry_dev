////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpTrafficGen.h
// Description:
//   
//
// Modification History:
// 12/07/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UdpTraffic_Gen_UdpTrafficGen_h
#define Aos_UdpTraffic_Gen_UdpTrafficGen_h


#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/File.h"

class AosUdpTrafficGen : public virtual OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnUdpPtr		mSock;
	OmnThreadPtr	mSendThread;
	OmnThreadPtr	mRecvThread;

    OmnString		mFileName;
	OmnFile        *mFile;
    OmnIpAddr		mLocalAddr;
	int				mLocalPort;
    OmnIpAddr		mRemoteAddr;
	int				mRemotePort;

	int				mPacketSize; // packet size once sent
	int				mSendRate;   // how many packets per second

	bool 		mIsStopped;


	unsigned int	mPacketSent;
	unsigned int 	mPacketReceived;
	unsigned int 	mBytesSent;
	unsigned int 	mBytesReceived;
	unsigned int 	mMaxPackets;

	int 		mBreakTime;
	bool		mIsBreak;

public:
	pthread_mutex_t mMutex;
	pthread_cond_t mCond;

public:
	
	AosUdpTrafficGen( 
					const OmnIpAddr &localAddr, 
					const int localPort,
					const OmnIpAddr &remoteAddr, 
					const int remotePort,
					const OmnString &filename, 
					const int packetSize=512,
					const int sendRate=1000,
					const unsigned int maxPackets=0,
					const int breakTime=0);

	bool start();
	bool stop();
	bool isSendingStopped();
	bool sendThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;


	unsigned int getPacketSent();
	unsigned int getPacketReceived();
	unsigned int getBytesSent();
	unsigned int getBytesReceived();

	bool getIsBreak();
};

#endif

