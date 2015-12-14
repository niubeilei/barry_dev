////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FWTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_FWTorturer_FWTester_h
#define Omn_UnitTest_FWTorturer_FWTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UnitTest/FWTorturer/Ptrs.h"


class AosFWTester: public OmnTestPkg
{
private:
	enum
	{
		eConnNum = 20, 
		eTestTime= 10000,
		eCliDeamonListenPort = 28000,
		eCliTimeOut = 1,
		eMaxFailedTester = 100,
		eHTTPFormat_Percent = 90,
		eMaxNoHTTPFormatLength= 100,
		eMaxCLINum = 5,
		eMaxKeyword = 40,
		eLowKeywordNum = 15,
		eAddCliRateLow = 70,
		eAddCliRateHigh = 30,
		eHighKeywordNum = 30,
		eMaxKeyWordLength = 30,

		eMaxHostNameLength = 30,
		eMaxResourceLength = 60,
		
		eEmptyResourceRate = 50,
		eInsertKeywordsRate = 50,
		eInsertKeywordsMaxNum = 5,
		eOneKeywordRate = 50,
		ePartKeywordRate = 10,
		eEmpytResourceRate = 50,
	};

	enum Variables
	{
		eMaxVariablesNum = 1
	};

	struct NetFilter
	{
		
		
	};
	
	struct TestPlan
	{
		
		
		TestPlan();
		void resetValidityFlag();
	};

	struct TestDataStatus
	{
		int mOperationCounter;
		bool mOperationChanged;
		
		TestDataStatus();
		void resetChangeFlag(const bool flag);
	};

private:	
	OmnTcpClientPtr	mProxyConn;
	OmnTcpClientPtr	mBouncerConn;

	int			mTestNum;
	int			mConnNum;
	int			mRepeatNum;
	int			mSuccessConns;
	int			mFailedConns;

	OmnTestVariablePtr		mVariList[eMaxVariablesNum];
	
		
	TestDataStatus 		mDataStatus;
	TestPlan			mPostTestPlan;
	TestPlan			mCurTestPlan;
		
	int			mRepeat;
	int			mNumConns;
	int			mContentLen;
	int			maxConns;
	int			mSendBlockSize;
	
	OmnIpAddr	mLocalAddr;
	OmnIpAddr	mProxyIP;
	OmnIpAddr	mBouncerIP;
	
	OmnString	mHTTPMsg;
	
	OmnString	mCLIs[eMaxCLINum];
	
	OmnVList<Keyword>		mKeywords;

	bool		mHasKeyword;
	bool		mIsHTTP;
	bool		mShouldPass;
	OmnString	mHostname;
	OmnString	mURI;
public:
	AosFWTester(const OmnIpAddr &localAddr,
						const OmnIpAddr &serverIp,
						const OmnIpAddr &bouncerIp);
	virtual ~AosFWTester(); 
	
	virtual bool	start();
	void	trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients);
	
	void 	setTestNum(const int num){mTestNum = num;}
	void	setConnNum(const int num){mConnNum = num;}
	void	setRepeatNum(const int num){mRepeatNum = num;}
	
private:
	bool	launchTraffic(int, int);
	bool	sendCmd(const OmnString &sendBuff, const OmnTcpClientPtr &dstServer);
	bool	genRandomData();
	bool	genStaticData1();
	bool	initData();

	bool	setInitialEnv(const bool sendInitMsg);
	bool	setIntegrateEnv();
	bool	setCapacityEnv();
	bool	setAbnormalEnv();
	bool	functionalTest();
	bool	capacityTest();
	bool	abnormalTest();
	bool	checkPmiResults();

	bool	showTestPlan(const TestPlan &plan);

	void	showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff);
	void	setHttpMsg(const OmnString &resStr,
					   const OmnString &host,
					   OmnString &fullResource);

	bool	generateURI(OmnString &uri);

	bool	getExpectResults();

	bool	checkResults();

	bool	generateCLI(OmnString &cli);
	
	bool	generateAddCLI(OmnString &cli);
	
	bool	generateRemoveCLI(OmnString &cli);
	
	bool	generateURL(OmnString &uri,OmnString &host);

	bool	checkRule(const Keyword &keyword);
	
	bool	insertKeywords(OmnString &uri,OmnString &host);	

	void	cutKeyword(OmnString &keyword);
	
};

#endif
