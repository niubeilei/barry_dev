////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastFtpReceiver.h
// Description:
//   
//
// Modification History:
// 01/03/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_FastFtp_Receiver_FastFtpReceiver_h
#define Aos_FastFtp_Receiver_FastFtpReceiver_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/File.h"

class AosFastFtpReceiver : public virtual OmnThreadedObj
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

public:
	
	AosFastFtpReceiver( const OmnString &filename, 
					const OmnIpAddr &localAddr, 
					const int localPort);

	virtual ~AosFastFtpReceiver();

	bool start();
	bool sendThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
};

#endif

