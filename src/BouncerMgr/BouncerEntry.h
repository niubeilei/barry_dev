////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BouncerEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_BouncerMgr_BouncerEntry_h
#define Omn_BouncerMgr_BouncerEntry_h

#include "BouncerMgr/Ptrs.h"

#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "UtilComm/TcpBouncerListener.h"


class AosBouncerEntry : public virtual OmnRCObject
{
	OmnDefineRCObject;
protected:
	OmnVList<AosBouncerConnPtr>		mConnList;
    OmnMutexPtr         			mLock;
	
	OmnIpAddr						mRecverIp;
	int								mRecverPort;
	OmnString						mProtocol;

public:
	AosBouncerEntry(const OmnIpAddr	&recvIp,
					const int		&recvPort,
					const OmnString &protocol);
	AosBouncerEntry();
	virtual ~AosBouncerEntry();
public:
	bool	entryExist(const OmnIpAddr 		&recverIp,
					   const int		  	&recverPort,
					   const OmnString 		&protocol);

	bool	addConn(const OmnIpAddr 		&senderIp,
					const int		  		&senderPort);
					
	bool	removeConn(const OmnIpAddr 		&senderIp,
					   const int		  	&senderPort);
									
	int		connNum();								
					   
	bool	getBandwidth(const OmnIpAddr 		&senderIp,
					     const int		  		&senderPort,
					     int		  			&curBandwidth,
					     int		  			&avgBandwidth);

	bool	resetConnStat(const OmnIpAddr 		&senderIp,
					   	  const int		  		&senderPort);

	virtual bool	start();						
	virtual bool	stop();						
};
#endif

