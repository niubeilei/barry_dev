////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpBouncerEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_BouncerMgr_UdpBouncerEntry_h
#define Omn_BouncerMgr_UdpBouncerEntry_h

#include "BouncerMgr/BouncerEntry.h"
#include "UtilComm/CommListener.h"


class AosUdpBouncerEntry :public AosBouncerEntry, public OmnCommListener
{
	OmnDefineRCObject;
private:
	
	OmnUdpCommPtr			mUdpComm;
	OmnUdpPtr			mTestComm;

public:	
	AosUdpBouncerEntry(const OmnIpAddr& senderIp, const int& senderPort);
	virtual ~AosUdpBouncerEntry();

public:

	virtual bool	start();						
	virtual bool	stop();						

    virtual bool        msgRead(const OmnConnBuffPtr &buff);
    virtual OmnString   getCommListenerName() const;

};
#endif

