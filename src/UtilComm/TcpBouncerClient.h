////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncerClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_BouncerClient_h
#define Omn_UtilComm_BouncerClient_h
 
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "UtilComm/TcpBouncer.h"


class AosTcpBouncerClient: public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Status
	{
		eInvalidStatus,

		eIdle,
		eBouncing,
		eFinished,
		eFailed,
	};

private:
	int				mClientId;
	Status			mStatus;
	AosTcpBouncer::BounceType	mBounceType;

	OmnTcpClientPtr	mConn;
	AosTcpBouncerListenerPtr 	mListener;
	AosTcpBouncerPtr			mBouncer;

    u32             mSentPos;
    u32             mBytesReceived;
    u32             mBytesExpected;

	OmnString		mErrmsg;

public:
	AosTcpBouncerClient(
            const AosTcpBouncerPtr &bouncer,
			const OmnTcpClientPtr &conn, 
			AosTcpBouncer::BounceType bm,
			const u32 bytesExpected);
	~AosTcpBouncerClient() {}

	// 
	// These functions can be overridden by subclasses.
	//
    virtual void    msgReceived(const OmnConnBuffPtr &buff);

	int		getStatus() const {return mStatus;}
	// int		getClientId() const {return mClientId;}
	void	registerListener(const AosTcpBouncerListenerPtr &listener);
	int		getSock() const;
	OmnTcpClientPtr getConn() const;		
};

#endif

