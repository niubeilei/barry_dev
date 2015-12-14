////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpAttacker.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AttackMgr_TcpAttacker_h
#define Aos_AttackMgr_TcpAttacker_h

#include "AttackMgr/Attacker.h"
#include "Thread/ThreadedObj.h"


class AosTcpAttacker:public AosAttacker
{
private:
	int 		mRawsock;
	int 		mPsize;
//	int 		mKiddie = 0, fw00ding = 0, nospoof = 0, pid[CHLD_MAX + 5];
	char 		mSynb[8192];
	struct 	timespec 	mSmall;// = {0, 1337};

public:	
	AosTcpAttacker();
	AosTcpAttacker(const OmnIpAddr &sip,
				   const int &sport,
				   const OmnIpAddr &dip,
				   const int &dport,
				   const int &num,
				   const int &interval);
	virtual ~AosTcpAttacker();
	
	virtual bool		bind();
	virtual bool		unbind();
	virtual bool		sendPacket();

	void	setStyle(const AosAttacker::Protocol protocol);
private:
	bool	send_syn();
	bool	send_fin();
	bool	send_ack();
	bool	send_reset();

};
#endif
