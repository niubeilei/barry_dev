////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysMsg_SvrDeathMsg_h
#define AOS_SysMsg_SvrDeathMsg_h

#include "Util/Buff.h"
#include "TransBasic/AppMsg.h"

class AosSvrDeathMsg: public AosAppMsg
{

private:
	int		mDeathSvrId;

public:
	AosSvrDeathMsg(const bool reg_flag);
	AosSvrDeathMsg(const int to_svrid, const int death_sid);
	~AosSvrDeathMsg();
	
	virtual bool 	serializeTo(const AosBuffPtr &buff);
	virtual bool 	serializeFrom(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eSvrDeath"; };
	virtual AosConnMsgPtr clone2();
	virtual bool	proc();
	
};

#endif
