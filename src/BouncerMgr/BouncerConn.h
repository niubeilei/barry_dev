////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BouncerConn.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_BouncerMgr_BouncerConn_h
#define Omn_BouncerMgr_BouncerConn_h

#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpBouncerListener.h"


class AosBouncerConn : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eCollectorLen = 10
	};	
private:
	OmnIpAddr			mSenderIp;
	int					mSenderPort;
	OmnIpAddr			mRecverIp;
	int					mRecverPort;
	OmnString			mProtocol;

	int					mCollector[eCollectorLen];
	int					mLastPos;
	int					mLastSecTick;
	int					mLastSlotTick;

	int					mStartTimeSec;
	int					mStartTimeUsec;
	long				mTotalVolumn;
	int					mConnTag;

    OmnMutexPtr         mLock;	
public:	
	AosBouncerConn(const OmnIpAddr 		&senderIp,
				   const int		  	&senderPort,
				   const OmnIpAddr 		&recverIp,
				   const int		  	&recverPort,
				   const OmnString 		&protocol);
	virtual ~AosBouncerConn();
public:
	bool	connExist(const OmnIpAddr 		&senderIp,
			  		  const int		  		&senderPort);
	bool	getBandwidth(int	&curBandwidth,
				     	 int	&avgBandwidth);
	bool	resetStat();
	
	bool	getPacket(const OmnConnBuffPtr &buff,const OmnString &proto);
private:
	bool	addVolume(const int size);
	void	clearExpiredSlots();
};
#endif

