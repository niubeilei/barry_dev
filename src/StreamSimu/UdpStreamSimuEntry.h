////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpStreamSimuEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_StreamSimu_UdpStreamSimuEntry_h
#define Omn_StreamSimu_UdpStreamSimuEntry_h

#include "StreamSimu/StreamSimuEntry.h"
#include "UtilComm/Ptrs.h"


class AosUdpStreamSimuEntry :public AosStreamSimuEntry
{
	OmnDefineRCObject;
private:
	
	OmnUdpPtr			mUdp;

public:	
	AosUdpStreamSimuEntry(const OmnIpAddr& senderIp, const int& senderPort);
	virtual ~AosUdpStreamSimuEntry();

public:

	virtual bool	start();						
	virtual bool	stop();						

	virtual bool    sendPacket(const OmnIpAddr &recverIp, 
							   const int recverPort,
							   const int size);

};
#endif

