////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpTrafficGen.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpTrafficGen_h
#define Omn_UtilComm_TcpTrafficGen_h
 
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpTrafficGenDef.h"

class AosPCTestsuite;

class AosTcpTrafficGen : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eGroupSize = 20,
		eMaxThreads = 50
	};

    enum Action
    {
        eInvalidAction,

        eIgnore,
        eStop
    };
    
    enum InputDataType
	{
		eTestCaseConfigFile = 0,
		eBuffer = 1
	};

	
private:
	OmnIpAddr		mRemoteAddr;
	int				mRemotePort;
	int				mNumPorts;
	int				mRepeat;
	int 			mConcurrentConns;
	int				mBytesExpected;
	OmnString		mContents;
	int				mContentLen;
	AosTcpTrafficGenThreadPtr	mThreads[eMaxThreads];
	int				mNumThreads;
	OmnMutexPtr		mLock;
	AosTcpTrafficGenListenerPtr mListener;
	int				mNumClients;
	u32				mStartSec;
    AosSendBlockType	mSendBlockType;
    u32             mSendBlockSize;
	u32				mGeneratorId;
	OmnVList<OmnIpAddr>	mLocalAddrList;

	bool			mCheckContent;
	AosPCTestsuite*	mTestSuite;
	int				mInputDataType;
	
public:
	AosTcpTrafficGen(
			const u32 genId, 
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int repeat,
            const int concurrentConns,
            char  c,
            const int contentLen, 
			AosSendBlockType sendBlockType, 
			const u32 sendBlockSize, 
			const u32 bytesExpected);

//add by xiaoqing 12/08/05/
	AosTcpTrafficGen(
			const u32 genId, 
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int repeat,
            const int concurrentConns,
            const char*  cStr,
            const int contentLen, 
			AosSendBlockType sendBlockType, 
			const u32 sendBlockSize, 
			const u32 bytesExpected);
////////////////////////////////////////////////////////////
	
	AosTcpTrafficGen(
			const u32 genId, 
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int repeat,
            const int concurrentConns,
			AosPCTestsuite* testSuite);


	/*
	AosTcpTrafficGen(
			const u32 genId,
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts); 
	*/
	virtual ~AosTcpTrafficGen() {}

	bool		start();
	bool		stop();
	void		printStatus() const;
	bool		closeConn(const OmnTcpClientPtr &client);
	bool		restartClient(const OmnTcpClientPtr &client);
	bool		checkFinish() const;
	int			addClient(const AosTcpTrafficGenClientPtr &client);
	void		registerCallback(const AosTcpTrafficGenListenerPtr &callback);
	bool		checkConns();
	int			getRecv();
	int			getFailedConns();
	int			getSuccessConns();

	int			addLocalAddr(const OmnIpAddr &localAddr);
	void		setCheckContent(const bool check){mCheckContent = check;}

private:
	bool		createClients();
	
};

#endif

