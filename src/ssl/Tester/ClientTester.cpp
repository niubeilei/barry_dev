////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ClientTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl/Tester/ClientTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
// #include "ssl/aosSslProc.h"
#include "ssl/SslClient.h"
#include "ssl/ReturnCode.h"
#include "ssl/SslCommon.h"
#include "ssl/SslStatemachine.h"
#include "ssl/SslSessionMgr.h"
#include "ssl/SslServer.h"
#include "ssl/SslCipher.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TrafficGenThread.h"
#include "ssl/cavium.h"

extern AosSslAcceleratorType gAosSslAcceleratorType;
extern bool AosSSLTesterIsCaviumCardFlag;

static AosSslClientTester *sgClientTester = 0;

int app_data_callback(
			int rc, 
			struct tcp_vs_conn *conn, 
			char *rslt_data, 
			int rslt_data_len)
{
	if (!sgClientTester)
	{
		OmnAlarm << "Client is null" << enderr;
		return -1;
	}

	return sgClientTester->appDataReceived(rc, conn, 
		rslt_data, rslt_data_len);
}


int 
AosSslClientTester::appDataReceived(
			int rc, 
			struct tcp_vs_conn *conn, 
			char *rslt_data, 
			int rslt_data_len)
{
	OmnTrace << "Received app data: " << rslt_data << ". " 
		<< rslt_data_len << endl;
	return true;
	
	/*
	for (int i=0; i<=mMaxClientId; i++)
	{
		if (&mTests[i].mConn != conn)
		{
			continue;
		}

		// 
		// Found the test. 
		//
		if (mTests[i].mSentPos < mTests[i].mDataLen)
		{
			sendAppData(i);
		}

		// 
		// Check the received
		//
		if (mTests[i].mRecvPos >= mTests[i].mDataLen)
		{
			OmnTC(OmnExpected<int>(mTests[i].mRecvPos), 
				  OmnActual<int>(mTests[i].mDataLen)) 
				<< "Received too many data: " 
				<< (int)mTests[i].mRecvPos 
				<< ", " << (int)mTests[i].mDataLen
				<< ", " << (int)rslt_data_len << endtc;
			return -eAosRc_SslTestErr;
		}

		if (mTests[i].mRecvPos + rslt_data_len > mTests[i].mDataLen)
		{
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(false)) 
				<< "RecvPos: " << (int)mTests[i].mRecvPos
				<< ". DataLen: " << (int)mTests[i].mDataLen
				<< ". rslt_data_len: " << (int)rslt_data_len << endtc;
			return -eAosRc_SslTestErr;
		}

		if (memcmp(mTests[i].mDataToSend + mTests[i].mRecvPos, 
				rslt_data, rslt_data_len))
		{
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(false)) 
				<< "Receiving error. Expected: " 
				<< mTests[i].mDataToSend + mTests[i].mRecvPos
				<< ". Actual: " << rslt_data << endtc;
			return -eAosRc_SslTestErr;
		}

		mTests[i].mRecvPos += rslt_data_len;
		if (mTests[i].mRecvPos == mTests[i].mDataLen)
		{
			//
			// Sending and receiving finished
			//
			OmnTC(OmnExpected<int>(mTests[i].mDataLen), 
				OmnActual<int>(mTests[i].mSentPos)) << endtc;
			
			mTests[i].mExecuted++;

			if (mTests[i].mExecuted == mTests[i].mRepeat)
			{
				// 
				// Finished. Close the connection.
				//
				mTcpTrafficGen->closeConn(mTests[i].mTcp);
				return 0;
			}

			// 
			// Need to restart
			//
			return mTcpTrafficGen->restartClient(mTests[i].mTcp);
		}

		// 
		// Not finished yet.
		//
		return 0;
	}			
			
	// 
	// Did not find the client. This is an error
	//
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(false))
		<< "Failed to find the client" << endtc; 
		
	return -eAosRc_SslTestErr;
*/
}


bool AosSslClientTester::start()
{
	int ret;
	// 
	// Test default constructor
	//
	sgClientTester = this;
	if( AosSSLTesterIsCaviumCardFlag)
	{
		gAosSslAcceleratorType = eAosSslAcceleratorType_CaviumPro;
		if ((ret = Csp1Initialize(CAVIUM_DIRECT)))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1Initialize failed %d", ret);
			return false;
		}
	}
	basicTest();
	basicTest();
	// volumeTest(100, OmnIpAddr("192.168.0.201"), 5000, 200);
	return true;
}


bool AosSslClientTester::volumeTest(
			const u32 repeat, 
			const OmnIpAddr &addr,
			const int port,
			const u32 numSessions)
{
	static int lsNameIdx = 1;
	OmnDelete mTests;
/*	
	mTcpTrafficGen = OmnNew AosTcpTrafficGen(
		1, addr, port, 1, repeat, numSessions, 'a', 1000,  eAosSendBlockType_Fixed, 1024, 0);

	mTests = OmnNew AosSslTest[numSessions];
	mMaxClientId = numSessions-1;
	
	for (u32 i=0; i<numSessions; i++)
	{
		OmnString name = "SslTest_";
		name << lsNameIdx++;
		mTests[i].set(repeat, name, addr, 0);
		mTests[i].mDataLen = OmnRandom::nextInt(1, 32000);
		mTests[i].mDataToSend = (char *)aos_malloc(mTests[i].mDataLen);
		mTests[i].mSentPos = 0;
		mTests[i].mAppRecordSize = OmnRandom::nextInt(1, 32000);
		mTests[i].mDataSendingStarted = 0;
	}

	mTcpTrafficGen->start();*/
	return true;
}


