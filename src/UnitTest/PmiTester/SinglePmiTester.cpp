////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SinglePmiTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/PmiTester/SinglePmiTester.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpTrafficGen.h"
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
#include "UnitTest/PmiTester/Ptrs.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"

//#define   SINGLE_PMI_SOFT_TESTING
#define   SINGLE_PMI_TESTING_DEBUG 

AosSinglePmiTester::aosTestPlan::aosTestPlan()
:
mOrgNo(0),
mSysNo(0),
mSNNo(0),
mIs518SN(false),
mIsResourceOnly518(false),
mOrgValidity(false),
mSysValidity(false),
mSNValidity(false),
mResourceValidity(false)
{
}

void
AosSinglePmiTester::aosTestPlan::resetValidityFlag()
{
	
	mOrgValidity = false;
	mSysValidity = false;
	mSNValidity = false;
	mResourceValidity = false;
}

AosSinglePmiTester::aosTestDataStatus::aosTestDataStatus()
:
mOrgCounter(0),
mSysCounter(0),
mSNCounter(0),
mResourceCounter(0),
mOperationCounter(0),
mOrgChanged(false),
mSysChanged(false),
mSNChanged(false),
mResourceChanged(false),
mOperationChanged(false)
{
}

void
AosSinglePmiTester::aosTestDataStatus::resetChangeFlag(const bool flag)
{
	// false - means has not been changed;
	// true - changed
	mOrgChanged	= flag;
	mSysChanged = flag;
	mSNChanged = flag;
	mResourceChanged = flag;
	mOperationChanged = flag;
}

AosSinglePmiTester::AosSinglePmiTester(
			const OmnIpAddr &localAddr,
			const OmnIpAddr &clientIp1,
			const OmnIpAddr &clientIp2,
			const OmnIpAddr &clientIP3, 
			const OmnIpAddr &serverIp,
			const OmnIpAddr &bouncerIp,
			const OmnIpAddr &PMIServerIp)
:
mTestNum(eAosSinglePmi_TestTime),
mSuccessConns(0),
mFailedConns(0),
mSendBlockSize(1000)
{   
	mName = "AosSinglePmiTester";	
	initData(localAddr,clientIp1, clientIp2, clientIP3, serverIp, bouncerIp, PMIServerIp);
} 

AosSinglePmiTester::~AosSinglePmiTester()
{
} 

