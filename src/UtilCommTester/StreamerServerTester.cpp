////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamerServerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilCommTester/StreamerServerTester.h"

#include "Debug/Debug.h"
#include "Network/NetIf.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "UtilComm/Streamer.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/StreamRequester.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/StreamRecvTrans.h"
#include "UtilComm/SmStreamInvite.h"
#include "UtilComm/SmStreamInviteResp.h"
#include "UtilComm/SmStreamAck.h"
#include "UtilComm/SmStreamAckResp.h"

bool OmnStreamerServerTester::start()
{
	// 
	// Test default constructor
	//
/*	OmnBeginTest << "Test Constructor";
	mTcNameRoot = "Streamer-CTR";
	mStreamer = OmnNew OmnStreamer(OmnIpAddr("192.168.0.101"), 5014);
	mCrtSessionId = 10;
	OmnStreamRequesterPtr thisPtr(this, false);
	mStreamer->requestStream(mCrtSessionId, 100, thisPtr, true);
*/
	return true;
}


/*
void	
OmnStreamerServerTester::sendComplete(const OmnRespCode::E respCode,
									  const OmnString &errmsg, 
									  const uint sessionId, 
									  const uint transId)
{
	OmnCreateTc << (respCode == OmnRespCode::e200Ok) << errmsg << endtc;
	OmnCreateTc << (sessionId == mCrtSessionId)
		<< "Actual: " << (int)sessionId << ". Expected: " << (int)mCrtSessionId << endtc;
}

 
void	
OmnStreamerServerTester::streamRecved(const uint sessionId, 
								const OmnStringPtr &buff, 
								const OmnIpAddr &senderAddr,
								const int senderPort)
{
	OmnScreen << "received " << sessionId << endl;
	OmnCreateTc << (mCrtSessionId == sessionId) << endtc;
	OmnFilePtr file = OmnNew OmnFile("c:\\download\\tmp1.txt", OmnFile::eCreate);
	file->append(*buff);
}
*/

