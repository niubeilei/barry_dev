////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpBouncerEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_BouncerMgr_TcpBouncerEntry_h
#define Omn_BouncerMgr_TcpBouncerEntry_h

#include "BouncerMgr/BouncerEntry.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"


class AosTcpBouncerEntry :public AosBouncerEntry , public OmnTcpListener
{
private:
	OmnTcpServerPtr			mTcpServer;
public:
	AosTcpBouncerEntry(const OmnIpAddr& senderIp, const int& senderPort);
	virtual ~AosTcpBouncerEntry();

public:

	virtual bool	start();						
	virtual bool	stop();						

	// listener functions
    virtual OmnString   getTcpListenerName() const;
    virtual void        msgRecved(const OmnConnBuffPtr &buff,
                                  const OmnTcpClientPtr &conn);
};
#endif

