////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WebWallTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/WebWallTorturer/WebWallTester.h"

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
#include "UnitTest/WebWallTorturer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"

//#define   SINGLE_PMI_SOFT_TESTING
#define   SINGLE_PMI_TESTING_DEBUG 

AosWebWallTester::TestPlan::TestPlan()
{
}

void
AosWebWallTester::TestPlan::resetValidityFlag()
{
}

AosWebWallTester::TestDataStatus::TestDataStatus()
{
}

void
AosWebWallTester::TestDataStatus::resetChangeFlag(const bool flag)
{
	// false - means has not been changed;
	// true - changed
//	mOrgChanged	= flag;
//	mSysChanged = flag;
//	mSNChanged = flag;
//	mResourceChanged = flag;
//	mOperationChanged = flag;
}

AosWebWallTester::AosWebWallTester(const OmnIpAddr &localAddr,
								   const OmnIpAddr &proxyIP,
								   const OmnIpAddr &bouncerIP)
:
mTestNum(eTestTime),
mSuccessConns(0),
mFailedConns(0),
mSendBlockSize(1000),
mLocalAddr(localAddr),
mProxyIP(proxyIP),
mBouncerIP(bouncerIP)
{   
	mName = "AosWebWallTester";
} 

AosWebWallTester::~AosWebWallTester()
{
} 

