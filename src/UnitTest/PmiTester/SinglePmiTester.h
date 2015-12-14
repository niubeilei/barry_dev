////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SinglePmiTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_PmiTester_SinglePmiTester_h
#define Omn_UnitTest_PmiTester_SinglePmiTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UnitTest/PmiTester/Ptrs.h"


class AosSinglePmiTester: public OmnTestPkg
{
private:
	enum
	{
		eAosSinglePmi_VALID_ORG_PERCENT = 50,
		eAosSinglePmi_VALID_SYS_PERCENT = 50,
		eAosSinglePmi_VALID_SN_PERCENT = 30,
		eAosSinglePmi_VALID_RESOURCE_PERCENT = 30,
		eAosSinglePmi_NOT_HTML_RESOURCE_PERCENT = 30,
//		eAosSinglePmi_OPERATION_518_PERCENT = 50,

		eAosSinglePmi_MAX_ORGNAMES = 1,
		eAosSinglePmi_MAX_SYSNAMES = 2,
		eAosSinglePmi_MAX_SN = 3,
		eAosSinglePmi_MAX_RESOURCE = 4,
		eAosSinglePmi_VALID_SN_NO = 2,
//		eAosSinglePmi_MAXosSinglePmi_LoopSize_Org = 1000,
		eAosSinglePmi_LoopSize_Org = 50,
		eAosSinglePmi_LoopSize_Sys = 50,
		eAosSinglePmi_LoopSize_SN = 10,
		eAosSinglePmi_LoopSize_Resource = 1,
//		eAosSinglePmi_LoopSize_Operation = 10,
		
		eAosSinglePmi_MaxFailedTester = 1000, 
		eAosSinglePmi_CliDeamonListenPort = 28000, 

		eAosSinglePmi_RepeatTime = 1, 
		eAosSinglePmi_ConnNum = 20, 
		eAosSinglePmi_TestTime= 10000,
		eAosSinglePmi_CliTimeOut = 1
	};

	struct aosTestPlan
	{
		OmnString mOrg;
		OmnString mSys;
		int		  mOrgNo;
		int		  mSysNo;
		OmnIpAddr mSNAddr;
		int		  mSNNo;
		OmnString mResource;
		bool mIs518SN;
		bool mIsResourceOnly518;
		bool mOrgValidity;
		bool mSysValidity;
		bool mSNValidity;
		bool mResourceValidity;
		
		aosTestPlan();
		void resetValidityFlag();
	};

	struct aosTestDataStatus
	{
		int mOrgCounter;
		int mSysCounter;
		int mSNCounter;
		int mResourceCounter;
		int mOperationCounter;
		bool mOrgChanged;
		bool mSysChanged;
		bool mSNChanged;
		bool mResourceChanged;
		bool mOperationChanged;
		
		aosTestDataStatus();
		void resetChangeFlag(const bool flag);
	};

private:	
	OmnTcpClientPtr	mProxyClientConn;
	OmnTcpClientPtr	mProxyServerConn;
	OmnTcpClientPtr	mBouncerConn;
	OmnTcpClientPtr	mPMIServerConn;

	OmnTcpClientPtr	mProxyClients517Conn[eAosSinglePmi_MAX_SN];

	OmnString	mOrgNames[eAosSinglePmi_MAX_ORGNAMES]; // orgnization name
	OmnString	mSysNames[eAosSinglePmi_MAX_SYSNAMES];
//	OmnString	mValidSNs[eAosSinglePmi_MAX_SN]; 
//	OmnString	mInvalidSNs[eAosSinglePmi_MAX_SN];
	OmnIpAddr	mSnAddr[eAosSinglePmi_MAX_SN];
	bool		mSnValidity[eAosSinglePmi_MAX_SN];
	bool		mAllowEncrypt[eAosSinglePmi_MAX_SN];

	OmnString	mResourceNames[eAosSinglePmi_MAX_RESOURCE];
	bool		mSourceOnly518[eAosSinglePmi_MAX_RESOURCE];

	int			mTestNum;
	int			mConnNum;
	int			mRepeatNum;
	int			mSuccessConns;
	int			mFailedConns;
	
	aosTestDataStatus 	mDataStatus;
	aosTestPlan			mPostTestPlan;
	aosTestPlan			mCurTestPlan;
		
	int			mRepeat;
	int			mNumConns;
	int			mContentLen;
	int			maxConns;
	int			mSendBlockSize;
	
//	OmnIpAddr	mClientProxy_IP[3];
	OmnIpAddr	mLocalAddr;
	OmnIpAddr	mTrafficGen_IP;
	OmnIpAddr	mServerProxy_IP;
	OmnIpAddr	mBouncer_IP;
	OmnIpAddr	mPMIServer_IP;
	
public:
	AosSinglePmiTester(const OmnIpAddr &localAddr,
						const OmnIpAddr &clientIp1,
						const OmnIpAddr &clientIp2,
						const OmnIpAddr &clientIp3, 
						const OmnIpAddr &serverIp,
						const OmnIpAddr &bouncerIp,
						const OmnIpAddr &PMIServerIp);
	virtual ~AosSinglePmiTester(); 
	
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
	void	initData(const OmnIpAddr &localAddr,
					 const OmnIpAddr &clientIp1,
					 const OmnIpAddr &clientIp2,
					 const OmnIpAddr &clientIp3,
					 const OmnIpAddr &serverIp,
					 const OmnIpAddr &bouncerIp,
					 const OmnIpAddr &PMIServerIp);
	bool	setInitialEnv(const bool sendInitMsg);
	bool	setIntegrateEnv();
	bool	setCapacityEnv();
	bool	setAbnormalEnv();
	bool	integrateTest();
	bool	capacityTest();
	bool	abnormalTest();
	bool	checkPmiResults();

	bool	showTestPlan(const aosTestPlan &plan);

	void	showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff);
	void	setHttpMsg(const OmnString &resStr,OmnString &fullResource);

};

#endif
