////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpProxyTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_TcpProxy_Tester_h
#define Aos_TcpProxy_Tester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosTcpProxyTester : public OmnTestPkg
{
	OmnDefineRCObject;

private:
	int					mNumTries;
	int					mSuccessConns;
	int					mFailedConns;
	int					mNumConns;
	OmnString			mStrSend;
	static int			mContentLen;
	static int			maxConns;
	static int			sendBlockSize;
	
	int					mSynPort;
	char				mTarget[1024];
	
	enum
	{
		eAosTcpProxy_CliDeamonListenPort = 28000, 	
		eAosTcpProxy_ChangesNum = 3, 	
			/*	mRepeat
			 *	mNumConns
			 *	mContentLen
			 *  mTargetIP
			 *  mTargetPort
			 */
		eAosTcpProxy_TargetVSNum=36,
		//eAosTcpProxy_TargetIPSelectNum=6,
		//eAosTcpProxy_TargetPortSelectNum=6,
		eAosTcpProxy_NumTries = 8
	};

	enum
	{
		eWeightLess256 = 90,
		eWeightLess1024 = 93,
		eWeightLess10K = 95,
		eWeightLess1M = 98,
		eWeightLess500M = 100
		/*
		eWeightLess256 = 40,
		eWeightLess1024 = 80,
		eWeightLess10K = 90,
		eWeightLess1M = 98,
		eWeightLess500M = 100
		*/
	};

	enum 
	{
		eRepeat = 0,
		eNumConns = 1,
		eContentLen = 2//,
		//eTargetIP = 3,
		//eTargetPort = 4
	};

	struct ChangeEntry 
	{
		int weight;
		int min;
		int max;
		int changed;
		int last;  
		//union last;
		int choosed;
	} mChangeArray[eAosTcpProxy_ChangesNum]; 
	
	struct TargetVS 
	{
		char * ip;
		int port;
	} mVSArray[eAosTcpProxy_TargetVSNum]; 
		
	//char * mTargetIP_select[eAosTcpProxy_TargetIPSelectNum];
	//int mTargetPort_select[eAosTcpProxy_TargetPortSelectNum];
private:

	OmnIpAddr 	mClientAddr;
	OmnIpAddr 	mServerAddr;
	OmnIpAddr 	mLocalAddr;
	OmnIpAddr 	mBouncerAddr;
	int 		mClientPort;
	int			mConnNum;    
	int			mRepeat;    
	int			mBlockSize;	
public:
	AosTcpProxyTester(const OmnIpAddr 	&clientAddr,
			    	  const OmnIpAddr 	&serverAddr,
				      const OmnIpAddr 	&localAddr,
				      const OmnIpAddr 	&bouncerAddr,
				      const int			clientPort,
					  const int			numOfTries,
					  const int			connNum,
					  const int			repeat,
					  const int			blockSize);

	~AosTcpProxyTester()
	{
	}
	virtual bool		start();


private:
	bool	basicTest();
	bool	specialTest();
	bool	tortureTest();
	bool	attackTest();
	bool 	doOperations();
	bool	checkResults();
	bool	generateData();
	
};
#endif

