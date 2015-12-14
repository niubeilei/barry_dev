////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspCrlTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/OcspCrlTester/OcspCrlTester.h"
#include "UtilComm/TcpClient.h"
#include "UnitTest/OcspCrlTester/OcspCrlStatus.h"
#include "UnitTest/OcspCrlTester/Ptrs.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpBouncer.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpBouncerListener.h"
#include "UtilComm/TcpBouncerClient.h"
#include "UtilComm/TcpTrafficGenClient.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpClient.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"

//#define	OCSP_CRL_SOFT_TESTING
//#define	OCSP_CRL_TESTING_DEBUG 
#define	OCSP_CRL_TESTING_EASY

//int mContenLen;
int AosOcspCrlTester::maxConns = 100;
int AosOcspCrlTester::sendBlockSize = 1000;
AosSendBlockType sendBlockType = eAosSendBlockType_Fixed;
OmnString strSend = "ok";
int AosOcspCrlTester::mContentLen = strlen(strSend);
AosOcspCrlStatusPtr ocsPtr = new AosOcspCrlStatus();
int successConns,failedConns;

AosOcspCrlTester::AosOcspCrlTester(const OmnIpAddr &localAddr,
			const OmnIpAddr &clientIp1, 
			const OmnIpAddr &clientIp2, 
			const OmnIpAddr &serverIp,
			const OmnIpAddr &bouncerIp,
			const OmnIpAddr &OcspCrlServerIp)
{    
	mName = "AosOcspCrlTester";	
	mLocalAddr = localAddr;
	mClientProxy_IP[0] = clientIp1;
	mClientProxy_IP[1] = clientIp2;
	mServerProxy_IP = serverIp;
	mBouncer_IP = bouncerIp;
	mOcspCrlServer_IP = OcspCrlServerIp;
} 

AosOcspCrlTester::~AosOcspCrlTester()
{
} 

