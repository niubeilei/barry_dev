////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpTrafficGenClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TrafficGenClient_h
#define Omn_UtilComm_TrafficGenClient_h
 
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "UtilComm/TcpTrafficGenDef.h"

class AosPCTestsuite;

class AosTcpTrafficGenClient: public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		//eMaxConnTimer = 3,
		//eMaxClientLife = 10, 		// 300 seconds
		eMaxConnTimer = 60,
		eMaxClientLife = 120, 		// 300 seconds
	};

	enum Status
	{
		eInvalidStatus = 		0,

		eIdle = 				1,
		eConnCreated = 			2,
		eReceiveFinished = 		3,
		eReceivePartial = 		4,
		eSendingFailed = 		5,
		eSendingFinished = 		6,
		eIsSending = 			7,
		eFailedToConnect = 		8,
		eFinished = 			9,
		eFailed = 				10,
	};

	enum ReceiveMode
	{
		eInvalidReceiveMode,

		eBouncing,
		eInteractive,
		ePacketCheck
	};

	enum SendMode
	{
		eInvalidSendMode,

		eReceiveDriven,
	};
	
	enum InputDataType
	{
		eTestCaseConfigFile = 0,
		eBuffer = 1
	};

private:
	int				mClientId;
	OmnIpAddr		mLocalAddr;
	OmnIpAddr		mRemoteAddr;
	int				mRemotePort;
	int				mRepeat;
	int				mSuccessed;
	int				mFailed;
	OmnMutexPtr		mLock;
	int				mIndex;
	Status			mStatus;
	ReceiveMode		mReceiveMode;

	OmnTcpCltGrpPtr	mGroup;
	OmnTcpClientPtr	mConn;
	AosTcpTrafficGenListenerPtr mListener;
	AosTcpTrafficGenPtr			mTrafficGen;

    u32             mSentPos;
    u64             mBytesReceived;
    u64             mBytesExpected;
	u32				mRecvCursor;
    char *          mDataToRecv;
    char *          mDataToSend;
    u32             mDataLen;
    int             mDataSendingStarted;
	AosSendBlockType	mSendBlockType;
    u32             mSendBlockSize;
	SendMode		mSendMode;

	OmnString		mErrmsg;
	bool			mRecvFinished;
	int				mConnStartTick;
	int				mConnLastRcvTick;
	bool			mNeedToRestart;

	u64				mBytesSent;
	
	bool			mCheckContent;
	AosPCTestsuite	*mTestSuite;
	int				mInputDataType;
	
	
public:
	AosTcpTrafficGenClient(
            const AosTcpTrafficGenPtr &trafficGen,
            const OmnTcpCltGrpPtr &group,
            int clientId,
            const OmnIpAddr &localAddr,
            const OmnIpAddr &remoteAddr,
            const int remotePort,
            const int repeat,
            const int bytesExpected,
            char * dataToSend,
            const int dataLen,
			const AosSendBlockType type,
            const u32 sendBlockSize,
            const SendMode sendMode,
            const int index,
			const AosTcpTrafficGenListenerPtr &listener);
			
	AosTcpTrafficGenClient(
            const AosTcpTrafficGenPtr &trafficGen,
            const OmnTcpCltGrpPtr &group,
            int clientId,
            const OmnIpAddr &localAddr,
            const OmnIpAddr &remoteAddr,
            const int remotePort,
            const int repeat,
            const AosSendBlockType type,
            const SendMode sendMode,
            const int index,
			const AosTcpTrafficGenListenerPtr &listener,
			AosPCTestsuite* testSuite);
			
	~AosTcpTrafficGenClient() {}

	// 
	// These functions can be overridden by subclasses.
	//
    virtual void    msgReceived(const OmnConnBuffPtr &buff);
	virtual bool	isFinished() const;
	virtual bool	needToRestart() const;
	virtual int		restart();

	int		getRepeated() const {return mSuccessed + mFailed;}
	int		getStatus() const {return mStatus;}
	void	printStatus() const;
	int		sendData();
	int		getClientId() const {return mClientId;}
	int		repeated() const {return mSuccessed + mFailed;}
	int		received() const {return mBytesReceived;}
	u32		getBytesSent() const {return mBytesSent;}
	u32		getBytesRcvd() const {return mSuccessed * mBytesExpected;}
	u32		getTotalConns() const {return mSuccessed + mFailed;}
	u32		getFailedConns() const {return mFailed;}
	bool	checkConn();
	//modified by <a href=mailto:xw_cn@163.com>xiawu</a>
	int		validateResult(char *src, char *dst);
	//end of modified
	
	void	setCheckContent(const bool check){mCheckContent = check;}
};

#endif

