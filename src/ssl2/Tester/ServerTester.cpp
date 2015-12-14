////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl/Tester/ServerTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "PKCS/x509.h"
#include "Porting/Sleep.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/ReturnCode.h"
#include "ssl2/SslStatemachine.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"
#include "ssl2/Ssl.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "ssl2/cavium.h"

extern bool AosSSLTesterIsClientAuthFlag;

const int sgMaxConns = 100;
static OmnTcpClientPtr sgTcp[sgMaxConns];
static tcp_vs_conn *sgConn[sgMaxConns];

extern u32 sgServerCiphersSupported[eAosMaxCipher];
extern  u8 sgNumServerCiphersSupported;

extern AosSslAcceleratorType gAosSslAcceleratorType;
extern bool AosSSLTesterIsCaviumCardFlag;

static char *cert_base64 =
"-----BEGIN CERTIFICATE-----"
"MIICiTCCAfICAQEwDQYJKoZIhvcNAQEEBQAwgYgxCzAJBgNVBAYTAkNOMRAwDgYD"
"VQQIEwdiZWlqaW5nMRAwDgYDVQQHEwdiZWlqaW5nMQ8wDQYDVQQKEwZBT1NMVEQx"
"DDAKBgNVBAsTA0FPUzEOMAwGA1UEAxMFQU9TQ0ExJjAkBgkqhkiG9w0BCQEWF2Fv"
"c2NhQGlwYWNrZXRlbmdpbmUuY29tMB4XDTA1MDkxNTA5MDExMVoXDTA2MDkxNTA5"
"MDExMVowgZAxCzAJBgNVBAYTAkNOMRAwDgYDVQQIEwdiZWlqaW5nMRAwDgYDVQQH"
"EwdiZWlqaW5nMQ8wDQYDVQQKEwZBT1NMVEQxDDAKBgNVBAsTA0FPUzESMBAGA1UE"
"AxMJQW9zU2VydmVyMSowKAYJKoZIhvcNAQkBFhthb3NzZXJ2ZXJAaXBhY2tldGVu"
"Z2luZS5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMenQSdmWB3x8HOA"
"rc1zhqsunSFJDhKBRi2Zdl1921zFcdaCsawWZtKJBnplsgpfRFOMEq0woFOohFxo"
"wHwp0XShQhMmwMRd/A4cgwiljRIDAJpAbmQj5qYKoKj30qmx1kcZCKDvm5w6rE/8"
"MFEWuyWKtx5bDd9eotWxRBIGA9q9AgMBAAEwDQYJKoZIhvcNAQEEBQADgYEAUhH+"
"XgzJgTSPNmrPjWvYioD4dRA9J1Y+kOxI2oDxLZdBb74zgUZK+By8WAkelQk5SU9p"
"9zscbde4SQuGyb0bek570wIs/vaO8oRk83jWrefcDpE7V5g24AfcTW07y96Q63nT"
"7Mb2njW5kf4RM37swkqku8WFtV8pKt7V+SLiZSA="
"-----END CERTIFICATE-----";

static X509_CERT_INFO *syscert = 0;

bool AosSslServerTester::start()
{
	unsigned int ret;
	// 
	// Test default constructor
	//
	syscert = (X509_CERT_INFO*)aos_malloc(sizeof(X509_CERT_INFO));
	x509_decode_certinfo_base64(cert_base64, syscert);

	for (int i=0; i<sgMaxConns; i++)
	{
		sgTcp[i] = 0;
		sgConn[i] = 0;
	}
	AosSsl_Init();

	if( AosSSLTesterIsCaviumCardFlag)
	{
		gAosSslAcceleratorType = eAosSslAcceleratorType_CaviumPro;
		if ((ret = Csp1Initialize(CAVIUM_DIRECT)))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1Initialize failed %d", ret);
			aos_free(syscert);
			syscert = NULL;
			return false;
		}
	}
	basicTest();
	return true;
}


