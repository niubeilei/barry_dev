////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_TcpBouncer_h
#define Omn_UtilComm_TcpBouncer_h
 
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"

class AosTcpBouncer : public OmnTcpCltGrpListener
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxSock = 4000, //2000
		eMaxAttacker = 5,
	};

    enum BounceType
    {
        eInvalidBounceType,
        
        eSimpleBounce,
		eSinker,
		eSpeedLimite,
    };  

    enum Action
    {
        eInvalidAction,

        eContinue,
        eIgnore,
		eAbort,
    };

private:
	OmnString			mName;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	int					mNumLocalPorts;
	OmnMutexPtr			mLock;
	BounceType			mBounceType;
	OmnTcpServerGrpPtr	mServer;
	AosTcpBouncerListenerPtr 	mListener;
	AosTcpBouncerClientPtr		mClients[eMaxSock];
	u32 				mConnFinished;
	u32					mExpectedBytes;

	OmnTcpClientPtr		mPinholeAttacker[eMaxAttacker];
	bool				mRunAttacker;
	AosBandWidthMgrPtr	mBandWidthMgr;//bandwidth limit

public:
	AosTcpBouncer(const OmnString &name, 
			const OmnIpAddr &localAddr, 
			const int localPort, 
			const int numLocalPorts, 
			const int maxConns,
			const BounceType bm, 
			const u32 expectedBytes);

	~AosTcpBouncer();

	// 
	// TcpListener Interface
	//
    virtual OmnString   getTcpListenerName() const {return mName;}
	virtual void		msgRecved(const OmnTcpCltGrpPtr &group, 
								  const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	virtual void		newConn(const OmnTcpClientPtr&);
    virtual bool        connClosed(const OmnTcpCltGrpPtr &, 
                                const OmnTcpClientPtr &client);


	int			removeClient(const AosTcpBouncerClientPtr &client);
	bool		connect(OmnString &errmsg);
	bool		startReading();
	bool		stopReading();
	bool		registerCallback(const AosTcpBouncerListenerPtr &callback);
	void		connFinished(const int clientId) {mConnFinished++;}

	bool		addAttacker(const OmnIpAddr &localIP,const int &port);
	bool		removeAttacker(const OmnIpAddr &localIP,const int &port);
	void		startAttackers();
	void		stopAttackers();
	bool		needAttackPinhole();
	bool		attackPinhole(const OmnIpAddr &remoteIp,const int &remotePort);
	void		setBWMgr(const AosBandWidthMgrPtr &limit);
	
	AosBandWidthMgrPtr		getBWMgr()const ;
private:
	bool		sendPinholeAttackMsg(const OmnTcpClientPtr &attacker);
	
};

#endif