bool AosSslClientTester::basicTest()
{

	OmnBeginTest << "Test SSL Client";
	mTcNameRoot = "Test_SSLClient";
	OmnIpAddr addr("192.168.10.191");
	u16 port = 443;
	//OmnIpAddr addr("192.168.10.190");
	//u16 port = 5000;

	// 
	// Test 'data' == 0
	//

	AosSsl_cipherFunInit();

	// 
	// Create a TCP connection
	// 
	OmnTcpClient tcp("SSL", addr, port, 1, OmnTcp::eNoLengthIndicator);
	
	OmnString rslt;
	bool r = tcp.connect(rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(r)) << rslt << endtc;

	// 
	// Start handshake
	//
	tcp_vs_conn conn;
	tcp_vs_dest dest;
	dest.addr = addr.getIPv4();

	memset(&conn, 0, sizeof(tcp_vs_conn));
	conn.dest = &dest;

	aosSslSession_t *session = AosSsl_getClientSession(addr.getIPv4());
	int ret;
	if (session)
	{
		/*
		ret = AosSslStm_startClient(&conn, eAosSSLFlag_Front, 0, 
			(char *)session->sessionid, eAosSSL_SessionIdLen, 
			(char *)session->master_secret, MAX_RSA_MODULUS_LEN, NULL, NULL, 
			tcp.getSock());*/
	}
	else
	{
		/*
		ret = AosSslStm_startClient(&conn, eAosSSLFlag_Front, 
			0, 0, 0, 0, 0, 0, 0, tcp.getSock());*/
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(conn.front_context != 0)) 
		<< endtc;
	(conn.front_context)->sock = tcp.getSock();
	//(conn.front_context)->conn = &conn;

	OmnTest(ret == 0) << "test" << endtc;

	OmnConnBuffPtr connbuff;
	bool timeout;
	bool broken;
	bool data_sent = false;
	// tcp.writeTo(buffer, msglen);

	while (1)
	{
		r = tcp.readFrom(connbuff, 10, timeout, broken);
		if (!r)
		{
			// 
			// Connection broken. 
			//
			return true;	
		}

		if (!connbuff)
		{
			break;
		}

		// aos_trace_hex("Server Response", connbuff->getBuffer(), 
		// 	connbuff->getDataLength());
		/*
		ret = AosSslStm_procState(
				connbuff->getBuffer(),
				connbuff->getDataLength(),
				 eAosSSLFlag_Front|eAosSSLFlag_Client|eAosSSLFlag_Receive,
				&conn, 
				conn.front_context,
				app_data_callback);
		
		if ((conn.front_context)->stateFunc == AosSsl_Established && !data_sent)
		{
			data_sent = true;
			OmnString request = "GET / HTTP/1.1\r\n"
				"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, "
					"application/vnd.ms-powerpoint, application/msword, "
					"application/x-shockwave-flash, *\r\n"
				"Accept-Language: zh-cn\r\n"
				"Accept-Encoding: gzip, deflate\r\n"
				"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
				"Host: 192.168.0.201\r\n"
				"Connection: Keep-Alive\r\n\r\n";

			AosSslStm_procRequest((char *)request.data(), request.length(), 
				eAosSSLFlag_Front | eAosSSLFlag_Send | eAosSSLFlag_Plain, 
				&conn, app_data_callback);

			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(r)) << endtc;
		}*/
	}

	return true;
}	


bool AosSslClientTester::startSslSession(AosSslTest &test)
{
/*
	// 
	// Create a TCP connection
	// 
	test.mTcp = OmnNew OmnTcpClient(test.mName, 
		test.mAddr, test.mPort, 1, OmnTcp::eNoLengthIndicator);
	
	OmnString rslt;
	bool r = (test.mTcp)->connect(rslt);
	OmnTest(r) << rslt << endtc;

	// 
	// Start handshake
	//
	memset(&test.mConn, 0, sizeof(tcp_vs_conn));
	int ret = AosSslStm_startClient(&test.mConn, eAosSSLFlag_Front|eAosSSLFlag_Client, 
			0, 0, 0, 0, 0, (test.mTcp)->getSock());
	OmnTest(ret == 0) << endtc;
	OmnTest(test.mConn.front_context != 0) << endtc;
	(test.mConn.front_context)->sock = (test.mTcp)->getSock();
*/	return true;
}	


