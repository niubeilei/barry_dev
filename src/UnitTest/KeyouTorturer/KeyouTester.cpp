////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KeyouTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/KeyouTorturer/KeyouTester.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/TestVariable.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServer.h"
#include "UnitTest/KeyouTorturer/KeyouTestCmd.h"
#include "UnitTest/KeyouTorturer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"

//#define   SINGLE_PMI_SOFT_TESTING
#define   SINGLE_PMI_TESTING_DEBUG 

AosKeyouTester::AosKeyouTester(const OmnIpAddr 	&localAddr,
							   const OmnIpAddr 	&proxyIP,
							   const int		&ftpPort,
							   const int		&ftpDataPort,
							   const int		&telnetPort)
:
mTestNum(eTestTime),
mSuccessConns(0),
mFailedConns(0),
//mSendBlockSize(1000),
mLocalAddr(localAddr),
mProxyIP(proxyIP),
mFtpPort(ftpPort),
mFtpDataPort(ftpDataPort),
mTelnetPort(telnetPort),
mTestMode(eFtp)
{   
	mName = "AosKeyouTester";
} 

AosKeyouTester::~AosKeyouTester()
{
} 

bool
AosKeyouTester::initData()
{
	if(eFtpTestRate	>= RAND_INT(1,100))
	{
		mTestMode = eFtp;
	}
	else
	{
		mTestMode = eTelnet;
	}
	
	if(eFtp ==mTestMode)
	{
		mProxyConn 	= new OmnTcpClient("proxy", mProxyIP, mFtpPort,
												1, OmnTcp::eNoLengthIndicator);
		mFtpDataSrv	= new OmnTcpServer(mLocalAddr, mFtpDataPort, 1, "ftpData");
	}
	else
	{
		mProxyConn = new OmnTcpClient("proxy", mProxyIP, mTelnetPort,
												1, OmnTcp::eNoLengthIndicator);
	}
	
	OmnString errmsg;
	if(!mProxyConn->connect(errmsg))
	{		
		// warning errmsg
		OmnAlarm << "Failed to process connection:" << errmsg << enderr;
		return false;
	}
	
	mFtpDataSrv->startReading();
	
	return true;
}

bool
AosKeyouTester::start()
{
	//1. Initialize Environment 
	

	if(!initData())
	{
		return false;
	}
	
    setInitialEnv(false);
    
	functionalTest();

    return true;
}

bool
AosKeyouTester::setInitialEnv(const bool sendInitMsg)
{
	return true;
}

