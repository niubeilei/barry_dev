////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StreamerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilCommTester/StreamerTester.h"

#include "Debug/Debug.h"
#include "Network/NetIf.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/File.h"
#include "UtilComm/Streamer.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/StreamRequester.h"
#include "UtilComm/UdpComm.h"
#include "UtilComm/StreamRecvTrans.h"
#include "UtilComm/SmStreamInvite.h"
#include "UtilComm/SmStreamInviteResp.h"
#include "UtilComm/SmStreamAck.h"
#include "UtilComm/SmStreamAckResp.h"


bool OmnStreamerTester::start()
{
/*
	// 
	// Test default constructor
	//
	OmnUint8 t1, t2;

	OmnBeginTest << "Test Constructor";
	mTcNameRoot = "Streamer-CTR";
	mStreamer = OmnNew OmnStreamer(OmnIpAddr("192.168.0.101"), 5001);

	// 
	// To send a buffer
	//
	OmnConnBuffPtr buff = OmnNew OmnConnBuff();
	char *data = buff->getBuffer();
	OmnString str("1234567890abcdefghijklmnopqrstuvwxyz");
	memcpy(data, str.data(), str.length());
	buff->setDataLength(str.length());
	OmnStreamRequesterPtr thisPtr(this, false);
	mCrtSessionId = 10;
	mTransId = 100;
//	mStreamer->sendBuffer(OmnIpAddr("192.168.0.101"), 5014, mCrtSessionId, 100, buff, thisPtr);

	// 
	// To send a file
	// 
	OmnFilePtr file = OmnNew OmnFile("c:\\download\\tmp.txt", OmnFile::eReadOnly);
	mStreamer->sendFile(OmnIpAddr("192.168.0.101"), 5014, mCrtSessionId, mTransId, 100, file, thisPtr);

*/
	return true;
}


/*
void	
OmnStreamerTester::sendComplete(const OmnRespCode::E respCode,
								const OmnString &errmsg, 
								const uint sessionId, 
								const uint transId)
{
	OmnTrace << "Send completed!" << endl;

	OmnCreateTc << (respCode == OmnRespCode::e200Ok) << errmsg << endtc;
	OmnCreateTc << (sessionId == mCrtSessionId)
		<< "Actual: " << (int)sessionId << ". Expected: " << (int)mCrtSessionId << endtc;
}


void	
OmnStreamerTester::streamRecved(const uint sessionId, 
								const OmnStringPtr &buff, 
								const OmnIpAddr &senderAddr,
								const int senderPort)
{
}
*/

