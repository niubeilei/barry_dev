////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastFtpSender.h
// Description:
//   
//
// Modification History:
// 01/03/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_FastFtp_Sender_FastFtpSender_h
#define Aos_FastFtp_Sender_FastFtpSender_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/File.h"

class AosFastFtpSender : public virtual OmnThreadedObj
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
	bool			mSendingFinished;

public:
	
	AosFastFtpSender( const OmnString &filename, 
					const OmnIpAddr &localAddr, 
					const int localPort,
					const OmnIpAddr &remoteAddr, 
					const int remotePort);

	bool start();
	bool sendThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool recvThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool finished() const {return mSendingFinished;}

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
};

#endif