bool
AosWebWallTester::initData()
{
	mProxyConn = new OmnTcpClient("proxy", mProxyIP, eCliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	mBouncerConn = new OmnTcpClient("bouncer", mBouncerIP, eCliDeamonListenPort,
												1, OmnTcp::eNoLengthIndicator);
	OmnString errmsg;
	if(!mProxyConn->connect(errmsg))
	{		
		// warning errmsg
		return false;
	}
	if(!mBouncerConn->connect(errmsg))
	{		
		// warning errmsg
		return false;
	}
	return true;
}

bool
AosWebWallTester::start()
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
AosWebWallTester::setInitialEnv(const bool sendInitMsg)
{
	// construct tcpclient for proxyclient. proxy and bouncer
	
//		mProxyClients517Conn[i]->connect(errmsg);
	//
	//	PROXY CLIENT SIDE CONFIGURATION
	//

	return true;
}

bool
AosWebWallTester::functionalTest()
{
	int runnedNum = 0;
	genStaticData1();
	mPostTestPlan = mCurTestPlan;

	//setIntegrateEnv();
	while(runnedNum++ < mTestNum && mNumTcsFailed < eMaxFailedTester)
	{
		cout << "=================================Round " << runnedNum << "=======================================" << endl;
		mDataStatus.resetChangeFlag(false);
		mPostTestPlan = mCurTestPlan;

		// Generate random test data this time 
		genRandomData();

		// Show the test plan we created.
		showTestPlan(mCurTestPlan);

		// To set environment
		setIntegrateEnv();

		// Launch Generator
		mNumConns = 1;		
		mRepeat = 1;	

		// Get expected results 
		getExpectResults();

		// Run TrafficGen
		launchTraffic(mConnNum, mRepeatNum);

		// Get expected results 
		checkResults();
	    
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
AosWebWallTester::capacityTest()
{

	return true;
}

bool
AosWebWallTester::abnormalTest()
{

	return true;
}

bool
AosWebWallTester::sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer)
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
AosWebWallTester::showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff)
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
AosWebWallTester::checkResults()
{
	
	if (mShouldPass)
    {
        OmnTC(OmnExpected<int>(mConnNum*mRepeatNum), OmnActual<int>(mSuccessConns)) << "\nwrong rslt !!!\n" << endtc;
        OmnTC(OmnExpected<int>(0), OmnActual<int>(mFailedConns)) << "\nwrong rslt !!!\n" << endtc;
    }
    else
    {
        OmnTC(OmnExpected<int>(0), OmnActual<int>(mSuccessConns)) << "\nwrong rslt !!!\n" << endtc;
        OmnTC(OmnExpected<int>(mConnNum*mRepeatNum), OmnActual<int>(mFailedConns)) << "\nwrong rslt !!!\n" << endtc;
    }
	
	
	return true;
}

bool
AosWebWallTester::launchTraffic(int conn, int repeat)
{
    // start generator, generate a connection
    AosTcpTrafficGen clientGen(001,
    						   mLocalAddr,
							   mProxyIP,
    						   8001, 
    						   1, 
    						   repeat, 
    						   conn, 
    						   "", 
    						   0,
                               eAosSendBlockType_Fixed, 
                               mSendBlockSize, 
                               0);
	clientGen.setCheckContent(true);
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
AosWebWallTester::genRandomData()
{

	// Create CLI
	int cliNum = RAND_INT(0,eMaxCLINum);
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
	

	return true;
}

bool
AosWebWallTester::genStaticData1()
{
	// 1. Get org name
	mDataStatus.resetChangeFlag(true);

                                                                                                                                                                           
	return true;
}

bool
AosWebWallTester::setIntegrateEnv()
{
	for(int i = 0;i < eMaxCLINum;i++)
	{
		if(mCLIs[i] == "")
		{
			break;
		}
		else
		{
			sendCmd(mCLIs[i],mProxyConn);
		}
	}	
	return true;
}

bool	
AosWebWallTester::showTestPlan(const TestPlan &plan)
{
	cout << "\n---------------Test Plan--------------" << endl;
	cout << "\n-------------Test Plan End------------\n";

	return true;	
}

void
AosWebWallTester::setHttpMsg(const OmnString &resStr,
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
AosWebWallTester::getExpectResults()
{
	// If it is not HTTP mode , Proxy should let it pass
	if(!mIsHTTP)
	{
		mShouldPass = true;
		return true;
	}
	
	mShouldPass = true;

	mKeywords.reset();
	while(mKeywords.hasMore())
	{
		if(checkRule(mKeywords.crtValue()))
		{
			mShouldPass = false;
			break;
		}
		mKeywords.next();
	}
	return true;
}



bool
AosWebWallTester::generateCLI(OmnString &cli)
{
	int entries = mKeywords.entries();
	int rate = 0;
	if(0 == entries)
	{
		rate = 100;
	}
	else if(entries < eLowKeywordNum)
	{
		rate = eAddCliRateLow;
	}
	else if(entries < eHighKeywordNum)
	{
		rate = 50;
	}
	else if(entries < eMaxKeyword)
	{
		rate = eAddCliRateHigh;
	}
	else// entries >= eMaxKeyword
	{
		rate = 0;
	}
	if(rate >= RAND_INT(1,100))
	{
		generateAddCLI(cli);
	}
	else
	{
		generateRemoveCLI(cli);
	}
	
	return true;
}

bool
AosWebWallTester::generateAddCLI(OmnString &cli)
{
	int length = RAND_INT(1,eMaxKeyWordLength);
	Keyword keyword;
	RAND_STR(keyword.mKeyword,length);
	cli = "web wall add keyword ";
	cli << keyword.mKeyword;
	mKeywords.append(keyword);
	return true;
}

bool
AosWebWallTester::generateRemoveCLI(OmnString &cli)
{
	int index = RAND_INT(1,mKeywords.entries());
	mKeywords.reset();
	for(int i = 0;i < index; i ++)
	{
		if(!mKeywords.hasMore())
		{
			// should not happen
			return false;
		}
		mKeywords.next();
	}
	
	cli = "web wall delete keyword ";
	cli << mKeywords.crtValue().mKeyword;
	mKeywords.eraseCrt();
	
	return true;
}

bool
AosWebWallTester::generateURL(OmnString &uri,OmnString &host)
{
	int hostLength = RAND_INT(1,eMaxHostNameLength);
	RAND_STR(host,hostLength);

	uri = "/";
	if(eEmpytResourceRate <= RAND_INT(1,100))
	{
		int resLength = RAND_INT(1,eMaxResourceLength);
		OmnString rs;
		RAND_STR(rs,resLength);
		uri << rs;
	}

	insertKeywords(uri,host);

	return true;
}

bool
AosWebWallTester::insertKeywords(OmnString &uri,OmnString &host)
{
	if(eInsertKeywordsRate > RAND_INT(1,100))
	{
		// no need to insert keyword
		return true;
	}
	
	// insert keyword
	int num = RAND_INT(1,eInsertKeywordsMaxNum);
	int index = 0;
	for(int i = 0;i < num;i++)
	{
		// get one keyword		
		index = RAND_INT(1,mKeywords.entries());
		mKeywords.reset();
		for(int i = 0;i < index; i ++)
		{
			if(!mKeywords.hasMore())
			{
				// should not happen
				return false;
			}
			mKeywords.next();
		}
		OmnString key = mKeywords.crtValue().mKeyword;
		
		// cut keyword
		if(ePartKeywordRate >= RAND_INT(1,100))
		{
			cutKeyword(key);
		}
		else
		{
			// note that this message contain keyword
			mHasKeyword = true;
		}

		// insert into host or resource		
		if(RAND_BOOL())
		{
			int pos = RAND_INT(0,uri.length());
			uri.insert(key,pos);
		}
		else
		{
			int pos = RAND_INT(0,host.length());
			host.insert(key,pos);
		}	
	}
	return true;
}

bool
AosWebWallTester::checkRule(const Keyword &keyword)
{
	if(mHTTPMsg.findSubString(keyword.mKeyword,0) > 0)
	{
		return true;
	}	
	return false;
}

void
AosWebWallTester::cutKeyword(OmnString &keyword)
{
	if(0 == keyword.length())
	{
		return;
	}
	
	if(RAND_BOOL())
	{
		// cut header
		keyword.assign(keyword.getBuffer(),RAND_INT(1,keyword.length()));
	}
	else
	{
		// cut tail
		int cutLength = RAND_INT(1,keyword.length());
		keyword.assign(keyword.getBuffer()+ cutLength, keyword.length()-cutLength);
	}
	
	return;
}
