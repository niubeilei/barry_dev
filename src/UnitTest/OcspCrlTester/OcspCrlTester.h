////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspCrlTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_OcspCrlTester_OcspCrlTester_h
#define Omn_UnitTest_OcspCrlTester_OcspCrlTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UnitTest/OcspCrlTester/Ptrs.h"


class AosOcspCrlTester: public OmnTestPkg
{
private:
	enum
	{
		
		eAosOcspCrl_CliDeamonListenPort = 28000, 	
		eAosOcspCrl_NumTries = 200,
		eAosOcspCrl_NumClients = 2
	
	};
	
private:
	OmnTcpClientPtr	mOcspCrlServerCliConn;
	OmnTcpClientPtr	mTcpBouncerCliConn;
	OmnTcpClientPtr	mProxyClientConn[2];
	OmnTcpClientPtr	mProxyServerConn;
	OmnTcpClientPtr	mBouncerConn;
	OmnTcpClientPtr	mOcspCrlServerConn;
	int	mRepeat;
	int	mNumConns;
	static int	mContentLen;
	static int	maxConns;
	static int	sendBlockSize;
	int	mSuccessConns;
	int	mFailedConns;
	int mNumTries;
	OmnIpAddr	mLocalAddr;
	OmnIpAddr	mClientProxy_IP[2];
	OmnIpAddr	mServerProxy_IP;
	OmnIpAddr	mBouncer_IP;
	OmnIpAddr	mOcspCrlServer_IP;
	OmnIpAddr	misValidCert;
	
	
public:
    AosOcspCrlTester(const OmnIpAddr &localAddr,
						const OmnIpAddr &clientIp1,
						const OmnIpAddr &clientIp2, 
						const OmnIpAddr &serverIp,
						const OmnIpAddr &bouncerIp,
						const OmnIpAddr &OcspCrlServerIp);
    virtual ~AosOcspCrlTester(); 

    virtual bool	start();
	bool	launchTraffic(int, int);
	bool	changeConfig();	
	
	bool	checkOcspStatus(const AosOcspCrlStatusPtr &);	

private:
	void	printStatus();
    bool    integrateTest();
    bool    capacityTest();
    bool    abnormalTest();
    bool	sendCmd(const OmnString &sendBuff, const OmnTcpClientPtr &dstServer);
    void	showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff);

};

#endif