/*
bool AosSslServerTester::basicTest()
{
	OmnBeginTest << "Test SSL Server";
	mTcNameRoot = "Test_SSLServer";
	OmnIpAddr addr("0.0.0.0");

	// 
	// Test 'data' == 0
	//

	// 
	// Create a TCP connection
	// 

    //
    // Traffic Bouncer instance
    //
    int maxConns = 1000;
    AosTcpBouncer theBouncer("TcpBouncer",
        OmnIpAddr("192.168.0.200"), 5000, 5,
        maxConns, AosTcpBouncer::eSimpleBounce);
    OmnString errmsg;
    if (!theBouncer.connect(errmsg))
    {
        cout << "********* Failed to connect: " << errmsg << endl;
        return -1;
    }

    AosTcpBouncerListenerPtr bouncerCallback(this, false);
    theBouncer.registerCallback(bouncerCallback);
    theBouncer.startReading();

	while (1)
	{
		OmnSleep(1);
	}

	return true;
}	


int ssl_call_back(
			int rc, 
			struct tcp_vs_conn *conn, 
			char *rslt_data, 
			int rslt_data_len)
{
	switch(rc)
	{
	case eAosContentType_AppData:
		if (rslt_data)
			cout<<rslt_data<<endl;
		break;

	default:
		cout<<"unknow type in ssl_call_back"<<endl;
	}
	return 0;
}


AosTcpBouncer::Action 
AosSslServerTester::msgReceived(
            const AosTcpBouncerClientPtr &client,
            const OmnConnBuffPtr &buff)
{
	// 
	// Retrieve the conn
	//
	OmnTcpClientPtr tcp = client->getConn();
	for (int i=0; i<sgMaxConns; i++)
	{
		if (sgTcp[i] == tcp)
		{
			// 
			// Found the connection
			//
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(sgConn[i] != 0)) 
				<< endtc;
			if (!sgConn[i]) return AosTcpBouncer::eIgnore;

			//aos_trace_hex("Server response", 
			//	buff->getBuffer(), buff->getDataLength());

			int ret = AosSslStm_procRequest(buff->getBuffer(), 
				buff->getDataLength(), 
				eAosSSLFlag_Server | eAosSSLFlag_Front, 
				sgConn[i], ssl_call_back);

			bool r = (ret == eAosRc_DoNothing) || (ret == eAosRc_Success);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(r)) << endtc;
			return AosTcpBouncer::eIgnore;
		}
	}

	// 
	// Didn't find the connection. This is an error
	//
	OmnTC(OmnExpected<int>(1), OmnActual<int>(2)) 
		<< "Did not find the connection: " << tcp->getSock() << endtc;
	return AosTcpBouncer::eIgnore;
}	


void        
AosSslServerTester::clientCreated(const AosTcpBouncerClientPtr &client)
{
	int i,ret;
	OmnString errmsg;
	OmnTcpClientPtr tcp = client->getConn();

	// 
	// Find an idle slot in sgTcp[].
	//
	for (i=0; i<sgMaxConns; i++)
	{
		if (sgTcp[i])
		{
			if (!sgTcp[i]->isConnGood())
			{
				delete sgConn[i];
				sgTcp[i] = 0;
			}
		}

		if (!sgTcp[i])
		{
			sgTcp[i] = tcp;
			sgConn[i] = new tcp_vs_conn();

			memset(sgConn[i], 0, sizeof(tcp_vs_conn));
			sgConn[i]->front_cert = AosCertMgr_getSystemCert();
			//sgConn[i]->front_cert = syscert;
			sgConn[i]->front_prikey = AosCertMgr_getSystemPrivKey();
			sgConn[i]->front_authorities = AosCertChain_getByName("system");
			if(AosSSLTesterIsClientAuthFlag)
			{
				sgConn[i]->front_ssl_flags = eAosSSLFlag_ClientAuth;
				sgConn[i]->backend_ssl_flags = eAosSSLFlag_ClientAuth;
//				ret = AosSslStm_startServer(sgConn[i], sgServerCiphersSupported, 
//						sgNumServerCiphersSupported, 
				ret = AosSslStm_startServer(sgConn[i], 
						eAosSSLFlag_Server|eAosSSLFlag_Front|eAosSSLFlag_ClientAuth, 
						tcp->getSock());
			}
			else
			{
//				ret = AosSslStm_startServer(sgConn[i], sgServerCiphersSupported, 
//						sgNumServerCiphersSupported, eAosSSLFlag_Server | eAosSSLFlag_Front, tcp->getSock());
				ret = AosSslStm_startServer(sgConn[i], 
						eAosSSLFlag_Server | eAosSSLFlag_Front, tcp->getSock());
			}

			if (ret!=0)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl server start failed");
				return;
			}
			return;
		}
	}

	aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Too many connections");
}


AosTcpBouncer::Action 
AosSslServerTester::recvFailed(
            const AosTcpBouncerClientPtr &client,
            char *data,
            u32 dataLen)
{
	return AosTcpBouncer::eContinue;
}


AosTcpBouncer::Action 
AosSslServerTester::readyToBounce(
            const AosTcpBouncerClientPtr &client,
            char *data,
            u32 dataLen)
{
	return AosTcpBouncer::eContinue;
}


AosTcpBouncer::Action 
AosSslServerTester::bounceFailed(
            const AosTcpBouncerClientPtr &client,
            char *data,
            u32 dataLen)
{
	return AosTcpBouncer::eContinue;
}


AosTcpBouncer::Action 
AosSslServerTester::clientClosed(
            const AosTcpBouncerClientPtr &client)
{
	return AosTcpBouncer::eContinue;
}
*/


