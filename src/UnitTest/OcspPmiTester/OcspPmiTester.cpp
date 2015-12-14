////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspPmiTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/OcspPmiTester/OcspPmiTester.h"

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
#include "UnitTest/OcspPmiTester/Ptrs.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"

AosOcspPmiTester::aosTestPlan::aosTestPlan()
:
mOcspStatus(false),
mCrlStatus(false),
mClientAuthStatus(false),
mConnOcspSrvStatus(false),
mOcspSrvResponse(false),
mPmiStatus(false),
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
AosOcspPmiTester::aosTestPlan::resetValidityFlag()
{
	mOrgValidity = false;
	mSysValidity = false;
	mSNValidity = false;
	mResourceValidity = false;
}

AosOcspPmiTester::aosTestDataStatus::aosTestDataStatus()
:
mOrgCounter(0),
mSysCounter(0),
mSNCounter(0),
mResourceCounter(0),
mOperationCounter(0),
mOcspStatusCounter(0),
mCrlStatusCounter(0),
mClientAuthStatusCounter(0),
mConnOcspSrvStatusCounter(0),
mOcspSrvResponseCounter(0),
mPmiStatusCounter(0),
mOrgChanged(false),
mSysChanged(false),
mSNChanged(false),
mResourceChanged(false),
mOperationChanged(false),
mOcspStatusChanged(false),
mCrlStatusChanged(false),
mClientAuthStatusChanged(false),
mConnOcspSrvStatusChanged(false),
mOcspSrvResponseChanged(false),
mPmiStatusChanged(false)
{
}

void
AosOcspPmiTester::aosTestDataStatus::resetChangeFlag(const bool flag)
{
	mOrgChanged	= flag;
	mSysChanged = flag;
	mSNChanged = flag;
	mResourceChanged = flag;
	mOperationChanged = flag;
	mOcspStatusChanged= flag;
	mCrlStatusChanged= flag;
	mClientAuthStatusChanged= flag;
	mConnOcspSrvStatusChanged= flag;
	mOcspSrvResponseChanged= flag;
	mPmiStatusChanged = flag;
}

AosOcspPmiTester::AosOcspPmiTester(
			const OmnIpAddr &clientIp, 
			const OmnIpAddr &serverIp,
			const OmnIpAddr &bouncerIp,
			const OmnIpAddr &PMIServerIp)
:
mTestNum(eAosOcspPmi_TestTime),
mConnNum(eAosOcspPmi_ConnNum),
mRepeatNum(eAosOcspPmi_RepeatTime),
mSuccessConns(0),
mFailedConns(0),
sendBlockSize(1000)
{   
	mName = "AosOcspPmiTester";	
	initData(clientIp, serverIp, bouncerIp, PMIServerIp);
} 

AosOcspPmiTester::~AosOcspPmiTester()
{
} 

void
AosOcspPmiTester::initData(const OmnIpAddr &trafficGenIp, 
											const OmnIpAddr &serverIp,
											const OmnIpAddr &bouncerIp,
											const OmnIpAddr &PMIServerIp)
{
	mOrgNames[0] =  "/I=公安部";
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
	mSnAddr[0] = OmnIpAddr("192.168.1.232");
	mSnAddr[1] = OmnIpAddr("192.168.1.231");
//	mSnAddr[2] = OmnIpAddr("192.168.1.232");

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
	mTrafficGen_IP =  trafficGenIp;
	//mClientProxy_IP =  clientIp;
	mServerProxy_IP = serverIp;
	mBouncer_IP = bouncerIp;
	mPMIServer_IP = PMIServerIp;
}

bool
AosOcspPmiTester::start()
{
    setInitialEnv(false);
	integrateTest();
	capacityTest();
	abnormalTest();

    return true;
}