void        
AosSslClientTester::msgRecved(
			const OmnTcpCltGrpPtr &group,
            const OmnConnBuffPtr &buff,
            const OmnTcpClientPtr &client)
{
	// 
	int clientId = client->getClientId();
	int ret;
	if (clientId < 0 || clientId > mMaxClientId)
	{
		OmnAlarm << "Incorrect client ID: " << clientId << enderr;
		return;
	}
	
	mTests[clientId].mTcp = client;
	/*
	ret = AosSslStm_procState(
		buff->getBuffer(),
		buff->getDataLength(),
		0,
		&mTests[clientId].mConn, 
		mTests[clientId].mConn.front_context,
		app_data_callback);
	
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret))
		<< clientId << endtc;

	if ((mTests[clientId].mConn.front_context)->stateFunc == 
			AosSsl_Established && !mTests[clientId].mDataSendingStarted)
	{
		ret = sendAppData(clientId);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		mTests[clientId].mDataSendingStarted = 1;
	}*/
}


bool
AosSslClientTester::sendAppData(const int clientId)
{
	int ret;
	if (clientId < 0 || clientId > mMaxClientId)
	{
		OmnAlarm << "Incorrect client ID: " << clientId << enderr;
		return false;
	}

	if (mTests[clientId].mSentPos >= mTests[clientId].mDataLen)
	{
		// 
		// No more data to send. 
		//
		return false;
	}
	
	u32 sendLen = mTests[clientId].mDataLen - mTests[clientId].mSentPos;
	if (sendLen > mTests[clientId].mAppRecordSize) 
	{
		sendLen = mTests[clientId].mAppRecordSize;
	}

	mTests[clientId].mSentPos += sendLen;
	/*
	int ret = AosSslStm_procRequest(mTests[clientId].mDataToSend, 
		sendLen, 
		eAosSSLFlag_Front | eAosSSLFlag_Send | eAosContentType_PlainData, 
		&mTests[clientId].mConn, app_data_callback);
	*/
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	return ret == 0;
}


void        
AosSslClientTester::connCreated(
			const OmnTcpCltGrpPtr &group, 
			const OmnTcpClientPtr &client)
{
/*
	// 
	// Start handshake
	//
	int clientId = client->getClientId();
	if (clientId < 0 || clientId > mMaxClientId)
	{
		OmnAlarm << "Incorrect client ID: " << clientId << enderr;
		return;
	}
	
	mTests[clientId].mTcp = client;
	memset(&mTests[clientId].mConn, 0, sizeof(tcp_vs_conn));
	int ret = AosSslStm_startClient(&mTests[clientId].mConn, 
		eAosSSLFlag_Front, 0, 0, 0, 0, 0, client->getSock());
	OmnTest(ret == 0) << endtc;
	OmnTest(mTests[clientId].mConn.front_context != 0) << endtc;
	(mTests[clientId].mConn.front_context)->sock = client->getSock();
*/	return;
}


void        
AosSslClientTester::connClosed(
			const OmnTcpCltGrpPtr &group, 
			const OmnTcpClientPtr &client)
{
/*
	// 
	// Start handshake
	//
	int clientId = client->getClientId();
	if (clientId < 0 || clientId > mMaxClientId)
	{
		OmnAlarm << "Incorrect client ID: " << clientId << enderr;
		return;
	}
	
	mTests[clientId].mTcp = client;
	memset(&mTests[clientId].mConn, 0, sizeof(tcp_vs_conn));
	int ret = AosSslStm_startClient(&mTests[clientId].mConn, 
		eAosSSLFlag_Front, 0, 0, 0, 0, 0, client->getSock());
	OmnTest(ret == 0) << endtc;
	OmnTest(mTests[clientId].mConn.front_context != 0) << endtc;
	(mTests[clientId].mConn.front_context)->sock = client->getSock();
*/	return;
}


void        
AosSslClientTester::msgRecved(
			const AosTcpTrafficGenClientPtr &client,
            const OmnConnBuffPtr &buff)
{
}


void        
AosSslClientTester::connCreated(
			const AosTcpTrafficGenClientPtr &client,
            const OmnTcpClientPtr &conn)
{
}


void        
AosSslClientTester::connClosed(
			const AosTcpTrafficGenClientPtr &client,
            const OmnTcpClientPtr &conn)
{
}


void        
AosSslClientTester::readyToSend(
			const AosTcpTrafficGenClientPtr &client,
            const char *data,
            const int dataLen,
            bool &needToSend)
{
}


void        
AosSslClientTester::sendFinished(const AosTcpTrafficGenClientPtr &client)
{
}


void        
AosSslClientTester::dataSent(
			const AosTcpTrafficGenClientPtr &client,
            const char *data,
            const int dataLen)
{
}


void        
AosSslClientTester::sendFailed(
			const AosTcpTrafficGenClientPtr &client,
            const char *data,
            const int dataLen,
            const OmnRslt &rslt)
{
}


void        
AosSslClientTester::recvFailed(
			const AosTcpTrafficGenClientPtr &client,
            const OmnConnBuffPtr &buff,
            AosTcpTrafficGen::Action &action)
{
}


void        
AosSslClientTester::trafficGenFinished(
			OmnVList<AosTcpTrafficGenClientPtr> &clients)
{
}



