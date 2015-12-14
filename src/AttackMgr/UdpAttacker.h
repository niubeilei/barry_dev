////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpAttacker.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AttackMgr_UdpAttacker_h
#define Aos_AttackMgr_UdpAttacker_h

#include "AttackMgr/Attacker.h"


class AosUdpAttacker: public AosAttacker
{
	OmnDefineRCObject;
private:
	int 		mRawsock;
public:	
	AosUdpAttacker();
	AosUdpAttacker(const OmnIpAddr &sip,
				   const int &sport,
				   const OmnIpAddr &dip,
				   const int &dport,
				   const int &num,
				   const int &interval);
	virtual ~AosUdpAttacker();
	
	virtual bool		bind();
	virtual bool		unbind();
	virtual bool		sendPacket();

};
#endif
