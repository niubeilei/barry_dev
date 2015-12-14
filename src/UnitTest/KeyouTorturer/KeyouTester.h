////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KeyouTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_KeyouTorturer_KeyouTester_h
#define Omn_UnitTest_KeyouTorturer_KeyouTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UnitTest/KeyouTorturer/Ptrs.h"


class AosKeyouTester: public OmnTestPkg
{
private:
	enum
	{
//		eConnNum = 20, 
		eTestTime= 10,
//		eCliDeamonListenPort = 28000,
		eCliTimeOut = 1,
		eFtpMsgTimeOut = 5,
		eMaxFailedTester = 100,
		eFtpTestRate		= 100,
	};
	
	enum TestMode
	{
		eFtp,
		eTelnet
	};
	

private:	
	OmnTcpClientPtr	mProxyConn;
	OmnTcpServerPtr	mFtpDataSrv;

	int				mTestNum;
	int				mConnNum;
	int				mRepeatNum;
	int				mSuccessConns;
	int				mFailedConns;
                	
	OmnIpAddr		mLocalAddr;
	OmnIpAddr		mProxyIP;

	unsigned int	mFtpPort;
	unsigned int	mFtpDataPort;
	unsigned int	mTelnetPort;

	TestMode		mTestMode;

		

/*	OmnTestVariablePtr		mVariList[eMaxVariablesNum];
	
		
	TestDataStatus 		mDataStatus;
	TestPlan			mPostTestPlan;
	TestPlan			mCurTestPlan;
		
	int			mContentLen;
	int			maxConns;
	int			mSendBlockSize;
	
	OmnIpAddr	mBouncerIP;
	
	OmnString	mHTTPMsg;
	
	OmnString	mCLIs[eMaxCLINum];
	
	OmnVList<Keyword>		mKeywords;

	bool		mHasKeyword;
	bool		mIsHTTP;
	bool		mShouldPass;
	OmnString	mHostname;
	OmnString	mURI;
*/

public:
	AosKeyouTester(const OmnIpAddr &localAddr,
					const OmnIpAddr &serverIp,
					const int		&ftpPort,
					const int		&ftpDataPort,
					const int		&telnetPort);
	virtual ~AosKeyouTester(); 
	
	virtual bool	start();
	void	trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients);
	
	void 	setTestNum(const int num){mTestNum = num;}
	void	setConnNum(const int num){mConnNum = num;}
	void	setRepeatNum(const int num){mRepeatNum = num;}
	
private:
	bool	sendCmd(const OmnString &sendBuff, const OmnTcpClientPtr &dstServer);
	bool	genRandomData();
	bool	initData();

	bool	setInitialEnv(const bool sendInitMsg);
	bool	setIntegrateEnv();
	bool	setCapacityEnv();
	bool	setAbnormalEnv();
	bool	functionalTest();
	bool	capacityTest();
	bool	abnormalTest();
	bool	checkPmiResults();

//	bool	showTestPlan(const TestPlan &plan);

	void	showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff);
	void	setHttpMsg(const OmnString &resStr,
					   const OmnString &host,
					   OmnString &fullResource);

	bool	generateURI(OmnString &uri);

	bool	getExpectResults();

	bool	checkResults();

	bool	runFtpTest();

	bool	runTelnetTest();

};

#endif
