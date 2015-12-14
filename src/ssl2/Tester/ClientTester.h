////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ClientTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_SslClientTester_h
#define Omn_SslTester_SslClientTester_h

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "ssl2/Tester/Ptrs.h"
#include "ssl2/aosSslProc.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpTrafficGenListener.h"


class AosSslTest : public OmnRCObject
{
	OmnDefineRCObject;

public:
	int				mRepeat;
	int				mExecuted;
	OmnString		mName;
	OmnIpAddr		mAddr;
	int				mPort;
	tcp_vs_conn 	mConn;
	OmnTcpClientPtr	mTcp;
	u32				mSentPos;
	u32				mRecvPos;
	char *			mDataToSend;
	u32				mDataLen;
	int				mDataSendingStarted;
	u32				mAppRecordSize;

	void set(int r, const OmnString name, const OmnIpAddr &addr, int port)
	{
		mRepeat = r;
		mName = name;
		mAddr = addr;	
		mPort = port;
	}
};
	
class AosSslTest;
class OmnTestcase;
class OmnIpAddr;
struct tcp_vs_conn;

class AosSslClientTester : public OmnTestPkg, 
						   public AosTcpTrafficGenListener
{
private:
	AosSslTest		*mTests;
	int				mMaxClientId;
	AosTcpTrafficGenPtr	mTcpTrafficGen;

public:
	AosSslClientTester()
	{
		mName = "AosSslClientTester";
	}
	~AosSslClientTester() {}

	virtual bool		start();

	// 
	// OmnTcpCltGrpListener interface
	//
    virtual void        msgRecved(const OmnTcpCltGrpPtr &group,
                                  const OmnConnBuffPtr &buff,
                                  const OmnTcpClientPtr &conn);
    virtual void        connCreated(const OmnTcpCltGrpPtr &group, 
								  const OmnTcpClientPtr &conn);
    virtual void        connClosed(const OmnTcpCltGrpPtr &group, 
								  const OmnTcpClientPtr &conn);

	// 
	// AosTcpTrafficGenListener Interface
	//
    virtual void        msgRecved(const AosTcpTrafficGenClientPtr &client,
                                  const OmnConnBuffPtr &buff);
    virtual void        connCreated(const AosTcpTrafficGenClientPtr &client,
                                  const OmnTcpClientPtr &conn);
    virtual void        connClosed(const AosTcpTrafficGenClientPtr &client,
                                  const OmnTcpClientPtr &conn);
    virtual void        readyToSend(const AosTcpTrafficGenClientPtr &client,
                                  const char *data,
                                  const int dataLen,
                                  bool &needToSend);
    virtual void        sendFinished(const AosTcpTrafficGenClientPtr &client);
    virtual void        dataSent(const AosTcpTrafficGenClientPtr &client,
                                  const char *data,
                                  const int dataLen);
    virtual void        sendFailed(const AosTcpTrafficGenClientPtr &client,
                                  const char *data,
                                  const int dataLen,
                                  const OmnRslt &rslt);
    virtual void        recvFailed(const AosTcpTrafficGenClientPtr &client,
                                  const OmnConnBuffPtr &buff,
                                  AosTcpTrafficGen::Action &action);
    virtual void        trafficGenFinished(OmnVList<AosTcpTrafficGenClientPtr> &clients);

	int appDataReceived(
			int rc, 
			struct tcp_vs_conn *conn, 
			char *rslt_data, 
			int rslt_data_len);

private:
	bool	basicTest();
	bool	volumeTest(const u32 repeat, 
				const OmnIpAddr &addr, 
				const int port,
				const u32 numSessions);
	bool 	startSslSession(AosSslTest &test);
	bool	sendAppData(const int clientId);
};
#endif