void
AosSinglePmiTester::initData(const OmnIpAddr &localAddr,
											const OmnIpAddr &clientIp1,
											const OmnIpAddr &clientIp2,
											const OmnIpAddr &clientIp3, 
											const OmnIpAddr &serverIp,
											const OmnIpAddr &bouncerIp,
											const OmnIpAddr &PMIServerIp)
{
	mLocalAddr = localAddr;
	mOrgNames[0] = 	"/I=公安部";
//	mOrgNames[1] = 	"/I=武装部";
	
	mSysNames[0] = "/dc=公安涉密信息系统/dc=霄云1";
	mSysNames[1] = "/dc=公安涉密信息系统/dc=霄云2";
	
/*	mValidSNs[0] = "518valid1";
	mValidSNs[1] = "518valid2";
	mValidSNs[2] = "518valid3";
	mValidSNs[3] = "usbvalid1";
	mValidSNs[4] = "usbvalid2";
	mValidSNs[5] = "usbvalid3";
	
	mInvalidSNs[0] = "518invalid1";
	mInvalidSNs[1] = "518invalid2";
	mInvalidSNs[2] = "518invalid3";
	mInvalidSNs[3] = "usbinvalid1";
	mInvalidSNs[4] = "usbinvalid2";
	mInvalidSNs[5] = "usbinvalid3";
*/
	mSnAddr[0] = clientIp1;
	mSnAddr[1] = clientIp2;
	mSnAddr[2] = clientIp3;

	mSnValidity[0] = true;
	mSnValidity[1] = true;
//	mSnValidity[2] = false;

	mAllowEncrypt[0] = true;
	mAllowEncrypt[1] = false;
//	mAllowEncrypt[2] = false;
	
/*	mAllowEncrypt[0] = true;
	mAllowEncrypt[1] = true;
	mAllowEncrypt[2] = true;
	mAllowEncrypt[3] = false;
	mAllowEncrypt[4] = false;
	mAllowEncrypt[5] = false;
*/	
	mResourceNames[0] = "/english";
	mResourceNames[1] = "/中文";
	mResourceNames[2] = "/english/index.html";
	mResourceNames[3] = "/中文/index.html";

/*	mResourceNames[0] = "/index1.html";
	mResourceNames[1] = "/index2.html";
	mResourceNames[2] = "/index3.html";
	mResourceNames[3] = "/picture1.jpg";
	mResourceNames[4] = "/picture2.jpg";
	mResourceNames[5] = "/picture3.jpg";
	mResourceNames[6] = "/picture4.png";
	mResourceNames[7] = "/picture5.png";
	mResourceNames[8] = "/picture6.png";
	mResourceNames[9] = "/Movie1.rm";
	mResourceNames[10] = "/Movie2.rm";
	mResourceNames[11] = "/Movie3.rm";
	mResourceNames[12] = "/Download1.exe";
	mResourceNames[13] = "/Download2.exe";
	mResourceNames[14] = "/Download3.exe";
	mResourceNames[15] = "/directory1/page1.html";
	mResourceNames[16] = "/directory1/download21.exe";
	mResourceNames[17] = "/directory1/movie21.rm";
	mResourceNames[18] = "/directory2/page2.html";
	mResourceNames[19] = "/directory3/directory31/page22.html";
	mResourceNames[20] = "/directory4/1234/page22.html";
	mResourceNames[21] = "/directory4/1234/download21.exe";
	mResourceNames[22] = "/directory4/1234/picture21.jpg";
	mResourceNames[23] = "/directory4/1234/movie21.rm";
	mResourceNames[24] = "/directory5/one-two/page31.html";
	mResourceNames[25] = "/网页_1.html";
	mResourceNames[26] = "/网页_2.html";
	mResourceNames[27] = "/网页_3.html";
	mResourceNames[28] = "/照片_1.jpg";
	mResourceNames[29] = "/照片_2.jpg";
	mResourceNames[30] = "/照片_3.jpg";
	mResourceNames[31] = "/照片_4.png";
	mResourceNames[32] = "/照片_5.png";
	mResourceNames[33] = "/照片_6.png";
	mResourceNames[34] = "/电影-英雄.rm";
	mResourceNames[35] = "/电影-十面埋伏.rm";
	mResourceNames[36] = "/电影-佐罗传奇.rm";
	mResourceNames[37] = "/目录 1/网页11.html";
	mResourceNames[38] = "/目录-二/文件夹三/照片_4.jpg";
	mResourceNames[39] = "/目录/文件夹/子目录/末目录/电影-金刚.rm";
*/

	mSourceOnly518[0] = false;
	mSourceOnly518[1] = false;
	mSourceOnly518[2] = false;
	mSourceOnly518[3] = false;

/*	mSourceOnly518[0] = false;
	mSourceOnly518[1] = false;
	mSourceOnly518[2] = false;
	mSourceOnly518[3] = false;
	mSourceOnly518[4] = false;
	mSourceOnly518[5] = true;
	mSourceOnly518[6] = true;
	mSourceOnly518[7] = true;
	mSourceOnly518[8] = true;
	mSourceOnly518[9] = true;
	mSourceOnly518[10] = true;
	mSourceOnly518[11] = true;
	mSourceOnly518[12] = true;
	mSourceOnly518[13] = true;
	mSourceOnly518[14] = true;
	mSourceOnly518[15] = true;
	mSourceOnly518[16] = true;
	mSourceOnly518[17] = true;
	mSourceOnly518[18] = true;
	mSourceOnly518[19] = true;
	mSourceOnly518[20] = false;
	mSourceOnly518[21] = false;
	mSourceOnly518[22] = false;
	mSourceOnly518[23] = false;
	mSourceOnly518[24] = false;
	mSourceOnly518[25] = false;
	mSourceOnly518[26] = false;
	mSourceOnly518[27] = false;
	mSourceOnly518[28] = true;
	mSourceOnly518[29] = true;
	mSourceOnly518[30] = true;
	mSourceOnly518[31] = true;
	mSourceOnly518[32] = true;
	mSourceOnly518[33] = true;
	mSourceOnly518[34] = true;
	mSourceOnly518[35] = true;
	mSourceOnly518[36] = true;
	mSourceOnly518[37] = false;
	mSourceOnly518[38] = false;
	mSourceOnly518[39] = false;
*/
	// set ipaddr for proxyclient. proxyserver, bouncer devices
	//mTrafficGen_IP =  trafficGenIp;
	//mClientProxy_IP =  clientIp;
	mServerProxy_IP = serverIp;
	mBouncer_IP = bouncerIp;
	mPMIServer_IP = PMIServerIp;
}