bool AosSslServerTester::basicTest()
{
	OmnBeginTest << "Test SSL Server";
	mTcNameRoot = "Test_SSLServer";
	OmnIpAddr addr("0.0.0.0");
	u16 port = 5000;

	// 
	// Create a TCP connection
	// 
	OmnTcpServerGrp tcp(addr, port, 1, "SSLServer", 1000, OmnTcp::eNoLengthIndicator);
    OmnTcpCltGrpListenerPtr thisPtr(this, false);
    tcp.setListener(thisPtr);
	tcp.startReading();

	while (1)
	{
		OmnSleep(1);
	}

	return true;
}	


int 
ssl_call_back(
			int rc, 
			struct tcp_vs_conn *conn, 
			char *rslt_data, 
			int rslt_data_len)
{
	switch(rc)
	{
	case eAosContentType_AppData:
		 if (rslt_data)
		 {
			cout<<rslt_data<<endl;
	 	 }
		 break;

	default:
		 cout<<"unknow type in ssl_call_back"<<endl;
	}
	return 0;
}


void
AosSslServerTester::msgRecved(const OmnTcpCltGrpPtr &group,
                              const OmnConnBuffPtr &buff,
                              const OmnTcpClientPtr &tcp)
{
	// 
	// Retrieve the conn
	//
	for (int i=0; i<sgMaxConns; i++)
	{
		if (sgTcp[i] == tcp)
		{
			// 
			// Found the connection
			//
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(sgConn[i] != 0)) 
				<< endtc;

			if (!sgConn[i]) return;

			aos_trace_hex("Server response", buff->getBuffer(), 
				buff->getDataLength());

			int ret = AosSslStm_procRequest(buff->getBuffer(), 
				buff->getDataLength(), 
				eAosSSLFlag_Server | eAosSSLFlag_Front, 
				sgConn[i], ssl_call_back);

			if (ret == eAosRc_DoNothing)
			{
				return;
			}

			// if (ret == eAosRc_ForwardToSender || ret == eAosRc_ForwardToPeer)
			// {
			// 	bool r = tcp->writeTo(data, data_len);
			// 	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(r)) << endtc;
			// }				

			// It is an error
			OmnTC(OmnExpected<int>(100000), OmnActual<int>(ret)) << endtc;
			return;
		}
	}

	// 
	// Didn't find the connection. This is an error
	//
	OmnTC(OmnExpected<int>(1), OmnActual<int>(2)) << endtc;
	return;
}	

void        
AosSslServerTester::newConn(const OmnTcpClientPtr& tcp)
{
	int i,ret;
	OmnString errmsg;

	for (i=0; i<sgMaxConns; i++)
	{
		if (sgTcp[i])
		{
			if (!sgTcp[i]->isConnGood())
			{
				delete sgConn[i];
				sgTcp[i] = 0;
			}
		}

		if (!sgTcp[i])
		{
			sgTcp[i] = tcp;
			sgConn[i] = new tcp_vs_conn();
			memset(sgConn[i], 0, sizeof(tcp_vs_conn));
			sgConn[i]->front_cert = AosCertMgr_getSystemCert();
			sgConn[i]->front_prikey = AosCertMgr_getSystemPrivKey();
			sgConn[i]->front_authorities = AosCertChain_getByName("system");
			if(AosSSLTesterIsClientAuthFlag)
			{
				sgConn[i]->front_ssl_flags = eAosSSLFlag_ClientAuth;
				sgConn[i]->backend_ssl_flags = eAosSSLFlag_ClientAuth;
//				ret = AosSslStm_startServer(sgConn[i], sgServerCiphersSupported, 
//						sgNumServerCiphersSupported, 
				ret = AosSslStm_startServer(sgConn[i], 
						eAosSSLFlag_Server|eAosSSLFlag_Front|eAosSSLFlag_ClientAuth, 
						tcp->getSock());
			}
			else
			{
//				ret = AosSslStm_startServer(sgConn[i], sgServerCiphersSupported, 
//						sgNumServerCiphersSupported, eAosSSLFlag_Server | eAosSSLFlag_Front, tcp->getSock());
				ret = AosSslStm_startServer(sgConn[i], 
						eAosSSLFlag_Server | eAosSSLFlag_Front, tcp->getSock());
			}

			if (ret!=0)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl server start failed");
				return;
			}
			return;
		}
	}

	aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Too many connections");
}


bool        
AosSslServerTester::connClosed(
			const OmnTcpCltGrpPtr &,
            const OmnTcpClientPtr &client)
{
	return true;
}