bool
AosOcspCrlTester::start()
{
    //
    // Test default constructor
    //
    mProxyClientConn[0] = new OmnTcpClient("proxyclientvalid", mClientProxy_IP[0], eAosOcspCrl_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	mProxyClientConn[1] = new OmnTcpClient("proxyclientinvalid", mClientProxy_IP[1], eAosOcspCrl_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);						
	mProxyServerConn = new OmnTcpClient("proxyserver", mServerProxy_IP, eAosOcspCrl_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
/*	mBouncerConn = new OmnTcpClient("bouncer", mBouncer_IP, eAosOcspCrl_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);*/
	mOcspCrlServerConn = new OmnTcpClient("OCSPCRLServer", mOcspCrlServer_IP, eAosOcspCrl_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	OmnString errmsg;
	mProxyClientConn[0]->connect(errmsg);
	mProxyClientConn[1]->connect(errmsg);
	mProxyServerConn->connect(errmsg);
//	mBouncerConn->connect(errmsg);
//	mOcspCrlServerConn->connect(errmsg);
	for (int numsClient = 0; numsClient < eAosOcspCrl_NumClients; numsClient++)
//	for (int numsClient = 0; numsClient < 1; numsClient++)
	{
		// clear all configurations
		sendCmd(OmnString("clear config"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("clear config"), mProxyServerConn);
		sendCmd(OmnString("ssl hardware on"), mProxyClientConn[numsClient]);	
		sendCmd(OmnString("ssl hardware on"), mProxyServerConn);
		//
		//	PROXY CLIENT SIDE CONFIGURATION
			//
			//	a. configure a virtual service
			//
		OmnString appProxyAddCmd = "app proxy add clientvs1 http ";
		appProxyAddCmd << mClientProxy_IP[numsClient].toString() << " 8001 plain";
		sendCmd(appProxyAddCmd, mProxyClientConn[numsClient]);
		sendCmd(OmnString("app proxy rs add clientvs1 192.168.0.235 4432"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("app proxy ssl backend clientvs1 on"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("app proxy set status clientvs1 on"), mProxyClientConn[numsClient]);

		#ifdef OCSP_CRL_SOFT_TESTING 
			//
			//	b. valid usb certificate, 3 valid 518 certificate at client side
			// 
		sendCmd(OmnString("cert manager import filecert db 518valid1 /temp/518valid1.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db 518valid2 /temp/518valid2.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db 518valid3 /temp/518valid3.crt"), mProxyClientConn[numsClient]);
		
		sendCmd(OmnString("cert manager import filecert db 518invalid1 /temp/518invalid1.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db 518invalid2 /temp/518invalid2.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db 518invalid3 /temp/518invalid3.crt"), mProxyClientConn[numsClient]);
		
		sendCmd(OmnString("cert manager import filecert db usbvalid1 /temp/usbvalid1.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db usbvalid2 /temp/usbvalid2.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db usbvalid3 /temp/usbvalid3.crt"), mProxyClientConn[numsClient]);
		
		sendCmd(OmnString("cert manager import filecert db usbinvalid1 /temp/usbinvalid1.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db usbinvalid2 /temp/usbinvalid2.crt"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import filecert db usbinvalid3 /temp/usbinvalid3.crt"), mProxyClientConn[numsClient]);
			//
			//	c.import private keys at client side
			//
		sendCmd(OmnString("cert manager import file private key 518valid1 /temp/518valid1.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key 518valid2 /temp/518valid2.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key 518valid3 /temp/518valid3.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key 518invalid1 /temp/518invalid1.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key 518invalid2 /temp/518valid2.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key 518invalid3 /temp/518invalid3.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key usbvalid1 /temp/usbvalid1.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key usbvalid2 /temp/usbvalid2.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key usbvalid3 /temp/usbvalid3.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key usbinvalid1 /temp/usbinvalid1.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key usbinvalid2 /temp/usbinvalid2.key"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("cert manager import file private key usbinvalid3 /temp/usbinvalid3.key"), mProxyClientConn[numsClient]);
			//
			//	d. import ca certificate at client side
			//
		sendCmd(OmnString("cert manager import filecert db ca /temp/ca.crt"), mProxyClientConn[numsClient]);
			//
			//	e. import a cert chain at client side
			//
		sendCmd(OmnString("cert chain add system ca"), mProxyClientConn[numsClient]);
			//	d. choice a certificate as system
		sendCmd(OmnString("cert manager setcert system 518valid1"), mProxyClientConn[numsClient]); 
		#endif
		
			//	e. close ssl session reuse and open log filter as debug mode
				
		sendCmd(OmnString("ssl session reuse server off"), mProxyClientConn[numsClient]);
		sendCmd(OmnString("ssl session reuse client off"), mProxyClientConn[numsClient]);
		
		#ifdef OCSP_CRL_TESTING_DEBUG
		sendCmd(OmnString("log set filter all debug"), mProxyClientConn[numsClient]);
		#endif
	}	
	
		//	
		//	PROXY SERVER SIDE CONFIGURATION
			//
			//	a. configure a virtual service at server side
			//
		sendCmd(OmnString("app proxy add servervs2 http 192.168.0.235 4432 ssl"), mProxyServerConn);
		sendCmd(OmnString("app proxy rs add servervs2 192.168.1.245 8002"), mProxyServerConn);
		sendCmd(OmnString("app proxy ssl clientauth servervs2 on"), mProxyServerConn);
		sendCmd(OmnString("app proxy set status servervs2 on"), mProxyServerConn);
		
		#ifdef OCSP_CRL_SOFT_TESTING
			//
			//	b. import a certificate and its private key at server side
			//
		sendCmd(OmnString("cert manager import filecert system 518valid1 /temp/518valid1.crt"), mProxyServerConn);
		sendCmd(OmnString("cert manager import file private key 518valid1 /temp/518valid1.key"), mProxyServerConn);
			//
			//	c. import ca certificate and certificate chain at server side 
			//
		sendCmd(OmnString("cert manager import filecert db ca /temp/ca.crt"), mProxyServerConn);
		sendCmd(OmnString("cert chain add system ca"), mProxyServerConn); 
		#endif
		
			//
			//	d.initial ocsp setting
			//

//		sendCmd(OmnString("ocsp set addr 192.168.1.88 3904"), mProxyServerConn);	

		sendCmd(OmnString("ocsp set addr 192.168.1.245 3904"), mProxyServerConn);	
		sendCmd(OmnString("ocsp status on"), mProxyServerConn);
			//
			//	e. initial crl setting
			//
//		sendCmd(OmnString("crl set addr 192.168.1.88 80"), mProxyServerConn);		
//		sendCmd(OmnString("crl set url http://192.168.1.88/cert/crl.der"), mProxyServerConn);	
//		sendCmd(OmnString("crl set timeout 10"), mProxyServerConn);	
//		sendCmd(OmnString("crl status on"), mProxyServerConn);	
//		sendCmd(OmnString("crl update"), mProxyServerConn);	
			//
			//	f. close ssl session reuse and open log filter as debug mode
			//
		sendCmd(OmnString("ssl session reuse client off"), mProxyServerConn);
		sendCmd(OmnString("ssl session reuse server off"), mProxyServerConn);
		
		#ifdef OCSP_CRL_TESTING_DEBUG
		sendCmd(OmnString("log set filter all debug"), mProxyServerConn);
		#endif
		
		#ifdef OCSP_CRL_TESTING_EASY
			//
			// g. change ocsp server response mode to easy mode for oscp/crl simulate testing
			//
cout << "enter into easymode" << endl;
		sendCmd(OmnString("ocsp set resp mode easymode"), mProxyServerConn);	
		#endif
		
	//	wait 5 seconds until configurations become effective
	OmnSleep(5);
	
	integrateTest();
//  capacityTest();
//  abnormalTest();

    return true;
}

bool
AosOcspCrlTester::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
{
	OmnString errmsg;
	OmnRslt rslt;
	
	int sec = 5;
	bool timeout = false;
	bool connBroken;
	
	if (!dstServer->isConnGood())
	{
		dstServer->closeConn();
		dstServer->connect(errmsg);
		if (!dstServer->isConnGood())
		{
			//alarm and return;
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
			return false;
		}
	}

	OmnString sendBuff = cmd;
	sendBuff << "\n\n";
	int sendLen = sendBuff.length();
	dstServer->writeTo(sendBuff, sendLen);

	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!dstServer->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		//alarm and return
        showCmd(cmd,revBuff);
        OmnAlarm << "cannot successfully get message from cli server" << rslt.getErrmsg() << enderr;
        return false;
	}
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))
    {
        showCmd(cmd,revBuff);
    }

	return true;
}

bool
AosOcspCrlTester::integrateTest()
{
	
	OmnRslt rslt;
	mNumTries = eAosOcspCrl_NumTries;
	
   	while(mNumTries--)
	{
		//
		// change numChanges parameters
		// ocsp sw. - crl sw. - certificate - clientAuth - ocsp response - crl - response
		// ocsp connection - crl connection
		//
		int numChanges = OmnRandom::nextInt(1, 6); 

		//
		// change numChanges parameters, here number 4 is for real ocsp server testing
		// ocsp sw. - crl sw. - certificate - clientAuth
		//
//		int numChanges = OmnRandom::nextInt(1, 4); 
		for (int i=0; i<numChanges; i++)
		{
			changeConfig();
		}
		// print status
		printStatus();
				
		bool shouldAllow = checkOcspStatus(ocsPtr);
		mNumConns = OmnRandom::nextInt(1, 100);
		mRepeat = OmnRandom::nextInt(1, 10);

		// wait 2 seconds to ensure changeConfig become effective
		launchTraffic(mNumConns, mRepeat);
	
		if (shouldAllow)
		{
			OmnTC(OmnExpected<int>(mNumConns*mRepeat), OmnActual<int>(mSuccessConns)) << endtc;
			OmnTC(OmnExpected<int>(0), OmnActual<int>(mFailedConns)) << endtc;
		}
		else
		{
			OmnTC(OmnExpected<int>(0), OmnActual<int>(mSuccessConns)) << endtc;
			OmnTC(OmnExpected<int>(mNumConns*mRepeat), OmnActual<int>(mFailedConns)) << endtc;
		}
		#ifdef OCSP_CRL_TESTING_DEBUG
		while(1)
		{
			OmnSleep(1);
			char c;
			cin >> c;
			
			switch(c)
			{
				case 'b':
					goto STOP;
					break;
				default:
					break;
			}
		}
		STOP: cout << "jump out" << endl;
		#endif
		
	}

	return true;
}

bool
AosOcspCrlTester::capacityTest()
{

	return true;
}

bool
AosOcspCrlTester::abnormalTest()
{

	return true;
}

bool
AosOcspCrlTester::launchTraffic(int conn, int repeat)
{
	// start generator, generate a connection
	AosTcpTrafficGen clientGen((eAosOcspCrl_NumTries - (mNumTries+1)),
                               mLocalAddr,
							   misValidCert,//mClientProxy_IP, 
                               8001,
                               1,
                               repeat,
                               conn,
						   	   strSend,
                               mContentLen,
                               eAosSendBlockType_Fixed,
                               sendBlockSize,
                               mContentLen);
	clientGen.start();
    while(!clientGen.checkFinish())
    {
        clientGen.checkConns();
        OmnSleep(1);
    }
	clientGen.stop();
    mSuccessConns = clientGen.getSuccessConns();
    mFailedConns = clientGen.getFailedConns();
    return true;
}

bool
AosOcspCrlTester::checkOcspStatus(const AosOcspCrlStatusPtr &config)
{
	// clientAuthentication off, allow
	if (!config->statusClientAut) 
	{
		return true;
	}
	
	// ocsp sw. and crl sw. both set as off, allow 
	if (!config->statusOcsp&&!config->statusCrl)
	{
		return true;
	}
	
	// invalid certificate(now ocsp or crl is on at least one ), disallow
	if (!config->statusCert)
	{
		return false;	
	}
		
	// crl on, allow
	if (config->statusCrl)
	{
		return true;
	}
	// crl off, ocsp on, ocsp conn. on, ocsp response on
	if (config->statusConnOcspSer&&config->responseOcspSer)
	{
		return true;	
	}
	
	return false;
		
}

bool
AosOcspCrlTester::changeConfig()
{
	
	/****************************************
	   statusOcsp; //ocsp sw. on/off
       statusCrl; //crl sw. on/off
       statusClientAut; //clientauth sw on/off
       statusCert; // cert invalid/valid
       statusConnOcspSer; // conn to ocsp on/offline
       responseOcspSer; // whether to response
    ****************************************/
    int itemChoice = OmnRandom::nextInt(0,5); // to choice a variable
//cout <<"we change number " << itemChoice << endl;	
	
	switch (itemChoice)
	{
		case 0:
				//
				// We want to toggle OCSP configuration status.
				//
				if (ocsPtr->statusOcsp)
				{
		 			//
					// The OCSP flag is on. We need to turn it off.
					//
					ocsPtr->statusOcsp = false;
					sendCmd(OmnString("ocsp status off"), mProxyServerConn);
		 		}
		 		else
		 		{
		 			//
					// The OCSP flag is off. We need to turn it on.
					//
					ocsPtr->statusOcsp = true;
					sendCmd(OmnString("ocsp status on"), mProxyServerConn);
		 		}
		 		break;
		case 1:
		 		//
				// We want to toggle CRL configuration status
				//
				if (ocsPtr->statusCrl)
				{
					//
					// The Crl flag is on. We need to turn it off
					//
					ocsPtr->statusCrl = false;
					sendCmd(OmnString("crl status off"), mProxyServerConn);
				}
				else
				{
					//
					// The Crl flag is off. We need to turn it on
					//
					ocsPtr->statusCrl = true;
					sendCmd(OmnString("crl status on"), mProxyServerConn);
				}
				break;
		case 2:
		 		//
		 		// We want to toggle client authentication flag
		 		//
		 		if (ocsPtr->statusClientAut)
		 		{
		 			//
		 			// client auth. flag is on. We need to turn it off
		 			//
		 			ocsPtr->statusClientAut = false;
		 			sendCmd(OmnString("app proxy set status servervs2 off"), mProxyServerConn);
					OmnSleep(5);
		 			sendCmd(OmnString("app proxy ssl clientauth servervs2 off"), mProxyServerConn);
		 			sendCmd(OmnString("app proxy set status servervs2 on"), mProxyServerConn);
		 		}
		 		else
		 		{
		 			//
		 			// client auth. flag is off. We need to turn it on
		 			//
		 			ocsPtr->statusClientAut = true;
		 			sendCmd(OmnString("app proxy set status servervs2 off"), mProxyServerConn);
					OmnSleep(5);
		 			sendCmd(OmnString("app proxy ssl clientauth servervs2 on"), mProxyServerConn);
		 			sendCmd(OmnString("app proxy set status servervs2 on"), mProxyServerConn);
		 		}
		 		break;

		case 3:
		 		//
		 		// We want to toggle valid/invalid certificate
		 		//
			 	if (ocsPtr->statusCert)
			 	{
					// choice an invalid certificate
					ocsPtr->statusCert = false;
					//sendCmd(OmnString("cert manager setcert system 518valid2"), mProxyClientConn[0]);
					misValidCert = mClientProxy_IP[1];
				}
			 	else
			 	{
					// choice an valid certificate
					ocsPtr->statusCert = true;
				//	sendCmd(OmnString("cert manager setcert system 518valid1"), mProxyClientConn[1]);
					misValidCert = mClientProxy_IP[0];
				}
			 	break;
	#ifdef OCSP_CRL_TESTING_DEBUG
		case 4:
		 		//
		 		// We want to toggle ocsp server connection status
		 		//
		 		if (ocsPtr->statusConnOcspSer)
		 		{
		 			//
		 			// let ocsp server disallow connections
		 			//
		 			ocsPtr->statusConnOcspSer = false;
					sendCmd("OCSPserver stop", mOcspCrlServerConn);
		 		}
		 		else
		 		{
		 			//
		 			// let ocsp server allow connections
		 			//
		 			ocsPtr->statusConnOcspSer = true;
		 			sendCmd("OCSPserver start", mOcspCrlServerConn);
		 		}
		 		break;

		case 5:
				//
				// We want to toggle Ocsp server as response/none-response
				//
				if (ocsPtr->responseOcspSer)
				{
					//
					// let ocsp server response client request
					//
					ocsPtr->responseOcspSer = false;
					sendCmd("OCSPserver resp stop", mOcspCrlServerConn);
				}
				else
				{
					//
					// let ocsp server not to response client request
					//
					ocsPtr->responseOcspSer = true;
					sendCmd("OCSPserver resp on", mOcspCrlServerConn);
				}
		 		break;
/*
		case 6:
				//
				// We want to toggle Crl server connection status
				//
				if (ocsPtr->statusConnCrl)
				{
					//
					// let crl server allow connections
					//
					ocsPtr->statusConnCrl = false;
					sendCmd("crl set status on", mOcspCrlServerConn);
				}
				else
				{
					//
					// let crl server disallow connections
					//
					ocsPtr->statusConnCrl = true;
					sendCmd("crl set status off", mOcspCrlServerConn);
				}
				break;

		case 7:
				//
				// We want to toggle Crl server as response/none-response
				//
				if (ocsPtr->responseCrlSer)
				{
					//
					// let crl server response crl client request
					//
					ocsPtr->responseCrlSer = false;
//					sendCmd("command");
				}
				else
				{
					//
					// let crl server not to response crl client request
					//
					ocsPtr->responseCrlSer = true;
//					sendCmd("command");
				}
				break;
*/
	#endif
		default:
				break;
		}

	return true;
}

void
AosOcspCrlTester::printStatus()
{
	cout << "------------------------------------------------------------------------------------------------" << endl;
	cout <<"NumTries -" <<(eAosOcspCrl_NumTries - (mNumTries + 1)) <<"-: ";
		
	if (ocsPtr->statusOcsp)
	{
		cout <<"ocsp sw. " <<"on" <<", ";
	}
	else
	{
		cout <<"ocsp sw. " <<"off" <<", ";
	}

	if (ocsPtr->statusCrl)
	{
		cout <<"crl sw. " <<"on" <<", ";
	}
	else
	{
		cout <<"crl sw. " <<"off" <<", ";
	}

	if (ocsPtr->statusClientAut)
	{
		cout <<"clientAuth " <<"on " <<", ";
	}
	else
	{
		cout <<"clientAuth " <<"off " <<", ";
	}

	if (ocsPtr->statusCert)
	{
		cout <<"valid cert " <<", ";
	}
	else
	{
		cout <<"invalid cert " <<", ";
	}

	if (ocsPtr->statusConnOcspSer)
	{
		cout <<"ocspConn " <<"on" <<", ";
	}
	else
	{
		cout <<"ocspConn " <<"off" <<", ";
	}

	if (ocsPtr->responseOcspSer)
	{
		cout <<"with "<<"ocspResponse ";
	}
	else
	{
		cout <<"without "<<"ocspResponse ";
	}

	cout << endl;	
	cout << "------------------------------------------------------------------------------------------------" << endl;
}

void
AosOcspCrlTester::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff)
{
        cout << "\n----------SEND CMD----------" << endl;
        cout << cmd << endl;
        cout << "----------RESPONSE----------" << endl;
        if(revBuff)
        {
            cout << OmnString(revBuff->getBuffer(),revBuff->getDataLength());
        }
        else
        {
            cout << "empty";
        }
        cout << "\n---------------------------" << endl;
}   
                                                                                                                                                                 