bool
AosSinglePmiTester::start()
{
    setInitialEnv(false);
	integrateTest();
//	capacityTest();
//	abnormalTest();

    return true;
}

bool
AosSinglePmiTester::setInitialEnv(const bool sendInitMsg)
{
	// construct tcpclient for proxyclient. proxyserver and bouncer
	mProxyServerConn = new OmnTcpClient("proxyserver", mServerProxy_IP, eAosSinglePmi_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
//	mPMIServerConn = new OmnTcpClient("PMIServer", mPMIServer_IP, eAosSinglePmi_CliDeamonListenPort,
//												1, OmnTcp::eNoLengthIndicator);

	
//	mProxyClientConn = new OmnTcpClient("proxyclient", mClientProxy_IP, eAosSinglePmi_CliDeamonListenPort,
//												1, OmnTcp::eNoLengthIndicator);

	for(int i = 0; i < eAosSinglePmi_MAX_SN; i++)
	{
		OmnString name = "proxyclient ";
		name << i+1;
		mProxyClients517Conn[i] = new OmnTcpClient(name, mSnAddr[i], eAosSinglePmi_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	}
	
	
	OmnString errmsg;
	mProxyServerConn->connect(errmsg);
//	mPMIServerConn->connect(errmsg);

	for(int i=0;i < eAosSinglePmi_MAX_SN;i ++)
	{
		mProxyClients517Conn[i]->connect(errmsg);
	}
	//
	//	PROXY CLIENT SIDE CONFIGURATION
	//
	//	a. add a virtual service
	//

/*	if(!sendInitMsg)
	{
		return true;
	}*/
	
	for(int i=0;i < eAosSinglePmi_MAX_SN;i ++)
	{
		mProxyClientConn = mProxyClients517Conn[i];

		sendCmd("clear config", mProxyClientConn);
		OmnSleep(3);
		sendCmd("ssl hardware on", mProxyClientConn);

		OmnString appProxyAddCmd("app proxy add clientvs1 http ");
		appProxyAddCmd << mSnAddr[i].toString() << " 8001 plain";

		sendCmd(appProxyAddCmd, mProxyClientConn);
		sendCmd(OmnString("app proxy rs add clientvs1 192.168.0.235 4431"), mProxyClientConn);
		sendCmd(OmnString("app proxy ssl backend clientvs1 on"), mProxyClientConn);
		sendCmd(OmnString("app proxy set status clientvs1 on"), mProxyClientConn);
/*		//
		//	b. import three valid usb certificate, three valid 518 certificate
		//
		sendCmd(OmnString("cert manager import filecert db 518valid1 /temp/518valid1.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db 518valid2 /temp/518valid2.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db 518valid3 /temp/518valid3.crt"), mProxyClientConn);
		
		sendCmd(OmnString("cert manager import filecert db 518invalid1 /temp/518invalid1.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db 518invalid2 /temp/518invalid2.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db 518invalid3 /temp/518invalid3.crt"), mProxyClientConn);
		
		sendCmd(OmnString("cert manager import filecert db usbvalid1 /temp/usbvalid1.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db usbvalid2 /temp/usbvalid2.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db ubsvalid3 /temp/usbvalid3.crt"), mProxyClientConn);
		
		sendCmd(OmnString("cert manager import filecert db usbinvalid1 /temp/usbvalid1.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db usbinvalid2 /temp/usbvalid2.crt"), mProxyClientConn);
		sendCmd(OmnString("cert manager import filecert db usbinvalid3 /temp/usbvalid3.crt"), mProxyClientConn);	
*/		//
		//	c. import private keys at client side
		//
/*		sendCmd(OmnString("cert manager import file private key 518valid1 /temp/518valid1.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key 518valid2 /temp/518valid2.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key 518valid3 /temp/518valid3.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key 518invalid1 /temp/518invalid1.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key 518invalid2 /temp/518valid2.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key 518invalid3 /temp/518invalid3.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key usbvalid1 /temp/usbvalid1.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key usbvalid2 /temp/usbvalid2.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key usbvalid3 /temp/usbvalid3.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key usbinvalid1 /temp/usbinvalid1.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key usbinvalid2 /temp/usbinvalid2.key"), mProxyClientConn);
		sendCmd(OmnString("cert manager import file private key usbinvalid3 /temp/usbinvalid3.key"), mProxyClientConn);
		//
		//	d. import ca certificate at client side
		//
		sendCmd(OmnString("cert manager import filecert db ca /temp/ca.crt"), mProxyClientConn);
		//
		//	e. import a cert chain at client side
		//
		sendCmd(OmnString("cert chain add system ca"), mProxyClientConn);
		//	f. choice a certificate as system
		sendCmd(OmnString("cert manager setcert system 518valid1"), mProxyClientConn);
*/
		//	g. close ssl session reuse and open log filter as debug mode
		sendCmd(OmnString("ssl session reuse server off"), mProxyClientConn);
		sendCmd(OmnString("ssl session reuse client off"), mProxyClientConn);
		
		#ifdef SINGLE_PMI_TESTING_DEBUG 
		sendCmd(OmnString("log set filter all debug"), mProxyClientConn);
		#endif

	}	
	//	
	//	PROXY SERVER SIDE CONFIGURATION
		//	
		//	a. set a virtual service at proxyserver
		//
	sendCmd("clear config", mProxyServerConn);
	OmnSleep(3);
	sendCmd("ssl hardware on", mProxyServerConn);
	
	sendCmd("app proxy add servervs1 http 192.168.0.235 4431 ssl",mProxyServerConn);
	sendCmd("app proxy ssl clientauth servervs1 on",mProxyServerConn);

	OmnString appProxyRsAdd("app proxy rs add servervs1 ");
	appProxyRsAdd << mBouncer_IP.toString() << " 8001";
	sendCmd(appProxyRsAdd, mProxyServerConn);

	sendCmd("app proxy set status servervs1 on", mProxyServerConn);
		//
		//	b. set a full cipher suites list to proxyserver
		//
//	sendCmd(OmnString("ssl set ciphers server TLS_RSA_WITH_NULL_MD5 TLS_RSA_WITH_RC4_128_MD5 "
//						"TLS_RSA_WITH_RC4_128_SHA TLS_RSA_WITH_DES_CBC_SHA JNSTLS_RSA_WITH_NULL_MD5"
//						"JNSTLS_RSA_WITH_NULL_SHA"), mProxyServerConn);
		//				
		//	c. set global pmi 
		//
	OmnString ammSetAddr("amm set addr ");
	ammSetAddr << mPMIServer_IP.toString() << " 7408";
	sendCmd(ammSetAddr , mProxyServerConn);

	sendCmd(OmnString("amm set timeout 100"), mProxyServerConn);
	sendCmd(OmnString("amm status on"), mProxyServerConn);
		//
		//	d. set pmi on virtual service
		//
	sendCmd(OmnString("app proxy set pmi servervs1 on"), mProxyServerConn);
		//
		//	e. import a certificate and its private key at server side
		//
/*	sendCmd(OmnString("cert manager import filecert system 518valid1 /temp/518valid1.crt"), mProxyServerConn);
	sendCmd(OmnString("cert manager import file private key 518valid1 /temp/518valid1.key"), mProxyServerConn);
		//
		//	f. import ca certificate and certificate chain at server side 
		//
	sendCmd(OmnString("cert manager import filecert db ca /temp/ca.crt"), mProxyServerConn);
	sendCmd(OmnString("cert chain add system ca"), mProxyServerConn);
*/		//
		//	g. close ssl session reuse and open log filter as debug mode
		//
	sendCmd(OmnString("ssl session reuse client off"), mProxyServerConn);
	sendCmd(OmnString("ssl session reuse server off"), mProxyServerConn);
	
	#ifdef SINGLE_PMI_TESTING_DEBUG 
	sendCmd(OmnString("log set filter all debug"), mProxyServerConn);
	#endif
	
	//	wait 2 seconds until configurations become effective
	OmnSleep(2);
	
	return true;
}

bool
AosSinglePmiTester::integrateTest()
{
	int runnedNum = 0;
	genStaticData1();
	mPostTestPlan = mCurTestPlan;
//	mDataStatus.resetChangeFlag(true);

	setIntegrateEnv();
	while(runnedNum++ < mTestNum && mNumTcsFailed < eAosSinglePmi_MaxFailedTester)
	{
cout << "=================================Round " << runnedNum << "=======================================" << endl;
		mDataStatus.resetChangeFlag(false);
		mPostTestPlan = mCurTestPlan;

		//
		// Generate random test data this time 
		genRandomData();
//		genStaticData1();

		showTestPlan(mCurTestPlan);
		//
		// To set proxyclient and proxyserver
		setIntegrateEnv();

		//
		// Launch Generator
		// generate conn, repeat
		#ifndef SINGLE_PMI_TESTING_DEBUG
		mNumConns = OmnRandom::nextInt(1, 100);		
		mRepeat = OmnRandom::nextInt(1, 2);
		#endif

		#ifdef SINGLE_PMI_TESTING_DEBUG
		mNumConns = 1;		
		mRepeat = 1;	
		#endif
		//
		// check results
		bool shouldAllow = checkPmiResults();
cout << "Num conns:"<< mNumConns<< endl;
cout << "Num repeat:"<< mRepeat<< endl;
		launchTraffic(mConnNum, mRepeatNum);

		if (shouldAllow)
	    {
	        OmnTC(OmnExpected<int>(mConnNum*mRepeatNum), OmnActual<int>(mSuccessConns)) << "\nwrong rslt !!!\n" << endtc;
	        OmnTC(OmnExpected<int>(0), OmnActual<int>(mFailedConns)) << "\nwrong rslt !!!\n" << endtc;
	    }
	    else
	    {
	        OmnTC(OmnExpected<int>(0), OmnActual<int>(mSuccessConns)) << "\nwrong rslt !!!\n" << endtc;
	        OmnTC(OmnExpected<int>(mConnNum*mRepeatNum), OmnActual<int>(mFailedConns)) << "\nwrong rslt !!!\n" << endtc;
	    }
	    
	    #ifdef SINGLE_PMI_TESTING_DEBUG
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
		
	    
/*		cout << "--------------------------------------" << endl;
	    if (shouldAllow)
	    {
			cout << "This Plan Should Allow" << endl;
		}
		else
		{
			cout << "This Plan Shouldn't Allow" << endl;
		}
		cout << "Now failed connections is :" << mFailedConns << endl;
		cout << "And successed connections is :" << mSuccessConns << endl;
		cout << "\n--------------------------------------" << endl;
		
		OmnSleep(2);
*/	}
	
	return true;
}

bool
AosSinglePmiTester::capacityTest()
{

	return true;
}

bool
AosSinglePmiTester::abnormalTest()
{

	return true;
}

bool
AosSinglePmiTester::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
{
	OmnString errmsg;
	OmnRslt rslt;
	int sec = eAosSinglePmi_CliTimeOut;
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

void
AosSinglePmiTester::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff)
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

bool
AosSinglePmiTester::checkPmiResults()
{
	// 
	// Check whether the test should be "allowed" or "rejected".
	//
	if (!mCurTestPlan.mOrgValidity || 
		!mCurTestPlan.mSysValidity || 
		!mCurTestPlan.mSNValidity || 
		!mCurTestPlan.mResourceValidity )
	{
		return false;
	}

	// 
	// Now, the Org, Sys, SN, and resource are all valid. We need to determine
	// the "allow" or "reject" based on the actual data. 
	// 
	if(!mCurTestPlan.mIsResourceOnly518)
	{
		return true;
	}
	
	if(mCurTestPlan.mIs518SN && mCurTestPlan.mIsResourceOnly518)
	{
		return true;
	}
	
	if(!mCurTestPlan.mIs518SN && mCurTestPlan.mIsResourceOnly518)
	{
		//
		// for now allow to connect
		//
		return true;
		//return false;
		
	}

	return true;
}

bool
AosSinglePmiTester::launchTraffic(int conn, int repeat)
{
    // start generator, generate a connection
    AosTcpTrafficGen clientGen(001,
    						   mLocalAddr,
							   mCurTestPlan.mSNAddr,//OmnIpAddr("192.168.1.55"),//mClientProxy_IP, 
    						   8001, 
    						   1, 
    						   repeat, 
    						   conn, 
    						   mCurTestPlan.mResource, 
    						   mCurTestPlan.mResource.length(),
                               eAosSendBlockType_Fixed, 
                               mSendBlockSize, 
                               mCurTestPlan.mResource.length());
    clientGen.start();
//	while(0)
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
AosSinglePmiTester::genRandomData()
{
	// 1. Get org name
	// 1.1 count 100, random(1,10), 30% invalid probability
	
	if (++mDataStatus.mOrgCounter >= eAosSinglePmi_LoopSize_Org && 
		eAosSinglePmi_MAX_ORGNAMES > 1)
	{
		mDataStatus.mOrgCounter = 0;
		mDataStatus.mOrgChanged = true;
		// generate Org
		if (OmnRandom::nextInt(1,100) <= eAosSinglePmi_VALID_ORG_PERCENT)
		{
 			// choice an valid orgnization name
			mCurTestPlan.mOrgValidity = true;
			mCurTestPlan.mOrgNo = OmnRandom::nextInt(0, eAosSinglePmi_MAX_ORGNAMES-1);
        	mCurTestPlan.mOrg = mOrgNames[mCurTestPlan.mOrgNo];
		}
		else
		{
			// random choice an invalid orgnization name
			mCurTestPlan.mOrgValidity = false;
			mCurTestPlan.mOrg = OmnRandom::nextMixedString(1, 30);	
		}

	}
			
	// 2. Get sys name
	// 2.1 count 100, random(1,10), 30% invalid probability
	if (++mDataStatus.mSysCounter >= eAosSinglePmi_LoopSize_Sys &&
		eAosSinglePmi_MAX_SYSNAMES > 1)
	{
		mDataStatus.mSysCounter = 0;
		mDataStatus.mSysChanged = true;
		// generate Sys
		if (OmnRandom::nextInt(1,100) <= eAosSinglePmi_VALID_SYS_PERCENT)
		{
			// choice an valid system name
			mCurTestPlan.mSysValidity = true;
			mCurTestPlan.mSysNo = OmnRandom::nextInt(0, eAosSinglePmi_MAX_SYSNAMES-1);
        	mCurTestPlan.mSys = mSysNames[mCurTestPlan.mSysNo];
		}
		else
		{
			// random choice an invalid system name
			mCurTestPlan.mSysValidity = false;
			mCurTestPlan.mSys = OmnRandom::nextMixedString(1, 30);	
		}

	}
		
	// 3. Get SN
	// 3.1 count 10, random(0,1), 0 means invalid sn, 1 means valid sn
	if (++mDataStatus.mSNCounter >= eAosSinglePmi_LoopSize_SN)
	{
		mDataStatus.mSNCounter = 0;

//		mDataStatus.mSNChanged = true;
		//
		// If we change SN , that means we need to change to another 517,
		// we should set all configuration to the selected 517.
		// Just set all changeFlag and function setIntegrateEnv() will 
		// help us to do the remained.
		//
		mDataStatus.resetChangeFlag(true);
		

		// generate SN . If no inValid sn, just generate valid
		if (OmnRandom::nextInt(1,100) <= eAosSinglePmi_VALID_SN_PERCENT ||
			(eAosSinglePmi_VALID_SN_NO >= eAosSinglePmi_MAX_SN))
		{
			// choice an valid SN
			mCurTestPlan.mSNValidity = true;
			mCurTestPlan.mSNNo = OmnRandom::nextInt(0, eAosSinglePmi_VALID_SN_NO-1);
        	mCurTestPlan.mSNAddr = mSnAddr[mCurTestPlan.mSNNo];
		    mCurTestPlan.mIs518SN = mAllowEncrypt[mCurTestPlan.mSNNo];
/*			mCurTestPlan.mSNValidity = true;
			int snIndex = OmnRandom::nextInt(0, eAosSinglePmi_MAX_SN-1);
        	mCurTestPlan.mSN = mValidSNs[snIndex];
        	snNo = snIndex;
		    mCurTestPlan.mIs518SN = mAllowEncrypt[snNo];
*/
		}
		else
		{
			// random choice an invalid orgnization name
			mCurTestPlan.mSNValidity = false;
			mCurTestPlan.mSNNo = OmnRandom::nextInt(eAosSinglePmi_VALID_SN_NO, eAosSinglePmi_MAX_SN-1);
        	mCurTestPlan.mSNAddr = mSnAddr[mCurTestPlan.mSNNo];
		    mCurTestPlan.mIs518SN = mAllowEncrypt[mCurTestPlan.mSNNo];

/*			mCurTestPlan.mSNValidity = false;
			int snIndex = OmnRandom::nextInt(0, eAosSinglePmi_MAX_SN-1);
        	mCurTestPlan.mSN = mInvalidSNs[snIndex];
        	mCurTestPlan.mIs518SN = false;
*/
		}

	}
	    
	// 4. Get src
	// 4.1 random(0,1), 0 means invalid resource, 1 means valid resource 
	if (++mDataStatus.mResourceCounter >= eAosSinglePmi_LoopSize_Resource)
	{
		mDataStatus.mResourceCounter = 0;
		mDataStatus.mResourceChanged = true;
		// generate Sys
		if (OmnRandom::nextInt(1,100) <= eAosSinglePmi_VALID_RESOURCE_PERCENT)
		{
			// choice an valid system name
			mCurTestPlan.mResourceValidity = true;
			int resourceIndex = OmnRandom::nextInt(0, eAosSinglePmi_MAX_RESOURCE-1);
        	setHttpMsg(mResourceNames[resourceIndex], mCurTestPlan.mResource);
		    mCurTestPlan.mIsResourceOnly518 = mSourceOnly518[resourceIndex];
		}
		else
		{
			// random choice an invalid system name
			mCurTestPlan.mResourceValidity = false;
			if(OmnRandom::nextInt(1,100) <= eAosSinglePmi_NOT_HTML_RESOURCE_PERCENT)
			{
	        	mCurTestPlan.mResource = +OmnRandom::nextMixedString(1, 30);
			}
			else
			{
				OmnString strSend("/");
				strSend += OmnRandom::nextMixedString(1, 30);
cout <<"\n\nsend resource is : "<< strSend << "\n\n" << endl;
	        	setHttpMsg(strSend, mCurTestPlan.mResource);
			}
        	mCurTestPlan.mIsResourceOnly518 = false;
		}
	}
                                                                                                                                                                           
	return true;
}

bool
AosSinglePmiTester::genStaticData1()
{
	// 1. Get org name
	mDataStatus.resetChangeFlag(true);

	// generate Org
	mCurTestPlan.mOrgValidity = true;
	mCurTestPlan.mOrgNo = 0;
	mCurTestPlan.mOrg = mOrgNames[mCurTestPlan.mOrgNo];
			
	// 2. Get sys name
	mCurTestPlan.mSysValidity = true;
	mCurTestPlan.mSysNo = 0;
	mCurTestPlan.mSys = mSysNames[mCurTestPlan.mSysNo];
		
	// 3. Get SN
	mCurTestPlan.mSNValidity = true;
	mCurTestPlan.mSNNo = 0;
   	mCurTestPlan.mSNAddr = mSnAddr[mCurTestPlan.mSNNo];
mCurTestPlan.mIs518SN = mSourceOnly518[mCurTestPlan.mSNNo];
	    
	// 4. Get src
	mCurTestPlan.mResourceValidity = true;
	setHttpMsg(mResourceNames[0],mCurTestPlan.mResource);
    mCurTestPlan.mIsResourceOnly518 = mSourceOnly518[0];
                                                                                                                                                                           
	return true;
}

bool
AosSinglePmiTester::setIntegrateEnv()
{
	if (mCurTestPlan.mSNNo >= eAosSinglePmi_MAX_SN || mCurTestPlan.mSNNo < 0)
	{
		OmnAlarm << "SNNo is wrong:" << mCurTestPlan.mSNNo << enderr;
		return false;
	}
	
	mProxyClientConn = mProxyClients517Conn[mCurTestPlan.mSNNo];
	
/*	if(mDataStatus.mSNChanged)
	{
		// set sn
		// c. Need to configure the certificate.
		OmnString cmdToClient("cert manager setcert system ");
		cmdToClient << mCurTestPlan.mSN;
		sendCmd(cmdToClient, mProxyClientConn);
				
		// set operation method(518/usb)
		// set proxyclient environments for integrate test
//		if (mPostTestPlan.mIs518SN && !mCurTestPlan.mIs518SN)
		if (!mCurTestPlan.mIs518SN)
		{
			sendCmd(OmnString("ssl set ciphers client TLS_RSA_WITH_NULL_MD5"), mProxyClientConn);
		}
	
//		if (!mPostTestPlan.mIs518SN && mCurTestPlan.mIs518SN)
		if (mCurTestPlan.mIs518SN)
		{
			sendCmd(OmnString("ssl set ciphers client TLS_RSA_WITH_RC4_128_MD5 TLS_RSA_WITH_RC4_128_SHA TLS_RSA_WITH_DES_CBC_SHA JNSTLS_RSA_WITH_NULL_MD5 JNSTLS_RSA_WITH_NULL_SHA"), mProxyClientConn);
		}

	}	*/
	// set proxyserver environments for integrate test
	// a. set orgnization name
	if(mDataStatus.mOrgChanged)
	{
		OmnString cmd("amm set org ");
		cmd << mCurTestPlan.mOrg;
		sendCmd(cmd, mProxyServerConn);
	}	
	
	// b. send system name
	if(mDataStatus.mSysChanged)
	{
		OmnString cmd("app proxy pmi sysname servervs1 ");
		cmd << mCurTestPlan.mSys;
		sendCmd(cmd, mProxyServerConn);
	}	
	
	return true;
}

bool	
AosSinglePmiTester::showTestPlan(const aosTestPlan &plan)
{
	cout << "\n---------------Test Plan--------------" << endl;
	cout << "Org:		" << plan.mOrg << endl;
	cout << "Sys:		" << plan.mSys << endl;     
	cout << "SNAddr:	" << plan.mSNAddr.toString() << endl;      
	cout << "Resource:	" << plan.mResource << endl;
	cout << "mIs518SN:		" << plan.mIs518SN			<< endl;
	cout << "mIsResourceOnly518:" << plan.mIsResourceOnly518 << endl;
	cout << "mOrgValidity:		" << plan.mOrgValidity		<< endl;
	cout << "mSysValidity:		" << plan.mSysValidity		<< endl;
	cout << "mSNValidity:		" << plan.mSNValidity		<< endl;
	cout << "mResourceValidity:	" << plan.mResourceValidity	<< endl;
	cout << "\n-------------Test Plan End------------\n";

	return true;	
}

void
AosSinglePmiTester::setHttpMsg(const OmnString &resStr,OmnString &fullResource)
{
	fullResource = "GET "; 
	fullResource << resStr << " HTTP/1.1\r\n";
	fullResource << "Accept: */*\r\n";
	fullResource << "UA-CPU: x86\r\n";
	fullResource << "Accept-Encoding: gzip, deflate\r\n";
	fullResource << "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; NET CLR 1.1.4322)\r\n";
	fullResource << "Host: 192.168.1.240\r\n";
	fullResource << "Connection: Keep-Alive\r\n";
	fullResource << "\r\n";
	return;
}