bool
AosOcspPmiTester::setInitialEnv(const bool sendInitMsg)
{
	// construct tcpclient for proxyclient. proxyserver and bouncer
//	mProxyClientConn = new OmnTcpClient("proxyclient", mClientProxy_IP, eAosOcspPmi_CliDeamonListenPort,
//												1, OmnTcp::eNoLengthIndicator);
	mProxyServerConn = new OmnTcpClient("proxyserver", mServerProxy_IP, eAosOcspPmi_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	mPMIServerConn = new OmnTcpClient("PMIServer", mPMIServer_IP, eAosOcspPmi_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);

	
//	mProxyClientConn = new OmnTcpClient("proxyclient", mClientProxy_IP, eAosOcspPmi_CliDeamonListenPort,
//												1, OmnTcp::eNoLengthIndicator);

	for(int i=0;i < eAosOcspPmi_MAX_SN;i ++)
	{
		OmnString name = "proxyclient ";
		name << i+1;
		mProxyClients517Conn[i] = new OmnTcpClient(name, mSnAddr[i], eAosOcspPmi_CliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	}
	
	
	OmnString errmsg;
//	mProxyClientConn->connect(errmsg);
	mProxyServerConn->connect(errmsg);
	mPMIServerConn->connect(errmsg);

	for(int i=0;i < eAosOcspPmi_MAX_SN;i ++)
	{
		mProxyClients517Conn[i]->connect(errmsg);
	}
	//
	//	PROXY CLIENT SIDE CONFIGURATION
	//
	//	a. add a virtual service
	//

	if(!sendInitMsg)
	{
		return true;
	}
	
	for(int i=0;i < eAosOcspPmi_MAX_SN;i ++)
	{
		mProxyClientConn = mProxyClients517Conn[i];

		sendCmd("clear config", mProxyClientConn);
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
		sendCmd(OmnString("log set filter all debug"), mProxyClientConn);
	}	
	//	
	//	PROXY SERVER SIDE CONFIGURATION
		//	
		//	a. set a virtual service at proxyserver
		//
	sendCmd(OmnString("clear config"), mProxyServerConn);
	sendCmd(OmnString("ssl hardware on"), mProxyServerConn);
	
	sendCmd(OmnString("app proxy add servervs1 http 192.168.0.235 4431 ssl"),mProxyServerConn);
	sendCmd(OmnString("app proxy ssl clientauth servervs1 on"),mProxyServerConn);

	OmnString appProxyRsAdd("app proxy rs add servervs1 ");
	appProxyRsAdd << mBouncer_IP.toString() << " 8001";
	sendCmd(appProxyRsAdd, mProxyServerConn);
//sendCmd("app proxy rs add servervs1 192.168.1.245 80",mProxyServerConn);

	sendCmd(OmnString("app proxy set status servervs1 on"), mProxyServerConn);
	//
	//	b. set a full cipher suites list to proxyserver
	//
//	sendCmd(OmnString("ssl set ciphers server TLS_RSA_WITH_NULL_MD5 TLS_RSA_WITH_RC4_128_MD5 "
//						"TLS_RSA_WITH_RC4_128_SHA TLS_RSA_WITH_DES_CBC_SHA JNSTLS_RSA_WITH_NULL_MD5"
//						"JNSTLS_RSA_WITH_NULL_SHA"), mProxyServerConn);


	sendCmd(OmnString("ocsp set addr 192.168.1.88 3904"), mProxyServerConn);	
	sendCmd(OmnString("ocsp status on"), mProxyServerConn);


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
	sendCmd(OmnString("log set filter all debug"), mProxyServerConn);
	
	#ifdef OCSP_CRL_TESTING_DEBUG
	sendCmd(OmnString("log set filter all debug"), mProxyServerConn);
	#endif

	#ifdef OCSP_CRL_TESTING_EASY
		//
		// g. change ocsp server response mode to easy mode for oscp/crl simulate testing
		//
	sendCmd(OmnString("ocsp set resp mode easymode"), mProxyServerConn);	
	#endif
	//	wait 2 seconds until configurations become effective
	OmnSleep(2);
	
	return true;
}

bool
AosOcspPmiTester::integrateTest()
{
	int runnedNum = 0;
	genStaticData1();
	mPostTestPlan = mCurTestPlan;
	mDataStatus.resetChangeFlag(true);

	setIntegrateEnv();
cout << "start testing now!" << endl;
	while(runnedNum++ < mTestNum && mNumTcsFailed < eAosOcspPmi_MaxFailedTester)
	{
cout << "=================================Round " << runnedNum << "=======================================" << endl;
		mDataStatus.resetChangeFlag(false);
		mPostTestPlan = mCurTestPlan;

		//
		// Generate random test data this time 
cout << "generate random data " << endl;
		genRandomData();
//		genStaticData1();
cout << "random data generated" << endl;
cout << endl;

		showTestPlan(mCurTestPlan);
		//
		// To set proxyclient and proxyserver
cout << "set integrate environment" << endl;
		setIntegrateEnv();
cout << "integrate environment has been set" << endl;
cout << endl;

		//
		// Launch Generator
		// generate conn, repeat
cout << "launch traffic" << endl;
		launchTraffic(mConnNum, mRepeatNum);
cout << "traffic finished" << endl;

		//
		// check results
		bool shouldAllow = checkAllResults();//checkPmiResults();

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
	    
		cout << "--------------------------------------" << endl;
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
	}
	
	return true;
}

bool
AosOcspPmiTester::capacityTest()
{

	return true;
}

bool
AosOcspPmiTester::abnormalTest()
{

	return true;
}

bool
AosOcspPmiTester::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
{
	OmnString errmsg;
	OmnRslt rslt;
	int sec = eAosOcspPmi_CliTimeOut;
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
AosOcspPmiTester::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff)
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
AosOcspPmiTester::checkAllResults()
{
	return checkOcspResults() && checkPmiResults();
}


bool
AosOcspPmiTester::checkOcspResults()
{
	// clientAuthentication off, allow
	if (!mCurTestPlan.mClientAuthStatus) 
	{
		return true;
	}
	
	// ocsp sw. and crl sw. both set as off, allow 
	if (!mCurTestPlan.mOcspStatus && !mCurTestPlan.mCrlStatus)
	{
		return true;
	}
	
	// invalid certificate(now ocsp or crl is on at least one ), disallow
	if (!mCurTestPlan.mSNValidity)
	{
		return false;	
	}
		
	// crl on, allow
	if (mCurTestPlan.mCrlStatus)
	{
		return true;
	}
	// crl off, ocsp on, ocsp conn. on, ocsp response on
	if (mCurTestPlan.mConnOcspSrvStatus && mCurTestPlan.mOcspSrvResponse)
	{
		return true;	
	}
	
	return false;
}


bool
AosOcspPmiTester::checkPmiResults()
{
	if(!mCurTestPlan.mPmiStatus)
	{
		return true;
	}
	
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
	}

	return true;
}

bool
AosOcspPmiTester::launchTraffic(int conn, int repeat)
{
    // start generator, generate a connection
    AosTcpTrafficGen clientGen(001, 
    						   mCurTestPlan.mSNAddr,//OmnIpAddr("192.168.1.55"),//mClientProxy_IP, 
    						   8001, 
    						   1, 
    						   repeat, 
    						   conn, 
    						   mCurTestPlan.mResource, 
    						   mCurTestPlan.mResource.length(),
                               eAosSendBlockType_Fixed, 
                               sendBlockSize, 
                               mCurTestPlan.mResource.length());
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
AosOcspPmiTester::genRandomData()
{
	// 1. Get org name
	// 1.1 count 100, random(1,10), 30% invalid probability
	
	if (++mDataStatus.mOrgCounter >= eAosOcspPmi_LoopSize_Org && 
		eAosOcspPmi_MAX_ORGNAMES > 1)
	{
		mDataStatus.mOrgCounter = 0;
		mDataStatus.mOrgChanged = true;
		// generate Org
		if (OmnRandom::nextInt(1,100) <= eAosOcspPmi_VALID_ORG_PERCENT)
		{
 			// choice an valid orgnization name
			mCurTestPlan.mOrgValidity = true;
			mCurTestPlan.mOrgNo = OmnRandom::nextInt(0, eAosOcspPmi_MAX_ORGNAMES-1);
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
	if (++mDataStatus.mSysCounter >= eAosOcspPmi_LoopSize_Sys &&
		eAosOcspPmi_MAX_SYSNAMES > 1)
	{
		mDataStatus.mSysCounter = 0;
		mDataStatus.mSysChanged = true;
		// generate Sys
		if (OmnRandom::nextInt(1,100) <= eAosOcspPmi_VALID_SYS_PERCENT)
		{
			// choice an valid system name
			mCurTestPlan.mSysValidity = true;
			mCurTestPlan.mSysNo = OmnRandom::nextInt(0, eAosOcspPmi_MAX_SYSNAMES-1);
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
	if (++mDataStatus.mSNCounter >= eAosOcspPmi_LoopSize_SN)
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
		if (OmnRandom::nextInt(1,100) <= eAosOcspPmi_VALID_SN_PERCENT ||
			(eAosOcspPmi_VALID_SN_NO >= eAosOcspPmi_MAX_SN))
		{
			// choice an valid SN
			mCurTestPlan.mSNValidity = true;
			mCurTestPlan.mSNNo = OmnRandom::nextInt(0, eAosOcspPmi_VALID_SN_NO-1);
        	mCurTestPlan.mSNAddr = mSnAddr[mCurTestPlan.mSNNo];
		    mCurTestPlan.mIs518SN = mAllowEncrypt[mCurTestPlan.mSNNo];
/*			mCurTestPlan.mSNValidity = true;
			int snIndex = OmnRandom::nextInt(0, eAosOcspPmi_MAX_SN-1);
        	mCurTestPlan.mSN = mValidSNs[snIndex];
        	snNo = snIndex;
		    mCurTestPlan.mIs518SN = mAllowEncrypt[snNo];
*/
		}
		else
		{
			// random choice an invalid orgnization name
			mCurTestPlan.mSNValidity = false;
			mCurTestPlan.mSNNo = OmnRandom::nextInt(eAosOcspPmi_VALID_SN_NO, eAosOcspPmi_MAX_SN-1);
        	mCurTestPlan.mSNAddr = mSnAddr[mCurTestPlan.mSNNo];
		    mCurTestPlan.mIs518SN = mAllowEncrypt[mCurTestPlan.mSNNo];

/*			mCurTestPlan.mSNValidity = false;
			int snIndex = OmnRandom::nextInt(0, eAosOcspPmi_MAX_SN-1);
        	mCurTestPlan.mSN = mInvalidSNs[snIndex];
        	mCurTestPlan.mIs518SN = false;
*/
		}

	}
	    
	// 4. Get src
	// 4.1 random(0,1), 0 means invalid resource, 1 means valid resource 
	if (++mDataStatus.mResourceCounter >= eAosOcspPmi_LoopSize_Resource)
	{
		mDataStatus.mResourceCounter = 0;
		mDataStatus.mResourceChanged = true;
		// generate Sys
		if (OmnRandom::nextInt(1,100) <= eAosOcspPmi_VALID_RESOURCE_PERCENT)
		{
			// choice an valid system name
			mCurTestPlan.mResourceValidity = true;
			int resourceIndex = OmnRandom::nextInt(0, eAosOcspPmi_MAX_RESOURCE-1);
        	setHttpMsg(mResourceNames[resourceIndex], mCurTestPlan.mResource);
		    mCurTestPlan.mIsResourceOnly518 = mSourceOnly518[resourceIndex];
		}
		else
		{
			// random choice an invalid system name
			mCurTestPlan.mResourceValidity = false;
			if(OmnRandom::nextInt(1,100) <= eAosOcspPmi_NOT_HTML_RESOURCE_PERCENT)
			{
	        	mCurTestPlan.mResource = OmnRandom::nextMixedString(1, 30);
			}
			else
			{
	        	setHttpMsg(OmnRandom::nextMixedString(1, 30), mCurTestPlan.mResource);
			}
        	mCurTestPlan.mIsResourceOnly518 = false;
		}
	}
                                         
	// new area 1                                                                                                                                                                           
	if (++mDataStatus.mOcspStatusCounter >= eAosOcspPmi_LoopSize_Ocsp_Status)
	{
		mDataStatus.mOcspStatusCounter = 0;
		mDataStatus.mOcspStatusChanged = true;
		// generate Sys
		mCurTestPlan.mOcspStatus = (OmnRandom::nextInt(1,100) <= eAosOcspPmi_OCSP_ON_PERCENT);
	}

	// new area 2                                                                                                                                                                           
	if (++mDataStatus.mCrlStatusCounter >= eAosOcspPmi_LoopSize_Crl_Status)
	{
		mDataStatus.mCrlStatusCounter = 0;
		mDataStatus.mCrlStatusChanged = true;
		// generate Sys
		mCurTestPlan.mCrlStatus = (OmnRandom::nextInt(1,100) <= eAosOcspPmi_CRL_ON_PERCENT);
	}

	// new area 3                                                                                                                                                                           
	if (++mDataStatus.mClientAuthStatusCounter >= eAosOcspPmi_LoopSize_Client_Auth_Status)
	{
		mDataStatus.mClientAuthStatusCounter = 0;
		mDataStatus.mClientAuthStatusChanged = true;
		// generate Sys
		mCurTestPlan.mClientAuthStatus = (OmnRandom::nextInt(1,100) <= eAosOcspPmi_CLIENTAUTH_ON_PERCENT);
	}

	// new area 5                                                                                                                                                                           
	if (++mDataStatus.mConnOcspSrvStatusCounter >= eAosOcspPmi_LoopSize_Conn_Ocsp_Srv_Status)
	{
		mDataStatus.mConnOcspSrvStatusCounter = 0;
		mDataStatus.mConnOcspSrvStatusChanged = true;
		// generate Sys
		mCurTestPlan.mConnOcspSrvStatus = (OmnRandom::nextInt(1,100) <= eAosOcspPmi_CONNOCSP_SRV_ON_PERCENT);
	}

	// new area 6                                                                                                                                                                           
	if (++mDataStatus.mOcspSrvResponseCounter >= eAosOcspPmi_LoopSize_Ocsp_Srv_Response_Status)
	{
		mDataStatus.mOcspSrvResponseCounter = 0;
		mDataStatus.mOcspSrvResponseChanged = true;
		// generate Sys
		mCurTestPlan.mOcspSrvResponse = (OmnRandom::nextInt(1,100) <= eAosOcspPmi_OCSP_SRV_RESPONSE_ON_PERCENT);
	}

	// new area 7                                                                                                                                                                           
	if (++mDataStatus.mPmiStatusCounter >= eAosOcspPmi_LoopSize_Pmi_Status)
	{
		mDataStatus.mPmiStatusCounter = 0;
		mDataStatus.mPmiStatusChanged = true;
		// generate Sys
		mCurTestPlan.mPmiStatus = (OmnRandom::nextInt(1,100) <= eAosOcspPmi_PMI_ON_PERCENT);
	}
	return true;
}

bool
AosOcspPmiTester::genStaticData1()
{
	// 1. Get org name
	// 1.1 count 1000, random(1,10), 30% invalid probability
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

	mCurTestPlan.mOcspStatus = true;
	mCurTestPlan.mCrlStatus = true;
	mCurTestPlan.mClientAuthStatus = true;
	mCurTestPlan.mConnOcspSrvStatus = true;
	mCurTestPlan.mOcspSrvResponse = true;
	mCurTestPlan.mPmiStatus = true;

	return true;
}

bool
AosOcspPmiTester::setIntegrateEnv()
{
	if(mCurTestPlan.mSNNo >= eAosOcspPmi_MAX_SN || mCurTestPlan.mSNNo < 0)
	{
		OmnAlarm << "SNNo is wrong:" << mCurTestPlan.mSNNo << enderr;
		return false;
	}
	
	mProxyClientConn = mProxyClients517Conn[mCurTestPlan.mSNNo];
	
	if(mDataStatus.mSNChanged)
	{
		// set sn
		// c. Need to configure the certificate.
/*		OmnString cmdToClient("cert manager setcert system ");
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
*/
	}	
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


	if(mDataStatus.mOcspStatusChanged)
	{
		if(mCurTestPlan.mOcspStatus)
		{
			sendCmd(OmnString("ocsp status on"), mProxyServerConn);
		}
		else
		{
			sendCmd(OmnString("ocsp status off"), mProxyServerConn);
		}
	}
	
	if(mDataStatus.mCrlStatusChanged)
	{
		if(mCurTestPlan.mCrlStatus)
		{
			sendCmd(OmnString("crl status on"), mProxyServerConn);
		}
		else
		{
			sendCmd(OmnString("crl status off"), mProxyServerConn);
		}
	}

	if(mDataStatus.mClientAuthStatusChanged)
	{
		if(mCurTestPlan.mCrlStatus)
		{
 			sendCmd(OmnString("app proxy set status servervs1 off"), mProxyServerConn);
			OmnSleep(5);
 			sendCmd(OmnString("app proxy ssl clientauth servervs1 on"), mProxyServerConn);
 			sendCmd(OmnString("app proxy set status servervs1 on"), mProxyServerConn);
		}
		else
		{
 			sendCmd(OmnString("app proxy set status servervs1 off"), mProxyServerConn);
			OmnSleep(5);
 			sendCmd(OmnString("app proxy ssl clientauth servervs1 off"), mProxyServerConn);
 			sendCmd(OmnString("app proxy set status servervs1 on"), mProxyServerConn);
		}
	}

/*		
	if(mDataStatus.mConnOcspSrvStatusChanged)
	{
		if(mCurTestPlan.mConnOcspSrvStatus)
		{
			sendCmd("OCSPserver start", mOcspCrlServerConn);
		}
		else
		{
			sendCmd("OCSPserver stop", mOcspCrlServerConn);
		}
	}

	if(mDataStatus.mOcspSrvResponseChanged)
	{
		if(mCurTestPlan.mOcspSrvResponse)
		{
			sendCmd("OCSPserver resp on", mOcspCrlServerConn);
		}
		else
		{
			sendCmd("OCSPserver resp off", mOcspCrlServerConn);
		}
	}
*/
	if(mDataStatus.mPmiStatusChanged)
	{
		if(mCurTestPlan.mPmiStatus)
		{
 			sendCmd(OmnString("app proxy set pmi servervs1 on"), mProxyServerConn);
		}
		else
		{
 			sendCmd(OmnString("app proxy set pmi servervs1 off"), mProxyServerConn);
		}
	}
	
		
	return true;
}


bool	
AosOcspPmiTester::showTestPlan(const aosTestPlan &plan)
{
	cout << "\n---------------Test Plan--------------" << endl;
	cout << "Org:		" << plan.mOrg << endl;
	cout << "Sys:		" << plan.mSys << endl;     
	cout << "SNAddr:	" << plan.mSNAddr.toString() << endl;      
	cout << "Resource:	" << plan.mResource << endl;
	cout << "mIs518SN:			" << plan.mIs518SN			<< endl;
	cout << "mIsResourceOnly518:" << plan.mIsResourceOnly518 << endl;
	cout << "mOrgValidity:		" << plan.mOrgValidity		<< endl;
	cout << "mSysValidity:		" << plan.mSysValidity		<< endl;
	cout << "mSNValidity:		" << plan.mSNValidity		<< endl;
	cout << "mResourceValidity:	" << plan.mResourceValidity	<< endl;
	cout << "\n-------------Test Plan End------------\n";

	return true;	
}

void
AosOcspPmiTester::setHttpMsg(const OmnString &resStr,OmnString &fullResource)
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
