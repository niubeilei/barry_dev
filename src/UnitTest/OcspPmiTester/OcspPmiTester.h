////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspPmiTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_PmiTester_OcspPmiTester_h
#define Omn_UnitTest_PmiTester_OcspPmiTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UnitTest/OcspPmiTester/Ptrs.h"


class AosOcspPmiTester: public OmnTestPkg
{
private:
	enum
	{
		eAosOcspPmi_VALID_ORG_PERCENT = 50,
		eAosOcspPmi_VALID_SYS_PERCENT = 50,
		eAosOcspPmi_VALID_SN_PERCENT = 30,
		eAosOcspPmi_VALID_RESOURCE_PERCENT = 30,
		eAosOcspPmi_NOT_HTML_RESOURCE_PERCENT = 30,
//		eAosOcspPmi_OPERATION_518_PERCENT = 50,

		eAosOcspPmi_OCSP_ON_PERCENT = 80,
		eAosOcspPmi_CRL_ON_PERCENT = 80,
		eAosOcspPmi_CLIENTAUTH_ON_PERCENT = 80,
		eAosOcspPmi_CERT_ON_PERCENT = 80,
		eAosOcspPmi_CONNOCSP_SRV_ON_PERCENT = 80,
		eAosOcspPmi_OCSP_SRV_RESPONSE_ON_PERCENT = 80,
		eAosOcspPmi_PMI_ON_PERCENT = 80,
 
		eAosOcspPmi_MAX_ORGNAMES = 1,
		eAosOcspPmi_MAX_SYSNAMES = 2,
		eAosOcspPmi_MAX_SN = 2,
		eAosOcspPmi_MAX_RESOURCE = 4,
		eAosOcspPmi_VALID_SN_NO = 1,
//		eAosOcspPmi_MAXosOcspPmi_LoopSize_Org = 1000,
		eAosOcspPmi_LoopSize_Org = 50,
		eAosOcspPmi_LoopSize_Sys = 50,
		eAosOcspPmi_LoopSize_SN = 10,
		eAosOcspPmi_LoopSize_Resource = 1,
//		eAosOcspPmi_LoopSize_Operation = 10,

		eAosOcspPmi_LoopSize_Ocsp_Status = 50,
		eAosOcspPmi_LoopSize_Crl_Status = 50,
		eAosOcspPmi_LoopSize_Client_Auth_Status = 50,
		eAosOcspPmi_LoopSize_Cert_Status = 50,
		eAosOcspPmi_LoopSize_Conn_Ocsp_Srv_Status = 50,
		eAosOcspPmi_LoopSize_Ocsp_Srv_Response_Status = 50,
		eAosOcspPmi_LoopSize_Pmi_Status = 50,
		
		eAosOcspPmi_MaxFailedTester = 1000, 
		eAosOcspPmi_CliDeamonListenPort = 28000, 

		eAosOcspPmi_RepeatTime = 1, 
		eAosOcspPmi_ConnNum = 1, 
		eAosOcspPmi_TestTime= 10000,
		eAosOcspPmi_CliTimeOut = 10
	};

	struct aosTestPlan
	{
		bool	mOcspStatus;
		bool	mCrlStatus;
		bool	mClientAuthStatus;
		bool	mConnOcspSrvStatus;
		bool	mOcspSrvResponse;
		bool	mPmiStatus;

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

		int mOcspStatusCounter;
		int mCrlStatusCounter;
		int mClientAuthStatusCounter;
		int mConnOcspSrvStatusCounter;
		int mOcspSrvResponseCounter;
		int mPmiStatusCounter;


		bool mOrgChanged;
		bool mSysChanged;
		bool mSNChanged;
		bool mResourceChanged;
		bool mOperationChanged;

		bool mOcspStatusChanged;
		bool mCrlStatusChanged;
		bool mClientAuthStatusChanged;
		bool mConnOcspSrvStatusChanged;
		bool mOcspSrvResponseChanged;
		bool mPmiStatusChanged;
		
		aosTestDataStatus();
		void resetChangeFlag(const bool flag);
	};
    
private:	
	OmnTcpClientPtr	mProxyClientConn;
	OmnTcpClientPtr	mProxyServerConn;
	OmnTcpClientPtr	mBouncerConn;
	OmnTcpClientPtr	mPMIServerConn;
    
	OmnTcpClientPtr	mProxyClients517Conn[eAosOcspPmi_MAX_SN];
    
	OmnString	mOrgNames[eAosOcspPmi_MAX_ORGNAMES]; // orgnization name
	OmnString	mSysNames[eAosOcspPmi_MAX_SYSNAMES];
//	OmnString	mValidSNs[eAosOcspPmi_MAX_SN]; 
//	OmnString	mInvalidSNs[eAosOcspPmi_MAX_SN];
	OmnIpAddr	mSnAddr[eAosOcspPmi_MAX_SN];
	bool		mSnValidity[eAosOcspPmi_MAX_SN];
	bool		mAllowEncrypt[eAosOcspPmi_MAX_SN];
    
	OmnString	mResourceNames[eAosOcspPmi_MAX_RESOURCE];
	bool		mSourceOnly518[eAosOcspPmi_MAX_RESOURCE];
    
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
	int			sendBlockSize;
	
	OmnIpAddr	mTrafficGen_IP;
	OmnIpAddr	mServerProxy_IP;
	OmnIpAddr	mBouncer_IP;
	OmnIpAddr	mPMIServer_IP;
	
public:
	AosOcspPmiTester(const OmnIpAddr &clientIp, 
						const OmnIpAddr &serverIp,
						const OmnIpAddr &bouncerIp,
						const OmnIpAddr &PMIServerIp);
	virtual ~AosOcspPmiTester(); 
	
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
	void	initData(const OmnIpAddr &trafficGenIp,
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
	bool	checkOcspResults();
	bool	checkAllResults();
    
	bool	showTestPlan(const aosTestPlan &plan);
    
	void	showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff);
	void	setHttpMsg(const OmnString &resStr,OmnString &fullResource);
    
};    
#endif