bool
AosKeyouTester::functionalTest()
{
	int runnedNum = 0;

	while(runnedNum++ < mTestNum && mNumTcsFailed < eMaxFailedTester)
	{
		cout << "=================================Round " << runnedNum << "=======================================" << endl;

		// Generate random test data this time 
		genRandomData();

		// Show the test plan we created.
//		showTestPlan(mCurTestPlan);

		// To set environment
		setIntegrateEnv();

/*		// Get expected results 
		getExpectResults();

		// Run TrafficGen

		// Get expected results 
		checkResults();
*/
		switch(mTestMode)
		{
		case eFtp:
			runFtpTest();
			break;
		case eTelnet:
			runTelnetTest();
			break;
		default:
			break;
		}	    
	    #ifdef TESTING_DEBUG
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
AosKeyouTester::runFtpTest()
{
	// 
	OmnString errmsg;
	OmnRslt rslt;
	int sec = eFtpMsgTimeOut;
	bool timeout = false;
	bool connBroken = false;
	
	// First we should make sure the tcp connection is good
	mProxyConn->closeConn();
	mProxyConn->connect(errmsg);
	if (!mProxyConn->isConnGood())
	{
		mProxyConn->closeConn();
		mProxyConn->connect(errmsg);
		if (!mProxyConn->isConnGood())
		{
			//alarm and return;
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
			return false;
		}
	}

	// sending something
	OmnString 		sendBuff;
	OmnConnBuffPtr 	revBuff;
	int 			sendLen = 0;
	OmnString		resp;
	for(int i = 0;i < AosKeyouTestCmd::eFtpHandshakeTimes;i++)
	{
		// First we received a word
		if(AosKeyouTestCmd::eFtpHandshakeResps[i] == "")
		{
			break;
		}
		
		revBuff = new OmnConnBuff();
		timeout = false;
		connBroken = false;
		if (!mProxyConn->readFrom(revBuff, sec,
							timeout, connBroken, false)||timeout)
		{
			//alarm and return
cout << __FILE__ << ":" << __LINE__ << "turn " << i << endl;
cout << "not recved" << endl;
			OmnAlarm << "cannot successfully get message from ftp server" << rslt.getErrmsg() << enderr;
			return false;
		}

		if(timeout||connBroken||(!revBuff))
		{
			// not right
cout << __FILE__ << ":" << __LINE__ << "turn " << i << endl;
cout << "timeout or ..." << endl;
			OmnAlarm << "cannot successfully get message from ftp server" << rslt.getErrmsg() << enderr;
		}

		// compare with expect result
		resp = OmnString(revBuff->getBuffer(),revBuff->getDataLength());
//cout << "recv  :" << resp << endl;

		if(resp != AosKeyouTestCmd::eFtpHandshakeResps[i])
		{
cout << __FILE__ << ":" << __LINE__ << "turn " << i << endl;
cout << "expect:" << AosKeyouTestCmd::eFtpHandshakeResps[i] << endl;
cout << "now   :" << resp << endl;
			OmnAlarm << "not right answer\n expect:" 
				<< AosKeyouTestCmd::eFtpHandshakeResps[i] << "\nrecv:"
				<< resp << "\n" << enderr;
			return false;
		}
		
//		if(revBuff) && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))	

		// Second we send a command
		if(AosKeyouTestCmd::eFtpHandshakeCmds[i] == "")
		{
			break;
		}

		sendBuff = AosKeyouTestCmd::eFtpHandshakeCmds[i];
		sendLen = sendBuff.length();

//cout << "send  :" << sendBuff << endl;
		mProxyConn->writeTo(sendBuff, sendLen);

	}
		
	return true;	
}


bool
AosKeyouTester::runTelnetTest()
{
	return false;
}


bool
AosKeyouTester::capacityTest()
{

	return true;
}

bool
AosKeyouTester::abnormalTest()
{

	return true;
}

bool
AosKeyouTester::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
{
	OmnString errmsg;
	OmnRslt rslt;
	int sec = eCliTimeOut;
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
AosKeyouTester::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff)
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
AosKeyouTester::checkResults()
{
	
/*	if (mShouldPass)
    {
        OmnTC(OmnExpected<int>(mConnNum*mRepeatNum), OmnActual<int>(mSuccessConns)) << "\nwrong rslt !!!\n" << endtc;
        OmnTC(OmnExpected<int>(0), OmnActual<int>(mFailedConns)) << "\nwrong rslt !!!\n" << endtc;
    }
    else
    {
        OmnTC(OmnExpected<int>(0), OmnActual<int>(mSuccessConns)) << "\nwrong rslt !!!\n" << endtc;
        OmnTC(OmnExpected<int>(mConnNum*mRepeatNum), OmnActual<int>(mFailedConns)) << "\nwrong rslt !!!\n" << endtc;
    }
*/	
	
	return true;
}

bool
AosKeyouTester::genRandomData()
{
	// Create CLI
/*	int cliNum = RAND_INT(0,eMaxCLINum);
	for(int i = 0;i < eMaxCLINum;i++)
	{
		if(i<cliNum)
		{
			generateCLI(mCLIs[i]);
		}
		else
		{
			mCLIs[i] = "";
		}
	}
	
	
	
	// Create HTTP message	
	// 1. Message is HTTP?
	if (RAND_INT(1,100) <= eHTTPFormat_Percent)
	{
		mIsHTTP = true;
		
		generateURL(mURI,mHostname);

//		int length = RAND_INT(1,eMaxNoHTTPFormatLength);
//		RAND_STR(mHTTPMsg.getBuffer(),length);
//		mHTTPMsg.setLength(length);

	    setHttpMsg(mURI,mHostname,mHTTPMsg);
	}
	else
	{
		mIsHTTP = false;
		int length = RAND_INT(1,eMaxNoHTTPFormatLength);
		RAND_STR(mHTTPMsg,length);
	}
*/	

	return true;
}


bool
AosKeyouTester::setIntegrateEnv()
{
	return true;
}

/*bool	
AosKeyouTester::showTestPlan(const TestPlan &plan)
{
	cout << "\n---------------Test Plan--------------" << endl;
	cout << "\n-------------Test Plan End------------\n";

	return true;	
}
*/
void
AosKeyouTester::setHttpMsg(const OmnString &resStr,
							 const OmnString &host,
							 OmnString &fullResource)
{
	fullResource = "GET "; 
	fullResource << resStr << " HTTP/1.1\r\n";
	fullResource << "Accept: */*\r\n";
	fullResource << "UA-CPU: x86\r\n";
	fullResource << "Accept-Encoding: gzip, deflate\r\n";
	fullResource << "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; NET CLR 1.1.4322)\r\n";
	fullResource << "Host: " << host << "\r\n";
	fullResource << "Connection: Keep-Alive\r\n";
	fullResource << "\r\n";
	return;
}

bool	
AosKeyouTester::getExpectResults()
{
	// If it is not HTTP mode , Proxy should let it pass
	return true;
}



